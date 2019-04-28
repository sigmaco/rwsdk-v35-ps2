/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                       - mpeg streaming -
 *
 *                         Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            main.c
 *             main function for mpeg streming program
 *
 *       Version        Date            Design   Log
 *  --------------------------------------------------------------------
 *       0.10           12.17.1999      umemura  the first version
 *       0.20           01.14.2000      umemura  stack size is modified
 *       0.30           02.29.2000      umemura  audio -> main thread
 *       0.40           03.01.2000      umemura  strFile is added
 *       1.00           08.04.2000      ywashizu using library
 *       1.01           10.26.2000      Fumikazu Kumagae  InitThreadへの対応
 */

#include <eekernel.h>
#include <stdio.h>
#include <string.h>
#include <libsdr.h>
#include <sdrcmd.h>
#include <libpkt.h>
#include <libdma.h>
#include "defs.h"
#include "sifrpc.h"
#include "sifdev.h"
#include "videodec.h"
#include "disp.h"
#include "readbuf.h"
#include "libpad.h"
#include "vobuf.h"
#include "audiodec.h"
#include "strfile.h"

// ////////////////////////////////////////////////////////////////
//
// Definitions
//
#define STACK_SIZE    (16*1024)
#define DEF_STACK_SIZE    2048
#define MAX_MBX		(MAX_WIDTH/16)
#define MAX_MBY		(MAX_HEIGHT/16)
#define IOP_BUFF_SIZE (12288*2) // 512 * 48
#define MOVIE_ABORTED SCE_PADRdown
#define MODULEDIR	"host0:/usr/local/sce/iop/modules/"
#define DEF_PRIORITY       1 //InitThreadへの対応
#define ERR_STOP while(1)

// ////////////////////////////////////////////////////////////////
//
// Variables
//
// These variables could be accessed from Uncached Area
static VoData voBufData[N_VOBUF] bss_align(64);
static VoTag voBufTag[N_VOBUF] bss_align(64);

static u_long128 viBufTag[VIBUF_SIZE + 1] bss_align(64);
// -------------- this needs to be 64 byte boudary -------------------

// These variables are NOT accessed from Uncached Area

static u_char mpegWork[SCE_MPEG_BUFFER_SIZE(MAX_WIDTH, MAX_HEIGHT)]
							bss_align(64);
static char defStack[DEF_STACK_SIZE] bss_align(64);
ReadBuf readBuf bss_align(64);
static u_char audioBuff[IOP_BUFF_SIZE*2] bss_align(64);
static u_long128 viBufData[VIBUF_SIZE * VIBUF_ELM_SIZE/16]
							bss_align(64);
static char videoDecStack[STACK_SIZE] bss_align(64);
static TimeStamp timeStamp[VIBUF_TS_SIZE];
static u_long128 controller_dma_buf[scePadDmaBufferMax]  __attribute__((aligned (64)));

int videoDecTh;
int defaultTh;
StrFile infile;
VideoDec videoDec;
AudioDec audioDec;
VoBuf voBuf;
int frd;
u_int controller_val;
int isWithAudio = 1;

char *commandname = NULL;
char *VERSION = "1.2";

sceGsDBuff scedb;

// ////////////////////////////////////////////////////////////////
//
// Function declarations
//
static void initAll(char *bsfilename);
static void termAll();
static void defMain(void* param);
static u_int movie(char *name);
static int readMpeg(VideoDec *vd, ReadBuf *rb, StrFile *file);
static int isAudioOK();
static void usage();

void dispMain(void);
void videoDecMain(void* pvd);
int videoCallback(sceMpeg *mp, sceMpegCbDataStr *str, void *data);
int pcmCallback(sceMpeg *mp, sceMpegCbDataStr *str, void *data);
int vblankHandler(int);
int handler_endimage(int);
void loadModule(char *moduleName);
void PlayMPEG(const char *fname);

// ////////////////////////////////////////////////////////////////
//
// Main function
//
void PlayMPEG(const char *name)
{
  
    ChangeThreadPriority(GetThreadId(),DEF_PRIORITY);

    sceSifInitRpc(0);
    sceSifInitIopHeap();

 
    sceGsResetPath();
    sceDmaReset(1);

    sceGsResetGraph(0, SCE_GS_INTERLACE, TV_MODE, SCE_GS_FRAME);

    //  Initialize GS memory
    clearGsMem(0x00, 0x00, 0x00, DISP_WIDTH, DISP_HEIGHT);

    sceGsSetDefDBuff(&scedb, SCE_GS_PSMCT32, DISP_WIDTH, (DISP_HEIGHT/2),
		     SCE_GS_ZNOUSE, 0, SCE_GS_CLEAR);
    FlushCache(0);

    movie((char*)name);
}


// ////////////////////////////////////////////////////////////////
//
// Decode MPEG bitstream
//
// ret:
//   1: ok
//   0: error
//  -1: abort
static u_int movie(char *name)
{
    static int count = 0;

    printf("========================== decode MPEG2 ============= %d ===========\n", count++);

    initAll(name);

    readMpeg(&videoDec, &readBuf, &infile);

    termAll();

    return controller_val;
}

// ////////////////////////////////////////////////////////////////
//
// Read MPEG data
//
// return value
//     1: normal end
//     -1: aborted
static int readMpeg(VideoDec *vd, ReadBuf *rb, StrFile *file)
{
    u_int ctrlmask =  SCE_PADRright | SCE_PADRdown | SCE_PADRleft;
    u_char *put_ptr;
    u_char *get_ptr;
    int putsize;
    int getsize;
    int readrest = file->size;
    int writerest = file->size;
    int count;
    int proceed;
    int isStarted = 0;
    u_int button_old = 0;
    u_int pushed = 0;
    u_char cdata[32];
    int isPaused = 0;

    // writerest > 4: to skip the last 4 bytes
    while (isPaused
    	|| (writerest > 4 && videoDecGetState(vd) != VD_STATE_END)) {

    	// /////////////////////////////////////////////////
	//
	// Get controller information
	//
    	if (scePadRead(0, 0, cdata) > 0) {
	    controller_val = 0xffff ^ ((cdata[2] << 8) | cdata[3]);
	} else {
	    controller_val = 0;
	}
	pushed = (button_old ^ controller_val)
			& controller_val & ctrlmask;
	button_old = controller_val;
	if (pushed && vd->mpeg.frameCount > 10) {

	    if (pushed & SCE_PADRleft) {
	    	if (isPaused) {
		    startDisplay(1);
		    if (isWithAudio) {
			audioDecResume(&audioDec);
		    }
		} else {
		    endDisplay();
		    if (isWithAudio) {
			audioDecPause(&audioDec);
		    }
		}
		isPaused ^= 1;
	    } else if (!isPaused) {

		// /////////////////////////////////////////////////
		//
		// Abort decoding
		//
		videoDecAbort(&videoDec);
	    }
	}

    	// /////////////////////////////////////////////////
	//
	// Read data to the read buffer
	//
        putsize = readBufBeginPut(rb, &put_ptr);
	if (readrest > 0 && putsize >= READ_UNIT_SIZE) {
	    count = strFileRead(file, put_ptr, READ_UNIT_SIZE);
	    readBufEndPut(rb, count);
	    readrest -= count;
	}

	switchThread();

    	// /////////////////////////////////////////////////
	//
	// De-multiplex and put data on video/audio input buffer
	//
	getsize = readBufBeginGet(rb, &get_ptr);
	if (getsize > 0) {

	    proceed = sceMpegDemuxPssRing(&vd->mpeg,
	    		get_ptr, getsize, rb->data, rb->size);

	    readBufEndGet(rb, proceed);
	    writerest -= proceed;

	}

    	// /////////////////////////////////////////////////
	//
	// Send audio data to IOP
	//
	proceedAudio();

    	// /////////////////////////////////////////////////
	//
	// Wait until video and audio output buffer become full
	//
	if (!isStarted && voBufIsFull(&voBuf) && isAudioOK()) {

	    startDisplay(1);		// start video
	    if (isWithAudio) {
		audioDecStart(&audioDec);	// start audio
	    }
	    isStarted = 1;
	}
    }

    // try to flush buffers inside decoder
    while (!videoDecFlush(vd)) {
	switchThread();
    }

    // wait till buffers are flushed
    while (!videoDecIsFlushed(vd)
    	&& videoDecGetState(vd) != VD_STATE_END) {

	switchThread();
    }

    endDisplay();
    if (isWithAudio) {
	audioDecReset(&audioDec);
    }

    return 1;
}

// /////////////////////////////////////////////////
//
// Switch to another thread
//
void switchThread()
{
    RotateThreadReadyQueue(DEF_PRIORITY);
}

// /////////////////////////////////////////////////
//
// Check audio
//
static int isAudioOK()
{
    return (isWithAudio)? audioDecIsPreset(&audioDec): 1;
}

// ////////////////////////////////////////////////////////////////
//
// Initialize all modules
//
static void initAll(char *bsfilename)
{
    struct ThreadParam th_param;

    *D_CTRL = (*D_CTRL | 0x003);
    *D_STAT = 0x4; // clear D_STAT.CIS2

    // /////////////////////////////
    // 
    //  Create read buffer
    // 
    readBufCreate(&readBuf);

    // /////////////////////////////
    // 
    //  Initialize video decoder
    // 
    sceMpegInit();
    videoDecCreate(&videoDec,
    	mpegWork, sizeof(mpegWork),
    	viBufData, viBufTag, VIBUF_SIZE, timeStamp, VIBUF_TS_SIZE);

    // /////////////////////////////
    // 
    //  Initialize audio decoder
    // 
    sceSdRemoteInit();
    sceSdRemote(1, rSdInit, SD_INIT_COLD);
    audioDecCreate(&audioDec, audioBuff, sizeof(audioBuff), IOP_BUFF_SIZE);

    ///////////////////////////////
    // 
    //  Choose stream to be played
    // 
    videoDecSetStream(&videoDec,
	    sceMpegStrM2V, 0, (sceMpegCallback)videoCallback, &readBuf);
    if (isWithAudio) {
	videoDecSetStream(&videoDec,
	    sceMpegStrPCM, 0, (sceMpegCallback)pcmCallback, &readBuf);
    }

    // /////////////////////////////
    // 
    //  Initialize video output buffer
    // 
    voBufCreate(&voBuf, UncAddr(voBufData), voBufTag, N_VOBUF);

    // /////////////////////////////
    // 
    //  Create 'default' thread
    // 
    th_param.entry = defMain;
    th_param.stack = defStack;
    th_param.stackSize = DEF_STACK_SIZE;
    th_param.initPriority = DEF_PRIORITY;
    th_param.gpReg = &_gp;
    th_param.option = 0;
    defaultTh = CreateThread(&th_param);
    StartThread(defaultTh, NULL);

    // /////////////////////////////
    // 
    //  Create docode thread
    // 
    th_param.entry = videoDecMain;
    th_param.stack = videoDecStack;
    th_param.stackSize = STACK_SIZE;
    th_param.initPriority = DEF_PRIORITY;
    th_param.gpReg = &_gp;
    th_param.option = 0;
    videoDecTh = CreateThread(&th_param);
    StartThread(videoDecTh, &videoDec);

    // /////////////////////////////
    // 
    //  Initialize controller
    // 
    {
	static int isFirst = 1;
	if (isFirst) {
	    scePadInit(0);
	    scePadPortOpen(0, 0, controller_dma_buf);
	    isFirst = 0;
	}
    }

    // /////////////////////////////
    // 
    //  Open bitstream file
    // 
    while (!strFileOpen(&infile, bsfilename)) {
        printf ("Can't Open file %s\n", bsfilename);
    }

    // /////////////////////////////
    // 
    //  Set Interrupt handlers
    // 
	sceGsSyncVCallback(vblankHandler);

    videoDec.hid_endimage = AddDmacHandler(DMAC_GIF, handler_endimage, 0);
    EnableDmac(DMAC_GIF);
}

// ////////////////////////////////////////////////////////////////
//
// Terminate all modules
//
static void termAll()
{
    readBufDelete(&readBuf);
    voBufDelete(&voBuf);

    TerminateThread(videoDecTh);
    DeleteThread(videoDecTh);

    TerminateThread(defaultTh);
    DeleteThread(defaultTh);

    DisableDmac(DMAC_GIF);
    RemoveDmacHandler(DMAC_GIF, videoDec.hid_endimage);

	sceGsSyncVCallback(NULL);

    videoDecDelete(&videoDec);
    audioDecDelete(&audioDec);

    strFileClose(&infile);
}

// ////////////////////////////////////////////////////////////////
//
// Main function of default thread
//
static void defMain(void* param)
{
	(void)(param);

    while (1) {
	switchThread();
    }
}

// ////////////////////////////////////////////////////////////////
//
// Print error message
//
void ErrMessage(char *message)
{
    printf("[ Error ] %s\n", message);
}

// ////////////////////////////////////////////////////////////////
//
// Usage
//
static void usage()
{
    printf(
	"usage: run %s [options] pss_file         (version %s)\n"
	"    < options >\n"
	"        -noaudio               decode video only\n"
	"        -help\n"
	"\n"
	"    < pss_file >\n"
	"\n"
	"        = from HD =\n"
	"        dir/file.pss           -> host0:dir/file.pss\n"
	"        host0:dir/file.pss     -> host0:dir/file.pss\n"
	"\n"
	"        = from CD/DVD =\n"
	"        cdrom0:\\dir\\file.pss;1 -> cdrom0:\\DIR\\FILE.PSS;1\n"
	"        cdrom0:/dir/file.pss;1 -> cdrom0:\\DIR\\FILE.PSS;1\n"
	"        cdrom0:/dir/file.pss   -> cdrom0:\\DIR\\FILE.PSS;1\n"
	"\n"
	"    < operations >\n"
	"	circle:  repeat\n"
	"	cross:   quit\n"
	"	square:  pause\n"
	, commandname, VERSION
    );
}

// ////////////////////////////////////////////////////////////////
//
//  Send audio data to IOP
//
void proceedAudio()
{
    audioDecSendToIOP(&audioDec);
}

