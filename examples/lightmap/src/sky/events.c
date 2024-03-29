
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
 * Purpose: Example to demonstrate the functions of light maps as
   as they are provided in RpLightmap and RtLightmap.
 *
*****************************************************************************/

#include "rwcore.h"
#include "rpworld.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"

#include "main.h"

#define  CAMERASPEED    (0.4f) /* For buttons */
#define  CAMERAROTSPEED (350.0f*MAXCAMERAROTSPEED)

/*
 *****************************************************************************
 */
static              RsEventStatus
HandlePadButtonDown(RsPadButtonStatus * padButtonStatus)
{
    if (padButtonStatus->padButtons & rsPADBUTTON1)
    {
        /*
         * TRIANGLE...
         */
        CameraSpeed = CameraMaxSpeed*CAMERASPEED;

        return rsEVENTPROCESSED;
    }
    else if (padButtonStatus->padButtons & rsPADBUTTON2)
    {
        /*
         * CROSS...
         */
        CameraSpeed = -CameraMaxSpeed*CAMERASPEED;

        return rsEVENTPROCESSED;
    }
    else if (padButtonStatus->padButtons & rsPADBUTTON3)
    {
        /*
         * SQUARE...
         */
        CameraStrafeSpeed = CameraMaxSpeed*CAMERASPEED;

        return rsEVENTPROCESSED;
    }
    else if (padButtonStatus->padButtons & rsPADBUTTON4)
    {
        /*
         * CIRCLE...
         */
        CameraStrafeSpeed = -CameraMaxSpeed*CAMERASPEED;

        return rsEVENTPROCESSED;
    }
    else if (padButtonStatus->padButtons & rsPADBUTTON5)
    {
        /*
         * Left shoulder '1'...
         */
        return rsEVENTPROCESSED;
    }
    else if (padButtonStatus->padButtons & rsPADBUTTON6)
    {
        /*
         * Left shoulder '2'...
         */
        return rsEVENTPROCESSED;
    }
    else if (padButtonStatus->padButtons & rsPADBUTTON7)
    {
        /*
         * Right shoulder '1'...
         */
        return rsEVENTPROCESSED;
    }
    else if (padButtonStatus->padButtons & rsPADBUTTON8)
    {
        /*
         * Right shoulder '2'...
         */
        return rsEVENTPROCESSED;
    }
    else if (padButtonStatus->padButtons & rsPADDPADUP)
    {
        /*
         * D-PAD up...
         */
        return rsEVENTPROCESSED;
    }
    else if (padButtonStatus->padButtons & rsPADDPADDOWN)
    {
        /*
         * D-PAD down...
         */
        return rsEVENTPROCESSED;
    }
    else if (padButtonStatus->padButtons & rsPADDPADLEFT)
    {
        /*
         * D-PAD left...
         */
        return rsEVENTPROCESSED;
    }
    else if (padButtonStatus->padButtons & rsPADDPADRIGHT)
    {
        /*
         * D-PAD right...
         */
        return rsEVENTPROCESSED;
    }
    else
    {
        return rsEVENTNOTPROCESSED;
    }
}

/*
 *****************************************************************************
 */
static              RsEventStatus
HandlePadButtonUp(RsPadButtonStatus * __RWUNUSED__ padButtonStatus)
{

    if (padButtonStatus->padButtons & rsPADBUTTON1)
    {
        /*
         * TRIANGLE...
         */
        CameraSpeed = 0.0f;

        return rsEVENTPROCESSED;
    }
    else if (padButtonStatus->padButtons & rsPADBUTTON2)
    {
        /*
         * CROSS...
         */
        CameraSpeed = 0.0f;

        return rsEVENTPROCESSED;
    }
    else if (padButtonStatus->padButtons & rsPADBUTTON3)
    {
        /*
         * SQUARE...
         */
        CameraStrafeSpeed = 0.0f;

        return rsEVENTPROCESSED;
    }
    else if (padButtonStatus->padButtons & rsPADBUTTON4)
    {
        /*
         * CIRCLE...
         */
        CameraStrafeSpeed = 0.0f;

        return rsEVENTPROCESSED;
    }
 
    return rsEVENTNOTPROCESSED;
}

/*
 *****************************************************************************
 */
static              RsEventStatus
HandlePadAnalogueRight(void *param)
{
    RwV2d               delta = *(RwV2d *) param;

    CameraSpeed = CameraMaxSpeed*delta.y*RwRealAbs(delta.y);
    CameraStrafeSpeed = CameraMaxSpeed*delta.x*RwRealAbs(delta.x);

    return rsEVENTPROCESSED;
}

/*
 *****************************************************************************
 */
static              RsEventStatus
HandlePadAnalogueRightReset(void *param __RWUNUSED__)
{
    CameraSpeed = 0.0f;
    CameraStrafeSpeed = 0.0f;

    return rsEVENTPROCESSED;
}

/*
 *****************************************************************************
 */
static              RsEventStatus
HandlePadAnalogueLeft(void *param)
{
    RwV2d               delta = *(RwV2d *) param;

    CameraPitchRate = CAMERAROTSPEED * delta.y * RwRealAbs(delta.y);
    CameraTurnRate = CAMERAROTSPEED * delta.x * RwRealAbs(delta.x);

    return rsEVENTPROCESSED;
}

/*
 *****************************************************************************
 */
static              RsEventStatus
HandlePadAnalogueLeftReset(void *param __RWUNUSED__)
{
    CameraPitchRate = 0.0f;
    CameraTurnRate = 0.0f;

    return rsEVENTPROCESSED;
}

/*
 *****************************************************************************
 */
static              RsEventStatus
PadHandler(RsEvent event, void *param)
{
    if (MenuPadHandler(event, param) == rsEVENTPROCESSED)
    {
        return rsEVENTPROCESSED;
    }

    switch (event)
    {
        case rsPADBUTTONDOWN:
            {
                return HandlePadButtonDown((RsPadButtonStatus *) param);
            }

        case rsPADBUTTONUP:
            {
                return HandlePadButtonUp((RsPadButtonStatus *) param);
            }

        case rsPADANALOGUELEFT:
            {
                return HandlePadAnalogueLeft(param);
            }

        case rsPADANALOGUERIGHT:
            {
                return HandlePadAnalogueRight(param);
            }

        case rsPADANALOGUELEFTRESET:
            {
                return HandlePadAnalogueLeftReset(param);
            }

        case rsPADANALOGUERIGHTRESET:
            {
                return HandlePadAnalogueRightReset(param);
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
