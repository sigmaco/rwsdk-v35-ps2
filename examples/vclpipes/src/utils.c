#include "libgraph.h"
#include "rwcore.h"
#include "utils.h"

void
SetGSReg(RwUInt64 reg, RwUInt64 value)
{
    _rwDMAOpenVIFPkt(RWDMA_FIXUP, 2);
    RWDMA_LOCAL_BLOCK_BEGIN();
    ADDTOPKT(SCE_GIF_PACKED_AD, SCE_GIF_SET_TAG(1, 1, 0, 0, SCE_GIF_PACKED, 1));
    ADDTOPKT(reg, value);
    RWDMA_LOCAL_BLOCK_END();       
}

