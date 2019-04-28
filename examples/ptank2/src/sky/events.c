
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
 * Purpose: The PTANK2 example shows how to create a PTank.
 * In this second example the particles are long shaped and have no texture.
 * Color is allocated per-particle, 2D rotation is applied per-particle.
 *                         
 ****************************************************************************/

#include "rwcore.h"
#include "rpworld.h"

#include "rtpng.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"

#include "ptank.h"



static RwBool TransformLight = FALSE;

extern RwInt32 NumSides;
extern RwInt32 NumRings;

extern void PTankClearData(void);
extern void PTankCreate(void);

extern void 
CameraPoint(RwReal tilt, RwReal turn);

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
HandlePadAnalogueLeft(void *param __RWUNUSED__)
{

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

