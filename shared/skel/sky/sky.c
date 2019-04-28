/****************************************************************************
 *
 * sky.c
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
 * Copyright (c) 1999, 2000 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */
#ifdef __MWERKS__
	#include "mwUtils.h"
	#include "mwUtils_PS2.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <float.h>

#include <sys/errno.h>

#include <rwcore.h>
#include "rpworld.h"
#include "rppds.h"
#include "rtcharse.h"
#include "rtbmp.h"
#include "skeleton.h"
#include "camera.h"
#include "platform.h"
#include "vecfont.h"
#include "skyfs.h"

#include "eeregs.h"
#include "sifdev.h"
#include "sifrpc.h"

#if (defined(CDROM))
#include "libcdvd.h"
#endif /* (defined(CDROM)) */

#include "libpad.h"

#define PADNORMAL (4)
#define PADANALOG (7)
#define JOYCLAMPFLOAT (0.50f)
#define JOYMAXRADIUSFLOAT (127.5f)
#define PADSTABDELAY (50)

#if (!defined(RWDEBUG))
#define  __RWRELEASEUNUSED__   __RWUNUSED__
#endif /* (!defined(RWDEBUG)) */

#if (!defined(__RWRELEASEUNUSED__))
#define  __RWRELEASEUNUSED__   /* No op */
#endif /* (!defined(__RWRELEASEUNUSED__)) */

#if (!defined(__GNUC__))
#define __FUNCTION__ "Unknown"
#endif /* (!defined(__GNUC__)) */

typedef RwChar      SkyString[256];

typedef struct _RwSkyState RwSkyState;
struct _RwSkyState
{
    volatile unsigned long sweHighCount;
    int                 skyTimerHandlerHid;
    int                 keyboardHandle;
    char               *iopPathOverride;
    RwMemoryFunctions   bugWorkAround;
    SkyString           elfDir;
};

static RwSkyState   SkyState;

static unsigned int PadData;
static unsigned char ReadData[32] __attribute__ ((aligned(64)));
static u_long128    PadDmaBuffer[scePadDmaBufferMax]
    __attribute__ ((aligned(64)));

/*
 * If MENUING is defined on exit we launch a new elf.
 * This holds the name...
 */
#if (defined(MENUING))
SkyString           ExecuteElf;
#endif /* (defined(MENUING)) */

#if (defined(RWSPLASH))
extern void         PlayMPEG(const char *fname);
#endif /* (defined(RWSPLASH)) */

#if (defined(DEBUGSTATION))
char                debugStationFilePrefix[32];
#endif /* (defined(DEBUGSTATION)) */

#if (defined(CWDEBUGAT))
static int          cwDbgAtStdIOPort = -1;
#endif

#if (defined(CWDEBUGAT))
/*
 *****************************************************************************
 */
void
cwDbgPrintf(const char *fmt, ...)
{
    char                str[512];
    va_list             arg;

    /* make the string to write... */
    va_start(arg, fmt);
    vsprintf(str, fmt, arg);
    va_end(ap);

    /* write to the port */
    sceWrite(cwDbgAtStdIOPort, str, strlen(str) + 1);

}

#define printf cwDbgPrintf
#endif

/*
 *****************************************************************************
 */
#if (defined(RWTERMINAL))
static RwBool
SkyKeyboardOpen(void)
{
    /*
     * Opens a pipe, created with mkfifo on the Linux box...
     */
    static char         _rwpipe[] = "host0:rwpipe";
    RwBool              result;
    const int           mode =
        SCE_RDONLY | SCE_NOWAIT /*|SCE_NOWBDC */ ;

    RwDebugSendMessage(rwDEBUGMESSAGE, __FUNCTION__,
                       "Waiting for process to feed pipe\n");

    SkyState.keyboardHandle = sceOpen(_rwpipe, mode);
    result = (SkyState.keyboardHandle >= 0);

    if (result)
    {
        RwDebugSendMessage(rwDEBUGMESSAGE, __FUNCTION__, "Pipe opened");
    }
    else
    {
        RwDebugSendMessage(rwDEBUGMESSAGE, __FUNCTION__,
                           "Continuing with no keyboard");
    }

    return result;
}
#endif /* defined(RWTERMINAL) */

/*
 *****************************************************************************
 */
static int
TimerHandler(int ca)
{
    if ((ca == INTC_TIM0) && (*T0_MODE & 0x800))
    {
        *T0_MODE |= 0x800;

        SkyState.sweHighCount += 0x10000;
    }

    /* This is a TRC requirement at the moment */
    ExitHandler();

    return 0;
}

/*
 *****************************************************************************
 */
static void
SkyHandleKeyboard(void)
{
    if (SkyState.keyboardHandle >= 0)
    {
        static RwBool       first = TRUE;
        static unsigned char c[10];

        if (!first)
        {
            int                 is_end = 1, k;

            k = sceIoctl(SkyState.keyboardHandle, SCE_FS_EXECUTING,
                         &is_end);

            if (is_end == 0)
            {
                RsKeyStatus         keys;

                /* we don't have a scan code, just ascii */
                keys.keyScanCode = -1;
                keys.keyCharCode = c[0];

                RsKeyboardEventHandler(rsKEYDOWN, &keys);
                RsKeyboardEventHandler(rsKEYUP, &keys);

                /* read more for later... */
                sceRead(SkyState.keyboardHandle, c, sizeof(char));
            }
        }
        else
        {
            sceRead(SkyState.keyboardHandle, c, sizeof(char));

            first = FALSE;
        }
    }

    return;
}

/*
 *****************************************************************************
 */
static RwUInt32
remapButtons(int sceButtons)
{
    RwUInt32            rsButtons = 0;

    rsButtons |= (sceButtons & SCE_PADLup) ? rsPADDPADUP : 0;
    rsButtons |= (sceButtons & SCE_PADLdown) ? rsPADDPADDOWN : 0;
    rsButtons |= (sceButtons & SCE_PADLleft) ? rsPADDPADLEFT : 0;
    rsButtons |= (sceButtons & SCE_PADLright) ? rsPADDPADRIGHT : 0;
    rsButtons |= (sceButtons & SCE_PADstart) ? rsPADSTART : 0;
    rsButtons |= (sceButtons & SCE_PADselect) ? rsPADSELECT : 0;
    rsButtons |= (sceButtons & SCE_PADRup) ? rsPADBUTTON1 : 0;
    rsButtons |= (sceButtons & SCE_PADRdown) ? rsPADBUTTON2 : 0;
    rsButtons |= (sceButtons & SCE_PADRleft) ? rsPADBUTTON3 : 0;
    rsButtons |= (sceButtons & SCE_PADRright) ? rsPADBUTTON4 : 0;
    rsButtons |= (sceButtons & SCE_PADL1) ? rsPADBUTTON5 : 0;
    rsButtons |= (sceButtons & SCE_PADL2) ? rsPADBUTTON6 : 0;
    rsButtons |= (sceButtons & SCE_PADR1) ? rsPADBUTTON7 : 0;
    rsButtons |= (sceButtons & SCE_PADR2) ? rsPADBUTTON8 : 0;
    rsButtons |= (sceButtons & SCE_PADi) ? rsPADBUTTONA1 : 0;
    rsButtons |= (sceButtons & SCE_PADj) ? rsPADBUTTONA2 : 0;

    return rsButtons;
}

/*
 *****************************************************************************
 */
static void        *
private_malloc(size_t s)
{
    /* Since memalign() is broken, we fake it here */
    void               *newMem, *alignedMem;

    newMem = malloc(s + 64);
    if (newMem)
    {
        alignedMem = (void *) (((RwUInt32) newMem + 64) & ~63);

        /* Norty - assume sizeof(void **) == sizeof(RwUInt32) */
        ((void **) alignedMem)[-1] = newMem;
        ((RwUInt32 *) alignedMem)[-2] = s;
    }
    else
    {
        alignedMem = NULL;
    }

    return alignedMem;
}

/*
 *****************************************************************************
 */
static void
private_free(void *alignedMem)
{
    /* Since memalign() is broken, we fake it here */
    free(((void **) alignedMem)[-1]);

    return;
}

/*
 *****************************************************************************
 */
static void *
private_realloc(void *oldMem, size_t newSize)
{
    /* Since memalign() is broken, we fake it here */
    void               *newMem;

    newMem = private_malloc(newSize);
    if (newMem)
    {
        RwUInt32            oldSize = ((RwUInt32 *) oldMem)[-2];

        if (oldSize < newSize)
        {
            memcpy(newMem, oldMem, oldSize);
        }
        else
        {
            memcpy(newMem, oldMem, newSize);
        }

        private_free(oldMem);
    }

    return newMem;
}

/*
 *****************************************************************************
 */
static void *
private_calloc(size_t a, size_t b)
{
    /* Since memalign() is broken, we fake it here */
    size_t              realSize = a * b;
    void               *newMem;

    newMem = private_malloc(realSize);
    if (newMem)
    {
        memset(newMem, 0, realSize);
    }

    return newMem;
}

/*
 *****************************************************************************
 */
void
psMouseSetVisibility(RwBool visible __RWUNUSED__)
{
    return;
}

/*
 *****************************************************************************
 */
void
psMouseSetPos(RwV2d * pos __RWUNUSED__)
{
    return;
}

/*
 *****************************************************************************
 */
void
psWindowSetText(const RwChar * text __RWRELEASEUNUSED__)
{
    RwDebugSendMessage(rwDEBUGMESSAGE, __FUNCTION__, text);

    return;
}

/*
 *****************************************************************************
 */
void
psErrorMessage(const RwChar * message __RWRELEASEUNUSED__)
{
    RwDebugSendMessage(rwDEBUGERROR, __FUNCTION__, message);

    return;
}

/*
 *****************************************************************************
 */
void
psWarningMessage(const RwChar * message __RWRELEASEUNUSED__)
{
    RwDebugSendMessage(rwDEBUGMESSAGE, __FUNCTION__, message);

    return;
}

/*
 *****************************************************************************
 */
void
psCameraShowRaster(RwCamera * camera)
{
    RwCameraShowRaster(camera, NULL, 0);

    return;
}

/*
 *****************************************************************************
 */
RwImage *
psGrabScreen(RwCamera * camera)
{
    RwImage            *result = (RwImage *) NULL;
    RwRaster           *camRas;
    RwInt32             width, height, depth;
    RwImage            *image;
    RwUInt8            *oldCamPixels, *pixels;

    camRas = RwCameraGetRaster(camera);

    if (camRas)
    {
        width = RwRasterGetWidth(camRas);
        height = RwRasterGetHeight(camRas);
        depth = RwRasterGetDepth(camRas) >> 3;

        /*
         * On PS2, we must give the Camera a data area.
         * Ordinarily, PS2 camera rasters have no in memory image
         * Under normal conditions it is impossible to lock the camera
         * raster on PS2 as its a real performance problem. However,
         * by setting the cpPixel pointer the driver will assume that
         * you know what you are doing and are willing to take the hit
         */

        /* We know that malloc aligns on qw */
        oldCamPixels = camRas->cpPixels;

        pixels = (RwUInt8 *) malloc(width * height * depth);

        if ((pixels))
        {
            camRas->cpPixels = pixels;
            /* Lock the camera raster for read. This will do what ever is */
            /* required to get the data into memory */

            /* The PS2 driver doesn't do this for you */
            if (!RwRasterLock(camRas, 0, rwRASTERLOCKREAD))
            {
                camRas->cpPixels = oldCamPixels;
                free(pixels);
            }

            image = RwImageCreate(width, height, 32);
            if (image)
            {
                RwImageAllocatePixels(image);
                RwImageSetFromRaster(image, camRas);

                RwRasterUnlock(camRas);
                camRas->cpPixels = oldCamPixels;
                free(pixels);

                result = (image);
            }
            else
            {
                RwRasterUnlock(camRas);
                camRas->cpPixels = oldCamPixels;
                free(pixels);

                result = (RwImage *) NULL;
            }
        }
        else
        {
            result = (RwImage *) NULL;
        }

    }
    else
    {
        result = (RwImage *) NULL;
    }

    return result;
}

/*
 *****************************************************************************
 */
void
psDebugMessageHandler(RwDebugType type, const RwChar * str)
{
    switch (type)
    {
#if (defined(COLOR))
        case rwDEBUGASSERT:   /* red */
            printf("\033[31m%s\033[0m\n", str);
            break;
        case rwDEBUGERROR:    /* bold red */
            printf("\033[31;1m%s\033[0m\n", str);
            break;
        case rwDEBUGMESSAGE:  /* blue */
            printf("\033[34m%s\033[0m\n", str);
            break;
#endif /* (defined(COLOR)) */

        default:
            printf("%s\n", str);
    }

    return;
}

/*
 *****************************************************************************
 */
RwUInt32
psTimer(void)
{
    unsigned long       high0, low0, high1, low1;

    /* Just to make sure that the compiler can't mess with this... */
    asm volatile (".set noat
                   .set noreorder
                   .set nomacro
                   sync.l
                   sync.p
                   lui $1, 0x1000
                   ld  %0, 0(%4)
                   sync.l
                   sync.p
                   lwu %1, 0($1)
                   sync.l
                   sync.p
                   ld  %2, 0(%4)
                   sync.l
                   sync.p
                   lwu %3, 0($1)
                   sync.l
                   sync.p
                   .set macro
                   .set reorder
                   .set at
                   nop"
                  : "=r&" (high0), "=r&" (low0), "=r&" (high1), "=r&" (low1)
                  : "r" (&SkyState.sweHighCount));

    if (high0 == high1)
    {
        return ((RwUInt32) ((high0 | (low0 & 0xffff)) / 9216));
    }
    else
    {
        return ((RwUInt32) ((high1 | (low1 & 0xffff)) / 9216));
    }
}

/*
 *****************************************************************************
 */
RwUInt64
psMicroTimer(void)
{
    unsigned long       high0, low0, high1, low1;

    /* Just to make sure that the compiler can't mess with this... */
    asm volatile (".set noat
                   .set noreorder
                   .set nomacro
                   sync.l
                   sync.p
                   lui $1, 0x1000
                   ld  %0, 0(%4)
                   sync.l
                   sync.p
                   lwu %1, 0($1)
                   sync.l
                   sync.p
                   ld  %2, 0(%4)
                   sync.l
                   sync.p
                   lwu %3, 0($1)
                   sync.l
                   sync.p
                   .set macro
                   .set reorder
                   .set at
                   nop"
                  : "=r&" (high0), "=r&" (low0), "=r&" (high1), "=r&" (low1)
                  : "r" (&SkyState.sweHighCount));

    if (high0 == high1)
    {
        return (((RwUInt64) (high0 | (low0 & 0xffff))) * 1000) / 9216;
    }
    else
    {
        return (((RwUInt64) (high1 | (low1 & 0xffff))) * 1000) / 9216;
    }
}

/*
 *****************************************************************************
 */
RwChar *
psPathnameCreate(const RwChar * pathname)
{
    if (pathname)
    {
        char               *dstBuffer;

        dstBuffer = (char *)
            RwMalloc(sizeof(RwChar) * (rwstrlen(pathname) + 1));

        if (dstBuffer)
        {
            char               *charToConvert;

            strcpy(dstBuffer, pathname);

            /* Convert a path for use on PSX 2.
             * Convert all \s and :s into /s */
            while ((charToConvert = rwstrchr(dstBuffer, '\\')))
            {
                *charToConvert = '/';
            }

            while ((charToConvert = rwstrchr(dstBuffer, ':')))
            {
                *charToConvert = '/';
            }
        }

        return dstBuffer;
    }

    return 0;
}

/*
 *****************************************************************************
 */
void
psPathnameDestroy(RwChar * pathname)
{
    if (pathname)
    {
        RwFree(pathname);

        pathname = 0;
    }

    return;
}

/*
 *****************************************************************************
 */
RwChar
psPathGetSeparator(void)
{
#ifdef CDROM
    return '\\';
#else
    return '/';
#endif /* CDROM */
}

/*
 *****************************************************************************
 */
void
skyOverideIOPPath(char *newPath)
{
    /*
     * Stealth function to overide IOP paths, I use strdup
     * and free in case this is called before RenderWare is
     * initialised
     */
    if (SkyState.iopPathOverride)
    {
        free(SkyState.iopPathOverride);
    }

    if (newPath)
    {
        SkyState.iopPathOverride = strdup(newPath);
    }
    else
    {
        SkyState.iopPathOverride = 0;
    }

    return;
}

/*
 *****************************************************************************
 */
RwBool
psAlwaysOnTop(RwBool AlwaysOnTop __RWUNUSED__)
{
    return TRUE;
}

/*
 *****************************************************************************
 */
RwBool
psSelectDevice(RwBool useDefault)
{
    RwVideoMode         videoMode;

    if (useDefault)
    {
        /*
         * Select the default as 640x448x32xFSAA1
         */
        RwVideoMode vmodeInfo;
        RwInt32 i;
        RwInt32 numVideoModes = RwEngineGetNumVideoModes();

        for (i = 0; i < numVideoModes; i++)
        {
            if (RwEngineGetVideoModeInfo(&vmodeInfo, i))
            {
                if (vmodeInfo.width == 640 &&
                    vmodeInfo.height == 448 &&
                    vmodeInfo.depth == 32 &&
                    vmodeInfo.flags == (rwVIDEOMODEEXCLUSIVE|
                                        rwVIDEOMODEINTERLACE|
                                        rwVIDEOMODEFSAA1))
                {
                    RwEngineSetVideoMode(i);
                    break;
                }
            }
        }
    }
    else
    {
        /*
         * At this point the Engine is Init() and Open()
         * The Pad is also open()
         */
        RwEngineGetVideoModeInfo(&videoMode,
                                 RwEngineGetCurrentVideoMode());
        /*
         * Define the maximum extents based on the default video mode
         */
        RsGlobal.maximumWidth = videoMode.width;
        RsGlobal.maximumHeight = videoMode.height;

        /* Start the engine */
        if (!RwEngineStart())
        {
            return (FALSE);
        }
        else
        {
            RwCamera           *camera;
            RtCharset          *charset, *invCharset;
            RwBool              vModeSelected = FALSE;
            RwInt32             selectedVideoMode = 0;
            SkyString           videoModeDesc[100];
            RwInt32             numVideoModes;
            RwRGBA              foregroundColor =
                { 200, 200, 200, 255 };
            RwRGBA              backgroundColor = { 64, 64, 64, 0 };

            /*
             * Create two Charsets, one for unselected and one for selected
             */
            charset =
                RtCharsetCreate(&foregroundColor, &backgroundColor);
            invCharset =
                RtCharsetCreate(&backgroundColor, &foregroundColor);
            if (charset == NULL || invCharset == NULL)
            {
                RsErrorMessage(RWSTRING("Cannot create charsets."));
                return (FALSE);
            }

            /*
             * Create a camera using the democom way...
             */
            camera =
                CameraCreate(RsGlobal.maximumWidth,
                             RsGlobal.maximumHeight, TRUE);
            if (camera)
            {
                RwCameraSetNearClipPlane(camera, 0.1f);
                RwCameraSetFarClipPlane(camera, 5.0f);
            }
            else
            {
                RsErrorMessage(RWSTRING("Cannot create camera"));
                return (FALSE);
            }

            /*
             * Construct list of all available video modes
             */
            {
                RwVideoMode         vmodeInfo;
                RwInt32             i;

                /*
                 * One extra for default mode (will appear twice)
                 */
                numVideoModes = RwEngineGetNumVideoModes() + 1;

                /*
                 * Set up default video mode to appear at top of menu
                 */
                sprintf(videoModeDesc[0],
                        RWSTRING
                        ("Default Video Mode: %dx%dx%d %s%s%s%s%s\n"),
                        videoMode.width, videoMode.height,
                        videoMode.depth,
                        (videoMode.
                         flags & rwVIDEOMODEEXCLUSIVE ? "EXCLUSIVE " :
                         ""),
                        (videoMode.
                         flags & rwVIDEOMODEINTERLACE ? "INTERLACED " :
                         ""),
                        (videoMode.
                         flags & rwVIDEOMODEFFINTERLACE ?
                         "FLICKER-FREE " : ""),
                        (videoMode.
                         flags & rwVIDEOMODEFSAA0 ? "ANTI-ALIASED-0 " :
                         ""),
                        (videoMode.
                         flags & rwVIDEOMODEFSAA1 ? "ANTI-ALIASED-1 " :
                         ""));

                for (i = 1; i < numVideoModes; i++)
                {
                    if (RwEngineGetVideoModeInfo(&vmodeInfo, (i - 1)))
                    {
                        sprintf(videoModeDesc[i],
                                RWSTRING
                                ("Video Mode %d: %dx%dx%d %s%s%s%s%s\n"),
                                (i - 1), vmodeInfo.width,
                                vmodeInfo.height, vmodeInfo.depth,
                                (vmodeInfo.
                                 flags & rwVIDEOMODEEXCLUSIVE ?
                                 "EXCLUSIVE " : ""),
                                (vmodeInfo.
                                 flags & rwVIDEOMODEINTERLACE ?
                                 "INTERLACED " : ""),
                                (vmodeInfo.
                                 flags & rwVIDEOMODEFFINTERLACE ?
                                 "FLICKER-FREE " : ""),
                                (vmodeInfo.
                                 flags & rwVIDEOMODEFSAA0 ?
                                 "ANTI-ALIASED-0 " : ""),
                                (vmodeInfo.
                                 flags & rwVIDEOMODEFSAA1 ?
                                 "ANTI-ALIASED-1 " : ""));
                    }
                }
            }

            /*
             * Our own event loop
             */
            while (!vModeSelected)
            {
                RtCharsetDesc       charsetDesc;
                RwInt32             currentPage = 0;
                RwInt32             numPerPage = 0;

                RwCameraClear(camera, &backgroundColor,
                              rwCAMERACLEARZ | rwCAMERACLEARIMAGE);

                RtCharsetGetDesc(charset, &charsetDesc);

                numPerPage =
                    (RwRasterGetHeight(RwCameraGetRaster(camera)) /
                     charsetDesc.height) - 2;

                currentPage = selectedVideoMode / numPerPage;

                if (RwCameraBeginUpdate(camera))
                {
                    RwInt32             i, j = 1;

                    /*
                     * Display Video Modes
                     */
                    for (i = (currentPage * numPerPage);
                         i < ((currentPage + 1) * numPerPage); i++)
                    {
                        if (i > numVideoModes)
                        {
                            break;
                        }
                        if (i == selectedVideoMode)
                        {
                            RtCharsetPrint(invCharset, videoModeDesc[i],
                                           5 * charsetDesc.width,
                                           charsetDesc.height * j++);
                        }
                        else
                        {
                            RtCharsetPrint(charset, videoModeDesc[i],
                                           5 * charsetDesc.width,
                                           charsetDesc.height * j++);
                        }
                    }
                    RwCameraEndUpdate(camera);
                }
                RsCameraShowRaster(camera);

                /*
                 * Simple event loop...
                 */
                {
                    static RwUInt32     oldButtons = 0;
                    RwUInt32            curButtons;
                    RsPadButtonStatus   padButtonsDown;

                    /*
                     * Read the current state of the pad
                     */
                    if (scePadRead(0, 0, ReadData) > 0)
                    {
                        PadData =
                            0xffff ^ ((ReadData[2] << 8) | ReadData[3]);
                    }
                    else
                    {
                        PadData = 0;
                    }

                    /*
                     * Map buttons to RenderWare buttons.
                     */
                    curButtons = remapButtons(PadData);

                    /*
                     * Detect changes in button states (button down).
                     */
                    padButtonsDown.padButtons =
                        ~oldButtons & curButtons;

                    if (padButtonsDown.padButtons & rsPADDPADUP)
                    {
                        /*
                         *  Select previous video mode
                         */
                        selectedVideoMode =
                            (selectedVideoMode ==
                             0) ? (numVideoModes -
                                   1) : (selectedVideoMode - 1);
                    }
                    if (padButtonsDown.padButtons & rsPADDPADDOWN)
                    {
                        /*
                         *  Select next video mode
                         */
                        selectedVideoMode =
                            (selectedVideoMode + 1) % numVideoModes;
                    }
                    if (padButtonsDown.padButtons & rsPADBUTTON5)
                    {
                        /*
                         *  View previous page of video modes
                         */
                        RwInt32             newPage =
                            (currentPage ==
                             0) ? (numVideoModes /
                                   numPerPage) : (currentPage - 1);
                        selectedVideoMode = newPage * numPerPage;
                    }
                    if (padButtonsDown.padButtons & rsPADBUTTON6)
                    {
                        /*
                         *  View next page of video modes
                         */
                        RwInt32             newPage =
                            (currentPage ==
                             numVideoModes /
                             numPerPage) ? 0 : (currentPage + 1);
                        selectedVideoMode = newPage * numPerPage;
                    }
                    if (padButtonsDown.padButtons & rsPADSTART)
                    {
                        /*
                         *  Terminate loop, selection made
                         */
                        vModeSelected = TRUE;
                    }
                    /*
                     * Remeber old button state - events only occur once, no autorepeat.
                     */
                    oldButtons = curButtons;
                }
            }
            RtCharsetDestroy(charset);
            RtCharsetDestroy(invCharset);

            CameraDestroy(camera);

            RwEngineStop();

            /*
             * Set video mode if different from the default
             */
            if (selectedVideoMode != 0)
            {
                RwEngineSetVideoMode(selectedVideoMode - 1);
            }
        }
    }

    /*
     * Define the maximum extents based on the selected video mode
     */
    RwEngineGetVideoModeInfo(&videoMode, RwEngineGetCurrentVideoMode());

    RsGlobal.maximumWidth = videoMode.width;
    RsGlobal.maximumHeight = videoMode.height;

    return (TRUE);
}

/*
 *****************************************************************************
 */
RwMemoryFunctions*
psGetMemoryFunctions(void)
{
    /* Since memalign() is broken, we fake it here */

    /* We return a work around for malloc alignment on EB-2000S */
    /* One day, this may not be necessary */
    SkyState.bugWorkAround.rwmalloc = private_malloc;
    SkyState.bugWorkAround.rwfree = private_free;
    SkyState.bugWorkAround.rwrealloc = private_realloc; /* Hmm, this might need a fix too */
    SkyState.bugWorkAround.rwcalloc = private_calloc;

    /* Appears fixed in the 0909 flash so return NULL */
    /* return(&SkyState.bugWorkAround); */
    return 0;
}

/*
 *****************************************************************************
 */
RwBool
psInstallFileSystem(void)
{
    if (SkyInstallFileSystem(SkyState.elfDir))
    {
        RwDebugSendMessage(rwDEBUGMESSAGE, __FUNCTION__,
                           "sky file system installed");
        return TRUE;
    }

    return FALSE;
}

/*
 *****************************************************************************
 */
RwBool
psNativeTextureSupport(void)
{
    return TRUE;
}

/*
 *****************************************************************************
 */
void
psTerminate(void)
{
#if (defined(RWDEBUG) && defined(RWMEMDEBUG))
    RwUInt32            hist;
    RwUInt32            bytes =
        (RtDBMallocInuse(__FILE__, __LINE__, &hist));

    printf("%s(%d):\n    %d = RtDBMallocInuse( ... , %d == hist)\n",
           __FILE__, __LINE__, bytes, hist);

    RtDBMallocDump();
#endif /* (defined(RWDEBUG) && defined(RWMEMDEBUG)) */

    return;
}

/*
 *****************************************************************************
 */
RwBool
psInitialize(void)
{
#if (defined(RWDEBUG) && defined(RWMEMDEBUG))
    union RtDBmalloptarg m;

    m.i = 0;                   /* disable zero length allocation warnings */
    RtDBmallopt(MALLOC_ZERO, &m);

#if (defined(RWTHOROUGHCHECKS))
    m.i = 1;                   /* enable chain checking */
    RtDBmallopt(MALLOC_CKCHAIN, &m);

    m.i = 3;                   /* enable fill area */
    RtDBmallopt(MALLOC_FILLAREA, &m);
#endif /* (defined(RWTHOROUGHCHECKS)) */

#endif /* (defined(RWDEBUG) && defined(RWMEMDEBUG)) */

    return TRUE;
}

/*
 *****************************************************************************
 */
#if (defined(RWSPLASH))
RwBool
psDisplaySplashScreen(RwBool state)
{
    if (!state)
    {
        /* only play at startup - before video modes are initialised */
#error Insert name of the splash pss file here
        PlayMPEG("splash.pss");
    }

    return TRUE;
}
#endif /* (defined(RWSPLASH)) */

/*
 *****************************************************************************
 */
static RwBool
SkyPadDetect(void)
{
    int                 state, id, exid;
    int                 nbStabFrame;

    /*
     * * Wait until the pad is stable...
     */
    nbStabFrame = 0;
    do
    {
        state = scePadGetState(0, 0);
        nbStabFrame++;
    }
    while (state != scePadStateStable &&
           state != scePadStateFindCTP1 && nbStabFrame < PADSTABDELAY);

    if (state != scePadStateStable && state != scePadStateFindCTP1)
    {
        return FALSE;
    }

    /*
     * * Get the ID of the pad...
     */
    id = scePadInfoMode(0, 0, InfoModeCurID, 0);

    exid = scePadInfoMode(0, 0, InfoModeCurExID, 0);
    if (exid > 0)
    {
        id = exid;
    }

    switch (id)
    {
        case PADNORMAL:       /* STANDARD */
            {
                /*
                 * Check the pad supports analogue \ vibration functionality...
                 */
                if (scePadInfoMode(0, 0, InfoModeCurExID, 0) == 0)
                {
                    break;
                }
                /*
                 * Change to analogue mode and lock...
                 */
                do
                {
                    scePadSetMainMode(0, 0, 1, 1);
                }
                while (scePadGetReqState(0, 0) == scePadReqStateFaild);

                break;
            }
        case PADANALOG:       /* ANALOG */
            {
                /*
                 * leave in analogue mode...
                 */
                break;
            }
        default:              /* UNKNOWN */
            {
                break;
            }
    }

    return TRUE;
}

#if (defined(CDROM) || defined(DEBUGSTATION))
#if (!defined(SKELETON_CDROM))
#define SKELETON_CDROM
#endif /* (!defined(SKELETON_CDROM)) */
#endif /* (defined(CDROM) || defined(DEBUGSTATION)) */

#if (defined(RWSPLASH) || defined(RWSOUND) )
#if (!defined(SKELETON_SOUND))
#define SKELETON_SOUND
#endif /* (!defined(SKELETON_SOUND)) */
#endif /* (defined(RWSOUND)) */

#define SkyPadOpen_Local_string(arg) #arg
#define SkyPadOpen_Local_eval(arg) SkyPadOpen_Local_string(arg)

/*
 *****************************************************************************
 */
static              RwBool
SkyPadOpen(void)
{
    SkyString           module[4];

#if (defined(SKELETON_CDROM))
    {
        strcpy(module[0], "cdrom0:\\SIO2MAN.IRX;1");
        strcpy(module[1], "cdrom0:\\PADMAN.IRX;1");

#if (defined(SKELETON_SOUND))
        strcpy(module[2], "cdrom0:\\LIBSD.IRX;1");
        strcpy(module[3], "cdrom0:\\SDRDRV.IRX;1");
#endif /* (defined(SKELETON_SOUND)) */

    }
#else /* (defined(SKELETON_CDROM)) */
    {
        SkyString           iopPath;

        if (SkyState.iopPathOverride)
        {
            strcpy(iopPath, SkyState.iopPathOverride);
            strcat(iopPath, "/modules/");
        }
        else
#if (defined(IOPPATH))
        {
            static const RwChar iopDir[] =
                SkyPadOpen_Local_eval(IOPPATH);

            strcpy(iopPath, iopDir);
            strcat(iopPath, "/modules/");
        }
#else /* (defined(IOPPATH)) */
        {
            strcpy(iopPath, "");
        }
#endif /* (defined(IOPPATH)) */

        sprintf(module[0], "host0:%ssio2man.irx", iopPath);
        sprintf(module[1], "host0:%spadman.irx", iopPath);

#if (defined(SKELETON_SOUND))
        sprintf(module[2], "host0:%slibsd.irx", iopPath);
        sprintf(module[3], "host0:%ssdrdrv.irx", iopPath);
#endif /* (defined(SKELETON_SOUND)) */

    }

#endif /* (defined(SKELETON_CDROM)) */

#if (defined(IOPPATH))
#undef SkyPadOpen_Local_eval
#undef SkyPadOpen_Local_string
#endif /* (defined(IOPPATH)) */

    /* sceSifInitRpc(0); */
    {
        static char         _empty[] = "";
        RwChar             *irxPath;

        irxPath = module[0];
        if (sceSifLoadModule(irxPath, 0, _empty) < 0)
        {
#if (defined(RWDEBUG))
            SkyString           buf;

            sprintf(buf, "%s(%d): Can't Load Module %s",
                    __FILE__, __LINE__, irxPath);
            printf("Skeleton Error(%s) : %s", __FUNCTION__, buf);
#endif /* (defined(RWDEBUG)) */
            return FALSE;
        }

        irxPath = module[1];
        if (sceSifLoadModule(irxPath, 0, _empty) < 0)
        {
#if (defined(RWDEBUG))
            SkyString           buf;

            sprintf(buf, "%s(%d): Can't Load Module %s",
                    __FILE__, __LINE__, irxPath);
            printf("Skeleton Error(%s) : %s", __FUNCTION__, buf);
#endif /* (defined(RWDEBUG)) */
            return FALSE;
        }

#if (defined(SKELETON_SOUND))
        {
            irxPath = module[2];
            if (sceSifLoadModule(irxPath, 0, _empty) < 0)
            {
#if (defined(RWDEBUG))
                SkyString           buf;

                sprintf(buf, "%s(%d): Can't Load Module %s",
                        __FILE__, __LINE__, irxPath);
                printf("Skeleton Error(%s) : %s", __FUNCTION__, buf);
#endif /* (defined(RWDEBUG)) */
                return FALSE;
            }

            irxPath = module[3];
            if (sceSifLoadModule(irxPath, 0, _empty) < 0)
            {
#if (defined(RWDEBUG))
                SkyString           buf;

                sprintf(buf, "%s(%d): Can't Load Module %s",
                        __FILE__, __LINE__, irxPath);
                printf("Skeleton Error(%s) : %s", __FUNCTION__, buf);
#endif /* (defined(RWDEBUG)) */
                return FALSE;
            }

        }
#endif /* (defined(SKELETON_SOUND)) */

    }

#if (!defined(NOPADHANDLING))
    scePadInit(0);
    scePadPortOpen(0, 0, PadDmaBuffer);
    SkyPadDetect();
#endif /* (!defined(NOPADHANDLING)) */
    return TRUE;
}

/*
 *****************************************************************************
 */
static void
SkyHandlePad(void)
{
    RsPadButtonStatus   padButtonsDown;
    RsPadButtonStatus   padButtonsUp;
    static unsigned char leftH = 0;
    static unsigned char leftV = 0;
    RwInt32             dLeftx, dLefty;
    static RwReal       leftJoyMaxFloat, leftJoyClampFloat;
    static RwInt32      leftJoyClampSquared;
    RwInt32             leftRadSquared;
    static RwBool       LeftPadMove = FALSE;
    static unsigned char rightH = 0;
    static unsigned char rightV = 0;
    RwInt32             dRightx, dRighty;
    static RwReal       rightJoyMaxFloat, rightJoyClampFloat;
    static RwInt32      rightJoyClampSquared;
    RwInt32             rightRadSquared;
    static RwBool       RightPadMove = FALSE;
    static RwUInt32     oldButtons = 0;
    RwUInt32            buttons;
    int                 padId;

    /* Read controller data */
    if (SkyPadDetect())
    {

        if (scePadRead(0, 0, ReadData) > 0)
        {
            PadData = 0xffff ^ ((ReadData[2] << 8) | ReadData[3]);
        }
        else
        {
            PadData = 0;
        }

        /* get id of pad type  */
        padId = (ReadData[1] >> 4);
        /* Only one pad supported */
        padButtonsDown.padID = padButtonsUp.padID = 0;
        /* Remap the buttons to RenderWare ones */
        buttons = remapButtons(PadData);
        /* Figure out what has changed */
        padButtonsDown.padButtons = ~oldButtons & buttons;
        padButtonsUp.padButtons = oldButtons & ~buttons;
        /* left analogue stick */
        if (!leftH)
        {
            RwReal              centreDistFloat;
            RwReal              leftJoyClampCeil;

            /* first time through - sample the centre pos */
            leftH = ReadData[6];
            leftV = ReadData[7];
            /* We limit our range to the biggest circle (centred on
             * (leftH, leftV) ) which fits in the [(0,0), (255, 255)]
             * range (or rather the 127.5 radius circle centred on
             * (127.5, 127.5) ), so that the magnitude of the joystick's
             * range is the same in all directions from the centre. */
            rwSqrt(&centreDistFloat,
                   (leftH - JOYMAXRADIUSFLOAT) *
                   (leftH - JOYMAXRADIUSFLOAT) +
                   (leftV - JOYMAXRADIUSFLOAT) *
                   (leftV - JOYMAXRADIUSFLOAT));
            /* maxDist is maxDist from (leftH, leftV) */
            leftJoyMaxFloat = JOYMAXRADIUSFLOAT - centreDistFloat;

            if (leftJoyMaxFloat < 0)
                leftJoyMaxFloat = 0; /* Dude, get a new pad. */

            leftJoyClampFloat = JOYCLAMPFLOAT * leftJoyMaxFloat;
            /* Be careful to make this at or *above* the float
             * clamp value so that we don't accept values inside
             * the clamp region resulting in occasional negative
             * values right on the edge of the clamp region. */
            leftJoyClampCeil = RwCeil(leftJoyClampFloat);
            leftJoyClampSquared = (RwInt32) leftJoyClampCeil;
            leftJoyClampSquared *= leftJoyClampSquared;
        }

        dLeftx = leftH - ReadData[6];
        dLefty = leftV - ReadData[7];
        leftRadSquared = dLeftx * dLeftx + dLefty * dLefty;
        if (leftRadSquared > leftJoyClampSquared && padId == PADANALOG)
        {
            RwV2d               delta;
            RwReal              radFloat, scaleFloat;

            /* Get distance from the centre */
            rwSqrt(&radFloat, (RwReal) leftRadSquared);
            RSASSERT(radFloat >= leftJoyClampFloat);
            if (radFloat > leftJoyMaxFloat)
            {
                /* We're beyond the outer limit,
                 * clamp vector length to 1.0 */
                scaleFloat = 1.0f / radFloat;
            }
            else
            {
                /* Scale the vector length to [0,1].
                 * Zero at the edge of the clamp region,
                 * one at the outer limit. */
                scaleFloat =
                    (radFloat -
                     leftJoyClampFloat) /
                    ((leftJoyMaxFloat - leftJoyClampFloat) * radFloat);
            }

            /* Map to the appropriate value */
            delta.x = scaleFloat * (RwReal) dLeftx;
            delta.y = scaleFloat * (RwReal) dLefty;
            LeftPadMove = TRUE;
            RsPadEventHandler(rsPADANALOGUELEFT, &delta);
        }
        else if (LeftPadMove == TRUE)
        {

            LeftPadMove = FALSE;
            RsPadEventHandler(rsPADANALOGUELEFTRESET, NULL);
        }

        /* right analogue stick */
        if (!rightH)
        {
            RwReal              centreDistFloat;
            RwReal              rightJoyClampCeil;

            rightH = ReadData[4];
            rightV = ReadData[5];
            rwSqrt(&centreDistFloat,
                   (rightH - JOYMAXRADIUSFLOAT) *
                   (rightH - JOYMAXRADIUSFLOAT) +
                   (rightV - JOYMAXRADIUSFLOAT) *
                   (rightV - JOYMAXRADIUSFLOAT));
            rightJoyMaxFloat = JOYMAXRADIUSFLOAT - centreDistFloat;
            rightJoyClampFloat = JOYCLAMPFLOAT * rightJoyMaxFloat;
            rightJoyClampCeil = RwCeil(rightJoyClampFloat);
            rightJoyClampSquared = (RwInt32) rightJoyClampCeil;
            rightJoyClampSquared *= rightJoyClampSquared;
        }

        dRightx = rightH - ReadData[4];
        dRighty = rightV - ReadData[5];
        rightRadSquared = dRightx * dRightx + dRighty * dRighty;
        if (rightRadSquared >
            rightJoyClampSquared && padId == PADANALOG)
        {
            RwV2d               delta;
            RwReal              radFloat, scaleFloat;

            rwSqrt(&radFloat, (RwReal) rightRadSquared);
            RSASSERT(radFloat >= rightJoyClampFloat);
            if (radFloat > rightJoyMaxFloat)
            {
                scaleFloat = 1.0f / radFloat;
            }
            else
            {
                scaleFloat =
                    (radFloat -
                     rightJoyClampFloat) /
                    (radFloat *
                     (rightJoyMaxFloat - rightJoyClampFloat));
            }

            delta.x = scaleFloat * (RwReal) dRightx;
            delta.y = scaleFloat * (RwReal) dRighty;
            RightPadMove = TRUE;
            RsPadEventHandler(rsPADANALOGUERIGHT, &delta);
        }
        else if (RightPadMove == TRUE)
        {

            RightPadMove = FALSE;
            RsPadEventHandler(rsPADANALOGUERIGHTRESET, NULL);
        }

        /* If any buttons just went down, or came up, message the app */
        if (padButtonsDown.padButtons)
        {
            RsPadEventHandler(rsPADBUTTONDOWN, &padButtonsDown);
        }

        if (padButtonsUp.padButtons)
        {
            RsPadEventHandler(rsPADBUTTONUP, &padButtonsUp);
        }

        /* Start and select at same time to quit */
        if ((buttons &
             (rsPADSTART | rsPADSELECT)) == (rsPADSTART | rsPADSELECT))
        {
            /* Send a quit message - this allows app to do stuff */
            RsEventHandler(rsQUITAPP, NULL);
        }

        /* Save buttons so we know what's changed next time */
        oldButtons = buttons;
    }
    return;
}

/*
 *****************************************************************************
 */
#if (defined(RWMETRICS))
void
psMetricsRender(RsVectorFont
                * vecFont, RwV2d * pos, RwMetrics * metrics)
{
    RwSkyMetrics       *skyMetrics =
        (RwSkyMetrics *) metrics->devSpecificMetrics;
    if ((skyMetrics) && (skyMetrics->profTotal))
    {
        RwChar              message[200];
        RwInt32             val, integer, fraction;

        sprintf(message, "numextrapasstris = %08d", 
            skyMetrics->numExtraPassTriangles);
        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;
        val = (10000 * skyMetrics->vu1Running) / skyMetrics->profTotal;
        integer = val / 100;
        fraction = val % 100;
        sprintf(message,
                "vu1 utilisation = %02d.%02d%%", integer, fraction);
        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;
        val = (10000 * skyMetrics->dma1Running) / skyMetrics->profTotal;
        integer = val / 100;
        fraction = val % 100;
        sprintf(message,
                "dma1 utilisation = %02d.%02d%%", integer, fraction);
        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;
        val = (10000 * skyMetrics->dma2Running) / skyMetrics->profTotal;
        integer = val / 100;
        fraction = val % 100;
        sprintf(message,
                "dma2 utilisation = %02d.%02d%%", integer, fraction);
        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;
        val = (10000 * skyMetrics->cpuRunning) / skyMetrics->profTotal;
        integer = val / 100;
        fraction = val % 100;
        sprintf(message,
                "cpu utilisation = %02d.%02d%%", integer, fraction);
        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;
        sprintf(message,
                "VSyncs between flips = %d",
                skyMetrics->vSyncsSinceLastFlip);
        RsVecFontPrint(vecFont, pos, message);
        pos->y += 10.0f;
        /* If you want metrics as though at 60Hz, multiply percentages by:
         * (float) skyMetrics->totalVSyncsOverPeriod
         * /(rwSKYMETRICSSAMPLES<<rwSKYMETRICSFRAMESPERSAMPLE);
         */
    }

    return;
}
#endif /* (defined(RWMETRICS)) */

/*
 *****************************************************************************
 */
int
main(int argc, char *argv[])
{
    RwInt32             i;
    RwInt32             firstArg = 1;

#ifdef __MWERKS__
#ifdef CWATWIN
	    mwAtInit(argv[0]);
#else
#if ((defined __cplusplus) || (defined USE_RWAI))
      mwInit();
#endif /* ((defined __cplusplus) || (defined USE_RWAI)) */
#endif /* CWATWIN */
#endif /* __MWERKS__ */

#if (defined(SKYREBOOTIOP))
    char                iopImageFile[64];
#endif /* (defined(SKYREBOOTIOP)) */
    /* by default don't look in subdirs for assets */
    SkyState.skyTimerHandlerHid = -1;
    SkyState.keyboardHandle = -1;
    SkyState.iopPathOverride = 0;
    SkyState.elfDir[0] = '\0';
    /* say that we want a normal shutdown */
#if (defined(MENUING))
    ExecuteElf[0] = '\0';
    /* look at the program name to extract the elf dir */
    {
        RwChar             *a, *b;

        a = argv[0];
        while (*a != ':')
            a++;
        b = a + strlen(a) - 1;
        while (*b != '\\')
            b--;
        if (b == (a + 1))
        {
            strcpy(SkyState.elfDir, "\\");
        }
        else
        {
            strncpy(SkyState.elfDir, a + 1, b - a - 1);
        }
    }
#endif /* (defined(MENUING)) */

    sceSifInitRpc(0);
    sceSifInitIopHeap();
#if (defined(CDROM))
    sceCdInit(SCECdINIT);
    sceCdMmode(SCECdCD);
#endif /* (defined(CDROM)) */
#if (defined(SKYREBOOTIOP))
#if (defined(CDROM) || defined(DEBUGSTATION))
    sprintf(iopImageFile, "cdrom0:\\%s;1", IOP_IMAGE_FILE);
#else /* (defined(CDROM)) */
    sprintf(iopImageFile, "host0:%s", IOP_IMAGE_file);
#endif /* (defined(CDROM)) */
#if (!defined(DEBUGSTATION))
    /* reboot IOP, replace default modules */
    while (!sceSifRebootIop(iopImageFile))
    {
        continue;
    }

    while (!sceSifSyncIop())
    {
        continue;
    }
#endif

    /* reinitialize */
    sceSifInitRpc(0);
    sceSifInitIopHeap();
#if (defined(CDROM))
    sceCdInit(SCECdINIT);
    sceCdMmode(SCECdCD);
#endif /* (defined(CDROM)) */
    sceFsReset();
#endif /* (defined(SKYREBOOTIOP) */
    SkyState.
        skyTimerHandlerHid = AddIntcHandler(INTC_TIM0, TimerHandler, 0);
    /* Set up time0 */
    SkyState.sweHighCount = 0;
    *T0_COUNT = 0;
    *T0_COMP = 0;
    *T0_HOLD = 0;
    *T0_MODE = 0x281;
    EnableIntc(INTC_TIM0);
    /*
     * Initialize the platform independent data.
     * This will in turn initialise the platform specific data...
     */
    if (RsEventHandler(rsINITIALIZE, NULL) == rsEVENTERROR)
    {
        return -1;
    }

#if (defined(DEBUGSTATION))
#if (defined(CWDEBUGAT))
    {
        char                argv0[1024];
        char               *pStr;

        strcpy(argv0, argv[0]);
        pStr = strtok((char *) argv0, ",");
        if (pStr == NULL)
            strcpy(debugStationFilePrefix, "");
        else
            strcpy(debugStationFilePrefix, pStr);
        strcat(debugStationFilePrefix, ",");
        firstArg = 1;
    }
#else

    /*
     * Setup prefix for files when using a debug station...
     */
    strcpy(debugStationFilePrefix, "atfile:");
    /*
     * argv[1] should be the IP address of host machine
     */
    strcat(debugStationFilePrefix, argv[1]);
    strcat(debugStationFilePrefix, ",");
    /*
     * Application arguments will start at argv[2]...
     */
    firstArg = 2;
#endif
#endif /* (defined(DEBUGSTATION)) */
#if (defined(CWDEBUGAT))
    /*
     * Open printf support
     */
    {
        RwChar              outPort[1024];

        sprintf(outPort, "%s%s", debugStationFilePrefix, "mwAtLog.txt");
        cwDbgAtStdIOPort =
            sceOpen(outPort, SCE_CREAT | SCE_TRUNC | SCE_RDWR);
    }
#endif
    /*
     * Parse any command line parameters before RW startup...
     */
    for (i = firstArg; i < argc; i++)
    {
        RsEventHandler(rsPREINITCOMMANDLINE, argv[i]);
    }

    if (!SkyPadOpen())
    {
        psErrorMessage("Unable to open pad\n");
    }

    /*
     * Initialize the 3D (RenderWare) components of the app...
     */
    if (RsEventHandler(rsRWINITIALIZE, NULL) == rsEVENTERROR)
    {
        RsEventHandler(rsTERMINATE, NULL);
        return -1;
    }

    /* Force a camera resize event */
    {
        RwRect              r;

        r.x = 0;
        r.y = 0;
        r.w = RsGlobal.maximumWidth;
        r.h = RsGlobal.maximumHeight;
        RsEventHandler(rsCAMERASIZE, &r);
    }

#if (defined(RWTERMINAL))
    /* this will fail if the pipe doesn't exist, but that's okay */
    SkyKeyboardOpen();
#endif /* defined(RWTERMINAL) */
    /*
     * Parse any command line parameters...
     */
    for (i = firstArg; i < argc; i++)
    {
        RsEventHandler(rsCOMMANDLINE, argv[i]);
    }

    /*
     * Events loop...
     */
    while (!RsGlobal.quit)
    {
        static RwUInt32     lastpoll = 0;
        RwUInt32            polltime;

        polltime = RsTimer();
        if (polltime - lastpoll > 5)
        {
#if (!defined(NOPADHANDLING))
            SkyHandlePad();
#endif /* (!defined(NOPADHANDLING)) */
            SkyHandleKeyboard();
            lastpoll = polltime;
        }

        RsEventHandler(rsIDLE, NULL);
    }

    /*
     * Tidy up the 3D (RenderWare) components of the application...
     */
    RsEventHandler(rsRWTERMINATE, NULL);
    /*
     * Free the platform dependent data...
     */
    RsEventHandler(rsTERMINATE, NULL);
    DisableIntc(INTC_TIM0);
    RemoveIntcHandler(INTC_TIM0, SkyState.skyTimerHandlerHid);
#if (defined(MENUING))
    if (ExecuteElf[0] != '\0')
    {
#if (defined(RWDEBUG))
        printf("Executing: %s\n", ExecuteElf);
#endif
        LoadExecPS2(ExecuteElf, 0, NULL);
    }
#endif /* (defined(MENUING)) */

#if (defined(DEBUGSTATION))
#if (!defined(CWDEBUGAT))

    /*
     * Signal application with file request
     */
    {
        SkyString           name;

        strcpy(name, debugStationFilePrefix);
        strcat(name, "dbgstn.kill");
        sceOpen(name, SCE_RDONLY | SCE_NOWAIT);
    }
#else
    sceClose(cwDbgAtStdIOPort);
#endif /* (!defined(CWDEBUGAT)) */
    /*
     * Reset debug station to listen for any new requests...
     */
    LoadExecPS2("cdrom0:\\SLPS_666.66", 0, NULL);
#endif /* (defined(DEBUGSTATION)) */

#ifdef __MWERKS__
#ifdef CWATWIN
	    mwAtExit();
#else
#if ((defined __cplusplus) || (defined USE_RWAI))
        mwExit();
#endif /* ((defined __cplusplus) || (defined USE_RWAI)) */
#endif /* CWATWIN */
#endif /* __MWERKS__ */

    return 0;
}

/*
 *****************************************************************************
 */

RsEventStatus
PsEventHandler(RsEvent event, void *param __RWUNUSED__)
{
    RsEventStatus es = rsEVENTNOTPROCESSED;

    switch (event)
    {
    case rsPLUGINATTACH:
        RpPDSPluginAttach(RpWorldNumPipes);

        RpWorldPipesAttach();
        break;

    default:
        break;
    }

    return es;
}
