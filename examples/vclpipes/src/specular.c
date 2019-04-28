#include "rwcore.h"
#include "rpworld.h"
#include "rppds.h"
#include "specular.h"
#include "skeleton.h"

#include "utils.h"
#include "eeregs.h"
#include "eestruct.h"
#include "libgraph.h"


/*
 *****************************************************************************
 */
static void
SpecularProcessPixel(RwRGBA *pixel)
{
    pixel->alpha = (RwUInt8)(255 * (RwUInt32)pixel->alpha / 128);
} 


/*
 *****************************************************************************
 */
void
SpecularProcessRaster(RwRaster *raster)
{
    RSASSERT(rwRASTERFORMAT8888 & RwRasterGetFormat(raster));
    
    /* Process each pixel or palette entry */
    if ((rwRASTERFORMATPAL4 | rwRASTERFORMATPAL8) 
        & RwRasterGetFormat(raster))
    {
        RwRGBA *palette;
        RwUInt32 i;
        
        palette = (RwRGBA *)RwRasterLockPalette(raster, 
            rwRASTERLOCKREAD | rwRASTERLOCKWRITE);
        
        for (i = 0; i < 256; i++)
        {
            SpecularProcessPixel(palette++);
        }
        
        RwRasterUnlockPalette(raster);
    }
    else
    {
        RwRGBA *pixels;
        RwUInt32 width, height, offset;
        RwUInt32 i, j, k;
        
        /* Remove mipmap auto generation flag as each mip level must be 
           processed. */
        raster->cFormat &= ~(rwRASTERFORMATAUTOMIPMAP >> 8);
        
        /* Process each mip level */
        for (i = 0; i < (RwUInt32)RwRasterGetNumLevels(raster); i++)
        {
            pixels = (RwRGBA *)RwRasterLock(raster, i,
                rwRASTERLOCKREAD | rwRASTERLOCKWRITE);
            width = RwRasterGetWidth(raster);
            height = RwRasterGetHeight(raster);
            offset = (RwRasterGetStride(raster) >> 2) - width;
                
            for (j = 0; j < height; j++)
            {
                for (k = 0; k < width; k++)
                {
                    SpecularProcessPixel(pixels++);
                }
                
                pixels += offset;
            }                    
            
            RwRasterUnlock(raster);
        }
    }
}


/*
 *****************************************************************************
 */
void
SpecularProcess(const RwRaster *dst, RwReal intensity)
{
    RwUInt32 width, height;
    RwUInt8 color;
    
    width = dst->originalWidth;
    height = dst->originalHeight;
    color = (RwUInt8)(128.0f * intensity);
    
    /* Open VIF1 DMA packet to set renderstates, draw a sprite and then reset
       renderstates. */
    _rwDMAOpenVIFPkt(RWDMA_FIXUP, 10);
    RWDMA_LOCAL_BLOCK_BEGIN();    
    ADDTOPKT(SCE_GIF_PACKED_AD, SCE_GIF_SET_TAG(9, 1, 1, 
        SCE_GS_SET_PRIM(6, 1, 0, 0, 1, 0, 0, 0, 0), SCE_GIF_PACKED, 1));
    
    /* Disable z writes */
    ADDTOPKT(SCE_GS_ZBUF_1, skyZbuf_1 | (1l << 32));
    /* Disable z test */
    ADDTOPKT(SCE_GS_TEST_1, SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 1, 1));       
    /* Set blend mode to (Cs-0)Ad+Cd */
    ADDTOPKT(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(0, 2, 1, 1, 0));
    /* Set color and color. */
    ADDTOPKT(SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(color, color, color, 0, 0));
    
    /* Draw a fullscreen sprite to add alpha to RGB. This can be done more
       efficiently as 32 pixel wide strips. */
    ADDTOPKT(SCE_GS_XYZF2, 
        SCE_GS_SET_XYZF(32768 - 8 * width, 32768 - 8 * height, 0, 0));
    ADDTOPKT(SCE_GS_XYZF2, 
        SCE_GS_SET_XYZF(32768 + 8 * width, 32768 + 8 * height, 0, 0));
    
    /* Restore renderstates */
    ADDTOPKT(SCE_GS_ALPHA_1, skyAlpha_1);
    ADDTOPKT(SCE_GS_TEST_1, skyTest_1);
    ADDTOPKT(SCE_GS_ZBUF_1, skyZbuf_1);
    
    RWDMA_LOCAL_BLOCK_END();                        
}


/*
 *****************************************************************************
 */
