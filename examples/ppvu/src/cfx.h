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
 * cfx.h
 *
 * Copyright (C) 2001 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *                                                                         
 * Purpose: To illustrate the possibility of creating a custom VU pipe 
 *                         
 ****************************************************************************/
 
#ifndef CFX_H
#define CFX_H

extern RwReal CFXTrans[2];
extern RwReal CFXScale[2];

extern RwReal CFXRotMat0[2];
extern RwReal CFXRotMat1[2];

extern RwTexture  *CustomTexture;

#ifdef    __cplusplus
extern              "C"
{
#endif                          /* __cplusplus */

extern void
CustomFXSetTranslation(RwV2d *trans);

extern void
CustomFXSetScale(RwV2d *scale);

extern void
CustomFXSetAngle(RwReal *angle);

extern void
CustomFXSetTexture(RwTexture *customTexture);

extern RpAtomic *
CustomFXAtomicSetup(RpAtomic *atomic);

extern RwBool
CustomFXCreate(void);

extern void
CustomFXDestroy(void);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* CFX_H */
