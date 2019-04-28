// instance_sky.cpp : Defines the entry point for the DLL application.
//

// Disable pointless warning about debugger truncation
#pragma warning( disable: 4786 )

#include "stdafx.h"
#include <rwcore.h>
#include <rpworld.h>
#include <rpcollis.h>
#include <rtintsec.h>
#include <rpskin.h>
#include <rtcharse.h>
#include <rppvs.h>
#include <rpmorph.h>
#include <rpdmorph.h>
#include <rplodatm.h>
#include <rppatch.h>
#include <rpmatfx.h>
#include <rpptank.h>
#include <rpprtstd.h>
#include <rpprtadv.h>
#include <rpltmap.h>
#include <rpmipkl.h>
#include <rptoon.h>
#include <rpusrdat.h>
#include <rtpitexd.h>
#include <rt2d.h>
#include <rtskinsp.h>

#include "exppipes.h"
#include <rppds.h>
#include <rtslctpipe.h>

#include "..\CustomInstancer.h"

#define CUSTOM_INSTANCER_API __declspec(dllexport)

extern "C"
{
CUSTOM_INSTANCER_API InitFn Init;
CUSTOM_INSTANCER_API EndSessionFn EndSession;
CUSTOM_INSTANCER_API InstanceChunkFn InstanceChunk;
CUSTOM_INSTANCER_API InstanceFileFn InstanceFile;
}

// Constants
#define TOO_MANY_BONES 65
#define SPLIT_NUM_BONES 64

// Static vars
namespace
{
    RwBool opened = FALSE;
    RwTextureCallBackRead DefaultTextureReadCallBack;
}

// TextureReadCallBack
//    Allows materials to retain textures 
static RwTexture *
TextureReadCallBack(const RwChar *name, const RwChar *mask)
{
   RwTexture *tex = (*DefaultTextureReadCallBack)(name, mask);
 
   if (!tex)
   {
      RwUInt32 rasterFlags = rwRASTERTYPETEXTURE | rwRASTERFORMAT8888;
      
      if (RwTextureGetMipmapping())
      {
          rasterFlags |= rwRASTERFORMATMIPMAP;
      }

      if (RwTextureGetAutoMipmapping())
      {
          rasterFlags |= rwRASTERFORMATAUTOMIPMAP;
      }
         
      RwRaster *raster
         = RwRasterCreate(
            32,32,32,
            rasterFlags
         );

      tex = RwTextureCreate(raster);
      RwTextureSetName(tex, name);

      // If there's a mask, we need to set it too
      if (mask)
      {
          RwTextureSetMaskName(tex, mask);
      }

      // Add to the texture dictionary (create if necessary)
      RwTexDictionary *_TexDictionary = RwTexDictionaryGetCurrent();
       
      if (_TexDictionary == 0)
      {
          _TexDictionary = RwTexDictionaryCreate ();
                    
          RwTexDictionarySetCurrent (_TexDictionary);
      }


      RwTexDictionaryAddTexture(_TexDictionary,  tex);
   }
   
   return tex;
}

static void
InterceptTextureRead()
{
   DefaultTextureReadCallBack = RwTextureGetReadCallBack();
   RwTextureSetReadCallBack(TextureReadCallBack);
}

static void
ResetTextureRead()
{
   RwTextureSetReadCallBack(DefaultTextureReadCallBack);
}

static bool
AttachPlugins()
{
    RpPDSPluginAttach(RpWorldNumPipes + 
                      RpMatfxNumPipes +
                      RpSkinfxNumPipes +
                      RpPatchSkinfxNumPipes +
                      RpLtMapNumPipes +
                      Rt2DNumPipes);

    // Register PDS pipes
    _rpPDSSkyRegisterAllPipes();
    
    // Attach other plugins
    RpWorldPluginAttach();
    RpPVSPluginAttach();
    RpCollisionPluginAttach();
    RpSkinPluginAttach();
    RpHAnimPluginAttach();
    RpMorphPluginAttach();
    RpDMorphPluginAttach();
    RpPatchPluginAttach();
    RpMatFXPluginAttach();
    RpLtMapPluginAttach();
    RpPTankPluginAttach();
    RpPrtStdPluginAttach();
    RpPrtAdvPluginAttach();
    RpToonPluginAttach();
    RpUserDataPluginAttach();

    /* This plugin implements pipe overrides specified by the exporters */
    RpExpPipesPluginAttach();

    return TRUE;
}

static RwBool
OpenRW(RwMemoryFunctions *memfuncs)
{
    if (RwEngineInit(memfuncs, 0, 32000000))
    {
        if (AttachPlugins())
        {
            RwEngineOpenParams params;
            params.displayID = NULL;
            if (RwEngineOpen(&params))
            {
                RwEngineSetSubSystem(0);

                if (RwEngineStart())
                {
                    opened = TRUE;
                    return opened;
                }

                RwEngineClose();
            }

            RwEngineTerm();
        }
    }

    return FALSE;
}

static RwBool
CloseRW(void)
{
    ResetTextureRead();

    RwEngineStop();
    RwEngineClose();
    RwEngineTerm();

    return TRUE;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
            break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
            break;
		case DLL_PROCESS_DETACH:
            if (opened)
                return CloseRW();
			break;
    }
    return TRUE;
}

static bool
InstanceTexDict(RwChunkHeaderInfo *chunkHeader, RwStream *input, RwStream *output)
{

    RwTexDictionary *texd = RtPITexDictionaryStreamRead(input);
    if(!texd)
    {
        return FALSE;
    }

    // write the dictionary to the output stream
    if (!RwTexDictionaryStreamWrite(texd, output))
    {
        // cleanup on failure
        RwTexDictionaryDestroy(texd);
        return FALSE;
    }

    // no longer need the dictionary
    RwTexDictionaryDestroy(texd);

    // success
    return TRUE;
}

// Skin split atomics if necessary
static RpAtomic *
SkinSplitAtomic(RpAtomic *atomic, void *data)
{
    RpGeometry *g = RpAtomicGetGeometry(atomic);
    RpSkin *skin = RpSkinGeometryGetSkin(g);
    RpPatchMesh* pm = RpPatchAtomicGetPatchMesh(atomic);

    // Can only split if skinned and not a patched object
    if (skin && !pm)
    {
        RwFrame *frame = (RwFrame *)data;
        RpHAnimHierarchy *hierarchy = RpHAnimGetHierarchy(frame->child);
        if (hierarchy)
        {
            RpSkinAtomicSetHAnimHierarchy(atomic, hierarchy);

            int i;
            RwMatrix *matrixArray = RpHAnimHierarchyGetMatrixArray(hierarchy);
            const RwMatrix *invBoneMats = RpSkinGetSkinToBoneMatrices(skin);
            for (i=0; i<hierarchy->numNodes && i<(int)RpSkinGetNumBones(skin); i++)
            {
                RwMatrixInvert(&matrixArray[i], &invBoneMats[i]);
            }
        }
        
        if (RpSkinGetNumBones(skin) >= TOO_MANY_BONES)
        {
            if (RtSkinSplitAtomicSplitGeometry(atomic, SPLIT_NUM_BONES) != atomic)
            {
                return atomic;
            }
        }
    }

    return atomic;
}

static RpAtomic *
OverloadPipelinesForClumpAtomic(RpAtomic *atomic, void *data)
{
    // select the default pipes for the atomic
    RtSlctPipeAtomicSelectPipes(atomic, NULL, NULL, 0);

    // prevent streaming of update pipes extension data
    RpAtomicEnableExpPipeStreaming(atomic, FALSE);

    return RpAtomicOverloadPipelines(atomic);
}

static bool
InstanceClump(RwChunkHeaderInfo *chunkHeader, RwStream *input, RwStream *output)
{

    RpClump *clump = RpClumpStreamRead(input);
    if(!clump)
    {
        return FALSE;
    }

    // Opportunity to do skin splitting
    RpClumpForAllAtomics(clump, SkinSplitAtomic, RpClumpGetFrame(clump));

    // overload pipelines (as specified in exporters)
    RpClumpForAllAtomics(clump, OverloadPipelinesForClumpAtomic, 0);

    // write the clump to the output stream
    if (!RpClumpStreamWrite(clump, output))
    {
        // cleanup on failure
        RpClumpDestroy(clump);
        return FALSE;
    }

    // no longer need the clump
    RpClumpDestroy(clump);

    // success
    return TRUE;
}

static bool
InstanceAtomic(RwChunkHeaderInfo *chunkHeader, RwStream *input, RwStream *output)
{

    RpAtomic *atomic = RpAtomicStreamRead(input);
    if(!atomic)
    {
        return FALSE;
    }

    // select the default pipes for the atomic
    RtSlctPipeAtomicSelectPipes(atomic, NULL, NULL, 0);

    // overload pipelines
    RpAtomicOverloadPipelines(atomic);

    // prevent streaming of update pipes extension data
    RpAtomicEnableExpPipeStreaming(atomic, FALSE);

    // write the Atomic to the output stream
    if (!RpAtomicStreamWrite(atomic, output))
    {
        // cleanup on failure
        RpAtomicDestroy(atomic);
        return FALSE;
    }

    // no longer need the Atomic
    RpAtomicDestroy(atomic);

    // success
    return TRUE;
}

static bool
InstanceWorld(RwChunkHeaderInfo *chunkHeader, RwStream *input, RwStream *output)
{

    RpWorld *world = RpWorldStreamRead(input);
    if(!world)
    {
        return FALSE;
    }

    // select the pipe for the world
    RtSlctPipeWorldSelectPipes(world, NULL, NULL, 0);

    // overload pipelines
    RpWorldOverloadPipelines(world);

    // prevent streaming of update pipes extension data
    RpWorldEnableExpPipeStreaming(world, FALSE);

    // write the world to the output stream
    if (!RpWorldStreamWrite(world, output))
    {
        // cleanup on failure
        RpWorldDestroy(world);
        return FALSE;
    }

    // no longer need the world
    RpWorldDestroy(world);

    // success
    return TRUE;
}

extern "C"
{
CUSTOM_INSTANCER_API void
Init(RwMemoryFunctions *memoryFuncs, RwFileFunctions *fileFuncs, RwDebugHandler debugHandler)
{
    /* Open RenderWare */
    OpenRW(memoryFuncs);

    RwDebugSetHandler(debugHandler);
    memcpy(RwOsGetFileInterface(),   fileFuncs,   sizeof(RwFileFunctions));   

    InterceptTextureRead();
}

CUSTOM_INSTANCER_API void
BeginSession(void)
{

}

CUSTOM_INSTANCER_API void
EndSession(void)
{
    
}

CUSTOM_INSTANCER_API void
InstanceChunk(
    RwChunkHeaderInfo *header, RwStream *input, const RwChar *platformOptions,
    RwStream *augmentedOutput,
    RwStream *finalOutput,
    HandleMode *handleMode, RwBool *success)
{
    switch (header->type)
    {
        case rwID_PITEXDICTIONARY:
            *success = InstanceTexDict(header, input, finalOutput);
            *handleMode = ChunkFinalized;
            break;
        case rwID_CLUMP:
            *success = InstanceClump(header, input, augmentedOutput);
            *handleMode = ChunkAugmented; 
            break;
        case rwID_ATOMIC:
            *success = InstanceAtomic(header, input, augmentedOutput);
            *handleMode = ChunkAugmented; 
            break;
        case rwID_WORLD:
            *success = InstanceWorld(header, input, augmentedOutput);
            *handleMode = ChunkAugmented; 
            break;
        default:
            *handleMode = ChunkIgnored;
            break;
    }

    return;
}

CUSTOM_INSTANCER_API void
InstanceFile(
    const RwChar *filename,
    RwBool *success)
{
    // No special handling for Sky
    *success = TRUE;
}

}
