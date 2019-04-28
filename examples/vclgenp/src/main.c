#include "rwcore.h"
#include "rpworld.h"
#include "rppds.h"
#include "rpmorph.h"
#include "rpskin.h"
#include "rphanim.h"
#include "rpmatfx.h"
#include "rprandom.h"

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
static RwBool TrueClipOn = TRUE;
static RwInt32 FaceCullMode = rwCULLMODECULLNONE;
static RwBool FogOn = FALSE;
static RwBool AnimOn = FALSE;
static RwBool UVAnimOn = FALSE;
static RwBool TestOn = TRUE;
static RwBool MorphOn = FALSE;
static RwBool DualOn = TRUE;
static RwBool SpinClumpOn = FALSE;
static RwBool SpinLightOn = TRUE;

static RwInt32 NumAmbient = 1;
static RwInt32 NumDirectional = 1;
static RwInt32 NumPoint = 0;
static RwInt32 NumSpot = 0;
static RwInt32 NumSpotSoft = 0;

static RwReal LightRadius = 2.0f;
static RwReal ConeAngle = 0.5f;

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
static RwMatrix UVBaseTransform;
static RwMatrix UVDualTransform;
static RwReal ClumpRadius;

typedef struct _DualTextures
{
    RwTexture *pass1;
    RwTexture *pass2;
} DualTextures;


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
    
    /* calculate bounding radius of clump (not necessarily tight) */
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
            colorScale *= 1.0f / RwRealMax2(color.red, 
                RwRealMax2(color.green, color.blue));
            color.red *= colorScale;
            color.green *= colorScale;
            color.blue *= colorScale;            
            
            rotX = 360.0f / RPRANDMAX * RpRandom();        
            rotY = 360.0f / RPRANDMAX * RpRandom();        
            
            RwV3dScale(&pos, &LightPos, ClumpRadius);
            RwFrameTranslate(frame, &pos, rwCOMBINEREPLACE);
            RwFrameRotate(frame, &Yaxis, rotY, rwCOMBINEPOSTCONCAT);
            RwFrameRotate(frame, &Xaxis, rotX, rwCOMBINEPOSTCONCAT);
            RpLightSetFrame(light, frame);
            RpLightSetColor(light, &color);
            RpLightSetRadius(light, LightRadius * ClumpRadius);
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

        RsGlobal.appName = RWSTRING("RenderWare Generic VCL Pipeline Example");

        RsGlobal.maxFPS = 120;

        return TRUE;
    }

    return FALSE;
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
    RwInt32 i;
    
    if (testEnable)
    {
        return TRUE;
    }
    
    /* destroy any existing lights */
    RpWorldForAllLights(World, LightDestroy, World);
    
    /* create lights */
    RpRandomSeed(0xe5a716bc);
    for (i = 0; i < NumSpot; i++)
    {
        RpWorldAddLight(World, CreateLight(rpLIGHTSPOT, 1.0f));
    }
    RpRandomSeed(0x5ad16b8e);
    for (i = 0; i < NumSpotSoft; i++)
    {
        RpWorldAddLight(World, CreateLight(rpLIGHTSPOTSOFT, 1.0f));
    }
    RpRandomSeed(0xdc16b17f);
    for (i = 0; i < NumPoint; i++)
    {
        RpWorldAddLight(World, CreateLight(rpLIGHTPOINT, 1.0f));
    }
    RpRandomSeed(0x8e6a465c);
    for (i = 0; i < NumDirectional; i++)
    {
        RpWorldAddLight(World, CreateLight(rpLIGHTDIRECTIONAL, 0.8f));
    }
    RpRandomSeed(0x53fcba71);
    for (i = 0; i < NumAmbient; i++)
    {
        RpWorldAddLight(World, CreateLight(rpLIGHTAMBIENT, 0.4f));
    }
    
    return TRUE;
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
        return TRUE;
    }

    RpWorldForAllLights(World, LightUpdate, NULL);
    
    return TRUE;    
}


/*
 *****************************************************************************
 */
static RwBool
TestCB(RwBool testEnable)
{
    static RpPDSPipeID AtomicPipes[2] = {
        rwPDS_G3_Generic_AtmPipeID,
        rwPDS_G3_Skin_AtmPipeID
    };
    static RpPDSPipeID MaterialPipes[8] = {
        rwPDS_G3_Uva_MatPipeID,
        rwPDS_G3_SkinUva_MatPipeID,
        rwPDS_G3_DupUva_MatPipeID,
        rwPDS_G3_SkinDupUva_MatPipeID,
        rwPDS_VCL_Uva_MatPipeID,
        rwPDS_VCL_SkinUva_MatPipeID,
        rwPDS_VCL_DupUva_MatPipeID,
        rwPDS_VCL_SkinDupUva_MatPipeID
    };
    
    if (testEnable)
    {
        return TRUE;
    }
    
    /* destroy an instanced data as format may be different for new pipeline */
    RwResourcesEmptyArena();
    
    RpAtomicSetDefaultPipeline(RpPDSGetPipe(AtomicPipes[AnimOn]));
    RpMaterialSetDefaultPipeline(RpPDSGetPipe(
        MaterialPipes[AnimOn | (DualOn << 1) | (TestOn << 2)]));
    
    return TRUE;
}


/*
 *****************************************************************************
 */
static RwBool 
InitializeMenu(void)
{    
    static RwChar fpsLabel[] = RWSTRING("FPS_F");
    static RwChar trueClipLabel[] = RWSTRING("True clipper_T");
    static RwChar faceCullLabel[] = RWSTRING("Face cull mode");
    static RwChar fogLabel[] = RWSTRING("Fog");
    static RwChar animLabel[] = RWSTRING("Animation_A");
    static RwChar uvAnimLabel[] = RWSTRING("UV Animation_U");
    static RwChar dualLabel[] = RWSTRING("Dual pass_D");
    static RwChar morphLabel[] = RWSTRING("Morph_M");
    static RwChar spinClumpLabel[] = RWSTRING("Spin clump_C");
    static RwChar spinLightLabel[] = RWSTRING("Spin lights_S");
    static RwChar testLabel[] = RWSTRING("Pipeline");
    static RwChar ambientLabel[] = RWSTRING("Ambient lights");
    static RwChar directionalLabel[] = RWSTRING("Directional lights");
    static RwChar pointLabel[] = RWSTRING("Point lights");
    static RwChar spotLabel[] = RWSTRING("Spot lights");
    static RwChar spotSoftLabel[] = RWSTRING("Soft spot lights");
    static RwChar radiusLabel[] = RWSTRING("Light radius");
    static RwChar coneLabel[] = RWSTRING("Cone angle");
    
    static const RwChar *pipelineStrings[2] = {
        RWSTRING("G3"), 
        RWSTRING("VCL")
    };
    
    static const RwChar *cullModeStrings[3] = {
        RWSTRING("None"),
        RWSTRING("Back"),
        RWSTRING("Front")
    };        
    
    if( MenuOpen(TRUE, &ForegroundColor, &BackgroundColor) )
    {
        MenuAddEntryBool(fpsLabel, &FPSOn, NULL);
        MenuAddEntryBool(trueClipLabel, &TrueClipOn, NULL);
        MenuAddEntryInt(faceCullLabel, &FaceCullMode, NULL, 
            rwCULLMODECULLNONE, rwCULLMODECULLFRONT, 1, cullModeStrings);
        MenuAddEntryBool(fogLabel, &FogOn, NULL);
        MenuAddEntryBool(animLabel, &AnimOn, TestCB);
        MenuAddEntryBool(dualLabel, &DualOn, TestCB);
        MenuAddEntryBool(morphLabel, &MorphOn, NULL);
        MenuAddEntryBool(uvAnimLabel, &UVAnimOn, NULL);
        MenuAddEntryBool(spinClumpLabel, &SpinClumpOn, NULL);
        MenuAddEntryBool(spinLightLabel, &SpinLightOn, NULL);
        MenuAddEntryInt(testLabel, &TestOn, TestCB, 0, 1, 1, pipelineStrings);
        MenuAddSeparator();
        MenuAddEntryInt(ambientLabel, &NumAmbient, LightCB, 0, 2, 1, NULL);
        MenuAddEntryInt(directionalLabel, &NumDirectional, 
            LightCB, 0, 4, 1, NULL);
        MenuAddEntryInt(pointLabel, &NumPoint, LightCB, 0, 3, 1, NULL);
        MenuAddEntryInt(spotLabel, &NumSpot, LightCB, 0, 1, 1, NULL);
        MenuAddEntryInt(spotSoftLabel, &NumSpotSoft, LightCB, 0, 1, 1, NULL);
        MenuAddSeparator();
        MenuAddEntryReal(radiusLabel, &LightRadius, 
            LightUpdateCB, 0.5f, 5.0f, 0.1f);
        MenuAddEntryReal(coneLabel, &ConeAngle, 
            LightUpdateCB, 0.05f, 1.55f, 0.05f);

        return TRUE;
    }

    return FALSE;
}


/*
 *****************************************************************************
 */
static RpAtomic *
AtomicSetupInterpolators(RpAtomic *atomic, 
                         void *data __RWUNUSED__)
{
    RpGeometry *geometry;
    
    geometry = RpAtomicGetGeometry(atomic);

    /* if geometry doesn't have at least 2 morph targets create a second morph
       target to test morphing code with. */
    if (RpGeometryGetNumMorphTargets(geometry) < 2)
    {
        RpMorphTarget *mt1, *mt2;
        RwUInt32 i;
        
        RpGeometryAddMorphTarget(geometry);
        mt1 = RpGeometryGetMorphTarget(geometry, 0);
        mt2 = RpGeometryGetMorphTarget(geometry, 1);
        
        for (i = 0; i < (RwUInt32)geometry->numVertices; i++)
        {
            RwV3dScale(mt2->verts + i, mt1->verts + i, 0.5f);
            mt2->normals[i] = mt1->normals[i];
        }
    }
    
    /* create morphing interpolator to morph between the first 2 targets */
    RpMorphGeometryCreateInterpolators(geometry, 1);
    RpMorphGeometrySetInterpolator(geometry, 0, 0, 1, 1.0f);
    RpMorphAtomicSetCurrentInterpolator(atomic, 0);

    return atomic;
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
static RpMaterial *
MaterialSetup(RpMaterial *material, void *data)
{
    RpMatFXMaterialSetEffects(material, rpMATFXEFFECTDUALUVTRANSFORM);
    RpMaterialSetTexture(material, ((DualTextures *)data)->pass1);
    RpMatFXMaterialSetupDualTexture(material,
        ((DualTextures *)data)->pass2, rwBLENDSRCALPHA, rwBLENDINVSRCALPHA);
    RpMatFXMaterialSetUVTransformMatrices(
        material, &UVBaseTransform, &UVDualTransform);
    
    /* Set the material pipeline to NULL to ensure the default material
       pipeline is used. */
    RpMaterialSetPipeline(material, NULL);
    
    return material;
}

 
/*
 *****************************************************************************
 */
static RpAtomic *
AtomicSetup(RpAtomic *atomic, void *data)
{
    RpGeometryForAllMaterials(
        RpAtomicGetGeometry(atomic), MaterialSetup, data);
    
    /* Set the atomic pipeline to NULL to ensure the default atomic pipeline
       is used. */
    RpAtomicSetPipeline(atomic, NULL);
    
    return atomic;
} 


/*
 *****************************************************************************
 */
static RwBool 
Initialize3D(void *param)
{
    RwV3d pos;
    DualTextures textures;
    
    _rwDMAPreAlloc(8*1024*1024, 1024, NULL);
    
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
    
    /* Load clump */
    RwImageSetPath(RWSTRING("models/textures/"));    
    Clump = ClumpLoad(RWSTRING("models/hand.dff"));
    
    /* Setup clump for skinning */
    RwFrameForAllChildren(
        RpClumpGetFrame(Clump), FrameGetHierarchy, &Hierarchy);
    Hierarchy->flags |= rpHANIMHIERARCHYLOCALSPACEMATRICES;
    RpClumpForAllAtomics(Clump, AtomicSetSkinHierarchy, Hierarchy);    
    Anim = AnimationLoad(RWSTRING("models/hand.anm"));    
    RpHAnimHierarchySetCurrentAnim(Hierarchy, Anim);    

    /* Setup materials */
    textures.pass1 = RwTextureRead(RWSTRING("whiteash.png"), NULL);
    (void)RwTextureSetFilterMode(textures.pass1, rwFILTERLINEAR);
    textures.pass2 = RwTextureRead(RWSTRING("rw.png"), NULL);
    (void)RwTextureSetFilterMode(textures.pass2, rwFILTERLINEAR);
    RpClumpForAllAtomics(Clump, AtomicSetup, &textures);
    RwTextureDestroy(textures.pass1);
    RwTextureDestroy(textures.pass2);
    
    /* Setup for morphing */
    RpClumpForAllAtomics(Clump, AtomicSetupInterpolators, NULL);

    /* Rotate and position clump */
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
    
    /* Setup initial state */
    LightCB(FALSE);
    LightUpdateCB(FALSE);
    TestCB(FALSE);    
    RwMatrixSetIdentity(&UVBaseTransform);
    RwMatrixSetIdentity(&UVDualTransform);
    
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
    if( !RpMorphPluginAttach() )
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
    if( !RpMatFXPluginAttach() )
    {
        return FALSE;
    }
    if( !RpRandomPluginAttach() )
    {
        return FALSE;
    }
    
    /* Register pipelines */
    rwPDS_G3_Generic_AtmPipeRegister();
    rwPDS_G3_Uva_MatPipeRegister();
    rwPDS_G3_DupUva_MatPipeRegister();
    rwPDS_G3_Skin_AtmPipeRegister();
    rwPDS_G3_SkinUva_MatPipeRegister();
    rwPDS_G3_SkinDupUva_MatPipeRegister();
    rwPDS_VCL_Uva_MatPipeRegister();
    rwPDS_VCL_DupUva_MatPipeRegister();
    rwPDS_VCL_SkinUva_MatPipeRegister();
    rwPDS_VCL_SkinDupUva_MatPipeRegister();

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
            static RwUInt32 FogColor=RWRGBALONG(128, 128,128, 0);
            
            RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void *)FogOn);
            RwRenderStateSet(rwRENDERSTATEFOGCOLOR,(void *)FogColor);
            RwCameraSetFogDistance(Camera, 3.0f);
            RwRenderStateSet(rwRENDERSTATEFOGTYPE, (void *)rwFOGTYPELINEAR);
            RpSkySelectTrueTSClipper(TrueClipOn);
            RwRenderStateSet(rwRENDERSTATECULLMODE, (void *)FaceCullMode);

            RpWorldRender(World);
            
            RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void *)FALSE);

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
static RpAtomic *
AtomicMorphUpdate(RpAtomic *atomic, void *data)
{
    if (MorphOn)
    {
        RpMorphAtomicAddTime(atomic, *(RwReal*)data);
    }
    else
    {
        RpMorphAtomicSetTime(atomic, 0.0f);
    }        

    return atomic;
}


/*
 *****************************************************************************
 */
static void
UpdateUVTransforms(RwReal deltaTime)
{
    static RwReal baseTime = 0.0f;
    static RwReal dualTime = 0.0f;
    
    baseTime += 0.3f * deltaTime;
    dualTime += 0.7f * deltaTime;
    
    UVBaseTransform.right.x = (RwReal)RwCos(baseTime);
    UVBaseTransform.right.y = (RwReal)RwSin(baseTime);
    UVBaseTransform.up.x = -(RwReal)RwSin(baseTime);
    UVBaseTransform.up.y = (RwReal)RwCos(baseTime);
    UVBaseTransform.at.x = 0.0f;
    UVBaseTransform.at.y = 0.0f;
    UVDualTransform.right.x = 2.0f - (RwReal)RwCos(dualTime);
    UVDualTransform.right.y = 0.0f;
    UVDualTransform.up.x = 0.0f;
    UVDualTransform.up.y = 2.0f - (RwReal)RwCos(dualTime);
    UVDualTransform.at.x = 0.0f;
    UVDualTransform.at.y = 0.0f;
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
static RpLight *
LightGetDirection(RpLight *light, void *data)
{
    if (rpLIGHTDIRECTIONAL == RpLightGetType(light))
    {
        *(RwV3d *)data 
            = *RwMatrixGetAt(RwFrameGetMatrix(RpLightGetFrame(light)));
        
        return NULL;
    }
    
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

    if (UVAnimOn)
    {
        UpdateUVTransforms(deltaTime);
    }
    
    if (AnimOn)
    {
        RpHAnimHierarchyAddAnimTime(Hierarchy, deltaTime);
        RpHAnimHierarchyUpdateMatrices(Hierarchy);
    }
    
    RpClumpForAllAtomics(Clump, AtomicMorphUpdate, &deltaTime);
    
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
