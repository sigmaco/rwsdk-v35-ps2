
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
 * Copyright (C) 2001 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: An introduction to PS2All and demonstration of how it
 *          can be used to maximize CPU-side rendering performance
 *
 ****************************************************************************/

#include "rwcore.h"
#include "rpworld.h"
#include "rppds.h"
#include "rtworld.h"

#include "skeleton.h"

#include "menu.h"
#include "ps2all.h"

/* The number of atomics to start up with */
#define PS2ALLNUMALLOCEDATOMICS (50*50)

/* We can toggle between PS2AllBasic and PS2AllOptimised render pipelines */
#define STATEPS2ALLBASIC 0
#define STATEPS2ALLOPTIMIZED 1
#define PS2ALLNUMSTATES 2

/* Settings for different program states */
static RwUInt32     ps2AllStates[PS2ALLNUMSTATES] = { 1300, 2500 };

/* The RpGeometry which we'll procedurally generate */
static RpGeometry  *ps2AllGeom = NULL;

/* The texture we'll be using */
static RwTexture   *ps2AllTexture = NULL;

/* An array of RpAtomics referencing the source RpGeometry */
static RpAtomic   **ps2AllAtomics = NULL;

/* Number of RpAtomics to display */
static RwInt32      ps2AllNumDrawnAtomics = 0;

/* Whether to auto-set ps2AllNumDrawnAtomics to the
 * maximum possible at 60Hz for each configuration */
static RwBool       ps2AllAutoMaximize = TRUE;

/* Cache global bounding box/sphere for the source RpClump for convenience */
static RwV3d        ps2AllAtomicBBox;

/* Our PS2All and PS2AllMat render pipelines */
static RxPipeline  *ps2AllPipeOptimized = NULL;
static RxPipeline  *ps2AllMatPipeOptimized = NULL;

/* What pipeline are we currently rendering with? */
static RwInt32      ps2AllState = STATEPS2ALLBASIC;

/* Used to optimize pipeline execution */
static RwBool       ps2AllFirstAtomic;

/* Used to differentiate between the menu and code in here accessing CBs */
static RwBool       menuCBAccess = TRUE;

#define AppPS2AllNumAtomicsCB(_justCheck)   \
MACRO_START                                 \
{                                           \
    menuCBAccess = FALSE;                   \
    PS2AllNumAtomicsCB(_justCheck);         \
    menuCBAccess = TRUE;                    \
}                                           \
MACRO_STOP

/* Useful values */
const RwV3d         XAxis = { 1, 0, 0 };
const RwV3d         YAxis = { 0, 1, 0 };
const RwV3d         ZAxis = { 0, 0, 1 };

/* Grab these from main.c */
extern RpWorld     *World;
extern RwCamera    *Camera;
extern RpLight     *DirectionalLight;

/*
 *****************************************************************************
 */
static RpClump *
ClumpLoad(const RwChar * name)
{
    RpClump            *clump = NULL;
    RwStream           *stream;
    RwChar             *path;

    path = RsPathnameCreate(name);
    stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, path);
    RsPathnameDestroy(path);

    if (stream)
    {
        if (RwStreamFindChunk(stream, rwID_CLUMP, NULL, NULL))
        {
            clump = RpClumpStreamRead(stream);
        }

        RwStreamClose(stream, NULL);
    }

    return (clump);
}

/*
 *****************************************************************************
 */

static RwBool
PS2AllNumAtomicsCB(RwBool justCheck)
{
    RwReal              count;
    /* Percentage of screen (vertically) to leave as an empty border around the atomics */
    RwReal              border = 0.05f;

    /* The angle by which the camera looks down from the horizontal (in degrees)
     * This actually varies to be less when there are only a few things on the screen */
    RwReal              elevation = 45;

    /* The side length of the grid of atomics */
    RwUInt32            side;

    /* Vectors used to position our camera */
    const RwV3d         zeroVec = { 0, 0, 0 };
    RwV3d               columnSpacing = { 0, 0, 0 };
    RwV3d               rowSpacing = { 0, 0, 0 };
    RwV3d               pos, curPos;
    RwFrame            *frame;
    RwReal              theta, sineTheta, cosTheta, distance, rTemp;

    if (FALSE != justCheck)
    {
        return (TRUE);
    }

    /* Did the menu wrap around? */
    if ( (((~(RwUInt32)0))>>1) == ps2AllNumDrawnAtomics)
    {
        /* Let's not actually wrap around, drawing ((1 << 31) - 1)
         * RpAtomics might take some time, even using PS2All! */
        ps2AllNumDrawnAtomics = 1;
        return (TRUE);
    }

    /* Alterations of ps2AllNumDrawnAtomics through
     * the menu should turn auto-maximize off */
    if (FALSE != menuCBAccess)
    {
        ps2AllAutoMaximize = FALSE;
    }

    /* Atomic frames are set up in a square-ish grid */
    count = sqrt(ps2AllNumDrawnAtomics - 1);
    side = 1 + (RwUInt32) count;
    columnSpacing.x = 1.05f * ps2AllAtomicBBox.x;
    rowSpacing.z = 1.05f * ps2AllAtomicBBox.z;

    /* Put the camera in a sensible position
     * [this makes sure the sphere is inside the view frustum
     * with a percentage of empty border to spare] */
    frame = RwCameraGetFrame(Camera);

    /* The camera looks down from the horizontal at an angle of
     * 'elevation' degrees. Modify 'elevation' according to 'side',
     * so that low 'side' means low 'elevation' */
    elevation = elevation * (1.0f - (1.0f / side));
    RwFrameRotate(frame, &XAxis, elevation, rwCOMBINEREPLACE);

    /* X position is trivial, half-way along the grid */
    pos = columnSpacing;
    RwV3dScale(&pos, &pos, 0.5f * (side - 1));
    RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);

    /* Choose initial Z position outside the front of the
     * the grid by half the depth of the object's bounding box */
    pos = zeroVec;
    pos.z = -ps2AllAtomicBBox.z;
    RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);

    /* Choose initial Y position such that the bottom of the front-most
     * RpAtomics fits inside the screen with 'border' to spare */
    pos = zeroVec;
    /* Theta is the angle of the Camera's lower clip-plane
     * (adjusted by border), down from horizontal */
    theta = RwATan(RwCameraGetViewWindow(Camera)->y * (1.0f - border));
    elevation *= (2.0f * rwPI / 360.0f); /* Into radians */
    theta += elevation;
    pos.y = (0.5f * ps2AllAtomicBBox.z) * RwTan(theta);
    pos.y -= (0.5f * ps2AllAtomicBBox.y);
    RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);
    curPos = *(RwMatrixGetPos(RwFrameGetLTM(frame)));

    /* Now move the camera back along its lower view plane (constant X
     * and screen-space position of the bottom of the front-most RpAtomics)
     * until the top of the rear RpAtomics fit inside the screen with 'border'
     * to spare */
    pos = zeroVec;
    pos.y = RwSin(theta);
    pos.z = -RwCos(theta);
    /* Theta is now the angle of the Camera's upper clip
     * plane (adjusted by border), down from horizontal */
    theta = elevation;
    theta -= RwATan(RwCameraGetViewWindow(Camera)->y * (1.0f - border));
    sineTheta = RwSin(theta);
    cosTheta = RwCos(theta);
    /* It looks complex but it's just a bit of trig and simultaneous equations */
    if (1 == side)
    {
        /* Since the camera is below the top of the grid, we fit the
         * *front*-top-middle edge on screen, not the rear-top-middle */
        rTemp = sineTheta * (-(0.5f * ps2AllAtomicBBox.z) - curPos.z);
    }
    else
    {
        rTemp =
            sineTheta * ((0.5f * ps2AllAtomicBBox.z) +
                         (side - 1) * rowSpacing.z - curPos.z);
    }
    rTemp -= cosTheta * (curPos.y - (0.5f * ps2AllAtomicBBox.y));
    rTemp /= (cosTheta * pos.y + sineTheta * pos.z);
    RwV3dScale(&pos, &pos, rTemp);
    RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);

    /* Finally, take into account the view frustum's aspect ratio.
     * If the grid is too wide for the screen, move the camera further
     * back along its view direction */

    /* Half the width of the grid */
    rTemp = 0.5f * (ps2AllAtomicBBox.x + (side - 1) * columnSpacing.x);
    /* Calculate the distance to the top-front-middle of the grid */
    pos.x = 0.5f * (side - 1) * columnSpacing.x;
    pos.y = 0.5f * ps2AllAtomicBBox.y;
    pos.z = -0.5f * ps2AllAtomicBBox.z;
    RwV3dSub(&pos, &pos, RwMatrixGetPos(RwFrameGetLTM(frame)));
    distance =
        RwV3dDotProduct(&pos, RwMatrixGetAt(RwFrameGetLTM(frame)));
    /* Put in rTemp the x-size of the projection of half the
     * grid onto the viewscreen. Adjust it if it's too big */
    rTemp /= distance;
    if (rTemp > (1.0f - border) * (RwCameraGetViewWindow(Camera))->x)
    {
        rTemp /= (1.0f - border) * (RwCameraGetViewWindow(Camera))->x;
        distance *= (rTemp - 1.0f);
        pos = *(RwMatrixGetAt(RwFrameGetLTM(frame)));
        RwV3dScale(&pos, &pos, -distance);
        RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);
    }

    /* Adjust the near/far clip planes for optimum z-buffer usage */

    /* Distance to the top-front-middle of the grid */
    pos.x = 0.5f * (side - 1) * columnSpacing.x;
    pos.y = 0.5f * ps2AllAtomicBBox.y;
    pos.z = -0.5f * ps2AllAtomicBBox.z;

    RwV3dSub(&pos, &pos, RwMatrixGetPos(RwFrameGetLTM(frame)));
    rTemp = RwV3dDotProduct(&pos, RwMatrixGetAt(RwFrameGetLTM(frame)));
    /* Avoid a warning */
    if (rTemp < 0.01f)
        rTemp = 0.01f;
    if (rTemp > RwCameraGetFarClipPlane(Camera))
    {
        RwCameraSetFarClipPlane(Camera, rTemp + 1);
    }
    RwCameraSetNearClipPlane(Camera, rTemp);

    /* Distance to the bottom-rear-middle of the grid */
    pos.y = -0.5f * ps2AllAtomicBBox.y;
    pos.z = 0.5f * ps2AllAtomicBBox.z + (side - 1) * rowSpacing.z;

    RwV3dSub(&pos, &pos, RwMatrixGetPos(RwFrameGetLTM(frame)));
    rTemp = RwV3dDotProduct(&pos, RwMatrixGetAt(RwFrameGetLTM(frame)));
    if (rTemp <= RwCameraGetNearClipPlane(Camera))
    {
        rTemp = 2.0f*RwCameraGetNearClipPlane(Camera);
    }
    RwCameraSetFarClipPlane(Camera, rTemp);

    return (TRUE);
}

/*
 *****************************************************************************
 */

static RwBool
ps2AllStateCB(RwBool justCheck)
{
    if (FALSE != justCheck)
    {
        return (TRUE);
    }

    if (ps2AllState == STATEPS2ALLBASIC)
    {
        /* Set the default (unoptimized) RenderWare PS2All pipeline */
        RpAtomicSetDefaultPipeline(RpAtomicSkyGetPS2AllPipeline());
    }
    else if (ps2AllState == STATEPS2ALLOPTIMIZED)
    {
        /* Only toggle the atomic pipeline - we only create one
         * PS2AllMat RpMaterial pipeline, so our PS2All RpAtomic
         * pipeline is set up to use that one exclusively, ignoring
         * the pipes found in the actual RpMaterials of objects. */
        RpAtomicSetDefaultPipeline(ps2AllPipeOptimized);
    }

    /* Set the number of atomics drawn to the maximum
     * possible at 60Hz for this configuration, UNLESS
     * the user is manually adjusting the number */
    if (FALSE != ps2AllAutoMaximize)
    {
        ps2AllNumDrawnAtomics = ps2AllStates[ps2AllState];
    }

    /* Move the camera into position */
    AppPS2AllNumAtomicsCB(FALSE);

    /* When swapping pipelines, make sure all data is reinstanced */
    ps2AllGeom->lockedSinceLastInst = rpGEOMETRYLOCKALL;

    return (TRUE);
}

/*
 *****************************************************************************
 */

void
PS2AllRender(void)
{
    RwInt32             i;

    ps2AllFirstAtomic = TRUE;
    for (i = 0; i < ps2AllNumDrawnAtomics; i++)
    {
        RpAtomicRender(ps2AllAtomics[i]);
        ps2AllFirstAtomic = FALSE;
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
    ps2AllAutoMaximize = TRUE;
    ps2AllNumDrawnAtomics = ps2AllStates[ps2AllState];
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
    static RwChar numAtomicsLabel[] =
        RWSTRING("Number of atomics");
    static RwChar numAtomics25Label[] = RWSTRING("Number [x25]");
    static RwChar pipeTypeLabel[] = RWSTRING("Render pipeline");
    static RwChar name1[] = RWSTRING("PS2All");
    static RwChar name2[] = RWSTRING("PS2All Opt");
    static RwChar *pipeTypeNames[PS2ALLNUMSTATES] =
        { name1, name2 };
    static RwChar maxLabel[] = RWSTRING("Maximize");

    MenuAddSeparator();

    /* Add an entry to allow changing the number of atomics drawn */
    MenuAddEntryInt(numAtomicsLabel, /* "Number of atomics" */
                    &ps2AllNumDrawnAtomics, /* The value variable */
                    PS2AllNumAtomicsCB, /* CallBack for value change */
                    1,         /* Minimum value */
                    PS2ALLNUMALLOCEDATOMICS, /* Maximum value */
                    1,         /* Step size for value change */
                    NULL);     /* No names for particular values */

    /* Add an entry to allow changing the number of atomics drawn */
    MenuAddEntryInt(numAtomics25Label, /* "Number of atomics" */
                    &ps2AllNumDrawnAtomics, /* The value variable */
                    PS2AllNumAtomicsCB, /* CallBack for value change */
                    1,         /* Minimum value */
                    PS2ALLNUMALLOCEDATOMICS, /* Maximum value */
                    25,        /* Step size for value change */
                    NULL);     /* No names for particular values */

    /* Add an entry to allow changing between PS2All and PS2AllOptimized render pipelines */
    MenuAddEntryInt(pipeTypeLabel, /* "Render pipeline" */
                    &ps2AllState, /* The value variable */
                    ps2AllStateCB, /* CallBack for value change */
                    STATEPS2ALLBASIC, /* Minimum value */
                    STATEPS2ALLOPTIMIZED, /* Maximum value */
                    1,         /* Step size for value change */
                    (const char **) pipeTypeNames); /* {"PS2All", "PS2All Opt"} */

    /* Add an entry to automatically jump to the known max. number of renderable
     * atomics for a given configuration */
    MenuAddEntryTrigger(maxLabel, /* "Maximize" */
                        PS2AllMaximizeCB); /* CallBack */

    return;
}

/*
 *****************************************************************************
 */

/* This version only draws one half of the Moebius strip */
#define PS2AllEval(_ps, _lT, _xT)                                           \
MACRO_START                                                                 \
{                                                                           \
    RwReal _tmp  = 0.5f*(outer + inner);                                    \
    RwReal _tmp2 = 2*rwPI*_lT*numTurns;                                     \
    RwReal _tmp3;                                                           \
    _ps.x = cos(2*rwPI*_lT);                                                \
    _ps.z = sin(2*rwPI*_lT);                                                \
    _tmp3 = (_xT - 0.5f)*(outer - inner)*cos(_tmp2);                        \
    _ps.x = _tmp*_ps.x + _ps.x*_tmp3;                                       \
    _ps.z = _tmp*_ps.z + _ps.z*_tmp3;                                       \
    _ps.y = (_xT - 0.5f)*(outer - inner)*sin(_tmp2);                        \
}                                                                           \
MACRO_STOP

static RpGeometry *
PS2AllGeometryCreate(RwUInt32 length, RwUInt32 width, RwUInt32 numMats)
{
    RwUInt32            numVerts, numTriangles;
    RpMaterial        **mats;
    RpGeometry         *geom;
    RwUInt32            i;

    /* Create numMats materials for this RpGeometry */
    mats = (RpMaterial **) RwMalloc(numMats * sizeof(RpMaterial *));
    if (NULL == mats) return ((RpGeometry *) NULL);

    for (i = 0; i < numMats; i++)
    {
        mats[i] = RpMaterialCreate();
        if (NULL == mats[i])
        {
            for (i = (i - 1); i > 0; i--)
            {
                RpMaterialDestroy(mats[i]);
            }
            RwFree(mats);
            return ((RpGeometry *) NULL);
        }
        RpMaterialSetTexture(mats[i], ps2AllTexture);
    }

    numVerts = length * width;
    numTriangles = 2 * (length - 1) * (width - 1);

    geom = RpGeometryCreate(numVerts, numTriangles,
                            rpGEOMETRYTRISTRIP | rpGEOMETRYTEXTURED |
                            rpGEOMETRYPRELIT | rpGEOMETRYNORMALS |
                            rpGEOMETRYLIGHT);

    if (NULL != RpGeometryLock(geom, rpGEOMETRYLOCKALL))
    {
        RpMorphTarget      *target;
        RwV3d              *verts;
        RpTriangle         *triangles;
        RwTexCoords        *UVs;
        RwRGBA             *RGBAs;

        triangles = RpGeometryGetTriangles(geom);
        target = RpGeometryGetMorphTarget(geom, 0);
        verts = RpMorphTargetGetVertices(target);
        UVs =
            RpGeometryGetVertexTexCoords(geom,
                                         rwTEXTURECOORDINATEINDEX0);
        RGBAs = RpGeometryGetPreLightColors(geom);
        if ((NULL != triangles) && (NULL != verts) &&
            (NULL != UVs) && (NULL != RGBAs))
        {
            const RwReal        numTurns = 1.5f;
            const RwReal        inner = 5.0f;
            const RwReal        outer = 15.0f;

            RwRGBAReal          realRGBA = { 0, 0, 0, 1.0 };
            RwV3d               colorCentre = { 0.25f, 0.25f, 0.25f };
            RwV3d               pos = { 0, 0, 0 };
            RwV3d               delta;
            RwReal              lTemp, xTemp, lStep, xStep;
            RwUInt32            l, x;


            /* Generate vertex positions and colours */
            lStep = 1.0f / (length - 1);
            xStep = 1.0f / (width - 1);
            lTemp = xTemp = 0;
            i = 0;
            for (l = 0; l < length; l++)
            {
                xTemp = 0;
                for (x = 0; x < width; x++)
                {
                    PS2AllEval(pos, lTemp, xTemp);
                    *verts = pos;
                    verts++;

                    PS2AllEval(delta, lTemp - lStep, xTemp - xStep);
                    RwV3dSub(&delta, &pos, &delta);
                    RwV3dNormalize(&delta, &delta);
                    RwV3dScale(&delta, &delta, 0.25f);
                    RwV3dAdd(&delta, &delta, &colorCentre);
                    realRGBA.red = delta.x;
                    realRGBA.green = delta.y;
                    realRGBA.blue = delta.z;
                    RwRGBAFromRwRGBAReal(RGBAs, &realRGBA);
                    RGBAs++;

                    xTemp += xStep;
                }
                lTemp += lStep;
            }

            /* Cache a bounding box for the geometry */
            ps2AllAtomicBBox.x = 2.0f * outer;
            ps2AllAtomicBBox.y = outer - inner;
            ps2AllAtomicBBox.z = 2.0f * outer;

            /* Index the front of the surface, tristrip-friendly order */
            i = 0;
            for (x = 0; x < (width - 1); x++)
            {
                for (l = 0; l < (length - 1); l++)
                {
                    RpGeometryTriangleSetVertexIndices((const RpGeometry
                                                        *) geom,
                                                       triangles,
                                                       x + l * width,
                                                       x + l * width +
                                                       1,
                                                       x + (l +
                                                            1) * width);
                    RpGeometryTriangleSetMaterial(geom, triangles,
                                                  mats[i]);
                    triangles++;
                    i++;

                    if (i >= numMats)
                        i = 0;

                    RpGeometryTriangleSetVertexIndices(geom,
                                                       triangles,
                                                       x + (l +
                                                            1) * width,
                                                       x + l * width +
                                                       1,
                                                       x + (l +
                                                            1) * width +
                                                       1);
                    RpGeometryTriangleSetMaterial(geom, triangles,
                                                  mats[i]);
                    triangles++;
                    i++;
                    if (i >= numMats)
                        i = 0;
                }
            }
            /* No need for rear faces on PS2, by default the transforms won't back-face cull */

            /* Generate tex coords, uniform mapping in (l, x),
             * as we have normalized inter-vertex distances */
            for (l = 0; l < length; l++)
            {
                for (x = 0; x < width; x++)
                {
                    UVs->u = x * xStep;
                    UVs->v = numTurns * l * lStep;
                    UVs++;
                }
            }

            if (NULL != RpGeometryUnlock(geom))
            {
                RwSphere            sphere;

                /* Generate normals (code from RtImport) */
                RtGeometryCalculateVertexNormals(geom);

                /* Generate a bounding sphere */
                target = RpGeometryGetMorphTarget(geom, 0);
                RpMorphTargetCalcBoundingSphere(target, &sphere);
                RpMorphTargetSetBoundingSphere(target, &sphere);

                /* Make sure the materials get destroyed with the geometry */
                for (i = 0; i < numMats; i++) RpMaterialDestroy(mats[i]);
                RwFree(mats);

                return (geom);
            }
        }
    }

    /* Exit on failure */
    for (i = 0; i < numMats; i++)
    {
        RpMaterialDestroy(mats[i]);
    }
    RwFree(mats);
    RpGeometryDestroy(geom);

    return ((RpGeometry *) NULL);
}

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
    if (FALSE == ps2AllFirstAtomic)
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
PS2AllBridgeCB(RxPS2AllPipeData * ps2AllPipeData)
{
    RwUInt32            numInitialQW = 0, numExtraQW = 0;

    if (FALSE == ps2AllFirstAtomic)
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
    numInitialQW = rpMESHPS2ALLGIFTAGNUMINITIALQW +
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

    /* Upload material color, dependent on whether there's a texture
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

/*
 *****************************************************************************
 */

static RwBool
PS2AllOptimizedPipelinesCreate(void)
{
    RxPipeline         *allMatPipe, *allPipe;

    /* We will use the default RenderWare PS2All pipeline as our 'basic'
     * (unoptimized) pipe (the default PS2AllMat pipe will be used by it
     * automatically, since our object's material has a NULL pipeline
     * pointer) */

    allMatPipe = RxPipelineCreate();
    if (allMatPipe)
    {
        RxNodeDefinition   *ps2AllMat;
        RxPipelineNode     *plNode, *result;
        RxLockedPipe       *lPipe;

        lPipe = RxPipelineLock(allMatPipe);

        ps2AllMat = RxNodeDefinitionGetPS2AllMat();

        /* The material pipeline is trivial, containing just the PS2AllMat node */
        lPipe = RxLockedPipeAddFragment(lPipe,
                                        (RwUInt32 *) NULL,
                                        ps2AllMat,
                                        (RxNodeDefinition *) NULL);

        plNode = RxPipelineFindNodeByName(lPipe,
                                          ps2AllMat->name,
                                          (RxPipelineNode *) NULL,
                                          (RwInt32 *) NULL);

        /* Get the PS2AllMat node to generate cluster "xyz" */
        result =
            RxPipelineNodePS2AllMatGenerateCluster(plNode, &RxClPS2xyz,
                                                   CL_XYZ);

        /* Get the PS2AllMat node to generate cluster "xyz" */
        result =
            RxPipelineNodePS2AllMatGenerateCluster(plNode, &RxClPS2uv,
                                                   CL_UV);

        /* Get the PS2AllMat node to generate cluster "xyz" */
        result =
            RxPipelineNodePS2AllMatGenerateCluster(plNode, &RxClPS2rgba,
                                                   CL_RGBA);

        /* Get the PS2AllMat node to generate cluster "xyz" */
        result =
            RxPipelineNodePS2AllMatGenerateCluster(plNode,
                                                   &RxClPS2normal,
                                                   CL_NORMAL);

        /* Set PS2AllMat VU1 buffer sizes */
        result = RxPipelineNodePS2AllMatSetTriangleVUBufferSizes(plNode,
                     _rwskyStrideOfInputCluster,
                     _rwskyTSVertexCount,
                     _rwskyTLTriCount);

        allMatPipe = RxLockedPipeUnlock(lPipe);

        if (allMatPipe != NULL)
        {
            plNode = RxPipelineFindNodeByName(lPipe,
                                              ps2AllMat->name,
                                              (RxPipelineNode *) NULL,
                                              (RwInt32 *) NULL);

            plNode = RxPipelineNodePS2AllMatSetVIFOffset(plNode,
                                                         _rwskyVIFOffset);

            /* Attach the VU1 code array. */
            result = RxPipelineNodePS2AllMatSetVU1CodeArray(
                         plNode,
                         rwPDS_G3_Generic_VU1Transforms,
                         rwPDS_G3_Generic_CodeArraySize );
            RWASSERT(NULL != result);

            /* Set up the necessary callbacks */

            /* We don't need to test for instancing meshes, we won't
             * be editing our data at run-time (this line isn't
             * actually necessary, it's included for clarity) */
            RxPipelineNodePS2AllMatSetCallBack(plNode, rxPS2ALLMATCALLBACKMESHINSTANCETEST, NULL); /* RpMeshPS2AllMeshInstanceTestCallBack); */
            /* The resEntryAlloc callback is called only once (during the
             * initial data instance) and so does not affect performance */
            RxPipelineNodePS2AllMatSetCallBack(plNode,
                                               rxPS2ALLMATCALLBACKRESENTRYALLOC,
                                               RpMeshPS2AllResEntryAllocCallBack);
            /* Likewise, the matInstance callback is called
             * only once and so does not affect performance. */
            RxPipelineNodePS2AllMatSetCallBack(plNode,
                                               rxPS2ALLMATCALLBACKINSTANCE,
                                               RpMeshPS2AllInstanceCallBack);
            /* We are replacing the default bridgeCB with an optimized
             * version (see above for its definition). */
            RxPipelineNodePS2AllMatSetCallBack(plNode, rxPS2ALLMATCALLBACKBRIDGE, PS2AllBridgeCB); /* RpMeshPS2AllBridgeCallBack); */
#if (defined(RWMETRICS))
            /* In a metrics build, include the default metrics-counting
             * callback (our pipeline modifications don't invalidate it). */
            RxPipelineNodePS2AllMatSetCallBack(plNode,
                                               rxPS2ALLMATCALLBACKPOSTMESH,
                                               RpMeshPS2AllPostMeshCallBack);
#endif /* (defined(RWMETRICS)) */

            ps2AllMatPipeOptimized = allMatPipe;
        }
    }
    else
    {
        return (FALSE);
    }

    allPipe = RxPipelineCreate();
    if (allPipe)
    {
        RxLockedPipe       *lPipe;

        lPipe = RxPipelineLock(allPipe);

        if (NULL != lPipe)
        {
            RxNodeDefinition   *ps2All = RxNodeDefinitionGetPS2All();
            RxPipelineNode     *plNode;

            /* The PS2All RpAtomic pipeline is trivial, containing just the PS2All node */
            lPipe = RxLockedPipeAddFragment(lPipe,
                                            (RwUInt32 *) NULL,
                                            ps2All,
                                            (RxNodeDefinition *) NULL);

            allPipe = RxLockedPipeUnlock(lPipe);

            if (allPipe != NULL)
            {
                plNode = RxPipelineFindNodeByName(allPipe,
                                                  ps2All->name,
                                                  (RxPipelineNode *)
                                                  NULL,
                                                  (RwInt32 *) NULL);

                /* Set up the necessary callbacks */

                /* We are replacing the default RpAtomic objectSetupCB with an
                 * optimized version (see above for its definition). */
                RxPipelineNodePS2AllSetCallBack(plNode, rxPS2ALLCALLBACKOBJECTSETUP, RPS2AllObjectSetupCB); /* RpAtomicPS2AllObjectSetupCallBack); */
                /* As with the default PS2All RpAtomic pipeline, there's no need
                 * for an objectFinalizeCallBack */
                RxPipelineNodePS2AllSetCallBack(plNode,
                                                rxPS2ALLCALLBACKOBJECTFINALIZE,
                                                NULL);

                /* We cause our optimized PS2AllMat pipeline to *always* be
                 * used by this pipeline (ignoring material pipeline pointers) */
                RxPipelineNodePS2AllGroupMeshes(plNode,
                                                ps2AllMatPipeOptimized);

                ps2AllPipeOptimized = allPipe;

                return (TRUE);
            }
            else
            {
                RxPipelineDestroy(lPipe);
            }
        }
        else
        {
            RxPipelineDestroy(allPipe);
        }
    }

    /* Clean up and exit on failure */
    PS2AllClose();
    return (FALSE);
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

    if (NULL != ps2AllAtomics)
    {
        /* Destroy all the RpAtomics we allocated */
        for (i = 0; i < PS2ALLNUMALLOCEDATOMICS; i++)
        {
            atomic = ps2AllAtomics[i];
            if (NULL != atomic)
            {
                frame = RpAtomicGetFrame(ps2AllAtomics[i]);
                RpAtomicSetFrame(ps2AllAtomics[i], (RwFrame *) NULL);
                RwFrameDestroy(frame);
                RpAtomicDestroy(ps2AllAtomics[i]);
            }
        }

        /* Destroy the array of RpAtomic pointers */
        RwFree(ps2AllAtomics);
        ps2AllAtomics = (RpAtomic **) NULL;
    }

    /* Destroy our generated RpGeometry */
    if (NULL != ps2AllGeom)
    {
        RpGeometryDestroy(ps2AllGeom);
        ps2AllGeom = NULL;
    }

    /* Destroy our texture */
    if (NULL != ps2AllTexture)
    {
        RwTextureDestroy(ps2AllTexture);
        ps2AllTexture = NULL;
    }

    /* Destroy our PS2All pipelines */
    if (NULL != ps2AllPipeOptimized)
    {
        RxPipelineDestroy(ps2AllPipeOptimized);
        ps2AllPipeOptimized = NULL;
    }
    if (NULL != ps2AllMatPipeOptimized)
    {
        RxPipelineDestroy(ps2AllMatPipeOptimized);
        ps2AllMatPipeOptimized = NULL;
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
    if (FALSE == result)
    {
        /* Clean up and exit on failure */
        PS2AllClose();
        return (FALSE);
    }

    /* Load a texture */
    path = RsPathnameCreate(RWSTRING("./textures/"));
    RwImageSetPath(path);
    RsPathnameDestroy(path);
    ps2AllTexture = RwTextureRead(RWSTRING("flagstone"), NULL);
    if (NULL == ps2AllTexture)
    {
        /* Clean up and exit on failure */
        PS2AllClose();
        return (FALSE);
    }

    /* Procedurally generate an RpGeometry
     * NOTE: because the PS2All optimized pipeline can render so many
     * RpAtomics per frame, we have to make this a very small geometry
     * in order that VU1 can render as many in a frame! */
    ps2AllGeom = PS2AllGeometryCreate(17, 2, 1);
    if (NULL == ps2AllGeom)
    {
        /* Clean up and exit on failure */
        PS2AllClose();
        return (FALSE);
    }

    /* Alloc the ps2AllAtomics array of RpAtomic pointers */
    {
        RpAtomic          **array;

        array =
            (RpAtomic **) RwMalloc(PS2ALLNUMALLOCEDATOMICS *
                                   sizeof(RpAtomic *));

        if (NULL == array)
        {
            /* Clean up and exit on failure */
            PS2AllClose();
            return (FALSE);
        }

        ps2AllAtomics = array;
        memset(ps2AllAtomics, 0,
               PS2ALLNUMALLOCEDATOMICS * sizeof(RpAtomic *));
    }

    /* Create and initialize the new atomics */
    {
        RpAtomic           *atomic;
        RwFrame            *frame;
        RwUInt32            i;

        for (i = 0; i < PS2ALLNUMALLOCEDATOMICS; i++)
        {
            atomic = RpAtomicCreate();
            if (NULL == atomic)
            {
                /* Clean up and exit on failure */
                PS2AllClose();
                return (FALSE);
            }
            RpAtomicSetGeometry(atomic, ps2AllGeom, 0);

            frame = RwFrameCreate();
            if (NULL == frame)
            {
                RpAtomicDestroy(atomic);
                /* Clean up and exit on failure */
                PS2AllClose();
                return (FALSE);
            }
            RpAtomicSetFrame(atomic, frame);

            ps2AllAtomics[i] = atomic;
        }
    }

    /* Place the atomics in a grid */
    {
        RwFrame            *frame;
        RwV3d               columnSpacing = { 0, 0, 0 };
        RwV3d               rowSpacing = { 0, 0, 0 };
        RwV3d               startPos = { 0, 0, 0 };
        RwV3d               pos, offset;
        RwUInt32            maxSide, side, x, y, i;
        RwReal              temp;

        /* In camera-space, the coordinates of this grid will appear:
         * X+ to the left, Y+ up, Z+ to the rear */

        columnSpacing.x = 1.05f * ps2AllAtomicBBox.x;
        rowSpacing.z = 1.05f * ps2AllAtomicBBox.z;

        rwSqrt(&temp, PS2ALLNUMALLOCEDATOMICS - 1);
        maxSide = 1 + (RwUInt32)temp;

        offset = RpAtomicGetBoundingSphere(ps2AllAtomics[0])->center;
        RwV3dNegate(&offset, &offset);
        i = 0;
        for (side = 1; side <= maxSide; side++)
        {
            pos = startPos;

            for (y = 0; y < (side - 1); y++)
            {
                if (i >= PS2ALLNUMALLOCEDATOMICS)
                    break;
                frame = RpAtomicGetFrame(ps2AllAtomics[i]);

                /* Center the geometry on its origin for (MUCH!) convenience */
                RwFrameTranslate(frame, &offset, rwCOMBINEREPLACE);
                /* Rotate the geometry to make the grid less dull-lookin' */
                RwFrameRotate(frame, &YAxis, i * 222,
                              rwCOMBINEPOSTCONCAT);

                /* Translate it to its rightful position in the Grid Of Life[tm] */
                RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);

                /* Move along the columns */
                RwV3dAdd(&pos, &pos, &rowSpacing);
                i++;
            }

            for (x = 0; x < side; x++)
            {
                if (i >= PS2ALLNUMALLOCEDATOMICS)
                    break;
                frame = RpAtomicGetFrame(ps2AllAtomics[i]);

                /* Center the geometry on its origin for (MUCH!) convenience */
                RwFrameTranslate(frame, &offset, rwCOMBINEREPLACE);
                /* Rotate the geometry to make the grid less dull-lookin' */
                RwFrameRotate(frame, &YAxis, i * 222,
                              rwCOMBINEPOSTCONCAT);

                /* Translate it to its rightful position in the Grid Of Life[tm] */
                RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);

                /* Move along the rows */
                RwV3dSub(&pos, &pos, &columnSpacing);
                i++;
            }
            RwV3dAdd(&startPos, &startPos, &columnSpacing);
        }
    }

    /* The number of drawn RpAtomics will be set to the maximum possible
     * at 60Hz for each configuration (until the user manually adjusts
     * the number of drawn RpAtomics) */
    ps2AllAutoMaximize = TRUE;

    /* Make PS2All the default pipeline. This will set up state
     * (texturing, lighting, material color, surface properties, vector
     * code, etc) in VU1 every frame, but the PS2AllOptimized pipeline will
     * assume that it is all there already and thus save a lot of CPU
     * overhead. */
    ps2AllState = STATEPS2ALLBASIC;
    ps2AllStateCB(FALSE);

    return (TRUE);
}

/*
 *****************************************************************************
 */
