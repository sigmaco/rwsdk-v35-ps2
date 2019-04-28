
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
 * Original author: Alexandre Hadjadj
 * Reviewed by:
 *
 * Purpose: Event handler for clump viewer
 *
 ****************************************************************************/

#include "rwcore.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"

#include "main.h"

#include "clmppick.h"
#include "clmpview.h"

#include "clmpcntl.h"

static RwInt32 NumRotationButtons = 0;
static RwInt32 NumTranslationButtons = 0;

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
        ClumpTranslate = TRUE;
        ClumpTranslateDeltaZ = 1.0f;
        NumTranslationButtons++;        
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON2 )
    {
        /*
         * CROSS...
         */
        ClumpTranslate = TRUE;
        ClumpTranslateDeltaZ = -1.0f;
        NumTranslationButtons++;
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON4 )
    {
        /*
         * CIRCLE...
         */
        if( Clump )
        {
            SelectNextAtomic();
        }

        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADUP )
    {
        /*
         * D-PAD up...
         */
        ClumpRotate = TRUE;
        ClumpRotateDelta.x = 1.0f;
        NumRotationButtons++;
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADDOWN )
    {
        /*
         * D-PAD down...
         */
        ClumpRotate = TRUE;
        ClumpRotateDelta.x = -1.0f;
        NumRotationButtons++;
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADLEFT )
    {
        /*
         * D-PAD left...
         */
        ClumpRotate = TRUE;
        ClumpRotateDelta.y = -1.0f;
        NumRotationButtons++;
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADRIGHT )
    {
        /*
         * D-PAD right...
         */
        ClumpRotate = TRUE;
        ClumpRotateDelta.y = 1.0f;
        NumRotationButtons++;
        return rsEVENTPROCESSED;
    }
    else
    {
        return rsEVENTNOTPROCESSED;
    }

    return rsEVENTNOTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadButtonUp(RsPadButtonStatus *padButtonStatus)
{
    if( padButtonStatus->padButtons & rsPADBUTTON1 ||
        padButtonStatus->padButtons & rsPADBUTTON2 )
    {
        /*
         * TRIANGLE or CROSS...
         */
        NumTranslationButtons--;
        if( NumTranslationButtons == 0 )
        {
            ClumpTranslate = FALSE;
            ClumpTranslateDeltaZ = 0.0f;
        }
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADUP ||
             padButtonStatus->padButtons & rsPADDPADDOWN ||
             padButtonStatus->padButtons & rsPADDPADLEFT ||
             padButtonStatus->padButtons & rsPADDPADRIGHT )
    {
        /*
         * D-PAD left or D-PAD right...
         * D-PAD up or D-PAD down...
         */
        NumRotationButtons--;
        if ( NumRotationButtons <= 0 )
        {
            ClumpRotate = FALSE;
            NumRotationButtons = 0;
            ClumpRotateDelta.x = ClumpRotateDelta.y = 0.0f;
        }

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
    if( Clump )
    {
        RwV2d delta = *(RwV2d *)param;

        delta.x = (-delta.x * RwRealAbs(delta.x)) * 2.0f;
        delta.y = (-delta.y * RwRealAbs(delta.y)) * 2.0f;

        ClumpControlDirectRotate(delta.y, delta.x);

        return rsEVENTPROCESSED;
    }

    return rsEVENTNOTPROCESSED;
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

    return rsEVENTNOTPROCESSED;
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
