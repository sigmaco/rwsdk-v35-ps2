// OptTextureExporter.cpp: implementation of the OptTextureExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OptTextureExporter.h"

#include "RwCommImage.h"
#include "RwCommtexture.h"

#define OPTIMIZED_TEXTURE_SIZE 64

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OptTextureExporter::OptTextureExporter()
{

}

OptTextureExporter::~OptTextureExporter()
{

}

void
OptTextureExporter::Destroy()
{
    delete this;
}

RwCommError     
OptTextureExporter::Export(RwCommObject *pObj, RwCommObject *pParent,RwCommScene *pScene,
                           const RwCommExportOptions *pExportOptions )
{
    // Call the base class implementation
    return RwExpRwTexture::Export(pObj, pParent, pScene, pExportOptions);
}

// This method should create a valid RwCommImage object. 
// The RwExpRwTexture class will create the RwTexture from this image.
RwCommError 
OptTextureExporter::ExtractImage(RwCommTexture *pTexture, RwCommTexture *pAlphaTexture, RwCommImage &image)
{
    RwCommError eResult = RWCOMM_FAIL;
    RwCommImageInfo info;
    int w = OPTIMIZED_TEXTURE_SIZE, h = OPTIMIZED_TEXTURE_SIZE;

    // Calculate new optimized texture size
    if (pTexture->GetImageInfo(info) == RWCOMM_SUCCESS)
    {
        if (info.width < OPTIMIZED_TEXTURE_SIZE)
        {
            w = info.width;
        }

        if (info.height < OPTIMIZED_TEXTURE_SIZE)
        {
            h = info.height;
        }
    }

    // Get the image, using the new size
    eResult = pTexture->GetImage(image, w, h, pAlphaTexture);

    return eResult;
}
