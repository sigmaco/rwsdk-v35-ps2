
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
 * cfx.c
 *
 * Copyright (C) 2001 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: To illustrate the possibility of creating a custom VU pipe
 *
 ****************************************************************************/


#include "rwcore.h"
#include "rpworld.h"
#include "cfx.h"

#include "skycfx.h"

static RxPipeline *CustomFXPipeline = NULL;


RwReal CFXTrans[2] = { 0.0f , 0.0f };
RwReal CFXScale[2] = { 1.0f , 1.0f };

RwReal CFXRotMat0[2] = { 1.0f , 0.0f};
RwReal CFXRotMat1[2] = { 0.0f , 1.0f};

RwTexture  *CustomTexture;


/*
 *****************************************************************************
 */
void
CustomFXSetTranslation(RwV2d *trans)
{
    CFXTrans[0] = trans->x;
    CFXTrans[1] = trans->y;

    return;
}


/*
 *****************************************************************************
 */
void
CustomFXSetScale(RwV2d *scale)
{
    CFXScale[0] = scale->x;
    CFXScale[1] = scale->y;

    return;
}


/*
 *****************************************************************************
 */
void
CustomFXSetAngle(RwReal *angle)
{
    CFXRotMat0[0] = RwCos(*angle); CFXRotMat0[1] = -RwSin(*angle);
    CFXRotMat1[0] = RwSin(*angle); CFXRotMat1[1] = RwCos(*angle);

    return;
}


/*
 *****************************************************************************
 */
void
CustomFXSetTexture(RwTexture *customTexture)
{
    CustomTexture = customTexture;

    return;
}


/*
 *****************************************************************************
 */
RpAtomic *
CustomFXAtomicSetup(RpAtomic *atomic)
{
    if( atomic && CustomFXPipeline != NULL )
    {
        RpAtomicSetPipeline(atomic, CustomFXPipeline);
    }

    return atomic;
}


/*
 *****************************************************************************
 */
RwBool
CustomFXCreate(void)
{

    CustomFXPipeline = SkyCustomFXCreatePipe();

    if( CustomFXPipeline == NULL)
    {
        return FALSE;
    }

    return TRUE;
}


/*
 *****************************************************************************
 */
void CustomFXDestroy(void)
{
    if( CustomFXPipeline != NULL )
    {
        RxPipelineDestroy(CustomFXPipeline);
        CustomFXPipeline = NULL;
    }

    return;
}
