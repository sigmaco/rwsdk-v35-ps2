
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
 * Purpose: To illustrate a motion-blur technique on PS2.
 *                         
 ****************************************************************************/

#include "rwcore.h"
#include "rpworld.h"

#ifdef RWLOGO
#include "rplogo.h"
#endif

#include "rtcharse.h"
#include "rprandom.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"
#include "camera.h"

#ifdef RWMETRICS
#include "metrics.h"
#endif

#include "mblur.h"

#define DEFAULT_SCREEN_WIDTH (640)
#define DEFAULT_SCREEN_HEIGHT (480)

#ifdef WIDE_SCREEN
#define DEFAULT_ASPECTRATIO (16.0f/9.0f)
#else
#define DEFAULT_ASPECTRATIO (4.0f/3.0f)
#endif

#define DEFAULT_VIEWWINDOW (0.5f)

#define MAXBALLS (5)
#define RED (0)
#define YELLOW (1)
#define BLUE (2)

static RwBool FPSOn = FALSE;

static RwInt32 FrameCounter = 0;
static RwInt32 FramesPerSecond = 0;

static RwRGBA ForegroundColor = {200, 200, 200, 255};
static RwRGBA BackgroundColor = {  0,   0,   0,   0};

static RwBBox Box = 
{
    { 10.0f,  10.0f,  10.0f}, 
    {-10.0f, -10.0f, -10.0f}

};
static RpWorld *World = NULL;
static RwCamera *Camera = NULL;
static RtCharset *Charset = NULL;
static RpClump *Balls[3][MAXBALLS];
static RpLight *MainLight = NULL;

static RwV3d Velocity[3][MAXBALLS];

static RwReal Scale = 0.5f;
static RwReal BoxScale = 0.5f;

static RwInt32 MotionBlurAlpha = 128;


static RwReal 
RandomFloat(void)
{
    const RwUInt32 random = RpRandom();
    
    return ((RwReal)random)/((RwReal)RPRANDMAX);
}

/*
 *****************************************************************************
 */
static RpWorld *
CreateWorld(void)
{
    RpWorld *world;

    world = RpWorldCreate(&Box);

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
        RwFrame *cameraFrame;
        const RwBBox *bbox;
        RwV3d pos;

        bbox = RpWorldGetBBox(world);
        RwV3dAdd(&pos, &bbox->inf, &bbox->sup);
        pos.z = bbox->inf.z;

        cameraFrame = RwCameraGetFrame(camera);
        RwFrameTranslate(cameraFrame, &pos, rwCOMBINEREPLACE);

        RwCameraSetNearClipPlane(camera, 0.1f);
        RwCameraSetFarClipPlane(camera, 20.0f);

        RpWorldAddCamera(world, camera);

        return camera;
    }

    return NULL;
}


/*
 *****************************************************************************
 */
static RpLight *
CreateLight(RpWorld *world)
{
    RpLight *light;

    light = RpLightCreate(rpLIGHTDIRECTIONAL);
    if( light )
    {
        RpLightSetFrame(light, RwCameraGetFrame(Camera));

        RpWorldAddLight(world, light);
    }

    return light;
}


/*
 *****************************************************************************
 */
static RpClump *
LoadClump(const RwChar *name)
{
    RwStream *stream;
    RwChar *path;
    RpClump *clump;

    path = RsPathnameCreate(name);
    stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, path);
    RsPathnameDestroy(path);

    clump = NULL;

    if( stream )
    {
        if( RwStreamFindChunk(stream, rwID_CLUMP, NULL, NULL) )
        {
            clump = RpClumpStreamRead(stream);
        }

        RwStreamClose(stream, NULL);
    }

    return clump;
}


/*
 *****************************************************************************
 */
static RpClump *
PositionClump(RpWorld *world __RWUNUSED__, RpClump *clump, RwV3d *offset)
{
    if( clump )
    {
        RwFrame *clumpFrame;
        
        clumpFrame = RpClumpGetFrame(clump);

        if( clumpFrame )
        {
            RwFrameTranslate(clumpFrame, offset, rwCOMBINEPOSTCONCAT);
        }
    }

    return clump;
}


/*
 *****************************************************************************
 */
static RpWorld *
AssetsCreate(RpWorld *world)
{
    const RwChar *names[] = 
    {
        RWSTRING("./models/redball.dff"), 
        RWSTRING("./models/yellball.dff"), 
        RWSTRING("./models/blueball.dff")
    };
    
    RwV3d offsets[] = 
    {
        {-2.0f, 0.0f, 0.0f},
        { 0.0f, 0.0f, 0.0f},
        { 2.0f, 0.0f, 0.0f}
    };
    
    RwUInt32 i;

    for(i=0; i<MAXBALLS; i++)
    {
        if( i == 0 )
        {
            Balls[RED][0] = LoadClump(names[0]);
            RpWorldAddClump(world, Balls[RED][0]);

            Balls[YELLOW][0] = LoadClump(names[1]);
            RpWorldAddClump(world, Balls[YELLOW][0]);

            Balls[BLUE][0] = LoadClump(names[2]);
            RpWorldAddClump(world, Balls[BLUE][0]);
        }
        else
        {
            Balls[RED][i]    = RpClumpClone(Balls[RED][0]);
            Balls[YELLOW][i] = RpClumpClone(Balls[YELLOW][0]);
            Balls[BLUE][i]   = RpClumpClone(Balls[BLUE][0]);
        }

        PositionClump(world, Balls[RED][i], offsets+0);
        Velocity[RED][i].x = (2.0f * RandomFloat()) - 1.0f;
        Velocity[RED][i].y = (2.0f * RandomFloat()) - 1.0f;
        Velocity[RED][i].z = RandomFloat();

        PositionClump(world, Balls[YELLOW][i], offsets+1);
        Velocity[YELLOW][i].x = (2.0f * RandomFloat()) - 1.0f;
        Velocity[YELLOW][i].y = (2.0f * RandomFloat()) - 1.0f;
        Velocity[YELLOW][i].z = RandomFloat();

        PositionClump(world, Balls[BLUE][i], offsets+2);
        Velocity[BLUE][i].x = (2.0f * RandomFloat()) - 1.0f;
        Velocity[BLUE][i].y = (2.0f * RandomFloat()) - 1.0f;
        Velocity[BLUE][i].z = RandomFloat();
    }

    return world;
}


/*
 *****************************************************************************
 */
static RpWorld *
AssetsDestroy(RpWorld *world)
{
    if( World )
    {
        RwUInt32 i, j;
        
        for(i=0; i<MAXBALLS; i++)
        {
            /*
             * We assume only RED, YELLOW and BLUE are defined...
             */
            for(j=0; j<3; j++)
            {
                if( world == RpClumpGetWorld(Balls[j][i]) )
                {
                    RpWorldRemoveClump(world, Balls[j][i]);

                    RpClumpDestroy(Balls[j][i]);
                }
            }
        }
    }
    
    return world;
}


/*
 *****************************************************************************
 */
static void
UpdateBall(RwFrame *frame, RwV3d *velocity)
{
    RwV3d pos;
    RwV3d vel;

    pos = *RwMatrixGetPos(RwFrameGetMatrix(frame));

    RwV3dScale(&vel, velocity, Scale * 0.1f);
    RwV3dAdd(&pos, &pos, &vel);
    RwFrameTranslate(frame, &pos, rwCOMBINEREPLACE);

    /*
     * Bounce the ball...
     */
    if( pos.x < Box.inf.x * BoxScale )
    {
        velocity->x = -velocity->x;
    }
    
    if( pos.y < Box.inf.y * BoxScale )
    {
        velocity->y = -velocity->y;
    }
    
    if( pos.z < Box.inf.z * BoxScale )
    {
        velocity->z = -velocity->z;
    }
    
    if( pos.x > Box.sup.x * BoxScale )
    {
        velocity->x = -velocity->x;
    }
    
    if( pos.y > Box.sup.y * BoxScale )
    {
        velocity->y = -velocity->y;
    }
    
    if( pos.z > Box.sup.z * BoxScale )
    {
        velocity->z = -velocity->z;
    }

    return;
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

        RsGlobal.appName = RWSTRING("RenderWare Graphics Motion-Blur Example");

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
    static RwChar alphaLabel[] = RWSTRING("Motion blur alpha");
    static RwChar speedLabel[] = RWSTRING("Speed");

    static RwChar fpsLabel[] = RWSTRING("FPS_F");

    if( MenuOpen(TRUE, &ForegroundColor, &BackgroundColor) )
    {   
        MenuAddEntryInt(alphaLabel, &MotionBlurAlpha, NULL, 0, 255, 1, NULL);
        MenuAddEntryReal(speedLabel, &Scale, NULL, 0.0f, 1.0f, 0.01f);

        MenuAddSeparator();

        MenuAddEntryBool(fpsLabel, &FPSOn, NULL);

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

    MainLight = CreateLight(World);
    if( MainLight == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create light."));

        return FALSE;
    }

    if( !AssetsCreate(World) )
    {
        RsErrorMessage(RWSTRING("Error loading assets."));

        return FALSE;
    }

    if( !MotionBlurOpen(Camera) )
    {
        RsErrorMessage(RWSTRING("Error creating motion-blur raster."));

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

    MotionBlurClose();

    AssetsDestroy(World);

    if( MainLight ) 
    {
        RwFrame *frame;

        RpWorldRemoveLight(World, MainLight);

        /*
         * The light shares the camera's frame, so just
         * detach the light...
         */
        frame = RpLightGetFrame(MainLight);
        RpLightSetFrame(MainLight, NULL);

        RpLightDestroy(MainLight);
    }

    if( Camera )
    {
        RpWorldRemoveCamera(World, Camera);

        /*
         * This assumes the camera was created the democom way...
         * (This comment should be removed.)
         */
        CameraDestroy(Camera);
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

    if( !RpRandomPluginAttach() )
    {
        return FALSE;
    }

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
            RpWorldRender(World);

            DisplayOnScreenInfo();
        }

        /* 
         * Now that the scene has been rendered, superimpose 
         * the previous frame...
         */
        if( MotionBlurAlpha != 0 )
        {
            MotionBlurRender(Camera, (MotionBlurAlpha & 0xff));
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
    RwInt32 i;

    static RwBool firstCall = TRUE;
    static RwUInt32 lastFrameTime;

    if( firstCall )
    {
        lastFrameTime = RsTimer();

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
     * Update ball positions...
     */
    for(i=0; i<MAXBALLS; i++)
    {
        UpdateBall(RpClumpGetFrame(Balls[RED][i]), Velocity[RED]+i);
        UpdateBall(RpClumpGetFrame(Balls[YELLOW][i]), Velocity[YELLOW]+i);
        UpdateBall(RpClumpGetFrame(Balls[BLUE][i]), Velocity[BLUE]+i);
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
