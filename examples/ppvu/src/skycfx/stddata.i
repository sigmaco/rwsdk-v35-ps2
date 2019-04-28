; ----------------------------------------------------------------------
; Warning: This file will be processed using a rather simple sed script
; to turn it into a C include file.
; ----------------------------------------------------------------------

; Enums
.ifndef vuEnumLightNaLight
; Warning these must be reconciled with the C enums.
; Check auto-generated file "baequate.i"

.equ vuEnumLightNaLight, 0      ; rpNALIGHTTYPE
.equ vuEnumLightDirectional, 1  ; rpLIGHTDIRECTIONAL
.equ vuEnumLightAmbient, 2      ; rpLIGHTAMBIENT
.equ vuEnumLightPoint, 128      ; rpLIGHTPOINT
.equ vuEnumLightSpot, 129       ; rpLIGHTSPOT
.equ vuEnumLightSpotSoft, 130   ; rpLIGHTSPOTSOFT

.endif

; Data in high memory

.ifndef vuSDmat0

.equ vuSDLightOffset, 	 0x3d0
.equ vuSDBlockLow, 		 0x3c3
.equ vuSDBlockHigh, 	 0x3f0
.equ vuSDmat0, 			 vuSDBlockHigh
.equ vuSDmat1, 			 vuSDBlockHigh+1
.equ vuSDmat2, 			 vuSDBlockHigh+2
.equ vuSDmat3, 			 vuSDBlockHigh+3
.equ vuSDnearClip,  	 vuSDBlockHigh+4
.equ vuSDfarClip, 		 vuSDBlockHigh+5
.equ vuSDxMaxyMax, 		 vuSDBlockHigh+6
.equ vuSDcamWcamHzScale, vuSDBlockHigh+7
.equ vuSDoffXoffYzShift, vuSDBlockHigh+8
.equ vuSDrealOffset, 	 vuSDBlockHigh+9
.equ vuSDgifTag, 		 vuSDBlockHigh+10
.equ vuSDcolScale, 		 vuSDBlockHigh+11
.equ vuSDsurfProps, 	 vuSDBlockHigh+12
.equ vuSDpingPongAddr, 	 vuSDBlockHigh+13
.equ vuSDpingPongCount,  vuSDBlockHigh+14
.equ vuSDClipvec1, 	 	 vuSDBlockHigh+13
.equ vuSDClipvec2, 		 vuSDBlockHigh+14
.equ vuSDVUSwitch, 		 vuSDBlockHigh+15
.endif

;------------- Custom FX ALLOCATIONS ------------------------
.equ vuSymbTransv,   (vuSDLightOffset-1)									;
.equ vuSymbMtxR0,    (vuSymbTransv-1)										;
.equ vuSymbGifTagc2, (vuSymbMtxR0-1)
                                                             ;
;----------------------------------------------------------------------------

;------------------ BUFFERS STRIDE ------------------------------------------
.equ vuSymbStrideOfInputCluster, 4											; Size of input vertices
.equ vuSymbStrideOfOutputCluster, 3											; size of output vertices
;----------------------------------------------------------------------------

;------------------ DOUBLE BUFFER -------------------------------------------
.equ vuSymbStaticDataStart, vuSymbGifTagc2									; Starting position of the second buffer
;----------------------------------------------------------------------------

;------------------ BUFFERS SIZE --------------------------------------------
.equ vuSymbTSVertexCount, (((vuSymbStaticDataStart-3)/((2*vuSymbStrideOfInputCluster+3*vuSymbStrideOfOutputCluster))))
.equ vuSymbVIFOffset, (vuSymbStrideOfInputCluster*vuSymbTSVertexCount)		;
.equ vuSymbTLTriCount, (vuSymbTSVertexCount/3)								;
;----------------------------------------------------------------------------

