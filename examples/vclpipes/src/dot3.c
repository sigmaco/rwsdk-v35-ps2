#include "rwcore.h"
#include "rpworld.h"
#include "rppds.h"
#include "dot3.h"
#include "skeleton.h"

#include "utils.h"
#include "eeregs.h"
#include "eestruct.h"
#include "libgraph.h"
#include "fullscreenfx.h"


/*
 *****************************************************************************
 */
RwRaster *
Dot3CreateRaster(void)
{
    RwRaster *raster;
    RwRGBA *pixels;
    RwUInt32 i;
    RwUInt32 entry;    
    RwUInt8 x, y;
    
    /* Create a 16x16 32bit texture to use as a palette for the last fullscreen
       pass. */
    raster = RwRasterCreate(
        16, 16, 32, rwRASTERTYPETEXTURE | rwRASTERFORMAT8888);
    pixels = (RwRGBA *)RwRasterLock(raster, 0, rwRASTERLOCKWRITE);
    
    for (i = 0; i < 256; i++)
    {
        x = 134 + 2 * i;
        y = x < 128 ? 0 : 2 * x;
        entry = (i & 0xe7) | ((i & 8) << 1) | ((i & 16) >> 1);
        pixels[entry].red = y;
        pixels[entry].green = y;
        pixels[entry].blue = y;
        pixels[entry].alpha = 255;    
    }
    
    RwRasterUnlock(raster);
    
    return raster;
}


/*
 *****************************************************************************
 */
static void
Dot3ProcessPixel(RwRGBA *pixel)
{
    RwV3d normal;
    
    /* Calculate unit normal vector. */
    normal.x = (RwReal)pixel->red / 128.0f - 1.0f;
    normal.y = (RwReal)pixel->green / 128.0f - 1.0f;
    normal.z = (RwReal)pixel->blue / 128.0f - 1.0f;
    RwV3dNormalize(&normal, &normal);
    
    /* Encode normal as RGB. The length of normal must be <= 1 so each
       channel is rounded towards 128. */
    pixel->red = normal.x < 0.0f 
        ? 128 - (RwUInt8)(126.99f * RwRealAbs(normal.x)) 
        : 128 + (RwUInt8)(126.99f * RwRealAbs(normal.x));
    pixel->green = normal.y < 0.0f 
        ? 128 - (RwUInt8)(126.99f * RwRealAbs(normal.y)) 
        : 128 + (RwUInt8)(126.99f * RwRealAbs(normal.y));
    pixel->blue = normal.z < 0.0f 
        ? 128 - (RwUInt8)(126.99f * RwRealAbs(normal.z)) 
        : 128 + (RwUInt8)(126.99f * RwRealAbs(normal.z));
        
    /* Calculate alpha channel value needed to correct bias. */        
    pixel->alpha = (128 - pixel->red - pixel->green - pixel->blue) / 2;
} 


/*
 *****************************************************************************
 */
void
Dot3ProcessRaster(RwRaster *raster)
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
            Dot3ProcessPixel(palette++);
        }
        
        RwRasterUnlockPalette(raster);
    }
    else
    {
        RwRGBA *pixels;
        RwUInt32 width, height, offset;
        RwUInt32 i, j, k;
        
        /* Remove mipmap auto generation flag as each mip level must be 
           processed separately. */
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
                    Dot3ProcessPixel(pixels++);
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
Dot3Process(RwRaster *dst, RwRaster *raster, RwUInt32 tempAddr, 
    RwBool attenuation)
{
    RwBool zTestEnable;
    RwBool zWriteEnable;
    RwTextureFilterMode filterMode;
    RwTextureAddressMode addressMode;
    RwRaster *textureRaster;
    RwUInt32 i, j, k;
    RwUInt32 width, height;
    
    /* 
     * If we are using a third pass to write the light attenuation to the
     * alpha channel the frame buffer contains:
     * R = 64n_x(1 + l_x)
     * G = 64n_y(1 + l_y)
     * B = 64n_z(1 + l_z) + 64(2 - n_x - n_y - n_z)
     * A = 128a
     * 
     * where: 
     * n = (n_x, n_y, n_z) is the normal from the normal map.
     * l = (l_x, l_y, l_z) is the light direction.
     * a is the attenuation.
     * 
     * If we aren't using a third pass the frame buffer contains:
     * R = 64n_x(1 + l_x)
     * G = 64n_y(1 + l_y)
     * B = 64n_z(1 + l_z)
     * A = 64(2 - n_x - n_y - n_z)
     */
     
    width = dst->originalWidth;
    height = dst->originalHeight;
    
    RwRenderStateGet(rwRENDERSTATETEXTUREADDRESS, &addressMode);
    RwRenderStateGet(rwRENDERSTATETEXTURERASTER, &textureRaster);
    RwRenderStateGet(rwRENDERSTATETEXTUREFILTER, &filterMode);
    RwRenderStateGet(rwRENDERSTATEZTESTENABLE, &zTestEnable);
    RwRenderStateGet(rwRENDERSTATEZWRITEENABLE, &zWriteEnable);
    
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)rwFILTERNEAREST);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)FALSE);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)FALSE);

    /* If we only rendered two passes then add the alpha channel to the blue
       channel (if we rendered three passes then the third pass will have 
       already done this. This can be done more efficiently as 32 pixel wide
       strips. */
    if (!attenuation)
    {
        /* R += A */                        
        _rwDMAOpenVIFPkt(RWDMA_FIXUP, 5);

        RWDMA_LOCAL_BLOCK_BEGIN();
        
        ADDTOPKT(SCE_GIF_PACKED_AD, SCE_GIF_SET_TAG(4, 1, 1, 
            SCE_GS_SET_PRIM(6, 1, 0, 0, 1, 0, 1, 0, 0), SCE_GIF_PACKED, 1));
        ADDTOPKT(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(0, 2, 1, 1, 0));
        ADDTOPKT(SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(0, 0, 128, 0, 0));
        ADDTOPKT(SCE_GS_XYZF2, 
            SCE_GS_SET_XYZF(32768 - 8 * width, 32768 - 8 * height, 0, 0));
        ADDTOPKT(SCE_GS_XYZF2, 
            SCE_GS_SET_XYZF(32768 + 8 * width, 32768 + 8 * height, 0, 0));

        RWDMA_LOCAL_BLOCK_END();
    }    

    /* Add red and green channels to the blue channel. The red and green 
       channels of each page are copied into the blue and alpha channels
       of the temporary buffer. The alpha channel of the temporary buffer
       is then added to the blue channel which is then added to the frame
       buffer blue channel. */
    for (i = 0; i < height; i += 32)
    {
        for (j = 0; j < width; j += 64)
        {
            _rwDMAOpenVIFPkt(RWDMA_FIXUP, 38);

            RWDMA_LOCAL_BLOCK_BEGIN();
    
            /* B2 = R, A2 = G */
            ADDTOPKT(SCE_GIF_PACKED_AD, SCE_GIF_SET_TAG(19, 1, 1, 
                SCE_GS_SET_PRIM(6, 1, 1, 0, 0, 0, 1, 0, 0), 
                SCE_GIF_PACKED, 1));
            ADDTOPKT(SCE_GS_FRAME_1, SCE_GS_SET_FRAME(
                tempAddr >> 5, width / 64, SCE_GS_PSMCT16, 0));
            ADDTOPKT(SCE_GS_TEX0_1, SCE_GS_SET_TEX0((skyFrame_1 & 0x1ff) << 5,
                width / 64, SCE_GS_PSMCT16, 10, 10, 1, 1, 0, 0, 0, 0, 0));
            
            for (k = 0; k < 64; k += 16)
            {
                ADDTOPKT(SCE_GS_UV, 
                    SCE_GS_SET_UV(8 + 16 * k + 16 * j, 8 + 32 * i));
                ADDTOPKT(SCE_GS_XYZF2, SCE_GS_SET_XYZF(32768 - 8 * width 
                    + 16 * (8 + k), 32768 - 8 * height, 0, 0));
                ADDTOPKT(SCE_GS_UV, SCE_GS_SET_UV(
                    8 + 16 * (8 + k) + 16 * j, 8 + 16 * 64 + 32 * i));
                ADDTOPKT(SCE_GS_XYZF2, SCE_GS_SET_XYZF(32768 - 8 * width 
                    + 16 * (16 + k), 32768 - 8 * height + 16 * 64, 0, 0));
            }
            ADDTOPKT(SCE_GS_TEXFLUSH, 0l);

            /* B2 += A2 */      
            ADDTOPKT(SCE_GIF_PACKED_AD, SCE_GIF_SET_TAG(8, 1, 1, 
                SCE_GS_SET_PRIM(6, 0, 0, 0, 1, 0, 1, 0, 0),
                SCE_GIF_PACKED, 1));
            ADDTOPKT(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(0, 2, 1, 1, 0));
            ADDTOPKT(SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(0, 0, 128, 0, 0));
            ADDTOPKT(SCE_GS_FRAME_1,  SCE_GS_SET_FRAME(tempAddr >> 5,
                width / 64, SCE_GS_PSMCT32, 0xff00ffff00000000l));
            ADDTOPKT(SCE_GS_UV, SCE_GS_SET_UV(8, 8));
            ADDTOPKT(SCE_GS_XYZF2, 
                SCE_GS_SET_XYZF(32768 - 8 * width, 32768 - 8 * height, 0, 0));
            ADDTOPKT(SCE_GS_UV, SCE_GS_SET_UV(1032, 520));
            ADDTOPKT(SCE_GS_XYZF2, 
                SCE_GS_SET_XYZF(33792 - 8 * width, 33280 - 8 * height, 0, 0));
            ADDTOPKT(SCE_GS_TEXFLUSH, 0l);
            
            /* B += B2 */
            ADDTOPKT(SCE_GIF_PACKED_AD, SCE_GIF_SET_TAG(8, 1, 1, 
                SCE_GS_SET_PRIM(6, 1, 1, 0, 1, 0, 1, 0, 0), 
                SCE_GIF_PACKED, 1));
            ADDTOPKT(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(0, 2, 2, 1, 0x80));
            ADDTOPKT(SCE_GS_FRAME_1, skyFrame_1 | 0xff00ffff00000000l);
            ADDTOPKT(SCE_GS_TEX0_1, SCE_GS_SET_TEX0(tempAddr, 1, 
                SCE_GS_PSMCT32, 10, 10, 1, 1, 0, 0, 0, 0, 0));
            ADDTOPKT(SCE_GS_UV, SCE_GS_SET_UV(8, 8));
            ADDTOPKT(SCE_GS_XYZF2, SCE_GS_SET_XYZF(32768 - 8 * width 
                + 16 * j, 32768 - 8 * height + 16 * i, 0, 0));
            ADDTOPKT(SCE_GS_UV, SCE_GS_SET_UV(1032, 520));
            ADDTOPKT(SCE_GS_XYZF2, SCE_GS_SET_XYZF(32768 - 8 * width 
                + 16 * (64 + j), 32768 - 8 * height + 16 * (32 + i), 0, 0));
            ADDTOPKT(SCE_GS_TEXFLUSH, 0l);
            
            RWDMA_LOCAL_BLOCK_END();
        }
    }
    
    /* We now have B = 128 + 64(n_xl_x + n_yl_y + n_zl_z). As we aren't
       texturing the model we just read the blue channel as an 8bit texture
       scaling the output colors by the attenuation if we stored it in
       the alpha channel. If texturing the light intensity should be written
       to the alpha channel instead of the RGB channels (if using attenuation
       this will require writing the light intensity scaled by the attenuation
       to the green channel and then copying this to the alpha channel). The
       model should then be rendered again with a suitable blend mode
       (e.g. SRCBLEND=DESTALPHA, DESTBLEND=ZERO). */
    if (attenuation)
    {
        SetGSReg(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(0, 2, 1, 2, 0));
    }
    else
    {        
        SetGSReg(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(0, 2, 2, 2, 0x80));
    }        
    
    FullScreenCopyChannel( RpSkyGetDrawBufferRaster(),
                           RpSkyGetDrawBufferRaster(),
                           raster, 2 );
    
    /* Reset renderstates */
    SetGSReg(SCE_GS_FRAME_1, skyFrame_1);
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void *)addressMode);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, textureRaster);
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)filterMode);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)zTestEnable);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)zWriteEnable);
}


/*
 *****************************************************************************
 */
