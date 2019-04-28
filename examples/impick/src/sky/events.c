
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
 * Purpose: Illustrate the picking and dragging of 3D immediate vertices.
 *
*****************************************************************************/

#include "rwcore.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"
#include "mouse.h"

#include "impick.h"

#define DELTAX (1.0f)
#define DELTAY (0.5f)

static RwV2d CursorPos = {100.0f, 100.0f};

static RwBool CursorUp = FALSE;
static RwBool CursorDown = FALSE;
static RwBool CursorLeft = FALSE;
static RwBool CursorRight = FALSE;

static RwBool VertexTranslateXY = FALSE;


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

    if( VertexTranslateXY )
    {
        Im3DSetVertexXY(&CursorPos);
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

    if( padButtonStatus->padButtons & rsPADBUTTON6 ) 
    {
        /* 
         * Left shoulder '2'... 
         */
        VertexTranslateXY = Im3DPickVertices(&CursorPos);

        buttonDownHandled = TRUE;
    }
    else
    {
        if( padButtonStatus->padButtons & rsPADBUTTON1 )
        {
            /*
             * TRIANGLE...
             */
            CursorUp = TRUE;

            buttonDownHandled = TRUE;
        }

        if( padButtonStatus->padButtons & rsPADBUTTON2 )
        {
            /*
             * CROSS...
             */
            CursorDown = TRUE;

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

    if( padButtonStatus->padButtons & rsPADBUTTON6 ) 
    {
        /* 
         * Left shoulder '2'... 
         */
        if( VertexTranslateXY )
        {
            Im3DResetPickedVertexColor();

            VertexTranslateXY = FALSE;
        }
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON1 )
    {
        /*
         * TRIANGLE...
         */
        CursorUp = FALSE;

        buttonUpHandled = TRUE;
    }
    
    if( padButtonStatus->padButtons & rsPADBUTTON2 )
    {
        /*
         * CROSS...
         */
        CursorDown = FALSE;

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

    return buttonUpHandled ? rsEVENTPROCESSED : rsEVENTNOTPROCESSED;
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

    Im3DRotate(delta.x, -delta.y);

    return rsEVENTPROCESSED;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadAnalogueRight(void *param)
{
    RwV2d delta = *(RwV2d *)param;

    delta.y = delta.y * RwRealAbs(delta.y) * 0.5f;

    Im3DTranslateZ(delta.y);

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

