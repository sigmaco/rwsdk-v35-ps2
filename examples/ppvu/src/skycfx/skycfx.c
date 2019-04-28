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
 * skycfx.c
 *
 * Copyright (C) 2001 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: To illustrate the possibility of creating a custom VU pipe
 *
 ****************************************************************************/

#include "rwcore.h"
#include "rpworld.h"
#include "skeleton.h"

#include "cfx.h"
#include "skycfx.h"
#include "stddata.h"

#define VIFCMD_DIRECT (0x50L << 24)
#define VIFCMD_NOP    (0x00L << 24)
#define VIFCMD_UNPACK (0x6CL << 24)
#define VIFCMD_CYCLE  (0x01L << 24)


/*
 *****************************************************************************
 */
static RwBool
SkyCustomFXBridgeCallBack(RxPS2AllPipeData *ps2AllPipeData)
{
    RwUInt32  numInitialQW, numUserInitialQW, numExtraQW;
    RwUInt32  prim, primSwitch, fogSwitch;
    RwRaster *customRaster;
    RwUInt64  tmp, tmp1;
    u_long128 ltmp = 0;

    /* Set up our second-pass texture */
    RSASSERT(NULL != CustomTexture);
    customRaster = RwTextureGetRaster(CustomTexture);
    RSASSERT(NULL != customRaster);
    if (NULL != customRaster)
    {
        RpSkyTexCacheAccessSpeculate(customRaster);
        RpSkyTexCacheAccessRaster(customRaster, TRUE);
    }


    /* Asynchronously upload the texture if nec/poss */
    RpMeshPS2AllAsyncTextureUpload(ps2AllPipeData);
    /* If changed, does skyTexCacheAccessRaster and updates global renderstate */
    RpMeshPS2AllSyncTextureUpload(ps2AllPipeData);

    /* We will transfer 1QW for a VIF tag (transferring 1QW of context 2 GS settings
     * and 2QW of data for the texture transform) and 1QW for a direct-to-GS VIF tag
     * (transferring a GIF tag and 3QW of context-2 state settings) */
    numUserInitialQW = 9;

    /* Open a VIF transfer packet, with a header DMA tag */
    numInitialQW = rpMESHPS2ALLGIFTAGNUMINITIALQW +
                   rpMESHPS2ALLMATCOLNUMINITIALQW +
                   rpMESHPS2ALLSURFPROPSNUMINITIALQW +
                   rpMESHPS2ALLCLIPINFONUMINITIALQW +
                   rpMESHPS2ALLTEXTURESTATENUMINITIALQW +
                   numUserInitialQW;
    numExtraQW   = rpMESHPS2ALLVU1CODEUPLOADNUMEXTRAQW;
    RpMeshPS2AllStartVIFUploads(numInitialQW, numExtraQW);


    /*------ Begin custom FX uploads --------------------------*/
    RWDMA_LOCAL_BLOCK_BEGIN();

    /*------- Transfer Information ----------------------*/
    tmp = VIFCMD_NOP   | (VIFCMD_NOP << 32);
    tmp1 = (((VIFCMD_UNPACK | (/**/ 3 /**/ << 16) |
           ((RwUInt64) (pipeASymbStaticDataStart))) << 32) |
           (VIFCMD_CYCLE | (4 << 8) | (4)));
    MAKE128(ltmp, tmp1, tmp);
    RWDMA_ADD_TO_PKT(ltmp);


    /* Set the second-context primitive type to be the same as for context one */
    primSwitch = ps2AllPipeData->primType;
    /* Is fog active? */
    fogSwitch = ps2AllPipeData->transType & rxSKYTRANSTYPEFOG;
    RSASSERT(1 == rxSKYTRANSTYPEFOG);

    /*----------- Context 2 GifTag ------------*/
    prim = ( /* fix  */ 0x0L << 10 |
             /* ctxt */ 0x1L << 9 |
             /* fst  */ 0x0L << 8 |
             /* aa1  */ 0x0L << 7 |
             /* abe  */ 0x1L << 6 |
             /* fge  */ fogSwitch << 5 |
             /* tme  */ 0x1L << 4 |
             /* iip  */ 0x1L << 3 |
             /* prim */ primSwitch << 0);
    tmp = ( /* regs */ 0x3L << (60 - 32) |
            /* flg  */ 0x0L << (58 - 32) |
            /* prim */ prim << (47 - 32) |
            /* pre  */ 0x1L << (46 - 32)) << 32;
    tmp1 = 0x412L;             /* registers */
    MAKE128(ltmp, tmp1, tmp);
    RWDMA_ADD_TO_PKT(ltmp);

    /* CustomFX Data */

    /* Rotation Matrix Row 0 */
    tmp  = *(RwUInt64 *) (CFXRotMat0);
    tmp1 = *(RwUInt64 *) (CFXRotMat1);
    MAKE128(ltmp, tmp1, tmp);
    RWDMA_ADD_TO_PKT(ltmp);

    /* UV Translation */
    tmp  = *(RwUInt64 *) (CFXTrans);
    /* UV Scale */
    tmp1 = *(RwUInt64 *) (CFXScale);
    MAKE128(ltmp, tmp1, tmp);
    RWDMA_ADD_TO_PKT(ltmp);

    /* Transfer some QWs (context two state) direct-to-GS */
    tmp  =  VIFCMD_NOP  |  (VIFCMD_NOP << 32);
    tmp1 = (VIFCMD_NOP) | ((VIFCMD_DIRECT | 4) << 32);
    MAKE128(ltmp, tmp1, tmp);
    RWDMA_ADD_TO_PKT(ltmp);

    /* Set up the second context registers */
    tmp = /* NLOOP */  3L       /* Second Context settings */
        | /* EOP   */ (1L << 15)
        | /* PRE   */ (0L << 46)
        | /* FLG   */ (0L << 58)
        | /* NREG  */ (1L << 60);
    tmp1 = /* A+D */ (0xEL << (64 - 64));
    MAKE128(ltmp, tmp1, tmp);
    RWDMA_ADD_TO_PKT(ltmp);
    MAKE128(ltmp, GS_CLAMP_2, 0x0);
    RWDMA_ADD_TO_PKT(ltmp);
    /* Blend mode is Cf = Cd x (1-As) + As x Cs
     * Additive with Alpha FB combiner */
    MAKE128(ltmp, GS_ALPHA_2, 0x8000000044L);
    RWDMA_ADD_TO_PKT(ltmp);
    /* No alpha test */
    MAKE128(ltmp, GS_TEST_2, 0x50002L);
    RWDMA_ADD_TO_PKT(ltmp);

    RWDMA_LOCAL_BLOCK_END();
    /*------ End CustomFX VIF uploads --------------------------*/


    /* Here follow the standard VIF uploads */

    /* Upload a GIF tag for the code to submit geom to the GS under */
    RpMeshPS2AllGIFTagUpload(ps2AllPipeData);

    /* Upload material colour, dependent on whether there's a texture
     * (also does some renderstate setup based on alpha - that's why
     * this needs to be before the texture state setup func) */
    RpMeshPS2AllMatColUpload(ps2AllPipeData);

    /* Upload surface properties, including the 'extra' float in W */
    RpMeshPS2AllSurfPropsUpload(ps2AllPipeData);

    /* Sets up clipping info and J-C's switch QWs (NOTE: uses "transType&7") */
    RpMeshPS2AllClipInfoUpload(ps2AllPipeData);

    /* Upload texture renderstate to the GS (this sends stuff thru VIF
     * only, ergo can be combined with other VIF stuff! :) ). Note that this
     * is for the first-pass texture only (we did the second-pass state
     * ourselves above - which doesn't affect RW CPU-side renderstate) */
    RpMeshPS2AllTextureStateUpload(ps2AllPipeData);

    /* Set up vu1CodeIndex */
    RpMeshPS2AllVU1CodeIndexSetup(ps2AllPipeData);
    /* Upload the VU1 code (if it changed) last, since it uses a DMA tag (ref transfer of the code) */
    RpMeshPS2AllVU1CodeUpload(ps2AllPipeData);

    /* Kicks off geometry transfer, sets up refCnt/clrCnt */
    RpMeshPS2AllEndVIFUploads(ps2AllPipeData);

    return(TRUE);
}


/*
 *****************************************************************************
 */
static RxPipeline *
SkyCustomFXCreateMaterialPipe(void)
{
    static void        *cfxVUCodeArray[VU1CODEARRAYSIZE] =
    {
        &CustomFXNT, &CustomFXNT, &CustomFXNT, &CustomFXNT,
        &CustomFXNT, &CustomFXNT, &CustomFXNT, &CustomFXNT,
        &CustomFXNT, &CustomFXNT, &CustomFXNT, &CustomFXNT,
        &CustomFXNT, &CustomFXNT, &CustomFXNT, &CustomFXNT,
        &CustomFXNT, &CustomFXNT, &CustomFXNT, &CustomFXNT,
        &CustomFXNT, &CustomFXNT, &CustomFXNT, &CustomFXNT,
        &CustomFXNT, &CustomFXNT, &CustomFXNT, &CustomFXNT,
        &CustomFXNT, &CustomFXNT, &CustomFXNT, &CustomFXNT
    };

    RxPipeline          *newPipe = (RxPipeline *)NULL;
    RxPipelineNode      *plNode, *result;
    RxLockedPipe        *lPipe;
    RxNodeDefinition    *ps2AllMat;

    newPipe = RxPipelineCreate();
    RSASSERT(NULL != newPipe);

    lPipe = RxPipelineLock(newPipe);
    RSASSERT(NULL != lPipe);

    ps2AllMat = RxNodeDefinitionGetPS2AllMat();
    RSASSERT(NULL != ps2AllMat);

    lPipe = RxLockedPipeAddFragment( lPipe,
                                     (RwUInt32 *)NULL,
                                     ps2AllMat,
                                     (RxNodeDefinition *)NULL );
    RSASSERT(NULL != lPipe);

    plNode = RxPipelineFindNodeByName( lPipe,
                                       ps2AllMat->name,
                                       (RxPipelineNode *)NULL,
                                       (RwInt32 *)NULL );
    RSASSERT(NULL != plNode);

    result = RxPipelineNodePS2AllMatGenerateCluster(
                 plNode, &RxClPS2xyz, CL_XYZ);
    RSASSERT(NULL != result);
    plNode = RxPipelineNodePS2AllMatGenerateCluster(
                 plNode, &RxClPS2uv, CL_UV);
    RSASSERT(NULL != result);
    plNode = RxPipelineNodePS2AllMatGenerateCluster(
                 plNode, &RxClPS2rgba, CL_RGBA);
    RSASSERT(NULL != result);
    plNode = RxPipelineNodePS2AllMatGenerateCluster(
                 plNode, &RxClPS2normal, CL_NORMAL);
    RSASSERT(NULL != result);

    result = RxPipelineNodePS2AllMatSetTriangleVUBufferSizes(
                 plNode,
                 pipeASymbStrideOfInputCluster,
                 pipeASymbTSVertexCount,
                 pipeASymbTLTriCount );
    RSASSERT(NULL != result);

    /* Unlock! */
    newPipe = RxLockedPipeUnlock(lPipe);
    RSASSERT(NULL != newPipe);

    /* Best to re-find nodes after an unlock */
    plNode = RxPipelineFindNodeByName( lPipe,
                                       ps2AllMat->name,
                                       (RxPipelineNode *)NULL,
                                       (RwInt32 *)NULL );
    RSASSERT(plNode != NULL);

    result = RxPipelineNodePS2AllMatSetVIFOffset(
                 plNode, pipeASymbVIFOffset);
    RSASSERT(NULL != result);

    /* Set the VU code array to point to our Custom VU pipeline */
    cfxVUCodeArray[TRANSNFOG|TRANSNCL |TRANSLIST |TRANSPER] = &CustomFXPRS;
    cfxVUCodeArray[TRANSFOG |TRANSNCL |TRANSLIST |TRANSPER] = &CustomFXPRS;
    cfxVUCodeArray[TRANSNFOG|TRANSNCL |TRANSSTRIP|TRANSPER] = &CustomFXPRS;
    cfxVUCodeArray[TRANSFOG |TRANSNCL |TRANSSTRIP|TRANSPER] = &CustomFXPRS;
    cfxVUCodeArray[TRANSNFOG|TRANSCLIP|TRANSLIST |TRANSPER] = &CustomFXPRS;
    cfxVUCodeArray[TRANSFOG |TRANSCLIP|TRANSLIST |TRANSPER] = &CustomFXPRS;
    cfxVUCodeArray[TRANSNFOG|TRANSCLIP|TRANSSTRIP|TRANSPER] = &CustomFXPRS;
    cfxVUCodeArray[TRANSFOG |TRANSCLIP|TRANSSTRIP|TRANSPER] = &CustomFXPRS;
    
    cfxVUCodeArray[TRANSNFOG|TRANSNCL |TRANSLIST |TRANSISO] = &CustomFXPRL;
    cfxVUCodeArray[TRANSFOG |TRANSNCL |TRANSLIST |TRANSISO] = &CustomFXPRL;
    cfxVUCodeArray[TRANSNFOG|TRANSNCL |TRANSSTRIP|TRANSISO] = &CustomFXPRL;
    cfxVUCodeArray[TRANSFOG |TRANSNCL |TRANSSTRIP|TRANSISO] = &CustomFXPRL;
    cfxVUCodeArray[TRANSNFOG|TRANSCLIP|TRANSLIST |TRANSISO] = &CustomFXPRL;
    cfxVUCodeArray[TRANSFOG |TRANSCLIP|TRANSLIST |TRANSISO] = &CustomFXPRL;
    cfxVUCodeArray[TRANSNFOG|TRANSCLIP|TRANSSTRIP|TRANSISO] = &CustomFXPRL;
    cfxVUCodeArray[TRANSFOG |TRANSCLIP|TRANSSTRIP|TRANSISO] = &CustomFXPRL;

    /* Adding the VU code to the pipeline */
    result = RxPipelineNodePS2AllMatSetVU1CodeArray(
                 plNode, cfxVUCodeArray, VU1CODEARRAYSIZE);
    RSASSERT(NULL != result);

    /* Set up the necessary callbacks */
    RxPipelineNodePS2AllMatSetCallBack(
        plNode, rxPS2ALLMATCALLBACKMESHINSTANCETEST,
        RpMeshPS2AllMeshInstanceTestCallBack);
    RxPipelineNodePS2AllMatSetCallBack(
        plNode, rxPS2ALLMATCALLBACKRESENTRYALLOC,
        RpMeshPS2AllResEntryAllocCallBack);
    RxPipelineNodePS2AllMatSetCallBack(
        plNode, rxPS2ALLMATCALLBACKINSTANCE,
        RpMeshPS2AllInstanceCallBack);
    RxPipelineNodePS2AllMatSetCallBack(
        plNode, rxPS2ALLMATCALLBACKBRIDGE,
        SkyCustomFXBridgeCallBack);
#if (defined(RWMETRICS))
    RxPipelineNodePS2AllMatSetCallBack(
        plNode, rxPS2ALLMATCALLBACKPOSTMESH,
        RpMeshPS2AllPostMeshCallBack);
#endif /* (defined(RWMETRICS)) */

    return(newPipe);
}


/*
 *****************************************************************************
 */
RxPipeline *
SkyCustomFXCreatePipe(void)
{
    RxPipeline          *newPipe;
    RxPipeline          *matPipe;
    RxPipelineNode      *plNode, *result;
    RxLockedPipe        *lPipe;
    RxNodeDefinition    *ps2All;

    matPipe = SkyCustomFXCreateMaterialPipe();
    RSASSERT(NULL != matPipe);
    if (NULL == matPipe) return(NULL);

    newPipe = RxPipelineCreate();
    RSASSERT(NULL != newPipe);

    lPipe = RxPipelineLock(newPipe);
    RSASSERT(NULL != lPipe);

    ps2All = RxNodeDefinitionGetPS2All();
    RSASSERT(NULL != ps2All);

    lPipe = RxLockedPipeAddFragment( lPipe,
                                     (RwUInt32 *)NULL,
                                     ps2All,
                                     (RxNodeDefinition *)NULL );
    RSASSERT(NULL != lPipe);

    /* Unlock! */
    newPipe = RxLockedPipeUnlock(lPipe);
    RSASSERT(NULL != newPipe);

    /* Best to re-find nodes after an unlock */
    plNode = RxPipelineFindNodeByName( lPipe,
                                       ps2All->name,
                                       (RxPipelineNode *)NULL,
                                       (RwInt32 *)NULL );
    RSASSERT(plNode != NULL);

    /* This is an object pipeline ('all-in-one'), so use groupMeshes */
    result = RxPipelineNodePS2AllGroupMeshes(plNode, matPipe);
    RSASSERT(NULL != result);

    /* Set up the necessary callbacks */
    result = RxPipelineNodePS2AllSetCallBack(plNode,
        rxPS2ALLCALLBACKOBJECTSETUP, RpAtomicPS2AllObjectSetupCallBack);
    RSASSERT(NULL != result);

    return(newPipe);
}