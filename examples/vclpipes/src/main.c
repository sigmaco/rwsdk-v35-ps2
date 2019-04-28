#include "rwcore.h"
#include "rpworld.h"
#include "rppds.h"
#include "rpskin.h"
#include "rphanim.h"
#include "rprandom.h"
#include "rtbmp.h"

#include "eetypes.h"
#include "libgraph.h"

#ifdef RWLOGO
#include "rplogo.h"
#endif

#include "rtcharse.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"
#include "camera.h"

#include "dot3.h"
#include "specular.h"
#include "utils.h"

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
static RwBool AnimOn = FALSE;
static RwBool SpinClumpOn = TRUE;
static RwBool SpinLightOn = FALSE;
static RwInt32 Lighting = 1;
static RwInt32 Effect = 0;

static RwCullMode CullMode = rwCULLMODECULLBACK;

static RwReal LightRadius = 2.0f;
static RwReal ConeAngle = 0.5f;
static RwReal SpecularIntensity = 1.0f;
static RwReal SpecularPower = 5.0f;

static RwInt32 FrameCounter = 0;
static RwInt32 FramesPerSecond = 0;

static RwRGBA ForegroundColor = {200, 200, 200, 255};
static RwRGBA BackgroundColor = { 64,  64,  64,   0};

static RwV3d Xaxis = {1.0f, 0.0f, 0.0f};
static RwV3d Yaxis = {0.0f, 1.0f, 0.0f};
static RwV3d LightPos = {0.0f, 0.0f, -1.0f};
static RwV3d CameraPos = {0.25f, 0.0f, -1.1f};
static RwV3d ClumpPos = {0.0f, 0.0f, 0.0f};

static RpWorld *World = NULL;
RwCamera *Camera = NULL;
static RpClump *Clump = NULL;
static RtCharset *Charset = NULL;
static RpHAnimHierarchy *Hierarchy = NULL;
static RpHAnimAnimation *Anim = NULL;
static RwRaster *Dot3Raster = NULL;
static RwReal ClumpRadius;
static RwTexture *Textures[2];
static RpLight *AmbientLight;
static RpLight *DirectionalLight;
static RpLight *PointLight;


/*
 *****************************************************************************
 */
static RpAtomic *
AtomicUpdateBBox(RpAtomic *atomic, void *data)
{
    RpGeometry *geometry;
    RwBBox *bbox;
    RwUInt32 i;
    
    geometry = atomic->geometry;
    bbox = (RwBBox *)data;
    
    for (i = 0; i < (RwUInt32)RpGeometryGetNumVertices(geometry); i++)
    {
        RwV3d *pos;
        
        pos = geometry->morphTarget->verts + i;
        
        bbox->inf.x = RwRealMin2(bbox->inf.x, pos->x);
        bbox->inf.y = RwRealMin2(bbox->inf.y, pos->y);
        bbox->inf.z = RwRealMin2(bbox->inf.z, pos->z);
        bbox->sup.x = RwRealMax2(bbox->sup.x, pos->x);
        bbox->sup.y = RwRealMax2(bbox->sup.y, pos->y);
        bbox->sup.z = RwRealMax2(bbox->sup.z, pos->z);
    }
    
    return atomic;
}

 
/*
 *****************************************************************************
 */
static RwReal
ClumpGetRadius(RpClump *clump)
{
    RwBBox bbox;
    RwReal rr, r;
    RwV3d size;
    
    bbox.inf.x = bbox.inf.y = bbox.inf.z = RwRealMAXVAL;
    bbox.sup.x = bbox.sup.y = bbox.sup.z = RwRealMINVAL;
    
    RpClumpForAllAtomics(clump, AtomicUpdateBBox, &bbox);
    RwV3dSub(&size, &bbox.sup, &bbox.inf);
    
    rr = size.x * size.x + size.y * size.y + size.z * size.z;
    rwSqrt(&r, rr);
    
    return r;
} 
 
 
/*
 *****************************************************************************
 */
static RpClump *
ClumpLoad(const RwChar *filename)
{
    RpClump *clump = NULL; 
    RwChar *path = RsPathnameCreate(filename);    
    RwStream *stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, path);
    RsPathnameDestroy(path);

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
static RpLight *
CreateLight(RpLightType type, RwReal colorScale)
{
    RpLight *light = RpLightCreate(type);
    
    if( NULL != light )
    {
        RwFrame *frame = RwFrameCreate();
        
        if( NULL != frame )
        {
            RwV3d pos;
            RwRGBAReal color;
            RwReal rotY, rotX;

            color.red = (RwReal)RpRandom();
            color.green = (RwReal)RpRandom();
            color.blue = (RwReal)RpRandom();
            colorScale 
                /= RwRealMax2(color.red, RwRealMax2(color.green, color.blue));
            color.red *= colorScale;
            color.green *= colorScale;
            color.blue *= colorScale;            
            
            rotX = 360.0f / RPRANDMAX * RpRandom();        
            rotY = 360.0f / RPRANDMAX * RpRandom();        
            
            RwV3dScale(&pos, &LightPos, ClumpRadius);
            RwFrameTranslate(frame, &pos, rwCOMBINEREPLACE);
            RwFrameRotate(frame, &Yaxis, 180.0f + rotY, rwCOMBINEPOSTCONCAT);
            RwFrameRotate(frame, &Xaxis, rotX, rwCOMBINEPOSTCONCAT);
            RpLightSetFrame(light, frame);
            RpLightSetColor(light, &color);
            RpLightSetRadius(light, ClumpRadius * LightRadius);
            RpLightSetConeAngle(light, ConeAngle);
            
            return light;
        }
        
        RpLightDestroy(light);
    }
    
    return NULL;
} 
 
 
/*
 *****************************************************************************
 */
static RpWorld *
CreateWorld(void)
{
    RpWorld *world;
    RwBBox bb;

    bb.inf.x = bb.inf.y = bb.inf.z = -1000.0f;
    bb.sup.x = bb.sup.y = bb.sup.z = 1000.0f;

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
        RwV3d pos;
        
        RwV3dScale(&pos, &CameraPos, ClumpRadius);
        RwFrameTranslate(RwCameraGetFrame(camera), &pos, rwCOMBINEREPLACE);

        RwCameraSetNearClipPlane(camera, 0.1f * ClumpRadius);
        RwCameraSetFarClipPlane(camera, 10.0f * ClumpRadius);

        RpWorldAddCamera(world, camera);

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

        RsGlobal.appName 
            = RWSTRING("RenderWare Graphics VCL Pipelines Example");

        RsGlobal.maxFPS = 120;

        return TRUE;
    }

    return FALSE;
}


/*
 *****************************************************************************
 */
static RwBool
PipelineCB(RwBool testEnable)
{
    static RpPDSPipeID atomicPipes[2] = {
        rwPDS_VCL_Generic_AtmPipeID,
        rwPDS_VCL_Skin_AtmPipeID
    };
    static RpPDSPipeID materialPipes[8] = {
        rwPDS_VCL_ADLDot3_MatPipeID,
        rwPDS_VCL_ADLSkinDot3_MatPipeID,
        rwPDS_VCL_APLDot3_MatPipeID,
        rwPDS_VCL_APLSkinDot3_MatPipeID,
        rwPDS_VCL_ADLSpec_MatPipeID,
        rwPDS_VCL_ADLSkinSpec_MatPipeID,
        rwPDS_VCL_APLSpec_MatPipeID,
        rwPDS_VCL_APLSkinSpec_MatPipeID
    };
    
    if (testEnable)
    {
        return TRUE;
    }
    
    /* Different pipelines have different instanced formats so we need to
       destroy any instanced data. */
    RwResourcesEmptyArena();
    
    RpAtomicSetDefaultPipeline(RpPDSGetPipe(atomicPipes[AnimOn]));
    RpMaterialSetDefaultPipeline(RpPDSGetPipe(
        materialPipes[AnimOn | (Lighting << 1) | (Effect << 2)]));
    
    return TRUE;
}


/*
 *****************************************************************************
 */
static RpMaterial *
MaterialSetup(RpMaterial *material, void *data)
{
    RpMaterialSetTexture(material, (RwTexture *)data);
    RpMaterialSetPipeline(material, NULL);
    
    return material;
}

 
/*
 *****************************************************************************
 */
static RpAtomic *
AtomicSetup(RpAtomic *atomic, void *data)
{
    RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), MaterialSetup, data);
    RpAtomicSetPipeline(atomic, NULL);
    
    return atomic;
} 


/*
 *****************************************************************************
 */
static RwBool
EffectCB(RwBool testEnable)
{
    if (testEnable)
    {
        return TRUE;
    }
    
    RpClumpForAllAtomics(Clump, AtomicSetup, Textures[Effect]);
    
    return PipelineCB(FALSE);
} 


/*
 *****************************************************************************
 */
static RpLight *
LightDestroy(RpLight *light, void *data)
{
    RwFrame *frame;
    
    RpWorldRemoveLight((RpWorld *)data, light);
    frame = RpLightGetFrame(light);
    RpLightDestroy(light);

    if( frame )
    {
        RwFrameDestroy(frame);
    }    
    
    return light;
}


/*
 *****************************************************************************
 */
static RwBool
LightCB(RwBool testEnable)
{
    if (testEnable)
    {
        return TRUE;
    }
    
    RpWorldForAllLights(World, LightDestroy, World);
    
    RpRandomSeed(0x5ad16b8e);
    if (Lighting)
    {
        PointLight = CreateLight(rpLIGHTPOINT, 1.0f);
        RpWorldAddLight(World, PointLight);
    }
    else
    {
        DirectionalLight = CreateLight(rpLIGHTDIRECTIONAL, 0.8f);
        RpWorldAddLight(World, DirectionalLight);
    }
    
    AmbientLight = CreateLight(rpLIGHTAMBIENT, 0.4f);
    RpWorldAddLight(World, AmbientLight);
    
    return PipelineCB(FALSE);
}


/*
 *****************************************************************************
 */
static RpLight *
LightUpdate(RpLight *light, void *data __RWUNUSED__)
{
    RpLightSetRadius(light, LightRadius * ClumpRadius);
    RpLightSetConeAngle(light, ConeAngle);
    
    return light;
} 


/*
 *****************************************************************************
 */
static RwBool
LightUpdateCB(RwBool testEnable)
{
    if (testEnable)
    {
        return Lighting;
    }

    RpWorldForAllLights(World, LightUpdate, NULL);
    
    return TRUE;    
}


/*
 *****************************************************************************
 */
static RwBool
SpecularCB(RwBool testEnable __RWUNUSED__)
{
    return Effect;
} 


/*
 *****************************************************************************
 */
static RwBool 
InitializeMenu(void)
{    
    static RwChar fpsLabel[] = RWSTRING("FPS_F");
    static RwChar effectLabel[] = RWSTRING("Effect");
    static RwChar lightingLabel[] = RWSTRING("Lighting");
    static RwChar animLabel[] = RWSTRING("Animation_A");
    static RwChar spinClumpLabel[] = RWSTRING("Spin clump_C");
    static RwChar spinLightLabel[] = RWSTRING("Spin lights_S");
    static RwChar radiusLabel[] = RWSTRING("Light radius");
    static RwChar sIntensLabel[] = RWSTRING("Specular intensity");
    static RwChar sPowerLabel[] = RWSTRING("Specular power");
    static RwChar cullModeLabel[] = RWSTRING("Cull mode");
    
    static const RwChar *cullModeStrings[] = {RWSTRING("None"), RWSTRING("Back"), RWSTRING("Front")};
    static const RwChar *effectStrings[] = {RWSTRING("Dot3 bump mapping"), RWSTRING("Specular")};
    static const RwChar *lightingStrings[] = {RWSTRING("Directional"), RWSTRING("Point")};

    if( MenuOpen(TRUE, &ForegroundColor, &BackgroundColor) )
    {
        MenuAddEntryBool(fpsLabel, &FPSOn, NULL);
        MenuAddEntryInt(effectLabel, &Effect, EffectCB, 0, 1, 1, effectStrings);
        MenuAddEntryBool(animLabel, &AnimOn, PipelineCB);
        MenuAddEntryBool(spinClumpLabel, &SpinClumpOn, NULL);
        MenuAddEntryBool(spinLightLabel, &SpinLightOn, NULL);
        MenuAddEntryInt(cullModeLabel, (RwInt32 *)&CullMode, NULL, 
            rwCULLMODECULLNONE, rwCULLMODECULLFRONT, 1, cullModeStrings);
        MenuAddSeparator();
        MenuAddEntryInt(lightingLabel, &Lighting, LightCB, 0, 1, 1, lightingStrings);
        MenuAddEntryReal(radiusLabel, &LightRadius, LightUpdateCB, 0.5f, 5.0f, 0.1f);
        MenuAddEntryReal(sIntensLabel, &SpecularIntensity, SpecularCB, 0.0f, 1.99f, 0.01f);
        MenuAddEntryReal(sPowerLabel, &SpecularPower, SpecularCB, 1.0f, 20.0f, 1.0f);

        return TRUE;
    }

    return FALSE;
}


/*
 *****************************************************************************
 */
static RwFrame *
FrameGetHierarchy(RwFrame *frame, void *data)
{    
    RpHAnimHierarchy *hierarchy;

    hierarchy = RpHAnimFrameGetHierarchy(frame);
    if (NULL == hierarchy)
    {
        RwFrameForAllChildren(frame, FrameGetHierarchy, data);

        return frame;
    }

    *(void **)data = (void *)hierarchy;

    return NULL;
}


/*
 *****************************************************************************
 */
static RpAtomic *
AtomicSetSkinHierarchy(RpAtomic *atomic, void *data)
{
    RpSkinAtomicSetHAnimHierarchy(atomic, (RpHAnimHierarchy *)data);
    
    return atomic;
}


/*
 *****************************************************************************
 */
static RtAnimAnimation *
AnimationLoad(const RwChar *file)
{
    RtAnimAnimation *anim = NULL;
    RwChar *path = NULL;

    path = RsPathnameCreate(file);
    anim = RtAnimAnimationRead(path);
    RsPathnameDestroy(path);

    return anim;
}


/*
 *****************************************************************************
 */
static RwBool 
Initialize3D(void *param)
{
    RwV3d pos;
    
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
    
    /*
     * Create an empty world if not loading a BSP...
     */
    World = CreateWorld();
    if( World == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create world."));

        return FALSE;
    }
    
    /* Set path to load textures from */
    RwImageSetPath(RWSTRING("models/textures/"));
    
    /* Load and process normal map for dot3 effect. Normals maps for this can
       be generated using the rtnormmap toolkit. */
    Textures[0] = RwTextureRead(RWSTRING("normal.png"), NULL);
    Dot3ProcessRaster(RwTextureGetRaster(Textures[0]));
    (void)RwTextureSetFilterMode(Textures[0], rwFILTERLINEAR);
    
    /* Load and process diffuse and specular maps for specular effect. The
       processing scales the specular map (stored in the alpha channel) back
       into the range [0,255] (it is scaled into the range [0,128] on load).*/
    Textures[1] 
        = RwTextureRead(RWSTRING("marble.png"), RWSTRING("specular.png"));
    SpecularProcessRaster(RwTextureGetRaster(Textures[1]));
    (void)RwTextureSetFilterMode(Textures[1], rwFILTERLINEAR);

    /* Load model to render. Both effects require the geometry to have normals
       and at least 1 set of texture coordinates. */
    Clump = ClumpLoad(RWSTRING("models/hand.dff"));
    
    /* Setup animation. This code requires the model to have skinning data. */
    RwFrameForAllChildren(
        RpClumpGetFrame(Clump), FrameGetHierarchy, &Hierarchy);
    Hierarchy->flags |= rpHANIMHIERARCHYLOCALSPACEMATRICES;
    RpClumpForAllAtomics(Clump, AtomicSetSkinHierarchy, Hierarchy);
    Anim = AnimationLoad(RWSTRING("models/hand.anm"));    
    RpHAnimHierarchySetCurrentAnim(Hierarchy, Anim);    
    RpHAnimHierarchyUpdateMatrices(Hierarchy);

    /* Position clump in world. */
    ClumpRadius = ClumpGetRadius(Clump);
    RwFrameRotate(RpClumpGetFrame(Clump), &Xaxis, 90.0f, rwCOMBINEREPLACE);
    RwV3dScale(&pos, &ClumpPos, ClumpRadius);
    RwFrameTranslate(RpClumpGetFrame(Clump), &pos, rwCOMBINEPOSTCONCAT);
    RpWorldAddClump(World, Clump);
    
    /*
     * Create a camera using the democom way...
     */
    Camera = CreateCamera(World);
    if( Camera == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create camera."));

        return FALSE;
    }
    
    if( !InitializeMenu() )
    {
        RsErrorMessage(RWSTRING("Error initializing menu."));

        return FALSE;
    }
    
    /* Create palette for dot3 effect */    
    Dot3Raster = Dot3CreateRaster();

    /* Set initial lighting and effect. */
    LightCB(FALSE);
    EffectCB(FALSE);    
    
#ifdef RWMETRICS
    RsMetricsOpen(Camera);
#endif

    return TRUE;
}


/*
 *****************************************************************************
 */
static RpClump *
ClumpDestroy(RpClump *clump, void *data)
{
    RpWorldRemoveClump((RpWorld *)data, clump);
    RpClumpDestroy(clump);
    
    return clump;
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
    
    if( Camera )
    {
        RpWorldRemoveCamera(World, Camera);

        /*
         * This assumes the camera was created the democom way...
         */
        CameraDestroy(Camera);
    }
    
    if( World )
    {
        RpWorldForAllClumps(World, ClumpDestroy, World);
        RpWorldForAllLights(World, LightDestroy, World);
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
    if( !RpWorldPluginAttach() )
    {
        return FALSE;
    }
    if( !RpHAnimPluginAttach() )
    {
        return FALSE;
    }
    if( !RpSkinPluginAttach() )
    {
        return FALSE;
    }
    if( !RtAnimInitialize() )
    {
        return FALSE;
    }
    if( !RpRandomPluginAttach() )
    {
        return FALSE;
    }
    
    rwPDS_VCL_Generic_AtmPipeRegister();
    rwPDS_VCL_Skin_AtmPipeRegister();
    rwPDS_VCL_ADLDot3_MatPipeRegister();
    rwPDS_VCL_ADLSkinDot3_MatPipeRegister();
    rwPDS_VCL_APLDot3_MatPipeRegister();
    rwPDS_VCL_APLSkinDot3_MatPipeRegister();
    rwPDS_VCL_ADLSpec_MatPipeRegister();
    rwPDS_VCL_ADLSkinSpec_MatPipeRegister();
    rwPDS_VCL_APLSpec_MatPipeRegister();
    rwPDS_VCL_APLSkinSpec_MatPipeRegister();

#ifdef RWLOGO
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
DisplayOnScreenInfo(RwCamera * __RWUNUSED__ camera)
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
            /* Select face culling mode. */
            RwRenderStateSet(rwRENDERSTATECULLMODE, (void *)CullMode);
            
            /* Turn off alpha test by setting test method to always */
            RpSkyRenderStateSet(rpSKYRENDERSTATEATEST_1, (void *)2);

            /* Set blend mode for specular effect. Dot3 effect pipelines set
               the blend modes. */
            if (Effect)
            {            
                RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDONE);
                RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDZERO);
            }

            /* Specular effect requires color clamping on.
               Dot3 effect requires color clamping off. */
            SetGSReg(SCE_GS_COLCLAMP, SCE_GS_SET_COLCLAMP(Effect));
            
            /* Upload lighting information */            
            if (Effect)
            {
                if (Lighting)
                {
                    RpPDS_Specular_APL_LightingUpload(
                        AmbientLight, PointLight, SpecularPower);
                }
                else
                {
                    RpPDS_Specular_ADL_LightingUpload(
                        AmbientLight, DirectionalLight, SpecularPower);
                }
            } 
            else
            {
                if (Lighting)
                {
                    RpPDS_Dot3_APL_LightingUpload(PointLight);
                }
                else
                {
                    RpPDS_Dot3_ADL_LightingUpload(DirectionalLight);
                }
            }    
            
            /* Render model */
            RpWorldRender(World);
          
            /* Post process frame buffer data */
            if (Effect)
            {                
                SpecularProcess(RwCameraGetRaster(Camera), SpecularIntensity);
            }
            else
            {                
                Dot3Process(RwCameraGetRaster(Camera), Dot3Raster, 
                    (skyZbuf_1 & 0x1ff) << 5, Lighting);
            }

            DisplayOnScreenInfo(Camera);
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
static RpLight *
LightSpin(RpLight *light, void *data)
{
    RwV3d axis;
    RwReal speed;
    
    speed = 10.0f + 50.0f / RPRANDMAX * RpRandom();
    axis.x = -1.0f + 2.0f / RPRANDMAX * RpRandom();
    axis.y = -1.0f + 2.0f / RPRANDMAX * RpRandom();
    axis.z = -1.0f + 2.0f / RPRANDMAX * RpRandom();
    RwV3dNormalize(&axis, &axis);
    RwFrameRotate(RpLightGetFrame(light), 
        &axis, speed * *(RwReal *)data, rwCOMBINEPOSTCONCAT);
    
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

    if (AnimOn)
    {
        RpHAnimHierarchyAddAnimTime(Hierarchy, deltaTime);
        RpHAnimHierarchyUpdateMatrices(Hierarchy);
    }
    
    if (SpinClumpOn)
    {
        RwFrameRotate(RpClumpGetFrame(Clump), 
            &Xaxis, 57.8f * deltaTime, rwCOMBINEPOSTCONCAT);
    }
    
    RpRandomSeed(0x4bef16a7);
    if (SpinLightOn)
    {
        RpWorldForAllLights(World, LightSpin, &deltaTime);
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
