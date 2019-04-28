
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
 * util.c
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

#include "ps2all.h"

/* Cache global bounding box/sphere for the source RpClump for convenience */
static RwV3d ps2AllAtomicBBox;

/* Useful values */
static const RwV3d XAxis = { 1, 0, 0 };
static const RwV3d YAxis = { 0, 1, 0 };
static const RwV3d ZAxis = { 0, 0, 1 };

/* The texture we'll be using */
RwTexture   *ps2AllTexture = NULL;

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

RpGeometry *
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

RwBool
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
    RwUInt32            i;

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
    if ( (((~(RwUInt32)0))>>1) == PS2AllNumDrawnAtomics)
    {
        /* Let's not actually wrap around, drawing ((1 << 31) - 1)
         * RpAtomics might take some time, even using PS2All! */
        PS2AllNumDrawnAtomics = 1;
        return (TRUE);
    }

    /* Alterations of ps2AllNumDrawnAtomics through
     * the menu should turn auto-maximize off */
    if (FALSE != PS2AllMenuCBAccess)
    {
        PSAllAutoMaximize = FALSE;
    }

    /* Atomic frames are set up in a square-ish grid */
    count = sqrt(PS2AllNumDrawnAtomics - 1);
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
    /* Theta is the angle of the Camera's lower clip plane
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

    /* Adjust the near/far clip planes for optimum ZBuffer usage */

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

    for( i = 0; i < (RwUInt32)PS2AllNumDrawnAtomics; i++ )
    {
        if(!RpAtomicGetWorld(PS2AllAtomics[i]))
        {
            RpWorldAddAtomic(World, PS2AllAtomics[i]);
        }
    }

    return (TRUE);
}

/*
 *****************************************************************************
 */

void
PS2AllPositionAtomics(RwInt32 maxNum)
{
    /* Place the atomics in a grid */
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

    rwSqrt(&temp, maxNum - 1);
    maxSide = 1 + (RwUInt32)temp;

    offset = RpAtomicGetBoundingSphere(PS2AllAtomics[0])->center;
    RwV3dNegate(&offset, &offset);
    i = 0;
    for (side = 1; side <= maxSide; side++)
    {
        pos = startPos;

        for (y = 0; y < (side - 1); y++)
        {
            if (i >= (RwUInt32)maxNum)
                break;
            frame = RpAtomicGetFrame(PS2AllAtomics[i]);

            /* Centre the geometry on its origin for (MUCH!) convenience */
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
            if (i >= (RwUInt32)maxNum)
                break;
            frame = RpAtomicGetFrame(PS2AllAtomics[i]);

            /* Centre the geometry on its origin for (MUCH!) convenience */
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

    return;
}
    
/*
 *****************************************************************************
 */
