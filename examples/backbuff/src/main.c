
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
 * main.c
 *
 * Copyright (C) 2000 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *                                                                         
 * Purpose: How to create a DMA packet to blit the backbuffer to a raster.
 *                         
 ****************************************************************************/

/*
 * For extern sceGsDBuffDc _rwDMAFlipData...
 */
#include "libgraph.h"

#include "rwcore.h"
#include "rpworld.h"

#ifdef RWLOGO
#include "rplogo.h"
#endif

#include "rtcharse.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"
#include "camera.h"

#ifdef RWMETRICS
#include "metrics.h"
#endif

#include "main.h"

#define DEFAULT_SCREEN_WIDTH (640)
#define DEFAULT_SCREEN_HEIGHT (480)

#ifdef WIDE_SCREEN
#define DEFAULT_ASPECTRATIO (16.0f/9.0f)
#else
#define DEFAULT_ASPECTRATIO (4.0f/3.0f)
#endif

#define DEFAULT_VIEWWINDOW (0.5f)

static RwBool FPSOn = FALSE;

static RwInt32 FrameCounter = 0;
static RwInt32 FramesPerSecond = 0;

static RwRGBA ForegroundColor = {200, 200, 200, 255};
static RwRGBA BackgroundColor = { 64,  64,  64,   0};

static RpWorld *World = (RpWorld *)NULL;
static RwCamera *Camera = (RwCamera *)NULL;
static RtCharset *Charset = (RtCharset *)NULL;
static RpLight *MainLight = (RpLight *)NULL;
static RpClump *Clump = (RpClump *)NULL;

static RwIm2DVertex Vertex[4];
static RwImVertexIndex Index[6] = {0, 1, 2, 0, 2, 3};

static RwRaster *Raster = (RwRaster *)NULL;
/* static RwTexture *Texture = (RwTexture *)NULL; */

static RwCamera *Camera2 = (RwCamera *)NULL;


/*
 *****************************************************************************
 */
static void
CreateImData(RwCamera *camera)
{
    RwReal width, height;
    RwReal u0, v0, u1, v1;

    width = RwRasterGetWidth(RwCameraGetRaster(camera));
    height = RwRasterGetHeight(RwCameraGetRaster(camera));

    u0 = 0.0f;
    v0 = 0.0f;
    u1 = (width + 0.5f) / (width <= 512? 512.0f:1024.0f);
    v1 = (height + 0.5f) / (height <= 256? 256.0f: (height <= 512 ? 512.0f: 1024.0f));

    width /= 2.0f;
    height /= 2.0f;

    RwIm2DVertexSetScreenX(&Vertex[0], 0.0f);
    RwIm2DVertexSetScreenY(&Vertex[0], 0.0f);
    RwIm2DVertexSetScreenZ(&Vertex[0], RwIm2DGetNearScreenZ());
    RwIm2DVertexSetCameraZ(&Vertex[0], RwCameraGetNearClipPlane(camera));
    RwIm2DVertexSetRecipCameraZ(&Vertex[0], 1.0f / RwCameraGetNearClipPlane(camera));
    RwIm2DVertexSetU(&Vertex[0], u0, 1.0f);
    RwIm2DVertexSetV(&Vertex[0], v0, 1.0f);
    RwIm2DVertexSetIntRGBA(&Vertex[0], 255, 255, 255, 128);
    
    RwIm2DVertexSetScreenX(&Vertex[1], 0.0f);
    RwIm2DVertexSetScreenY(&Vertex[1], height);
    RwIm2DVertexSetScreenZ(&Vertex[1], RwIm2DGetNearScreenZ());
    RwIm2DVertexSetCameraZ(&Vertex[1], RwCameraGetNearClipPlane(camera));
    RwIm2DVertexSetRecipCameraZ(&Vertex[1], 1.0f / RwCameraGetNearClipPlane(camera));
    RwIm2DVertexSetU(&Vertex[1], u0, 1.0f);
    RwIm2DVertexSetV(&Vertex[1], v1, 1.0f);
    RwIm2DVertexSetIntRGBA(&Vertex[1], 255, 255, 255, 128);
    
    RwIm2DVertexSetScreenX(&Vertex[2], width);
    RwIm2DVertexSetScreenY(&Vertex[2], height);
    RwIm2DVertexSetScreenZ(&Vertex[2], RwIm2DGetNearScreenZ());
    RwIm2DVertexSetCameraZ(&Vertex[2], RwCameraGetNearClipPlane(camera));
    RwIm2DVertexSetRecipCameraZ(&Vertex[2], 1.0f / RwCameraGetNearClipPlane(camera));
    RwIm2DVertexSetU(&Vertex[2], u1, 1.0f);
    RwIm2DVertexSetV(&Vertex[2], v1, 1.0f);
    RwIm2DVertexSetIntRGBA(&Vertex[2], 255, 255, 255, 128);
    
    RwIm2DVertexSetScreenX(&Vertex[3], width);
    RwIm2DVertexSetScreenY(&Vertex[3], 0.0f);
    RwIm2DVertexSetScreenZ(&Vertex[3], RwIm2DGetNearScreenZ());
    RwIm2DVertexSetCameraZ(&Vertex[3], RwCameraGetNearClipPlane(camera));
    RwIm2DVertexSetRecipCameraZ(&Vertex[3], 1.0f / RwCameraGetNearClipPlane(camera));
    RwIm2DVertexSetU(&Vertex[3], u1, 1.0f);
    RwIm2DVertexSetV(&Vertex[3], v0, 1.0f);
    RwIm2DVertexSetIntRGBA(&Vertex[3], 255, 255, 255, 128);

    return;
}


/*
 *****************************************************************************
 */
static void
CreateSecondCamera(RpWorld *world, RwRaster *raster)
{
    RwRaster *zRaster = (RwRaster *)NULL;

    zRaster = RwRasterCreate(RwRasterGetWidth(raster),
        RwRasterGetHeight(raster), 0, rwRASTERTYPEZBUFFER);

    if( zRaster )
    {
        RwFrame *frame = (RwFrame *)NULL;

        frame = RwFrameCreate();

        if( frame )
        {
            RwCamera *camera = (RwCamera *)NULL;

            camera = RwCameraCreate();

            if( camera)
            {
                RwV2d vw;

                RwCameraSetRaster(camera, raster);
                RwCameraSetZRaster(camera, zRaster);

                RwCameraSetFrame(camera, frame);

                RwCameraSetNearClipPlane(camera, 0.1f);
                RwCameraSetFarClipPlane(camera, 250.0f);

                vw.x = vw.y = 0.4f;
                RwCameraSetViewWindow(camera, &vw);

                RpWorldAddCamera(world, camera);

#ifdef RWLOGO
                RpLogoSetState(camera, FALSE);
#endif

                Camera2 = camera;

                return;
            }

            RwFrameDestroy(frame);
        }

        RwRasterDestroy(zRaster);
    }

    return;
}


/*
 *****************************************************************************
 */
static RwRaster *
CreateRaster(RwCamera *camera)
{
    RwRaster *raster = (RwRaster *)NULL;
    RwRaster *cameraRaster = (RwRaster *)NULL;
    RwInt32 width, height, depth;

    cameraRaster = RwCameraGetRaster(camera);
    width = RwRasterGetWidth(cameraRaster);
    height = RwRasterGetHeight(cameraRaster);
    depth = RwRasterGetDepth(cameraRaster);

    raster = RwRasterCreate(width, height, depth, rwRASTERTYPECAMERATEXTURE);

    CreateSecondCamera(World, raster);

    return raster;
}


/*
 *****************************************************************************
 */
static void
DestroyRaster(void)
{
    if( Raster )
    {
        RwRasterDestroy(Raster);

        Raster = (RwRaster *)NULL;
    }

    return;
}


/*
 *****************************************************************************
 */
static RpWorld *
CreateWorld(void)
{
    RpWorld *world;
    RwBBox bb;

    bb.inf.x = bb.inf.y = bb.inf.z = -100.0f;
    bb.sup.x = bb.sup.y = bb.sup.z = 100.0f;

    world = RpWorldCreate(&bb);

    return world;
}


/*
 *****************************************************************************
 */
static RwCamera *
CreateCamera(RpWorld *world)
{
    RwCamera *camera;

    camera = CameraCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight, TRUE);

    if( camera )
    {
        RwCameraSetNearClipPlane(camera, 0.1f);
        RwCameraSetFarClipPlane(camera, 40.0f);

        RpWorldAddCamera(world, camera);

        return camera;
    }

    return NULL;
}


/*
 *****************************************************************************
 */
static RwBool
CreateLights(RpWorld *world)
{
    RpLight *light = (RpLight *)NULL;

    light = RpLightCreate(rpLIGHTAMBIENT);
    if( light )
    {
        RpWorldAddLight(world, light);
    }
    else
    {
        return FALSE;
    }

    light = (RpLight *)NULL;

    light = RpLightCreate(rpLIGHTDIRECTIONAL);
    if( light )
    {
        RwFrame *frame = NULL;
        
        frame = RwFrameCreate();
        if( frame )
        {
            RwV3d xAxis = {1.0f, 0.0f, 0.0f};
            RwV3d yAxis = {0.0f, 1.0f, 0.0f};

            RpLightSetFrame(light, frame);

            RwFrameRotate(frame, &xAxis, 45.0f, rwCOMBINEREPLACE);
            RwFrameRotate(frame, &yAxis, 45.0f, rwCOMBINEPOSTCONCAT);

            RpWorldAddLight(world, light);

            MainLight = light;

            return TRUE;
        }

        RpLightDestroy(light);
    }

    return FALSE;
}


/*
 *****************************************************************************
 */
void
ClumpRotate(RwReal angleX, RwReal angleY)
{
    RwMatrix *cameraMatrix;
    RwV3d right, up, pos;
    RwFrame *frame;

    cameraMatrix = RwFrameGetMatrix(RwCameraGetFrame(Camera));
    right = *RwMatrixGetRight(cameraMatrix);
    up = *RwMatrixGetUp(cameraMatrix);

    frame = RpClumpGetFrame(Clump);
    pos = *RwMatrixGetPos(RwFrameGetMatrix(frame));

    /*
     * First translate back to the origin...
     */
    RwV3dScale(&pos, &pos, -1.0f);
    RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);

    /*
     * ...do the rotations...
     */
    RwFrameRotate(frame, &up, angleX, rwCOMBINEPOSTCONCAT);
    RwFrameRotate(frame, &right, angleY, rwCOMBINEPOSTCONCAT);

    /*
     * ...and translate back...
     */
    RwV3dScale(&pos, &pos, -1.0f);
    RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);

    return;
}


/*
 *****************************************************************************
 */
void 
ClumpTranslateZ(RwReal deltaZ)
{
    RwMatrix *cameraMatrix;
    RwV3d at;
    RwFrame *frame;

    cameraMatrix = RwFrameGetMatrix(RwCameraGetFrame(Camera));
    at = *RwMatrixGetAt(cameraMatrix);

    frame = RpClumpGetFrame(Clump);

    RwV3dScale(&at, &at, deltaZ);

    RwFrameTranslate(frame, &at, rwCOMBINEPOSTCONCAT);

    return;
}


/*
 *****************************************************************************
 */
static RpClump *
CreateClump(RpWorld *world)
{
    RwStream *stream = (RwStream *)NULL;
    RpClump *clump = (RpClump *)NULL;
    RwChar *path = (RwChar *)NULL;

    path = RsPathnameCreate(RWSTRING("./models/textures/"));
    RwImageSetPath(path);
    RsPathnameDestroy(path);

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
        RwV3d xAxis = {1.0f, 0.0f, 0.0f};
        RwV3d yAxis = {0.0f, 1.0f, 0.0f};
        RwV3d pos   = {0.0f, 0.0f, 5.0f};
        RwFrame *clumpFrame = NULL;

        clumpFrame = RpClumpGetFrame(clump);

        RwFrameRotate(clumpFrame, &xAxis, 45.0f, rwCOMBINEREPLACE);
        RwFrameRotate(clumpFrame, &yAxis, 45.0f, rwCOMBINEPOSTCONCAT);

        RwFrameTranslate(clumpFrame, &pos, rwCOMBINEPOSTCONCAT);

        RpWorldAddClump(world, clump);
    }

    return clump;
}


/*
 *****************************************************************************
 */
static RwBool 
InitializeMenu(void)
{
    static RwChar fpsLabel[] = RWSTRING("FPS_F");

    if( MenuOpen(TRUE, &ForegroundColor, &BackgroundColor) )
    {
        MenuAddEntryBool(fpsLabel, &FPSOn, NULL);

        MenuSetStatus(MENUOFF);

        return TRUE;
    }

    return FALSE;
}


/*
 *****************************************************************************
 */
static RwBool 
Initialize(void)
{
    if( RsInitialize() )
    {
        if( !RsGlobal.maximumWidth )
        {
            RsGlobal.maximumWidth = DEFAULT_SCREEN_WIDTH;
        }

        if( !RsGlobal.maximumHeight )
        {
            RsGlobal.maximumHeight = DEFAULT_SCREEN_HEIGHT;
        }

        RsGlobal.appName = RWSTRING("RenderWare Graphics BackBuffer Example");

        RsGlobal.maxFPS = 120;

        return TRUE;
    }

    return FALSE;
}


/*
 *****************************************************************************
 */
static RwBool 
Initialize3D(void *param)
{
    if( !RsRwInitialize(param) )
    {
        RsErrorMessage(RWSTRING("Error initializing RenderWare."));

        return FALSE;
    }

    Charset = RtCharsetCreate(&ForegroundColor, &BackgroundColor);
    if( Charset == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create raster charset."));
    
        return FALSE;
    }

    World = CreateWorld();
    if( World == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create world."));

        return FALSE;
    }

    Camera = CreateCamera(World);
    if( Camera == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create camera."));

        return FALSE;
    }

    Raster = CreateRaster(Camera);
    if( Raster == (RwRaster *)NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create raster."));

        return FALSE;
    }
    
    CreateImData(Camera);

    if( !CreateLights(World) )
    {
        RsErrorMessage(RWSTRING("Cannot create light sources"));
    }   

    Clump = CreateClump(World);
    if( !Clump )
    {
        RsErrorMessage(RWSTRING("Cannot load clump."));
        
        return FALSE;
    }

    if( !InitializeMenu() )
    {
        RsErrorMessage(RWSTRING("Error initializing menu."));

        return FALSE;
    }

#ifdef RWMETRICS
    RsMetricsOpen(Camera);
#endif

    return TRUE;
}


/*
 *****************************************************************************
 */
static RpLight*
DestroyLight(RpLight *light, 
             void *data __RWUNUSED__)
{
    RwFrame *frame = (RwFrame *)NULL;

    RpWorldRemoveLight(World, light);

    frame = RpLightGetFrame(light);
    if( frame )
    {
        RpLightSetFrame(light, NULL);

        RwFrameDestroy(frame);
    }

    RpLightDestroy(light);

    return light;
}


/*
 *****************************************************************************
 */
static void 
Terminate3D(void)
{
#ifdef RWMETRICS
    RsMetricsClose();
#endif

    MenuClose();

    if( Clump )
    {
        if( RpClumpGetWorld(Clump) )
        {
            RpWorldRemoveClump(World, Clump);
        }

        RpClumpDestroy(Clump);
    }

    RpWorldForAllLights(World, DestroyLight, NULL);

    DestroyRaster();

    if( Camera )
    {
        RpWorldRemoveCamera(World, Camera);

        CameraDestroy(Camera);
    }

    if( Camera2 )
    {
        RwFrame *frame;
        RwRaster *raster;

        RpWorldRemoveCamera(World, Camera2);

        frame = RwCameraGetFrame(Camera2);
        RwCameraSetFrame(Camera2, NULL);
        RwFrameDestroy(frame);

        raster = RwCameraGetZRaster(Camera2);
        RwCameraSetZRaster(Camera2, NULL);
        RwRasterDestroy(raster);

        RwCameraDestroy(Camera2);
    }

    if( World )
    {
        RpWorldDestroy(World);
    }

    if( Charset )
    {
        RwRasterDestroy(Charset);
    }

    RsRwTerminate();

    return;
}


/*
 *****************************************************************************
 */
static RwBool 
AttachPlugins(void)
{
    /* 
     * Attach world plug-in...
     */
    if( !RpWorldPluginAttach() )
    {
        return FALSE;
    }

#ifdef RWLOGO
    /* 
     * Attach logo plug-in...
     */
    if( !RpLogoPluginAttach() )
    {
        return FALSE;
    }
#endif

    return TRUE;
}


/*
 *****************************************************************************
 */
static void 
DisplayOnScreenInfo(void)
{
    RwChar caption[256];

    if( FPSOn )
    {
        RsSprintf(caption, RWSTRING("FPS: %03d"), FramesPerSecond);

        RsCharsetPrint(Charset, caption, 0, 0, rsPRINTPOSTOPRIGHT);
    }

    return;
}


/*
 *****************************************************************************
 */
static void
RenderImQuad(RwRaster *raster)
{   
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)raster);    
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)rwFILTERNEAREST);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void *)FALSE);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)FALSE);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)FALSE);   
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)FALSE);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDONE);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDZERO);

    RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, Vertex, 4, Index, 6);

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)TRUE);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)TRUE);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDINVSRCALPHA);

    return;
}


/*
 *****************************************************************************
 */
static void
BlitBackBufferToRaster(RwRaster *raster)
{

    /*
     * If your RenderWare libs are dated 20/07/01 or greater then you can 
	 * remove the renderstate setting code below as the sky2 driver now sets 
	 * up the raster extesion on a call to RpSkyTexCacheRasterLock...
     */

	/*
	 * This is a bit ugly.  The DMA packet construction code below need
	 * to know the address in GS video memory for the camera texture
	 * raster.  However, the address is not written into the shadow
	 * tex0 lsb and msb words until the texture is marked as the current
	 * texture target.
	 */
	 
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)Raster);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);

    /*
     * Create a DMA packet that will blit the back buffer to our raster.
     * This will be a local buffer to local buffer transfer...
     */

    if( _rwDMAOpenVIFPkt(RWDMA_FIXUP, 5) )
    {
        unsigned long tmp, tmp1;
        u_long128 ltmp = 0;
        unsigned long frameAddress;
        unsigned long rasterAddress;
        unsigned long width, height;
        unsigned long pixelFormat;
        RwUInt32 msb, lsb;

        /*
         * First tell RenderWare the amount of quadwords we are going to send.
		 * This is one for every call to RWDMA_ADD_TO_PKT...
         */
        tmp = 4L |       /* NLOOP */
            (1L << 15) | /* EOP */
            (0L << 46) | /* PRE */
            (0L << 58) | /* FLG */
            (1L << 60);  /* NREG */

        tmp1 = (0xEL << (64 - 64)); /* A+D */
        MAKE128(ltmp, tmp1, tmp);
        RWDMA_ADD_TO_PKT(ltmp);

        /*
         * Find which frame buffer is currently the back buffer...
         */
        if( skyFrameBit & 0x1 )
        {
            tmp = *(unsigned long *)&_rwDMAFlipData.db.draw01.frame1;
        }
        else
        {
            tmp = *(unsigned long *)&_rwDMAFlipData.db.draw11.frame1;
        }    
        
        frameAddress = tmp & 0x1FF;

        /*
         * Frame address is word/2k. We need /64...
         */
        frameAddress <<= 5;

        /*
         * Find the base pointer of our raster...
         */
        skyTexGetTex0(raster, &msb, &lsb);
        width = (lsb >> 14) & 0x3F;
		rasterAddress = lsb & 0x3FFF;
		
        pixelFormat = (lsb >> 20) & 0x3F;
        
        /*
         * Now setup the registers. First the BITBLTBUF register...
         */
        tmp = (frameAddress & 0x3FFF) |        /* src buffer base pointer */
            (width << 16) |                    /* src buffer width */
            (pixelFormat << 24) |              /* src pixel storage format */
            ((rasterAddress & 0x3FFF) << 32) | /* dest buffer base pointer */
            (width << 48) |                    /* dest buffer width */
            (pixelFormat << 56);               /* dest pixel storage format */
        
        MAKE128(ltmp, GS_BITBLTBUF, tmp);
        RWDMA_ADD_TO_PKT(ltmp);

        /*
         * ...then set the offset in the buffer, and pixel transmission
         * order...
         */
        MAKE128(ltmp, GS_TRXPOS, 0L);
        RWDMA_ADD_TO_PKT(ltmp);

        /*
         * ...the width and height of the transmission for the dest buffer...
         */
        width = (unsigned long)RwRasterGetWidth(raster);
        height = (unsigned long)RwRasterGetHeight(raster);

        tmp = width |       
            (height << 32);

        MAKE128(ltmp, GS_TRXREG, tmp);
        RWDMA_ADD_TO_PKT(ltmp);

        /*
         * ...and finally the direction of transmission, local-to-local...
         */
        MAKE128(ltmp, GS_TRXDIR, 2L);
        RWDMA_ADD_TO_PKT(ltmp);
    }

    return;
}


/*
 *****************************************************************************
 */
static void 
Render(void)
{
    RwCameraClear(Camera, &BackgroundColor, rwCAMERACLEARZ|rwCAMERACLEARIMAGE);

	/*
	 * STEP ONE - render the world as normal.
	 */
    if( RwCameraBeginUpdate(Camera) )
    {   
        if( MenuGetStatus() != HELPMODE )
        {
            RpWorldRender(World);

            DisplayOnScreenInfo();
        }

        RwCameraEndUpdate(Camera);
    }

	/*
	 * STEP TWO - setup the camera texture raster and blit the back
	 *            buffer into it.
	 */

    RpSkyTexCacheRasterLock(Raster, TRUE);

    if( RwCameraBeginUpdate(Camera2) )
    {
        BlitBackBufferToRaster(Raster);

        RwCameraEndUpdate(Camera2);
    }

	/*
	 * STEP THREE - render the camere texture raster over the back buffer.
	 */

    if( RwCameraBeginUpdate(Camera) )
    {
        if( MenuGetStatus() != HELPMODE )
        {
            RenderImQuad(Raster);

            DisplayOnScreenInfo();
        }

        MenuRender(Camera, NULL);

#ifdef RWMETRICS
        RsMetricsRender();
#endif
   
        RwCameraEndUpdate(Camera);
    }

    RsCameraShowRaster(Camera);

	/* Free up the memory that the camera texture raster used. */
    RpSkyTexCacheRasterLock(Raster, FALSE);

    FrameCounter++;

    return;
}


/*
 *****************************************************************************
 */
static void 
Idle(void)
{
    RwUInt32 thisTime;
  
    static RwBool firstCall = TRUE;
    static RwUInt32 lastFrameTime, lastAnimTime;

    if( firstCall )
    {
        lastFrameTime = lastAnimTime = RsTimer();

        firstCall = FALSE;
    }

    thisTime = RsTimer();

    /* 
     * Has a second elapsed since we last updated the FPS...
     */
    if( thisTime > (lastFrameTime + 1000) )
    {
        /* 
         * Capture the frame counter...
         */
        FramesPerSecond = FrameCounter;
        
        /*
         * ...and reset...
         */
        FrameCounter = 0;
        
        lastFrameTime = thisTime;
    }

    Render();

    return;
}


/*
 *****************************************************************************
 */
RsEventStatus
AppEventHandler(RsEvent event, void *param)
{
    switch( event )
    {
        case rsINITIALIZE:
        {
            return Initialize() ? rsEVENTPROCESSED : rsEVENTERROR;
        }

        case rsCAMERASIZE:
        {
            CameraSize(Camera, (RwRect *)param, 
                DEFAULT_VIEWWINDOW, DEFAULT_ASPECTRATIO);

            return rsEVENTPROCESSED;
        }

        case rsRWINITIALIZE:
        {
            return Initialize3D(param) ? rsEVENTPROCESSED : rsEVENTERROR;
        }

        case rsRWTERMINATE:
        {
            Terminate3D();

            return rsEVENTPROCESSED;
        }

        case rsPLUGINATTACH:
        {
            return AttachPlugins() ? rsEVENTPROCESSED : rsEVENTERROR;
        }

        case rsINPUTDEVICEATTACH:
        {
            AttachInputDevices();

            return rsEVENTPROCESSED;
        }

        case rsIDLE:
        {
            Idle();

            return rsEVENTPROCESSED;
        }

        default:
        {
            return rsEVENTNOTPROCESSED;
        }
    }
}

/*
 *****************************************************************************
 */
