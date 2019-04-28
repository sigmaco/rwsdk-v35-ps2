
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


static RwBool CursorUp = FALSE;
static RwBool CursorDown = FALSE;
static RwBool CursorLeft = FALSE;
static RwBool CursorRight = FALSE;


/*
 *****************************************************************************
 */
void
InitializeMouseCursor(void)
{
    rsMouseSetPos(&CursorPos);
    return;
}


/*
 *****************************************************************************
 */
void
UpdateMouseCursor(void)
{
	RwV2d delta;

    if( CursorUp )
    {
        delta.x = 0.0f;
        delta.y = -DELTAY;

        rsMouseAddDelta(&delta);
        rsMouseGetPos(&CursorPos);
    }

    if( CursorDown )
    {
         delta.x = 0.0f;
         delta.y = DELTAY;

         rsMouseAddDelta(&delta);
         rsMouseGetPos(&CursorPos);
    }

    if( CursorLeft )
    {
        delta.x = -DELTAX;
        delta.y = 0.0f;

        rsMouseAddDelta(&delta);
        rsMouseGetPos(&CursorPos);
    }

    if( CursorRight )
    {
        delta.x = DELTAX;
        delta.y = 0.0f;

        rsMouseAddDelta(&delta);
        rsMouseGetPos(&CursorPos);
    }


	if( CursorUp || CursorDown || CursorLeft || CursorRight )
	{
	    Rt2dMessage         message;
    	message.messageType = rt2dMESSAGETYPEMOUSEMOVETO;
    	message.index = 0;

    	message.intParam1 = (RwInt32)CursorPos.x;
    	message.intParam2 = (RwInt32)CursorPos.y;
    	/* Post the message and process. */
    	if (Maestro)
    	{
        	Rt2dMaestroPostMessages(Maestro, &message, 1);
    	}
	}

    return;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadButtonDown(RsPadButtonStatus *padButtonStatus)
{
    RwBool buttonDownHandled = FALSE;
    
    Position.x = WinWidth * 0.5f;
    Position.y = WinHeight * 0.5f;

    if( padButtonStatus->padButtons & rsPADBUTTON1 )
    {
        /*
            * TRIANGLE...
            */
        CursorUp = TRUE;

        if(MENUMODE != MenuGetStatus())
            PressButtonDown(Maestro,GetButtonID(CancelButtonID));
        
        buttonDownHandled = TRUE;
    }

    if( padButtonStatus->padButtons & rsPADBUTTON2 )
    {
        /*
            * CROSS...
            */
        CursorDown = TRUE;


        if(MENUMODE != MenuGetStatus())
            PressButtonDown(Maestro,GetButtonID(SelectButtonID));
        
        buttonDownHandled = TRUE;
    }

    if( padButtonStatus->padButtons & rsPADBUTTON3 )
    {
        /*
            * SQUARE...
            */
        CursorLeft = TRUE;

        buttonDownHandled = TRUE;
    }

    if( padButtonStatus->padButtons & rsPADBUTTON4 )
    {
        /*
            * CIRCLE...
            */
        CursorRight = TRUE;

        buttonDownHandled = TRUE;
    }


	if( padButtonStatus->padButtons & rsPADBUTTON5 )
    {
        /*
         * Left shoulder '1'...
         */
        CmdZoomIn = TRUE;
        buttonDownHandled = TRUE;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON6 )
    {
        /*
         * Left shoulder '2'...
         */
        CmdZoomOut = TRUE;
        buttonDownHandled = TRUE;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON7 )
    {
        /*
         * Right shoulder '1'...
         */

        buttonDownHandled = TRUE;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON8 )
    {
        /*
         * Right shoulder '2'...
         */
        buttonDownHandled = TRUE;
    }
    else if( padButtonStatus->padButtons & rsPADDPADUP )
    {
        /*
         * D-PAD up...
         */
        if(MENUMODE != MenuGetStatus())
            PressButtonDown(Maestro,GetButtonID(UpButtonID));
        
        buttonDownHandled = TRUE;
    }
    else if( padButtonStatus->padButtons & rsPADDPADDOWN )
    {
        /*
         * D-PAD down...
         */
        if(MENUMODE != MenuGetStatus())
            PressButtonDown(Maestro,GetButtonID(DownButtonID));
        
        buttonDownHandled = TRUE;
    }
    else if( padButtonStatus->padButtons & rsPADDPADLEFT )
    {
        /*
         * D-PAD left...
         */
        if(MENUMODE != MenuGetStatus())
            PressButtonDown(Maestro,GetButtonID(LeftButtonID));
        
        buttonDownHandled = TRUE;
    }
    else if( padButtonStatus->padButtons & rsPADDPADRIGHT )
    {
        /*
         * D-PAD right...
         */
        if(MENUMODE != MenuGetStatus())
            PressButtonDown(Maestro,GetButtonID(RightButtonID));
        
        buttonDownHandled = TRUE;
    }
    else
    {
        return rsEVENTNOTPROCESSED;
    }

	return buttonDownHandled ? rsEVENTPROCESSED : rsEVENTNOTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadButtonUp(RsPadButtonStatus *padButtonStatus)
{
	RwBool buttonUpHandled = FALSE;
    
    if( padButtonStatus->padButtons & rsPADBUTTON1 )
    {
        /*
         * TRIANGLE...
         */
        CursorUp = FALSE;

        if(MENUMODE != MenuGetStatus())
            PressButtonUp(Maestro,GetButtonID(CancelButtonID));

        buttonUpHandled = TRUE;
    }

    if( padButtonStatus->padButtons & rsPADBUTTON2 )
    {
        /*
         * CROSS...
         */
        CursorDown = FALSE;
        
        if(MENUMODE != MenuGetStatus())
            PressButtonUp(Maestro,GetButtonID(SelectButtonID));

        buttonUpHandled = TRUE;
    }

    if( padButtonStatus->padButtons & rsPADBUTTON3 )
    {
        /*
         * SQUARE...
         */
        CursorLeft = FALSE;

        buttonUpHandled = TRUE;
    }

    if( padButtonStatus->padButtons & rsPADBUTTON4 )
    {
        /*
         * CIRCLE...
         */
        CursorRight = FALSE;

        buttonUpHandled = TRUE;
    }

    if( padButtonStatus->padButtons & rsPADBUTTON5 )
    {
        /*
         * Left shoulder '1'...
         */
        CmdZoomIn = FALSE;
        buttonUpHandled = TRUE;
    }
    
    if( padButtonStatus->padButtons & rsPADBUTTON6 )
    {
        /*
         * Left shoulder '2'...
         */
        CmdZoomOut = FALSE;
        buttonUpHandled = TRUE;
    }
    
    if( padButtonStatus->padButtons & rsPADBUTTON7 )
    {
        /*
         * Right shoulder '1'...
         */

    	if (Maestro)
    	{
	    	Rt2dMessage     message;

        	message.messageType = rt2dMESSAGETYPEMOUSEBUTTONSTATE;
        	message.index = -1;
        	message.intParam1 = (RwInt32)FALSE;

        	Rt2dMaestroPostMessages(Maestro, &message, 1);
    	}

        buttonUpHandled = TRUE;
    }
    
    if( padButtonStatus->padButtons & rsPADDPADUP )
    {
        /*
         * D-PAD up...
         */
        if(MENUMODE != MenuGetStatus())
            PressButtonUp(Maestro,GetButtonID(UpButtonID));
        
        buttonUpHandled = TRUE;
    }
    
    if( padButtonStatus->padButtons & rsPADDPADDOWN )
    {
        /*
         * D-PAD down...
         */
        if(MENUMODE != MenuGetStatus())
            PressButtonUp(Maestro,GetButtonID(DownButtonID));
        
        buttonUpHandled = TRUE;
    }
    
    if( padButtonStatus->padButtons & rsPADDPADLEFT )
    {
        /*
         * D-PAD left...
         */
        if(MENUMODE != MenuGetStatus())
            PressButtonUp(Maestro,GetButtonID(LeftButtonID));
        
        buttonUpHandled = TRUE;
    }
    
    if( padButtonStatus->padButtons & rsPADDPADRIGHT )
    {
        /*
         * D-PAD right...
         */
        if(MENUMODE != MenuGetStatus())
            PressButtonUp(Maestro,GetButtonID(RightButtonID));
        
        buttonUpHandled = TRUE;
    }

	return buttonUpHandled ? rsEVENTPROCESSED : rsEVENTNOTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadAnalogueLeft(void *param)
{
    RwV2d delta;
    RwV2d temp;

    RwV2dScale (&delta, (RwV2d *)param, 4.0f);

    Rt2dDeviceGetStep(&TranslateXStep, &TranslateYStep, &temp);

    Rt2dCTMTranslate(delta.x * TranslateXStep.x,
                    delta.x * TranslateXStep.y);

    Rt2dCTMTranslate(
        delta.y * TranslateYStep.x,
        delta.y * TranslateYStep.y);

    ViewChanged = TRUE;

    return rsEVENTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadAnalogueRight(void *param)
{
    RwV2d   delta;
    RwV2d xStep, yStep, origin;

    RwV2dScale (&delta, (RwV2d *)param, 2.0f);

    Rt2dDeviceGetStep(&xStep, &yStep, &origin);
    RwV2dScale(&xStep, &xStep, WinWidth * 0.5f);
    RwV2dScale(&yStep, &yStep, WinHeight - WinHeight * 0.5f);
    RwV2dAdd(&RotateOrigin, &xStep, &yStep);
    RwV2dAdd(&RotateOrigin, &RotateOrigin, &origin);

    Rt2dCTMTranslate(RotateOrigin.x, RotateOrigin.y);
    Rt2dCTMRotate(delta.x);
    Rt2dCTMTranslate(-RotateOrigin.x, -RotateOrigin.y);

    ViewChanged = TRUE;

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
