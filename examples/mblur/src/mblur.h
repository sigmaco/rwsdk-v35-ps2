
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
 * mblur.h
 *
 * Copyright (C) 2001 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: To illustrate a motion-blur technique on PS2.
 *
 ****************************************************************************/

#ifndef MBLUR_H
#define MBLUR_H

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

extern void MotionBlurRender(RwCamera *cam, RwUInt32 alpha);

extern RwBool MotionBlurOpen(RwCamera *cam);

extern RwBool MotionBlurClose(void);

#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif  /* MBLUR_H */
