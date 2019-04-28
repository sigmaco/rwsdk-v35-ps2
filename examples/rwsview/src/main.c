
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
 * Copyright (c) 2003 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/****************************************************************************
 *
 * main.c
 *
 * Copyright (C) 2003 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: Illustrating loading RWS assets
 *
 ****************************************************************************/

#include "rwcore.h"
#include "rpworld.h"

#ifdef RWLOGO
#include "rplogo.h"
#endif

#include "rtcharse.h"
#include "rtpitexd.h"
#include "rtanim.h"
#include "rphanim.h"

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

static RwChar rwsFilename[] = "models/scene.rws";

RtCharset *Charset = NULL;

static RpWorld *world = (RpWorld *)NULL;

static RwUInt32 numClumps = 0;
static RpClump **clumpList = (RpClump **)NULL;

static RwCamera *currentCamera = (RwCamera *)NULL;

static RwUInt32 numAnims = 0;
static RtAnimAnimation **animList = (RtAnimAnimation **)NULL;
static RtAnimInterpolator **animInterpList = (RtAnimInterpolator **)NULL;
static RwUInt32 currentAnim = 0;


/*
 *****************************************************************************
 */
static RwBool 
LoadRWSAssets( void )
{    
    RwChar      *pathname;

    RwBool      success;
    
    RwStream    *rwsStream;


    pathname = RsPathnameCreate( rwsFilename );
    if ( NULL == pathname )
    {
        return FALSE;
    }

    success = TRUE;
    numClumps = 0;

    rwsStream = RwStreamOpen( rwSTREAMFILENAME,
                              rwSTREAMREAD,
                              pathname );
    if ( NULL != rwsStream )
    {
        RwChunkHeaderInfo   chunkHdrInfo;


        while ( NULL != RwStreamReadChunkHeaderInfo( rwsStream, &chunkHdrInfo ) )
        {
            switch ( chunkHdrInfo.type )
            {
            case rwID_WORLD:
                {
                    if ( NULL != world )
                    {
                        RsErrorMessage( RWSTRING( "Unable to load more than one world." ) );

                        success = FALSE;
                    }
                    else
                    {
                        world = RpWorldStreamRead( rwsStream );
                        if ( NULL == world )
                        {
                            RsErrorMessage( RWSTRING( "Unable to load world." ) );

                            success = FALSE;
                        }
                    }
                }
                break;

            case rwID_CLUMP:
                {
                    RpClump **newClumpList;


                    newClumpList = (RpClump **)RwMalloc( (numClumps + 1 ) * sizeof(RpClump *) );
                    if ( numClumps > 0 )
                    {
                        memcpy( (void *)newClumpList, (const void *)clumpList, numClumps * sizeof(RpClump *) );
                        RwFree( clumpList );
                    }

                    clumpList = newClumpList;

                    clumpList[numClumps] = RpClumpStreamRead( rwsStream );
                    if ( NULL == clumpList[numClumps] )
                    {
                        RsErrorMessage( RWSTRING( "Unable to load clump." ) );

                        success = FALSE;
                    }
                    else
                    {
                        numClumps += 1;
                    }
                }
                break;

            case rwID_HANIMANIMATION:
                {
                    RtAnimAnimation    **newAnimList;

                    RtAnimInterpolator  **newAnimInterpList;


                    newAnimList = (RtAnimAnimation **)RwMalloc( (numAnims + 1) * sizeof(RtAnimAnimation *) );
                    newAnimInterpList = (RtAnimInterpolator **)RwMalloc( (numAnims + 1) * sizeof(RtAnimInterpolator *) );
                    if ( numAnims > 0 )
                    {
                        memcpy( (void *)newAnimList,
                                (const void *)animList,
                                numAnims * sizeof(RtAnimAnimation *) );

                        memcpy( (void *)newAnimInterpList,
                                (const void *)animInterpList,
                                numAnims * sizeof(RtAnimInterpolator *) );

                        RwFree( animList );
                        RwFree( animInterpList );
                    }

                    animList = newAnimList;
                    animInterpList = newAnimInterpList;

                    animList[numAnims] = RtAnimAnimationStreamRead( rwsStream );
                    if ( NULL == animList[numAnims] )
                    {
                        RsErrorMessage( RWSTRING( "Unable to load animation." ) );

                        success = FALSE;
                    }
                    else
                    {
                        RwInt32                 typeID;

                        RtAnimInterpolatorInfo  *interpInfo;


                        typeID = RtAnimAnimationGetTypeID( animList[numAnims] );

                        if ( rpHANIMSTDKEYFRAMETYPEID != typeID )
                        {
                            RsErrorMessage( RWSTRING( "Unable to handle animation types other than HANIM." ) );
                            success = FALSE;
                        }
                        else
                        {
                            RwInt32 numNodes;


                            interpInfo = RtAnimGetInterpolatorInfo( typeID );

                            numNodes = RtAnimAnimationGetNumNodes( animList[numAnims] );

                            animInterpList[numAnims] = RtAnimInterpolatorCreate( numNodes, interpInfo->interpKeyFrameSize );
                        
                            RtAnimInterpolatorSetCurrentAnim( animInterpList[numAnims], animList[numAnims] );
                        
                            RtAnimInterpolatorSetCurrentTime( animInterpList[numAnims], 0.0f );
                        
                            numAnims += 1;
                        }
                    }
                }
                break;

            case rwID_PITEXDICTIONARY:
                {
                    RwTexDictionary *texDict;


                    texDict = RtPITexDictionaryStreamRead( rwsStream );
                    if ( NULL == texDict )
                    {
                        RsErrorMessage( RWSTRING( "Unable to load PI texture dictionary." ) );

                        success = FALSE;
                    }

                    RwTexDictionarySetCurrent( texDict );
                }
                break;

            default:
                RwStreamSkip( rwsStream, chunkHdrInfo.length );
            }

            if ( FALSE == success )
            {
                break;
            }
        }

        RwStreamClose( rwsStream, NULL );
    }

    RsPathnameDestroy( pathname );
    pathname = (RwChar *)NULL;

    return success;
}


/*
 *****************************************************************************
 */
static RwBool 
SetDefaultAssets( void )
{
    RwUInt32    i;


    if ( NULL == world )
    {
        RsErrorMessage( RWSTRING( "No world has been loaded." ) );
        return FALSE;
    }

    /* add each clump to the world */
    for ( i = 0; i < numClumps; i += 1 )
    {
        RpWorldAddClump( world, clumpList[i] );
    }

    return TRUE;
}



/*
 *****************************************************************************
 */
static void
CameraInitPosition(RwCamera *camera, RpWorld *world)
{
    const RwBBox *bBox;
    const RwV2d *viewWindow;
    RwV3d pos, right, at;
    RwFrame *frame;
    RwReal size, distance, rTemp;
    
    /* How big's the world? */
    bBox = RpWorldGetBBox(world);
    RwV3dSub(&pos, &(bBox->sup), &(bBox->inf));
    size = RwV3dLength(&pos);
    
    /* Rotate the camera so it looks straight down... */
    frame = RwCameraGetFrame(camera);
    RwFrameSetIdentity(frame);
    right = *RwMatrixGetRight(RwFrameGetMatrix(frame));
    RwFrameRotate(frame, &right, 90.0f, rwCOMBINEREPLACE);
    
    /* Move it to the center of the world... */
    RwV3dSub(&pos, &(bBox->sup), &(bBox->inf));
    RwV3dScale(&pos, &pos, 0.5f);
    RwV3dAdd(&pos, &pos, &(bBox->inf));
    RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);
    
    /* Back it up till it can see the whole world... */
    viewWindow = RwCameraGetViewWindow(camera);
    rTemp = viewWindow->x;
    if (viewWindow->y < rTemp) rTemp = viewWindow->y;
    distance = (1.0f*size) / rTemp;
    at = *RwMatrixGetAt(RwFrameGetMatrix(frame));
    RwV3dScale(&at, &at, -distance);
    RwFrameTranslate(frame, &at, rwCOMBINEPOSTCONCAT);
    
    /* Set the clip planes to give good precision... */
    RwCameraSetNearClipPlane(camera, RwRealMINVAL);
    RwCameraSetFarClipPlane( camera, (1.5f*size));
    RwCameraSetNearClipPlane(camera, (1.5f*size) / 1500.0f);
    
    return;
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
        RpWorldAddCamera(world, camera);
        CameraInitPosition( camera, world );

        return camera;
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

        RsGlobal.appName = RWSTRING("RenderWare Graphics RWSView Example");

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

    if( MenuOpen(TRUE, &ForegroundColor, &BackgroundColor) )
    {
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

    /* load RWS assets */
    if ( FALSE == LoadRWSAssets() )
    {
        RsErrorMessage( RWSTRING( "Unable to load RWS assets." ) );

        return FALSE;
    }
   
    /* set up default assets */
    if ( FALSE == SetDefaultAssets() )
    {
        RsErrorMessage( RWSTRING( "Unable to set default assets." ) );

        return FALSE;
    }

    currentCamera = CreateCamera(world);
    if( NULL == currentCamera )
    {
        RsErrorMessage(RWSTRING("Cannot create camera."));
        
        return FALSE;
    }
    
    if( !InitializeMenu() )
    {
        RsErrorMessage(RWSTRING("Error initializing menu."));

        return FALSE;
    }

#ifdef RWMETRICS
    RsMetricsOpen( currentCamera );
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

    if( currentCamera )
    {
        RpWorldRemoveCamera(world, currentCamera);
        
        CameraDestroy(currentCamera);
    }

    /* destroy anims and anim interpolators */
    if ( numAnims > 0 )
    {
        RwUInt32    i;


        for ( i = 0; i < numAnims; i += 1 )
        {
            RtAnimInterpolatorDestroy( animInterpList[i] );
            animInterpList[i] = (RtAnimInterpolator *)NULL;

            RtAnimAnimationDestroy( animList[i] );
            animList[i] = (RtAnimAnimation *)NULL;
        }

        RwFree( animInterpList );
        RwFree( animList );
    }
    
    /* destroy clumps */
    if ( numClumps > 0 )
    {
        RwUInt32    i;


        for ( i = 0; i < numClumps; i += 1 )
        {
            RpWorldRemoveClump( world, clumpList[i] );
            RpClumpDestroy( clumpList[i] );
            clumpList[i] = (RpClump *)NULL;
        }

        RwFree( clumpList );
    }

    /* destroy the world */
    if ( NULL != world )
    {
        RpWorldDestroy( world );
        world = (RpWorld *)NULL;
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

    if ( FALSE == RtAnimInitialize() )
    {
        return FALSE;
    }

    if ( FALSE == RpHAnimPluginAttach() )
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
    RwCameraClear( currentCamera,
                   &BackgroundColor,
                   rwCAMERACLEARZ | rwCAMERACLEARIMAGE );

    if ( NULL != RwCameraBeginUpdate( currentCamera ) )
    {
        if( MenuGetStatus() != HELPMODE )
        {
            RpWorldRender( world );
            
            DisplayOnScreenInfo();
        }

        MenuRender( currentCamera, NULL );

#ifdef RWMETRICS
        RsMetricsRender();
#endif

        RwCameraEndUpdate( currentCamera );
    }

    /* 
     * Display camera's raster...
     */
    RsCameraShowRaster( currentCamera );

    FrameCounter++;

    return;
}


/*
 *****************************************************************************
 */
static RpAtomic*
atomicApplyAnim(RpAtomic * atomic, void *data __RWUNUSED__)
{
    RwInt32                     typeID;
    
    RtAnimKeyFrameApplyCallBack animApplyCB;
    
    RwFrame                     *frame;
    
    RwMatrix                    *frameLTM;
    
    RpHAnimKeyFrame             *animKeyFrame;
    
    RwUInt32                    j;
    
    
    typeID = RtAnimAnimationGetTypeID( animList[currentAnim] );
    animApplyCB = RtAnimGetInterpolatorInfo(typeID)->keyFrameApplyCB;
    
    frame = RpAtomicGetFrame( atomic );
    frameLTM = RwFrameGetMatrix( frame );
    
    /* apply the animation application callback to the atomic's matrix */
    for ( j = 0; j < RtAnimAnimationGetNumNodes( animList[currentAnim] ); j += 1 )
    {
        animKeyFrame = (RpHAnimKeyFrame *)rtANIMGETINTERPFRAME( animInterpList[currentAnim], j);
        animApplyCB( (void *)frameLTM, (void *)animKeyFrame );
    }

    /* don't forget to mark the atomic's frame as dirty now */
    RwFrameUpdateObjects( frame );

    return atomic;
}


/*
 *****************************************************************************
 */
static RpLight *
lightApplyAnim(RpLight * light, void *data __RWUNUSED__)
{
    RwInt32                     typeID;
    
    RtAnimKeyFrameApplyCallBack animApplyCB;
    
    RwFrame                     *frame;
    
    RwMatrix                    *frameLTM;
    
    RpHAnimKeyFrame             *animKeyFrame;
    
    RwUInt32                    j;
    
    
    typeID = RtAnimAnimationGetTypeID( animList[currentAnim] );
    animApplyCB = RtAnimGetInterpolatorInfo(typeID)->keyFrameApplyCB;
    
    frame = RpLightGetFrame( light );
    frameLTM = RwFrameGetMatrix( frame );
    
    /* apply the animation application callback to the light's matrix */
    for ( j = 0; j < RtAnimAnimationGetNumNodes( animList[currentAnim] ); j += 1 )
    {
        animKeyFrame = (RpHAnimKeyFrame *)rtANIMGETINTERPFRAME( animInterpList[currentAnim], j);
        animApplyCB( (void *)frameLTM, (void *)animKeyFrame );
    }
    
    /* don't forget to mark the atomic's frame as dirty now */
    RwFrameUpdateObjects( frame );
    
    return light;
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
     * Update any animations here...
     */
    for ( currentAnim = 0; currentAnim < numAnims; currentAnim += 1 )
    {
        RtAnimInterpolatorAddAnimTime( animInterpList[currentAnim], deltaTime );
        
        RpClumpForAllAtomics( clumpList[currentAnim], atomicApplyAnim, (void *)NULL );
        RpClumpForAllLights( clumpList[currentAnim], lightApplyAnim, (void *)NULL );
    }

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
            CameraSize( currentCamera,
                        (RwRect *)param, 
                        DEFAULT_VIEWWINDOW,
                        DEFAULT_ASPECTRATIO );

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
