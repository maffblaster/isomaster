#include "bk.h"
#include "bkMangle.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* length of aaa in aaa~xxxx.bbb */
#define NCHARS_9660_BASE 3

/*
* note that some unsigned ints in mangling functions are
* required to be 32 bits long for the hashing to work
* see the samba code for details
*/

bool charIsValid9660(char theChar)
{
    if( (theChar >= '0' && theChar <= '9') ||
        (theChar >= 'a' && theChar <= 'z') ||
        (theChar >= 'A' && theChar <= 'Z') ||
        strchr("_-$~", theChar) )
    {
        return true;
    }
    else
        return false;
}

/* 
   hash a string of the specified length. The string does not need to be
   null terminated 

   this hash needs to be fast with a low collision rate (what hash doesn't?)
*/
unsigned hashString(const char *str, unsigned int length)
{
    unsigned value;
    unsigned i;
    
    static const unsigned fnv1Prime = 0x01000193;
    
    /* Set the initial value from the key size. */
    /* fnv1 of the string: idra@samba.org 2002 */
    value = 0xa6b93095;
    for (i = 0; i < length; i++)
    {
        value *= (unsigned)fnv1Prime;
        value ^= (unsigned)(str[i]);
    }
    
    /* note that we force it to a 31 bit hash, to keep within the limits
       of the 36^6 mangle space */
    return value & ~0x80000000;  
}

/* filenametypes is all types required in the end */
int mangleDir(Dir* origDir, DirToWrite* newDir, int filenameTypes)
{
    int rc;
    bool haveCollisions;
    int numTimesTried;
    int numCollisions;
    char newName9660[13]; /* for remangling */
    
    DirLL* currentOrigDir;
    DirToWriteLL** currentNewDir;
    FileLL* currentOrigFile;
    FileToWriteLL** currentNewFile;
    
    DirToWriteLL* currentDir;
    FileToWriteLL* currentFile;
    DirToWriteLL* currentDirToCompare;
    FileToWriteLL* currentFileToCompare;
    
    /* MANGLE all names, create new dir/file lists */
    currentOrigDir = origDir->directories;
    currentNewDir = &(newDir->directories);
    while(currentOrigDir != NULL)
    /* have directories */
    {
        *currentNewDir = malloc(sizeof(DirToWriteLL));
        if(*currentNewDir == NULL)
            return -1;
        
        bzero(*currentNewDir, sizeof(DirToWriteLL));
        
        mangleNameFor9660(currentOrigDir->dir.name, (*currentNewDir)->dir.name9660, true);
        
        if(filenameTypes | FNTYPE_ROCKRIDGE)
            strcpy((*currentNewDir)->dir.nameRock, currentOrigDir->dir.name);
        else
            (*currentNewDir)->dir.nameRock[0] = '\0';
        
        if(filenameTypes | FNTYPE_JOLIET)
            strcpy((*currentNewDir)->dir.nameJoliet, currentOrigDir->dir.name);
        else
            (*currentNewDir)->dir.nameJoliet[0] = '\0';
        
        (*currentNewDir)->dir.posixFileMode = currentOrigDir->dir.posixFileMode;
        
        rc = mangleDir(&(currentOrigDir->dir), &((*currentNewDir)->dir), filenameTypes);
        if(rc < 0)
            return rc;
        
        currentOrigDir = currentOrigDir->next;
        
        currentNewDir = &((*currentNewDir)->next);
    }
    
    currentOrigFile = origDir->files;
    currentNewFile = &(newDir->files);
    while(currentOrigFile != NULL)
    /* have files */
    {
        *currentNewFile = malloc(sizeof(FileToWriteLL));
        if(*currentNewFile == NULL)
            return -1;
        
        bzero(*currentNewFile, sizeof(FileToWriteLL));
        
        mangleNameFor9660(currentOrigFile->file.name, (*currentNewFile)->file.name9660, false);
        
        if(filenameTypes | FNTYPE_ROCKRIDGE)
            strcpy((*currentNewFile)->file.nameRock, currentOrigFile->file.name);
        else
            (*currentNewFile)->file.nameRock[0] = '\0';
        
        if(filenameTypes | FNTYPE_JOLIET)
            strcpy((*currentNewFile)->file.nameJoliet, currentOrigFile->file.name);
        else
            (*currentNewFile)->file.nameJoliet[0] = '\0';
        
        (*currentNewFile)->file.posixFileMode = currentOrigFile->file.posixFileMode;
        
        (*currentNewFile)->file.size = currentOrigFile->file.size;
        
        (*currentNewFile)->file.onImage = currentOrigFile->file.onImage;
        
        (*currentNewFile)->file.position = currentOrigFile->file.position;
        
        if( !currentOrigFile->file.onImage )
        {
            (*currentNewFile)->file.pathAndName = malloc(strlen(currentOrigFile->file.pathAndName) + 1);
            if( (*currentNewFile)->file.pathAndName == NULL )
                return -3;
            
            strcpy((*currentNewFile)->file.pathAndName, currentOrigFile->file.pathAndName);
        }
        
        currentOrigFile = currentOrigFile->next;
        
        currentNewFile = &((*currentNewFile)->next);
    }
    /* END MANGLE all names, create new dir/file lists */
    
    haveCollisions = true;
    numTimesTried = 0;
    while(haveCollisions && numTimesTried < 5)
    {
        haveCollisions = false;
        
        // for each subdir
          // look through entire dir list and count collisions
          // look through entire file list and count collisions
          // if more then 1, remangle name
        
        // for each file
          // look through entire dir list and count collisions
          // look through entire file list and count collisions
          // if more then 1, remangle name
        
        currentDir = newDir->directories;
        while(currentDir != NULL)
        {
            numCollisions = 0;
            
            currentDirToCompare = newDir->directories;
            while(currentDirToCompare != NULL)
            {
                if(strcmp(currentDir->dir.name9660, 
                          currentDirToCompare->dir.name9660) == 0)
                {
                    numCollisions++;
                }
                
                currentDirToCompare = currentDirToCompare->next;
            }
            
            currentFileToCompare = newDir->files;
            while(currentFileToCompare != NULL)
            {
                if(strcmp(currentDir->dir.name9660, 
                          currentFileToCompare->file.name9660) == 0)
                {
                    numCollisions++;
                }
                
                currentFileToCompare = currentFileToCompare->next;
            }
            
            if(numCollisions != 1)
            {
                haveCollisions = true;
                
                mangleNameFor9660(currentDir->dir.name9660, newName9660, true);
                
                printf("remangled '%s' -> '%s'\n", currentDir->dir.name9660, newName9660);
                
                strcpy(currentDir->dir.name9660, newName9660);
            }
            
            currentDir = currentDir->next;
        }
        
        currentFile = newDir->files;
        while(currentFile != NULL)
        {
            numCollisions = 0;
            
            currentDirToCompare = newDir->directories;
            while(currentDirToCompare != NULL)
            {
                if(strcmp(currentFile->file.name9660, 
                          currentDirToCompare->dir.name9660) == 0)
                {
                    numCollisions++;
                }
                
                currentDirToCompare = currentDirToCompare->next;
            }
            
            currentFileToCompare = newDir->files;
            while(currentFileToCompare != NULL)
            {
                if(strcmp(currentFile->file.name9660, 
                          currentFileToCompare->file.name9660) == 0)
                {
                    numCollisions++;
                }
                
                currentFileToCompare = currentFileToCompare->next;
            }
            
            if(numCollisions != 1)
            {
                haveCollisions = true;
                
                mangleNameFor9660(currentFile->file.name9660, newName9660, false);
                
                printf("remangled '%s' -> '%s'\n", currentFile->file.name9660, newName9660);
                
                strcpy(currentFile->file.name9660, newName9660);
            }
            
            currentFile = currentFile->next;
        }
        
        numTimesTried++;
    }
    
    if(haveCollisions)
        return -2;
    
    return 1;
}

void mangleNameFor9660(char* origName, char* newName, bool isADir)
{
    char *dot_p;
    int i;
    char base[7]; /* max 6 chars */
    char extension[4]; /* max 3 chars */
    int extensionLen;
    unsigned hash;
    unsigned v;
    /* these are the characters we use in the 8.3 hash. Must be 36 chars long */
    static const char *baseChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    /* FIND extension */
    if(isADir)
    /* no extension */
    {
        dot_p = NULL;
    }
    else
    {
        dot_p = strrchr(origName, '.');
        
        if(dot_p)
        {
            /* if the extension contains any illegal characters or
               is too long (> 3) or zero length then we treat it as part
               of the prefix */
            for(i = 0; i < 4 && dot_p[i + 1] != '\0'; i++)
            {
                if( !charIsValid9660(dot_p[i + 1]) )
                {
                    dot_p = NULL;
                    break;
                }
            }
            
            if(i == 0 || i == 4)
                dot_p = NULL;
        }
    }
    /* END FIND extension */
    
    /* GET base */
    /* the leading characters in the mangled name is taken from
       the first characters of the name, if they are ascii otherwise
       '_' is used
    */
    for(i = 0; i < NCHARS_9660_BASE && origName[i] != '\0'; i++)
    {
        base[i] = origName[i];
        
        if ( !charIsValid9660(origName[i]) )
            base[i] = '_';
        
        base[i] = toupper(base[i]);
    }
    
    /* make sure base doesn't contain part of the extension */
    if(dot_p != NULL)
    {
        //!! test this
        if(i > dot_p - origName)
            i = dot_p - origName;
    }
    
    /* fixed length */
    while(i < NCHARS_9660_BASE)
    {
        base[i] = '_';
        
        i++;
    }
    
    base[NCHARS_9660_BASE] = '\0';
    /* END GET base */
    
    /* GET extension */
    /* the extension of the mangled name is taken from the first 3
       ascii chars after the dot */
    extensionLen = 0;
    if(dot_p)
    {
        for(i = 1; extensionLen < 3 && dot_p[i] != '\0'; i++)
        {
            extension[extensionLen] = toupper(dot_p[i]);
            
            extensionLen++;
        }
    }
    
    extension[extensionLen] = '\0';
    /* END GET extension */
    
    /* find the hash for this prefix */
    hash = hashString(origName, strlen(origName));
    
    /* now form the mangled name. */
    for(i = 0; i < NCHARS_9660_BASE; i++)
    {
            newName[i] = base[i];
    }
    
    newName[NCHARS_9660_BASE] = '~';
    
    v = hash;
    newName[7] = baseChars[v % 36];
    for(i = 6; i > NCHARS_9660_BASE; i--)
    {
        v = v / 36;
        newName[i] = baseChars[v % 36];
    }
    
    /* add the extension and terminate string */
    if(extensionLen > 0)
    {
        newName[8] = '.';
        
        strcpy(newName + 9, extension);
    }
    else
    {
        newName[8] = '\0';
    }
}
