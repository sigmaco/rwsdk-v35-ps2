;***************************************************************************
;*
;* This file is a product of Criterion Software Ltd.
;*
;* This file is provided as is with no warranties of any kind and is
;* provided without any obligation on Criterion Software Ltd.
;* or Canon Inc. to assist in its use or modification.
;*
;* Criterion Software Ltd. and Canon Inc. will not, under any
;* circumstances, be liable for any lost revenue or other damages
;* arising from the use of this file.
;*
;* Copyright (c) 2001 Criterion Software Ltd.
;* All Rights Reserved.
;*
;*
;****************************************************************************
;*
;* spritetrans.vcl
;*
;* Copyright (C) 2001 Criterion Technologies.
;*
;* Original author: Alexandre Hadjdaj.
;*
;* Purpose: a Sprite Transform pipeline
;*          this file should be preprocessed using VCL 1.23 before 
;*          beeing compiled using ee-dvp-as
;****************************************************************************/

--enter

.init_vf_all	      			
.init_vi_all	      			

.name Innerloop
in_vf	Matrix01(VF01)	; VF01 Contain Matrix #1 vector
in_vf	Matrix02(VF02)	; VF02 Contain Matrix #2 vector
in_vf	Matrix03(VF03)	; VF03 Contain Matrix #3 vector
in_vf	Matrix04(VF04)	; VF04 Contain Matrix #4 vector
in_vf	Clipvec1(VF26)	; VF04 Contain Clipping vector-1
in_vf	Clipvec2(VF24)	; VF04 Contain Clipping vector-2			
in_vf	PosOffset(VF30)	; VF30 Contain the GS&VP offset
--endenter


;**********************************************************************************************************************
; Set up the output buffers - this is only done when the rendering is started. We switch buffers from then on.

XTOP        InBuf1                                                  ; Get and save Input Buffer 1
IADDIU      InBuf2, VI00, vuSymbVIFOffset                           ; Calc and save Input Buffer 2
IADDIU      OutBuf1, VI00, vuSymbOutputBuffer1                      ; Calc and save Output Buffer 1
IADDIU      OutBuf2, VI00, vuSymbOutputBuffer2                      ; Calc and save Output Buffer 2

doAllSprites:
    IADD        OutBuf, VI00, OutBuf1                               ; Get Output Buffer
    IADD        InBuf, VI00, InBuf1                                 ; Get Input buffer
    XITOP       NumVert                                             ; Get number of vertices
    
;**********************************************************************************************************************
; Set up my GIFtag
    LQ.yzw      ftmp, vuSymbSpriteGifTag(VI00)                      ; Load custom GifTag
    IADDIU      itmp, VI00, 0x4000                                  ; Generate EOP step 1
    IADD        itmp, itmp, itmp                                    ; Generate EOP step 2 
    IOR         itmp, itmp, NumVert                                 ; OR EOP with the number of vertices
    SQ.yzw      ftmp, 0(OutBuf)                                     ; Write GIFTAG
    ISW.x       itmp, 0(OutBuf)                                     ; Write EOP
    IADD        KickAdd, VI00, OutBuf                               ; Save the Kick Addresse
    IADDIU      OutBuf, OutBuf, 1                                   ; Increment Output Buffer
    FCSET       00                                                  ; Set clipping Flag to Zero (VCL compatibility, not really needed)
    
    spriteLoop:
        ; Minimum 3 sprites ( based on TriList )
        --LoopCS	1,4

        LQ.xyz      V1Pos, (0*vertexSize)+inPos(InBuf)              ; Load input Position
        LQ.xyzw     V1RGBA, (0*vertexSize)+inRGBA(InBuf)            ; Load Color
        LQ.xy       V1HeWi, (0*vertexSize)+inHW(InBuf)              ; Load Height and Widthe
        LQ.xyzw     V1UV, (0*vertexSize)+inUV1(InBuf)               ; Load Top/Left and Bottom/Right UVs

        ADDz.x      V1UV2,VF00,V1UV                                 ; Expand UV2 from V1UV to V1UV2 - step 1
        ADDw.y      V1UV2,VF00,V1UV                                 ; Expand UV2 from V1UV to V1UV2 - step 2

    	MULAw.xyzw  ACC, Matrix04, VF00                             ; Transform Position - step 1
    	MADDAx.xyw  ACC, Matrix01, V1POS                            ; Transform Position - step 2
        MADDAy.xyw  ACC, Matrix02, V1POS                            ; Transform Position - step 3
        MADDz.xyzw  V1POS, Matrix03, V1POS                          ; Transform Position - step 4

        DIV         Q, VF00w, V1POSw                                ; Calculate Z recip (1/w)

    	MOVE.z      V1HeWi, VF00                                    ; Clear V1HeWi.z 
        MR32.w      V1HeWi, VF00                                    ; Clear V1HeWi.w

    	SUB.xyzw    V1POSa, V1POS, V1HeWi                           ; Offset Top/Left Position 
    	
    	ADD.xyzw    V1POSb, V1POS, V1HeWi                           ; Offset Bottom/Right Position 

        ADDA.z      ACC, VF00, Clipvec1                             ; Prepare Z Mapping

        MUL.xy      ZMapa, V1POSa, Clipvec2                         ; Calculate T/L Vertex Z Mapping step 1
        MADDw.z     ZMapa, Clipvec2, V1POSa                         ; Calculate T/L Vertex Z Mapping step 2
        
        MUL.xy      ZMapb, V1POSb, Clipvec2                         ; Calculate B/R Vertex Z Mapping step 1
        MADDw.z     ZMapb, Clipvec2, V1POSb                         ; Calculate B/R Vertex Z Mapping step 2

    	CLIPw.xyz   ZMapa, V1Posa                                   ; Clipping Test of T/L Vertex
    	CLIPw.xyz   ZMapb, V1Posb                                   ; Clipping Test of B/R Vertex

        FCAND       VI01, 0x0fff			                        ; Get the 2 last CLIP test (clipping flag 0000 0000 0000 1111 1111 1111)
        IADDIU 	    ClipRes,VI01,0x7fff                             ; Format Clip Resultat

        MULq.xyz    V1Posa, V1Posa, Q                               ; Project T/L Vertex
        MULq.xyz    V1Posb, V1Posb, Q                               ; Project B/R Vertex

        ADD.xyz     V1Posa, V1Posa, PosOffset                       ; Offset T/L Vertex by GS&VP
        ADD.xyz     V1Posb, V1Posb, PosOffset                       ; Offset B/R Vertex by GS&VP

		FTOI4.xyz	V1Posaf,V1Posa									; Convert T/L Vertex
		FTOI4.xyz	V1Posbf,V1Posb									; Convert B/R Vertex

    	ADD.xy      V1UVout, VF00, V1UV                             ; Prepare T/L Vertex's UVs Step 1
    	ADDw.z      V1UVout, VF00, VF00                             ; Prepare T/L Vertex's UVs Step 2

    	ADD.xy      V1UV2out, VF00, V1UV2                           ; Prepare B/R Vertex's UVs Step 1
    	ADDw.z      V1UV2out, VF00, VF00                            ; Prepare B/R Vertex's UVs Step 2
        
        SQ.xyz      V1UVout, (0*outVertexSize)+outSTa(OutBuf)       ; Write T/L Vertex UVs
        SQ.xyzw     V1RGBA, (0*outVertexSize)+outRGBA(OutBuf)       ; Write Sprite Color
        SQ.xyz      V1Posaf, (0*outVertexSize)+outPosa(OutBuf)       ; Write T/L vertex position
        SQ.xyz      V1UV2out, (0*outVertexSize)+outSTb(OutBuf)      ; Write B/R vertex UVs
        SQ.xyz      V1Posbf, (0*outVertexSize)+outPosb(OutBuf)       ; Write B/R Vertex position
        ISW.w  	    ClipRes, (0*outVertexSize)+outPosb(OutBuf)      ; Write ADC Flag according to the Clipping results

        IADDIU      OutBuf, OutBuf, (1*outVertexSize)               ; Increment Output Buffer
        ISUBIU      NumVert, NumVert, 1                             ; Decrement Number of sprite
        
        IADDIU      InBuf, InBuf, (1*vertexSize)                    ; Increment Input Buffer

    IBGTZ      NumVert, spriteLoop                                  ; Loop against Number of Vertex
       
    ;**********************************************************************************************************************
    ; XGKICK the drawing now

    XGKICK      KickAdd                                             ; Kick Output Buffer 

    IADD        itmp,VI00,InBuf1                                    ; Swap Input Buffer 1 & 2 - step 1
    IADD        InBuf1,VI00,InBuf2                                  ; Swap Input Buffer 1 & 2 - step 2
    IADD        InBuf2,VI00,itmp                                    ; Swap Input Buffer 1 & 2 - step 3

    IADD        itmp,VI00,OutBuf1                                   ; Swap Output Buffer 1 & 2 - step 1
    IADD        OutBuf1,VI00,OutBuf2                                ; Swap Output Buffer 1 & 2 - step 2
    IADD        OutBuf2,VI00,itmp                                   ; Swap Output Buffer 1 & 2 - step 3

    --cont                                                          ; Exit if no more batch to do

B   doAllSprites                                                    ; Proceed to next Batch


--exit
--endexit
