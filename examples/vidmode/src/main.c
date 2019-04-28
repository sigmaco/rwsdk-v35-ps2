
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
 * Copyright (c) 2000 Criterion Software Ltd.
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
 * Purpose: To demonstrate switching between different video modes on PS2.
 *                         
 ****************************************************************************/

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

static RtCharset *Charset = NULL;
static RpWorld *World = NULL;
static RwCamera *Camera = NULL;
static RpLight *AmbientLight = NULL;
static RpClump *Clump = NULL;

static RwBool MenuOn = TRUE;

static RwBool Initialize3D(void *param);
static void Terminate3D(void);

RwInt32 NewVideoModeIndex;

void UpdateVideoMode(void);



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
static RpLight *
CreateAmbientLight(RpWorld *world)
{
    RpLight *light = NULL;

    light = RpLightCreate(rpLIGHTAMBIENT);
    if( light )
    {
        RpWorldAddLight(world, light);
    }

    return light;
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
        RwV3d pos = {0.0f, 0.0f, -10.0f};
        RwFrame *frame;

        frame = RwCameraGetFrame(camera);

        RwFrameTranslate(frame, &pos, rwCOMBINEREPLACE);

        RwCameraSetNearClipPlane(camera, 0.1f);
        RwCameraSetFarClipPlane(camera, 15.0f);

        RpWorldAddCamera(world, camera);

        return camera;
    }

    return NULL;
}


/*
 *****************************************************************************
 */
static RpClump *
CreateClump(RpWorld *world)
{
    RpClump *clump = NULL;
    RwStream *stream = NULL;
    RwChar *path = NULL;

    /* 
     * Set images path...
     */
    path = RsPathnameCreate(RWSTRING("./models/textures/"));
    RwImageSetPath(path);
    RsPathnameDestroy(path);

    /* 
     * Open clump file...
     */
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
        RwFrameSetIdentity(RpClumpGetFrame(clump));

        RpWorldAddClump(world, clump);
    }

    return clump;
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

        RsGlobal.appName = RWSTRING("RenderWare Graphics PS2 Video Mode Example");

        return TRUE;
    }

    return FALSE;
}


/*
 *****************************************************************************
 */
static RwBool
PrintAvailableVideoModes(const RwChar *filename)
{
    RwFileFunctions *fileFuncs;
    void *fp;
    RwChar *path;
    RwChar string[128];
    RwInt32 i;
    RwInt32 numVideoModes;  
    RwInt32 numSubSystems;

    fileFuncs = RwOsGetFileInterface();

    /* 
     * Open output file...
     */
    path = RsPathnameCreate(filename);
    fp = fileFuncs->rwfopen(path, RWSTRING("wt"));
    RsPathnameDestroy(path);

    if( fp == NULL )
    {
        return FALSE;
    }

    /* 
     * Output any information about any subsystems that may be present...
     */
    numSubSystems = RwEngineGetNumSubSystems();

    for(i=0; i<numSubSystems; i++)     
    { 
        RwSubSystemInfo ssInfo;                  

        if( RwEngineGetSubSystemInfo(&ssInfo, i) ) 
        { 
            RsSprintf(string, RWSTRING("SubSystem %d: %s\n"), i, ssInfo.name); 

            fileFuncs->rwfputs(string, fp);
        } 
    }

    /*
     * Output all available video modes and their properties...
     */
    numVideoModes = RwEngineGetNumVideoModes(); 
    
    for(i=0; i<numVideoModes; i++) 
    {
        RwVideoMode vmInfo;

        if( RwEngineGetVideoModeInfo(&vmInfo, i) )             
        {
            RsSprintf(string, 
                RWSTRING("Video Mode %2d:  %3d x %3d x %2d  %s%s%s%s%s\n"), 
                i, vmInfo.width, vmInfo.height, vmInfo.depth, 
                (vmInfo.flags & rwVIDEOMODEEXCLUSIVE ? "EXCLUSIVE " : ""),
                (vmInfo.flags & rwVIDEOMODEINTERLACE ? "INTERLACED " : ""),
                (vmInfo.flags & rwVIDEOMODEFFINTERLACE ? "FLICKER-FREE " : ""),
                (vmInfo.flags & rwVIDEOMODEFSAA0 ? "ANTI-ALIASED-0 " : ""),
                (vmInfo.flags & rwVIDEOMODEFSAA1 ? "ANTI-ALIASED-1 " : ""));

            fileFuncs->rwfputs(string, fp);
        }         
    }        
    
    fileFuncs->rwfclose(fp); 
    
    return TRUE;
}


/*
 *****************************************************************************
 */
void 
UpdateVideoMode(void)
{
    RwRect rect;
    RwInt32 status;

    /*
     * Remember the menu status...
     */
    status = MenuGetStatus();
    MenuOn = status == MENUMODE ? TRUE : FALSE;

    /*
     * Close down RenderWare and restart...
     */    
    Terminate3D();

    Initialize();
    Initialize3D((void *)NULL);

    /*
     * Force a camera resize to adjust the aspect-ratio...
     */
    rect.x = 0;
    rect.y = 0;
    rect.w = RsGlobal.maximumWidth;
    rect.h = RsGlobal.maximumHeight;

    RsEventHandler(rsCAMERASIZE, &rect);

    return;
}


/*
 *****************************************************************************
 */
static RwBool
SwitchVideoModeCallback(RwBool testEnable)
{
    if( testEnable )
    {
        return TRUE;
    }

    UpdateVideoMode();    

    return TRUE;
}


static RwBool 
DefaultVideoModeCallback(RwBool testEnable)
{
    if( testEnable )
    {
        return TRUE;
    }

    NewVideoModeIndex = RwEngineGetNumVideoModes()-1;

    UpdateVideoMode();

    return TRUE;
}


static RwBool 
InitializeMenu(void)
{    

    static RwChar videoModeLabel[]   = RWSTRING("Video mode");
    static RwChar switchModeLabel[]  = RWSTRING("Switch mode");
    static RwChar defaultModeLabel[] = RWSTRING("Default mode");

    static RwChar fpsLabel[]         = RWSTRING("FPS_F");

    if( MenuOpen(TRUE, &ForegroundColor, &BackgroundColor) )
    {
        MenuAddEntryInt(videoModeLabel, &NewVideoModeIndex, NULL, 0, 
            RwEngineGetNumVideoModes()-1, 1, NULL); 
        
        MenuAddEntryTrigger(switchModeLabel, SwitchVideoModeCallback);

        MenuAddEntryTrigger(defaultModeLabel, DefaultVideoModeCallback);
        
        MenuAddSeparator();
    
        MenuAddEntryBool(fpsLabel, &FPSOn, NULL);

        MenuOn ? MenuSetStatus(MENUMODE) : MenuSetStatus(MENUOFF);

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

    AmbientLight = CreateAmbientLight(World);
    if( AmbientLight == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create ambient light."));

        return FALSE;
    }

    Camera = CreateCamera(World);
    if( Camera == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create camera."));

        return FALSE;
    }
    
    Clump = CreateClump(World);
    if( Clump == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create clump."));

        return FALSE;
    }

    if( !InitializeMenu() )
    {
        RsErrorMessage(RWSTRING("Error initializing menu."));

        return FALSE;
    }
    
    if( !PrintAvailableVideoModes(RWSTRING("./vmodes.txt")) )
    {
        RsErrorMessage(RWSTRING("Unable to save video modes."));        
    }

#ifdef RWMETRICS
    RsMetricsOpen(Camera);
#endif

    NewVideoModeIndex = RwEngineGetCurrentVideoMode();

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

    if( Clump )
    {
        RpWorldRemoveClump(World, Clump);
        
        RpClumpDestroy(Clump);
    }

    if( Camera )
    {
        RpWorldRemoveCamera(World, Camera);

        CameraDestroy(Camera);
    }

    if( AmbientLight )
    {
        RpWorldRemoveLight(World, AmbientLight);
        
        RpLightDestroy(AmbientLight);
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
static RwBool
DisplayVideoModeInfo(RwInt32 mode, RwInt32 line)
{
    RwVideoMode videoMode;

    if( RwEngineGetVideoModeInfo(&videoMode, mode) )                 
    {
        RwChar caption[128];         
        
        line += 5;

        /*
         * Print out video mode information...
         */             
        RsSprintf(caption, RWSTRING("Dimensions:                  %3d x %3d"), 
            videoMode.width, videoMode.height);
    
        line--;
        RsCharsetPrint(Charset, caption, 0, -line, rsPRINTPOSBOTTOMRIGHT);

        RsSprintf(caption, RWSTRING("Depth:                             %3d"), 
            videoMode.depth);
    
        line--;
        RsCharsetPrint(Charset, caption, 0, -line, rsPRINTPOSBOTTOMRIGHT);
        
        if( videoMode.flags & rwVIDEOMODEEXCLUSIVE )
        {
            RsSprintf(caption, RWSTRING("Flags:                       EXCLUSIVE"));
            line--;
            RsCharsetPrint(Charset, caption, 0, -line, rsPRINTPOSBOTTOMRIGHT);
        }

        if( videoMode.flags & rwVIDEOMODEINTERLACE )
        {
            RsSprintf(caption, RWSTRING("Flags:                       INTERLACE"));
            line--;
            RsCharsetPrint(Charset, caption, 0, -line, rsPRINTPOSBOTTOMRIGHT);
        }

        if( videoMode.flags & rwVIDEOMODEFFINTERLACE )
        {        
            RsSprintf(caption, RWSTRING("Flags:                     FFINTERLACE"));
            line--;
            RsCharsetPrint(Charset, caption, 0, -line, rsPRINTPOSBOTTOMRIGHT);
        }

        if( videoMode.flags & rwVIDEOMODEFSAA0 )
        {        
            RsSprintf(caption, RWSTRING("Flags:  rwVIDEOMODE_PS2_FSAASHRINKBLIT"));
            line--;
            RsCharsetPrint(Charset, caption, 0, -line, rsPRINTPOSBOTTOMRIGHT);
        }

        if( videoMode.flags & rwVIDEOMODEFSAA1 )
        {        
            RsSprintf(caption, RWSTRING("Flags: rwVIDEOMODE_PS2_FSAAREADCIRCUIT"));
            line--;
            RsCharsetPrint(Charset, caption, 0, -line, rsPRINTPOSBOTTOMRIGHT);
        }
        
        return TRUE;
    }

    return FALSE;
}


/*
 *****************************************************************************
 */
static void 
DisplayOnScreenInfo(void)
{
    RwChar caption[256];
    RwInt32 lineNumber = 0;

    if( FPSOn )
    {
        RsSprintf(caption, RWSTRING("FPS: %03d"), FramesPerSecond);

        RsCharsetPrint(Charset, caption, 0, 0, rsPRINTPOSTOPRIGHT);
    }
    
    /* Display info about current video mode... */
    DisplayVideoModeInfo(RwEngineGetCurrentVideoMode(), lineNumber);

    lineNumber += 5;
    
    RsSprintf(caption, RWSTRING("Current video mode (%d)"), 
        RwEngineGetCurrentVideoMode());

    RsCharsetPrint(Charset, caption, 0, -lineNumber, rsPRINTPOSBOTTOMRIGHT);
    lineNumber++;

    lineNumber++;        

    /* Display info about requested video mode... */    
    DisplayVideoModeInfo(NewVideoModeIndex, lineNumber);

    lineNumber += 5;
        
    RsSprintf(caption, RWSTRING("New video mode     (%d)"), NewVideoModeIndex);

    RsCharsetPrint(Charset, caption, 0, -lineNumber, rsPRINTPOSBOTTOMRIGHT);
    lineNumber++;

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

    Render();

    return;
}


/*
 *****************************************************************************
 */
static RwBool 
SetupVideoMode(void)
{
    RwVideoMode newVideoMode;

    static RwInt32 useDefaultVideoMode = TRUE;
    
    if( !useDefaultVideoMode )
    {
        /* 
         * Set any video mode chosen by the user, 
         * otherwise the default will be used...
         */
        RwEngineSetVideoMode(NewVideoModeIndex);
    }
    else
    {
        useDefaultVideoMode = FALSE;
    }

    RwEngineGetVideoModeInfo(&newVideoMode, RwEngineGetCurrentVideoMode());
    
    RsGlobal.maximumWidth = newVideoMode.width;
    RsGlobal.maximumHeight = newVideoMode.height;

    return TRUE;
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

        case rsSELECTDEVICE:
        {
            SetupVideoMode();
          
            return rsEVENTPROCESSED;
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
