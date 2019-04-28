
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
 * events.c
 *
 * Copyright (C) 2000 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: To illustrate the use of the RpCollision plugin to collide 
 *          an atomic with the static geometry in a world.
 ****************************************************************************/

#include "rwcore.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"

#include "collis2.h"


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadButtonDown(RsPadButtonStatus *padButtonStatus __RWUNUSED__)
{
    return rsEVENTNOTPROCESSED;
}


static RsEventStatus
HandlePadButtonUp(RsPadButtonStatus *padButtonStatus __RWUNUSED__)
{
    return rsEVENTNOTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadAnalogueLeft(void *param)
{
    RwV2d delta = *(RwV2d *)param;

    delta.x = (-delta.x * RwRealAbs(delta.x)) * 2.0f;
    delta.y = (-delta.y * RwRealAbs(delta.y)) * 2.0f;

    CameraPoint(delta.x, delta.y);

    return rsEVENTPROCESSED;
}


static RsEventStatus
HandlePadAnalogueRight(void *param)
{
    RwV2d delta = *(RwV2d *)param;

    delta.y = (-delta.y * RwRealAbs(delta.y)) * 5.0f;

    TranslateCameraZ(-delta.y * 0.5f);

    return rsEVENTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus 
PadHandler(RsEvent event, void *param)
{
    if( MenuPadHandler(event, param) == rsEVENTPROCESSED )
    {
        return rsEVENTPROCESSED;
    }

    switch( event )
    {
        case rsPADBUTTONDOWN:
        {
            return HandlePadButtonDown((RsPadButtonStatus *)param);
        }

        case rsPADBUTTONUP:
        {
            return HandlePadButtonUp((RsPadButtonStatus *)param);
        }

        case rsPADANALOGUELEFT:
        {
            return HandlePadAnalogueLeft(param);
        }

        case rsPADANALOGUERIGHT:
        {
            return HandlePadAnalogueRight(param);
        }

        default:
        {
            return rsEVENTNOTPROCESSED;
        }
    }
}

/*
 *****************************************************************************
 */
RwBool
AttachInputDevices(void)
{
    RsInputDeviceAttach(rsPAD, PadHandler);

    return TRUE;
}

/*
 *****************************************************************************
 */

