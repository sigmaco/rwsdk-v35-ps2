// RwRf3Shader.h: interface for the RwRf3Shader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWRF3SHADER_H__B0E44A75_E8BD_4ADA_AC7F_9AB0F7CE1877__INCLUDED_)
#define AFX_RWRF3SHADER_H__B0E44A75_E8BD_4ADA_AC7F_9AB0F7CE1877__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwRf3.h"
#include "RwRf3Object.h"
#include "RwRf3Param.h"

#include "RwCommShader.h"
#include "RwCommTexture.h"
#include "RwRf3ObjCreator.h"

#define RWRF3_SHADER_TAG                "shader"
#define RWRF3_SHADER_FUNC               "shaderfunc"
#define RWRF3_SHADER_FUNCTYPE           "material"
#define RWRF3_SHADER_DIFFUSE            "diffuse"
#define RWRF3_SHADER_AMBIENT            "ambient"
#define RWRF3_SHADER_SPECULAR           "specular"
#define RWRF3_SHADER_SHININESS          "shininess"
#define RWRF3_SHADER_SHINESTRENGTH      "shinestrength"
#define RWRF3_SHADER_TRANSPARENCY       "transparency"
#define RWRF3_SHADER_SELFILLUM          "selfillumination"
#define RWRF3_SHADER_DOUBLESIDED        "twosided"

#define RWRF3_SHADER_COLORBLENDFUNC     "colorblend"
#define RWRF3_SHADER_BUMPBLENDFUNC      "bumpblend"
#define RWRF3_SHADER_ENVBLENDFUNC       "envblend"
#define RWRF3_SHADER_ALPHABLENDFUNC     "transparencyblend"
#define RWRF3_SHADER_NORMALBLENDFUNC    "normalblend"

#define RWRF3_SHADER_CHANNEL            "channel"
#define RWRF3_SHADER_CHANNELNAME        "name"
#define RWRF3_SHADER_CHANNELUVSET       "uvset"
#define RWRF3_SHADER_CHANNELCOLOR       "color"
#define RWRF3_SHADER_CHANNELBUMP        "bump"
#define RWRF3_SHADER_CHANNELENV         "env"
#define RWRF3_SHADER_CHANNELALPHA       "transparency"
#define RWRF3_SHADER_CHANNELNORMAL      "normal"
#define RWRF3_SHADER_CHANNELFAC         "factor"

class RwRf3Exporter;
class RwMIShader;
struct RwRf3Channel;

/**
*   \ingroup rf3write
*
*   This class is responsible for exporting RwCommShader objects. In its Export() method the class
*   exports the shader's material properties (using ExportMaterialProperties()) and its texure channel
*   information, using the ExportTextures() method. It then adds its local XML m_pNode under the scene's
*   global shader list.
*
*   \see RwCommShader
*/
class RF3_API RwRf3ExpShader  : public RwRf3ExpObject
{
public:
	RwRf3ExpShader();
	virtual ~RwRf3ExpShader();

    virtual RwCommError     Export( RwCommObject* pObject,
                                RwCommObject* pParent,
                                RwCommScene* pScene,
                                const RwCommExportOptions* pExportOptions );

protected:

    virtual RwCommError     ExportMaterialProperties(RwCommShader* pShader);
    virtual RwCommError     ExportTextures(RwCommShader* pShader, RwCommObject *pParent, RwRf3Exporter *pExporter);
    virtual RwCommError     ExportBlendFuncs(const string &blendName, vector<RwCommShader::RwCommBlendFunction> &vFuncs);

    RwCommError     AddChannel(RwCommShader* pShader, set<RwRf3Channel, RwRf3Channel> &sChannels,
                        RwCommShader::TextureType, const string &blendFuncName);

    RwCommShader            *GetInstance(RwCommShader* pShader);
    RwCommError             GetActiveTextures(RwCommShader* pShader, vector<RwCommTexture*>& vActiveTextures) const;

protected:

    RwXMLNode    *m_pShaderFunc;
    RwRf3Param  *m_pDiffuse;
    RwRf3Param  *m_pSpecular;
    RwRf3Param  *m_pAmbient;
    RwRf3Param  *m_pShininess;
    RwRf3Param  *m_pShineStrength;
    RwRf3Param  *m_pTransparency;
    RwRf3Param  *m_pSelfIllum;
    
};


// Shader creator
/**
*   \ingroup rf3read
*
*   This class builds an RwMIShader from an XML node. 
*
*   \see RwMIShader
*/
class RF3_API RwRf3ShaderCreator : public RwRf3ObjCreator
{
public:
    RwRf3ShaderCreator() {}
    virtual ~RwRf3ShaderCreator() {}

	virtual RwCommObject* Create(const RwCommCreationData* pCreationData,
								const RwCommExportOptions* pExportOptions) const;

	virtual void Attach(RwCommObject* pObject, RwCommObject* pParent, RwCommScene* pScene,
                        const RwCommCreationData* pCreationData) const;

	virtual void Init(	RwCommObject* pObject,
						const RwCommCreationData* pCreationData,
						const RwCommExportOptions* pExportOptions) const;

protected:

    virtual void BuildShaderFunc(RwXMLNode *pNode, RwCommShader *pShader, RwRf3Builder* pBuilder) const;
    virtual void BuildChannel(RwXMLNode *pNode, RwCommShader *pShader, RwRf3Builder* pBuilder) const;
    void    GetBlendFuncs(RwXMLNode *pShaderFunc, const string &blendFunc,
                vector<RwCommShader::RwCommBlendFunction> &vFuncs) const;

};

#endif // !defined(AFX_RWRF3SHADER_H__B0E44A75_E8BD_4ADA_AC7F_9AB0F7CE1877__INCLUDED_)
