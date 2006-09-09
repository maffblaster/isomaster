/******************************* LICENCE **************************************
* Any code in this file may be redistributed or modified under the terms of
* the GNU General Public Licence as published by the Free Software 
* Foundation; version 2 of the licence.
****************************** END LICENCE ***********************************/

#include <string.h>

#include "bk.h"
#include "bkError.h"
#include "bkGet.h"

/*******************************************************************************
* bk_estimate_iso_size()
* Public function
* Estimates the size of the directory trees + file contents on the iso
* */
unsigned bk_estimate_iso_size(const VolInfo* volInfo, int filenameTypes)
{
    //!! add path tables, boot record
    return estimateIsoSize(&(volInfo->dirTree), filenameTypes);
}

/*******************************************************************************
* bk_get_dir_from_string()
* public function
* gets a pointer to a Dir in tree described by the string pathStr
* */
int bk_get_dir_from_string(const VolInfo* volInfo, const char* pathStr, 
                           Dir** dirFoundPtr)
{
    return getDirFromString(&(volInfo->dirTree), pathStr, dirFoundPtr);
}

/*******************************************************************************
* estimateIsoSize()
* Recursive
* Estimate the size of the directory trees + file contents on the iso
* */
unsigned estimateIsoSize(const Dir* tree, int filenameTypes)
{
    unsigned estimateDrSize;
    unsigned thisDirSize;
    int numItems; /* files and directories */
    DirLL* nextDir;
    FileLL* nextFile;
    
    thisDirSize = 0;
    numItems = 0;
    
    nextFile = tree->files;
    while(nextFile != NULL)
    {
        thisDirSize += nextFile->file.size;
        thisDirSize += nextFile->file.size % NBYTES_LOGICAL_BLOCK;
        
        numItems++;
        
        nextFile = nextFile->next;
    }
    
    nextDir = tree->directories;
    while(nextDir != NULL)
    {
        numItems++;
        
        thisDirSize += estimateIsoSize(&(nextDir->dir), filenameTypes);
        
        nextDir = nextDir->next;
    }
    
    estimateDrSize = 70;
    if(filenameTypes & FNTYPE_JOLIET)
        estimateDrSize += 70;
    if(filenameTypes & FNTYPE_ROCKRIDGE)
        estimateDrSize += 70;
    
    thisDirSize += 68 + (estimateDrSize * numItems);
    thisDirSize += NBYTES_LOGICAL_BLOCK - (68 + (estimateDrSize * numItems)) % NBYTES_LOGICAL_BLOCK;
    
    return thisDirSize;
}

/*******************************************************************************
* bk_get_dir_from_string()
* recursive
* gets a pointer to a Dir in tree described by the string pathStr
* */
int getDirFromString(const Dir* tree, const char* pathStr, Dir** dirFoundPtr)
{
    int count;
    int pathStrLen;
    bool stopLooking;
    /* name of the directory in the path this instance of the function works on */
    char* currentDirName;
    DirLL* dirNode;
    int rc;
    
    pathStrLen = strlen(pathStr);
    
    if(pathStrLen == 1 && pathStr[0] == '/')
    /* root, special case */
    {
        /* cast to prevent compiler const warning */
        *dirFoundPtr = (Dir*)tree;
        return 1;
    }
    
    if(pathStrLen < 3 || pathStr[0] != '/' || pathStr[1] == '/' || 
       pathStr[pathStrLen - 1] != '/')
        return BKERROR_MISFORMED_PATH;
    
    stopLooking = false;
    for(count = 2; count < pathStrLen && !stopLooking; count++)
    /* find the first directory in the path */
    {
        if(pathStr[count] == '/')
        /* found it */
        {
            /* make a copy of the string to use with strcmp */
            currentDirName = (char*)malloc(count);
            if(currentDirName == NULL)
                return BKERROR_OUT_OF_MEMORY;
            
            strncpy(currentDirName, &(pathStr[1]), count - 1);
            currentDirName[count - 1] = '\0';
            
            dirNode = tree->directories;
            while(dirNode != NULL && !stopLooking)
            /* each child directory in tree */
            {
                if( strcmp(dirNode->dir.name, currentDirName) == 0)
                /* found the right child directory */
                {
                    if(pathStr[count + 1] == '\0')
                    /* this is the directory i'm looking for */
                    {
                        *dirFoundPtr = &(dirNode->dir);
                        stopLooking = true;
                        rc = 1;
                    }
                    else
                    /* intermediate directory, go further down the tree */
                    {
                        rc = getDirFromString(&(dirNode->dir), 
                                              &(pathStr[count]), dirFoundPtr);
                        if(rc <= 0)
                        {
                            free(currentDirName);
                            return rc;
                        }
                        stopLooking = true;
                    }
                        
                }
                
                dirNode = dirNode->next;
            }
            
            free(currentDirName);
            
            if(!stopLooking)
                return BKERROR_DIR_NOT_FOUND_ON_IMAGE;
        } /* if(found it) */
    } /* for(find the first directory in the path) */
    
    /* can't see how i could get here but to keep the compiler happy */
    return 1;
}
