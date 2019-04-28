
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
 * Copyright (C) 2001 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 ****************************************************************************/

#include "rwcore.h"
#include "rpworld.h"

#ifdef RWLOGO
#include "rplogo.h"
#endif

#include "rpmatfx.h"
#include "rpskin.h"
#include "rphanim.h"
#include "rt2d.h"

#include "rppds.h"

#include "rtcharse.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"
#include "camera.h"

#include "ps2all.h"

#ifdef RWMETRICS
#include "metrics.h"
#endif

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

static RwRGBA ForegroundColor = { 200, 200, 200, 255 };
static RwRGBA BackgroundColor = { 64, 64, 64, 0 };

RpWorld   *World = NULL;
RwCamera  *Camera = NULL;
RtCharset *Charset = NULL;

RpLight *Ambiant = NULL;
RpLight *Direct[4] = { NULL, NULL, NULL, NULL };
RpLight *Point = NULL;
RwFrame *PointFrame;
RwFrame *DirectFrame[4];

RpLight *DirectionalLight = NULL;

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
CreateCamera(RpWorld * world)
{
    RwCamera *camera;

    camera =
        CameraCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight,
                     TRUE);
    if (camera)
    {
        RwCameraSetNearClipPlane(camera, 0.1f);
        RwCameraSetFarClipPlane(camera, 30.0f);

        RpWorldAddCamera(world, camera);

        return camera;
    }

    return NULL;
}

/*
 *****************************************************************************
 */
static RwBool
CreateLights(RpWorld * world __RWUNUSED__)
{
    /* Create a set of lights. */
    RwRGBAReal  cola, cold, colp;

    /* Ambient light. */
    Ambiant  = RpLightCreate(rpLIGHTAMBIENT);
    cola.red   = 0.5;
    cola.green = 0.1;
    cola.blue  = 0.1;
    RpLightSetColor(Ambiant, &cola);

    /* Point light. */
    Point  = RpLightCreate(rpLIGHTPOINT);
    colp.red   = 0.0;
    colp.green = 1.5;
    colp.blue  = 0.0;
    RpLightSetColor(Point, &colp);
    RpLightSetRadius(Point, 1000.0f);
    PointFrame = RwCameraGetFrame(Camera);
    RpLightSetFrame(Point, PointFrame);

    /* 4 Directional lights. */
    Direct[0] = RpLightCreate(rpLIGHTDIRECTIONAL);
    Direct[1] = RpLightCreate(rpLIGHTDIRECTIONAL);
    Direct[2] = RpLightCreate(rpLIGHTDIRECTIONAL);
    Direct[3] = RpLightCreate(rpLIGHTDIRECTIONAL);

    cold.red   = 0.0;
    cold.green = 0.0;
    cold.blue  = 0.5;
    RpLightSetColor(Direct[0], &cold);
    cold.red   = 0.5;
    cold.green = 0.0;
    cold.blue  = 0.0;
    RpLightSetColor(Direct[1], &cold);
    cold.red   = 0.0;
    cold.green = 0.5;
    cold.blue  = 0.0;
    RpLightSetColor(Direct[2], &cold);
    cold.red   = 0.0;
    cold.green = 0.0;
    cold.blue  = 0.5;
    RpLightSetColor(Direct[3], &cold);

    DirectFrame[0] = RwCameraGetFrame(Camera);
    DirectFrame[1] = RwFrameCreate();
    DirectFrame[2] = RwFrameCreate();
    DirectFrame[3] = RwFrameCreate();

    (void)RpLightSetFrame(Direct[0], DirectFrame[0]);
    (void)RpLightSetFrame(Direct[1], DirectFrame[1]);
    (void)RpLightSetFrame(Direct[2], DirectFrame[2]);
    (void)RpLightSetFrame(Direct[3], DirectFrame[3]);

    /* Remember one of the directional lights. */
    DirectionalLight = Direct[1];

    RpWorldAddLight(World, Ambiant);
    RpWorldAddLight(World, Direct[0]);

    return TRUE;
}

/*
 *****************************************************************************
 */
static RpLight *
lightDestroyCB(RpLight * light, void *data)
{
    RpWorld            *world = (RpWorld *) data;
    RwFrame *frame;

    RpWorldRemoveLight(world, light);
    frame = RpLightGetFrame(light);
    if (NULL != frame)
    {
        RpLightSetFrame(light, NULL);
        RwFrameDestroy(frame);
    }
    RpLightDestroy(light);

    return (light);
}

/*
 *****************************************************************************
 */
static RwBool
Initialize(void)
{
    if (RsInitialize())
    {
        if (!RsGlobal.maximumWidth)
        {
            RsGlobal.maximumWidth = DEFAULT_SCREEN_WIDTH;
        }

        if (!RsGlobal.maximumHeight)
        {
            RsGlobal.maximumHeight = DEFAULT_SCREEN_HEIGHT;
        }

        RsGlobal.appName = RWSTRING("RenderWare Graphics Ps2All PDS Custom Pipeline Example");

        RsGlobal.maxFPS = 120;

        return TRUE;
    }

    return FALSE;
}

/*
 *****************************************************************************
 */
static RwBool
InitializeMenu(void)
{
    static RwChar fpsLabel[] = RWSTRING("FPS_F");

    if (MenuOpen(TRUE, &ForegroundColor, &BackgroundColor))
    {
        MenuAddEntryBool(fpsLabel, &FPSOn, NULL);

        PS2AllMenuOpen();

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

    if (!RsRwInitialize(param))
    {
        RsErrorMessage(RWSTRING("Error initializing RenderWare."));

        return FALSE;
    }

    Charset = RtCharsetCreate(&ForegroundColor, &BackgroundColor);
    if (Charset == NULL)
    {
        RsErrorMessage(RWSTRING("Cannot create raster charset."));

        return FALSE;
    }

    /*
     * Create an empty world if not loading a BSP...
     */
    World = CreateWorld();
    if (World == NULL)
    {
        RsErrorMessage(RWSTRING("Cannot create world."));

        return FALSE;
    }

    /*
     * Create a camera using the democom way...
     */
    Camera = CreateCamera(World);
    if (Camera == NULL)
    {
        RsErrorMessage(RWSTRING("Cannot create camera."));

        return FALSE;
    }

    if (!CreateLights(World))
    {
        RsErrorMessage(RWSTRING("Cannot create lights."));

        return FALSE;
    }

    if (!PS2AllOpen())
    {
        RsErrorMessage(RWSTRING("Cannot initialize PS2All example."));

        return FALSE;
    }

    if (!InitializeMenu())
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
    /*
     * Close or destroy RenderWare components in the reverse order they
     * were opened or created...
     */

#ifdef RWMETRICS
    RsMetricsClose();
#endif

    MenuClose();

    PS2AllClose();

    if (Camera)
    {
        RpWorldRemoveCamera(World, Camera);

        /*
         * This assumes the camera was created the democom way...
         */
        CameraDestroy(Camera);
    }

    if (World)
    {
        (void)RpLightSetFrame(Direct[0],NULL);
        RpLightSetFrame(Point, NULL);
        
        RpWorldForAllLights(World, lightDestroyCB, (void *) World);
        RpWorldDestroy(World);
    }

    if (Charset)
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

    /* PDS system is automatically attached by the skeleton. */
    /* RpPDSPluginAttach(8 + 2 + 3 + RpWorldNumPipes); */

    /* Register custom pipelines. */
    rwPDS_G3x_APL_MatPipeRegister();
    rwPDS_G3x_ADL_MatPipeRegister();
    rwPDS_G3x_A4D_MatPipeRegister();
    rwPDS_G3x_ADLClone_MatPipeRegister();
    rwPDS_G3x_OPLClone_MatPipeRegister();

    rwPDS_G3x_Generic_AtmPipeRegister();
    rwPDS_G3x_ADLClone_AtmPipeRegister();
    rwPDS_G3x_OPLClone_AtmPipeRegister();

    /* Register new style world pipelines. */
    rwPDS_G3_Generic_AtmPipeRegister();
    rwPDS_G3_Generic_MatPipeRegister();

    /* We don't need to register our 3 custom pipelines here,
     * (but we could if we wanted to), as no plugin is requiring
     * them. So we do it in the PS2AllOpen() function. */

    /* World pipelines are automatically registered by the skeleton. */
    /* RpWorldPipesAttach(); */
    
    /* Attach world plug-in. */
    if (!RpWorldPluginAttach())
    {
        return FALSE;
    }

#ifdef RWLOGO
    /* Attach logo plug-in. */
    if (!RpLogoPluginAttach())
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

    if (FPSOn)
    {
        RsSprintf(caption, RWSTRING("FPS: %03d"), FramesPerSecond);

        RsCharsetPrint(Charset, caption, 0, 0, rsPRINTPOSTOPRIGHT);
    }

    PS2AllOnScreenInfo(FramesPerSecond);

    return;
}

/*
 *****************************************************************************
 */
static void
Render(void)
{
    RwCameraClear(Camera, &BackgroundColor,
                  rwCAMERACLEARZ | rwCAMERACLEARIMAGE);

    if (RwCameraBeginUpdate(Camera))
    {
        if (MenuGetStatus() != HELPMODE)
        {
            /* Render the example atomics */
            PS2AllRender();

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
    RwUInt32            thisTime;
    RwReal              deltaTime;

    static RwBool firstCall = TRUE;
    static RwUInt32 lastFrameTime, lastAnimTime;

    if (firstCall)
    {
        lastFrameTime = lastAnimTime = RsTimer();

        firstCall = FALSE;
    }

    thisTime = RsTimer();

    /*
     * Has a second elapsed since we last updated the FPS...
     */
    if (thisTime > (lastFrameTime + 1000))
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

    PS2AllUpdate(deltaTime);

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
    switch (event)
    {
        case rsINITIALIZE:
            {
                return Initialize()? rsEVENTPROCESSED : rsEVENTERROR;
            }

        case rsCAMERASIZE:
            {
                CameraSize(Camera, (RwRect *) param,
                           DEFAULT_VIEWWINDOW, DEFAULT_ASPECTRATIO);

                return rsEVENTPROCESSED;
            }

        case rsRWINITIALIZE:
            {
                return Initialize3D(param) ? rsEVENTPROCESSED :
                    rsEVENTERROR;
            }

        case rsRWTERMINATE:
            {
                Terminate3D();

                return rsEVENTPROCESSED;
            }

        case rsPLUGINATTACH:
            {
                return AttachPlugins()? rsEVENTPROCESSED : rsEVENTERROR;
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

        case rsSELECTDEVICE:
            {
                return rsEVENTNOTPROCESSED;
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
