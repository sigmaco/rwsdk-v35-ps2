; ----------------------------------------------------------------------
; Warning: This file will be processed using a rather simple sed script
; to turn it into a C include file.
; ----------------------------------------------------------------------

; Enums
.ifndef vuEnumLightNaLight
; Warning these must be reconciled with the C enums.
; Check auto-generated file "baequate.i"

.equ vuEnumLightNaLight,        0      ; rpNALIGHTTYPE
.equ vuEnumLightDirectional,    1  ; rpLIGHTDIRECTIONAL
.equ vuEnumLightAmbient,        2      ; rpLIGHTAMBIENT
.equ vuEnumLightPoint,          128      ; rpLIGHTPOINT
.equ vuEnumLightSpot,           129       ; rpLIGHTSPOT
.equ vuEnumLightSpotSoft,       130   ; rpLIGHTSPOTSOFT

.endif

; Data in high memory

.ifndef vuSDmat0

.equ vuSDLightOffset,           0x3d0
.equ vuSDBlockLow, 		        0x3c3
.equ vuSDBlockHigh,             0x3f0
.equ vuSDmat0, 			        vuSDBlockHigh
.equ vuSDmat1, 			        vuSDBlockHigh+1
.equ vuSDmat2, 			        vuSDBlockHigh+2
.equ vuSDmat3, 			        vuSDBlockHigh+3
.equ vuSDnearClip,              vuSDBlockHigh+4
.equ vuSDfarClip, 		        vuSDBlockHigh+5
.equ vuSDxMaxyMax, 		        vuSDBlockHigh+6
.equ vuSDcamWcamHzScale,        vuSDBlockHigh+7
.equ vuSDoffXoffYzShift,        vuSDBlockHigh+8
.equ vuSDrealOffset,            vuSDBlockHigh+9
.equ vuSDgifTag, 		        vuSDBlockHigh+10
.equ vuSDcolScale, 		        vuSDBlockHigh+11
.equ vuSDsurfProps,             vuSDBlockHigh+12
.equ vuSDpingPongAddr,          vuSDBlockHigh+13
.equ vuSDpingPongCount,         vuSDBlockHigh+14
.equ vuSDClipvec1, 	 	        vuSDBlockHigh+13
.equ vuSDClipvec2, 		        vuSDBlockHigh+14
.equ vuSDVUSwitch, 		        vuSDBlockHigh+15
.endif


; ----------------------------------------------------------------------
; Put your Pipe specific static data here
; Names should probably be in the "vuSymb" name space

.equ vuSymbSpriteGifTag, (vuSDLightOffset-1)

; ----------------------------------------------------------------------
; You must set these correctly for the amount of data above and the size
; of the vertex data. vuSymbStrideOfInputCluster will generally be the same
; as the number of clusters that you RpFXBridgeNodeRequestCluster() from
; nodePS2MatBridgeCSL. vuSymbStrideOfOutputCluster will generally be the NREG
; value in the gif tag if the output cluster is dispatched to GS

.equ vuSymbStrideOfInputCluster,  4
.equ vuSymbStrideOfOutputCluster, 5     ; We only store RGBA once

; Set the base of the high memory data here
.equ vuSymbStaticDataStart, (vuSymbSpriteGifTag)

; 2 input vertex + 2 output vertices due to output double buffering plus a bit of padding
.equ vuSymbOneVertexIOStryde,  (2*vuSymbStrideOfInputCluster+2*vuSymbStrideOfOutputCluster)

; number of sprite per batch
.equ vuSymbPointCount, ((vuSymbStaticDataStart-2)/vuSymbOneVertexIOStryde)

.equ vuSymbInputBufferSize,     vuSymbPointCount*vuSymbStrideOfInputCluster*2


; The following is calculated giving the VIF Offset for UNPACK double buffering
.equ vuSymbVIFOffset, (vuSymbPointCount*vuSymbStrideOfInputCluster)

.equ vuSymbOutputBuffer1, (vuSymbInputBufferSize)
.equ vuSymbOutputBuffer2, (vuSymbOutputBuffer1+(vuSymbPointCount*vuSymbStrideOfOutputCluster)+10)

