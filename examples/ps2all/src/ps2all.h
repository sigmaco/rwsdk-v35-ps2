
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


#ifdef __cplusplus
extern "C"
{
#endif  /* __cplusplus */

extern RwBool PS2AllOpen(void);
extern void   PS2AllClose(void);
extern void   PS2AllMenuOpen(void);

extern void   PS2AllUpdate(RwReal deltaTime);
extern void   PS2AllRender(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* PS2ALL_H */
