/******************************* LICENCE **************************************
* Any code in this file may be redistributed or modified under the terms of
* the GNU General Public Licence as published by the Free Software 
* Foundation; version 2 of the licence.
****************************** END LICENCE ***********************************/

/******************************************************************************
* Author:
* Andrew Smith, http://littlesvr.ca/misc/contactandrew.php
*
* Contributors:
* 
******************************************************************************/

#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <stdio.h>

#include "bk.h"
#include "bkDelete.h"
#include "bkPath.h"
#include "bkError.h"

void bk_cancel_operation(VolInfo* volInfo)
{
    volInfo->stopOperation = true;
}

/*******************************************************************************
* bk_destroy_vol_info()
* Frees any memory refered to by volinfo.
* If an image was open for reading closes it.
* Does not reinitialize the structure.
* */
void bk_destroy_vol_info(VolInfo* volInfo)
{
    deleteDirContents(volInfo, &(volInfo->dirTree));
    
    if(volInfo->bootRecordPathAndName != NULL)
        free(volInfo->bootRecordPathAndName);
    
    if(volInfo->imageForReading > 0)
        close(volInfo->imageForReading);
}

/*******************************************************************************
* bk_init_vol_info()
*
* */
int bk_init_vol_info(VolInfo* volInfo)
{
    bzero(volInfo, sizeof(VolInfo));
    
    volInfo->dirTree.base.posixFileMode = 040755;
    volInfo->posixFileDefaults = 0100644;
    volInfo->posixDirDefaults = 040755;
    
    /* detect endianness */
    unsigned char test[2] = {1, 0};
    if( *((short*)test) == 1 )
        volInfo->littleEndian = true;
    else
        volInfo->littleEndian = false;
    printf("little: %d\n", volInfo->littleEndian);
    return 1;
}

/*******************************************************************************
* bk_set_boot_file()
* Set a file on the image to be the no-emulation boot record for el torito.
* */
int bk_set_boot_file(VolInfo* volInfo, const char* srcPathAndName)
{
    int rc;
    NewPath path;
    BkDir* srcDirInTree;
    BkFileBase* child;
    bool found;
    
    rc = makeNewPathFromString(srcPathAndName, &path);
    if(rc <= 0)
    {
        freePathContents(&path);
        return rc;
    }
    
    path.numChildren--;
    found = findDirByNewPath(&path, &(volInfo->dirTree), &srcDirInTree);
    if(!found)
        return BKERROR_DIR_NOT_FOUND_ON_IMAGE;
    path.numChildren++;
    
    /* FIND the file */
    found = false;
    child = srcDirInTree->children;
    while(child != NULL && !found)
    {
        if(strcmp(child->name, path.children[path.numChildren - 1]) == 0)
        {
            if( !IS_REG_FILE(child->posixFileMode) )
            {
                freePathContents(&path);
                return BKERROR_NOT_REG_FILE_FOR_BR;
            }
            
            found = true;
            
            volInfo->bootMediaType = BOOT_MEDIA_NO_EMULATION;
            
            volInfo->bootRecordSize = BK_FILE_PTR(child)->size;
            
            if(volInfo->bootRecordPathAndName != NULL)
            {
                free(volInfo->bootRecordPathAndName);
                volInfo->bootRecordPathAndName = NULL;
            }
            
            volInfo->bootRecordIsVisible = true;
            
            volInfo->bootRecordOnImage = BK_FILE_PTR(child);
        }
        
        child = child->next;
    }
    if(!found)
    {
        freePathContents(&path);
        return BKERROR_FILE_NOT_FOUND_ON_IMAGE;
    }
    /* END FIND the file */
    
    freePathContents(&path);
    
    return 1;
}

/*******************************************************************************
* bk_set_publisher()
* Copies publisher into volInfo, a maximum of 128 characters.
* */
void bk_set_publisher(VolInfo* volInfo, const char* publisher)
{
    strncpy(volInfo->publisher, publisher, 128);
}

/*******************************************************************************
* bk_set_vol_name()
* Copies volName into volInfo, a maximum of 32 characters.
* */
void bk_set_vol_name(VolInfo* volInfo, const char* volName)
{
    strncpy(volInfo->volId, volName, 32);
}
