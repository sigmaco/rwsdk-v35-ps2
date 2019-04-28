
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
 * ppvu2.c
 *
 * Copyright (C) 2001 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: Illustratesthe possibility of creating a custom VU pipe
 *
 ****************************************************************************/

#include "rwcore.h"
#include "rpworld.h"

#include "skeleton.h"
#include "menu.h"

#include "ppvu2.h"
#include "ppsprite.h"

#define TWOPASSx
#define FIXEDSIZEx

#define SCALELIGHT (0.4f)
#define LIGHTRADIUS (10.0f)
#define LIGHTRADIUS2 (100.0f)

static RwMatrix *Im3DMeshMatrix = NULL;
static RwV3d *NormalsList = NULL;
static RwV3d *PositionsList = NULL;
RwInt32 Im3DMeshNumVertices = 0;

#if 1
    static RwRGBA SpecularColor = {255, 255, 255, 255};
    static RwRGBA DiffuseColor  = {16,   0,   16, 255};
    static RwRGBA AmbientColor  = { 00,   0,   0, 255};
#else
    static RwRGBA SpecularColor = {0, 255, 0, 255};
    static RwRGBA DiffuseColor  = {0, 255,   0, 255};
    static RwRGBA AmbientColor  = {0, 255,   0, 255};
#endif

static RwMatrix *Im3DLightMatrix = NULL;

static PPSprite *SpriteList = NULL;

RwInt32 LightMode = LIGHTDIRECTIONAL;
RwInt32 ShadeMode = SHADEGOURAUD;

static RwTexture *Im3DTexture = NULL;

RwInt32 NumSides = 60;
RwInt32 NumRings = 60;


/*
 *****************************************************************************
 */
void
Im3DMeshRotate(RwReal xAngle, RwReal yAngle)
{
    RwMatrix *cameraMatrix;
    RwV3d right, up, pos;

    cameraMatrix = RwFrameGetMatrix(RwCameraGetFrame(Camera));
    right = *RwMatrixGetRight(cameraMatrix);
    up = *RwMatrixGetUp(cameraMatrix);

    pos = *RwMatrixGetPos(Im3DMeshMatrix);

    /*
     * First translate back to the origin...
     */
    RwV3dScale(&pos, &pos, -1.0f);
    RwMatrixTranslate(Im3DMeshMatrix, &pos, rwCOMBINEPOSTCONCAT);

    /*
     * ...do the rotations...
     */
    RwMatrixRotate(Im3DMeshMatrix, &up, xAngle, rwCOMBINEPOSTCONCAT);
    RwMatrixRotate(Im3DMeshMatrix, &right, yAngle, rwCOMBINEPOSTCONCAT);

    /*
     * ...and translate back...
     */
    RwV3dScale(&pos, &pos, -1.0f);
    RwMatrixTranslate(Im3DMeshMatrix, &pos, rwCOMBINEPOSTCONCAT);

    return;
}


/*
 ***************************************************************************
 */
void
Im3DMeshTranslateZ(RwReal zDelta)
{
    RwFrame *cameraFrame;
    RwV3d delta;

    cameraFrame = RwCameraGetFrame(Camera);

    RwV3dScale(&delta, RwMatrixGetAt(RwFrameGetMatrix(cameraFrame)), zDelta);

    RwMatrixTranslate(Im3DMeshMatrix, &delta, rwCOMBINEPOSTCONCAT);

    return;
}


/*
 *****************************************************************************
 */
void
Im3DLightRotate(RwReal xAngle, RwReal yAngle)
{
    RwMatrix *cameraMatrix;
    RwV3d right, up;

    cameraMatrix = RwFrameGetMatrix(RwCameraGetFrame(Camera));
    right = *RwMatrixGetRight(cameraMatrix);
    up = *RwMatrixGetUp(cameraMatrix);

    /*
     * Do the rotations...
     */
    RwMatrixRotate(Im3DLightMatrix, &up, xAngle, rwCOMBINEPOSTCONCAT);
    RwMatrixRotate(Im3DLightMatrix, &right, yAngle, rwCOMBINEPOSTCONCAT);

    return;
}


/*
 ***************************************************************************
 */
void
Im3DLightTranslateZ(RwReal zDelta)
{
    RwFrame *cameraFrame;
    RwV3d delta;

    cameraFrame = RwCameraGetFrame(Camera);

    RwV3dScale(&delta, RwMatrixGetAt(RwFrameGetMatrix(cameraFrame)), zDelta);
    RwMatrixTranslate(Im3DLightMatrix, &delta, rwCOMBINEPOSTCONCAT);

    return;
}


/*
 ***************************************************************************
 */
void
Im3DLightTranslateXY(RwReal xDelta, RwReal yDelta)
{
    RwFrame *cameraFrame;
    RwV3d delta;

    cameraFrame = RwCameraGetFrame(Camera);

    RwV3dScale(&delta, RwMatrixGetRight(RwFrameGetMatrix(cameraFrame)), xDelta);
    RwMatrixTranslate(Im3DLightMatrix, &delta, rwCOMBINEPOSTCONCAT);

    RwV3dScale(&delta, RwMatrixGetUp(RwFrameGetMatrix(cameraFrame)), yDelta);
    RwMatrixTranslate(Im3DLightMatrix, &delta, rwCOMBINEPOSTCONCAT);

    return;
}


/*
 *****************************************************************************
 */
static void
Im3DMeshLighting(void)
{
    RwV3d lightDir, lightPos;
    RwMatrix *transform = NULL;
    RwInt32 i;

    if( LightMode != LIGHTAMBIENT )
    {
        transform = RwMatrixCreate();
        RwMatrixInvert(transform, Im3DMeshMatrix);

        if( LightMode == LIGHTDIRECTIONAL )
        {
            /*
             * Transform the light's direction to the mesh's local space...
             */
            lightDir = *RwMatrixGetAt(Im3DLightMatrix);
            RwV3dTransformVectors(&lightDir, &lightDir, 1, transform);
        }
        else
        {
            /*
             * Transform the light's position to the mesh's local space...
             */
            lightPos = *RwMatrixGetPos(Im3DLightMatrix);
            RwV3dTransformPoints(&lightPos, &lightPos, 1, transform);
        }

        RwMatrixDestroy(transform);
    }

    for(i=0; i<Im3DMeshNumVertices; i++)
    {
        RwReal lighting = 0.0f, c1, c2;

        switch( LightMode )
        {
            case LIGHTDIRECTIONAL:
            {
                /*
                 * The lighting factor is the dot product of the vertex
                 * normals and the light direction vector if the normal
                 * is facing the light (dot prod = -1.0f)...
                 */
                lighting = -RwV3dDotProduct(&lightDir, &NormalsList[i]);

                break;
            }

            case LIGHTPOINT:
            {
                /*
                 * The lighting factor is the distance
                 * between the light and the vertex
                 * scaled from [0.0f...Distance] to [-1.0f...1.0f]
                 */
                RwV3d range;
                RwReal distance;

                RwV3dSub(&range, &PositionsList[i], &lightPos);

                distance = RwV3dLength(&range);

                if( distance > LIGHTRADIUS )
                {
                    distance = LIGHTRADIUS;
                }

                lighting = 1.0f - 2.0f * distance / LIGHTRADIUS;

                break;
            }

            case LIGHTAMBIENT:
            {
                lighting = -1.0f;

                break;
            }
        }

        /*
         * Set vertex color using the lighting as a blend factor between
         * diffuse color and specular color if lighting is positive or
         * diffuse color and ambient color...
         */
        c1 = RwRealAbs(lighting);
        c2 = 1.0f - c1;

        if( lighting > 0.0f )
        {
            SpriteList[i].color.red = (RwUInt8)(c1 * SpecularColor.red   + c2 * DiffuseColor.red);
            SpriteList[i].color.green = (RwUInt8)(c1 * SpecularColor.green + c2 * DiffuseColor.green);
            SpriteList[i].color.blue = (RwUInt8)(c1 * SpecularColor.blue  + c2 * DiffuseColor.blue);
        }
        else
        {
            SpriteList[i].color.red = (RwUInt8)(c1 * AmbientColor.red   + c2 * DiffuseColor.red);
            SpriteList[i].color.green = (RwUInt8)(c1 * AmbientColor.green + c2 * DiffuseColor.green);
            SpriteList[i].color.blue = (RwUInt8)(c1 * AmbientColor.blue  + c2 * DiffuseColor.blue);
        }

        RwV3dTransformPoints(&SpriteList[i].pos, &PositionsList[i], 1, Im3DMeshMatrix);

#ifndef FIXEDSIZE
        SpriteList[i].size.x = 1000.0f*(lighting+1.0f);
        SpriteList[i].size.y = 500.0f*(lighting+1.0f);
#endif
    }


    return;
}


/*
 *****************************************************************************
 */
void
Im3DRender(void)
{
    static RwBool stateSet = FALSE;

    /*
     * Calculate the current vertex colors due to lighting...
     */
    Im3DMeshLighting();

    /*
     * Render the sprites...
     */
    if( FALSE == stateSet)
    {
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)TRUE);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)TRUE);

        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)TRUE);
        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER , (void *)rwFILTERLINEAR );
        stateSet = TRUE;
    }

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)RwTextureGetRaster(Im3DTexture));
#ifdef TWOPASS
    PPSPriteDraw(SpriteList, Im3DMeshNumVertices/2);
    PPSPriteDraw(&SpriteList[Im3DMeshNumVertices/2], Im3DMeshNumVertices/2);
#else
    PPSPriteDraw(SpriteList, Im3DMeshNumVertices);
#endif

    return;
}


/*
 *****************************************************************************
 */
void
Im3DTerminate(void)
{
    if( NormalsList )
    {
        RwFree(NormalsList);

        NormalsList = NULL;
    }

    if( PositionsList )
    {
        RwFree(PositionsList);

        PositionsList = NULL;
    }

    if( SpriteList )
    {
        RwFree(SpriteList);

        SpriteList = NULL;
    }

    if( Im3DMeshMatrix )
    {
        RwMatrixDestroy(Im3DMeshMatrix);

        Im3DMeshMatrix = NULL;
    }

    if( Im3DLightMatrix )
    {
        RwMatrixDestroy(Im3DLightMatrix);

        Im3DLightMatrix = NULL;
    }

    return;
}


/*
 *****************************************************************************
 */
void
Im3DLightReset(void)
{
    switch( LightMode )
    {
        case LIGHTDIRECTIONAL:
        {
            RwMatrixSetIdentity(Im3DLightMatrix);

            Im3DLightRotate(30.0f, 60.0f);

            break;
        }

        case LIGHTPOINT:
        {
            RwV3d lightPos = {0.0f, 0.0f, 40.0f};

            RwMatrixTranslate(Im3DLightMatrix, &lightPos, rwCOMBINEREPLACE);

            break;
        }

        case LIGHTAMBIENT:
        default:
        {
            RwMatrixSetIdentity(Im3DLightMatrix);

            break;
        }
    }


    return;
}


/*
 *****************************************************************************
 */
static RwBool
Im3DMeshCreate(void)
{
    int i, j;
    RwReal r = 4.0f, R = 7.0f;
    RwReal ringDelta, sideDelta;

    RwV3d        *nrmList;
    RwV3d        *posList;
    PPSprite     *sprList;
    RwInt32 numVert;
    RwReal theta, phi, theta1;
    RwReal cosTheta, sinTheta;
    RwReal cosTheta1, sinTheta1;
    RwReal cosPhi, sinPhi, dist;

    numVert = (NumSides+1) * NumRings ;

    NormalsList = (RwV3d *)RwMalloc(numVert * sizeof(RwV3d));
    if( NormalsList == NULL )
    {
        return FALSE;
    }
    nrmList = NormalsList;

    PositionsList = (RwV3d *)RwMalloc(numVert * sizeof(RwV3d));
    if( PositionsList == NULL )
    {
        return FALSE;
    }
    posList = PositionsList;

    SpriteList = (PPSprite *)RwMalloc(numVert * sizeof(PPSprite));
    if( SpriteList == NULL )
    {
        return FALSE;
    }
    sprList = SpriteList;

    ringDelta = 2.0f * rwPI / NumRings;
    sideDelta = 2.0f * rwPI / NumSides;

    theta = 0.0f;
    cosTheta = 1.0f;
    sinTheta = 0.0f;

    for(i=NumRings-1; i>=0; i--)
    {
        theta1 = theta + ringDelta;
        cosTheta1 = (RwReal)RwCos(theta1);
        sinTheta1 = (RwReal)RwSin(theta1);

        phi = 0.0f;

        for(j=NumSides; j>=0; j--)
        {
            phi += sideDelta;
            cosPhi = (RwReal)RwCos(phi);
            sinPhi = (RwReal)RwSin(phi);
            dist = R + r * cosPhi;

            posList->x = cosTheta1 * dist;
            posList->y = -sinTheta1 * dist;
            posList->z = r * sinPhi;

            nrmList->x = cosTheta1 * cosPhi;
            nrmList->y = -sinTheta1 * cosPhi;
            nrmList->z = sinPhi;

            sprList->uv1.x = 0.0f;
            sprList->uv1.y = 0.0f;

            sprList->uv2.x = 1.0f;
            sprList->uv2.y = 1.0f;

            sprList->color.red = 255;
            sprList->color.green = 255;
            sprList->color.blue = 255;
            sprList->color.alpha = 64;

#ifdef FIXEDSIZE
            sprList->size.x = 100.0f;
            sprList->size.y = 50.0f;
#endif

            posList++;
            nrmList++;
            sprList++;

        }

        theta = theta1;
        cosTheta = cosTheta1;
        sinTheta = sinTheta1;
    }

    Im3DMeshNumVertices = numVert;

    return TRUE;
}

/*
 *****************************************************************************
 */
RwBool
Im3DInitialize(void)
{
    RwChar *path;

    /*
     * Create the matrix for the IM geometry's transformation....
     */
    Im3DMeshMatrix = RwMatrixCreate();
    if( Im3DMeshMatrix == NULL )
    {
        return FALSE;
    }
    else
    {
        /*
         * Initialize the matrix so that it positions the IM geometry
         * in front of the camera...
         */
        RwMatrix *cameraMatrix;
        RwV3d  *at, pos;

        cameraMatrix = RwFrameGetMatrix(RwCameraGetFrame(Camera));
        at = RwMatrixGetAt(cameraMatrix);

        RwV3dScale(&pos, at, 60.0f);
        RwV3dAdd(&pos, &pos, RwMatrixGetAt(cameraMatrix));

        RwMatrixTranslate(Im3DMeshMatrix, &pos, rwCOMBINEREPLACE);
    }

    /*
     * Create the matrix for the light's transformation....
     */
    Im3DLightMatrix = RwMatrixCreate();

    if( Im3DLightMatrix == NULL )
    {
        return FALSE;
    }

    Im3DLightReset();

    /*
     * Start with a torus...
     */
    Im3DMeshCreate();

    /*
     * Load the texture...
     */
    path = RsPathnameCreate(RWSTRING("./textures/"));
    RwImageSetPath(path);
    RsPathnameDestroy(path);

    Im3DTexture = RwTextureRead(RWSTRING("sprite"), NULL);

    if( Im3DTexture == NULL )
    {
        return FALSE;
    }

    RwTextureSetFilterMode(Im3DTexture, rwFILTERLINEAR  );


    return TRUE;
}

/*
 *************************************************************************
 */
