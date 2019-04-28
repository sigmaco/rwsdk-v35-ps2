// Definitions for custom instancer dll functions

#ifndef CUSTOM_INSTANCER_H
#define CUSTOM_INSTANCER_H

#include "rwplcore.h"

#ifdef __cplusplus
extern "C"
{
#endif
 
typedef void
InitFn(RwMemoryFunctions *memoryFuncs, RwFileFunctions *fileFuncs, RwDebugHandler debugHandler);

// Called at the start of instancing a given file
typedef void
BeginSessionFn(void);

// Possible handleMode values
enum HandleMode {
    ChunkIgnored,
    ChunkAugmented,
    ChunkFinalized
};


// Called after instancing a given file
typedef void
EndSessionFn(void);

// Called for each chunk in the input file
//    The dll may either ignore the chunk, do some work on it and write it to the
//    augmentedOutput stream, or completely instance the chunk and write it to the
//    finalOutput stream
//   
//    header - chunk header
//    input  - input stream, directly after reading the chunk header
//    augmentedOutput
//           - stream for writing partially instanced version of the chunk
//    finalOutput
//           - stream for writing totally instanced version of the chunk
//    handleMode
//           - one of (ChunkIgnored, ChunkAugmented, ChunkFinalized). If the input
//             stream wasn't read, should be ChunkIgnored
//    success
//           - output var; set it to true if the .dll successfully augments of
//             finalizes the chunk. Must be set if handleMode was ChunkAugmented or
//             ChunkFinalized.
typedef void
InstanceChunkFn(
    RwChunkHeaderInfo *header, RwStream *input, const RwChar *platformOptions,
    RwStream *augmentedOutput,
    RwStream *finalOutput,
    HandleMode *handleMode, RwBool *success);

// Called for each auxiliary file. These files aren't passed on to remote instancers.
//    filename - name of an auxiliary file (eg .dds texture)
//    success
//           - output var; set it to true if the .dll successfully handles the file
//             false otherwise
typedef void
InstanceFileFn(
    const RwChar *filename,
    RwBool *success
);

#ifdef __cplusplus
}
#endif

#endif