#include "src/fullscreen.h"

--enter

.init_vf_all
.init_vi_all

--endenter

;*****************************************************************************

    LOI         16.0
    MAXi.w      STScale, VF00, I

    ; Calculate ST offsets
    LQ.xy       STOffsetInt, VCLFullscreenSTOffset(VI00)
    ITOF0.xy    STOffset, STOffsetInt
    LOI         0.5
    ADDi.xy     STOffset0, STOffset, I
    ADDi.x      STOffset4, STOffset, I
    LOI         2.5
    ADDi.y      STOffset1, STOffset, I
    LOI         4.5
    ADDi.xy     STOffset2, STOffset, I
    ADDi.x      STOffset6, STOffset, I
    LOI         6.5
    ADDi.y      STOffset3, STOffset, I
    LOI         8.5
    ADDi.x      STOffset1, STOffset, I
    ADDi.y      STOffset4, STOffset, I
    ADDi.x      STOffset5, STOffset, I
    LOI         10.5
    ADDi.y      STOffset5, STOffset, I
    LOI         12.5
    ADDi.x      STOffset3, STOffset, I
    ADDi.y      STOffset6, STOffset, I
    ADDi.x      STOffset7, STOffset, I
    LOI         14.5
    ADDi.y      STOffset7, STOffset, I

    ; calculate XY offsets
    LQ.xy       XYOffsetInt, VCLFullscreenXYOffset(VI00)
    ITOF4.xy    XYOffset0, XYOffsetInt
    MOVE.x      XYOffset2, XYOffset0
    MOVE.x      XYOffset4, XYOffset0
    MOVE.x      XYOffset6, XYOffset0
    LOI         2.0
    ADDi.y      XYOffset1, XYOffset0, I
    ADDi.y      XYOffset2, XYOffset0, I
    LOI         4.0
    ADDi.y      XYOffset3, XYOffset0, I
    ADDi.y      XYOffset4, XYOffset0, I
    LOI         6.0
    ADDi.y      XYOffset5, XYOffset0, I
    ADDi.y      XYOffset6, XYOffset0, I
    LOI         8.0
    ADDi.x      XYOffset1, XYOffset0, I
    ADDi.x      XYOffset3, XYOffset0, I
    ADDi.x      XYOffset5, XYOffset0, I
    ADDi.xy     XYOffset7, XYOffset0, I

    MAXx.w      XY1Int, VF00, VF00
    MAXx.w      XY3Int, VF00, VF00
    MAXx.w      XY5Int, VF00, VF00
    MAXx.w      XY7Int, VF00, VF00
    IADDIU      Output1, VI00, VCLFullscreenOutput1
    IADDIU      Output2, VI00, VCLFullscreenOutput2
    LQ.xyzw     ClampTag, VCLFullscreenClampTag(VI00)
    LQ.xyzw     GIFtag, VCLFullscreenGIFtag(VI00)

    ISUBIU      PageXLimit, VI00, 9
    ISUBIU      PageYLimit, VI00, 5

    ILW.z       PageXCoord, VCLFullscreenXYOffset(VI00)
    PageXLoop:;-------------------------------------------------------------------

    ILW.w       PageYCoord, VCLFullscreenXYOffset(VI00)
    PageYLoop:;-------------------------------------------------------------------

    IADDIU      BlockXCoord, PageXCoord, 8
    BlockXLoop:;------------------------------------------------------------------

    IADDIU      Output, Output1, 0
    SQI.xyzw    ClampTag, (Output++)
    LQ.xyzw     ClampReg, VCLFullscreenClampRegs(BlockXCoord)
    SQI.xyzw    ClampReg, (Output++)
    SQI.xyzw    GIFtag, (Output++)

    MFIR.x      BlockCoordsInt, BlockXCoord
    IADDIU      BlockYCoord, PageYCoord, 4
    BlockYLoop:;------------------------------------------------------------------
    --loopCS 4,1

    MFIR.y      BlockCoordsInt, BlockYCoord
    ITOF0.xy    BlockCoords, BlockCoordsInt
    MULw.xy     BlockSTOffset, BlockCoords, STScale
    MULi.xy     BlockXYOffset, BlockCoords, I

    ; generate ST coordinates
    ADD.xy      ST0, BlockSTOffset, STOffset0
    ADD.xy      ST1, BlockSTOffset, STOffset1
    ADD.xy      ST2, BlockSTOffset, STOffset2
    ADD.xy      ST3, BlockSTOffset, STOffset3
    ADD.xy      ST4, BlockSTOffset, STOffset4
    ADD.xy      ST5, BlockSTOffset, STOffset5
    ADD.xy      ST6, BlockSTOffset, STOffset6
    ADD.xy      ST7, BlockSTOffset, STOffset7

    ; generate positons
    ADD.xy      XY0, BlockXYOffset, XYOffset0
    ADD.xy      XY1, BlockXYOffset, XYOffset1
    ADD.xy      XY2, BlockXYOffset, XYOffset2
    ADD.xy      XY3, BlockXYOffset, XYOffset3
    ADD.xy      XY4, BlockXYOffset, XYOffset4
    ADD.xy      XY5, BlockXYOffset, XYOffset5
    ADD.xy      XY6, BlockXYOffset, XYOffset6
    ADD.xy      XY7, BlockXYOffset, XYOffset7

    ; convert positions
    FTOI4.xy    XY0Int, XY0
    FTOI4.xy    XY1Int, XY1
    FTOI4.xy    XY2Int, XY2
    FTOI4.xy    XY3Int, XY3
    FTOI4.xy    XY4Int, XY4
    FTOI4.xy    XY5Int, XY5
    FTOI4.xy    XY6Int, XY6
    FTOI4.xy    XY7Int, XY7

    ; store positions and tex coordinates
    SQI.xy      ST0, (Output++)
    SQI.xy      XY0Int, (Output++)
    SQI.xy      ST1, (Output++)
    SQI.xyw     XY1Int, (Output++)
    SQI.xy      ST2, (Output++)
    SQI.xy      XY2Int, (Output++)
    SQI.xy      ST3, (Output++)
    SQI.xyw     XY3Int, (Output++)
    SQI.xy      ST4, (Output++)
    SQI.xy      XY4Int, (Output++)
    SQI.xy      ST5, (Output++)
    SQI.xyw     XY5Int, (Output++)
    SQI.xy      ST6, (Output++)
    SQI.xy      XY6Int, (Output++)
    SQI.xy      ST7, (Output++)
    SQI.xyw     XY7Int, (Output++)

    ISUBIU      BlockYCoord, BlockYCoord, 1
    ;-----------------------------------------------------------------------------
    IBNE        BlockYCoord, PageYCoord, BlockYLoop

    XGKICK      Output1
    IADD        OutputTemp, VI00, Output1
    IADD        Output1, VI00, Output2
    IADD        Output2, VI00, OutputTemp

    ISUBIU      BlockXCoord, BlockXCoord, 1
    ;-----------------------------------------------------------------------------
    IBNE        BlockXCoord, PageXCoord, BlockXLoop

    ISUBIU      PageYCoord, PageYCoord, 4
    ;-----------------------------------------------------------------------------
    IBNE        PageYCoord, PageYLimit, PageYLoop

    ISUBIU      PageXCoord, PageXCoord, 8
    ;-----------------------------------------------------------------------------
    IBNE        PageXCoord, PageXLimit, PageXLoop

;*****************************************************************************

--exit
--endexit
