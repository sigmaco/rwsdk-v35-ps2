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
 * Copyright (c) 2000 Criterion Software Ltd.
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
 * Purpose: To illustrate how sub-rasters can be used to render multiple
 *          views.
 *
 ****************************************************************************/

#include "rwcore.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"

#include "subrast.h"


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadButtonDown(RsPadButtonStatus *padButtonStatus __RWUNUSED__)
{
    return rsEVENTNOTPROCESSED;
}


/*
 *****************************************************************************
 */
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

    delta.x = (-delta.x * RwRealAbs(delta.x)) * 5.0f;
    delta.y = (-delta.y * RwRealAbs(delta.y)) * 5.0f;

    RotateClump(delta.x, -delta.y);

    return rsEVENTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadAnalogueRight(void *param __RWUNUSED__)
{
    return rsEVENTNOTPROCESSED;
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

