// RwExpRwTexture.h: interface for the RwExpRwTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWEXPRWTEXTURE_H__8EB75E7E_3AD0_41BF_9129_0A797987472B__INCLUDED_)
#define AFX_RWEXPRWTEXTURE_H__8EB75E7E_3AD0_41BF_9129_0A797987472B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwExp.h"
#include "RwCommExportObject.h"
#include "RwCommImage.h"

#define RWEXP_TEXTURE_KVALUE "RwK"
#define RWEXP_TEXTURE_LVALUE "RwL"
#define RWEXP_TEXTURE_ANISOTROPY "RwMaxAnisotropy"

class RwCommTexture;

/**
*  \ingroup exportobject
*   This is the class responsible for exporting RwTextures from RwCommTextures. In its main Export() method the
*   RwExpRwTexture class extracts the RwCommImage from the RwCommTexture, builds an RwRaster from it,
*   creates an RwTexture from the raster, and sets the textures filtering mode, addressing mode, K&L values and
*   max anisotropy settings.
*   
*   Users can modify the exported texture image by overriding the ExtractImage() method.
*   
*   If the "embed in texture dictionary" scene setting is checked, the texture is added to the texture
*   dictionary. Textures whose names are already in the texture dictionary are not recreated.
*   If the exported texture is a color texture (the main diffuse texture) then it looks if a transparency 
*   texture is present. If it is present, it creates a texture with the alpha values burned in it. 
*   For this reason, transparency textures are never exported separately, and if a RwExpRwTexture class 
*   discovers it is exporting a transparency texture then it does nothing.
*/
class RWEXP_API RwExpRwTexture : public  RwCommExportObject
{
public:
	RwExpRwTexture();
	virtual ~RwExpRwTexture();

    virtual RwCommError Export( RwCommObject* pObj,
                                RwCommObject* pParent,
                                RwCommScene* pScene,
                                const RwCommExportOptions* pExportOptions );

    
    virtual RwCommError AddUserData(RwCommUserDataCollection &userDataColl);

    /** Get the generated RwTexture object.*/
    virtual RwTexture   *GetRwTexture() const { return m_pTexture; }

    virtual RwCommError CreateRwTexture(RwCommTexture* pTexture, RwCommTexture* pAlphaTexture,
                                                                           RwRaster *pRaster);
    virtual RwCommError SampleRwImage(RwCommTexture* pTexture, RwCommTexture* pAlphaTexture,
                            RwImage *&pRwImage,
                            RwInt32 &rasterWidth, RwInt32 &rasterHeight,
                            RwInt32 &rasterDepth, RwInt32 &rasterFlags);

    virtual const RwCommImageInfo &GetSourceImageInfo() const { return m_sourceInfo; };
    virtual const string &GetSourceImageName() const { return m_sSourceName; };

protected:

    // Overridables
    virtual RwCommError ExtractImage(RwCommTexture *pTexture, RwCommTexture *pAlphaTexture, RwCommImage &image);
    
    void                ProcessTextureName(string &textureName, const RwCommExportOptions *pOptions);
    RwRaster            *CreateDummyTextureRaster(RwCommTexture *pTexture);
    RwCommError         CreateTextureRaster(RwCommTexture *pTexture, RwCommTexture *pAlphaTexture,
                                                RwRaster *&pRaster);
    RwImage             *RwCommImageToRwImage(RwCommImage &image) const;
    RwTextureFilterMode GetFilterMode(const RwCommTexture *pTexture) const;
    void                SetAddressingMode(const RwCommTexture *pTexture);
    string              GetTextureName(RwCommTexture *pTexture);

protected:

    RwTexture       *m_pTexture;

    RwCommImageInfo m_sourceInfo;
    string          m_sSourceName;
};

#endif // !defined(AFX_RWEXPRWTEXTURE_H__8EB75E7E_3AD0_41BF_9129_0A797987472B__INCLUDED_)
