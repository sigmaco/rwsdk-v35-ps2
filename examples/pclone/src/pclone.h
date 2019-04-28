
/****************************************************************************
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 2001 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/****************************************************************************
 *                                                                         
 * main.h
 *
 * Copyright (C) 2001 Criterion Technologies.
 *
 * Original author: Alexandre Hadjdaj.
 *                                                                         
 * Purpose: Illustrate the use of the Standard Particle Plugin with the cloning 
 *          Pipeline.
 *                         
 ****************************************************************************/

#ifndef PRTSTD_H
#define PRTSTD_H

#include "rwcore.h"
#include "rpprtstd.h"

#define BATCHSIZE (140)

extern RpLight *Ambient;
extern RpLight *Directional;

extern RwCamera *Camera;

typedef struct pCloneData pCloneData;

struct pCloneData
{
    RpPrtStdEmitterClass        *eClass;
    RpPrtStdParticleClass       *pClass;
    
    RpAtomic                    *atmEmitter;
    
    RpPrtStdEmitter             *emitter;
    
    /* Standard properties */
    RpPrtStdEmitterStandard     *EmtStd;

    RpPrtStdEmitterPrtMatrix      *emtPrtMtx;

    RpPrtStdEmitterPTank        *EmtPtank;

	RpAtomic					*atmPTank;

};

extern pCloneData PrtData;

extern RwInt32 SrcBlendID;
extern RwInt32 DestBlendID;

extern RwBool VtxAlphaOn;

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

extern RwBool pCloneInitialize(void);
extern void pCloneTerminate(void);
extern void pCloneRender(void);

extern RwBool pCloneSetDefault(RwBool justcheck);

extern void pCloneCameraRotate(RwReal xAngle, RwReal yAngle);
extern void pCloneCameraTranslate(RwReal atDir);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif /* PRTSTD_H */


