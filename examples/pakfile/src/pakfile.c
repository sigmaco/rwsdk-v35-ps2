
/****************************************************************************
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 2000 Criterion Software Ltd.
 * All Rights Reserved.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * pakfile.c
 *
 * Copyright (C) 2000 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: To illustrate the use of PAK files for the storing and 
 *          retrieval of RenderWare data.
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "rwcore.h"

#include "skeleton.h"
#include "pakfile.h"


/* 
 * A PAK file table of contents entry...
 */
typedef struct PakTOCEntry PakTOCEntry;
struct PakTOCEntry
{
    RwChar name[56];
    RwInt32 offset;
    RwInt32 fileSize;
};

/* 
 * An open PAK file...
 */
typedef struct PakFile PakFile;
struct PakFile
{
    /* 
     * General...
     */
    RwChar dirName[260];
    void *file;
    RwUInt32 pakFileSize;
    RwUInt32 pakFilePosition;
    RwFreeList *fileFreeList;
    RwInt32 numOpenFiles;
    
    /* 
     * Table of contents...
     */
    PakTOCEntry *tocEntry;
    RwInt32 tocSize;
};


/* 
 * An open file within a PAK...
 */
typedef struct PakOpenFile PakOpenFile;
struct PakOpenFile
{
    RwUInt32 id;
    PakFile  *pakFile;
    PakTOCEntry *tocEntry;
    RwInt32 bytesRead;
    RwBool eof;
};

#define PAKHEADERID                             \
(                                               \
        ('P'<< 0) |                             \
	('A'<< 8) |                             \
	('C'<<16) |                             \
	('K'<<24)                               \
)
    

#define PAKMAXOPENFILES (10)

static RwFileFunctions RWFileInterface;
static RwBool FileInterfaceOverride = FALSE;

static PakFile *PAKFile = (PakFile *)NULL;



/*
 *****************************************************************************
 */
static RwBool
_PakFileSkipSet(PakFile *pakFile, RwUInt32 offset)
{
    /*
     * Purpose:   Set absolute position in PAK file.
     * On entry:  PAK file; file offset from start.
     * On exit:   Success status.
     */

    RwFileFunctions *fileFunctions = RwOsGetFileInterface();
    
    if( !fileFunctions || !pakFile || !pakFile->file )
    {
        return FALSE;
    }

    /* 
     * Ensure that our offset is within the file...
     */
    if( offset > pakFile->pakFileSize-1 )
    {
        offset = pakFile->pakFileSize-1;
    }

    /* 
     * Seek from start of file...
     */
    if( fileFunctions->rwfseek(pakFile->file, offset, SEEK_SET) )
    {
        return FALSE;
    }

    /* 
     * Store file position...
     */
    pakFile->pakFilePosition = offset;

    return TRUE;
}


/*
 *****************************************************************************
 */
static RwBool
_PakSortTOC(PakFile *pakFile)
{
   /*
    * Purpose:   Sort table of contents by name alphabetically.
    * On entry:  PAK file.
    * On exit:   Success status.
    */

    RwInt32 i, j;
    RwBool noSwaps;
    PakTOCEntry tempEntry;

    /* 
     * Sort TOC entries into alphabetical name order...
     */
    for(i=pakFile->tocSize-1; i>0; i--)
    {
        noSwaps = TRUE;

        for(j=1; j<=i; j++)
        {
            /* 
             * Swap entries...
             */
            if( rwstrcmp(pakFile->tocEntry[j-1].name, pakFile->tocEntry[j].name) > 0 )
            {
                tempEntry = pakFile->tocEntry[j-1];
                pakFile->tocEntry[j-1] = pakFile->tocEntry[j];
                pakFile->tocEntry[j] = tempEntry;

                noSwaps = FALSE;
            }
        }

        if( noSwaps )
        {
            return TRUE;
        }
    }

    return TRUE;
}


/*
 *****************************************************************************
 */
static RwInt32
_PakFindFirstMatch(PakFile *pakFile, const RwChar *pathname)
{
    /*
     * Purpose:   Find first file in TOC that matches this path.
     * On entry:  PAK file; path name.
     * On exit:   TOC index, -1 if fails.
     */

    RwInt32 start, end, halfway, compare;

    RwChar *psTOCpath;


    if( !pakFile || !pakFile->file || !pakFile->tocEntry )
    {
        return -1;
    }

    /* 
     * Do binary chop to find first matching entry...
     */
    start = 0;
    end = pakFile->tocSize-1;
    halfway = (start + end)>>1;

    psTOCpath = RsPathnameCreate( pakFile->tocEntry[halfway].name );

    compare = rwstrcmp(pathname, psTOCpath );

    RsPathnameDestroy( psTOCpath );

    while( (compare != 0) && (start != end) )
    {
        if( compare < 0 )
        {
            end = halfway;
        }
        else
        {
            if( start < (end-1) )
            {
                start = halfway;
            }
            else
            {
                start = end;
            }
        }

        halfway = (start + end)>>1;

        psTOCpath = RsPathnameCreate( pakFile->tocEntry[halfway].name );

        compare = rwstrcmp(pathname, psTOCpath );

        RsPathnameDestroy( psTOCpath );
    }

    /* 
     * If we didnt find an exact match is our path a sub-dir of our match...?
     */
    if( compare != 0 )
    {
        psTOCpath = RsPathnameCreate( pakFile->tocEntry[halfway].name );

        if( strncmp(pathname, psTOCpath, rwstrlen(pathname)) )
        {
            RsPathnameDestroy( psTOCpath );

            return -1;
        }

        RsPathnameDestroy( psTOCpath );
    }

    return halfway;
}


/*
 *****************************************************************************
 */
static void *
_PakFOpen(const RwChar *name, const RwChar *access)
{
    /*
     * Purpose:   File i/o override for PAK files: FOPEN.
     * On entry:  File name; access.
     * On exit:   File pointer.
     */

    RwInt32 nameLen, dirLen, i;

    RwChar *pathname;

    /* 
     * Check if open PAK file might contain requested file...
     */
    nameLen = rwstrlen(name);
    dirLen  = rwstrlen(PAKFile->dirName);
    
    if( (nameLen < dirLen) || strncmp(name, PAKFile->dirName, dirLen) )
    {
        /* 
         * Doesn't match open PAK file, 
         * Pass down to the file system...
         */
        return RWFileInterface.rwfopen(name, access);
    }

    /* 
     * Look for the file in the PAK file...
     */
    pathname = RsPathnameCreate(name + dirLen);

    i = _PakFindFirstMatch(PAKFile, pathname);

    RsPathnameDestroy(pathname);

    if( i >= 0 )
    {
        PakOpenFile *openFile;

        /* 
         * Allocate file...
         */
        if( PAKFile->fileFreeList )
        {
            openFile = (PakOpenFile *)RwFreeListAlloc(PAKFile->fileFreeList);
        }
        else
        {
            openFile = (PakOpenFile *)RwMalloc(sizeof(PakOpenFile));
        }

        if( openFile )
        {
            openFile->id = PAKHEADERID;
            openFile->pakFile = PAKFile;
            openFile->tocEntry = &PAKFile->tocEntry[i];
            openFile->bytesRead = 0;
            openFile->eof = FALSE;
            
            /* 
             * Increment number of files open in this PAK file...
             */
            PAKFile->numOpenFiles++;

            return openFile;
        }
    }

    /* 
     * File wasn't in PAK file so pass down to file system 
     * rather than fail...
     */
    return RWFileInterface.rwfopen(name, access);
}


/*
 *****************************************************************************
 */
static int
_PakFClose(void *file)
{
    /*
     * Purpose:   File i/o override for PAK files: FCLOSE.
     * On entry:  File pointer.
     * On exit:   0 if successful, -1 otherwise.
     */

    PakOpenFile *openFile = (PakOpenFile *)file;

    /* 
     * If this aint our file, pass it thru...
     */
    if( openFile->id != PAKHEADERID )
    {
        return RWFileInterface.rwfclose(file);
    }

    /* 
     * Decrement number of files open in this PAK file...
     */
    openFile->pakFile->numOpenFiles--;

    /* 
     * Free file...
     */
    if( openFile->pakFile->fileFreeList )
    {
        RwFreeListFree(openFile->pakFile->fileFreeList, openFile);
    }
    else
    {
        RwFree(openFile);
    }

    return 0;
}


/*
 *****************************************************************************
 */
static size_t
_PakFRead(void *addr, size_t size, size_t count, void *file)
{
    /*
     * Purpose:   File i/o override for PAK files: FREAD.
     * On entry:  Dest address; element size; num. of elements; file pointer.
     * On exit:   Number of elements read.
     */

    PakOpenFile *openFile = (PakOpenFile *)file;
    RwUInt32 position, bytesLeft, bytes, bytesToRead;

    /* 
     * If this aint our file, pass it thru...
     */
    if( openFile->id != PAKHEADERID )
    {
        return RWFileInterface.rwfread(addr, size, count, file);
    }

    /* 
     * End-of-file?
     */
    if( openFile->eof )
    {
        return 0;
    }

    /* 
     * Position file...
     */
    position = openFile->tocEntry->offset + openFile->bytesRead;
    if( openFile->pakFile->pakFilePosition != position )
    {
        _PakFileSkipSet(openFile->pakFile, position);
    }

    /* 
     * Cap bytes to read to end of file (allow to just overrun)...
     */
    bytesLeft = (openFile->tocEntry->fileSize - openFile->bytesRead);
    bytesToRead = size * count;
    if( bytesToRead > bytesLeft+1 )
    {
        bytesToRead = bytesLeft+1;
    }

#ifdef PAKMAXFREADBYTES

    /* 
     * Read from file in several limited size chunks...
     */
    bytes = 0;
    while( bytesToRead > PAKMAXFREADBYTES )
    {
        RwUInt32 bytesRead;

        bytesRead = RWFileInterface.rwfread(addr, 1, 
            PAKMAXFREADBYTES, openFile->pakFile->file);
        
        bytes += bytesRead;
        bytesToRead -= bytesRead;
        
        (RwUInt8 *)addr += bytesRead;
    }

    /* 
     * Read remaining bytes...
     */
    bytes += RWFileInterface.rwfread(addr, 1, 
        bytesToRead, openFile->pakFile->file);

#else /* PAKMAXFREADBYTES */

    /* 
     * Read from file in one chunk...
     */
    bytes = RWFileInterface.rwfread(addr, 1, 
        bytesToRead, openFile->pakFile->file);

#endif /* PAKMAXFREADBYTES */

    /* 
     * Increment file position...
     */
    openFile->pakFile->pakFilePosition += bytes;

    /* 
     * Increment bytes read...
     */
    openFile->bytesRead += bytes;

    /* 
     * If we have passed the bounds of the current chunk 
     * then report end-of-file...
     */
    if( openFile->bytesRead >= openFile->tocEntry->fileSize )
    {
        bytes -= (openFile->bytesRead - openFile->tocEntry->fileSize);
        
        openFile->eof = TRUE;
    }

    return bytes;
}


/*
 *****************************************************************************
 */
static RwChar *
_PakFGets(RwChar *buffer, int maxLen, void *file)
{
    /*
     * Purpose:   File i/o override for PAK files: FGETS.
     * On entry:  Receive buffer; max size of buffer; file pointer.
     * On exit:   Buffer.
     */

    PakOpenFile *openFile = (PakOpenFile *)file;
    RwUInt32 position, bytesLeft, bytes;

    /* 
     * If this aint our file, pass it thru...
     */
    if( openFile->id != PAKHEADERID )
    {
        return RWFileInterface.rwfgets(buffer, maxLen, file);
    }

    /* 
     * End-of-file?
     */
    if( openFile->eof )
    {
        return (RwChar *)NULL;
    }

    /* 
     * Position file...
     */
    position = openFile->tocEntry->offset + openFile->bytesRead;
    if( openFile->pakFile->pakFilePosition != position )
    {
        _PakFileSkipSet(openFile->pakFile, position);
    }

    /* 
     * Cap max len to end of file (allow to just overrun)...
     */
    bytesLeft = (openFile->tocEntry->fileSize - openFile->bytesRead);
    if( maxLen < 0 )
    {
        maxLen = 0;
    }
    else if( maxLen > (int)(bytesLeft+1) )
    {
        maxLen = (int)(bytesLeft+1);
    }

    /* 
     * Do normal fgets then check that it didnt overrun chunk...
     */
    if( !RWFileInterface.rwfgets(buffer, maxLen, openFile->pakFile->file) )
    {
        openFile->eof = TRUE;

        return (RwChar *)NULL;
    }

    bytes = rwstrlen(buffer);

    /* 
     * Increment file position...
     */
    openFile->pakFile->pakFilePosition += bytes;

    /* 
     * Increment bytes read...
     */
    openFile->bytesRead += bytes;

    /* 
     * If we have passed the bounds of the current chunk 
     * then report end-of-file...
     */
    if( openFile->bytesRead >= openFile->tocEntry->fileSize )
    {
        bytes -= (openFile->bytesRead - openFile->tocEntry->fileSize);
        buffer[bytes] = '\0';

        openFile->eof = TRUE;
    }

    /* 
     * Replace CR+LF with just LF...
     */
    if( (buffer[bytes-2] == 13) && (buffer[bytes-1] == 10) )
    {
        buffer[bytes-2] = 10;
        buffer[bytes-1] = 0;
    }

    return buffer;
}


/*
 *****************************************************************************
 */
static int
_PakFEof(void *file)
{
    /*
     * Purpose:   File i/o override for PAK files: FEOF.
     * On entry:  File pointer.
     * On exit:   EoF or not.
     */

    PakOpenFile *openFile = (PakOpenFile *)file;

    /* 
     * If this aint our file, pass it thru...
     */
    if( openFile->id != PAKHEADERID )
    {
        return RWFileInterface.rwfeof(file);
    }

    /* 
     * Did we get an end-of-file?...
     */
    if( openFile->eof )
    {
        return EOF;
    }
    else
    {
        return 0;
    }
}


/*
 *****************************************************************************
 */
static int
_PakFSeek(void *file, long offset, int origin)
{
    /*
     * Purpose:   File i/o override for PAK files: FSEEK.
     * On entry:  File pointer; offset; origin.
     * On exit:   0 = success.
     */

    PakOpenFile *openFile = (PakOpenFile *)file;

    /* 
     * If this aint our file, pass it thru...
     */
    if( openFile->id != PAKHEADERID )
    {
        return RWFileInterface.rwfseek(file, offset, origin);
    }

    /* 
     * What's the origin?...
     */
    if( origin == SEEK_SET )
    {
        RwUInt32 pos = openFile->tocEntry->offset + offset;

        if( pos > openFile->pakFile->pakFileSize-1 )
        {
            pos = openFile->pakFile->pakFileSize-1;
        }
        
        openFile->pakFile->pakFilePosition = pos;
        
        openFile->bytesRead = pos;

        return RWFileInterface.rwfseek(openFile->pakFile->file, pos, SEEK_SET);
    }
    else if( origin == SEEK_CUR )
    {
        RwUInt32 pos = offset;

        if( pos > openFile->pakFile->pakFileSize-1 )
        {
            pos = openFile->pakFile->pakFileSize-1;
        }

        openFile->pakFile->pakFilePosition += pos;

        openFile->bytesRead += pos;

        return RWFileInterface.rwfseek(openFile->pakFile->file, pos, origin);
    }
    else if( origin == SEEK_END )
    {
        RwUInt32 pos = openFile->tocEntry->offset + 
            openFile->tocEntry->fileSize + offset;

        if( pos > openFile->pakFile->pakFileSize-1 )
        {
            pos = openFile->pakFile->pakFileSize-1;
        }

        openFile->pakFile->pakFilePosition = pos;

        openFile->bytesRead = pos;

        return RWFileInterface.rwfseek(openFile->pakFile->file, pos, SEEK_SET);
    }

    return 1;
}


/*
 *****************************************************************************
 */
RwBool
PakFileClose(void)
{
    /*
     * Purpose:   Close a PAK file.
     * On entry:  PAK file.
     * On exit:   Success status.
     */

    if( !PAKFile )
    {
        return FALSE;
    }

    /* 
     * Files still open in this PAK file?...
     */
    if( PAKFile->numOpenFiles != 0 )
    {
        return FALSE;
    }

    /*
     * Restore standard RenderWare interface...
     */
    if( FileInterfaceOverride )
    {
        RwFileFunctions *fileFunctions = RwOsGetFileInterface();

        if( fileFunctions )
        {
            *fileFunctions = RWFileInterface;
        }
    }

    /* 
     * Free table of contents cache...
     */
    if( PAKFile->tocEntry )
    {
        RwFree(PAKFile->tocEntry);
        PAKFile->tocEntry = (PakTOCEntry *)NULL;
        PAKFile->tocSize = 0;
    }

    /* 
     * Close file...
     */
    if( PAKFile->file )
    {
        RWFileInterface.rwfclose(PAKFile->file);
        
        PAKFile->file = NULL;
    }

    /* 
     * Free file freelist...
     */
    if( PAKFile->fileFreeList )
    {
        RwFreeListDestroy(PAKFile->fileFreeList);
    }

    /* 
     * Free PAK file...
     */
    RwFree(PAKFile);
    PAKFile = (PakFile *)NULL;

    return TRUE;
}


/*
 *****************************************************************************
 */
RwBool
PakFileOpenExt(RwChar *filename, RwChar *directory)
{
    /*
     * Purpose:   Open a PAK file.
     * On entry:  File name; directory that this PAK file represents.
     * On exit:   PAK file.
     */

    RwFileFunctions *fileFunctions;
    RwChar *pos;
    RwInt32 id, tocOffset, tocLen, nameLen, i;
    RwChar separator;
    
    /* 
     * Check no PAK Files are already open...
     */
    if( PAKFile )
    {
        return FALSE;
    } 

    /* 
     * Get RenderWare file interface...
     */
    fileFunctions = RwOsGetFileInterface();
    if( !fileFunctions )
    {
        return FALSE;
    }

    /* 
     * Create PAK file structure...
     */
    PAKFile = (PakFile *) RwMalloc(sizeof(PakFile));
    if( !PAKFile )
    {
        return FALSE;
    }

    /* 
     * Initialize...
     */
    if( directory )
    {
        /* 
         * Directory has been specified...
         */
        rwstrcpy(PAKFile->dirName, directory);
    }
    else
    {
        /* 
         * Default directory - take path name of PAK file...
         */
        rwstrcpy(PAKFile->dirName, filename);
        pos = rwstrrchr(PAKFile->dirName, '.');
        if( pos )
        {
            *pos = '\0';
        }

        rwstrncat(PAKFile->dirName, RWSTRING("\\"), 1);
    }

    PAKFile->file = NULL;
    PAKFile->pakFileSize = 0;
    PAKFile->pakFilePosition = 0;
    PAKFile->fileFreeList = 
        RwFreeListCreate(sizeof(PakOpenFile), PAKMAXOPENFILES, 0);
    PAKFile->numOpenFiles = 0;
    PAKFile->tocEntry = (PakTOCEntry *)NULL;
    PAKFile->tocSize = 0;

    /* 
     * Open PAK file...
     */
    PAKFile->file = fileFunctions->rwfopen(filename, RWSTRING("rb"));
    if( !PAKFile->file )
    {
        PakFileClose();

        return FALSE;
    }

    /* 
     * Set file size...
     */    
#ifdef SKY
    fileFunctions->rwfseek(PAKFile->file, 0, SEEK_END);
    PAKFile->pakFileSize = RwInt32MAXVAL;
#else
    fileFunctions->rwfseek(PAKFile->file, 0, SEEK_END);
    PAKFile->pakFileSize = fileFunctions->rwftell(PAKFile->file);
#endif
    fileFunctions->rwfseek(PAKFile->file, 0, SEEK_SET);

    /* Is this a PAK file? */
    fileFunctions->rwfread(&id, sizeof(id), 1, PAKFile->file);
    (void)RwMemNative32(&id, sizeof(id));
    if( id != PAKHEADERID )
    {
        PakFileClose();

        return FALSE;
    }

    /* 
     * Cache table of contents...
     */
    fileFunctions->rwfread(&tocOffset, sizeof(tocOffset), 1, PAKFile->file);
    (void)RwMemNative32(&tocOffset, sizeof(tocOffset));
    fileFunctions->rwfread(&tocLen, sizeof(tocLen), 1, PAKFile->file);
    (void)RwMemNative32(&tocLen, sizeof(tocLen));
    tocLen /= 64;

    PAKFile->tocEntry = (PakTOCEntry *) RwMalloc(sizeof(PakTOCEntry)*tocLen);
    if( !PAKFile->tocEntry )
    {
        PakFileClose();

        return FALSE;
    } 

    /* 
     * Get platform path separator...
     */
    separator = RsPathGetSeparator();

    _PakFileSkipSet(PAKFile, tocOffset);

    for(PAKFile->tocSize=0; PAKFile->tocSize<tocLen; PAKFile->tocSize++)
    {
        /* 
         * Get next file from PAK file...
         */
        fileFunctions->rwfread(&PAKFile->tocEntry[PAKFile->tocSize],
                                  sizeof(PakTOCEntry), 1, PAKFile->file);
        (void)RwMemNative32(&PAKFile->tocEntry[PAKFile->tocSize].offset, sizeof(RwInt32));
        (void)RwMemNative32(&PAKFile->tocEntry[PAKFile->tocSize].fileSize, sizeof(RwInt32));

        /* 
         * Convert any misdirected path separators...
         */
        if( separator == '/' )
        {
            nameLen = rwstrlen(PAKFile->tocEntry[PAKFile->tocSize].name);
            
            for(i=0; i<nameLen; i++)
            {
                if( PAKFile->tocEntry[PAKFile->tocSize].name[i] == '\\' )
                {
                    PAKFile->tocEntry[PAKFile->tocSize].name[i] = '/';
                }
            }
        }
        else if( separator == '\\' )
        {
            nameLen = rwstrlen(PAKFile->tocEntry[PAKFile->tocSize].name);
            
            for(i=0; i<nameLen; i++)
            {
                if( PAKFile->tocEntry[PAKFile->tocSize].name[i] == '/' )
                {
                    PAKFile->tocEntry[PAKFile->tocSize].name[i] = '\\';
                }
            }
        }
    }

    /* 
     * Sort TOC entries into alphabetical order...
     */
    _PakSortTOC(PAKFile);

    /* 
     * Remember standard RenderWare interface...
     */
    RWFileInterface = *fileFunctions;

    /* 
     * Override functions to access PAK files...
     */
    fileFunctions->rwfopen  = _PakFOpen;
    fileFunctions->rwfclose = _PakFClose;
    fileFunctions->rwfread  = _PakFRead;
    fileFunctions->rwfgets  = _PakFGets;
    fileFunctions->rwfeof   = _PakFEof;
    fileFunctions->rwfseek  = _PakFSeek;

    FileInterfaceOverride = TRUE;

    return TRUE;
}


/*
 *****************************************************************************
 */
RwBool
PakFileOpen(RwChar *filename)
{
    /*
     * Purpose:   Open a PAK file.
     * On entry:  File name.
     * On exit:   PAK file.
     */
    const RwBool result = PakFileOpenExt(filename, (RwChar*)NULL);

    return result;
}

/*
 *****************************************************************************
 */
