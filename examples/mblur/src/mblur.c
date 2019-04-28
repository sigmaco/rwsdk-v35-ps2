
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
 * mblur.c
 *
 * Copyright (C) 2001 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *                                                                         
 * Purpose: To illustrate a motion-blur technique on PS2.
 *
 ****************************************************************************/

#include "rwcore.h"
#include "rpworld.h"

#include "mblur.h"

static RwRaster *FrontBuffer = NULL;

static RwIm2DVertex Vertex[4];
static RwImVertexIndex Index[6] = {0, 1, 2, 0, 2, 3};



/*
 *****************************************************************************
 */
static void
CreateImmediateModeData(RwCamera *camera, RwRect *rect)
{
    RwReal w = rect->w;
    RwReal h = rect->h;
    RwReal U, V, u, v;
    RwUInt32 xSize, ySize;

    /* Grab the raster size (camera textures get rounded up to
     * a power of two in size) so we can calc UVs correctly: */
    xSize = RwRasterGetWidth(FrontBuffer);
    ySize = RwRasterGetHeight(FrontBuffer);

    U = (w + 0.5f) / xSize;
    V = (h + 0.5f) / ySize;
    u =     (0.5f) / xSize;
    v =     (0.5f) / ySize;

    /* Set up the vertices... */
    RwIm2DVertexSetScreenX(&Vertex[0], 0.0f);
    RwIm2DVertexSetScreenY(&Vertex[0], 0.0f);
    RwIm2DVertexSetScreenZ(&Vertex[0], RwIm2DGetNearScreenZ());
    RwIm2DVertexSetCameraZ(&Vertex[0], RwCameraGetNearClipPlane(camera));
    RwIm2DVertexSetRecipCameraZ(&Vertex[0], 1.0f/RwCameraGetNearClipPlane(camera));
    RwIm2DVertexSetU(&Vertex[0], u, 1.0f);
    RwIm2DVertexSetV(&Vertex[0], v, 1.0f);
    RwIm2DVertexSetIntRGBA(&Vertex[0], 255, 255, 255, 255);
    
    RwIm2DVertexSetScreenX(&Vertex[1], 0.0f);
    RwIm2DVertexSetScreenY(&Vertex[1], (RwReal)RwCameraGetRaster(camera)->height);
    RwIm2DVertexSetScreenZ(&Vertex[1], RwIm2DGetNearScreenZ());
    RwIm2DVertexSetCameraZ(&Vertex[1], RwCameraGetNearClipPlane(camera));
    RwIm2DVertexSetRecipCameraZ(&Vertex[1], 1.0f/RwCameraGetNearClipPlane(camera));
    RwIm2DVertexSetU(&Vertex[1], u, 1.0f);
    RwIm2DVertexSetV(&Vertex[1], V, 1.0f);
    RwIm2DVertexSetIntRGBA(&Vertex[1], 255, 255, 255, 255);
    
    RwIm2DVertexSetScreenX(&Vertex[2], (RwReal)RwCameraGetRaster(camera)->width);
    RwIm2DVertexSetScreenY(&Vertex[2], (RwReal)RwCameraGetRaster(camera)->height);
    RwIm2DVertexSetScreenZ(&Vertex[2], RwIm2DGetNearScreenZ());
    RwIm2DVertexSetCameraZ(&Vertex[2], RwCameraGetNearClipPlane(camera));
    RwIm2DVertexSetRecipCameraZ(&Vertex[2], 1.0f/RwCameraGetNearClipPlane(camera));
    RwIm2DVertexSetU(&Vertex[2], U, 1.0f);
    RwIm2DVertexSetV(&Vertex[2], V, 1.0f);
    RwIm2DVertexSetIntRGBA(&Vertex[2], 255, 255, 255, 255);
    
    RwIm2DVertexSetScreenX(&Vertex[3], (RwReal)RwCameraGetRaster(camera)->width);
    RwIm2DVertexSetScreenY(&Vertex[3], 0.0f);
    RwIm2DVertexSetScreenZ(&Vertex[3], RwIm2DGetNearScreenZ());
    RwIm2DVertexSetCameraZ(&Vertex[3], RwCameraGetNearClipPlane(camera));
    RwIm2DVertexSetRecipCameraZ(&Vertex[3], 1.0f/RwCameraGetNearClipPlane(camera));
    RwIm2DVertexSetU(&Vertex[3], U, 1.0f);
    RwIm2DVertexSetV(&Vertex[3], v, 1.0f);
    RwIm2DVertexSetIntRGBA(&Vertex[3], 255, 255, 255, 255);

    return;
}


/*
 *****************************************************************************
 */
static void
OverlayRender(RwCamera *camera __RWUNUSED__, 
              RwRaster *ras, 
              RwRGBA col)
{
    /* 
     * Set up the vertices...
     */
    RwIm2DVertexSetIntRGBA(&Vertex[0], 
                           col.red, col.green, col.blue, col.alpha);
    RwIm2DVertexSetIntRGBA(&Vertex[1], 
                           col.red, col.green, col.blue, col.alpha);
    RwIm2DVertexSetIntRGBA(&Vertex[2], 
                           col.red, col.green, col.blue, col.alpha);
    RwIm2DVertexSetIntRGBA(&Vertex[3], 
                           col.red, col.green, col.blue, col.alpha);
    
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)rwFILTERNEAREST);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void *)FALSE);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)FALSE);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)FALSE);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)ras);    
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)TRUE);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDINVSRCALPHA);

    RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, Vertex, 4, Index, 6);

    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void *)FALSE);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)TRUE);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)TRUE);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);    
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)FALSE);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDINVSRCALPHA);

    return;
}


/*
 *****************************************************************************
 */
void
MotionBlurRender(RwCamera *camera, RwUInt32 alpha)
{
    if( FrontBuffer )
    {
        RwRGBA color = {255, 255, 255, alpha};
        
        OverlayRender(camera, FrontBuffer, color);
    }

    return;
}


/*
 *****************************************************************************
 */
RwBool
MotionBlurOpen(RwCamera *camera)
{
    RwRect rect = {0, 0, 0, 0};
    
    rect.w = RwRasterGetWidth(RwCameraGetRaster(camera));
    rect.h = RwRasterGetHeight(RwCameraGetRaster(camera));

    FrontBuffer = RpSkyGetDisplayBufferRaster();
    if (!FrontBuffer)
    {
        printf(RWSTRING("Error creating raster\n"));
    
        return FALSE;
    }

    /*
     * The immediate mode quad is the same from frame to frame, 
     * apart from the alpha used for the blending, so create 
     * this data once...
     */
    CreateImmediateModeData(camera, &rect);
    
    return TRUE;
}


/*
 *****************************************************************************
 */
RwBool
MotionBlurClose(void)
{
    if( FrontBuffer ) 
    {
#if 0
        /* We must not do this as we did not create this raster. */
        RwRasterDestroy(FrontBuffer);
#endif

        FrontBuffer = NULL;
        
        return TRUE;
    }
    
    return FALSE;
}

/*
 *****************************************************************************
 */
