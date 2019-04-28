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
 * ppsprite.h
 *
 * Copyright (C) 2001 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: Illustrate the possibility of creating a custom VU pipe
 *
 ****************************************************************************/

#ifndef PPSPRITE_H
#define PPSPRITE_H

#include "rwcore.h"


typedef struct PPSprite
{
    RwV3d   pos;
    RwV2d   size;
    RwV2d   uv1;
    RwV2d   uv2;
    RwRGBA  color;
} PPSprite;

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

extern RwBool
PPSpriteCreatePipe(void);

extern void
PPSpriteDestroyPipe(void);

extern RwBool
PPSPriteDraw(PPSprite *sprites, RwInt32 numSprites );

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif /* PPSSPRITE_H */


