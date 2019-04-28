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
 * skycfx.h
 *
 * Copyright (C) 2001 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: To illustrate the possibility of creating a custom VU pipe
 *
 ****************************************************************************/

#ifndef SKYCFX_H
#define SKYCFX_H

#ifdef    __cplusplus
extern              "C"
{
#endif                          /* __cplusplus */

extern long CustomFXNT     __attribute__ ((section(".vudata")));
extern long CustomFXPRS    __attribute__ ((section(".vudata")));
extern long CustomFXPRL    __attribute__ ((section(".vudata")));

extern RxPipeline *
SkyCustomFXCreatePipe(void);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* SKYCFX_H */
