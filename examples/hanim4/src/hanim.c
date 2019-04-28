
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
 * hanim.c
 *
 * Copyright (C) 2001 CCriterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: To illustrate how two H-anim sequences can be blended together.
 * *****************************************************************************/

#include "rwcore.h"
#include "rpworld.h"
#include "rphanim.h"
#include "rpskin.h"
#include "rtcmpkey.h"
#include "skeleton.h"

#include "hanim.h"

/*
 * The total number of animations this example will run...
 */
#define NUMANIMS (2)

RtAnimAnimation *UncompressedAnim;
RtAnimAnimation *CompressedAnim;
RtAnimAnimation *CurrentAnim;
RwInt32          CompressedAnimSize;
RwInt32          UncompressedAnimSize;


/*
 *****************************************************************************
 */
static RtAnimAnimation *
LoadAnimationFile(RwChar *file)
{
    RtAnimAnimation *anim = NULL;
    RwChar *path = NULL;

    path = RsPathnameCreate(file);
    anim = RtAnimAnimationRead(path);
    RsPathnameDestroy(path);

    return anim;
}


/*
 *****************************************************************************
 */
static RpClump *
LoadClump(RpWorld *world)
{
    RpClump *clump = NULL;
    RwStream *stream = NULL;
    RwChar *path = NULL;

    path = RsPathnameCreate(RWSTRING("./models/clump.dff"));
    stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, path);
    RsPathnameDestroy(path);

    if( stream )
    {
        if( RwStreamFindChunk(stream, rwID_CLUMP, NULL, NULL) )
        {
            clump = RpClumpStreamRead(stream);
        }

        RwStreamClose(stream, NULL);
    }

    if( clump )
    {
        RwV3d yAxis = {0.0f, 1.0f, 0.0f};
        RwV3d pos = {0.0f, 0.0f, 150.0f};

        RwFrameRotate(RpClumpGetFrame(clump), &yAxis, 180.0f,
            rwCOMBINEREPLACE);

        RwFrameTranslate(RpClumpGetFrame(clump), &pos,
            rwCOMBINEPOSTCONCAT);

        RpWorldAddClump(world, clump);
    }

    return clump;
}


/*
 *****************************************************************************
 */
static RwFrame *
GetChildFrameHierarchy(RwFrame *frame, void *data)
{    
    RpHAnimHierarchy *hierarchy = *(RpHAnimHierarchy **)data;

    /*
     * Return the first hierarchy found that is attached to one of the atomic
     * frames...
     */

    hierarchy = RpHAnimFrameGetHierarchy(frame);
    if( hierarchy == NULL )
    {
        RwFrameForAllChildren(frame, GetChildFrameHierarchy, data);

        return frame;
    }

    *(void **)data = (void *)hierarchy;

    return NULL;
}


RpHAnimHierarchy *
GetHierarchy(RpClump *clump)
{
    RpHAnimHierarchy *hierarchy = NULL;
    
    /*
     * Return the hierarchy for this model...
     */

    RwFrameForAllChildren(RpClumpGetFrame(clump), GetChildFrameHierarchy,
        (void *)&hierarchy);

    return hierarchy;
}


/*
 *****************************************************************************
 */
static RpAtomic *
SetHierarchyForSkinAtomic(RpAtomic *atomic, void *data)
{
    RpSkinAtomicSetHAnimHierarchy(atomic, (RpHAnimHierarchy *)data);
    
    return atomic;
}


/*
 *****************************************************************************
 */
RpClump *
CreateClump(RpWorld *world)
{
    RwChar *path = NULL;
    RpClump *clump = NULL;  
    RpHAnimHierarchy *hierarchy = NULL;

    /*
     * Load the clump that contains the skinned atomics and hierarchies, the
     * animations that will be run on the hierarchies
     */

    clump = LoadClump(world);
    if( clump == NULL )
    {
        return NULL;
    }

    hierarchy = GetHierarchy(clump);

    RpClumpForAllAtomics(clump, SetHierarchyForSkinAtomic, (void *)hierarchy);

    /*
     * load the two animations
     */

    /* First load the compressed animation */
    path = RsPathnameCreate(RWSTRING("./models/compressed.anm"));
    CompressedAnim = LoadAnimationFile(path);

    /* If it failed to load try loading the uncompressed animation
       and convert it */
    if (!CompressedAnim)
    {
        RtAnimAnimation *uncomp = NULL;
        RwStream *stream = NULL;

        /* Load the uncompressed anim */
        path = RsPathnameCreate(RWSTRING("./models/uncompressed.anm"));
        uncomp = LoadAnimationFile(path);
        RsPathnameDestroy(path);
        
        /* Compress and stream out the animation */
        path = RsPathnameCreate(RWSTRING("./models/compressed.anm"));
        CompressedAnim = RtCompressedKeyConvertFromStdKey(uncomp);
        stream = RwStreamOpen(rwSTREAMWRITE, rwSTREAMFILENAME, path);
        RtAnimAnimationStreamWrite(CompressedAnim, stream);
        RwStreamClose(stream, NULL);

        /* Destroy the uncompressed animation */
        RtAnimAnimationDestroy(uncomp);
    }

    /* Load the uncompressed animation for use */
    RsPathnameDestroy(path);
    path = RsPathnameCreate(RWSTRING("./models/uncompressed.anm"));
    UncompressedAnim = LoadAnimationFile(path);
    RsPathnameDestroy(path);

    if( UncompressedAnim == NULL ||
        CompressedAnim == NULL)
    {
        return NULL;
    }
    
    /* Calculate the animation sizes */
    //TODO : work out exact
    UncompressedAnimSize = RtAnimAnimationStreamGetSize(UncompressedAnim);
    CompressedAnimSize = RtAnimAnimationStreamGetSize(CompressedAnim);


    /*
     * We will play this animation first...
     */
    CurrentAnim = CompressedAnim;
    RpHAnimHierarchySetCurrentAnim(hierarchy, CurrentAnim);

    return clump;
}


/*
 *****************************************************************************
 */
void
DestroyClump(RpClump *clump, RpWorld *world)
{
    if( clump )
    {
        RpWorldRemoveClump(world, clump);

        RpClumpDestroy(clump);
    }

    /*
     * Destroy only the hierarchies and animations the app created...
     */

    RtAnimAnimationDestroy(UncompressedAnim);
    RtAnimAnimationDestroy(CompressedAnim);

    return;
}


/*
 *****************************************************************************
 */
static RpAtomic *
GetSkinHierarchy(RpAtomic *atomic, void *data)
{
    *(void **)data = (void *)RpSkinAtomicGetHAnimHierarchy(atomic);
    
    return NULL;
}


/*
 *****************************************************************************
 */
void
UpdateAnimation(RpClump *clump, RwReal deltaTime)
{
    static RpHAnimHierarchy *hierarchy = NULL;

    RpClumpForAllAtomics(clump, GetSkinHierarchy, (void *)&hierarchy);

    RpHAnimHierarchyAddAnimTime(hierarchy, deltaTime);
    
    RpHAnimHierarchyUpdateMatrices(hierarchy);

   
    return;
}

/*
 *****************************************************************************
 */
