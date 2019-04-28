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
 * Purpose: RenderWare Graphics camera example.
 *          can be calculated.
 ****************************************************************************/


#include "rwcore.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"

#include "main.h"
#include "viewer.h"
#include "camexamp.h"

static RwBool TransformCamera = FALSE;

static RwBool ViewWindow = FALSE;
static RwBool ViewOffset = FALSE;


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
        ViewWindow = TRUE;

        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON3 )
    {
        /*
         * SQUARE...
         */
        ViewOffset = TRUE;

        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON6 )
    {
        /*
         * Left shoulder '2'...
         */
        TransformCamera = TRUE;

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
    TransformCamera = FALSE;

    ViewWindow = FALSE;
    ViewOffset = FALSE;

    return rsEVENTPROCESSED;
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

    if( ViewWindow )
    {
        ChangeViewWindow(delta.x * 0.01f, -delta.y * 0.01f);
    }
    else if( ViewOffset )
    {
        ChangeViewOffset(delta.x * 0.01f, -delta.y * 0.01f);
    }
    else if( TransformCamera )
    {
        ViewerRotate(GetSubCamera(), delta.x * 0.5f, delta.y * 0.5f);
    }
    else
    {
        ClumpRotate(Clump, GetMainCamera(), delta.x, -delta.y);
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

    delta.x = (-delta.x * RwRealAbs(delta.x)) * 5.0f;
    delta.y = (-delta.y * RwRealAbs(delta.y)) * 5.0f;

    if( TransformCamera )
    {
        ViewerTranslate(GetSubCamera(), delta.x * 0.01f, -delta.y * 0.01f);
    }
    else
    {
        ClumpTranslate(Clump, 
            GetMainCamera(), -delta.x * 0.01f, -delta.y * 0.1f);
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
