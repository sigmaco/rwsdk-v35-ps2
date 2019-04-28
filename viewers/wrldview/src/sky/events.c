
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
 * events.c (sky)
 *
 * Copyright (C) 2001 Criterion Technologies.
 *
 * Original author: Alexandre Hadjadj & Matt Reynolds.
 * Reviewed by: John Irwin (with substantial edits).
 *
 * Purpose: RenderWare3 BSP viewer.
 *
 ****************************************************************************/

#include "rwcore.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"

#include "movement.h"
#include "main.h"
#include "world.h"
#include "render.h"

RwBool CameraPointing = TRUE;
RwBool CameraTranslating = FALSE;
RwReal CameraTranslateDelta;


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadButtonDown(RsPadButtonStatus *padButtonStatus)
{
    if( padButtonStatus->padButtons & rsPADBUTTON6 )
    {
        /*
         * Left shoulder '2'...
         */
        if( SingleSectorOn )
        {
            SelectPreviousWorldSector();
        }

        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON8 )
    {
        /*
         * Right shoulder '2'...
         */
        if( SingleSectorOn )
        {
            SelectNextWorldSector();
        }

        return rsEVENTPROCESSED;
    }
    else
    {
        return rsEVENTNOTPROCESSED;
    }
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

    delta.x = delta.x * RwRealAbs(delta.x) * 0.75f;
    delta.y = delta.y * RwRealAbs(delta.y) * 0.75f;

    CameraPoint(delta.y, delta.x);

    return rsEVENTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadAnalogueRight(void *param)
{
    RwV2d delta = *(RwV2d *)param;

    delta.y = -delta.y * RwRealAbs(delta.y) * TranslateScaleFactor * 10.0f;

    TranslateCameraZ(-delta.y);

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
