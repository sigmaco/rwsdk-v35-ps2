
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
 * ps2all.c
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

#include "skeleton.h"

#include "menu.h"
#include "ps2all.h"

/* The number of atomics to start up with */
#define PS2ALLNUMALLOCEDATOMICS (70*70)

/* We can toggle between PS2Manager and PS2All render pipelines */
#define STATEPS2ALL_G3              0
#define STATEPS2ALL_G3x_ADL         1
#define STATEPS2ALL_G3x_A4D         2
#define STATEPS2ALL_G3x_APL         3
#define STATEPS2ALL_G3x_ADLClone    4
#define STATEPS2ALL_G3x_OPLClone    5
#define STATEPS2ALL_G3_cpu_opt1     6
#define STATEPS2ALL_G3_cpu_opt2     7

#define PS2ALLNUMSTATES 8

#define LIGHTS_ADL 0
#define LIGHTS_A4D 1
#define LIGHTS_APL 2

#define PSALLNUMLIGHTS 3

#define TEST_VU1 0
#define TEST_EE  1
#define TEST_MIX 2

#define TEST_VU1_VALUES 400, 4, 1
#define TEST_MIX_VALUES 40,  3, 1
#define TEST_EE_VALUES  4,   2, 1

#define PSALLNUMTESTS 3

/* Settings for different program states */
static RwUInt32
ps2AllStates[PS2ALLNUMSTATES] = { 25, 25, 25, 25, 25, 25, 25, 25 };

/* The RpGeometry which we'll procedurally generate */
static RpGeometry  *ps2AllGeometrys[PSALLNUMTESTS] = {NULL, NULL, NULL};
static RpGeometry  *ps2AllGeom = NULL;

/* What pipeline are we currently rendering with? */
static RwInt32      ps2AllState = STATEPS2ALL_G3;
static RwInt32      lightState = LIGHTS_ADL;
static RwInt32      testState = TEST_VU1;
static RwBool       preLightState = TRUE;

/* Whether to auto-set ps2AllNumDrawnAtomics to the
 * maximum possible at 60Hz for each configuration */
RwBool PSAllAutoMaximize = TRUE;

/* Used to differentiate between the menu and code in here accessing CBs */
RwBool PS2AllMenuCBAccess = TRUE;

#define AppPS2AllNumAtomicsCB(_justCheck)   \
MACRO_START                                 \
{                                           \
    PS2AllMenuCBAccess = FALSE;             \
    PS2AllNumAtomicsCB(_justCheck);         \
    PS2AllMenuCBAccess = TRUE;              \
}                                           \
MACRO_STOP

/* Useful values */
static const RwV3d XAxis = { 1, 0, 0 };
static const RwV3d YAxis = { 0, 1, 0 };
static const RwV3d ZAxis = { 0, 0, 1 };

extern RtCharset   *Charset;

/* An array of RpAtomics referencing the source RpGeometry */
RpAtomic **PS2AllAtomics = NULL;
RwFrame **PS2AllFrames = NULL;
RwRGBAReal *PS2AllColors = NULL;

/*
 *****************************************************************************
 */

/*
 *****************************************************************************
 */

static RwBool
preLightCB(RwBool justCheck)
{
    RwInt32 flags = RpGeometryGetFlags(ps2AllGeom);

    if (FALSE != justCheck)
    {
        return (TRUE);
    }

    if(preLightState)
    {
        /* Enable prelightcolors. */
        flags |= rpGEOMETRYPRELIT;
    }
    else
    {
        /* Disable prelightcolors. */
        flags &= (~rpGEOMETRYPRELIT);
    }

    (void)RpGeometrySetFlags(ps2AllGeometrys[TEST_VU1], flags);
    (void)RpGeometrySetFlags(ps2AllGeometrys[TEST_EE], flags);
    (void)RpGeometrySetFlags(ps2AllGeometrys[TEST_MIX], flags);

    /* Force a reinstance. */
    ps2AllGeometrys[TEST_VU1]->lockedSinceLastInst = rpGEOMETRYLOCKALL;
    ps2AllGeometrys[TEST_EE]->lockedSinceLastInst = rpGEOMETRYLOCKALL;
    ps2AllGeometrys[TEST_MIX]->lockedSinceLastInst = rpGEOMETRYLOCKALL;

    return TRUE;
}

/*
 *****************************************************************************
 */

static RwBool
testStateCB(RwBool justCheck)
{
    RwUInt32            i;

    if (FALSE != justCheck)
    {
        return (TRUE);
    }

    /* Get the new geometry. */
    ps2AllGeom = ps2AllGeometrys[testState];

    /* Replace the old geometry in each atomic. */
    for (i = 0; i < PS2ALLNUMALLOCEDATOMICS; i++)
    {
        RpAtomicSetGeometry(PS2AllAtomics[i], ps2AllGeom, 0);
    }

    return TRUE;
}

/*
 *****************************************************************************
 */

static RpLight *
removeLights(RpLight *light, void *data __RWUNUSED__)
{
    RpWorldRemoveLight(World, light);

    return light;
}

static RwBool
lightStateCB(RwBool justCheck)
{
    if (FALSE != justCheck)
    {
        return (TRUE);
    }

    RpWorldForAllLights(World, removeLights, NULL);

    switch(lightState)
    {
        case LIGHTS_ADL:
        {
            RpWorldAddLight(World, Ambiant);
            RpWorldAddLight(World, Direct[0]);
            break;
        }
        case LIGHTS_A4D:
        {
            RpWorldAddLight(World, Ambiant);
            RpWorldAddLight(World, Direct[0]);
            RpWorldAddLight(World, Direct[1]);
            RpWorldAddLight(World, Direct[2]);
            RpWorldAddLight(World, Direct[3]);
            break;
        }
        case LIGHTS_APL:
        {
            RpWorldAddLight(World, Ambiant);
            RpLightSetFrame(Point, RpAtomicGetFrame(PS2AllAtomics[0]));
            RpWorldAddLight(World, Point);
            break;
        }
        default:
        {
            break;
        }
    }

    return TRUE;
}

/*
 *****************************************************************************
 */

static RwBool
ps2AllStateCB(RwBool justCheck)
{
    RxPipeline *atomicPipeline = NULL;
    RxPipeline *materialPipeline = NULL;
    RwBool unoptimiseMatrices = TRUE;
    RwUInt32 i;

    if (FALSE != justCheck)
    {
        return (TRUE);
    }

    switch(ps2AllState)
    {
        case STATEPS2ALL_G3:
        {
            atomicPipeline = RpPDSGetPipe(rwPDS_G3_Generic_AtmPipeID);
            materialPipeline = RpPDSGetPipe(rwPDS_G3_Generic_MatPipeID);
            break;
        }
        case STATEPS2ALL_G3x_ADL:
        {
            atomicPipeline = RpPDSGetPipe(rwPDS_G3x_Generic_AtmPipeID);
            materialPipeline = RpPDSGetPipe(rwPDS_G3x_ADL_MatPipeID);
            break;
        }
        case STATEPS2ALL_G3x_A4D:
        {
            atomicPipeline = RpPDSGetPipe(rwPDS_G3x_Generic_AtmPipeID);
            materialPipeline = RpPDSGetPipe(rwPDS_G3x_A4D_MatPipeID);
            break;
        }
        case STATEPS2ALL_G3x_APL:
        {
            atomicPipeline = RpPDSGetPipe(rwPDS_G3x_Generic_AtmPipeID);
            materialPipeline = RpPDSGetPipe(rwPDS_G3x_APL_MatPipeID);
            break;
        }
        case STATEPS2ALL_G3x_ADLClone:
        {
            atomicPipeline = RpPDSGetPipe(rwPDS_G3x_ADLClone_AtmPipeID);
            materialPipeline = RpPDSGetPipe(rwPDS_G3x_ADLClone_MatPipeID);
            break;
        }
        case STATEPS2ALL_G3x_OPLClone:
        {
            atomicPipeline = RpPDSGetPipe(rwPDS_G3x_OPLClone_AtmPipeID);
            materialPipeline = RpPDSGetPipe(rwPDS_G3x_OPLClone_MatPipeID);
            break;
        }
        case STATEPS2ALL_G3_cpu_opt1:
        {
            atomicPipeline = RpPDSGetPipe(User1AtmID);
            materialPipeline = RpPDSGetPipe(User1MatID);
            break;
        }
        case STATEPS2ALL_G3_cpu_opt2:
        {
            atomicPipeline = RpPDSGetPipe(User1AtmID);
            materialPipeline = RpPDSGetPipe(User2MatID);

            unoptimiseMatrices = FALSE;
            PS2AllOptimiseMatrices = TRUE;
            break;
        }
        default:
        {
            break;
        }
    }

    /* Necessary for the cloning pipeline. */
    (void)RpAtomicSetPipeline(PS2AllAtomics[0], atomicPipeline);

    RpAtomicSetDefaultPipeline(atomicPipeline);
    RpMaterialSetDefaultPipeline(materialPipeline);

    if(unoptimiseMatrices)
    {
        for (i = 1; i < PS2ALLNUMALLOCEDATOMICS; i++)
        {
            RpAtomic *atomic = PS2AllAtomics[i];
            RwFrame *frame = RpAtomicGetFrame(atomic);

            RwFrameUpdateObjects(frame);
        }
    }

    /* Set the number of atomics drawn to the maximum
     * possible at 60Hz for this configuration, UNLESS
     * the user is manually adjusting the number */
    if (FALSE != PSAllAutoMaximize)
    {
        PS2AllNumDrawnAtomics = ps2AllStates[ps2AllState];
    }

    /* Move the camera into position */
    AppPS2AllNumAtomicsCB(FALSE);

    /* When swapping pipelines, make sure all data is reinstanced */
    for (i = 0; i < PSALLNUMTESTS; i++)
    {
        ps2AllGeometrys[i]->lockedSinceLastInst = rpGEOMETRYLOCKALL;
    }

    return (TRUE);
}

/*
 *****************************************************************************
 */

void
PS2AllRender(void)
{
    RwInt32 i;

    switch(ps2AllState)
    {
        case STATEPS2ALL_G3:
        {
            /* Render the atomics. */
            for (i = 0; i < PS2AllNumDrawnAtomics; i++)
            {
                RpAtomicRender(PS2AllAtomics[i]);
            }
            break;
        }
        case STATEPS2ALL_G3x_ADL:
        {
            /* Setup the lights and then render the atomics. */
            RpPDS_G3x_ADL_LightingUpload(Ambiant,Direct[0]);
            for (i = 0; i < PS2AllNumDrawnAtomics; i++)
            {
                RpAtomicRender(PS2AllAtomics[i]);
            }
            break;
        }
        case STATEPS2ALL_G3x_A4D:
        {
            /* Setup the lights and then render the atomics. */
            RpPDS_G3x_A4D_LightingUpload(Ambiant,Direct);
            for (i = 0; i < PS2AllNumDrawnAtomics; i++)
            {
                RpAtomicRender(PS2AllAtomics[i]);
            }
            break;
        }
        case STATEPS2ALL_G3x_APL:
        {
            /* Setup the lights and then render the atomics. */
            RpPDS_G3x_APL_LightingUpload(Ambiant,Point);
            for (i = 0; i < PS2AllNumDrawnAtomics; i++)
            {
                RpAtomicRender(PS2AllAtomics[i]);
            }
            break;
        }
        case STATEPS2ALL_G3x_ADLClone:
        {
            /* Setup the lights and then render the atomics. */
            RpPDS_G3x_ADLClone_LightingUpload(Ambiant,Direct[0]);
            {
                RwFrame  *frame = RpLightGetFrame(Direct[0]);
                RwMatrix *ltm = RwFrameGetLTM(frame);
                RwV3d *directionalLTMat = RwMatrixGetAt(ltm);

                RpPDS_G3x_ADLCloneRender( PS2AllAtomics[0],
                                          PS2AllFrames,
                                          PS2AllNumDrawnAtomics,
                                          directionalLTMat );
            }
            break;
        }
        case STATEPS2ALL_G3x_OPLClone:
        {
            RpPDS_G3x_OPLCloneRender( PS2AllAtomics[0],
                                      PS2AllFrames,
                                      PS2AllNumDrawnAtomics,
                                      PS2AllColors );
            break;
        }
        case STATEPS2ALL_G3_cpu_opt1:
        {
            /* Setup the lights once (in the pipeline) using a bool. */
            PS2AllFirstAtomic = TRUE;

            for (i = 0; i < PS2AllNumDrawnAtomics; i++)
            {
                RpAtomicRender(PS2AllAtomics[i]);
                PS2AllFirstAtomic = FALSE;
            }
            break;
        }
        case STATEPS2ALL_G3_cpu_opt2:
        {
            /* Only render the first atomic, and clone the rest. */
            PS2AllFirstAtomic = TRUE;
            RpAtomicRender(PS2AllAtomics[0]);
            break;
        }
        default:
        {
            break;
        }
    }

    return;
}

/*
 *****************************************************************************
 */

void
PS2AllOnScreenInfo(RwInt32 fps)
{
    /* Display VU1 stats */

    if(testState == TEST_VU1)
    {
        RwUInt32 triangles;
        RwUInt32 vertices;
        RwChar caption[256];

        triangles = RpGeometryGetNumTriangles(ps2AllGeom);
        vertices = RpGeometryGetNumVertices(ps2AllGeom);

        triangles *= PS2AllNumDrawnAtomics;
        vertices *= PS2AllNumDrawnAtomics;

        triangles *= fps;
        vertices *= fps;

        RsSprintf(caption, RWSTRING("TPS: %d"), triangles);
        RsCharsetPrint(Charset, caption, 0, 0, rsPRINTPOSBOTTOMRIGHT);

        RsSprintf(caption, RWSTRING("VPS: %d"), vertices);
        RsCharsetPrint(Charset, caption, 0, 1, rsPRINTPOSBOTTOMRIGHT);
    }

    return;
}

/*
 *****************************************************************************
 */

static RwBool
PS2AllMaximizeCB(RwBool justCheck)
{
    if (FALSE != justCheck)
    {
        return (TRUE);
    }

    /* From now on (until the user manually adjusts the number
     * of drawn RpAtomics), we'll maximize the number of drawn
     * RpAtomics on changes of configuration */
    PSAllAutoMaximize = TRUE;
    PS2AllNumDrawnAtomics = ps2AllStates[ps2AllState];
    AppPS2AllNumAtomicsCB(FALSE);

    return (TRUE);
}

/*
 *****************************************************************************
 */

void
PS2AllUpdate(RwReal deltaTime __RWUNUSED__)
{
    static RwBool first = TRUE;
    static RwMatrix rotMatrix;

    if (FALSE != first)
    {
        /* Position the camera appropriately to view the grid of RpAtomics.
         * We do this here, rather than in PS2AllOpen, because the Camera
         * is modified after PS2AllOpen is called, and PS2AllNumAtomicsCB
         * needs the Camera's final state to be set in order to position
         * it correctly. */
        AppPS2AllNumAtomicsCB(FALSE);

        /* Set up a matrix for light animation */
        RwMatrixRotate(&rotMatrix, &YAxis, 2, rwCOMBINEREPLACE);

        first = FALSE;
        return;
    }

    /* Rotate the directional light */
    RwFrameTransform(RpLightGetFrame(DirectionalLight), &rotMatrix,
                     rwCOMBINEPOSTCONCAT);

    return;
}

/*
 *****************************************************************************
 */

void
PS2AllMenuOpen(void)
{
    static RwChar numAtomicsLabel[]   = RWSTRING("Num atomics");
    static RwChar numAtomics25Label[] = RWSTRING("Num [x25]");
    static RwChar pipeTypeLabel[] = RWSTRING("Pipeline");
    static RwChar name1[] = RWSTRING("G3_Generic");
    static RwChar name2[] = RWSTRING("G3x_ADL");
    static RwChar name3[] = RWSTRING("G3x_A4D");
    static RwChar name4[] = RWSTRING("G3x_APL");
    static RwChar name5[] = RWSTRING("G3x_ADLClone");
    static RwChar name6[] = RWSTRING("G3x_OPLClone");
    static RwChar name7[] = RWSTRING("UserOpt1");
    static RwChar name8[] = RWSTRING("UserOpt2");
    static RwChar *pipeTypeNames[PS2ALLNUMSTATES] =
        { name1, name2, name3, name4, name5, name6, name7, name8 };
    static RwChar lightTypeLabel[] = RWSTRING("Lighting");
    static RwChar light1[] = RWSTRING("Ambient + Directional");
    static RwChar light2[] = RWSTRING("Ambient + 4 Directionals");
    static RwChar light3[] = RWSTRING("Ambient + Point");
    static RwChar *lightTypeNames[PSALLNUMLIGHTS] =
        { light1, light2, light3 };
    static RwChar preLightLabel[] = RWSTRING("Pre Light Colors");
    static RwChar testTypeLabel[] = RWSTRING("Pipeline Test");
    static RwChar test1[] = RWSTRING("VU1");
    static RwChar test2[] = RWSTRING("EE");
    static RwChar test3[] = RWSTRING("Mix");
    static RwChar *testTypeNames[PSALLNUMTESTS] =
        { test1, test2, test3 };

    MenuAddSeparator();

    /* Add an entry to allow changing the number of atomics drawn */
    MenuAddEntryInt(numAtomicsLabel, /* "Number of atomics" */
                    &PS2AllNumDrawnAtomics, /* The value variable */
                    PS2AllNumAtomicsCB, /* Callback for value change */
                    1,         /* Minimum value */
                    PS2ALLNUMALLOCEDATOMICS, /* Maximum value */
                    1,         /* Step size for value change */
                    NULL);     /* No names for particular values */

    /* Add an entry to allow changing the number of atomics drawn */
    MenuAddEntryInt(numAtomics25Label, /* "Number of atomics" */
                    &PS2AllNumDrawnAtomics, /* The value variable */
                    PS2AllNumAtomicsCB, /* Callback for value change */
                    1,         /* Minimum value */
                    PS2ALLNUMALLOCEDATOMICS, /* Maximum value */
                    25,        /* Step size for value change */
                    NULL);     /* No names for particular values */

    /* Add an entry to allow changing between render pipelines */
    MenuAddEntryInt(pipeTypeLabel, /* "Render pipeline" */
                    &ps2AllState, /* The value variable */
                    ps2AllStateCB, /* Callback for value change */
                    STATEPS2ALL_G3, /* Minimum value */
                    STATEPS2ALL_G3_cpu_opt2, /* Maximum value */
                    1,         /* Step size for value change */
                    (const char **)pipeTypeNames);

    /* Add an entry to allow changing between lighting setup */
    MenuAddEntryInt(lightTypeLabel,
                    &lightState,
                    lightStateCB,
                    LIGHTS_ADL,
                    LIGHTS_APL,
                    1,
                    (const char **)lightTypeNames);

    /* Add entry to toggle the prelightcolors */
    MenuAddEntryBool(preLightLabel,
                     &preLightState,
                     preLightCB );

    /* Add entry to test vu1 and ee */
    MenuAddEntryInt(testTypeLabel,
                    &testState,
                    testStateCB,
                    TEST_VU1,
                    TEST_MIX,
                    1,
                    (const char **)testTypeNames);

    return;
}

/*
 *****************************************************************************
 */

void
PS2AllClose(void)
{
    RpAtomic           *atomic;
    RwFrame            *frame;
    RwUInt32            i;

    if (NULL != PS2AllAtomics)
    {
        /* Destroy all the RpAtomics we allocated */
        for (i = 0; i < PS2ALLNUMALLOCEDATOMICS; i++)
        {
            atomic = PS2AllAtomics[i];
            if (NULL != atomic)
            {
                RpWorld *world = RpAtomicGetWorld(PS2AllAtomics[i]);
                
                if(NULL != world)
                {
                    RpWorldRemoveAtomic(world, PS2AllAtomics[i]);
                }
                
                frame = RpAtomicGetFrame(PS2AllAtomics[i]);
                RpAtomicSetFrame(PS2AllAtomics[i], (RwFrame *) NULL);
                RwFrameDestroy(frame);
                RpAtomicDestroy(PS2AllAtomics[i]);
            }
        }

        /* Destroy the array of RpAtomic pointers */
        RwFree(PS2AllAtomics);
        PS2AllAtomics = (RpAtomic **) NULL;
    }

    /* Destroy our generated RpGeometry */
    if (NULL != ps2AllGeometrys[TEST_VU1])
    {
        RpGeometryDestroy(ps2AllGeometrys[TEST_VU1]);
    }
    if (NULL != ps2AllGeometrys[TEST_EE])
    {
        RpGeometryDestroy(ps2AllGeometrys[TEST_EE]);
    }
    if (NULL != ps2AllGeometrys[TEST_MIX])
    {
        RpGeometryDestroy(ps2AllGeometrys[TEST_MIX]);
    }
    ps2AllGeom = NULL;

    /* Destroy our texture */
    if (NULL != ps2AllTexture)
    {
        RwTextureDestroy(ps2AllTexture);
        ps2AllTexture = NULL;
    }

    return;
}

/*
 *****************************************************************************
 */

RwBool
PS2AllOpen(void)
{
    RwChar             *path;
    RwBool              result;

    /* Create the PS2All and PS2AllMat pipelines */
    result = PS2AllOptimizedPipelinesCreate();

    /* Load a texture */
    path = RsPathnameCreate(RWSTRING("./textures/"));
    RwImageSetPath(path);
    RsPathnameDestroy(path);
    ps2AllTexture = RwTextureRead(RWSTRING("flagstone"), NULL);

    /* Procedurally generate an RpGeometry
     * NOTE: because the PS2All optimized pipeline can render so many
     * RpAtomics per frame, we have to make this a very small geometry
     * in order that VU1 can render as many in a frame! */
    ps2AllGeometrys[TEST_VU1] = PS2AllGeometryCreate(TEST_VU1_VALUES);
    ps2AllGeometrys[TEST_EE]  = PS2AllGeometryCreate(TEST_EE_VALUES);
    ps2AllGeometrys[TEST_MIX] = PS2AllGeometryCreate(TEST_MIX_VALUES);
    ps2AllGeom = ps2AllGeometrys[TEST_VU1];

    /* Alloc the ps2AllAtomics array of RpAtomic pointers */
    {
        RpAtomic **array;
        RwFrame **frames;
        RwRGBAReal *colors;

        array =
            (RpAtomic **)RwMalloc(PS2ALLNUMALLOCEDATOMICS *
                                  sizeof(RpAtomic *));
        frames =
            (RwFrame **)RwMalloc(PS2ALLNUMALLOCEDATOMICS *
                                 sizeof(RwFrame *));
        colors =
            (RwRGBAReal *)RwMalloc(PS2ALLNUMALLOCEDATOMICS *
                                   sizeof(RwRGBAReal));

        PS2AllAtomics = array;
        PS2AllFrames  = frames;
        PS2AllColors  = colors;

        memset(PS2AllAtomics, 0,
               PS2ALLNUMALLOCEDATOMICS * sizeof(RpAtomic *));
        memset(PS2AllFrames, 0,
               PS2ALLNUMALLOCEDATOMICS * sizeof(RwFrame *));
    }

    /* Create and initialize the new atomics */
    {
        RpAtomic           *atomic;
        RwFrame            *frame;
        RwUInt32            i;

        for (i = 0; i < PS2ALLNUMALLOCEDATOMICS; i++)
        {
            atomic = RpAtomicCreate();
            RpAtomicSetGeometry(atomic, ps2AllGeom, 0);

            frame = RwFrameCreate();
            RpAtomicSetFrame(atomic, frame);

            PS2AllAtomics[i] = atomic;
            PS2AllFrames[i] = frame;

            PS2AllColors[i].red   = ((i >> 0) % 2) ? 1.0f : 0.0f;
            PS2AllColors[i].green = ((i >> 1) % 2) ? 1.0f : 0.0f;
            PS2AllColors[i].blue  = ((i >> 2) % 2) ? 1.0f : 0.0f;
            PS2AllColors[i].alpha = 1.0f;
        }
    }

    PS2AllPositionAtomics(PS2ALLNUMALLOCEDATOMICS);

    /* The number of drawn RpAtomics will be set to the maximum possible
     * at 60Hz for each configuration (until the user manually adjusts
     * the number of drawn RpAtomics) */
    PSAllAutoMaximize = TRUE;

    /* Make PS2Manager the default pipeline. This will set up state
     * (texturing, lighting, material colour, surface properties, vector
     * code, etc) in VU1 every frame, but the PS2All pipeline will assume
     * that it is all there already and thus save a lot of CPU overhead. */
    ps2AllState = STATEPS2ALL_G3;
    ps2AllStateCB(FALSE);

    return (TRUE);
}

/*
 *****************************************************************************
 */
