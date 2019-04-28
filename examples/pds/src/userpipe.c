
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
 * userpipe.c
 *
 * Copyright (C) 2001 Criterion Technologies.
 *
 * Author: RenderWare Team
 *
 ****************************************************************************/

#include "rwcore.h"
#include "rpworld.h"
#include "rtworld.h"
#include "rppds.h"

#include "ps2all.h"

/* Used to optimise pipeline execution */
RwBool PS2AllFirstAtomic = FALSE;
RwBool PS2AllOptimiseMatrices = FALSE;

/* Number of RpAtomics to display */
RwInt32 PS2AllNumDrawnAtomics = 0;

/*
 *****************************************************************************
 */

static RwBool
RPS2AllObjectSetupCB(RxPS2AllPipeData * ps2AllPipeData,
                     RwMatrix ** transform)
{
    RpAtomic           *atomic;
    RpGeometry         *geom;

    atomic = (RpAtomic *) (ps2AllPipeData->sourceObject);
    geom = atomic->geometry;

    /* We know that our RpAtomics have only one morphTarget, so
     * we can simplify RpAtomicPS2AllGetMeshHeaderMeshCache slightly
     * by unconditionally retrieving the RwMeshCache from the
     * RpGeometry. */

     /* RpAtomicPS2AllGetMeshHeaderMeshCache(atomic, ps2AllPipeData); */

    ps2AllPipeData->meshHeader = geom->mesh;
    ps2AllPipeData->meshCache =
        rpGeometryGetMeshCache(geom, geom->mesh->numMeshes);

    /* Gather metrics - this boils away to nothing in non-RWMETRICS builds */
    RpAtomicPS2AllGatherObjMetrics(atomic);

    /* We do not need to set up numMorphTargets and spExtra, because
     * we know that our RpAtomics all have one morphTarget only. */
    /* RpAtomicPS2AllMorphSetup(atomic, ps2AllPipeData); */

    /* We know that we're not editing our objects at run-time, so our
     * RpAtomics will not need re-instancing due to changes in the data. */
    /* RpAtomicPS2AllObjInstanceTest(atomic, ps2AllPipeData); */

    /* Objects aren't being edited, so dirty flags aren't
     * being set, so there's no need to clear them */
    /* RpAtomicPS2AllClear(atomic); */

    /* This makes sure that we do an initial instance (when resEntry == NULL) */
    ps2AllPipeData->objInstance = (RxInstanceFlags)
        ( (~rxINSTANCEDONTINSTANCE) &
          (RwUInt32)ps2AllPipeData->objInstance);

    /* This causes a reinstance if (geom->lockedSinceLastInst == rpGEOMETRYLOCKALL),
     * which is a signal from ps2AllStateCB that we need to reinstance because
     * we've swapped pipes (the instance data headers differ between PS2Manager and
     * PS2All) */
    if (geom->lockedSinceLastInst == rpGEOMETRYLOCKALL)
    {
        ps2AllPipeData->objInstance = (RxInstanceFlags)
            (rxINSTANCEFULLINSTANCE |
             (RwUInt32)ps2AllPipeData->objInstance);

        geom->lockedSinceLastInst = 0;
    }

    /* We need to cache the transform as usual.
     * NOTE: in the case of (for example) RpWorldSectors, many objects
     *   share the same transformation matrix, since their geometry is
     *   specified in world-space (geometry specified in camera-space
     *   would be optimizable in the same way). This means that the
     *   matrix need be uploaded only for the first sector in a world.
     *   For the rest of the sectors, we could use this code here instead:
     *      *transform = NULL;
     *   In the case of this example, this modification achieves a speed
     *   increase (in this optimized pipeline) of about 15% */
    RpAtomicPS2AllTransformSetup(atomic, transform);

    /* We don't need to do a frustum test here because we
     * have positioned the camera such that all RpAtomics lie wholly
     * within its view frustum */
    /* RpAtomicPS2AllFrustumTest(atomic, &inFrustum); */

    /* Set up primType and transtype - in our case, we know that
     * all RpAtomics to be rendered are formed from tristrips and
     * that they all lie fully inside the viewing frustum. Hence,
     * we can inline a vastly simplified version of
     * RpAtomicPS2AllPrimTypeTransTypeSetup(): */
    ps2AllPipeData->primType = 4;
    ps2AllPipeData->transType = skyTransType;

    /* We know that we don't modulate with material colours for
     * this geometry - we let matModulate default to FALSE */
    /* RpAtomicPS2AllMatModulateSetup(atomic, ps2AllPipeData); */

    /* All but the first RpAtomic rendered per frame need update
     * no data in the lighting data buffer. The data describing
     * the inverse lighting matrix and the lights themselves
     * persists from the first RpAtomic. */

     if (FALSE == PS2AllFirstAtomic)
     {
         /* We know that the only lights in scene (one directional and one
          * ambient) are position-independent, so there's no need to update
          * the inverse lighting matrix uploaded by the first RpAtomic. */
         RpAtomicPS2AllLightingPersist(NULL, 1.0f, 1.0f);

         return (TRUE);
    }

    /* The first RpAtomic rendered must generate
     * lighting information to be uploaded */
    RpAtomicPS2AllLightingSetup(ps2AllPipeData);

    return (TRUE);
}

/*
 *****************************************************************************
 */

static RwBool
PS2AllBridgeCB1(RxPS2AllPipeData * ps2AllPipeData)
{
    RwUInt32 numInitialQW = 0, numExtraQW = 0;

    if (FALSE == PS2AllFirstAtomic)
    {
        /* For all but the first RpAtomic rendered per frame, all we
         * do here, essentially, is kick off geometry transfer! */
        RpMeshPS2AllStartVIFUploads(numInitialQW, numExtraQW);
        RpMeshPS2AllEndVIFUploads(ps2AllPipeData);

        return (TRUE);
    }

    /* For the first RpAtomic rendered in a frame, we have to set up
     * state which will persist to the rest of the RpAtomics. */

    /* Asynchronously upload the texture if necessary and possible */
    RpMeshPS2AllAsyncTextureUpload(ps2AllPipeData);
    /* If changed, updates global renderstate and calls skyTexCacheAccessRaster */
    RpMeshPS2AllSyncTextureUpload(ps2AllPipeData);

    /* Open a VIF transfer packet, with a header DMA tag */
    numInitialQW += rpMESHPS2ALLGIFTAGNUMINITIALQW +
        rpMESHPS2ALLMATCOLNUMINITIALQW +
        rpMESHPS2ALLSURFPROPSNUMINITIALQW +
        rpMESHPS2ALLCLIPINFONUMINITIALQW +
        rpMESHPS2ALLTEXTURESTATENUMINITIALQW;
    numExtraQW = rpMESHPS2ALLVU1CODEUPLOADNUMEXTRAQW;
    RpMeshPS2AllStartVIFUploads(numInitialQW, numExtraQW);

    /* Extra user VIF uploads would go here... VIF tag(s) necessary.
     * Would have called _rxPS2AllStartVIFUploads with FALSE if needed
     * DMA tags in the user uploads (would have had to fix up with a
     * terminal DMA tag so the following standard transfers work). */

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
     * only, ergo it can be combined with other VIF stuff! :) ) */
    RpMeshPS2AllTextureStateUpload(ps2AllPipeData);

    /* Set up vu1CodeIndex */
    RpMeshPS2AllVU1CodeIndexSetup(ps2AllPipeData);
    /* Upload the VU1 code (if it changed) last, since it uses a DMA tag (ref transfer of the code) */
    RpMeshPS2AllVU1CodeUpload(ps2AllPipeData);

    /* Kicks off geometry transfer, sets up refCnt/clrCnt */
    RpMeshPS2AllEndVIFUploads(ps2AllPipeData);

    return (TRUE);
}

static RwBool
PS2AllBridgeCB2(RxPS2AllPipeData * ps2AllPipeData)
{
    RwUInt32 numInitialQW = 0, numExtraQW = 0;

    /* For the first RpAtomic rendered in a frame, we have to set up
     * state which will persist to the rest of the RpAtomics. */

    /* Asynchronously upload the texture if necessary and possible */
    RpMeshPS2AllAsyncTextureUpload(ps2AllPipeData);
    /* If changed, updates global renderstate and calls skyTexCacheAccessRaster */
    RpMeshPS2AllSyncTextureUpload(ps2AllPipeData);

    /* Open a VIF transfer packet, with a header DMA tag */
    numInitialQW += rpMESHPS2ALLGIFTAGNUMINITIALQW +
        rpMESHPS2ALLMATCOLNUMINITIALQW +
        rpMESHPS2ALLSURFPROPSNUMINITIALQW +
        rpMESHPS2ALLCLIPINFONUMINITIALQW +
        rpMESHPS2ALLTEXTURESTATENUMINITIALQW;
    numExtraQW = rpMESHPS2ALLVU1CODEUPLOADNUMEXTRAQW;
    RpMeshPS2AllStartVIFUploads(numInitialQW, numExtraQW);

    /* Extra user VIF uploads would go here... VIF tag(s) necessary.
     * Would have called _rxPS2AllStartVIFUploads with FALSE if needed
     * DMA tags in the user uploads (would have had to fix up with a
     * terminal DMA tag so the following standard transfers work). */

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
     * only, ergo it can be combined with other VIF stuff! :) ) */
    RpMeshPS2AllTextureStateUpload(ps2AllPipeData);

    /* Set up vu1CodeIndex */
    RpMeshPS2AllVU1CodeIndexSetup(ps2AllPipeData);
    /* Upload the VU1 code (if it changed) last, since it uses a DMA tag (ref transfer of the code) */
    RpMeshPS2AllVU1CodeUpload(ps2AllPipeData);

    /* Kicks off geometry transfer, sets up refCnt/clrCnt */
    RpMeshPS2AllEndVIFUploads(ps2AllPipeData);

    {
        RwMatrix *matrix = RwMatrixCreate();
        RwMatrix * const viewMatrix = &(((RwCamera *)RWSRCGLOBAL(curCamera))->viewMatrix);
        rwPS2AllResEntryHeader *p2rh;

        RwUInt128 objToCameraTag;
        RwUInt128 kickGeometry;

        RwUInt128 ltmp;
        RwUInt64 tmp, tmp1;

        RwUInt32 i;

        p2rh = RWPS2ALLRESENTRYHEADERFROMRESENTRY(*(ps2AllPipeData->cacheEntryRef));

        tmp = (1L << 28) | (4);
        tmp1 = ( ( ( (0x6cl << 24) |
               ( /**/ (4) /**/ << 16) |
               ((RwUInt64)(VU1DATABLOCKHIGH))) << 32) |
               ( (1l << 24) | (4 << 8) | (4) ) );
        MAKE128(objToCameraTag, tmp1, tmp);

        tmp  = (((RwUInt64)(RwUInt32)(p2rh->data)) << 32) | (5L << 28);
        tmp1 = (3L << 24) | 0 |
             (((2L << 24) | ps2AllPipeData->matPvtData->vifOffset) << 32);
        MAKE128(kickGeometry, tmp1, tmp);

        for (i = 1; i < (RwUInt32)PS2AllNumDrawnAtomics; i++)
        {
            RpAtomic *atomic = PS2AllAtomics[i];
            RwFrame *frame = RpAtomicGetFrame(atomic);
            RwMatrix *mpLocalToWorld = RwFrameGetLTM(frame);

            RwUInt32 *matVector;

            RwMatrixMultiplyMacro(matrix, mpLocalToWorld, viewMatrix);

            _rwDMAOpenVIFPkt(0, 6);

            RWDMA_ADD_TO_PKT(objToCameraTag);

            matVector = (RwUInt32 *)(&matrix->right);
            tmp  = matVector[0] | ((RwUInt64)(matVector[1])<<32);
            tmp1 = matVector[2] | ((RwUInt64)(matVector[2])<<32);
            MAKE128(ltmp, tmp1, tmp);
            RWDMA_ADD_TO_PKT(ltmp);

            matVector = (RwUInt32 *)(&matrix->up);
            tmp  = matVector[0] | ((RwUInt64)(matVector[1])<<32);
            tmp1 = matVector[2] | ((RwUInt64)(matVector[2])<<32);
            MAKE128(ltmp, tmp1, tmp);
            RWDMA_ADD_TO_PKT(ltmp);

            matVector = (RwUInt32 *)(&matrix->at);
            tmp  = matVector[0] | ((RwUInt64)(matVector[1])<<32);
            tmp1 = matVector[2] | ((RwUInt64)(matVector[2])<<32);
            MAKE128(ltmp, tmp1, tmp);
            RWDMA_ADD_TO_PKT(ltmp);

            matVector = (RwUInt32 *)(&matrix->pos);
            tmp  = matVector[0] | ((RwUInt64)(matVector[1])<<32);
            tmp1 = matVector[2] | ((RwUInt64)(matVector[2])<<32);
            MAKE128(ltmp, tmp1, tmp);
            RWDMA_ADD_TO_PKT(ltmp);

            RWDMA_ADD_TO_PKT(kickGeometry);
        }

        /* Reference count stuff (used when objects are destroyed)  */
        /* clrCnt improves efficiency by allowing many reference    */
        /* increments in one packet to be performed in one go       */
        RWDMA_CRITICAL_SECTION_BEGIN();
        p2rh->refCnt += 1;
        RWDMA_CRITICAL_SECTION_END();
        p2rh->clrCnt += 1;
        if (p2rh->clrCnt == 1)
        {
            _rwDMAAddPURef(&(p2rh->refCnt));
        }

        RwMatrixDestroy(matrix);
    }

    return (TRUE);
}

/*
 *****************************************************************************
 */

RwBool
PS2AllOptimizedPipelinesCreate()
{
    static RpPDSSkyVU1CodeTemplate UserOpt_VUCode =
    {
        &rwPDS_G3_Generic_VU1Transforms[0],     /* VU1 code array.      */
        rwPDS_G3_Generic_CodeArraySize          /* VU1 array lenght.    */
    };

    static RpPDSSkyMatTemplate UserOpt1_MatPipe =
    {
        0,                                      /* PluginId.            */
        User1MatID,                             /* PluginData.          */
        { { &RxClPS2xyz,    CL_XYZ    },        /* Cluster1.            */
          { &RxClPS2uv,     CL_UV     },        /* Cluster2.            */
          { &RxClPS2rgba,   CL_RGBA   },        /* Cluster3.            */
          { &RxClPS2normal, CL_NORMAL },        /* Cluster4.            */
          { NULL,           CL_MAXCL  },        /* Cluster5.            */
          { NULL,           CL_MAXCL  },        /* Cluster6.            */
          { NULL,           CL_MAXCL  },        /* Cluster7.            */
          { NULL,           CL_MAXCL  },        /* Cluster8.            */
          { NULL,           CL_MAXCL  },        /* Cluster9.            */
          { NULL,           CL_MAXCL  } },      /* Cluster10.           */
        { rwPRIMTYPETRISTRIP,                   /* Primitive type.      */
          _rwskyStrideOfInputCluster,           /* ClusterStride.       */
          _rwskyTSVertexCount,                  /* VertexCount.         */
          _rwskyTLTriCount,                     /* PrimCount.           */
          _rwskyVIFOffset },                    /* VifOffset.           */
        &UserOpt_VUCode,                        /* VUCode array.        */
        NULL,                                   /* InstanceTestCallBack */
        RpMeshPS2AllResEntryAllocCallBack,      /* ResEntryAllocCallBack*/
        RpMeshPS2AllInstanceCallBack,           /* InstanceCallBack     */
        PS2AllBridgeCB1,                        /* BridgeCallBack       */
        NULL                                    /* PostMeshCallBack     */
    };

    static RpPDSSkyMatTemplate UserOpt2_MatPipe =
    {
        0,                                      /* PluginId.            */
        User2MatID,                             /* PluginData.          */
        { { &RxClPS2xyz,    CL_XYZ    },        /* Cluster1.            */
          { &RxClPS2uv,     CL_UV     },        /* Cluster2.            */
          { &RxClPS2rgba,   CL_RGBA   },        /* Cluster3.            */
          { &RxClPS2normal, CL_NORMAL },        /* Cluster4.            */
          { NULL,           CL_MAXCL  },        /* Cluster5.            */
          { NULL,           CL_MAXCL  },        /* Cluster6.            */
          { NULL,           CL_MAXCL  },        /* Cluster7.            */
          { NULL,           CL_MAXCL  },        /* Cluster8.            */
          { NULL,           CL_MAXCL  },        /* Cluster9.            */
          { NULL,           CL_MAXCL  } },      /* Cluster10.           */
        { rwPRIMTYPETRISTRIP,                   /* Primitive type.      */
          _rwskyStrideOfInputCluster,           /* ClusterStride.       */
          _rwskyTSVertexCount,                  /* VertexCount.         */
          _rwskyTLTriCount,                     /* PrimCount.           */
          _rwskyVIFOffset },                    /* VifOffset.           */
        &UserOpt_VUCode,                        /* VUCode array.        */
        NULL,                                   /* InstanceTestCallBack */
        RpMeshPS2AllResEntryAllocCallBack,      /* ResEntryAllocCallBack*/
        RpMeshPS2AllInstanceCallBack,           /* InstanceCallBack     */
        PS2AllBridgeCB2,                        /* BridgeCallBack       */
        NULL                                    /* PostMeshCallBack     */
    };

    static RpPDSSkyObjTemplate UserOpt1_AtmPipe =
    {
        0,
        User1AtmID,
        RPS2AllObjectSetupCB,
        (RxPipelineNodePS2AllObjectFinalizeCallBack)NULL,
        (RxPipeline *)NULL
    };

    static RpPDSSkyObjTemplate UserOpt2_AtmPipe =
    {
        0,
        User2AtmID,
        RpAtomicPS2AllObjectSetupCallBack,
        (RxPipelineNodePS2AllObjectFinalizeCallBack)NULL,
        (RxPipeline *)NULL
    };

    RpPDSRegisterMatPipe(&UserOpt1_MatPipe, User1MatID);
    RpPDSRegisterMatPipe(&UserOpt2_MatPipe, User2MatID);
    RpPDSRegisterObjPipe(&UserOpt1_AtmPipe, User1AtmID);
    RpPDSRegisterObjPipe(&UserOpt2_AtmPipe, User2AtmID);

    return TRUE;
}

/*
 *****************************************************************************
 */
