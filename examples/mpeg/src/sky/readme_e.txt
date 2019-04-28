[SCEI CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.1
                  Copyright (C) 2000 by Sony Computer Entertainment Inc.
                                                     All Rights Reserved

Sample Program to play back MPEG2 Stream with sound for PlayStation (PSS)


<Description of the Sample>
---------------------------

     == Overview ==

	mpegstr is a sample program for playing back the MPEG2 Stream (PSS) 
	with the sound for "PlayStation".  The mpegstr plays back the 
	image and sound satisfying the following requirements.

	- Image -

	    Main Profile at Main Level(MP@ML)
	    Simple Profile at Main Level(SP@ML)
	    Maximum Size             : 720x576
	    Picture Structure        : Frame structure
				       (progressive frame is preferable)
	    
	- Sound -
	    
	    Syntax                 : Straight PCM
	    Sample Length          : 16bits
	    Endian                 : Little endian
	    Sampling Rate          : 48.0KHz
	    Number of Channels     : 2
	    (L/R) Interleave Size  : 512

	mpegstr plays back the PSS file in the hard disk drive of the host
	or the CD/DVD drive of the DTL-T10000.  The device using for 
	playback can be specified by the device name (host0:/cdrom0:) at 
	the top of the file name.

	The stream converter ps2str is used for multiplexing the image
	and sound.  The stream number of both the image and sound must be
	0 in this program.  Specify 0 to the stream number for multiplexing.

     == GS Memory ==

    In this program, the GS memory is initialized first then the normal double
    buffer setting is made.

    (a) clearGsMem function
	Firstly, the GS memory is initialized.
	The entire memory is set as a large drawing area.
	This is implemented by drawing a sheet of sprite.


		 -----------------------
		|			|
		|			|
		|			|
		|			|
		|			|
		|			|
		|			|
		|			|
		|	Frame		|
		|	 Buffer		|
		|			|
		|			|
		|			|
		|			|
		|			|
		|			|
		|			|
		|			|
		|			|
		 -----------------------
    (b) sceGsSetDefDBuff function
	Subsequently, the GS memory is specified as frame buffers
	and texture buffers.  The z-buffer is not used in this sample.

		 -----------------------
		|			|
		|	Frame		|
		|	 Buffer0	|
		|			|
		 -----------------------
		|			|
		|	Frame		|
		|	 Buffer1	|
		|	 		|
		 -----------------------
		|			|
		|	Texture		|
		|	 Buffer		|
		|			|
		|			|
		 -----------------------
		|			|
		|			|
		|			|
		 -----------------------


     == Thread Structure ==

	This program has a three-thread structure.

	(a) main thread 
	    main thread executes main().  The main thread carries out 
	    initialization and creates/activates other threads, then 
	    + loads data from a medium into the readBuf
	    + demultiplexes the data in the readBuf, and transfers image data 
	      to the viBuf of the videoDec and sound data to the buffer in 
	      the audioDec
	    + DMA-transfers image data to the IPU
	    + transfers sound data to the IOP.

	(b) video decode thread 
	    video decode thread decodes image data using the IPU.

	    + Decodes image data using the IPU.
	    + Stores the decoding results in the voBuf.

	(c) default thread 
	    default thread is used for debugging purposes.  This thread 
	    does nothing under normal conditions, though the threads including 
	    the default thread run their processes by turns.

    == Interrupt ==

    This program includes two user-interrupts as follows.

    (a) vblankHandler
	This function is called when vblank starts.  The following 
	are processed in this function.

	- With the sceGsSetHalfOffset function, the half a pixel deviation,
	  that is generated in between the odd field and the even field,
	  is adjusted.

	- Making a double buffer switch with the sceGsSwapDBuff function. 

	- With the sceDmaSend function, the picture data and the data 
	  for drawing is transferred to the GS.  

	The GS transfers the picture data only once while vblank occurs twice,
	the identical picture data is referred as the texture in the odd field
	and the even field.  Since the double buffer is replaced every vblank
	occurrence, the operation speed is set as 60 frames/s.

    (b) hadler_endimage
	When the DMA transfer via path 3 finishes, an interrupt occurs
	then this function is called.  The DMA via path 3 is used in the
	main loop for transferring the picture data and the sceGsSwapDBuff
	function.  When the DMA completes with the sceGsSwapDBuff function,
	the hadler_endimage function exits with no operation.  On completion 
	of the picture data transfer, the flag is set on and the function
	processes the following to control the number of picture data in 
	VoBuf.

	- Decrement the picture data counter in VoBuf by 1.
	- Set the flag on to indicate the frame drawing termination.

    == Flow of the Program ==

    The program flow is not simple since three threads and two interrupt
    handlers are used in this program, however, it is briefly explained 
    as follows.  See also the "Data Flow and Buffer Structure" item.
    

    (a) Initializes SIF.  Loads the required modules on the IOP.

    (b) After initializing the GS, sets the frame buffer and the 
	texture buffer.

    (c) Reserves the readBuf buffer to store the data (sound and picture)
	that is retrieved from the disk.

    (d) Creates the decoder for the sound and the picture (videoDec, audioDec).

    (e) Reserves the VoBuf buffer to store the picture data that is decoded
	by the IPU.
	
    (f) Creates two threads that are default and video decode (for details, 
	refer to the above).

    (g) Opens the file to perform streaming.
	
    (h) Registers two interrupt handlers that are vblankHandler and
	handler_endimage. (for details, refer to the above).

    (i)	Transfers data from the disk to the readBuf buffer.

    (j) Demultiplexes the data into the sound and the picture using the 
	sceMpegDemuxPssRing function.

    (k) Sends back the sound data to the IOP.

    (l) Decodes the picture data with the sceMpegGetPicture function.

    (m) Creates the packet for sending the decoded picture and the 
	drawing	data to the GS.

    (n) Sets the flag on to make the vblankHandler function is enabled.

    (o) Starts decoding the sound data.

    (p) Makes a switch between the drawing buffer and the display buffer every
	vblank occurrence then transfers packets to the GS.

    (q) Repeats (i) through (p) until all the data is processed.


     == Data Flow and Buffer Structure ==

	The arrows in the following figure indicate flows of data which is 
	loaded from the CD/DVD and played back.


            audioDec  readBuf   viBuf           voBuf
            +---+     +---+     +---+           +---+
            | A |     | V |---->| V |  Video    |   |
            +---+     +---+     +---+  decode   | V |
            | A |<----| A |     | V |--> IPU -->|   |--> GS
            +---+     +---+     +---+           +---+
              |       | V |     | V |           |   |
              |       +---+     +---+           | V |
              |       | A |     |-------------| |   |
              |       +---+       videoDec      +---+
              |       | V |                    
              |       +---+                                        EE 
              |         ^
            --|---------|-----------------------------------------------
              V         |
            +---+       |                                          IOP
     SPU2<--| A |     CD/DVD/HD
            +---+
            | A |
            +---+

        (a) Data loaded from the CD/DVD/HD is held in the EE buffer, 
	    readBuf, first.
        (b) The data in the readBuf is demultiplexed and split into sound 
	    and image.
        (c) Image data is transferred to the video input buffer, viBuf, in 
	    the videoDec module.
        (d) The data in the viBuf is decoded by the IPU, and the decoded 
	    image is held in the video output buffer, voBuf.
        (e) The image data in the voBuf is transferred to the GS 
	    synchronizing with the vblank interrupt, then displayed on 
	    the monitor screen.
        (f) Sound data demultiplexed in (b) is stored in the buffer of 
	    the audioDec module.
        (g) The sound data in the audioDec is transferred to the buffer in 
	    the IOP memory (SMEM).
        (h) Sound in SMEM is DMA-transferred to the SPU2 local memory, and 
	    played back.


 
     == Other ==
      
 	This program does not check the time stamp of the image and sound.  
	Therefore, a recovery operation when they do not synchronize is not 
	incorporated.  This program aligns the start of the image and sound 
	data, then starts to play back.  The vblank determines the playback 
	speed of the image, and the playback speed of the SPU2 determines 
	that of the sound.  There may be slight difference in the speed, but 
	it does not affect the normal playback.

	When pause or resume is executed, a maximum of 512 samples for
	sound (0.0107 sec at 48KHz) may be dropped out.

	In a case where the buffer underflow occurs in the middle as 
	reading or decoding cannot keep up, the decoded data up to that 
	point is played back repeatedly. It means the last decoded picture
	is displayed as an image.  As for the sound, the data left in the 
	buffer is repeatedly played back.  At this time, the sound and 
	image may not synchronize.  As long as the buffer is full,
	no more	data is read in; therefore, the buffer will not overflow.

	The function sceMpegGetPicture() uses SPR (Scratch Pad RAM) 
	internally.  Therefore, if using SPR in the other function, 
	the SPR must not be used while the sceMpegGetPicture() is
	performed.

< File >
	audiodec.c
	audiodec.h
	defs.h
	disp.c
	disp.h
	main.c
	read.c
	readbuf.c
	readbuf.h
	strfile.c
	strfile.h
	util.c
	vibuf.c
	vibuf.h
	videodec.c
	videodec.h
	vobuf.c
	vobuf.h

< Activating the Program >
	% make		: Compiles the program
	% make run	: Executes the program (with sound)
	% make noaudio  : Executes the program (without sound)
	% make help     : Displays the help information 
			  (File name specification procedure can be found)

	If the PSS file exists in the hard disk drive of the host, the 
	following steps can be taken.

	% dsreset 0 0
	% dsedb
	dsedb S> run main.elf 'host0:XXX.pss'

	If the PSS file exists in the CD/DVD drive of the DTL-T10000, the 
	following steps can be taken.

	% dsreset 0 0
	% dsedb
	dsedb S> run main.elf 'cdrom0:\\XXX.PSS;1'

< Using the Controller >

	Circle button: Plays back from the beginning 
	Eks button : Terminates playback
	Square button: pause/resume

< Notes >

	(a) When the streaming from the host is processed using the 
	    DTL-T10000, the playback may become unstable due to the 
	    network load.
	    When processing the streaming via network, note the following:
		Connects with 100Base-T
		Avoid connection via hub etc.

	(b) The sample data image in /usr/local/sce/data/movie/mpeg/sample.pss 
	    is partly unstable.
