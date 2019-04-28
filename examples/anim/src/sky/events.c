
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
 * Copyright (c) 2002 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/****************************************************************************
 *
 * events.c
 *
 * Copyright (C) 2002 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 * Purpose: This Example demonstrates the use of the rtanim plugin to load
 * and render models, and shows how to apply "inks" and "paints".
 *
 ****************************************************************************/

#include "rwcore.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"
#include "main.h"

/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadButtonDown(RsPadButtonStatus *padButtonStatus)
{
    if( padButtonStatus->padButtons & rsPADBUTTON1 )
    {
        /*
         * TRIANGLE...
         */
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON2 )
    {
        /*
         * CROSS...
         */
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON3 )
    {
        /*
         * SQUARE...
         */
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON4 )
    {
        /*
         * CIRCLE...
         */
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON5 )
    {
        /*
         * Left shoulder '1'...
         */
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON6 )
    {
        /*
         * Left shoulder '2'...
         */
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON7 )
    {
        /*
         * Right shoulder '1'...
         */
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON8 )
    {
        /*
         * Right shoulder '2'...
         */
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADUP )
    {
        /*
         * D-PAD up...
         */
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADDOWN )
    {
        /*
         * D-PAD down...
         */
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADLEFT )
    {
        /*
         * D-PAD left...
         */
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADRIGHT )
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

    delta.x = (-delta.x * RwRealAbs(delta.x)) * 1.0f;
    delta.y = (delta.y * RwRealAbs(delta.y)) * 1.0f;

    CameraLook(2.0f * delta.x, 2.0f * delta.y);

    return rsEVENTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadAnalogueRight(void *param)
{
    RwV2d delta = *(RwV2d *)param;

    delta.y = 1.0f * (delta.y * RwRealAbs(delta.y));

    CameraTranslateZ(10.0f * delta.y);
    
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
