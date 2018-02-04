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
** vr4181.c: Low Level Input Engine for NEC VR4181 debug board.
** 
** Author: Luo Gang (Red Flag Software).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _VR4181_IAL

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/kd.h>

#include "ial.h"
#include "vr4181.h"

/* define _VR_BUTTONS to include button handling */
#undef _VR_BUTTONS

#define TP_DEV_FILE  "/dev/tpanel"
#define BTN_DEV_FILE "/dev/vrbuttons"

#define WIDTH       320
#define HEIGHT      240

typedef struct {
    short b;
    short x;
    short y;
    short pad;
} POS;

typedef struct {
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
    int s;
} TRANSF_COEFF;

typedef struct
{
        int x, y;
} XYPOINT;

struct scanparam {
        unsigned int        interval;
        unsigned int        settletime;
};

#define TPGETSCANPARM _IOR( 0xB0, 0x00, struct scanparam )
#define TPSETSCANPARM _IOW( 0xB0, 0x01, struct scanparam )

static int ts = -1;
static int mousex = 0;
static int mousey = 0;
static int mousez = 0;
static unsigned short btn_state = 0;
static unsigned char state[NR_KEYS];

#ifdef _VR_BUTTONS
static int btn_fd = -1;
#endif

/*
 * Set default scan interval in microseconds (50 Hz).
 * If you adjust this value, you may also want to adust iir_shift_bits
 * in VrTpanelRead.
 */
int __mg_scan_interval = 20000;

/*
 * Set default settle time in microseconds.
 * Functional upper limit on settle time is approximately
 * (__mg_scan_interval / 5) - 60 (5 conversions and some fixed overhead
 * fitting within scan interval).  The opmtimal value is the lowest
 * that doesn't cause significant distortion.  The lower the value,
 * the more "simultaneous" and thus coherent the conversions.
 * 480us is probably a reasonable value for most hardware.  Lower
 * than that can get into distortion.
 */
int __mg_scan_settle_time = 480;

/*
 * Set default low z limit.  If z measurement data is below this value,
 * ignore the data.
 */
int __mg_low_z_limit = 800;

/*
 * Enable absolute coordinate transformation.
 * Normally this should be left at 1.
 * To disable transformation, set it to 0 before calling VrPanelInit.
 * This is done by the pointer calibration utility since it's used to produce the pointer
 * calibration data file.
 */
int __mg_vr_enable_transform = 1;


#if TEST
static int show_raw = 0;
static int show_filtered = 0;
static int show_enqueue = 0;
#endif

static TRANSF_COEFF tc;

static int GetPointerCalibrationData(void)
{
        /*
         * Read the calibration data from the calibration file.
         * Calibration file format are these values separated by spaces:
         * - Seven tranformation coefficients a, b, c, d, e, f, and g
         * - Scan interval microseconds
         * - Scan settle time in microseconds
         * - Low-z limit
         */

        /* Get pointer calibration data from this file */
        const char cal_filename[] = "/etc/pointercal";

        int items;

        FILE* cf = fopen (cal_filename, "r");
        if ( cf == NULL ) {
                fprintf(stderr, "Error %d opening pointer calibration file %s.\n",
                        errno, cal_filename);
                return -1;
        }

        items = fscanf(cf, "%d %d %d %d %d %d %d %d %d %d",
                &tc.a, &tc.b, &tc.c, &tc.d, &tc.e, &tc.f, &tc.s,
                &__mg_scan_interval, &__mg_scan_settle_time, &__mg_low_z_limit);
        if (items < 7) {
                fprintf(stderr, "Improperly formatted pointer calibration file %s.\n",
                        cal_filename);
                fclose(cf);
                return -1;
        }

        fclose(cf);
        return 0;
}

#ifndef TRANSFORMATION_UNITS_PER_PIXEL
#define TRANSFORMATION_UNITS_PER_PIXEL        4
#endif

XYPOINT DeviceToScreen(XYPOINT p)
{
        /*
         * Transform device coordinates to screen coordinates.
         * Take a point p in device coordinates and return the corresponding
         * point in fractional screen coodinates (fraction is
         * 1 / TRANSFORMATION_UNITS_PER_PIXEL).
         *
         * It can scale, translate, rotate and/or skew, based on the coefficients
         * calculated above based on the list of screen vs. device coordinates.
         */

        static XYPOINT prev;
        /* Slop is how far the pointer has to move before we will allow the output
         * to change.  Optimal is probably 3/4 of a pixel, but that might not be
         * enough for some panels that are very noisey.
         * Using larger values appears to makes the pointer movement more "blocky".
         * This is noticeable, say,  when drawing a line at a 45 degree angle.
         */
        /* TODO: make this configurable */
        const short slop = TRANSFORMATION_UNITS_PER_PIXEL * 3 / 4;
        XYPOINT new, out;

        /* Transform */
        new.x = (tc.a * p.x + tc.b * p.y + tc.c) / tc.s;
        new.y = (tc.d * p.x + tc.e * p.y + tc.f) / tc.s;

        /* Hysteresis (thanks to John Siau) */
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


static int VrTpanelInit(void)
{
     /*
     * Open up the touch-panel device.
     * Return the fd if successful, or negative if unsuccessful.
     */

    struct scanparam s;
    int result;
    int fd;

    /* Read the calibration file first for scan interval and settling time. */
    GetPointerCalibrationData();

    /* Open the touch-panel device. */
    fd = open (TP_DEV_FILE, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error %d opening touch panel\n", errno);
        return -1;
    }

    s.interval = __mg_scan_interval;
    s.settletime = __mg_scan_settle_time;
    result = ioctl (fd, TPSETSCANPARM, &s);
    if ( result < 0 )
        fprintf(stderr, "Error %d, result %d setting scan parameters.\n", result, errno);

    return fd;
}

/* mouse button bits in BUTTON */
#define  LBUTTON        04
#define  MBUTTON        02
#define  RBUTTON        01

static int VrTpanelRead(int fd, int *px, int *py, int *pz, unsigned int *pb)
{
        /*
         * This driver returns absolute postions, not deltas.
         * Returns the position in px, py, pressure in pz, and buttons pb.
         * Returns -1 on error.
         * Returns 0 if no new data is available.
         * Does not return 1 (which is for relative position data).
         * Returns 2 if position data is absolute (i.e. touch panels).
         * Returns 3 if position data is not available, but button data is.
         * This routine does not block.
         */

        /*
         * I do some error masking by tossing out really wild data points.
         * Lower data_change_limit value means pointer get's "left behind" more easily.
         * Higher value means less errors caught.
         * The right setting of this value is just slightly higher than the number of
         * units traversed per sample during a "quick" stroke.
         * I figure a fast pen scribble can cover about 3000 pixels in one second on a typical screen.
         * There are typically about 3 to 6 points of touch-panel resolution per pixel.
         * So 3000 pixels-per-second * 3 to 6 tp-points-per-pixel / 50 samples-per-second =
         * 180 to 360 tp points per scan.
         */
        /* TODO: make this configurable */
        const int data_change_limit = 360;
        static int have_last_data = 0;
        static int last_data_x = 0;
        static int last_data_y = 0;

        /*
         * Thanks to John Siau <jsiau@benchmarkmedia.com> for help with the noise filter.
         * I use an infinite impulse response low-pass filter on the data to filter out
         * high-frequency noise.  Results look better than a finite impulse response filter.
         * If I understand it right, the nice thing is that the noise now acts as a dither
         * signal that effectively increases the resolution of the a/d converter by a few bits
         * and drops the noise level by about 10db.
         * Please don't quote me on those db numbers however. :-)
         * The end result is that the pointer goes from very fuzzy to much more steady.
         * Hysteresis really calms it down in the end (elsewhere).
         *
         * iir_shift_bits effectively sets the number of samples used by the filter
         * (number of samples is 2^iir_shift_bits).
         *
         * Setting iir_shift_bits lower allows shorter "taps" and less pointer lag, but a
         * fuzzier pointer, which can be especially bad for display update when dragging.
         *
         * Setting iir_shift_bits higher requires longer "presses" and causes more pointer lag,
         * but it provides steadier pointer.
         *
         * If you adjust iir_shift_bits, you may also want to adjust the sample interval
         * in VrTpanelInit. 
         *
         * The filter gain is fixed at 8 times the input (gives room for increased resolution
         * potentially added by the noise-dithering).
         *
         * The filter won't start outputing data until iir_count is above iir_output_threshold.
         */
        const int iir_shift_bits = 2;
        const int iir_sample_depth = (1 << iir_shift_bits);
        const int iir_output_threshold = 1;
        const int iir_gain = 8;
        static int iir_accum_x = 0;
        static int iir_accum_y = 0;
        static int iir_accum_z = 0;
        static int iir_count = 0;
        int iir_out_x;
        int iir_out_y;
        int iir_out_z;

        int data_x, data_y, data_z;

        /* read a data point */
        short data[6];
        int bytes_read;

again:
        bytes_read = read (fd, data, sizeof(data));
        if (bytes_read != sizeof (data)) {
                if (errno == EINTR)
                        goto again;
                else
                        return -1;
        }

        /* did we lose any data? */
        if ( (data[0] & 0x2000) )
                fprintf(stderr, "Lost touch panel data\n");

#if TEST
        if ( show_raw )
                printf("Raw %.4hx %s %s  x %5hd %5hd %5hd  y %5hd %5hd %5hd  z %5hd\n",
                        data[0],
                        data[0] & 0x8000 ? "data   " : "no data",
                        data[0] & 0x4000 ? "down" : "up  ",
                        data[1], data[2], data[2] - data[1],
                        data[3], data[4], data[4] - data[3],
                        data[5]);
#endif

        /* do we only have contact state data (no position data)? */
        if ( (data[0] & 0x8000) == 0 ) {
                /* is it a pen-release? */
                if ( (data[0] & 0x4000) == 0 ) {
                        /* TODO: in order to provide maximum responsiveness
                         * maybe return the data in the filter
                         * if the filter count is still below
                         * the output threshold.
                         */

                        /* reset the limiter */
                        have_last_data = 0;

                        /* reset the filter */
                        iir_count = 0;
                        iir_accum_x = 0;
                        iir_accum_y = 0;
                        iir_accum_z = 0;

                        /* return the pen (button) state only, */
                        /* indicating that the pen is up (no buttons are down) */
                        *pb = 0;
                        return 3;
                }

                /* ignore pen-down since we don't know where it is */
                return 0;
        }

        /* we have position data */

        /*
         * Combine the complementary panel readings into one value (except z)
         * This effectively doubles the sampling freqency, reducing noise by approx 3db.
         * Again, please don't quote the 3db figure.
         * I think it also cancels out changes in the overall resistance of the panel
         * such as may be caused by changes in panel temperature.
         */
        data_x = data[2] - data[1];
        data_y = data[4] - data[3];
        data_z = data[5];

        /* isn't z big enough for valid position data? */
        if ( data_z <= __mg_low_z_limit ) {
#if TEST
                printf("Discarding low-z sample.\n");
#endif
                return 0;
        }

        /* has the position changed more than we will allow? */
        if (have_last_data) {
                if ( (abs(data_x - last_data_x) > data_change_limit)
                        || ( abs(data_y - last_data_y) > data_change_limit ) )
                {
#if TEST
                        printf("Discarding changed-too-much sample.\n");
#endif
                        return 0;
                }
        }

        /* save last position */
        last_data_x = data_x;
        last_data_y = data_y;
        have_last_data = 1;

        /* is filter full? */
        if ( iir_count == iir_sample_depth ) {
                /* make room for new sample */
                iir_accum_x -= iir_accum_x >> iir_shift_bits;
                iir_accum_y -= iir_accum_y >> iir_shift_bits;
                iir_accum_z -= iir_accum_z >> iir_shift_bits;
                iir_count--;
        }

        /* feed new sample to filter */
        iir_accum_x += data_x;
        iir_accum_y += data_y;
        iir_accum_z += data_z;
        /* TODO: optimize the unnecessary count--/++ when filter full */
        iir_count++;

        /* aren't we over the threshold yet? */
        if (iir_count <= iir_output_threshold)
                return 0;

        /* Touch-panel only reports left button. */
        /* I moved this up here in preparation for the below-threshold
         * on pen-up filter output TODO above.
         */
        *pb = LBUTTON;

        /* figure filter output */
        /* TODO: optimize for shifts instead of divides (when possible)? */
        iir_out_x = (iir_accum_x * iir_gain) / iir_count;
        iir_out_y = (iir_accum_y * iir_gain) / iir_count;
        iir_out_z = (iir_accum_z * iir_gain) / iir_count;

#if TEST
        if ( show_filtered )
                printf("data: %hd, %hd, %hd  accum: %d, %d, %d  out: %d, %d, %d\n",
                        data_x, data_y, data_z,
                        iir_accum_x, iir_accum_y, iir_accum_z,
                        iir_out_x, iir_out_y, iir_out_z);
#endif

        /* transformation enabled? */
        if (__mg_vr_enable_transform)
        {
                /* Transform filtered position info to screen coordinates.
                 * Yes, x and y are meant to be swapped.
                 * Odd as it seems, the x value really represents the y position
                 * of the pen and the y value represents the x position of the pen.
                 * This has to do with the way the touch panel hardware works.
                 * It applies a voltage to the y axis to measure x, then applies a
                 * voltage to the x axis to measure y.
                 * X and y values from the touch-panel driver are those
                 * voltage measurements, not position measurements.
                 */
#ifdef REMOTE_TEST
                XYPOINT transformed = {iir_out_x, iir_out_y};
#else                
                XYPOINT transformed = {iir_out_y, iir_out_x};
#endif                
                transformed = DeviceToScreen(transformed);

                /*
                 * HACK: move this from quarter pixels to whole pixels for now
                 * at least until I decide on the right interface to get the
                 * quarter-pixel data up to the next layer.
                 */
                *px = transformed.x >> 2;
                *py = transformed.y >> 2;
        }
        else
        {
                /*
                 * Return untransformed filtered position info.
                 * Same odd swapping of x and y due to the way that
                 * the touch panel works (see above).
                 */
                *px = iir_out_y;
                *py = iir_out_x;
        }

        /* Return filtered pressure.*/
        *pz = iir_out_z;

        return 2;
}



/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update(void)
{
    return 1;
}

static void mouse_getxy (int* x, int* y)
{
    *x = mousex;
    *y = mousey;
}

static int mouse_getbutton(void)
{
    return mousez;
}

static int keyboard_update(void)
{
    switch ((btn_state) & 0x0fff)
    {
    case 0x7:
//keycode = Key_Up
    break;
    case 0x9:
//keycode = Key_Right
    break;
    case 0x8:
//keycode = Key_Down    
    break;
    case 0xa:
//keycode = Key_Left    
    break;
    case 0x3:
//keycode = Key_Up
    break;
    case 0x4:
//keycode = Key_Down
    break;
    case 0x1:
//keycode = Key_Backspace
    break;
    case 0x2:
//keycode = Key_Escape
    break;
    case 0x5:
//keycode = Key_CONTRAST
    break;
    case 0x6:
//keycode = Key_BACKLIGHT
    break;
    case 256:
//keycode = Key_AP1
    break;
    case 257:
//keycode = Key_AP2  
    break;
    case 258:
//keycode = Key_AP3   
    break;
    case 259:
//keycode = Key_AP4    
    break;
    }            

    return 0;
}

static const char* keyboard_getstate(void)
{
    return (char *)state;
}

#ifdef _LITE_VERSION 
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    fd_set rfds;
    int    e;
    int    retvalue = 0;
    int    x,y,z,b;
    int    result;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if (which & IAL_MOUSEEVENT && ts >= 0) {
        FD_SET (ts, in);
        if (ts > maxfd) maxfd = ts;
    }

#ifdef _VR_BUTTONS
        if (which & IAL_KEYEVENT && btn_fd >= 0){
            FD_SET (btn_fd, in);
            if(btn_fd > maxfd)
            maxfd = btn_fd;
    }
#endif
        
    e = select (maxfd + 1, in, out, except, timeout) ;

    if (e > 0) { 
        if (ts >= 0 && FD_ISSET (ts, in)) {
            FD_CLR (ts, in);

            /* FIXME: while telnet: x & y should be exchanged. */
            result = VrTpanelRead (ts, &x, &y, &z, &b);
            if (result == 2 || result == 3) {
                mousex = x;
                mousey = y;
                mousez = b;

#if REMOTE_TEST            
                fprintf(stderr,"result:\t\t%d\n",result);
                fprintf(stderr,"mousex\tmousey\tmousez:\t%d\t%d\t%d\n",mousex,mousey,mousez);
#endif        
                mousez = ( mousez > 0 ? IAL_MOUSE_LEFTBUTTON:0);
                retvalue |=IAL_MOUSEEVENT; 
            }
        }
        
#ifdef _VR_BUTTONS
        if (btn_fd >= 0 && FD_ISSET(btn_fd, in)) {
            unsigned short code;
            FD_CLR(btn_fd, in);
            result = read (btn_fd, &code, sizeof(code));
            btn_state = code;
            retvalue |= IAL_KEYEVENT;
        }
#endif                           
    } else if (e < 0) {
        return -1;
    }

    return retvalue;
}
#else

static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    int    x,y,z,b,result;
    struct pollfd ufd;
    if (which & IAL_MOUSEEVENT) {
        ufd.fd     = ts;
        ufd.events = POLLIN; 
        if (poll (&ufd, 1, timeout) > 0) {
            result = VrTpanelRead(ts, &x, &y, &z, &b);
            if (result == 2 || result == 3) {
                mousex = x;
                mousey = y;
                mousez = b;
                return IAL_MOUSEEVENT;
            }
        }
    }
    return 0;
}
#endif

BOOL InitVR4181Input (INPUT* input, const char* mdev, const char* mtype)
{
    int i;

    ts = VrTpanelInit();
    if ( ts < 0 ) {
        fprintf (stderr, "IAL: Can not open vr41xx touch screen!\n");
        return FALSE;
    }

    for(i = 0; i < NR_KEYS; i++)
        state[i] = 0;

#ifdef _VR_BUTTONS
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
    mousex = 0;
    mousey = 0;
    return TRUE;
}

void TermVR4181Input (void)
{
    if ( ts >= 0 )
        close(ts);
}

#endif /* _VR4181_IAL */

