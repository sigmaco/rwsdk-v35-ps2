/* taken from
 "PlayStation 2" Programmer Tool Runtime Library Release 2.3
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
 *                            read.c
 *               callback functions for elemetary streams
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.10           12.17.1999      umemura     the first version
 */
#include <libmpeg.h>
#include <string.h>
#include "readbuf.h"
#include "videodec.h"
#include "defs.h"
#include "audiodec.h"

#if (!(defined(__cplusplus) || defined(__MWERKS__) || defined(__UNUSED__)))
#define __UNUSED__ __attribute__ ((unused))
#endif /* (!(defined(__cplusplus) || defined(__MWERKS__) || defined(__UNUSED__))) */


#if (!defined(__UNUSED__))
# define __UNUSED__ /* No op */
#endif /* (!defined(__UNUSED__)) */

extern VideoDec     videoDec;
extern AudioDec     audioDec;

static int          copy2area(u_char * pd0, int d0, u_char * pd1,
                              int d1, u_char * ps0, int s0,
                              u_char * ps1, int s1);

/* //////////////////////////////////////////////////////////////// */
/* */
/* Stream callback function for MPEG2 video stream */
/* */
int
videoCallback(sceMpeg * mp __UNUSED__, sceMpegCbDataStr * str,
              void *data)
{
    ReadBuf            *rb = (ReadBuf *) data;
    u_char             *ps0 = str->data;
    u_char             *ps1 = rb->data;
    const int           compare = rb->data + rb->size - str->data;
    int                 s0 = min(compare, (int) str->len);
    int                 s1 = str->len - s0;
    u_char             *pd0;
    u_char             *pd1;
    u_char             *pd0Unc;
    u_char             *pd1Unc;
    int                 d0, d1;
    int                 len;

    videoDecBeginPut(&videoDec, &pd0, &d0, &pd1, &d1);
    pd0Unc = (u_char *) UncAddr(pd0);
    pd1Unc = (u_char *) UncAddr(pd1);

    len = copy2area(pd0Unc, d0, pd1Unc, d1, ps0, s0, ps1, s1);

    /* set PTS */
    if (len > 0)
    {
        if (!videoDecPutTs(&videoDec, str->pts, str->dts, pd0, len))
        {
            static char         _pts_buffer_overflow[] =
                "pts buffer overflow\n";

            ErrMessage(_pts_buffer_overflow);
        }
    }

    videoDecEndPut(&videoDec, len);

    /* //////////////////////////////////////////// */
    /* */
    /* Return 0 if no data is put */
    /* */
    return (len > 0) ? 1 : 0;
}

/* //////////////////////////////////////////////////////////////// */
/* */
/* Stream callback function for PS2 PCM stream */
/* */
int
pcmCallback(sceMpeg * mp __UNUSED__, sceMpegCbDataStr * str, void *data)
{
    ReadBuf            *rb = (ReadBuf *) data;
    u_char             *ps0 = str->data;
    u_char             *ps1 = rb->data;
    int                 s0;
    int                 s1;
    u_char             *pd0;
    u_char             *pd1;
    int                 d0, d1;
    int                 len;
    int                 ret;

    /* skip */
    /* sub_stream_id */
    ps0 = str->data + 4;
    if (ps0 >= rb->data + rb->size)
    {
        ps0 -= rb->size;
    }
    len = str->len - 4;

    ps1 = rb->data;
    s0 = min(rb->data + rb->size - ps0, len);
    s1 = len - s0;

    audioDecBeginPut(&audioDec, &pd0, &d0, &pd1, &d1);
    ret = copy2area(pd0, d0, pd1, d1, ps0, s0, ps1, s1);

    audioDecEndPut(&audioDec, ret);

    /* //////////////////////////////////////////// */
    /* */
    /* Return 0 if no data is put */
    /* */
    return (ret > 0) ? 1 : 0;
}

/* //////////////////////////////////////////////////////////////// */
/* */
/* Copy two areas */
/* */
static int
copy2area(u_char * pd0, int d0, u_char * pd1, int d1,
          u_char * ps0, int s0, u_char * ps1, int s1)
{
    if (d0 + d1 < s0 + s1)
    {
        return 0;
    }

    if (s0 >= d0)
    {
        memcpy(pd0, ps0, d0);
        memcpy(pd1, ps0 + d0, s0 - d0);
        memcpy(pd1 + s0 - d0, ps1, s1);
    }
    else
    {                          /* s0 < d0 */
        if (s1 >= d0 - s0)
        {
            memcpy(pd0, ps0, s0);
            memcpy(pd0 + s0, ps1, d0 - s0);
            memcpy(pd1, ps1 + d0 - s0, s1 - (d0 - s0));
        }
        else
        {                      /* s1 < d0 - s0 */
            memcpy(pd0, ps0, s0);
            memcpy(pd0 + s0, ps1, s1);
        }
    }
    return s0 + s1;
}
