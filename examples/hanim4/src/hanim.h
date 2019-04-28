
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
 * hanim.c
 *
 * Copyright (C) 2001 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: To illustrate how two H-anim sequences can be blended together.
 * *****************************************************************************/

#ifndef HANIM1_H
#define HANIM1_H

#include "rwcore.h"
#include "rpworld.h"
#include "rtanim.h"

extern RtAnimAnimation *UncompressedAnim;
extern RtAnimAnimation *CompressedAnim;
extern RtAnimAnimation *CurrentAnim;
extern RwInt32          CompressedAnimSize;
extern RwInt32          UncompressedAnimSize;



#ifdef __cplusplus
extern "C"
{
#endif  /* __cplusplus */

extern RpClump *CreateClump(RpWorld *world);
extern void DestroyClump(RpClump *clump, RpWorld *world);

extern void UpdateAnimation(RpClump *clump, RwReal deltaTime);

extern RpHAnimHierarchy *GetHierarchy(RpClump *clump);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* HANIM1_H */
