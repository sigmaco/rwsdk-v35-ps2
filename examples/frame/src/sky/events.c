
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
 * Purpose: To illustrate the frame hierarchy of a series of atomics 
 * in a clump.
 *****************************************************************************/

#include "rwcore.h"

#include "skeleton.h"
#include "menu.h"
#include "events.h"

#include "main.h"
#include "frame.h"


/*
 *****************************************************************************
 */
static RwFrame *
FindFirstChild(RwFrame *frame, void *data)
{
    /*
     * Find the first atomic that is attached to a child frame...
     */
    RwFrameForAllObjects(frame, GetFirstAtomic, data);
    
    return NULL;
}


/*
 *****************************************************************************
 */
static RsEventStatus
HandlePadButtonDown(RsPadButtonStatus *padButtonStatus)
{
    RwFrame *frame;

    if( padButtonStatus->padButtons & rsPADBUTTON1 )
    {
        /*
         * TRIANGLE, to select the current frame's parent...
         */
        frame = RwFrameGetParent(RpAtomicGetFrame(SelectedAtomic));
        if( frame )
        {
            /*
             * Find the atomic that is attached to the parent frame...
             */
            RwFrameForAllObjects(frame, GetFirstAtomic, (void *)&SelectedAtomic);
        }

        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON2 )
    {
        /*
         * CROSS, to select the current frame's first child...
         */
        RwFrameForAllChildren(RpAtomicGetFrame(SelectedAtomic),
                FindFirstChild, (void *)&SelectedAtomic);

        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON3 )
    {
        /*
         * SQUARE, to select the previous frame on the same level...
         */
        SelectedAtomic = 
            NextAndPreviousAtomic[GetAtomicIndex(SelectedAtomic)].previous;

        return rsEVENTPROCESSED;
    }
    else if( padButtonStatus->padButtons & rsPADBUTTON4 )
    {
         /*
          * CIRCLE, to select the next frame on the same level...
          */
         SelectedAtomic = 
             NextAndPreviousAtomic[GetAtomicIndex(SelectedAtomic)].next;

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

    delta.x = (-delta.x * RwRealAbs(delta.x)) * 5.0f;
    delta.y = (-delta.y * RwRealAbs(delta.y)) * 5.0f;

    FrameRotate(delta.x, -delta.y);

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
