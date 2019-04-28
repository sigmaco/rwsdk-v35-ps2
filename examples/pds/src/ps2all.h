
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
 * ps2all.h
 *
 * Copyright (C) 2001 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: An introduction to PS2All and demonstration of how it
 *          can be used to maximize CPU-side rendering performance
 *
 ****************************************************************************/

#ifndef PS2ALL_H
#define PS2ALL_H

#include "rwcore.h"
#include "rpworld.h"

extern RpLight *Ambiant;
extern RpLight *Direct[4];
extern RpLight *Point;
extern RwFrame *PointFrame;
extern RwFrame *DirectFrame[4];

/* Used to optimise pipeline execution */
extern RwBool PS2AllFirstAtomic;
extern RwBool PS2AllOptimiseMatrices;
extern RwInt32 PS2AllNumDrawnAtomics;

#define PSALLVENDORID (0x10)
#define User1AtmID rpPDS_MAKEPIPEID(PSALLVENDORID, 0x00)
#define User2AtmID rpPDS_MAKEPIPEID(PSALLVENDORID, 0x01)
#define User1MatID rpPDS_MAKEPIPEID(PSALLVENDORID, 0x02)
#define User2MatID rpPDS_MAKEPIPEID(PSALLVENDORID, 0x03)

extern RpAtomic **PS2AllAtomics;
extern RwTexture *ps2AllTexture;

/* Grab these from main.c */
extern RpWorld     *World;
extern RwCamera    *Camera;
extern RpLight     *DirectionalLight;

extern RwBool PS2AllMenuCBAccess;
extern RwBool PSAllAutoMaximize;

#ifdef __cplusplus
extern "C"
{
#endif  /* __cplusplus */

extern RwBool PS2AllOpen(void);
extern void   PS2AllClose(void);
extern void   PS2AllMenuOpen(void);

extern void   PS2AllUpdate(RwReal deltaTime);
extern void   PS2AllRender(void);
extern void   PS2AllOnScreenInfo(RwInt32 fps);

extern RwBool PS2AllOptimizedPipelinesCreate();

extern RwBool PS2AllNumAtomicsCB(RwBool justCheck);
extern void   PS2AllPositionAtomics(RwInt32 maxNum);

extern RpGeometry *PS2AllGeometryCreate( RwUInt32 length,
                                         RwUInt32 width,
                                         RwUInt32 numMats );

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* PS2ALL_H */
