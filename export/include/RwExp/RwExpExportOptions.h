
#ifndef RWEXPEXPORTOPTIONS_H
#define RWEXPEXPORTOPTIONS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwCommExportOptions.h"
#include "RwCommError.h"

#include "RwExp.h"

typedef void (*RwExpSetCustomExportOptionsFunc) (RwCommExportOptions*     pExportOptions);
#define SET_EXPORTOPTIONS_FUNC  "SetCustomExportOptions"


// BSP settings

/** \ingroup expopts
\def BSP_WORLDSECTORMAXPOLYS The Bsp Exporter max polygons per sector number.*/
#define             BSP_WORLDSECTORMAXPOLYS             "Max Polys Per World Sector"

/** \ingroup expopts
\def BSP_WORLDSECTORSIZE The maximum per sector size.*/
#define             BSP_WORLDSECTORSIZE                 "Max Sector Size"

/** \ingroup expopts
\def BSP_MAXPLANECHECK The Bsp Exporter maximum number of plane checks.*/
#define             BSP_MAXPLANECHECK                   "Max Plane Checks"

/** \ingroup expopts
\def BSP_PARTITIONSELECTOR The Bsp Exporter partition scheme
\li 1 = Quick
\li 2 = Closed Occluder
\li 3 = Maximum Extent
\li 4 = Open occluder
\li 5 = General occluder
\li 6 = Material boundary
\li 7 = Material count
\li 8 = Partition hints
\li 0/9 = Fuzzy balanced).*/
#define             BSP_PARTITIONSELECTOR               "Partition Scheme"


/** \ingroup expopts
\def BSP_MAXMATERIALS When the BSP partition scheme is set to Material Count this option
controls the maximum number of materials allowed per world sector.*/
#define             BSP_MAXMATERIALS                    "Max Materials Per Sector"

/** \ingroup expopts
\def BSP_STEPUP When the BSP partition scheme is set to Open occluder or General occluder
this option controls the stetup value used.*/
#define             BSP_STEPUP                          "Stepup"

/** \ingroup expopts
\def BSP_MAXSECTOROVERLAP The Bsp Exporter maximum sector overlap.*/
#define             BSP_MAXSECTOROVERLAP                "Max Sector Overlap"

/** \ingroup expopts
\def BSP_WELDPOLYGONS The Bsp Exporter polygon weld option.*/
#define             BSP_WELDPOLYGONS                    "Weld Polygons"

/** \ingroup expopts
\def BSP_DECIMATIONMODE The Bsp Exporter polygon weld approach to edge decimation.*/
#define             BSP_DECIMATIONMODE                  "Decimation Mode"

/** \ingroup expopts
\def BSP_DECIMATIONPASSES The Bsp Exporter polygon weld number of decimation passes.*/
#define             BSP_DECIMATIONPASSES                "Decimation Passes"

/** \ingroup expopts
\def BSP_CONVEXPARTITIONMODE The Bsp Exporter polygon weld approach to retriangulation.*/
#define             BSP_CONVEXPARTITIONMODE             "Convex Partitioning Mode"

/** \ingroup expopts
\def BSP_POLYGONAREATHRESHOLD The Bsp Exporter polygon weld area theshold for small polygon removal.*/
#define             BSP_POLYGONAREATHRESHOLD            "Polygon Area Threshold"

/** \ingroup expopts
\def BSP_POLYGONNORMALTHRESHOLD The Bsp Exporter normal theshold for polygon welding.*/
#define             BSP_POLYGONNORMALTHRESHOLD          "Polygon Normal Threshold"

/** \ingroup expopts
\def BSP_POLYGONUVTHRESHOLD The Bsp Exporter uv theshold for polygon welding.*/
#define             BSP_POLYGONUVTHRESHOLD              "Polygon UV Threshold"

/** \ingroup expopts
\def BSP_POLYGONPRELITCOLTHRESHOLD The Bsp Exporter prelight color theshold for polygon welding.*/
#define             BSP_POLYGONPRELITCOLTHRESHOLD       "Polygon PreLitColor Threshold"

/** \ingroup expopts
\def BSP_TEXTUREWRAP The Bsp Exporter condition geometry's texture wrap option for UV realignment.*/
#define             BSP_TEXTUREWRAP                     "Texture Wrap Mode"

/** \ingroup expopts
\def BSP_TEXTUREWRAP0 The Bsp Exporter condition geometry's texture wrap option for UV realignment on UV[0].*/
#define             BSP_TEXTUREWRAP0                    "Texture Wrap Mode UV0"

/** \ingroup expopts
\def BSP_TEXTUREWRAP1 The Bsp Exporter condition geometry's texture wrap option for UV realignment on UV[1].*/
#define             BSP_TEXTUREWRAP1                    "Texture Wrap Mode UV1"

/** \ingroup expopts
\def BSP_TEXTUREWRAP2 The Bsp Exporter condition geometry's texture wrap option for UV realignment on UV[2].*/
#define             BSP_TEXTUREWRAP2                    "Texture Wrap Mode UV2"

/** \ingroup expopts
\def BSP_TEXTUREWRAP3 The Bsp Exporter condition geometry's texture wrap option for UV realignment on UV[3].*/
#define             BSP_TEXTUREWRAP3                    "Texture Wrap Mode UV3"

/** \ingroup expopts
\def BSP_TEXTUREWRAP4 The Bsp Exporter condition geometry's texture wrap option for UV realignment on UV[4].*/
#define             BSP_TEXTUREWRAP4                    "Texture Wrap Mode UV4"

/** \ingroup expopts
\def BSP_TEXTUREWRAP5 The Bsp Exporter condition geometry's texture wrap option for UV realignment on UV[5].*/
#define             BSP_TEXTUREWRAP5                    "Texture Wrap Mode UV5"

/** \ingroup expopts
\def BSP_TEXTUREWRAP6 The Bsp Exporter condition geometry's texture wrap option for UV realignment on UV[6].*/
#define             BSP_TEXTUREWRAP6                    "Texture Wrap Mode UV6"

/** \ingroup expopts
\def BSP_TEXTUREWRAP7 The Bsp Exporter condition geometry's texture wrap option for UV realignment on UV[7].*/
#define             BSP_TEXTUREWRAP7                    "Texture Wrap Mode UV7"

/** \ingroup expopts
\def BSP_WORLDOFFSETX The Bsp Exporter X world offset.*/
#define             BSP_WORLDOFFSETX                    "Offset X"

/** \ingroup expopts
\def BSP_WORLDOFFSETY The Bsp Exporter Y world offset.*/
#define             BSP_WORLDOFFSETY                    "Offset Y"

/** \ingroup expopts
\def BSP_WORLDOFFSETZ The Bsp Exporter Z world offset.*/
#define             BSP_WORLDOFFSETZ                    "Offset Z"

// Internal option
#define             BSP_EXPORTSELECTED                  "BSP Export Selected"

// DFF settings

#define             DFF_PRELIGHTCLUMP                   "Legacy Allocate Prelight Space"

/** \ingroup expopts
\def DFF_HANIMHIERARCHY Controls whether this clump will have an an HAnim hierarchy attached.*/
#define             DFF_HANIMHIERARCHY               "Create RpHAnimHierarchy"

/** \ingroup expopts
\def ANM_HANIMNOMATRICES Controls whether the RpHAnimHierarchy created will have the rpHANIMHIERARCHYNOMATRICES flag set.*/
#define             ANM_HANIMNOMATRICES                 "RpHAnimHierarchy No Matrices"

/** \ingroup expopts
\def ANM_HANIMLOCALSPACEMATRICES Controls whether the RpHAnimHierarchy created will have the rpHANIMHIERARCHYLOCALSPACEMATRICES flag set.*/
#define             ANM_HANIMLOCALSPACEMATRICES         "RpHAnimHierarchy Local Matrices"


/** \ingroup expopts
\def DFF_MORPHTARGETS Controls whether the DFF exporter creates RpMorph targets for any
geometry animation found.*/
#define             DFF_MORPHTARGETS                    "Export RpMorph Targets"

/** \ingroup expopts
\def DFF_MORPHSAMPLE Controls whether the DFF exporter should analyze the scene to decide
where to create keyframes or whether to use a specificed frame interval.*/
#define             DFF_MORPHSAMPLE                     "RpMorph Sample"

/** \ingroup expopts
\def DFF_ANIMFRAMEINTERVAL The Dff Exporter morph animation interval.*/
#define             DFF_ANIMFRAMEINTERVAL               "RpMorph Sample Interval"


/** \ingroup expopts
\def DFF_RETAINOBJINWORLDSPACE When enabled this option leaves animated hierarchies in world space rather
than transforming them to a local space.*/
#define             DFF_RETAINOBJINWORLDSPACE           "World Space"

/** \ingroup expopts
\def DFF_EXPORTSKINS Controls whether the DFF exporter should export RpSkin animation for any skin modifiers
found in the scene.*/
#define             DFF_EXPORTSKINS                     "Export Skinning"

/** \ingroup expopts
\def DFF_NUMWEIGHTS The maximum number of skin weights to export per vertex. Any additional weights
will be ignored.*/
#define             DFF_NUMWEIGHTS                      "Max Skin Weights Per Vertex"

/** \ingroup expopts
\def DFF_EXPORTLABELS When enabled the exporter will export user defined object properties as RpUserData
on frames.*/
#define             DFF_EXPORTLABELS                    "3ds max User Defined Properties"

/** \ingroup expopts
\def DFF_DMORPHENABLED Controls whether the DFF exporter creates RpDMorph targets for any
blend shape type geomety animation found.*/
#define             DFF_DMORPHENABLED                   "Export RpDMorph"

/** \ingroup expopts
\def DFF_OPTIMIZEHIERARCHY The Dff Exporter optimize hierarchy option.*/
#define             DFF_OPTIMIZEHIERARCHY               "Optimize Hierarchy"


/** \ingroup expopts
\def DFF_BEZIERENABLED The Dff Exporter export patches option.*/
#define             DFF_BEZIERENABLED                   "Export Patches"

/** \ingroup expopts
\def DFF_ENABLE_SKIN_SPLITTING The Dff Exporter enable skin splitting.*/
#define             DFF_ENABLE_SKIN_SPLITTING           "Enable Skin Splitting"

/** \ingroup expopts
\def DFF_BONE_LIMIT The Dff Exporter export bone limit option.*/
#define             DFF_BONE_LIMIT                      "Bone Limit"

// ANM settings

/** \ingroup expopts
\def ANM_NOEXTRAINTERPKEYS Don't insert extra keyframes to try and map 3ds max
TCB and Bezier controllers to the RenderWare linear interpolation scheme.*/
#define             ANM_NOEXTRAINTERPKEYS               "3ds max Do not add extra interpolation keys"

/** \ingroup expopts
\def ANM_FILTERTOPLEVELBIPEDKEYS The Anm Exporter filter top level biped keys option.*/
#define             ANM_FILTERTOPLEVELBIPEDKEYS         "3ds max Filter Biped Keys"

/** \ingroup expopts
\def ANM_HANIMSUBHIERARCHYANIM Controls whether the animation generated will be for the whole hierarchy or for a sub hierarchy.*/
#define             ANM_HANIMSUBHIERARCHYANIM           "Sub Hierarchy Animation"

// Internal option
#define             ANM_DYNAMICKEYFRAMEGENERATION       "Dynamic Keyframe Generation"

// Internal option
#define             ANM_KEYFRAMEGENERATIONTOLERANCE     "Keyframe Generation Tolerance"

/** \ingroup expopts
\def ANM_COMPRESSION Controls whether the animation generated will be compressed.*/
#define             ANM_COMPRESSION                     "Keyframe Compression"

enum KeyframeCompressionType
{
    NO_COMPRESSION = 0,
    FLOAT_COMPRESSION
};

// SPL settings.
/** \ingroup expopts
\def SPL_VERTEXTYPE Controls the spline exported, Edit points or Control points.*/
#define             SPL_VERTEXTYPE                      "Spline Vertex Type"

// Common settings

/** \ingroup expopts
\def SCALEFACTOR The global scale factor.*/
#define             SCALEFACTOR                         "Global Scale Factor"

/** \ingroup expopts
\def SCALETYPE The scale type
\li 0 = Scale By
\li 1 = Scale To).*/
#define             SCALETYPE                           "Global Scale Type"

/** \ingroup expopts
\def TRISTRIPMESH The tri strip mesh option.*/
#define             TRISTRIPMESH                        "Tri-strip Meshes"

/** \ingroup expopts
\def TRISTRIPMETHOD The tri strip method.
\li 0 = PreProccess
\li 1 = PreProccess ignore Winding
\li 2 = Tunnel
\li 3 = Tunnel igonre winding
\li 4 = Cache aware) */
#define             TRISTRIPMETHOD                      "Tri-strip Method"

/** \ingroup expopts
\def TRISTRIPFACTOR The Tunnel tri strip quality (0-100).*/
#define             TRISTRIPFACTOR                      "Tunnel Tri-strip Factor"

/** \ingroup expopts
\def LIMITUVS The Limit UVs option.*/
#define             LIMITUVS                            "Limit UVs"

/** \ingroup expopts
\def LIMITUVMIN The Limit UVs minimum range value.*/
#define             LIMITUVMIN                          "Min UV Value"

/** \ingroup expopts
\def LIMITUVMAX The Limit UVs maximum range value.*/
#define             LIMITUVMAX                          "Max UV Value"

/** \ingroup expopts
\def WELDVERTICES The weld vertex positions option.*/
#define             WELDVERTICES                        "Weld Vertices"

/** \ingroup expopts
\def WELDTHRESHOLD The weld vertex threshold.*/
#define             WELDTHRESHOLD                       "Position Weld Threshold"

/** \ingroup expopts
\def WELDTHRESHOLDUV The weld vertex UVs option.*/
#define             WELDTHRESHOLDUV                     "UV Weld Threshold"

/** \ingroup expopts
\def WELDTHRESHOLDANGULAR The weld vertex angular option.*/
#define             WELDTHRESHOLDANGULAR                "Angular Weld Threshold"

/** \ingroup expopts
\def WELDTHRESHOLDCOLOR The weld vertex prelit color option.*/
#define             WELDTHRESHOLDCOLOR                  "PreLitColor Weld Threshold"

/** \ingroup expopts
\def GENERATECOLLISION The generate collision option.*/
#define             GENERATECOLLISION               "Generate Collision"


/** \ingroup expopts
\def TWOSIDEDMATERIALS Duplicate polygons to maintain 2-sided material rendering.*/
#define             TWOSIDEDMATERIALS                   "3ds max Export Two Sided Materials"

/** \ingroup expopts
\def COLORVERTEXPRELIGHT The vertex pre lights option.*/
#define             COLORVERTEXPRELIGHT                 "Vertex Prelights"

#define             TEXTURENAMECASE                     "Texture Name Case"

/** \ingroup expopts
\def EXPORTNORMALS The vertex normals option.*/
#define             EXPORTNORMALS                       "Vertex Normals"

/** \ingroup expopts
\def EXPORTLIT The lighting flag option.*/
#define             EXPORTLIT                           "Lighting Flag"

// Internal option
#define             DISPLAYREPORT                       "DISPLAYREPORT"

/** \ingroup expopts
\def PROCESSFORLIGHTMAPS This option generates an additional UV set derived from face normals (as required by RtLtMap).*/
#define             PROCESSFORLIGHTMAPS                 "Generate RtLtMap UVs"

// Internal option
#define             EXPORTLIGHTMAPS                     "EXPORTLIGHTMAPS"

// Internal option
#define             EXPORTSTARTTIME                     "EXPORTSTARTTIME"

// Internal option
#define             EXPORTENDTIME                       "EXPORTENDTIME"

/** \ingroup expopts
\def EXPORTUSERDATA Export user data option.*/
#define             EXPORTUSERDATA                      "Export User Data"

/** \ingroup expopts
\def TOONEXPORT This option tells the exporter to process exported geometry for toon rendering*/
#define             TOONEXPORT                          "Export Toon Data"

/** \ingroup expopts
\def TOONCREASEFROMSGS This option tells the exporter to generate crease edges on smoothing group
boundaries in the exported data.*/
#define             TOONCREASEFROMSGS                   "Generate Crease Edges from Smoothing Groups"

/** \ingroup expopts
\def TOONDEFAULTCREASEINKNAME The default ink name used for crease edges in exported data.*/
#define             TOONDEFAULTCREASEINKNAME            "Default Crease Ink Name"

/** \ingroup expopts
\def TOONDEFAULTSILHOUETTEINKNAME The default ink name used for silhouette edges in exported data.*/
#define             TOONDEFAULTSILHOUETTEINKNAME        "Default Silhouette Ink Name"

/** \ingroup expopts
\def TOONDEFAULTPAINTNAME The default paint name used in exported toon data.*/
#define             TOONDEFAULTPAINTNAME                "Default Paint Name"

// Output Settings

/** \ingroup expopts
\def EXPORTTOLEGACYFILES Export to DFF/BSP/ANM/SPL files option.*/
#define             EXPORTTOLEGACYFILES                 "Export Legacy Files"

/** \ingroup expopts
\def EXPORTTORWS Export to RWS file option.*/
#define             EXPORTTORWS                         "Export RWS File"

/** \ingroup expopts
\def EXPORTTORF3 Export to RF3 file option.*/
#define             EXPORTTORF3                         "Export RF3 File"

/** \ingroup expopts
\def RF3_SPLITBYASSET Whether to export a single RF3 file or an RF3 per asset.*/
#define             RF3_SPLITBYASSET                    "Split By Asset"

/** \ingroup expopts
\def RF3_TEXPATH_METHOD Determines how to stream out the texture path.*/
#define             RF3_TEXPATH_METHOD                  "Texture Path Type"

/** \ingroup expopts
\def RF3_RELATIVE_TEXPATH Specifies the texture relavtive path. (Only relevant if RF3_TEXPATH_METHOD is 1)*/
#define             RF3_RELATIVE_TEXPATH                "Relative Texture Path"

/** \ingroup expopts
\def RF3_REAL_AS_BINARY Streams out the RF3 real numbers in their binary representation.*/
#define             RF3_REAL_AS_BINARY                  "Real As Binary"


/** \ingroup expopts
\def RWS_EMBEDTOC Embed TOC in RWS file option.*/
#define             RWS_EMBEDTOC                        "Embed TOC"

/** \ingroup expopts
\def RWS_EMBEDTEXTUREDICT Embed texture dictionary in RWS file option.*/
#define             RWS_EMBEDTEXTUREDICT                "Embed Texture Dictionary"

/** \ingroup expopts
\def RWS_EMBEDEFFECTDICT Embed effect dictionary in RWS file option.*/
#define             RWS_EMBEDEFFECTDICT                 "Embed Effect Dictionary"

/** \ingroup expopts
\def RWS_EMBEDAMBIENTLIGHT Embed ambient light in RWS file option.*/
#define             RWS_EMBEDAMBIENTLIGHT               "3ds max Embed Ambient Light"

enum EmbedAmbientEnum
{
    EMBED_AMBIENT_SMART = 0,
    EMBED_AMBIENT_TRUE,
    EMBED_AMBIENT_FALSE
};

/** \ingroup expopts
\def RWS_PREINSTANCE Whether exported data should be preinstanced.*/
#define             RWS_PREINSTANCE                     "Pre Instance"

/** \ingroup expopts
\def RWS_PREINSTANCE Whether preinstancing should be performed locally only or also on remote console targets.*/
#define             RWS_LOCAL_PREINSTANCE               "Pre Instance Locally Only"

// Internal option
#define             GLB_VERBOSITY                       "VERBOSITY"

/** \ingroup expopts
\def GLB_VISPARTITIONS The visualize partitions option.*/
#define             GLB_VISPARTITIONS                   "Visualize Partitions"

/** \ingroup expopts
\def ASSETTYPE The type of asset being exported.*/
#define             ASSETTYPE                           "Asset Type"

#define             RF3_TEST                            "Test RF3"

enum AssetType
{
    HANIMATION_ASSET = 0,
    WORLD_ASSET,
    CLUMP_ASSET,
    SPLINE_ASSET
};

#define             SHOW_EXPORT_WIZARD                  "Show Export Wizard"

// Per object settings
#define  RWEXP_LIGHT_FLAG               "RwLightFlag"
#define  RWEXP_EXPORT_NORMAL_STRING     "RwNormalsFlag"
#define  RWEXP_EXPORT_PRELIGHTS_STRING  "RwPrelightsFlag"
#define  RWEXP_MERGE_GROUP_STRING       "RwMergeGroup"
#define  RWEXP_TOON_EXPORT_STRING       "RwToonFlag"
#define  RWEXP_TOON_CREASES_STRING      "RwToonCreasesFlag"

RWEXP_API RwCommError RwExpInitArtistOptions(RwCommExportOptions &options, AssetType type);

#endif // RWEXPEXPORTOPTIONS_H
