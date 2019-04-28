#include "eeregs.h"
#include "eestruct.h"
#include "libgraph.h"

#include "rwcore.h"
#include "rpworld.h"

#include "utils.h"
#include "fullscreenfx.h"

extern RwUInt64 VCLFullScreen __attribute__ ((section(".vudata")));

static RwUInt32 SOffset[3] = {0, 0, 8};
static RwUInt32 TOffset[3] = {0, 2, 0};


/*
 *****************************************************************************
 */
void 
FullScreenCopyChannel(RwRaster *dest, RwRaster *src, 
    RwRaster *palette, RwUInt32 channel)
{
    RwUInt32 width, height;
    RwUInt32 i;
    RwUInt64 tmp, tmp1;
    RwSplitBits q;
    RwUInt32 destAddr, srcAddr, clutAddr;
    RwUInt32 xOffset, yOffset;
    
    /* Ensure palette raster is in GS memory and get addresses. */
    RpSkyTexCacheAccessRaster(palette, FALSE);
    destAddr = RpSkyTexCacheRasterGetAddr(dest) >> 5;
    srcAddr = RpSkyTexCacheRasterGetAddr(src) >> 5;
    clutAddr = RpSkyTexCacheRasterGetAddr(palette) >> 5;
        
    width = dest->originalWidth;
    height = dest->originalHeight;
    xOffset = 32768 - (width << 3);
    yOffset = 32768 - (height << 3);
    
    /* Upload microprogram if it isn't already in VU1 micro memory */
    if (&VCLFullScreen != skyUploadedCode)
    {
        _rwDMAOpenVIFPkt(0, 1);
        
        tmp = 0 << 0 | DMATAG_CALL << 28 | (RwUInt64)&VCLFullScreen << 32;
        tmp1 = SCE_VIF1_SET_NOP(0) | (RwUInt64)SCE_VIF1_SET_FLUSH(0) << 32;
        ADDTOPKT(tmp1, tmp);
        
        skyUploadedCode = &VCLFullScreen;
    }
    
    /* Upload constants*/
    _rwDMAOpenVIFPkt(0, 76);
    RWDMA_LOCAL_BLOCK_BEGIN();
    
    tmp = 75 << 0 | DMATAG_CNT << 28;
    tmp1 = SCE_VIF1_SET_STCYCL(1, 1, 0) 
        | (RwUInt64)SCE_VIF1_SET_UNPACK(0, 68, VIFCMD_UNPACKV4_32, 0) << 32;
    ADDTOPKT(tmp1, tmp);
    
    tmp = SOffset[channel] | (RwUInt64)TOffset[channel] << 32;
    ADDTOPKT(0, tmp);
    
    ADDTOPKT32(xOffset, yOffset, width > 512 ? 55 
        : 8 * ((width - 1) / 64) - 1, 4 * ((height - 1) / 32) - 1);

    tmp = SCE_GIF_SET_TAG(1, 0, 0, 0, 0, 1);
    ADDTOPKT(SCE_GIF_PACKED_AD, tmp);
    
    tmp = SCE_GIF_SET_TAG(32, 1, 1, 
        SCE_GS_SET_PRIM(6, 0, 1, 0, 1, 0, 0, 0, 0), 0, 2);
    tmp1 = SCE_GS_ST | SCE_GS_XYZF2 << 4;
    ADDTOPKT(tmp1, tmp);
    
    for (i = 0; i < 64; i++)
    {
        tmp = SCE_GS_SET_CLAMP(3, 0, 7, 8 + 16 * i, 0, 0); 
        ADDTOPKT(SCE_GS_CLAMP_1, tmp);
    }    
    
    /* Setup GS registers */
    ADDTOPKT32(SCE_VIF1_SET_NOP(0), SCE_VIF1_SET_NOP(0), 
        SCE_VIF1_SET_NOP(0), SCE_VIF1_SET_DIRECT(5, 0));
    
    tmp = SCE_GIF_SET_TAG(4, 1, 0, 0, 0, 1);
    ADDTOPKT(0xel, tmp);
    
    q.nReal = 1024.0f;
    ADDTOPKT(SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(128, 128, 128, 128, q.nUInt));
    
    ADDTOPKT(SCE_GS_FRAME_1,
        SCE_GS_SET_FRAME(destAddr, width / 64, SCE_GS_PSMCT32, 0));
    
    ADDTOPKT(SCE_GS_TEX0_1, SCE_GS_SET_TEX0(srcAddr << 5, width / 32,
        SCE_GS_PSMT8, 10, 10, 1, 1, clutAddr << 5, SCE_GS_PSMCT32, 0, 0, 1));
    
    ADDTOPKT(SCE_GS_TEX1_1, SCE_GS_SET_TEX1(0, 0, 0, 0, 0, 0, 0));
        
    /* Execute microprogram */
    ADDTOPKT32(SCE_VIF1_SET_NOP(0), SCE_VIF1_SET_NOP(0), 
        SCE_VIF1_SET_MSCAL(0, 0), SCE_VIF1_SET_FLUSH(0));
    
    RWDMA_LOCAL_BLOCK_END();
    
    /* If the width or height is greater than 512 the source buffer cannot be
       read as one 8bit texture so it is split up (this code assumes the height
       is never greater than 512). */
    if (width > 512)
    {
        /* Upload constants */
        _rwDMAOpenVIFPkt(0, 6);
        RWDMA_LOCAL_BLOCK_BEGIN();
        
        tmp = 5 << 0 | DMATAG_CNT << 28;
        tmp1 = SCE_VIF1_SET_STCYCL(1, 1, 0) 
            | (RwUInt64)SCE_VIF1_SET_UNPACK(1, 1, VIFCMD_UNPACKV4_32, 0) << 32;
        ADDTOPKT(tmp1, tmp);
        
        ADDTOPKT32(8192 + xOffset, yOffset, 
            8 * ((width - 513) / 64) - 1, 4 * ((height - 1) / 32) - 1);

        /* Setup GS registers */
        ADDTOPKT32(SCE_VIF1_SET_NOP(0), SCE_VIF1_SET_NOP(0), 
            SCE_VIF1_SET_NOP(0), SCE_VIF1_SET_DIRECT(2, 0));
        
        tmp = SCE_GIF_SET_TAG(1, 1, 0, 0, 0, 1);
        ADDTOPKT(0xel, tmp);
        
        ADDTOPKT(SCE_GS_TEX0_1, SCE_GS_SET_TEX0(
            (8 + srcAddr) << 5, width / 32, SCE_GS_PSMT8, 10, 10, 1, 1, 
            clutAddr << 5, SCE_GS_PSMCT32, 0, 0, 1));
                    
        /* Execute microprogram */
        ADDTOPKT32(SCE_VIF1_SET_NOP(0), SCE_VIF1_SET_NOP(0), 
            SCE_VIF1_SET_MSCAL(0, 0), SCE_VIF1_SET_FLUSH(0));
        
        RWDMA_LOCAL_BLOCK_END();
    }
}