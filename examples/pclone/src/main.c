
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
 * Copyright (C) 2001 Criterion Technologies.
 *
 * Original author: Alexandre Hadjdaj.
 * Reviewed by: .
 *                                                                         
 * Purpose: Illustrate the use of the Standard Particle Plugin with the cloning 
 *          Pipeline.
 *                         
 ****************************************************************************/

#include "rwcore.h"
#include "rpworld.h"
#include "rppds.h"

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

#include "pclone.h"

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
static RwRGBA BackgroundColor = { 64,  0,  0,   255};

static RtCharset *Charset = NULL;

RwCamera *Camera = NULL;

RpWorld *World = NULL;

RpLight *Ambient = NULL;
RpLight *Directional = NULL;

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
        RwV3d camPos = {0.0f, 00.0f, -200.0f};
        RwFrame *camFrame = NULL;
        RwFrame *Frame = NULL;
    
        camFrame = RwCameraGetFrame(camera);
        RwFrameSetIdentity(camFrame);
        
        Frame = RwFrameCreate();
        RwFrameAddChild(Frame,camFrame);
                
        
        RwFrameTranslate(Frame,&camPos, rwCOMBINEPOSTCONCAT);

        RwCameraSetNearClipPlane(camera, 1.0f);
        RwCameraSetFarClipPlane(camera, 200.0f);

        RpWorldAddCamera(world, camera);
        
        return camera;
    }

    return NULL;
}


/*
 *****************************************************************************
 */
static RpLight *
CreateDirectionalLight(RpWorld *world)
{
    RpLight *light;

    light = RpLightCreate(rpLIGHTDIRECTIONAL);

    if( light )
    {
        RwFrame *frame;

        frame = RwFrameCreate();

        if( frame )
        {
            RwRGBAReal color = {1.0f, 0.5f, 1.0f, 1.0f};
            RwV3d Xaxis = {1.0f, 0.0f, 0.0f};
            RwV3d Yaxis = {0.0f, 1.0f, 0.0f};

            RpLightSetColor(light, &color);

            RwFrameSetIdentity(frame);
            RwFrameRotate(frame, &Xaxis, 30.0f, rwCOMBINEREPLACE);
            RwFrameRotate(frame, &Yaxis, 60.0f, rwCOMBINEPOSTCONCAT);

            RpLightSetFrame(light, frame);

            RpWorldAddLight(world, light);

            return light;
        }

        RpLightDestroy(light);
    }

    return NULL;
}


/*
 *****************************************************************************
 */
static RpLight *
CreateAmbientLight(RpWorld *world)
{
    RpLight *light;

    light = RpLightCreate(rpLIGHTAMBIENT);

    if( light )
    {
        RwFrame *frame;

        frame = RwFrameCreate();

        if( frame )
        {
            RwRGBAReal color = {0.25f, 0.0f, 0.0f, 1.0f};

            RpLightSetColor(light, &color);

            RpWorldAddLight(world, light);

            return light;
        }

        RpLightDestroy(light);
    }

    return NULL;
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

        RsGlobal.appName = RWSTRING("RenderWare Graphics Cloning and Particles Example");

        RsGlobal.maxFPS = 120;

        return TRUE;
    }

    return FALSE;
}


/*
 *****************************************************************************
 */
static RwBool 
emtSizeMenuCB(RwBool justCheck)
{
    if( justCheck )
    {
        return TRUE;
    }

    

    return TRUE;
}


static RwBool 
emtEmitBiasCB(RwBool justCheck)
{
    if( justCheck )
    {
        return TRUE;
    }

    MenuSetRangeInt(&PrtData.EmtStd->emtPrtEmitBias,
                0,
                BATCHSIZE-PrtData.EmtStd->emtPrtEmit,
                1,
                NULL);

    return TRUE;
}


static RwBool 
InitializeMenu(void)
{
    static RwChar fpsLabel[]      = RWSTRING("FPS_F");

    if( MenuOpen(TRUE, &ForegroundColor, &BackgroundColor) )
    {   
        MenuAddEntryBool(fpsLabel, 
                        &FPSOn, 
                        NULL);

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
    /* Because we're drawing so many atomics per frame,
     * we need to enlarge the DMA buffers.
     */
    _rwDMAPreAlloc(5*1024*1024, 1024, NULL);

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

	Directional = CreateDirectionalLight(World);
    if( Directional == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create directional light."));

        return FALSE;
    }
	
	Ambient = CreateAmbientLight(World);
    if( Ambient == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create ambient light."));

        return FALSE;
    }

    if( !pCloneInitialize() )
    {
        RsErrorMessage(RWSTRING("Error initializing Im3D."));

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
static void 
Terminate3D(void)
{
#ifdef RWMETRICS
    RsMetricsClose();
#endif

    MenuClose();

    pCloneTerminate();

    if( Camera )
    {
        RpWorldRemoveCamera(World, Camera);

        CameraDestroy(Camera);
    }

	if( Ambient )
	{
		RpWorldRemoveLight(World,Ambient);
		
		RpLightDestroy(Ambient);
	}
	
	if( Directional )
	{
		RwFrame *frame = RpLightGetFrame(Directional);

		RpWorldRemoveLight(World,Directional);
		
		RpLightSetFrame(Directional,NULL);
		
		RwFrameDestroy(frame);
		RpLightDestroy(Directional);
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
    rwPDS_G3x_ADLClone_MatPipeRegister();
    rwPDS_G3x_ADLClone_AtmPipeRegister();
    rwPDS_G3x_OPLClone_MatPipeRegister();
    rwPDS_G3x_OPLClone_AtmPipeRegister();
    
    /*
     * Attach world plug-in...
     */
    if( !RpWorldPluginAttach() )
    {
        return FALSE;
    }

    /*
     * Attach PTank plug-in...
     */
    if( !RpPTankPluginAttach() )
    {
        return FALSE;
    }

    /*
     * Attach PrtStd plug-in...
     */
    if ( !RpPrtStdPluginAttach() )
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
Render(void)
{
    RwCameraClear(Camera, &BackgroundColor, rwCAMERACLEARZ|rwCAMERACLEARIMAGE);

    if( RwCameraBeginUpdate(Camera) )
    {
        if( MenuGetStatus() != HELPMODE )
        {
            pCloneRender();

            DisplayOnScreenInfo();
        }

        MenuRender(Camera, NULL);

#ifdef RWMETRICS
        RsMetricsRender();
#endif

        RwCameraEndUpdate(Camera);
    }

    /* 
     * Display camera's raster...
     */
    RsCameraShowRaster(Camera);

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
    RwReal deltaTime;

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

    /*
     * Animation update time in seconds...
     */
    deltaTime = (thisTime - lastAnimTime) * 0.001f;

    
    /*
     * Update the Particle System...
     */
	RpPrtStdAtomicUpdate(PrtData.atmEmitter, (void *)&deltaTime);

    lastAnimTime = thisTime;

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
