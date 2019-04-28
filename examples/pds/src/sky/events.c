
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
 * Copyright (C) 2001 Criterion Software Ltd
 *
 * Author: RenderWare Team
 *
 ****************************************************************************/

#include "rwcore.h"

#include "skeleton.h"
#include "menu.h"
#include "camera.h"

extern RwCamera *Camera;

/*
 *****************************************************************************
 */
static RwCamera *
ViewerRotate(RwCamera *camera, RwReal deltaX, RwReal deltaY)
{
    if(camera != NULL)
    {
        CameraTilt(camera, NULL, deltaY);
        CameraPan(camera, NULL, deltaX);

		RwCameraSetNearClipPlane(Camera, 0.1f);
    }

    return camera;
}


/*
 *****************************************************************************
 */
static RwCamera *
ViewerTranslate(RwCamera *camera, RwReal deltaX __RWUNUSED__, RwReal deltaY)
{
    if(camera != NULL)
    {   RwV3d offset;

        offset.x = 0.0f;
        offset.y = 0.0f;
        offset.z = deltaY;

		RwCameraSetNearClipPlane(Camera, 0.1f);

        CameraMove(camera, &offset);
    }

    return camera;
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

    ViewerRotate(Camera, delta.x * 0.5f, delta.y * 0.5f);

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

    ViewerTranslate(Camera, delta.x, -delta.y);

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
