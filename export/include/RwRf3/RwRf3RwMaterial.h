// RwRf3RwMaterial.h: interface for the RwRf3RwMaterial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWRF3RWMATERIAL_H__E074A264_2200_4815_B661_68246A43C3A4__INCLUDED_)
#define AFX_RWRF3RWMATERIAL_H__E074A264_2200_4815_B661_68246A43C3A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwRf3Shader.h"
#include "RwCommRwMaterial.h"

#define RWRF3_RWMATERIAL_SHADERFUNC         "rwmaterial"
#define RWRF3_RWMATERIAL_XBOX_EFFECT        "xboxeffect"
#define RWRF3_RWMATERIAL_BASECOLOR          "basecolor"
#define RWRF3_RWMATERIAL_SURFACE_PROPS      "surfaceprops"
#define RWRF3_RWMATERIAL_MATFX_FLAGS        "matfx"

#define RWRF3_RWMATERIAL_GC_EFFECT          "gceffect"
#define RWRF3_RWMATERIAL_TOON_PAINT         "toonpaint"

#define RWRF3_RWMATERIAL_CHANNELXBOX        "xbox"
#define RWRF3_RWMATERIAL_CHANNELGC          "gc"

#define RWRF3_RWMATERIAL_LMDENSITY           "lightmapdensity"
#define RWRF3_RWMATERIAL_LMFLAGS             "lightmapflags"
#define RWRF3_RWMATERIAL_LMALCOLOR           "lightmapalcolor"
#define RWRF3_RWMATERIAL_LMALDENSITY         "lightmapaldensity"
#define RWRF3_RWMATERIAL_LMALRADIUS          "lightmapalradius"

#define RWRF3_RWMATERIAL_NMENVFACTOR         "normalmapenv"
#define RWRF3_RWMATERIAL_NMCONVERTALPHA      "normalmapconvertalpha"

/**
*   \ingroup rf3write
*
*   This class is responsible for exporting RwCommRwMaterial objects. In its Export() method the class
*   calls RwRf3ExpShader::Export(), and then exports the base color, the surface properties, the MatFX
*   flags, the toon paint name, the light map information and the normal map information.
*
*   \see RwCommRwMaterial
*/
class RF3_API RwRf3ExpRwMaterial : public RwRf3ExpShader 
{
public:
	RwRf3ExpRwMaterial();
	virtual ~RwRf3ExpRwMaterial();

    virtual RwCommError     Export( RwCommObject* pObj,
                                RwCommObject* pParent,
                                RwCommScene* pScene,
                                const RwCommExportOptions* pExportOptions );
protected:

    virtual RwCommError     ExportTextures(RwCommShader* pShader, RwCommObject *pParent, RwRf3Exporter *pExporter);
    virtual RwCommError     ExportBaseColor(RwCommRwMaterial* pShader);
    virtual RwCommError     ExportSurfaceProps(RwCommRwMaterial* pShader);
    virtual RwCommError     ExportMatFXFlags(RwCommRwMaterial* pShader);
    virtual RwCommError     ExportXBoxExtension(RwCommRwMaterial* pShader, RwCommObject *pParent, RwRf3Exporter *pExporter);
    virtual RwCommError     ExportGCExtension(RwCommRwMaterial* pShader, RwCommObject *pParent, RwRf3Exporter *pExporter);
    virtual RwCommError     ExportToonPaint(RwCommRwMaterial* pShader);
    virtual RwCommError     ExportLightMapInfo(RwCommRwMaterial* pShader);
    virtual RwCommError     ExportNormalMapInfo(RwCommRwMaterial* pShader);

    RwCommError             ExportTextures(RwCommRwMaterial* pShader, const string &channel,
                                vector<RwCommTexture*> &vTextures, RwRf3Exporter *pExporter);

};

/**
*   \ingroup rf3read
*
*   This class builds an RwMIRwMaterial from an XML node. 
*
*   \see RwMIShader
*/
class RF3_API RwRf3RwMaterialCreator : public RwRf3ShaderCreator
{
public:
    RwRf3RwMaterialCreator() {}
    virtual ~RwRf3RwMaterialCreator() {}

	virtual RwCommObject* Create(const RwCommCreationData* pCreationData,
								const RwCommExportOptions* pExportOptions) const;

protected:

    virtual void BuildShaderFunc(RwXMLNode *pNode, RwCommShader *pShader, RwRf3Builder* pBuilder) const;
    virtual void BuildChannel(RwXMLNode *pNode, RwCommShader *pShader, RwRf3Builder* pBuilder) const;

    void    GetChannelTextures(RwXMLNode *pNode, vector<RwCommTexture*> &vTextures, RwRf3Builder* pBuilder,
                RwCommScene *pScene) const;

};

#endif // !defined(AFX_RWRF3RWMATERIAL_H__E074A264_2200_4815_B661_68246A43C3A4__INCLUDED_)
