/*
 *   This file is part of MiniGUI, a mature cross-platform windowing 
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 * 
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */
/*
** helio.c: Low Level Input Engine for Helio Touch Panel
** 
** Created by Wei YongMing, 2001/01/17
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "common.h"

#ifdef _HELIO_IAL

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/poll.h>

#include "ial.h"
#include "helio.h"

static unsigned char state [NR_KEYS];

static int tp_fd = -1;
static int btn_fd;
static char btn_state=0;
static int tp_px=0, tp_py=0, tp_pz=0, tp_pb=0;
static int enable_coor_transf = 1;

#define TPH_DEV_FILE "/dev/tpanel"
#define BTN_DEV_FILE "/dev/buttons"

/* define if have helio buttons */
#define _HELIO_BUTTONS  1

#define _TP_HELIO       1
#undef  _TP_TSBREF

#define TRANS_PER_PIXEL 4
#define BUTTON_L        4
#define DATACHANGELIMIT 50

typedef struct
{
    /*
     * Coefficients for the transformation formulas:
     *
     *     m = (ax + by + c) / s
     *     n = (dx + ey + f) / s
     *
     * These formulas will transform a device point (x, y) to a
     * screen point (m, n) in fractional pixels.  The fraction
     * is 1 / TRANS_PER_PIXEL.
     */

    int a, b, c, d, e, f, s;
} TRANSF_COEF;    

static TRANSF_COEF tc;

static int GetPointerCalibrationData(void)
{
    /*
     * Read the calibration data from the calibration file.
     * Calibration file format is seven coefficients separated by spaces.
     */

    /* Get pointer calibration data from this file */
    const char cal_filename[] = "/etc/pointercal";

    int items;

    FILE* f = fopen(cal_filename, "r");
    if ( f == NULL ) {
        /* now if we can't open /etc/pointercal, we should launch an application
         * to calibrate touch panel. so we should disable enable_coor_transf
         * so that the application could get raw data from touch panel 
         */
        enable_coor_transf = 0;
        return 0;
    }

    items = fscanf(f, "%d %d %d %d %d %d %d",
        &tc.a, &tc.b, &tc.c, &tc.d, &tc.e, &tc.f, &tc.s);
    if ( items != 7 ) {
    	fprintf(stderr, "Improperly formatted pointer calibration file %s.\n", cal_filename);
        return -1;
    }

    return 0;
}

static POINT DeviceToScreen(POINT p)
{
    /*
     * Transform device coordinates to screen coordinates.
     * Take a point p in device coordinates and return the corresponding
     * point in screen coodinates.
     * This can scale, translate, rotate and/or skew, based on the
     * coefficients calculated above based on the list of screen
     * vs. device coordinates.
     */

    static POINT prev;
    /* set slop at 3/4 pixel */
    const short slop = TRANS_PER_PIXEL * 3 / 4;
    POINT new, out;

    /* transform */
    new.x = (tc.a * p.x + tc.b * p.y + tc.c) / tc.s;
    new.y = (tc.d * p.x + tc.e * p.y + tc.f) / tc.s;

    /* hysteresis (thanks to John Siau) */
    if ( abs(new.x - prev.x) >= slop )
        out.x = (new.x | 0x3) ^ 0x3;
    else
        out.x = prev.x;

    if ( abs(new.y - prev.y) >= slop )
        out.y = (new.y | 0x3) ^ 0x3;
    else
        out.y = prev.y;

    prev = out;

    return out;
}

static int PD_Open(void)
{
     /*
     * open up the touch-panel device.
     * Return the fd if successful, or negative if unsuccessful.
     */
#ifndef _TP_HELIO
    struct scanparam s;
    int settle_upper_limit;
    int result;
#endif

    tp_fd = open(TPH_DEV_FILE, O_RDONLY);
    if (tp_fd < 0) {
    	fprintf(stderr, "Error %d opening touch panel\n", errno);
        return -1;
    }
#ifdef _TP_TSBREF
    return tp_fd;
#endif

#ifndef _TP_HELIO
    /* set interval to 5000us (200Hz) */
    s.interval = 1000;
    /*
     * Upper limit on settle time is approximately (scan_interval / 5) - 60
     * (5 conversions and some fixed overhead)
     * The opmtimal value is the lowest that doesn't cause significant
     * distortion.
     * 50% of upper limit works well on my Clio.  25% gets into distortion.
     */
    settle_upper_limit = (s.interval / 5) - 60;
    s.settletime = settle_upper_limit * 50 / 100;
    result = ioctl(tp_fd, TPSETSCANPARM, &s);
    if ( result < 0 )
    	fprintf(stderr, "Error %d, result %d setting scan parameters.\n", result, errno);
#endif

    if (enable_coor_transf) { 
        if (GetPointerCalibrationData() < 0) {
            close (tp_fd);
            return -1;
        }
    }

    return tp_fd;
}

static void PD_Close(void)
{
     /* Close the touch panel device. */
    if (tp_fd > 0)
        close(tp_fd);
    tp_fd = -1;
}

static int PD_Read(int *px, int *py, int *pz, int *pb)
{
    /*
     * Read the tpanel state and position.
     * Returns the position data in x, y, and button data in b.
     * Returns -1 on error.
     * Returns 0 if no new data is available.
     * Returns 1 if position data is relative (i.e. mice).
     * Returns 2 if position data is absolute (i.e. touch panels).
     * Returns 3 if position data is not available, but button data is.
     * This routine does not block.
     *
     * Unlike a mouse, this driver returns absolute postions, not deltas.
     */

    /* If z is below this value, ignore the data. */
    /* const int __mg_low_z_limit = 900; EVEREX*/
#ifndef _TP_HELIO
    const int __mg_low_z_limit = 815;
#endif
    /*
     * I do some error masking by tossing out really wild data points.
     * Lower data_change_limit value means pointer get's "left behind"
     * more easily.  Higher value means less errors caught.
     * The right setting of this value is just slightly higher than
     * the number of units traversed per sample during a "quick" stroke.
     */
#ifndef _TP_HELIO
    const int data_change_limit = DATACHANGELIMIT;
#endif
    static int have_last_data = 0;
    static int last_data_x = 0;
    static int last_data_y = 0;

    /*
     * Thanks to John Siau <jsiau@benchmarkmedia.com> for help with the
     * noise filter.  I use an infinite impulse response low-pass filter
     * on the data to filter out high-frequency noise.  Results look
     * better than a finite impulse response filter.
     * If I understand it right, the nice thing is that the noise now
     * acts as a dither signal that effectively increases the resolution
     * of the a/d converter by a few bits and drops the noise level by
     * about 10db.
     * Please don't quote me on those db numbers however. :-)
     * The end result is that the pointer goes from very fuzzy to much
     * more steady.  Hysteresis really calms it down in the end (elsewhere).
     *
     * iir_shift_bits effectively sets the number of samples used by
     * the filter * (number of samples is 2^iir_shift_bits).
     * Lower iir_width means less pointer lag, higher iir_width means
     * steadier pointer.
     */
#ifndef _TP_HELIO
    const int iir_shift_bits = 3;
    const int iir_sample_depth = (1 << iir_shift_bits);
#endif
    static int iir_accum_x = 0;
    static int iir_accum_y = 0;
    static int iir_accum_z = 0;
    static int iir_count = 0;
    int data_x, data_y, data_z;

    /* read a data point */
#ifdef _TP_HELIO
    short data[3];
#else
    short data[6];
#endif
    int bytes_read;
    bytes_read = read(tp_fd, data, sizeof(data));
    if (bytes_read != sizeof(data)) {
        if (errno == EINTR || errno == EAGAIN) {
            return 0;
        }
        return -1;
    }
#ifdef _TP_TSBREF
    if(data[0]&0x8000) {
        *px = data[2] - data[1];
        *py = data[4] - data[3];
        *pz = 0;
        *pb = BUTTON_L;
        return 2;
    }
    else {
        *pb = 0;
        return 3;
    }
#endif

#ifndef _TP_HELIO
    /* did we lose any data? */
    if ( (data[0] & 0x2000) )
        fprintf(stderr, "Lost touch panel data\n");

    /* do we only have contact state data (no position data)? */
    if ( (data[0] & 0x8000) == 0 ) {
        /* is it a pen-release? */
        if ( (data[0] & 0x4000) == 0 ) {
            /* reset the limiter */
            have_last_data = 0;

            /* reset the filter */
            iir_count = 0;
            iir_accum_x = 0;
            iir_accum_y = 0;
            iir_accum_z = 0;

            /* return the pen (button) state only, */
            /* indicating that the pen is up (no buttons are down)*/
            *pb = 0;
            return 3;
        }

        /* ignore pen-up since we don't know where it is */
        return 0;
    }
#endif
    /* we have position data */
#ifdef _TP_HELIO
    data_x = data[1];
    data_y = data[2];
    data_z = data[0] ? 2000 : 0;
#else
    /*
     * Combine the complementary panel readings into one value (except z)
     * This effectively doubles the sampling freqency, reducing noise
     * by approx 3db.
     * Again, please don't quote the 3db figure.  I think it also
     * cancels out changes in the overall resistance of the panel
     * such as may be caused by changes in panel temperature.
     */
    data_x = data[2] - data[1];
    data_y = data[4] - data[3];
    data_z = data[5];

    /* isn't z big enough for valid position data? */
    if ( data_z <= __mg_low_z_limit ) {
        return 0;
    }

    /* has the position changed more than we will allow? */
    if ( have_last_data ) {
        if ( (abs(data_x - last_data_x) > data_change_limit)
            || ( abs(data_y - last_data_y) > data_change_limit ) ) {
            return 0;
        }
    }
#endif

    /* save last position */
    last_data_x = data_x;
    last_data_y = data_y;
    have_last_data = 1;

#ifdef _TP_HELIO
    if (enable_coor_transf) {
        POINT transformed = {data_x, data_y};
        transformed = DeviceToScreen(transformed);

        *px = transformed.x >> 2;
        *py = transformed.y >> 2;
    } else {
        *px = data_x;
        *py = data_y;
    }
    *pb = data[0] ? BUTTON_L : 0;
    return 2;
#else
    /* is filter ready? */
    if ( iir_count == iir_sample_depth )
    {
        /* make room for new sample */
        iir_accum_x -= iir_accum_x >> iir_shift_bits;
        iir_accum_y -= iir_accum_y >> iir_shift_bits;
        iir_accum_z -= iir_accum_z >> iir_shift_bits;

        /* feed new sample to filter */
        iir_accum_x += data_x;
        iir_accum_y += data_y;
        iir_accum_z += data_z;

        /* transformation enabled? */
        if (enable_coor_transf) {
            /* transform x,y to screen coords */
            POINT transformed = {iir_accum_x, iir_accum_y};
            transformed = DeviceToScreen(transformed);
            /*
             * HACK: move this from quarter pixels to whole
             * pixels for now at least until I decide on the
             * right interface to get the quarter-pixel data
             * up to the next layer.
             */
            *px = transformed.x >> 2;
            *py = transformed.y >> 2;
        }
        else {
            /* return untransformed coords (for calibration) */
            *px = iir_accum_x;
            *py = iir_accum_y;
        }
        *pb = BUTTON_L;
        /* return filtered pressure */
        *pz = iir_accum_z;
        return 2;
    }
#endif

    /* prime the filter */
    iir_accum_x += data_x;
    iir_accum_y += data_y;
    iir_accum_z += data_z;
    iir_count += 1;

    return 0;
}

/************************  Low Level Input Operations **********************/

/*
 * Mouse operations -- Event
 */
static int mouse_update (void)
{
    return 1;
}

static void mouse_getxy (int* x, int* y)
{
    *x = tp_px;
    *y = tp_py;
}

static int mouse_getbutton(void)
{
    return tp_pb;
}

static int keyboard_update(void)
{
    switch (btn_state)
    {
    case (char)0x55: //left up-down
        state[SCANCODE_CURSORBLOCKUP] = 1;
    break;
    case (char)0xd5: //left up-up
        state[SCANCODE_CURSORBLOCKUP] = 0;
    break;
    case (char)0x44: //left down-down
        state[SCANCODE_CURSORBLOCKDOWN] = 1;
    break;
    case (char)0xc4: //left down-up
        state[SCANCODE_CURSORBLOCKDOWN] = 0;
    break;
    case (char)0x31: //down 2-down
        state[SCANCODE_LEFTALT] = 1;
    break;
    case (char)0xb1: //down 2-up
        state[SCANCODE_LEFTALT] = 0;
    break;
    case (char)0x32: //down 3-down
        state[SCANCODE_RIGHTALT] = 1;
    break;
    case (char)0xb2: //down 3-up
        state[SCANCODE_RIGHTALT] = 0;
    break;
    case (char)0x33: //down4-down
        state[SCANCODE_RIGHTCONTROL] = 1;
    break;
    case (char)0xb3: //down4-up
        state[SCANCODE_RIGHTCONTROL] = 0;
    break;
    }

    return NR_KEYS;
}

static const char* keyboard_getstate(void)
{
    return (char *)state;
}

#ifdef _LITE_VERSION
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#endif
{
    fd_set rfds;
    int e;
    int retvalue = 0;
    int result;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if (which & IAL_MOUSEEVENT) {
        FD_SET (tp_fd, in);
#ifdef _LITE_VERSION
        if (tp_fd > maxfd) maxfd = tp_fd;
#endif
    }
#ifdef _HELIO_BUTTONS
    if (which & IAL_KEYEVENT){
        FD_SET (btn_fd, in);
#ifdef _LITE_VERSION
        if(btn_fd > maxfd) maxfd = btn_fd;
#endif
    }
#endif

#ifdef _LITE_VERSION
    e = select (maxfd + 1, in, out, except, timeout) ;
#else
    e = select (FD_SETSIZE, in, out, except, timeout) ;
#endif

    if (e > 0) { 
        if (tp_fd >= 0 && FD_ISSET (tp_fd, in))
        {
            FD_CLR (tp_fd, in);
            result = PD_Read(&tp_px, &tp_py, &tp_pz, &tp_pb);
            if (result > 0) {
            	retvalue |= IAL_MOUSEEVENT;
            }
        }
#ifdef _HELIO_BUTTONS
        if (btn_fd >= 0 && FD_ISSET(btn_fd, in))
        {
            char data;
            FD_CLR(btn_fd, in);
            result = read(btn_fd, &data, sizeof(data));
            btn_state = data;
            retvalue |= IAL_KEYEVENT;
        }
#endif
    } else if (e < 0) {
        return -1;
    }

    return retvalue;
}

BOOL InitHelioInput (INPUT* input, const char* mdev, const char* mtype)
{

    if (PD_Open() < 0 ) {
        fprintf (stderr, "IAL Helio Engine: Can not open touch panel!\n");
        return FALSE;
    }
#ifdef _HELIO_BUTTONS
    btn_fd = open (BTN_DEV_FILE, O_RDONLY);
    if (btn_fd < 0) {
        return FALSE;
    }
#endif
    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->set_leds = NULL;

    input->wait_event = wait_event;
    return TRUE;
}

void TermHelioInput (void)
{
    PD_Close();
}

#endif /* _HELIO_IAL */

