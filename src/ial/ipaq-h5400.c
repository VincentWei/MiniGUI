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
** ipaq-h5400.c: Low Level Input Engine for iPAQ H5400.
** 
** Created by Wei Yongming, 2001/08/20
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGIAL_IPAQ_H5400

#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <linux/kd.h>
#include <linux/keyboard.h>

#include "ial.h"
#include "ipaq-h5400.h"

/*
 * Keyboard operations -- Event
 */

/* variables for keyboard (using tty directly) */
static const char* tty_dev;
static int kbd_fd = -1;        /* file descriptor for keyboard */
static struct termios startup_termios;        /* original terminal modes */
static struct termios work_termios;    /* startup_termios terminal modes */
static int startup_kbdmode;
static unsigned char state [NR_KEYS];

/*
 * Open the keyboard.
 * This is real simple, we just use a special file handle
 * that allows blocking I/O, and put the terminal into
 * character mode.
 */
static int TTY_Open (void)
{
    if (geteuid() == 0) /* is a super user, try to open the active console */
        tty_dev = "/dev/tty0";
    else    /* not a super user, so try to open the control terminal */
        tty_dev = "/dev/tty";

    kbd_fd = open(tty_dev, O_RDONLY | O_NOCTTY);
    if (kbd_fd < 0)
        return -1;

    if (tcgetattr(kbd_fd, &startup_termios) < 0)
        goto err;

    work_termios = startup_termios;

    work_termios.c_lflag &= ~(ICANON | ECHO  | ISIG);
    work_termios.c_iflag &= ~(ISTRIP | IGNCR | ICRNL | INLCR | IXOFF | IXON);
    work_termios.c_iflag |= IGNBRK;
    work_termios.c_cc[VMIN] = 0;
    work_termios.c_cc[VTIME] = 0;

    if(tcsetattr(kbd_fd, TCSAFLUSH, &work_termios) < 0)
        goto err;

    /* Put the keyboard into MEDIUMRAW mode.  Despite the name, this
     * is really "mostly raw", with the kernel just folding long
     * scancode sequences (e.g. E0 XX) onto single keycodes.
     */
    ioctl (kbd_fd, KDGKBMODE, &startup_kbdmode);
    if (ioctl(kbd_fd, KDSKBMODE, K_MEDIUMRAW) < 0) 
        goto err;
    return kbd_fd;

err:
    close(kbd_fd);
    kbd_fd = 0;
    return -1;
}

/*
 * Close the keyboard.
 * This resets the terminal modes.
 */
static void TTY_Close (void)
{
    ioctl (kbd_fd, KDSKBMODE, startup_kbdmode);
    tcsetattr (kbd_fd, TCSANOW, &startup_termios);
    close (kbd_fd);
    kbd_fd = -1;
}

/*
 * This reads one keystroke from the keyboard, and the current state of
 * the mode keys (ALT, SHIFT, CTRL).  Returns -1 on error, 0 if no data
 * is ready, and 1 if data was read.  This is a non-blocking call.
 */
static int TTY_Read (unsigned char *buf, int *modifiers)
{
    int    cc;            /* characters read */
    unsigned char buf1;

    *modifiers = 0;            /* no modifiers yet */
    cc = read (kbd_fd, &buf1, 1);
    *buf = buf1;
    if (cc > 0) {
        return 1;
    }

    if ((cc < 0) && (errno != EINTR) && (errno != EAGAIN))
        return -1;
    return 0;
}

static int keyboard_update(void)
{
    unsigned char buf;
    int modifier;
    int ch;
    int is_pressed;
    int retvalue;

    retvalue = TTY_Read (&buf, &modifier); 

    if ((retvalue == -1) || (retvalue == 0))
        return 0;

    else { /* retvalue > 0 */
        is_pressed = !(buf & 0x80);
        ch         = buf & 0x7f;
        switch (ch) {
            //navigation mid 
        case 96:
            state[SCANCODE_ENTER] = is_pressed;
            break;

            //navigation up
        case 103:
            state[SCANCODE_CURSORBLOCKUP] = is_pressed;
            break;

            //navigation left 
        case 105:
            state[SCANCODE_CURSORBLOCKLEFT] = is_pressed;
            break;

            //navigation right 
        case 106:
            state[SCANCODE_CURSORBLOCKRIGHT] = is_pressed;
            break;

            //navigation down 
        case 108:
            state[SCANCODE_CURSORBLOCKDOWN] = is_pressed;
            break;

            //left 2
        case 122: 
            state[SCANCODE_F3] = is_pressed;
            break;

            //left 1
        case 123: 
            state[SCANCODE_F1] = is_pressed;
            break;

            //right 1
        case 124: 
            state[SCANCODE_F2] = is_pressed;
            break;

            //right 2
        case 125:
            state[SCANCODE_F4] = is_pressed;
            break;
        }
    }
    return NR_KEYS;
}

static const char* keyboard_getstate(void)
{
    return (char *)state;
}

/*
 * Mouse operations -- Event
 */

#define DIS_NUM 4

/* for data reading from ts device */
typedef struct {
    short b;
    short x;
    short y;
    short pad;
} POS;

/* variables for touch screen */
static int ts_fd = -1;
static int button = 0;
static int mousex = 0;
static int mousey = 0;
static int nr_events = 0;
static POS ts_events [DIS_NUM];

static int mouse_update(void)
{
    return 1;
}

static void mouse_getxy(int *x, int* y)
{
    int i;
    int max_x = ts_events [0].x;
    int min_x = ts_events [0].x;
    int max_event = 0, min_event = 0;
    int sum_x = 0, sum_y = 0;

    if (nr_events != DIS_NUM) {
        *x = mousex;
        *y = mousey;
        return;
    }

    for (i = 1; i < nr_events; i++) {
        if (ts_events [i].x > max_x) {
            max_x = ts_events[i].x; //save the max x
            max_event = i;
        }
        if (ts_events [i].x < min_x) {
            min_x = ts_events[i].x ; //save the min x
            min_event = i;
        }
    }

    for (i = 0; i < nr_events; i++) {
        if (i != max_event && i != min_event) {
            sum_x += ts_events [i].x;
            sum_y += ts_events [i].y;
        }
    }

    if (max_event == min_event) {
        *x = sum_x / (nr_events - 1);
        *y = sum_y / (nr_events - 1);
    }
    else {
        *x = sum_x / (nr_events - 2);
        *y = sum_y / (nr_events - 2);
    }


    if (ABS (mousex - *x) > 10 || ABS (mousey - *y) > 10) {
        mousex = *x;
        mousey = *y;
    }

    *x = mousex;
    *y = mousey;
    nr_events = 0;
}

static int mouse_getbutton(void)
{
    return button;
}

static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    fd_set rfds;
    int    retvalue = 0;
    int    e;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if ((which & IAL_MOUSEEVENT) && ts_fd >= 0) {
        FD_SET (ts_fd, in);
#ifndef _MGRM_THREADS
        if (ts_fd > maxfd) maxfd = ts_fd;
#endif
    }
    if ((which & IAL_KEYEVENT) && kbd_fd >= 0){
        FD_SET (kbd_fd, in);
#ifndef _MGRM_THREADS
        if(kbd_fd > maxfd) maxfd = kbd_fd;
#endif
    }

#ifndef _MGRM_THREADS
    e = select (maxfd + 1, in, out, except, timeout);
#else
    e = select (FD_SETSIZE, in, out, except, timeout);
#endif

    if (e > 0) { 
        /* If data is present on the mouse fd, service it: */
        if (ts_fd >= 0 && FD_ISSET (ts_fd, in)) {
            short data [4];

            FD_CLR (ts_fd, in);
            read (ts_fd, data, sizeof(data));
            if (data[0] <= 0) {
                nr_events = 0;
                button = 0;
                retvalue |= IAL_MOUSEEVENT;
            }
            else {
                ts_events [nr_events].x = -(data[1]-906)*480/(750+764);
                ts_events [nr_events].y = -(data[2]-922)*640/(824+808);
                nr_events ++;
                if (nr_events >= DIS_NUM) {
                    button = IAL_MOUSE_LEFTBUTTON;
                    retvalue |= IAL_MOUSEEVENT;
                }
            }
        }

        /* If data is present on the keyboard kbd_fd, service it: */
        if (kbd_fd >= 0 && FD_ISSET (kbd_fd, in)) {
            FD_CLR (kbd_fd, in);
            retvalue |= IAL_KEYEVENT;
        }
    } else if (e < 0) {
        return -1;
    }

    return retvalue;
}

BOOL InitIPAQH5400Input (INPUT* input, const char* mdev, const char* mtype)
{
    ts_fd = open (mdev, O_RDONLY);
    if (ts_fd < 0) {
        _MG_PRINTF ("IAL>%s: Can not open touch screen device: %s!\n", __FILE__, mdev);
        return FALSE;
    }

    kbd_fd = TTY_Open ();
    if (kbd_fd < 0 ) {
        _MG_PRINTF ("IAL>%s: Can not open button key!\n", __FILE__);
        return FALSE;
    }

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

void TermIPAQH5400Input (void)
{
    if (ts_fd >= 0)
        close (ts_fd);

    if (kbd_fd >= 0)
        TTY_Close ();
}

#endif /* _MGIAL_IPAQ_H5400 */

