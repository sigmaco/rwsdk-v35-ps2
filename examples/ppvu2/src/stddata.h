// ----------------------------------------------------------------------
// Warning: This file will be processed using a rather simple sed script
// to turn it into a C include file.
// ----------------------------------------------------------------------

// Enums
#ifndef vuEnumLightNaLight
// Warning these must be reconciled with the C enums.
// Check auto-generated file "baequate.i"

#define vuEnumLightNaLight        0      // rpNALIGHTTYPE
#define vuEnumLightDirectional    1  // rpLIGHTDIRECTIONAL
#define vuEnumLightAmbient        2      // rpLIGHTAMBIENT
#define vuEnumLightPoint          128      // rpLIGHTPOINT
#define vuEnumLightSpot           129       // rpLIGHTSPOT
#define vuEnumLightSpotSoft       130   // rpLIGHTSPOTSOFT

#endif

// Data in high memory

#ifndef vuSDmat0

#define vuSDLightOffset           0x3d0
#define vuSDBlockLow                0x3c3
#define vuSDBlockHigh             0x3f0
#define vuSDmat0                    vuSDBlockHigh
#define vuSDmat1                    vuSDBlockHigh+1
#define vuSDmat2                    vuSDBlockHigh+2
#define vuSDmat3                    vuSDBlockHigh+3
#define vuSDnearClip              vuSDBlockHigh+4
#define vuSDfarClip                 vuSDBlockHigh+5
#define vuSDxMaxyMax                vuSDBlockHigh+6
#define vuSDcamWcamHzScale        vuSDBlockHigh+7
#define vuSDoffXoffYzShift        vuSDBlockHigh+8
#define vuSDrealOffset            vuSDBlockHigh+9
#define vuSDgifTag              vuSDBlockHigh+10
#define vuSDcolScale                vuSDBlockHigh+11
#define vuSDsurfProps             vuSDBlockHigh+12
#define vuSDpingPongAddr          vuSDBlockHigh+13
#define vuSDpingPongCount         vuSDBlockHigh+14
#define vuSDClipvec1                vuSDBlockHigh+13
#define vuSDClipvec2                vuSDBlockHigh+14
#define vuSDVUSwitch                vuSDBlockHigh+15
#endif


// ----------------------------------------------------------------------
// Put your Pipe specific static data here
// Names should probably be in the "pipeASymb" name space

#define pipeASymbSpriteGifTag (vuSDLightOffset-1)

// ----------------------------------------------------------------------
// You must set these correctly for the amount of data above and the size
// of the vertex data. pipeASymbStrideOfInputCluster will generally be the same
// as the number of clusters that you RpFXBridgeNodeRequestCluster() from
// nodePS2MatBridgeCSL. pipeASymbStrideOfOutputCluster will generally be the NREG
// value in the gif tag if the output cluster is dispatched to GS

#define pipeASymbStrideOfInputCluster  4
#define pipeASymbStrideOfOutputCluster 5     // We only store RGBA once

// Set the base of the high memory data here
#define pipeASymbStaticDataStart (pipeASymbSpriteGifTag)

// 2 input vertex + 2 output vertices due to output double buffering plus a bit of padding
#define pipeASymbOneVertexIOStryde  (2*pipeASymbStrideOfInputCluster+2*pipeASymbStrideOfOutputCluster)

// number of sprite per batch
#define pipeASymbPointCount ((pipeASymbStaticDataStart-2)/pipeASymbOneVertexIOStryde)

#define pipeASymbInputBufferSize     pipeASymbPointCount*pipeASymbStrideOfInputCluster*2


// The following is calculated giving the VIF Offset for UNPACK double buffering
#define pipeASymbVIFOffset (pipeASymbPointCount*pipeASymbStrideOfInputCluster)

#define pipeASymbOutputBuffer1 (pipeASymbInputBufferSize)
#define pipeASymbOutputBuffer2 (pipeASymbOutputBuffer1+(pipeASymbPointCount*pipeASymbStrideOfOutputCluster)+10)

