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
 * Copyright (c) 2001 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/****************************************************************************
 *
 * ppsprite.c
 *
 * Copyright (C) 2001 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: Illustrate the possibility of creating a custom VU pipe
 *
 ****************************************************************************/

#include "rwcore.h"
#include "rpworld.h"

#include "skeleton.h"

#include "ppsprite.h"

#include "stddata.h"

#define VIFCMD_UNPACK   (0x6cl << 24)
#define VIFCMD_CYCLE    (0x01l << 24)
#define VIFCMD_DIRECT   (0x50l << 24)
#define VIFCMD_NOP      (0x00l << 24)

#define PPSPRITESTATICDATASTRYDE (1)
static RxPipeline *MatPPSpritePipeline = NULL;
static RxPipeline *ObjPPSpritePipeline = NULL;

static PPSprite   *_CurrentSpriteList = NULL;

static RwMatrix *nullTMat = NULL;


extern long         ppspriteNT      __attribute__ ((section(".vudata")));
/* The developement pipeline */
extern long         ppspritePRSDev     __attribute__ ((section(".vudata")));
/* The optimized pipeline */
extern long         ppspritePRSOpt     __attribute__ ((section(".vudata")));

#define ppspritePRS ppspritePRSDev

static RxClusterDefinition PPSClPS2xyz;
static RxClusterDefinition PPSClPS2rgba;
static RxClusterDefinition PPSClPS2hNw;
static RxClusterDefinition PPSClPS2uv;


static void        *cfxPS2ALLVUCodeArray[VU1CODEARRAYSIZE] =
{
   &ppspriteNT, &ppspriteNT, &ppspriteNT, &ppspriteNT,
   &ppspriteNT, &ppspriteNT, &ppspriteNT, &ppspriteNT,
   &ppspriteNT, &ppspriteNT, &ppspriteNT, &ppspriteNT,
   &ppspriteNT, &ppspriteNT, &ppspriteNT, &ppspriteNT,
   &ppspriteNT, &ppspriteNT, &ppspriteNT, &ppspriteNT,
   &ppspriteNT, &ppspriteNT, &ppspriteNT, &ppspriteNT,
   &ppspriteNT, &ppspriteNT, &ppspriteNT, &ppspriteNT,
   &ppspriteNT, &ppspriteNT, &ppspriteNT, &ppspriteNT
};


/*
 *****************************************************************************
 */
static RwBool
SpriteInstanceCallBack(RxPS2AllPipeData *ps2AllPipeData, void **clusterData, RwUInt32 numClusters)
{
    const RpMesh       *mesh = (const RpMesh *) ps2AllPipeData->mesh;
    RwReal             *pos = (RwReal*)clusterData[0];
    RwUInt8            *RGBA = (RwUInt8*)clusterData[1];
    RwReal             *HW = (RwReal*)clusterData[2];
    RwReal             *uv1 = (RwReal*)clusterData[3];

    PPSprite           *sprList = _CurrentSpriteList;
    RwBool              aBool = FALSE;
    RwUInt32            i = numClusters;

    for (i = 0; i < mesh->numIndices; i++)
    {
        *pos++ = sprList->pos.x;
        *pos++ = sprList->pos.y;
        *pos++ = sprList->pos.z;

        *RGBA++ = sprList->color.red;
        *RGBA++ = sprList->color.green;
        *RGBA++ = sprList->color.blue;
        *RGBA++ = sprList->color.alpha;

        *HW++ = sprList->size.x * 0.5f;
        *HW++ = sprList->size.y * 0.5f;

        *uv1++ = sprList->uv1.x;
        *uv1++ = sprList->uv1.y;
        *uv1++ = sprList->uv2.x;
        *uv1++ = sprList->uv2.y;

        sprList++;
    }

    _CurrentSpriteList = sprList;
    aBool =_rwDMAOpenVIFPkt(0, 2+PPSPRITESTATICDATASTRYDE); /* 1 QW */
    if (aBool)
    {
        RwUInt32            prim = 0x0l;
        RwSkySplitBits128   packed;

        /* unpack transfer 1 quadwords */

        packed.field_64[1] = ((VIFCMD_CYCLE |
                               (4l << 8) |
                               ((4)))) |
            ((VIFCMD_UNPACK |
              (PPSPRITESTATICDATASTRYDE << 16) |
              ((long) (pipeASymbStaticDataStart))) << 32);
        packed.field_64[0] = (1l << 28) |
            (PPSPRITESTATICDATASTRYDE); /* 2 QW after the DMAtag */
        RWDMA_ADD_TO_PKT(packed.field128);

        /* giftag says use context 1 */
        prim = ( /* fix  */ 0x0l << 10 |
                 /* ctxt */ 0x0l << 9 |
                 /* fst  */ 0x0l << 8 |
                 /* aa1  */ 0x0l << 7 |
                 /* abe  */ 0x1l << 6 |
                 /* fge  */ 0x0l << 5 |
                 /* tme  */ 0x1l << 4 |
                 /* iip  */ 0x1l << 3 |
                 /* prim */ 0x6l << 0);
        /* 6 = sprite */

        packed.field_64[1] = 0x42412l; /* 5 registers, not 3, since we're writing a sprite */
        packed.field_64[0] = ( /* regs */ 0x5l << (60 - 32) |
                               /* flg  */ 0x0l << (58 - 32) |
                               /* prim */ prim << (47 - 32) |
                               /* pre  */ 0x1l << (46 - 32)) << 32;
        RWDMA_ADD_TO_PKT(packed.field128);
    }

    return TRUE;
}


/*
 *****************************************************************************
 */
static RxPipeline *
skyPPSpriteCreatePS2AllMatPipe(void)
{
    RxNodeDefinition   *defNodePS2AllMat = NULL;
    RxPipelineNode     *nodePS2AllMat = NULL;
    RxLockedPipe       *lockedPipe;
    RxPipeline         *pipe;

    pipe = RxPipelineCreate();
    if (pipe)
    {

        lockedPipe = RxPipelineLock(pipe);

        /* Get the PS2All Node */
        defNodePS2AllMat = RxNodeDefinitionGetPS2AllMat();

        if (defNodePS2AllMat == NULL)
        {
            RsErrorMessage(RWSTRING("Error : Could not get PS2AllMat node definition."));
            return NULL;
        }

        /* Add PS2 All fragment to the new pipeline */
        RxLockedPipeAddFragment(lockedPipe,
                                (RwUInt32 *) NULL,
                                defNodePS2AllMat,
                                (RxNodeDefinition *) NULL);

        /* Get a pointer to the new pipe's ps2allmat node */
        nodePS2AllMat = RxPipelineFindNodeByName(lockedPipe,
                                               defNodePS2AllMat->name,
                                               (RxPipelineNode *) NULL,
                                               (RwInt32 *) NULL);

        /* Setting cluster attributes */
        memcpy(&PPSClPS2xyz,&RxClPS2xyz,sizeof(RxClusterDefinition));
        PPSClPS2xyz.defaultAttributes &= ~CL_ATTRIB_OPAQUE;
        PPSClPS2xyz.defaultAttributes |= CL_ATTRIB_WRITE;
        PPSClPS2xyz.defaultAttributes |= CL_ATTRIB_REQUIRED;
        PPSClPS2xyz.defaultAttributes |= CL_ATTRIB_DONT_FILL;

        memcpy(&PPSClPS2rgba,&RxClPS2rgba,sizeof(RxClusterDefinition));
        PPSClPS2rgba.defaultAttributes &= ~CL_ATTRIB_OPAQUE;
        PPSClPS2rgba.defaultAttributes |= CL_ATTRIB_WRITE;
        PPSClPS2rgba.defaultAttributes |= CL_ATTRIB_REQUIRED;
        PPSClPS2rgba.defaultAttributes |= CL_ATTRIB_DONT_FILL;

        memcpy(&PPSClPS2hNw,&RxClPS2user1,sizeof(RxClusterDefinition));
        PPSClPS2hNw.defaultAttributes = CL_V2_32;
        PPSClPS2hNw.defaultAttributes |= CL_ATTRIB_WRITE;
        PPSClPS2hNw.defaultAttributes |= CL_ATTRIB_REQUIRED;
        PPSClPS2hNw.defaultAttributes |= CL_ATTRIB_DONT_FILL;

        memcpy(&PPSClPS2uv,&RxClPS2user2,sizeof(RxClusterDefinition));
        PPSClPS2uv.defaultAttributes = CL_V4_32;
        PPSClPS2uv.defaultAttributes |= CL_ATTRIB_WRITE;
        PPSClPS2uv.defaultAttributes |= CL_ATTRIB_REQUIRED;
        PPSClPS2uv.defaultAttributes |= CL_ATTRIB_DONT_FILL;

        /* Force the ps2allmat node to create those clusters */
        RxPipelineNodePS2AllMatGenerateCluster(nodePS2AllMat,
                                                &PPSClPS2xyz, CL_XYZ);

        RxPipelineNodePS2AllMatGenerateCluster(nodePS2AllMat,
                                                &PPSClPS2rgba, CL_RGBA);

        RxPipelineNodePS2AllMatGenerateCluster(nodePS2AllMat,
                                                &PPSClPS2hNw, CL_USER1);

        RxPipelineNodePS2AllMatGenerateCluster(nodePS2AllMat,
                                                &PPSClPS2uv, CL_USER2);

        /* Set the VU vertex buffer size, and stride */
        RxPipelineNodePS2AllMatSetVUBufferSizes(nodePS2AllMat,
                                                 pipeASymbStrideOfInputCluster, /* Stride of input cluster */
                                                 pipeASymbPointCount,           /* Tristrip vertex count */
                                                 pipeASymbPointCount/3);        /* Trilist vertex count */

        /* Unlock the pipe! */
        pipe = RxLockedPipeUnlock(lockedPipe);

        /* Allways re-find nodes after an unlock */
        nodePS2AllMat = RxPipelineFindNodeByName(pipe,
                                               defNodePS2AllMat->name,
                                               (RxPipelineNode *) NULL,
                                               (RwInt32 *) NULL);

        /* Set VIF offset */
        RxPipelineNodePS2AllMatSetVIFOffset(nodePS2AllMat,
                                             pipeASymbVIFOffset);

        /* Adding the VU code to the pipeline */
        /* Set the VU code array to point to our Custom VU pipeline */
        /**************   FOG   *   CLIP  *  TL/TS   * PEE/ISO*******/
        /**************    N    *    N    *    TL    *   PER  *************/
        cfxPS2ALLVUCodeArray[TRANSNFOG|TRANSNCL |TRANSLIST |TRANSPER]=&ppspritePRS;

        /**************    Y    *    N    *    TL    *   PER  *************/
        cfxPS2ALLVUCodeArray[TRANSFOG |TRANSNCL |TRANSLIST |TRANSPER]=&ppspritePRS;

        /**************    N    *    N    *    TS    *   PER  *************/
        cfxPS2ALLVUCodeArray[TRANSNFOG|TRANSNCL |TRANSSTRIP|TRANSPER]=&ppspritePRS;

        /**************    Y    *    N    *    TS    *   PER  *************/
        cfxPS2ALLVUCodeArray[TRANSFOG |TRANSNCL |TRANSSTRIP|TRANSPER]=&ppspritePRS;

        /**************    N    *    Y    *    TL    *   PER  *************/
        cfxPS2ALLVUCodeArray[TRANSNFOG|TRANSCLIP|TRANSLIST |TRANSPER]=&ppspritePRS;

        /**************    Y    *    Y    *    TL    *   PER  *************/
        cfxPS2ALLVUCodeArray[TRANSFOG |TRANSCLIP|TRANSLIST |TRANSPER]=&ppspritePRS;

        /**************    N    *    Y    *    TS    *   PER  *************/
        cfxPS2ALLVUCodeArray[TRANSNFOG|TRANSCLIP|TRANSSTRIP|TRANSPER]=&ppspritePRS;

        /**************    Y    *    Y    *    TS    *   PER  *************/
        cfxPS2ALLVUCodeArray[TRANSFOG |TRANSCLIP|TRANSSTRIP|TRANSPER]=&ppspritePRS;

        /**************    N    *    N    *    TL    *   ISO  *************/
        cfxPS2ALLVUCodeArray[TRANSNFOG|TRANSNCL |TRANSLIST |TRANSISO]=&ppspritePRS;

        /**************    Y    *    N    *    TL    *   ISO  *************/
        cfxPS2ALLVUCodeArray[TRANSFOG |TRANSNCL |TRANSLIST |TRANSISO]=&ppspritePRS;

        /**************    N    *    N    *    TS    *   ISO  *************/
        cfxPS2ALLVUCodeArray[TRANSNFOG|TRANSNCL |TRANSSTRIP|TRANSISO]=&ppspritePRS;

        /**************    Y    *    N    *    TS    *   ISO  *************/
        cfxPS2ALLVUCodeArray[TRANSFOG |TRANSNCL |TRANSSTRIP|TRANSISO]=&ppspritePRS;

        /**************    N    *    Y    *    TL    *   ISO  *************/
        cfxPS2ALLVUCodeArray[TRANSNFOG|TRANSCLIP|TRANSLIST |TRANSISO]=&ppspritePRS;

        /**************    Y    *    Y    *    TL    *   ISO  *************/
        cfxPS2ALLVUCodeArray[TRANSFOG |TRANSCLIP|TRANSLIST |TRANSISO]=&ppspritePRS;

        /**************    N    *    Y    *    TS    *   ISO  *************/
        cfxPS2ALLVUCodeArray[TRANSNFOG|TRANSCLIP|TRANSSTRIP|TRANSISO]=&ppspritePRS;

        /**************    Y    *    Y    *    TS    *   ISO  *******/
        cfxPS2ALLVUCodeArray[TRANSFOG |TRANSCLIP|TRANSSTRIP|TRANSISO]=&ppspritePRS;

        RxPipelineNodePS2AllMatSetVU1CodeArray(nodePS2AllMat,
                                                cfxPS2ALLVUCodeArray,
                                                VU1CODEARRAYSIZE);
        /* Set up the necessary callbacks */
         RxPipelineNodePS2AllMatSetCallBack(
             nodePS2AllMat, rxPS2ALLMATCALLBACKMESHINSTANCETEST,
             NULL); /* */

         RxPipelineNodePS2AllMatSetCallBack(
             nodePS2AllMat, rxPS2ALLMATCALLBACKRESENTRYALLOC,
             RwIm3DPS2AllResEntryAllocCallBack);

         RxPipelineNodePS2AllMatSetCallBack(
             nodePS2AllMat, rxPS2ALLMATCALLBACKINSTANCE,
             SpriteInstanceCallBack);

         RxPipelineNodePS2AllMatSetCallBack(
             nodePS2AllMat, rxPS2ALLMATCALLBACKBRIDGE,
             RwIm3DPS2AllBridgeCallBack);

#if (defined(RWMETRICS))
         RxPipelineNodePS2AllMatSetCallBack(
             nodePS2AllMat, rxPS2ALLMATCALLBACKPOSTMESH,
             RwIm3DPS2AllPostMeshCallBack);
#endif

    }

    return pipe;

}

/****************************************************************************
 CreateIm3DPS2AllRenderPipeline
 */
static RxPipeline*
skyPPSpriteCreatePS2AllPipe(RxPipeline *ps2AllMatPipe)
{
    RxPipeline *pipe;

    pipe = RxPipelineCreate();
    if (pipe)
    {
        RxLockedPipe       *lpipe;

        lpipe = RxPipelineLock(pipe);

        if (NULL != lpipe)
        {
            RxNodeDefinition   *ps2all = RxNodeDefinitionGetPS2All();
            RxPipelineNode     *plnode;

            lpipe = RxLockedPipeAddFragment(lpipe,
                                            (RwUInt32 *)NULL,
                                            ps2all,
                                            (RxNodeDefinition *)NULL);

            plnode = RxPipelineFindNodeByName(lpipe,
                                              ps2all->name,
                                              (RxPipelineNode *)NULL,
                                              (RwInt32 *)NULL);
            lpipe = RxLockedPipeUnlock(lpipe);

            if (lpipe != NULL)
            {
                plnode = RxPipelineFindNodeByName(lpipe,
                                                  ps2all->name,
                                                  (RxPipelineNode *)NULL,
                                                  (RwInt32 *)NULL);
                /* Set up the necesary callbacks */
                RxPipelineNodePS2AllSetCallBack(
                    plnode, rxPS2ALLCALLBACKOBJECTSETUP,
                    RwIm3DPS2AllObjectSetupCallBack);

                /* There are no materials to get pipes from, so group */
                RxPipelineNodePS2AllGroupMeshes(plnode, ps2AllMatPipe);

                return (pipe);
            }
        }

        RxPipelineDestroy(pipe);
    }

    return ((RxPipeline *)NULL);
}


/*
 *****************************************************************************
 */
static RxPipeline  *
skyPPSpriteCreatePipe(void)
{
    RxLockedPipe       *PS2AllPipe = NULL;
    
    MatPPSpritePipeline = skyPPSpriteCreatePS2AllMatPipe();
    if (MatPPSpritePipeline)
    {
            /*
             * Create an Object pipeline
             * and Attach the material Pipeline to it
             */
            PS2AllPipe = skyPPSpriteCreatePS2AllPipe(MatPPSpritePipeline);
    }

    return PS2AllPipe;
}


/*
 *****************************************************************************
 */
RwBool
PPSpriteCreatePipe(void)
{
    ObjPPSpritePipeline = skyPPSpriteCreatePipe();

    if( ObjPPSpritePipeline == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

/*
 *****************************************************************************
 */
void
PPSpriteDestroyPipe(void)
{
    if( NULL != ObjPPSpritePipeline )
    {
        RxPipelineDestroy(ObjPPSpritePipeline);
        ObjPPSpritePipeline = (RxPipeline *)NULL;
    }
    
    if( NULL != MatPPSpritePipeline )
    {
        RxPipelineDestroy(MatPPSpritePipeline);
        MatPPSpritePipeline = (RxPipeline *)NULL;
    }

    if( NULL != nullTMat )
    {
        RwMatrixDestroy(nullTMat);
        nullTMat = NULL;
    }    

    return;
}

/*
 *****************************************************************************
 */
RwBool
PPSPriteDraw(PPSprite *sprites, RwInt32 numSprites )
{
    if( NULL == nullTMat )
    {
        nullTMat = RwMatrixCreate();
        RwMatrixSetIdentity(nullTMat);

    }

    if(sprites)
    {
        rwIm3DPool         *pool;
        RwBool                          trueClipperState = RpSkyGetTrueTLClipper();
        /* establish geometry space */

        _CurrentSpriteList = sprites;

        pool = _rwIm3DGetPool();

        pool->elements = _CurrentSpriteList;

        pool->stride = sizeof(PPSprite);

        pool->stash.ltm = nullTMat;
        pool->stash.flags =  0;

        pool->stash.pipeline = (RxPipeline *) ObjPPSpritePipeline;
        pool->stash.primType = (RwPrimitiveType) rwPRIMTYPETRILIST;
        pool->stash.indices = (RxVertexIndex *) NULL;


        pool->numElements = ((RwUInt16) numSprites);
        pool->stash.numIndices = numSprites;
        pool->stash.numVerts = numSprites;

                /*
                 * Disable true TriList Clipper :
                 * use Large Clipping/Culling frustrum
                 * The user may have to switch it back on after
                 * Sprites has been rendered.
                 */
                RpSkySelectTrueTLClipper(FALSE);

        RxPipelineExecute(ObjPPSpritePipeline,  (void *) &pool->stash, TRUE);

                RpSkySelectTrueTLClipper(trueClipperState);

        return TRUE;
    }


    return FALSE;
}
