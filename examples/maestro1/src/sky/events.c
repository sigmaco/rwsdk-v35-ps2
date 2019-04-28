
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
 *
 * Purpose: 2d Viewer UI.
 *
 ****************************************************************************/

#include "rwcore.h"

#include "rt2d.h"
#include "rt2danim.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"
#include "mouse.h"

#include "view.h"

#include "button.h"

#define DELTAX (1.0f)
#define DELTAY (0.5f)

RwV2d CursorPos = {100.0f, 100.0f};

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

    if( padButtonStatus->padButtons & rsPADBUTTON2 )
    {
        /*
            * CROSS...
            */
        if(MENUMODE != MenuGetStatus())
            PressButton(Maestro, SelectButton,rt2dANIMBUTTONSTATEOVERUPTOOVERDOWN);

        return rsEVENTPROCESSED;
    }

    if( padButtonStatus->padButtons & rsPADBUTTON3 )
    {
        /*
            * SQUARE...
            */

        return rsEVENTPROCESSED;
    }

    if( padButtonStatus->padButtons & rsPADBUTTON4 )
    {
        /*
            * CIRCLE...
            */

        if(MENUMODE != MenuGetStatus())
            PressButton(Maestro, CancelButton,rt2dANIMBUTTONSTATEOVERUPTOOVERDOWN);
    }


	if( padButtonStatus->padButtons & rsPADBUTTON5 )
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
        if(MENUMODE != MenuGetStatus())
            PressButton(Maestro, UpButton,rt2dANIMBUTTONSTATEOVERUPTOOVERDOWN);
        
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADDOWN )
    {
        /*
         * D-PAD down...
         */
        if(MENUMODE != MenuGetStatus())
            PressButton(Maestro, DownButton,rt2dANIMBUTTONSTATEOVERUPTOOVERDOWN);
        
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADLEFT )
    {
        /*
         * D-PAD left...
         */
        if(MENUMODE != MenuGetStatus())
            PressButton(Maestro, LeftButton, rt2dANIMBUTTONSTATEOVERUPTOOVERDOWN);
        
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADRIGHT )
    {
        /*
         * D-PAD right...
         */
        if(MENUMODE != MenuGetStatus())
            PressButton(Maestro, RightButton, rt2dANIMBUTTONSTATEOVERUPTOOVERDOWN);
        
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
HandlePadButtonUp(RsPadButtonStatus *padButtonStatus)
{
    if( padButtonStatus->padButtons & rsPADBUTTON1 )
    {
        /*
         * TRIANGLE...
         */

        return rsEVENTPROCESSED;
    }

    if( padButtonStatus->padButtons & rsPADBUTTON2 )
    {
        /*
         * CROSS...
         */

        if(MENUMODE != MenuGetStatus())
            PressButton(Maestro, SelectButton,rt2dANIMBUTTONSTATEOVERDOWNTOOVERUP);

        return rsEVENTPROCESSED;
    }

    if( padButtonStatus->padButtons & rsPADBUTTON3 )
    {
        /*
         * SQUARE...
         */

        return rsEVENTPROCESSED;
    }

    if( padButtonStatus->padButtons & rsPADBUTTON4 )
    {
        /*
         * CIRCLE...
         */

        if(MENUMODE != MenuGetStatus())
            PressButton(Maestro, CancelButton, rt2dANIMBUTTONSTATEOVERDOWNTOOVERUP);

        return rsEVENTPROCESSED;
    }

    if( padButtonStatus->padButtons & rsPADBUTTON5 )
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
    else if( padButtonStatus->padButtons & rsPADDPADUP )
    {
        /*
         * D-PAD up...
         */
        if(MENUMODE != MenuGetStatus())
            PressButton(Maestro, UpButton,rt2dANIMBUTTONSTATEOVERDOWNTOOVERUP);
        
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADDOWN )
    {
        /*
         * D-PAD down...
         */
        if(MENUMODE != MenuGetStatus())
            PressButton(Maestro, DownButton,rt2dANIMBUTTONSTATEOVERDOWNTOOVERUP);
        
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADLEFT )
    {
        /*
         * D-PAD left...
         */
        if(MENUMODE != MenuGetStatus())
            PressButton(Maestro, LeftButton, rt2dANIMBUTTONSTATEOVERDOWNTOOVERUP);
        
        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADDPADRIGHT )
    {
        /*
         * D-PAD right...
         */
        if(MENUMODE != MenuGetStatus())
            PressButton(Maestro, RightButton, rt2dANIMBUTTONSTATEOVERDOWNTOOVERUP);
        
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
HandlePadAnalogueLeft(void *param __RWUNUSED__)
{
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
PadHandler(RsEvent event, void *param __RWUNUSED__)
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
