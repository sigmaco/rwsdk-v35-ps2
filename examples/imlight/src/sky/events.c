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
 * Copyright (C) 2001 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: Illustrate the lighting of 3D immediate vertices.
 *
*****************************************************************************/

#include "rwcore.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"

#include "imlight.h"

static RwBool TransformLight = FALSE;


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
        TransformLight = TRUE;

        return rsEVENTPROCESSED;
    }

    return rsEVENTNOTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadButtonUp(RsPadButtonStatus *padButtonStatus)
{
    if( padButtonStatus->padButtons & rsPADBUTTON6 )
    {
        /*
         * Left shoulder '2'...
         */
        TransformLight = FALSE;

        return rsEVENTPROCESSED;
    }

    return rsEVENTNOTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadAnalogueLeft(void *param)
{
    RwV2d delta = *(RwV2d *)param;

    delta.x = -delta.x * RwRealAbs(delta.x);
    delta.y = -delta.y * RwRealAbs(delta.y);

    if( TransformLight )
    {	
        switch( LightMode )
        {
            case LIGHTDIRECTIONAL:
            {
                delta.x *= -2.5f;
                delta.y *= -2.5f;

                Im3DLightRotate(delta.x, delta.y);
                
                break;
            }

            case LIGHTPOINT:
            {
                delta.x *= -0.1f;
                delta.y *= -0.2f;

                Im3DLightTranslateXY(delta.x, delta.y);
                
                break;
            }
        }
    }
    else
    {
        delta.x *= 10.0f;
        delta.y *= 10.0f;

        Im3DMeshRotate(delta.x, -delta.y);
    }


    return rsEVENTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadAnalogueRight(void *param)
{
    RwV2d delta = *(RwV2d *)param;

    if( TransformLight && LightMode == LIGHTPOINT )
    {
        delta.y = delta.y * RwRealAbs(delta.y) * 0.5f;

        Im3DLightTranslateZ(delta.y);
    }
    else
    {
        delta.y = delta.y * RwRealAbs(delta.y) * 0.5f;

        Im3DMeshTranslateZ(delta.y);
    }

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

