#ifndef _EXPPIPES_H_
#define _EXPPIPES_H_

/* FROM RPPDS.H */

#if (defined(SKY2_DRVMODEL_H) || defined(NULLSKY_DRVMODEL_H))

#if (defined(NULLSKY_DRVMODEL_H))
#define RWCROSS
#endif /* (defined(NULLSKY_DRVMODEL_H)) */

#include "rppds.h"

#else /* (defined(SKY2_DRVMODEL_H) || defined(NULLSKY_DRVMODEL_H)) */

struct pipeStringAndId
{
    const char *name;
    RwInt32 id;
};

/*---- start: ./PDS_id.h----*/

/* Generic PipeID construction. */
#define rpPDS_MAKEPIPEID(vendorID, pipeID)              \
    (((vendorID & 0xFFFF) << 16) | (pipeID & 0xFFFF))

#define rpPDS_GETPIPEID(pipeID)                         \
    (pipeID & 0xFFFF)

#define rpPDS_GETVENDORID(pipeID)                       \
    ((pipeID >> 16) & 0xFFFF)

/* Specific PipeID construction. */
#define rpPDS_MAKEGENERICPIPEID(name, pipeID)           \
    name = rpPDS_MAKEPIPEID(rwVENDORID_CORE, pipeID)

#define rpPDS_MAKEPLUGINPIPEID(name, pipeID)            \
    name = rpPDS_MAKEPIPEID(rwVENDORID_CRITERIONTK, pipeID)

#define rpPDS_MAKEWORLDPIPEID(name, pipeID)             \
    name = rpPDS_MAKEPIPEID(rwVENDORID_CRITERIONWORLD, pipeID)

/*---- start: ./sky2/PDS_pipeIds/PDS_idstrings.h----*/
#define rpPDS_PIPELINESTRINGS_GENERIC \
    	{ RWSTRING("rwPDS_G3_Generic_MatPipeID"), rwPDS_G3_Generic_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_Generic_AtmPipeID"), rwPDS_G3_Generic_AtmPipeID }, \
    	{ RWSTRING("rwPDS_G3_Generic_SctPipeID"), rwPDS_G3_Generic_SctPipeID }, \
    	{ RWSTRING("rwPDS_G3_Im3D_TriPipeID"), rwPDS_G3_Im3D_TriPipeID }, \
    	{ RWSTRING("rwPDS_G3_Im3D_SegPipeID"), rwPDS_G3_Im3D_SegPipeID }, \
    	{ RWSTRING("rwPDS_G3_Im3D_TriObjPipeID"), rwPDS_G3_Im3D_TriObjPipeID }, \
    	{ RWSTRING("rwPDS_G3_Im3D_SegObjPipeID"), rwPDS_G3_Im3D_SegObjPipeID }, \
    	{ RWSTRING("rwPDS_G3_Generic_GrpMatPipeID"), rwPDS_G3_Generic_GrpMatPipeID }, \
    	{ RWSTRING("rwPDS_G3_Generic_GrpAtmPipeID"), rwPDS_G3_Generic_GrpAtmPipeID }, \
    	{ RWSTRING("rwPDS_G3_Generic_GrpSctPipeID"), rwPDS_G3_Generic_GrpSctPipeID }, \
 
#define rpPDS_PIPELINESTRINGS_PLUGIN \
    	{ RWSTRING("rwPDS_G3x_Generic_AtmPipeID"), rwPDS_G3x_Generic_AtmPipeID }, \
    	{ RWSTRING("rwPDS_G3x_APL_MatPipeID"), rwPDS_G3x_APL_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_ADL_MatPipeID"), rwPDS_G3x_ADL_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_A4D_MatPipeID"), rwPDS_G3x_A4D_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_APLDup_MatPipeID"), rwPDS_G3x_APLDup_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_ADLDup_MatPipeID"), rwPDS_G3x_ADLDup_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_A4DDup_MatPipeID"), rwPDS_G3x_A4DDup_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_APLGem_MatPipeID"), rwPDS_G3x_APLGem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_ADLGem_MatPipeID"), rwPDS_G3x_ADLGem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_A4DGem_MatPipeID"), rwPDS_G3x_A4DGem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_Skin_AtmPipeID"), rwPDS_G3x_Skin_AtmPipeID }, \
    	{ RWSTRING("rwPDS_G3x_APLSkin_MatPipeID"), rwPDS_G3x_APLSkin_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_ADLSkin_MatPipeID"), rwPDS_G3x_ADLSkin_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_A4DSkin_MatPipeID"), rwPDS_G3x_A4DSkin_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_APLSkinDup_MatPipeID"), rwPDS_G3x_APLSkinDup_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_ADLSkinDup_MatPipeID"), rwPDS_G3x_ADLSkinDup_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_A4DSkinDup_MatPipeID"), rwPDS_G3x_A4DSkinDup_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_APLSkinGem_MatPipeID"), rwPDS_G3x_APLSkinGem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_ADLSkinGem_MatPipeID"), rwPDS_G3x_ADLSkinGem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_A4DSkinGem_MatPipeID"), rwPDS_G3x_A4DSkinGem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_ADLClone_MatPipeID"), rwPDS_G3x_ADLClone_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_ADLClone_AtmPipeID"), rwPDS_G3x_ADLClone_AtmPipeID }, \
    	{ RWSTRING("rwPDS_G3x_OPLClone_MatPipeID"), rwPDS_G3x_OPLClone_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_OPLClone_AtmPipeID"), rwPDS_G3x_OPLClone_AtmPipeID }, \
    	{ RWSTRING("rwPDS_G3x_Team_MatPipeID"), rwPDS_G3x_Team_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_DupTeam_MatPipeID"), rwPDS_G3x_DupTeam_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_GemTeam_MatPipeID"), rwPDS_G3x_GemTeam_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_SkinTeam_MatPipeID"), rwPDS_G3x_SkinTeam_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_SkinDupTeam_MatPipeID"), rwPDS_G3x_SkinDupTeam_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_SkinGemTeam_MatPipeID"), rwPDS_G3x_SkinGemTeam_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_Team_AtmPipeID"), rwPDS_G3x_Team_AtmPipeID }, \
    	{ RWSTRING("rwPDS_G3x_SkinTeam_AtmPipeID"), rwPDS_G3x_SkinTeam_AtmPipeID }, \
    	{ RWSTRING("rwPDS_G3x_APLUva_MatPipeID"), rwPDS_G3x_APLUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_ADLUva_MatPipeID"), rwPDS_G3x_ADLUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_A4DUva_MatPipeID"), rwPDS_G3x_A4DUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_APLDupUva_MatPipeID"), rwPDS_G3x_APLDupUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_ADLDupUva_MatPipeID"), rwPDS_G3x_ADLDupUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_A4DDupUva_MatPipeID"), rwPDS_G3x_A4DDupUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_APLSkinUva_MatPipeID"), rwPDS_G3x_APLSkinUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_ADLSkinUva_MatPipeID"), rwPDS_G3x_ADLSkinUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_A4DSkinUva_MatPipeID"), rwPDS_G3x_A4DSkinUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_APLSkDupUva_MatPipeID"), rwPDS_G3x_APLSkDupUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_ADLSkDupUva_MatPipeID"), rwPDS_G3x_ADLSkDupUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3x_A4DSkDupUva_MatPipeID"), rwPDS_G3x_A4DSkDupUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_APL_MatPipeID"), rwPDS_G3xd_APL_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_ADL_MatPipeID"), rwPDS_G3xd_ADL_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_A4D_MatPipeID"), rwPDS_G3xd_A4D_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_APLDup_MatPipeID"), rwPDS_G3xd_APLDup_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_ADLDup_MatPipeID"), rwPDS_G3xd_ADLDup_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_A4DDup_MatPipeID"), rwPDS_G3xd_A4DDup_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_APLGem_MatPipeID"), rwPDS_G3xd_APLGem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_ADLGem_MatPipeID"), rwPDS_G3xd_ADLGem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_A4DGem_MatPipeID"), rwPDS_G3xd_A4DGem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_APLUva_MatPipeID"), rwPDS_G3xd_APLUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_ADLUva_MatPipeID"), rwPDS_G3xd_ADLUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_A4DUva_MatPipeID"), rwPDS_G3xd_A4DUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_APLDupUva_MatPipeID"), rwPDS_G3xd_APLDupUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_ADLDupUva_MatPipeID"), rwPDS_G3xd_ADLDupUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_A4DDupUva_MatPipeID"), rwPDS_G3xd_A4DDupUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_APLSkin_MatPipeID"), rwPDS_G3xd_APLSkin_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_ADLSkin_MatPipeID"), rwPDS_G3xd_ADLSkin_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_A4DSkin_MatPipeID"), rwPDS_G3xd_A4DSkin_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_APLSkinDup_MatPipeID"), rwPDS_G3xd_APLSkinDup_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_ADLSkinDup_MatPipeID"), rwPDS_G3xd_ADLSkinDup_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_A4DSkinDup_MatPipeID"), rwPDS_G3xd_A4DSkinDup_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_APLSkinGem_MatPipeID"), rwPDS_G3xd_APLSkinGem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_ADLSkinGem_MatPipeID"), rwPDS_G3xd_ADLSkinGem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_A4DSkinGem_MatPipeID"), rwPDS_G3xd_A4DSkinGem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_APLSkinUva_MatPipeID"), rwPDS_G3xd_APLSkinUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_ADLSkinUva_MatPipeID"), rwPDS_G3xd_ADLSkinUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_A4DSkinUva_MatPipeID"), rwPDS_G3xd_A4DSkinUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_APLSkDupUva_MatPipeID"), rwPDS_G3xd_APLSkDupUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_ADLSkDupUva_MatPipeID"), rwPDS_G3xd_ADLSkDupUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3xd_A4DSkDupUva_MatPipeID"), rwPDS_G3xd_A4DSkDupUva_MatPipeID }, \
 
#define rpPDS_PIPELINESTRINGS_WORLD \
    	{ RWSTRING("rwPDS_G3_Skin_MatPipeID"), rwPDS_G3_Skin_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_Skin_AtmPipeID"), rwPDS_G3_Skin_AtmPipeID }, \
    	{ RWSTRING("rwPDS_G3_Emb_MatPipeID"), rwPDS_G3_Emb_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_Gem_MatPipeID"), rwPDS_G3_Gem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_Bem_MatPipeID"), rwPDS_G3_Bem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_Dup_MatPipeID"), rwPDS_G3_Dup_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_Toon_MatPipeID"), rwPDS_G3_Toon_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkinEmb_MatPipeID"), rwPDS_G3_SkinEmb_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkinGem_MatPipeID"), rwPDS_G3_SkinGem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkinBem_MatPipeID"), rwPDS_G3_SkinBem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkinDup_MatPipeID"), rwPDS_G3_SkinDup_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkinToon_MatPipeID"), rwPDS_G3_SkinToon_MatPipeID }, \
    	{ RWSTRING("rwPDS_G2_BPatch_AtmPipeID"), rwPDS_G2_BPatch_AtmPipeID }, \
    	{ RWSTRING("rwPDS_G2_BPatch_MatPipeID"), rwPDS_G2_BPatch_MatPipeID }, \
    	{ RWSTRING("rwPDS_G2_SkinBPatch_AtmPipeID"), rwPDS_G2_SkinBPatch_AtmPipeID }, \
    	{ RWSTRING("rwPDS_G2_SkinBPatch_MatPipeID"), rwPDS_G2_SkinBPatch_MatPipeID }, \
    	{ RWSTRING("rwPDS_G2_EmbBPatch_MatPipeID"), rwPDS_G2_EmbBPatch_MatPipeID }, \
    	{ RWSTRING("rwPDS_G2_GemBPatch_MatPipeID"), rwPDS_G2_GemBPatch_MatPipeID }, \
    	{ RWSTRING("rwPDS_G2_BemBPatch_MatPipeID"), rwPDS_G2_BemBPatch_MatPipeID }, \
    	{ RWSTRING("rwPDS_G2_DupBPatch_MatPipeID"), rwPDS_G2_DupBPatch_MatPipeID }, \
    	{ RWSTRING("rwPDS_G2_SkinEmbBPatch_MatPipeID"), rwPDS_G2_SkinEmbBPatch_MatPipeID }, \
    	{ RWSTRING("rwPDS_G2_SkinGemBPatch_MatPipeID"), rwPDS_G2_SkinGemBPatch_MatPipeID }, \
    	{ RWSTRING("rwPDS_G2_SkinBemBPatch_MatPipeID"), rwPDS_G2_SkinBemBPatch_MatPipeID }, \
    	{ RWSTRING("rwPDS_G2_SkinDupBPatch_MatPipeID"), rwPDS_G2_SkinDupBPatch_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_2DStroke_MatPipeID"), rwPDS_G3_2DStroke_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_2DStroke_ObjPipeID"), rwPDS_G3_2DStroke_ObjPipeID }, \
    	{ RWSTRING("rwPDS_G3_2DFont_MatPipeID"), rwPDS_G3_2DFont_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_2DFont_ObjPipeID"), rwPDS_G3_2DFont_ObjPipeID }, \
    	{ RWSTRING("rwPDS_G3_2DFill_MatPipeID"), rwPDS_G3_2DFill_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_2DFill_ObjPipeID"), rwPDS_G3_2DFill_ObjPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkSpt_AtmPipeID"), rwPDS_G3_SkSpt_AtmPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkSpt_MatPipeID"), rwPDS_G3_SkSpt_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkSptEmb_MatPipeID"), rwPDS_G3_SkSptEmb_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkSptGem_MatPipeID"), rwPDS_G3_SkSptGem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkSptBem_MatPipeID"), rwPDS_G3_SkSptBem_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkSptDup_MatPipeID"), rwPDS_G3_SkSptDup_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkSptToon_MatPipeID"), rwPDS_G3_SkSptToon_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_Uva_MatPipeID"), rwPDS_G3_Uva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_DupUva_MatPipeID"), rwPDS_G3_DupUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkinUva_MatPipeID"), rwPDS_G3_SkinUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkinDupUva_MatPipeID"), rwPDS_G3_SkinDupUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkSptUva_MatPipeID"), rwPDS_G3_SkSptUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkSptDupUva_MatPipeID"), rwPDS_G3_SkSptDupUva_MatPipeID }, \
    	{ RWSTRING("rwPDS_G3_Skin_GrpMatPipeID"), rwPDS_G3_Skin_GrpMatPipeID }, \
    	{ RWSTRING("rwPDS_G3_Skin_GrpAtmPipeID"), rwPDS_G3_Skin_GrpAtmPipeID }, \
    	{ RWSTRING("rwPDS_G2_DarkMaps_GrpMatPipeID"), rwPDS_G2_DarkMaps_GrpMatPipeID }, \
    	{ RWSTRING("rwPDS_G2_DarkMaps_GrpAtmPipeID"), rwPDS_G2_DarkMaps_GrpAtmPipeID }, \
    	{ RWSTRING("rwPDS_G2_DarkMaps_GrpSctPipeID"), rwPDS_G2_DarkMaps_GrpSctPipeID }, \
    	{ RWSTRING("rwPDS_G3_Toon_GrpMatPipeID"), rwPDS_G3_Toon_GrpMatPipeID }, \
    	{ RWSTRING("rwPDS_G3_Toon_GrpAtmPipeID"), rwPDS_G3_Toon_GrpAtmPipeID }, \
    	{ RWSTRING("rwPDS_G3_Toon_GrpSctPipeID"), rwPDS_G3_Toon_GrpSctPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkinToon_GrpMatPipeID"), rwPDS_G3_SkinToon_GrpMatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkinToon_GrpAtmPipeID"), rwPDS_G3_SkinToon_GrpAtmPipeID }, \
    	{ RWSTRING("rwPDS_G3_MatfxUV1_GrpMatPipeID"), rwPDS_G3_MatfxUV1_GrpMatPipeID }, \
    	{ RWSTRING("rwPDS_G3_MatfxUV2_GrpMatPipeID"), rwPDS_G3_MatfxUV2_GrpMatPipeID }, \
    	{ RWSTRING("rwPDS_G3_MatfxUV1_GrpAtmPipeID"), rwPDS_G3_MatfxUV1_GrpAtmPipeID }, \
    	{ RWSTRING("rwPDS_G3_MatfxUV2_GrpAtmPipeID"), rwPDS_G3_MatfxUV2_GrpAtmPipeID }, \
    	{ RWSTRING("rwPDS_G3_MatfxUV1_GrpSctPipeID"), rwPDS_G3_MatfxUV1_GrpSctPipeID }, \
    	{ RWSTRING("rwPDS_G3_MatfxUV2_GrpSctPipeID"), rwPDS_G3_MatfxUV2_GrpSctPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkinfxUV1_GrpMatPipeID"), rwPDS_G3_SkinfxUV1_GrpMatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkinfxUV2_GrpMatPipeID"), rwPDS_G3_SkinfxUV2_GrpMatPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkinfxUV1_GrpAtmPipeID"), rwPDS_G3_SkinfxUV1_GrpAtmPipeID }, \
    	{ RWSTRING("rwPDS_G3_SkinfxUV2_GrpAtmPipeID"), rwPDS_G3_SkinfxUV2_GrpAtmPipeID }, \
    	{ RWSTRING("rwPDS_G2_BPatch_GrpAtmPipeID"), rwPDS_G2_BPatch_GrpAtmPipeID }, \
    	{ RWSTRING("rwPDS_G2_BPatch_GrpMatPipeID"), rwPDS_G2_BPatch_GrpMatPipeID }, \
    	{ RWSTRING("rwPDS_G2_SkinBPatch_GrpAtmPipeID"), rwPDS_G2_SkinBPatch_GrpAtmPipeID }, \
    	{ RWSTRING("rwPDS_G2_SkinBPatch_GrpMatPipeID"), rwPDS_G2_SkinBPatch_GrpMatPipeID }, \
    	{ RWSTRING("rwPDS_G2_MatfxUV1BPatch_GrpMatPipeID"), rwPDS_G2_MatfxUV1BPatch_GrpMatPipeID }, \
    	{ RWSTRING("rwPDS_G2_MatfxUV2BPatch_GrpMatPipeID"), rwPDS_G2_MatfxUV2BPatch_GrpMatPipeID }, \
    	{ RWSTRING("rwPDS_G2_MatfxUV1BPatch_GrpAtmPipeID"), rwPDS_G2_MatfxUV1BPatch_GrpAtmPipeID }, \
    	{ RWSTRING("rwPDS_G2_MatfxUV2BPatch_GrpAtmPipeID"), rwPDS_G2_MatfxUV2BPatch_GrpAtmPipeID }, \
    	{ RWSTRING("rwPDS_G2_SkinfxUV1BPatch_GrpMatPipeID"), rwPDS_G2_SkinfxUV1BPatch_GrpMatPipeID }, \
    	{ RWSTRING("rwPDS_G2_SkinfxUV2BPatch_GrpMatPipeID"), rwPDS_G2_SkinfxUV2BPatch_GrpMatPipeID }, \
    	{ RWSTRING("rwPDS_G2_SkinfxUV1BPatch_GrpAtmPipeID"), rwPDS_G2_SkinfxUV1BPatch_GrpAtmPipeID }, \
    	{ RWSTRING("rwPDS_G2_SkinfxUV2BPatch_GrpAtmPipeID"), rwPDS_G2_SkinfxUV2BPatch_GrpAtmPipeID }, \
 
/*---- end: ./sky2/PDS_pipeIds/PDS_idstrings.h----*/

#define rpPDS_PIPELINESTRINGS                           \
{                                                       \
    rpPDS_PIPELINESTRINGS_GENERIC                       \
    rpPDS_PIPELINESTRINGS_PLUGIN                        \
    rpPDS_PIPELINESTRINGS_WORLD                         \
}


enum RpPDSPipeID
{
    rpNAPDSPIPEID = 0,

/*---- start: ./sky2/PDS_pipeIds/PDS_genericPipes.h----*/

    /*== Generic RenderWare pipelines. ====================================*/

    /*-- Generic pipelines. -----------------------------------------------*/
    rpPDS_MAKEGENERICPIPEID( rwPDS_G3_Generic_MatPipeID,            0x0001 ),
    /**<Generic Material rendering pipeline
     * \see \ref rppdsps2_g3_generic
     */
    rpPDS_MAKEGENERICPIPEID( rwPDS_G3_Generic_AtmPipeID,            0x0002 ),
    /**<Object only Atomic rendering pipeline
     * \see \ref rppdsps2_g3_generic
     */
    rpPDS_MAKEGENERICPIPEID( rwPDS_G3_Generic_SctPipeID,            0x0003 ),
    /**<Object only World Sector Generic rendering pipeline
     * \see \ref rppdsps2_g3_generic
     */

    /*-- Generic Im3D pipelines. ------------------------------------------*/
    rpPDS_MAKEGENERICPIPEID( rwPDS_G3_Im3D_TriPipeID,               0x0004 ),
    /**<(internal Triangle Im3D material pipeline)
     * \see \ref rppdsps2_g3_im3d
     */
    rpPDS_MAKEGENERICPIPEID( rwPDS_G3_Im3D_SegPipeID,               0x0005 ),
    /**<(internal Line Im3D material pipeline)
     * \see \ref rppdsps2_g3_im3d
     */
    rpPDS_MAKEGENERICPIPEID( rwPDS_G3_Im3D_TriObjPipeID,            0x0006 ),
    /**<Grouped Triangle Im3D pipeline
     * \see \ref rppdsps2_g3_im3d
     */
    rpPDS_MAKEGENERICPIPEID( rwPDS_G3_Im3D_SegObjPipeID,            0x0007 ),
    /**<Grouped Line Im3D pipeline
     * \see \ref rppdsps2_g3_im3d
     */

    /*=====================================================================*/

    /*-- Grouped Generic pipelines. ---------------------------------------*/
    rpPDS_MAKEGENERICPIPEID( rwPDS_G3_Generic_GrpMatPipeID,         0x1001 ),
    /**<Grouped Material Generic rendering pipeline
     * \see \ref rppdsps2_g3_im3d
     */
    rpPDS_MAKEGENERICPIPEID( rwPDS_G3_Generic_GrpAtmPipeID,         0x1002 ),
    /**<Grouped Atomic Generic rendering pipeline
     * \see \ref rppdsps2_g3_im3d
     */
    rpPDS_MAKEGENERICPIPEID( rwPDS_G3_Generic_GrpSctPipeID,         0x1003 ),
    /**<Grouped World Sector Generic rendering pipeline
     * \see \ref rppdsps2_g3_im3d
     */

    /*=====================================================================*/

/*---- end: ./sky2/PDS_pipeIds/PDS_genericPipes.h----*/
/*---- start: ./sky2/PDS_pipeIds/PDS_pluginPipes.h----*/

    /*== RenderWare Plugin pipelines. =====================================*/

    /*-- Skinning generic pipelines. --------------------------------------*/

    /* Skinning generic pipelines. */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_Skin_MatPipeID,                0x0001 ),
    /**<Generic Skinning material rendering pipeline
     * \see \ref rppdsps2_g3_skin
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_Skin_AtmPipeID,                0x0002 ),
    /**<Generic Skinning atomic rendering pipeline
     * \see \ref rppdsps2_g3_skin
     */

    /*-- Material effect rendering pipelines. -----------------------------*/

    /* Material effect rendering pipelines. */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_Emb_MatPipeID,                 0x0003 ),
    /**<Emboss Bump Map material rendering pipeline
     * \see \ref rppdsps2_g3_emb
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_Gem_MatPipeID,                 0x0004 ),
    /**<Gloss Environment Map material rendering pipeline
     * \see \ref rppdsps2_g3_gem
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_Bem_MatPipeID,                 0x0005 ),
    /**<Bump Environment Map material rendering pipeline
     * \see \ref rppdsps2_g3_bem
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_Dup_MatPipeID,                 0x0006 ),
    /**<Dual Pass material rendering pipeline
     * \see \ref rppdsps2_g3_dup
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_Toon_MatPipeID,                0x0007 ),
    /**<Toon material rendering pipeline
     * \see \ref rppdsps2_g3_toon
     */

    /*-- Skin Material effect rendering pipelines. ------------------------*/

    /* Skin material effect rendering pipelines. */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkinEmb_MatPipeID,             0x0008 ),
    /**<Skinning Emboss Bump Map material rendering pipeline
     * \see \ref rppdsps2_g3_skinemb
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkinGem_MatPipeID,             0x0009 ),
    /**<Skinnig Gloss Environment Map material rendering pipeline
     * \see \ref rppdsps2_g3_skingem
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkinBem_MatPipeID,             0x000a ),
    /**<Skinning Bump Environment Map material rendering pipeline
     * \see \ref rppdsps2_g3_skinbem
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkinDup_MatPipeID,             0x000b ),
    /**<Skinning Dual Pass material rendering pipeline
     * \see \ref rppdsps2_g3_skindup
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkinToon_MatPipeID,            0x000c ),
    /**<Skinnig Toon Cell Shading material rendering pipeline
     * \see \ref rppdsps2_g3_skintoon
     */

    /*-- Patch pipelines. -------------------------------------------------*/

    /* Generic and Skinned BPatch object pipelines. */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_BPatch_AtmPipeID,              0x000d ),
    /**<Generic Bezier Patch atomic rendering pipeline
     * \see \ref rppdsps2_g2_bpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_BPatch_MatPipeID,              0x000e ),
    /**<Generic Bezier Patch material rendering pipeline
     * \see \ref rppdsps2_g2_bpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_SkinBPatch_AtmPipeID,          0x000f ),
    /**<Skinning Bezier Patch atomic rendering pipeline
     * \see \ref rppdsps2_g2_skinbpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_SkinBPatch_MatPipeID,          0x0010 ),
    /**<Skinning Bezier Patcj material rendering pipeline
     * \see \ref rppdsps2_g2_skinbpatch
     */

    /* Generic & Matfx BPatch rendering pipelines. */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_EmbBPatch_MatPipeID,           0x0011 ),
    /**<Emboss bumpmapping Bezier Patch material rendering pipeline
     * \see \ref rppdsps2_g2_embbpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_GemBPatch_MatPipeID,           0x0012 ),
    /**<Gloss environment map Bezier Patch material rendering pipeline
     * \see \ref rppdsps2_g2_gembpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_BemBPatch_MatPipeID,           0x0013 ),
    /**<Bump environment map Bezier Patch material rendering pipeline
     * \see \ref rppdsps2_g2_bembpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_DupBPatch_MatPipeID,           0x0014 ),
    /**<Dual pass Bezier Patch material rendering pipeline
     * \see \ref rppdsps2_g2_dupbpatch
     */

    /* Skin Matfx and Skin Generic BPatch rendering pipelines. */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_SkinEmbBPatch_MatPipeID,       0x0015 ),
    /**<Skinning Emboss bumpmapping Bezier Patch material rendering pipeline
     * \see \ref rppdsps2_g2_skinembbpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_SkinGemBPatch_MatPipeID,       0x0016 ),
    /**<Skinning Gloss environment map Bezier Patch material rendering pipeline
     * \see \ref rppdsps2_g2_skingembpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_SkinBemBPatch_MatPipeID,       0x0017 ),
    /**<Skinning Bump environment map Bezier Patch material rendering pipeline
     * \see \ref rppdsps2_g2_skinbembpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_SkinDupBPatch_MatPipeID,       0x0018 ),
    /**<Skinning Dual pass Bezier Patch material rendering pipeline
     * \see \ref rppdsps2_g2_skindupbpatch
     */

    /*-- 2D pipelines. ----------------------------------------------------*/

    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_2DStroke_MatPipeID,            0x0019 ),
    /**<2D stroke material rendering pipeline
     * \see \ref rppdsps2_g3_2dstroke
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_2DStroke_ObjPipeID,            0x001a ),
    /**<2D stroke grouped object rendering pipeline
     * \see \ref rppdsps2_g3_2dstroke
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_2DFont_MatPipeID,              0x001b ),
    /**<2D font material rendering pipeline
     * \see \ref rppdsps2_g3_2dfont
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_2DFont_ObjPipeID,              0x001c ),
    /**<2D font grouped object rendering pipeline
     * \see \ref rppdsps2_g3_2dfont
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_2DFill_MatPipeID,              0x001d ),
    /**<2D fill material rendering pipeline
     * \see \ref rppdsps2_g3_2dfill
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_2DFill_ObjPipeID,              0x001e ),
    /**<2D fill grouped object rendering pipeline
     * \see \ref rppdsps2_g3_2dfill
     */

    /*-- Skin splittting pipelines. ---------------------------------------*/

    /* Skin splitting generic pipelines. */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkSpt_AtmPipeID,               0x001f ),
    /**<Skin Splitting atomic rendering pipeline
     * \see \ref rppdsps2_g3_skspt
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkSpt_MatPipeID,               0x0020 ),
    /**<Skin Splitting generic material rendering pipeline
     * \see \ref rppdsps2_g3_skspt
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkSptEmb_MatPipeID,            0x0021 ),
    /**<Skin Splitting Emboss bump mapping material rendering pipeline
     * \see \ref rppdsps2_g3_sksptemb
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkSptGem_MatPipeID,            0x0022 ),
    /**<Skin Splitting Gloss Enivronment mapping material rendering pipeline
     * \see \ref rppdsps2_g3_sksptgem
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkSptBem_MatPipeID,            0x0023 ),
    /**<Skin Splitting Bump Environment mapping material rendering pipeline
     * \see \ref rppdsps2_g3_sksptbem
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkSptDup_MatPipeID,            0x0024 ),
    /**<Skin Splitting Dual pass material rendering pipeline
     * \see \ref rppdsps2_g3_sksptdup
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkSptToon_MatPipeID,           0x0025 ),
    /**<Skin Splitting Toon Cell Shading material rendering pipeline
     * \see \ref rppdsps2_g3_skspttoon
     */

    /*-- UV Animation pipelines. ------------------------------------------*/

    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_Uva_MatPipeID,                 0x0026 ),
    /**<Single pass texture coordinated (u,v) matrix transform animation
     * pipeline.
     * \see \ref rppdsps2_g3_uva
     */
     
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_DupUva_MatPipeID,              0x0027 ),
    /**<Dual pass texture coordinated (u,v) matrix transform animation
     * pipeline.
     * \see \ref rppdsps2_g3_dupuva
     */

    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkinUva_MatPipeID,             0x0028 ),
    /**<Single pass texture coordinated (u,v) matrix transform animation
     * skinning pipeline.
     * \see \ref rppdsps2_g3_skinuva
     */

    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkinDupUva_MatPipeID,          0x0029 ),
    /**<Dual pass texture coordinated (u,v) matrix transform animation
     * skinning pipeline.
     * \see \ref rppdsps2_g3_skindupuva
     */

    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkSptUva_MatPipeID,            0x002a ),
    /**<Single pass texture coordinated (u,v) matrix transform animation
     * skinning splitting pipeline.
     * \see \ref rppdsps2_g3_sksptuva
     */

    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkSptDupUva_MatPipeID,         0x002b ),
    /**<Dual pass texture coordinated (u,v) matrix transform animation
     * skinning splitting pipeline.
     * \see \ref rppdsps2_g3_sksptdupuva
     */

    /*=====================================================================*/

    /*-- Grouped pipelines. -----------------------------------------------*/

    /* Grouped skinning generic pipelines. */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_Skin_GrpMatPipeID,             0x1001 ),
    /**<Grouped Skinning material rendering pipeline
     * \see \ref rppdsps2_g3_skin
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_Skin_GrpAtmPipeID,             0x1002 ),
    /**<Grouped Skinning atomic rendering pipeline
     * \see \ref rppdsps2_g3_skin
     */

    /* Grouped DarkMap pipelines. */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_DarkMaps_GrpMatPipeID,         0x1003 ),
    /**<Grouped Dark Maps material rendering pipeline
     * \see \ref rppdsps2_g2_darkmaps
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_DarkMaps_GrpAtmPipeID,         0x1004 ),
    /**<Grouped Dark Maps atomic rendering pipeline
     * \see \ref rppdsps2_g2_darkmaps
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_DarkMaps_GrpSctPipeID,         0x1005 ),
    /**<Grouped Dark Maps world sector rendering pipeline
     * \see \ref rppdsps2_g2_darkmaps
     */

    /* Grouped Toon pipelines. */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_Toon_GrpMatPipeID,             0x1006 ),
    /**<Grouped Toon Cell Shading material rendering pipeline
     * \see \ref rppdsps2_g3_toon
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_Toon_GrpAtmPipeID,             0x1007 ),
    /**<Grouped Toon Cell Shading atomic rendering pipeline
     * \see \ref rppdsps2_g3_toon
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_Toon_GrpSctPipeID,             0x1008 ),
    /**<Grouped Toon Cell Shading world sector rendering pipeline
     * \see \ref rppdsps2_g3_toon
     */

    /* Grouped Skinning Toon pipelines. */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkinToon_GrpMatPipeID,         0x1009 ),
    /**<Grouped Skinning Toon Cell Shading material rendering pipeline
     * \see \ref rppdsps2_g3_skintoon
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkinToon_GrpAtmPipeID,         0x100a ),
    /**<Grouped Skinning Toon Cell Shading atomic rendering pipeline
     * \see \ref rppdsps2_g3_skintoon
     */

    /* Grouped material-atomic and material-sector pipelines. */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_MatfxUV1_GrpMatPipeID,         0x100b ),
    /**<Grouped single UV set material effects material rendering pipeline
     * \see \ref rppdsps2_g3_matfx
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_MatfxUV2_GrpMatPipeID,         0x100c ),
    /**<Grouped double UV set material effects material rendering pipeline
     * \see \ref rppdsps2_g3_matfx
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_MatfxUV1_GrpAtmPipeID,         0x100d ),
    /**<Grouped single UV set material effects atomic rendering pipeline
     * \see \ref rppdsps2_g3_matfx
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_MatfxUV2_GrpAtmPipeID,         0x100e ),
    /**<Grouped double UV set material effects atomic rendering pipeline
     * \see \ref rppdsps2_g3_matfx
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_MatfxUV1_GrpSctPipeID,         0x100f ),
    /**<Grouped single UV set material effects world sector rendering pipeline
     * \see \ref rppdsps2_g3_matfx
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_MatfxUV2_GrpSctPipeID,         0x1010 ),
    /**<Grouped double UV set material effects world sector rendering pipeline
     * \see \ref rppdsps2_g3_matfx
     */

    /* Grouped material-atomic pipelines. */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkinfxUV1_GrpMatPipeID,        0x1011 ),
    /**<Grouped Skinning single UV set material effects material rendering pipeline
     * \see \ref rppdsps2_g3_skinfx
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkinfxUV2_GrpMatPipeID,        0x1012 ),
    /**<Grouped Skinning double UV set material effects material rendering pipeline
     * \see \ref rppdsps2_g3_skinfx
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkinfxUV1_GrpAtmPipeID,        0x1013 ),
    /**<Grouped Skinning single UV set material effects atomic rendering pipeline
     * \see \ref rppdsps2_g3_skinfx
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G3_SkinfxUV2_GrpAtmPipeID,        0x1014 ),
    /**<Grouped Skinning double UV set material effects atomic rendering pipeline
     * \see \ref rppdsps2_g3_skinfx
     */

    /* Grouped Generic and skinning BPatch object pipelines. */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_BPatch_GrpAtmPipeID,           0x1015 ),
    /**<Grouped Bezier Patch atomic rendering pipeline
     * \see \ref rppdsps2_g2_bpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_BPatch_GrpMatPipeID,           0x1016 ),
    /**<Grouped Bezier Patch material rendering pipeline
     * \see \ref rppdsps2_g2_bpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_SkinBPatch_GrpAtmPipeID,       0x1017 ),
    /**<Grouped Skinning Bezier Patch atomic rendering pipeline
     * \see \ref rppdsps2_g2_skinbpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_SkinBPatch_GrpMatPipeID,       0x1018 ),
    /**<Grouped Skinning Bezier Patch material rendering pipeline
     * \see \ref rppdsps2_g2_skinbpatch
     */

    /* Grouped Matfx BPatch pipelines. */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_MatfxUV1BPatch_GrpMatPipeID,   0x1019 ),
    /**<Grouped single UV set Bezier Patch material rendering pipeline
     * \see \ref rppdsps2_g2_matfxbpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_MatfxUV2BPatch_GrpMatPipeID,   0x101a ),
    /**<Grouped double UV set Bezier Patch material rendering pipeline
     * \see \ref rppdsps2_g2_matfxbpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_MatfxUV1BPatch_GrpAtmPipeID,   0x101b ),
    /**<Grouped single UV set Bezier Patch atomic rendering pipeline
     * \see \ref rppdsps2_g2_matfxbpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_MatfxUV2BPatch_GrpAtmPipeID,   0x101c ),
    /**<Grouped double UV set Bezier Patch atomic rendering pipeline
     * \see \ref rppdsps2_g2_matfxbpatch
     */

    /* Grouped Skin Matfx BPatch pipelines. */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_SkinfxUV1BPatch_GrpMatPipeID,  0x101d ),
    /**<Grouped Skinning single UV set Bezier Patch material rendering pipeline
     * \see \ref rppdsps2_g2_skinfxbpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_SkinfxUV2BPatch_GrpMatPipeID,  0x101e ),
    /**<Grouped Skinning double UV set Bezier Patch material rendering pipeline
     * \see \ref rppdsps2_g2_skinfxbpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_SkinfxUV1BPatch_GrpAtmPipeID,  0x101f ),
    /**<Grouped Skinning single UV set Bezier Patch atomic rendering pipeline
     * \see \ref rppdsps2_g2_skinfxbpatch
     */
    rpPDS_MAKEPLUGINPIPEID( rwPDS_G2_SkinfxUV2BPatch_GrpAtmPipeID,  0x1020 ),
    /**<Grouped Skinning double UV set Bezier Patch atomic rendering pipeline
     * \see \ref rppdsps2_g2_skinfxbpatch
     */

    /*=====================================================================*/

/*---- end: ./sky2/PDS_pipeIds/PDS_pluginPipes.h----*/
/*---- start: ./sky2/PDS_pipeIds/PDS_worldPipes.h----*/

    /*== Custom RenderWare pipelines. =====================================*/

    /*-- Generic Fast Object pipelines. -----------------------------------*/

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_Generic_AtmPipeID,             0x0001 ),
    /**<Object only Custom lighting Atomic pipeline.
     * Attached material pipelines should share their lighting setup.
     * \see \ref rppdsps2_g3x_generic
     */

    /*-- Generic Fast Lighting Material pipelines. ------------------------*/

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_APL_MatPipeID,                 0x0002 ),
    /**<Ambient and Point Light Generic rendering
     * \see \ref rppdsps2_g3x_apl
     * \see \ref rppdsps2_g3x_generic
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_ADL_MatPipeID,                 0x0003 ),
    /**<Ambient and Direction Light Generic rendering
     * \see \ref rppdsps2_g3x_adl
     * \see \ref rppdsps2_g3x_generic
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_A4D_MatPipeID,                 0x0004 ),
    /**<Ambient and 4 Direction Lights Generic rendering
     * \see \ref rppdsps2_g3x_a4d
     * \see \ref rppdsps2_g3x_generic
     */

    /*-- Dual Pass Fast Lighting Material pipelines. ----------------------*/

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_APLDup_MatPipeID,              0x0005 ),
    /**<Ambient and Point Light DUal Pass rendering
     * \see \ref rppdsps2_g3x_apldup
     * \see \ref rppdsps2_g3x_generic
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_ADLDup_MatPipeID,              0x0006 ),
    /**<Ambient and Direction Light DUal Pass rendering
     * \see \ref rppdsps2_g3x_adldup
     * \see \ref rppdsps2_g3x_generic
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_A4DDup_MatPipeID,              0x0007 ),
    /**<Ambient and 4 Direction Lights DUal Pass rendering
     * \see \ref rppdsps2_g3x_a4ddup
     * \see \ref rppdsps2_g3x_generic
     */

    /*-- Env Map Fast Lighting Material pipelines. ------------------------*/

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_APLGem_MatPipeID,              0x0008 ),
    /**<Ambient and Point Light Gloss Environment Map rendering
     * \see \ref rppdsps2_g3x_aplgem
     * \see \ref rppdsps2_g3x_generic
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_ADLGem_MatPipeID,              0x0009 ),
    /**<Ambient and Direction Light Gloss Environment Map rendering
     * \see \ref rppdsps2_g3x_adlgem
     * \see \ref rppdsps2_g3x_generic
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_A4DGem_MatPipeID,              0x000a ),
    /**<Ambient and 4 Direction Lights Gloss Environment Map rendering
     * \see \ref rppdsps2_g3x_a4dgem
     * \see \ref rppdsps2_g3x_generic
     */

    /*-- Generic Skin Fast Object pipelines. ------------------------------*/

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_Skin_AtmPipeID,                0x000b ),
    /**<Object only Custom lighting & adaptive skinning Atomic pipeline.
     * Attached material pipelines should share their lighting setup.
     * \see \ref rppdsps2_g3x_skin
     */

    /*-- Generic Skinning Fast Lighting Material pipelines. ---------------*/

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_APLSkin_MatPipeID,             0x000c ),
    /**<Ambient and Point Light Skin Generic rendering
     * \see \ref rppdsps2_g3x_aplskin
     * \see \ref rppdsps2_g3x_skin
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_ADLSkin_MatPipeID,             0x000d ),
    /**<Ambient and Direction Light Skin Generic rendering
     * \see \ref rppdsps2_g3x_adlskin
     * \see \ref rppdsps2_g3x_skin
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_A4DSkin_MatPipeID,             0x000e ),
    /**<Ambient and 4 Direction Lights Skin Generic rendering
     * \see \ref rppdsps2_g3x_a4dskin
     * \see \ref rppdsps2_g3x_skin
     */

    /*-- Dual Pass Skinning Fast Lighting Material pipelines. -------------*/

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_APLSkinDup_MatPipeID,          0x000f ),
    /**<Ambient and Point Light Skin DUal Pass rendering
     * \see \ref rppdsps2_g3x_aplskindup
     * \see \ref rppdsps2_g3x_skin
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_ADLSkinDup_MatPipeID,          0x0010 ),
    /**<Ambient and Direction Light Skin DUal Pass rendering
     * \see \ref rppdsps2_g3x_adlskindup
     * \see \ref rppdsps2_g3x_skin
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_A4DSkinDup_MatPipeID,          0x0011 ),
    /**<Ambient and 4 Direction Lights Skin DUal Pass rendering
     * \see \ref rppdsps2_g3x_a4dskindup
     * \see \ref rppdsps2_g3x_skin
     */

    /*-- Env Map Skinning Fast Lighting Material pipelines. ---------------*/

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_APLSkinGem_MatPipeID,          0x0012 ),
    /**<Ambient and Point Light Skin Gloss Environment Map rendering
     * \see \ref rppdsps2_g3x_aplskingem
     * \see \ref rppdsps2_g3x_skin
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_ADLSkinGem_MatPipeID,          0x0013 ),
    /**<Ambient and Direction Light Skin Gloss Environment Map rendering
     * \see \ref rppdsps2_g3x_adlskingem
     * \see \ref rppdsps2_g3x_skin
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_A4DSkinGem_MatPipeID,          0x0014 ),
    /**<Ambient and 4 Direction Lights Skin Gloss Environment Map rendering
     * \see \ref rppdsps2_g3x_a4dskingem
     * \see \ref rppdsps2_g3x_skin
     */

    /*-- Custom Cloning Fast Lighting pipelines. --------------------------*/

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_ADLClone_MatPipeID,            0x0015 ),
    /**<Ambient and Directional clone rendering
     * \see \ref rppdsps2_g3x_adlclone
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_ADLClone_AtmPipeID,            0x0016 ),
    /**<Grouped Custom lighting cloning Atomic pipeline.
     * All atomic's materials will be rendered with the grouped
     * \ref rwPDS_G3x_ADLClone_MatPipeID pipeline.
     * \see \ref rppdsps2_g3x_adlclone
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_OPLClone_MatPipeID,            0x0017 ),
    /**<Object pre-light clone rendering
     * \see \ref rppdsps2_g3x_oplclone
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_OPLClone_AtmPipeID,            0x0018 ),
    /**<Grouped Custom lighting cloning Atomic pipeline.
     * All atomic's materials will be rendered with the grouped
     * \ref rwPDS_G3x_OPLClone_MatPipeID pipeline.
     * \see \ref rppdsps2_g3x_oplclone
     */

    /*-- Custom Team Fast Lighting pipelines. -----------------------------*/

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_Team_MatPipeID,                0x0019 ),
    /**<Team static and 1 to 4 plane projected shadow rendering pipeline.
     *  This pipeline contains custom rendering code for the following
     * light combinations :-
     * \li 1 Ambient and 1 Directional lights,
     * \li 1 Ambient and 4 Directional lights,
     * \li 1 Ambient and 1 Point lights.
     *
     * The pipeline is highly custom and should only be used from within
     * the Team plugin.
     *
     * \see \ref rppdsps2_g3x_team
     */

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_DupTeam_MatPipeID,             0x001a ),
    /**<Team static dual pass and 1 to 4 plane projected shadow rendering
     * pipeline. This pipeline contains custom rendering code for the
     * following light combinations :-
     * \li 1 Ambient and 1 Directional lights,
     * \li 1 Ambient and 4 Directional lights,
     * \li 1 Ambient and 1 Point lights.
     *
     * The pipeline is highly custom and should only be used from within
     * the Team plugin.
     *
     * \see \ref rppdsps2_g3x_dupteam
     */

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_GemTeam_MatPipeID,             0x001b ),
    /**<Team static gloss enivronment mapping and 1 to 4 plane projected 
     * shadow rendering pipeline. This pipeline contains custom rendering 
     * code for the following light combinations :-
     * \li 1 Ambient and 1 Directional lights,
     * \li 1 Ambient and 4 Directional lights,
     * \li 1 Ambient and 1 Point lights.
     *
     * The pipeline is highly custom and should only be used from within
     * the Team plugin.
     *
     * \see \ref rppdsps2_g3x_gemteam
     */

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_SkinTeam_MatPipeID,            0x001c ),
    /**<Team ataptive skinning and 1 to 4 plane projected shadow rendering 
     * pipeline. This pipeline contains custom rendering code for the 
     * following light combinations :-
     * \li 1 Ambient and 1 Directional lights,
     * \li 1 Ambient and 4 Directional lights,
     * \li 1 Ambient and 1 Point lights.
     *
     * The pipeline is highly custom and should only be used from within
     * the Team plugin.
     *
     * \see \ref rppdsps2_g3x_skinteam
     */

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_SkinDupTeam_MatPipeID,         0x001d ),
    /**<Team ataptive skinning dual pass and 1 to 4 plane projected shadow 
     * rendering pipeline. This pipeline contains custom rendering code for
     * the following light combinations :-
     * \li 1 Ambient and 1 Directional lights,
     * \li 1 Ambient and 4 Directional lights,
     * \li 1 Ambient and 1 Point lights.
     *
     * The pipeline is highly custom and should only be used from within
     * the Team plugin.
     *
     * \see \ref rppdsps2_g3x_skindupteam
     */

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_SkinGemTeam_MatPipeID,         0x001e ),
    /**<Team ataptive skinning gloss environment mapping and 1 to 4 plane 
     * projected shadow rendering pipeline. This pipeline contains custom 
     * rendering code for the following light combinations :-
     * \li 1 Ambient and 1 Directional lights,
     * \li 1 Ambient and 4 Directional lights,
     * \li 1 Ambient and 1 Point lights.
     *
     * The pipeline is highly custom and should only be used from within
     * the Team plugin.
     *
     * \see \ref rppdsps2_g3x_skingemteam
     */

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_Team_AtmPipeID,                0x001f ),
    /**<Team object atomic pipeline. This pipeline should be attached to any
     * atomics using the custom team material pipelines.
     * The pipeline is highly custom and should only be used from within
     * the Team plugin.
     *
     * \see \ref rppdsps2_g3x_team
     * \see \ref rppdsps2_g3x_dupteam
     * \see \ref rppdsps2_g3x_gemteam
     */

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_SkinTeam_AtmPipeID,            0x0020 ),
    /**<Team object skinning atomic pipeline. This pipeline should be attached
     * to any atomics using the custom team skinning material pipelines.
     * The pipeline is highly custom and should only be used from within
     * the Team plugin.
     *
     * \see \ref rppdsps2_g3x_skinteam
     * \see \ref rppdsps2_g3x_skindupteam
     * \see \ref rppdsps2_g3x_skingemteam
     */

    /*-- UV Anim Fast Lighting Material pipelines. ------------------------*/

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_APLUva_MatPipeID,              0x0021 ),
    /**<Ambient and Point Light Texture Coordinate Transform rendering
     * \see \ref rppdsps2_g3x_apluva
     * \see \ref rppdsps2_g3x_generic
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_ADLUva_MatPipeID,              0x0022 ),
    /**<Ambient and Direction Light Texture Coordinate Transform rendering
     * \see \ref rppdsps2_g3x_adluva
     * \see \ref rppdsps2_g3x_generic
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_A4DUva_MatPipeID,              0x0023 ),
    /**<Ambient and 4 Direction Lights Texture Coordinate Transform rendering
     * \see \ref rppdsps2_g3x_a4duva
     * \see \ref rppdsps2_g3x_generic
     */

    /*-- UV Anim Dual Pass Fast Lighting Material pipelines. --------------*/

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_APLDupUva_MatPipeID,           0x0024 ),
    /**<Ambient and Point Light Texture Coordinate Transform 
     * Dual pass rendering
     * \see \ref rppdsps2_g3x_apldupuva
     * \see \ref rppdsps2_g3x_generic
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_ADLDupUva_MatPipeID,           0x0025 ),
    /**<Ambient and Direction Light Texture Coordinate Transform
     * Dual pass rendering
     * \see \ref rppdsps2_g3x_adldupuva
     * \see \ref rppdsps2_g3x_generic
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_A4DDupUva_MatPipeID,           0x0026 ),
    /**<Ambient and 4 Direction Lights Texture Coordinate Transform
     * Dual pass rendering
     * \see \ref rppdsps2_g3x_a4ddupuva
     * \see \ref rppdsps2_g3x_generic
     */

    /*-- UV Anim Skinning Fast Lighting Material pipelines. ---------------*/

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_APLSkinUva_MatPipeID,          0x0027 ),
    /**<Ambient and Point Light Texture Coordinate Transform 
     * Skinned rendering
     * \see \ref rppdsps2_g3x_aplskinuva
     * \see \ref rppdsps2_g3x_skin
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_ADLSkinUva_MatPipeID,          0x0028 ),
    /**<Ambient and Direction Light Texture Coordinate Transform
     * Skinned rendering
     * \see \ref rppdsps2_g3x_adlskinuva
     * \see \ref rppdsps2_g3x_skin
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_A4DSkinUva_MatPipeID,          0x0029 ),
    /**<Ambient and 4 Direction Lights Texture Coordinate Transform
     * Skinned rendering
     * \see \ref rppdsps2_g3x_a4dskinuva
     * \see \ref rppdsps2_g3x_skin
     */

    /*-- UV Anim Dual Pass Skinning Fast Lighting Material pipelines. -----*/

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_APLSkDupUva_MatPipeID,         0x002a ),
    /**<Ambient and Point Light Texture Coordinate Transform 
     * Dual pass Skinned rendering
     * \see \ref rppdsps2_g3x_aplskindupuva
     * \see \ref rppdsps2_g3x_skin
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_ADLSkDupUva_MatPipeID,         0x002b ),
    /**<Ambient and Direction Light Texture Coordinate Transform
     * Dual pass Skinned rendering
     * \see \ref rppdsps2_g3x_adlskindupuva
     * \see \ref rppdsps2_g3x_skin
     */
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3x_A4DSkDupUva_MatPipeID,         0x002c ),
    /**<Ambient and 4 Direction Lights Texture Coordinate Transform
     * Dual pass Skinned rendering
     * \see \ref rppdsps2_g3x_a4dskindupuva
     * \see \ref rppdsps2_g3x_skin
     */

    /*-- Custom lighting ADC Static Material pipelines. -------------------*/

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_APL_MatPipeID,                0x002d ),
    /**<Ambient and Point Light 
     * Generic rendering
     * \see \ref rppdsps2_g3xd_apl
     * \see \ref rppdsps2_g3x_generic
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_ADL_MatPipeID,                0x002e ),
    /**<Ambient and Directional Light 
     * Generic rendering
     * \see \ref rppdsps2_g3xd_adl
     * \see \ref rppdsps2_g3x_generic
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_A4D_MatPipeID,                0x002f ),
    /**<Ambient and 4 Directional Lights 
     * Generic rendering
     * \see \ref rppdsps2_g3xd_a4d
     * \see \ref rppdsps2_g3x_generic
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_APLDup_MatPipeID,             0x0030 ),
    /**<Ambient and Point Light 
     * Dual Pass rendering
     * \see \ref rppdsps2_g3xd_apldup
     * \see \ref rppdsps2_g3x_generic
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_ADLDup_MatPipeID,             0x0031 ),
    /**<Ambient and Directional Light 
     * Dual Pass rendering
     * \see \ref rppdsps2_g3xd_adldup
     * \see \ref rppdsps2_g3x_generic
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_A4DDup_MatPipeID,             0x0032 ),
    /**<Ambient and 4 Directional Lights 
     * Dual Pass rendering
     * \see \ref rppdsps2_g3xd_a4ddup
     * \see \ref rppdsps2_g3x_generic
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_APLGem_MatPipeID,             0x0033 ),
    /**<Ambient and Point Light 
     * Gloss Environment Mapping rendering
     * \see \ref rppdsps2_g3xd_aplgem
     * \see \ref rppdsps2_g3x_generic
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_ADLGem_MatPipeID,             0x0034 ),
    /**<Ambient and Directional Light
     * Gloss Environment Mapping rendering
     * \see \ref rppdsps2_g3xd_adlgem
     * \see \ref rppdsps2_g3x_generic
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_A4DGem_MatPipeID,             0x0035 ),
    /**<Ambient and 4 Directional Lights
     * Gloss Environment Mapping rendering
     * \see \ref rppdsps2_g3xd_a4dgem
     * \see \ref rppdsps2_g3x_generic
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_APLUva_MatPipeID,             0x0036 ),
    /**<Ambient and Point Light
     * Texture Coordinate Transform rendering
     * \see \ref rppdsps2_g3xd_apluva
     * \see \ref rppdsps2_g3x_generic
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_ADLUva_MatPipeID,             0x0037 ),
    /**<Ambient and Directional Light
     * Texture Coordinate Transform rendering
     * \see \ref rppdsps2_g3xd_adluva
     * \see \ref rppdsps2_g3x_generic
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_A4DUva_MatPipeID,             0x0038 ),
    /**<Ambient and 4 Directional Lights
     * Texture Coordinate Transform rendering
     * \see \ref rppdsps2_g3xd_a4duva
     * \see \ref rppdsps2_g3x_generic
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_APLDupUva_MatPipeID,          0x0039 ),
    /**<Ambient and Point Light 
     * Texture Coordinate Transform Dual pass rendering
     * \see \ref rppdsps2_g3xd_apldupuva
     * \see \ref rppdsps2_g3x_generic
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_ADLDupUva_MatPipeID,          0x003a ),
    /**<Ambient andDirectional Light 
     * Texture Coordinate Transform Dual pass rendering
     * \see \ref rppdsps2_g3xd_adldupuva
     * \see \ref rppdsps2_g3x_generic
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_A4DDupUva_MatPipeID,          0x003b ),
    /**<Ambient and4 Directional Lights 
     * Texture Coordinate Transform Dual pass rendering
     * \see \ref rppdsps2_g3xd_a4ddupuva
     * \see \ref rppdsps2_g3x_generic
     */

    /*-- Custom lighting ADC Skinning Material pipelines. -----------------*/

    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_APLSkin_MatPipeID,            0x003c ),
    /**<Ambient and Point Light
     * Skin Generic rendering
     * \see \ref rppdsps2_g3xd_aplskin
     * \see \ref rppdsps2_g3x_skin
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_ADLSkin_MatPipeID,            0x003d ),
    /**<Ambient and Directional Light
     * Skin Generic rendering
     * \see \ref rppdsps2_g3xd_adlskin
     * \see \ref rppdsps2_g3x_skin
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_A4DSkin_MatPipeID,            0x003e ),
    /**<Ambient and 4 Directionl Lights
     * Skin Generic rendering
     * \see \ref rppdsps2_g3xd_a4dskin
     * \see \ref rppdsps2_g3x_skin
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_APLSkinDup_MatPipeID,         0x003f ),
    /**<Ambient and Point Light
     * Skin Dual Pass rendering
     * \see \ref rppdsps2_g3xd_aplskindup
     * \see \ref rppdsps2_g3x_skin
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_ADLSkinDup_MatPipeID,         0x0040 ),
    /**<Ambient and Directional Light
     * Skin Dual Pass rendering
     * \see \ref rppdsps2_g3xd_adlskindup
     * \see \ref rppdsps2_g3x_skin
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_A4DSkinDup_MatPipeID,         0x0041 ),
    /**<Ambient and 4 Directionl Lights
     * Skin Dual Pass rendering
     * \see \ref rppdsps2_g3xd_a4dskindup
     * \see \ref rppdsps2_g3x_skin
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_APLSkinGem_MatPipeID,         0x0042 ),
    /**<Ambient and Point Light
     * Skin Gloss Environment Mapping rendering
     * \see \ref rppdsps2_g3xd_aplskingem
     * \see \ref rppdsps2_g3x_skin
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_ADLSkinGem_MatPipeID,         0x0043 ),
    /**<Ambient and Directional Light
     * Skin Gloss Environment Mapping rendering
     * \see \ref rppdsps2_g3xd_adlskingem
     * \see \ref rppdsps2_g3x_skin
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_A4DSkinGem_MatPipeID,         0x0044 ),
    /**<Ambient and 4 Directionl Lights
     * Skin Gloss Environment Mapping rendering
     * \see \ref rppdsps2_g3xd_a4dskingem
     * \see \ref rppdsps2_g3x_skin
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_APLSkinUva_MatPipeID,         0x0045 ),
    /**<Ambient and Point Light
     * Skin Texture Coordinate Transform rendering
     * \see \ref rppdsps2_g3xd_aplskinuva
     * \see \ref rppdsps2_g3x_skin
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_ADLSkinUva_MatPipeID,         0x0046 ),
    /**<Ambient and Directional Light
     * Skin Texture Coordinate Transform rendering
     * \see \ref rppdsps2_g3xd_adlskinuva
     * \see \ref rppdsps2_g3x_skin
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_A4DSkinUva_MatPipeID,         0x0047 ),
    /**<Ambient and 4 Directionl Lights
     * Skin Texture Coordinate Transform rendering
     * \see \ref rppdsps2_g3xd_a4dskinuva
     * \see \ref rppdsps2_g3x_skin
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_APLSkDupUva_MatPipeID,        0x0048 ),
    /**<Ambient and Point Light
     * Skin Texture Coordinate Transform Dual pass rendering
     * \see \ref rppdsps2_g3xd_aplskdupuva
     * \see \ref rppdsps2_g3x_skin
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_ADLSkDupUva_MatPipeID,        0x0049 ),
    /**<Ambient and Directional Light
     * Skin Texture Coordinate Transform Dual pass rendering
     * \see \ref rppdsps2_g3xd_adlskdupuva
     * \see \ref rppdsps2_g3x_skin
     */
     
    rpPDS_MAKEWORLDPIPEID( rwPDS_G3xd_A4DSkDupUva_MatPipeID,        0x004a ),
    /**<Ambient and 4 Directionl Lights
     * Skin Texture Coordinate Transform Dual pass rendering
     * \see \ref rppdsps2_g3xd_a4dskdupuva
     * \see \ref rppdsps2_g3x_skin
     */

    /*=====================================================================*/

/*---- end: ./sky2/PDS_pipeIds/PDS_worldPipes.h----*/

    rpPDSMAXPIPEID,
    rpPDSPIPEIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RpPDSPipeID RpPDSPipeID;

#endif /* (defined(SKY2_DRVMODEL_H) || defined(NULLSKY_DRVMODEL_H)) */

typedef struct RpExpPipeTransform RpExpPipeTransform;

struct RpExpPipeTransform
{
    RwUInt32 fromPipe;
    RwUInt32 toPipe;
};

#ifdef __cplusplus
extern "C"
{
#endif  /* __cplusplus */

/* Atomic API */

extern RpExpPipeTransform *
RpAtomicExpPipeTransformsAllocate(RpAtomic *atomic, RwInt32 numTransforms);

extern RpExpPipeTransform *
RpAtomicGetExpPipeTransforms(RpAtomic *atomic);

extern RwInt32
RpAtomicGetNumExpPipeTransforms(RpAtomic *atomic);

extern void
RpAtomicEnableExpPipeStreaming(RpAtomic *atomic, RwBool enable);

/* World API */

extern RpExpPipeTransform *
RpWorldExpPipeTransformsAllocate(RpWorld *world, RwInt32 numTransforms);

extern RpExpPipeTransform *
RpWorldGetExpPipeTransforms(RpWorld *world);

extern RwInt32
RpWorldGetNumExpPipeTransforms(RpWorld *world);

extern void
RpWorldEnableExpPipeStreaming(RpWorld *world, RwBool enable);

/* Plugin API */

extern RwBool
RpExpPipesPluginAttach(void);

extern RpAtomic *
RpAtomicOverloadPipelines(RpAtomic *atomic);

extern RpWorld *
RpWorldOverloadPipelines(RpWorld *world);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _EXPPIPES_H_ */