// ----------------------------------------------------------------------
// Warning: This file will be processed using a rather simple sed script
// to turn it into a C include file.
// ----------------------------------------------------------------------

// Enums
#ifndef vuEnumLightNaLight
// Warning these must be reconciled with the C enums.
// Check auto-generated file "baequate.i"

#define vuEnumLightNaLight 0      // rpNALIGHTTYPE
#define vuEnumLightDirectional 1  // rpLIGHTDIRECTIONAL
#define vuEnumLightAmbient 2      // rpLIGHTAMBIENT
#define vuEnumLightPoint 128      // rpLIGHTPOINT
#define vuEnumLightSpot 129       // rpLIGHTSPOT
#define vuEnumLightSpotSoft 130   // rpLIGHTSPOTSOFT

#endif

// Data in high memory

#ifndef vuSDmat0

#define vuSDLightOffset     0x3d0
#define vuSDBlockLow        0x3c3
#define vuSDBlockHigh       0x3f0
#define vuSDmat0            vuSDBlockHigh
#define vuSDmat1            vuSDBlockHigh+1
#define vuSDmat2            vuSDBlockHigh+2
#define vuSDmat3            vuSDBlockHigh+3
#define vuSDnearClip        vuSDBlockHigh+4
#define vuSDfarClip         vuSDBlockHigh+5
#define vuSDxMaxyMax        vuSDBlockHigh+6
#define vuSDcamWcamHzScale  vuSDBlockHigh+7
#define vuSDoffXoffYzShift  vuSDBlockHigh+8
#define vuSDrealOffset      vuSDBlockHigh+9
#define vuSDgifTag          vuSDBlockHigh+10
#define vuSDcolScale        vuSDBlockHigh+11
#define vuSDsurfProps       vuSDBlockHigh+12
#define vuSDpingPongAddr    vuSDBlockHigh+13
#define vuSDpingPongCount   vuSDBlockHigh+14
#define vuSDClipvec1        vuSDBlockHigh+13
#define vuSDClipvec2        vuSDBlockHigh+14
#define vuSDVUSwitch        vuSDBlockHigh+15

#endif

/*----------------- Custom FX DATA ---------------------*/
#define pipeASymbTransv   (vuSDLightOffset-1)
#define pipeASymbMtxR0    (pipeASymbTransv-1)
#define pipeASymbGifTagc2 (pipeASymbMtxR0-1)
/*----------------------------------------------------------------------*/

/*----------------- BUFFERS STRIDE -------------------------------------*/
#define pipeASymbStrideOfInputCluster  (4)
#define pipeASymbStrideOfOutputCluster (3)
/*----------------------------------------------------------------------*/

/*----------------- DOUBLE BUFFER --------------------------------------*/
#define pipeASymbStaticDataStart pipeASymbGifTagc2
/*----------------------------------------------------------------------*/

/*----------------- BUFFERS SIZE ---------------------------------------*/
#define pipeASymbTSVertexCount (((pipeASymbStaticDataStart-3)/(2*pipeASymbStrideOfInputCluster+3*pipeASymbStrideOfOutputCluster)))
#define pipeASymbVIFOffset     (pipeASymbStrideOfInputCluster*pipeASymbTSVertexCount)
#define pipeASymbTLTriCount    (pipeASymbTSVertexCount/3)
/*----------------------------------------------------------------------*/


