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
** native.c: Native IAL engine
**
** Created by Song Lixin at 2000/10/17
** Clean code for MiniGUI 1.1.x by Wei Yongming, 20001/09/18
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <linux/kd.h>
#include <linux/keyboard.h>
#include <errno.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h" /* for GetTickCount() */
#include "window.h" /* for GetTickCount() */
#include "misc.h"
#include "ial.h"
#include "gal.h"
#include "native.h"

static int mouse_fd;
static int kbd_fd;

static int    xpos;        /* current x position of mouse */
static int    ypos;        /* current y position of mouse */
static int    minx;        /* minimum allowed x position */
static int    maxx;        /* maximum allowed x position */
static int    miny;        /* minimum allowed y position */
static int    maxy;        /* maximum allowed y position */
static int    buttons;     /* current state of buttons */
static int    scale;       /* acceleration scale factor */
static int    thresh;      /* acceleration threshhold */

static MOUSEDEVICE * mousedev;

static unsigned char state [NR_KEYS];
static KBDDEVICE * kbddev = &kbddev_tty;
static char keyboard_engine[1024] = "tty";
static char keyboard_device[PATH_MAX] = "";
static char mouse_device[PATH_MAX] = "";

/************************  Low Level Input Operations **********************/

/*
 * Mouse operations -- Event
 */
static int mouse_update(void)
{
    int dx,dy,dz;
    int r;
    int sign;
    
    if (!mousedev)
        return 0;

    dx = dy = 0;
    r = mousedev->Read(&dx, &dy, &dz, &buttons);
    if (r <= 0)
        return 0;

    sign = 1;
    if (dx < 0) {
        sign = -1;
        dx = -dx;
    }
    if (dx > thresh)
        dx = thresh + (dx - thresh) * scale;
    dx *= sign;
    xpos += dx;
    if( xpos < minx )
        xpos = minx;
    if( xpos > maxx )
        xpos = maxx;

    sign = 1;
    if (dy < 0) {
        sign = -1;
        dy = -dy;
    }
    if (dy > thresh)
        dy = thresh + (dy - thresh) * scale;
    dy *= sign;
    ypos += dy;
    if ( ypos < miny )
        ypos = miny;
    if ( ypos > maxy )
        ypos = maxy;

    return 1;
}

static void mouse_getxy (int* x, int* y)
{
    *x = xpos;
    *y = ypos;
}

static void mouse_setposition (int newx, int newy)
{
    if (newx < minx)
        newx = minx;
    if (newx > maxx)
        newx = maxx;
    if (newy < miny)
        newy = miny;
    if (newy > maxy)
        newy = maxy;
    if (newx == xpos && newy == ypos)
        return;
    xpos = newx;
    ypos = newy;
}

static int mouse_getbutton (void)
{
    return buttons;
}

static void mouse_setrange (int newminx, int newminy, int newmaxx, int newmaxy)
{
    minx = newminx;
    miny = newminy;
    maxx = newmaxx;
    maxy = newmaxy;
    mouse_setposition ((newminx + newmaxx) / 2, (newminy + newmaxy) / 2);
}

static void mouse_suspend(void)
{
    if (mousedev) {
        mousedev->Suspend();
        mouse_fd = -1;
    }
}

static int mouse_resume(void)
{
    if (mousedev) {
        mouse_fd = mousedev->Resume();
        if (mouse_fd < 0) {
            fprintf (stderr, "IAL Native Engine: Can not open mouse!\n");
        }
    }

    return mouse_fd;
}
/*
 * 0 : no data
 * > 0 : the possible maximal key scancode
 */
static int keyboard_update(void)
{
    unsigned char buf;
    int modifier;
    int ch;
    int is_pressed;
    int retvalue;

    retvalue = kbddev->Read (&buf, &modifier); 

    if ((retvalue == -1) || (retvalue == 0))
        return 0;

    else { /* retvalue > 0 */
        is_pressed = !(buf & 0x80);
        ch         = buf & 0x7f;
        if (is_pressed) {
#ifndef _MGRM_THREADS
            if ((ch >= SCANCODE_F1) && (ch <= SCANCODE_F6) && 
                            (state[SCANCODE_RIGHTCONTROL]==1)) 
            {
                vtswitch_try (ch - SCANCODE_F1 + 1);
                state [SCANCODE_RIGHTCONTROL] = 0;
                return 0;
            }
#endif
            state[ch] = 1;
        }
        else 
            state[ch] = 0;
    }

    return NR_KEYS;
}

static const char* keyboard_getstate(void)
{
    return (char*)state;
}

static void keyboard_suspend(void)
{
    kbddev->Suspend();    
    kbd_fd = -1;
}

static int keyboard_resume(void)
{
    memset(state, 0, NR_KEYS);
    kbd_fd = kbddev->Resume ();
    if (kbd_fd < 0) {
        fprintf (stderr, "IAL Native Engine: Can not open keyboard!\n");
    }

    return kbd_fd;
}

static int keyboard_open(const char *device) {
    int fd;

    fd = kbddev->Open(device);
    if (fd < 0) {
        return -1;
    }
    fprintf(stderr, "IAL Native Engine: keyboard device has been connected.\n");
#ifndef _MGRM_THREADS

#ifdef _MGRM_PROCESSES
    if (mgIsServer)
#endif
        init_vtswitch (fd);
#endif
    return fd;
}

static int mouse_open(const char *device) {
    int fd;
    if (mousedev) {
        fd = mousedev->Open(device);
        if (fd < 0) {
            return -1;
        }
        fprintf(stderr, "IAL Native Engine: mouse device has been connected.\n");
        mousedev->GetDefaultAccel (&scale, &thresh);
        return fd;
    } else {
        return -1;
    }
}

/* NOTE by weiym: Do not ignore the fd_set in, out, and except */
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    fd_set    rfds, wfds;
    int    retvalue = 0;
    int    fd, e;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }
    if (! out) {
        out = &wfds;
        FD_ZERO (out);
    }

    if (mouse_fd < 0 || kbd_fd < 0) {
        static unsigned int last_time;
        unsigned int now = GetTickCount();
        if (last_time + 100 <= now || last_time > now) {
            if (mouse_fd < 0) {
                mouse_fd = mouse_open(mouse_device);
            }
            if (kbd_fd < 0) {
                kbd_fd = keyboard_open(keyboard_device);
            }
            last_time = now;
        }
    }

    if (which & IAL_MOUSEEVENT && mouse_fd >= 0) {
        fd = mouse_fd;          /* FIXME: mouse fd may be changed in vt switch! */
        FD_SET (fd, in);
#ifndef _MGRM_THREADS
        if (fd > maxfd) maxfd = fd;
#endif
        if (kbddev == &kbddev_event) {
            FD_SET (fd, out);
        }
    }

    if (which & IAL_KEYEVENT && kbd_fd >= 0){
        fd = kbd_fd;          /* FIXME: keyboard fd may be changed in vt switch! */
        FD_SET (kbd_fd, in);
#ifndef _MGRM_THREADS
        if (fd > maxfd) maxfd = fd;
#endif
        if (kbddev == &kbddev_event) {
            FD_SET (fd, out);
        }
    }

    /* FIXME: pass the real set size */
#ifndef _MGRM_THREADS
    e = select (maxfd + 1, in, out, except, timeout) ;
#else
    e = select (MAX (mouse_fd, kbd_fd) + 1, in, out, except, timeout) ;
#endif

    if (e > 0) { 
        fd = mouse_fd;
        /* If data is present on the mouse fd, service it: */
        if (fd >= 0) {
            if (kbddev == &kbddev_event && FD_ISSET (fd, out)){
                FD_CLR (fd, out);
                mousedev->Close();
                mouse_fd = -1;
                fprintf(stderr, "IAL Native Engine: mouse device has been disconnected.\n");
            }else if (FD_ISSET (fd, in)) {
                FD_CLR (fd, in);
                retvalue |= IAL_MOUSEEVENT;
            }
        }

        fd = kbd_fd;
        /* If data is present on the keyboard fd, service it: */
        if (fd >= 0) {
            if (kbddev == &kbddev_event && FD_ISSET (fd, out)){
                FD_CLR (fd, out);
                kbddev->Close();
                kbd_fd = -1;
                fprintf(stderr, "IAL Native Engine: keyboard device has been disconnected.\n");
            }else if (FD_ISSET (fd, in)) {
                FD_CLR (fd, in);
                retvalue |= IAL_KEYEVENT;
            }
        }

    } else if (e < 0) {
        return -1;
    }

    return retvalue;
}

static void set_leds (unsigned int leds)
{
    ioctl (0, KDSETLED, leds);
}

BOOL InitNativeInput (INPUT* input, const char* mdev, const char* mtype)
{
    if (GetMgEtcValue("console", "kbd_interface", keyboard_engine, sizeof(keyboard_engine)) != 0) {
        // XXX: To be compatible with old configure files
        GetMgEtcValue("ial_console", "keyboard_engine", keyboard_engine, sizeof(keyboard_engine));
    }

    strncpy(mouse_device, mdev, sizeof(mouse_device));
    mouse_device[sizeof(mouse_device)-1] = 0;

    if (strcasecmp(keyboard_engine, "input") == 0
            // XXX: To be compatible with old configure files
            || strcasecmp(keyboard_engine, "event") == 0)
    {
        kbddev = &kbddev_event;
    }
    else if (strcasecmp(keyboard_engine, "tty") == 0)
    {
        kbddev = &kbddev_tty;
    }
    else
    {
        fprintf(stderr, "IAL: [warning] unknown keyboard_engine '%s'\n", keyboard_engine);
    }

    if (GetMgEtcValue ("console", "kbd_device", keyboard_device, sizeof(keyboard_device)) != 0) {
        // XXX: To be compatible with old configure files
        GetMgEtcValue ("ial_console", "keyboard_device", keyboard_device, sizeof(keyboard_device));
    }

    fprintf(stderr, "IAL: use '%s' native keyboard engine, with the device file '%s'\n", keyboard_engine, keyboard_device);

    kbd_fd = keyboard_open(keyboard_device);
    if (kbd_fd < 0) {
        fprintf (stderr, "[WARNING] IAL Native Engine: Can not init keyboard right now, %s(%s).\nPlease plug in the KEYBOARD!\n", strerror(errno), keyboard_device);
    }
    
    if (strcasecmp (mtype, "none") == 0) {
        mousedev = NULL;
        goto found;
    }
#ifdef _MGCONSOLE_IMPS2
    if (strcasecmp (mtype, "imps2") == 0 ) {
        mousedev = &mousedev_IMPS2;
        goto found;
    }
#endif
#ifdef _MGCONSOLE_PS2
    if (strcasecmp (mtype, "ps2") == 0 ) {
        mousedev = &mousedev_PS2;
        goto found;
    }
#endif
#ifdef _MGCONSOLE_GPM
    if (strcasecmp (mtype, "gpm") == 0) {
        mousedev = &mousedev_GPM;
        goto found;
    }
#endif
#ifdef _MGCONSOLE_MS
    if (strcasecmp (mtype, "ms") == 0) {
        mousedev = &mousedev_MS;
        goto found;
    }
#endif
#ifdef _MGCONSOLE_MS3
    if (strcasecmp (mtype, "ms3") == 0) {
        mousedev = &mousedev_MS3;
        goto found;
    }
#endif

    fprintf (stderr, "IAL Native Engine: Can not init pointing device!\n");
    return FALSE;

found:
    if (mousedev) {
        mouse_fd = mouse_open(mdev);
        if (mouse_fd <0) {
            fprintf (stderr, "[WARNING] IAL Native Engine: Can not init mouse right now, %s(%s).\nPlease plug in the MOUSE!\n", strerror(errno), mdev);
        }
    }
    else
        mouse_fd = -1;

    xpos = 0;
    ypos = 0;
    buttons = 0;
    minx = 0;
    miny = 0;
/*
    maxx = WIDTHOFPHYGC - 1;
    maxy = HEIGHTOFPHYGC - 1;
*/
    maxx = WIDTHOFPHYSCREEN;
    maxy = HEIGHTOFPHYSCREEN;

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = mouse_setposition;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = mouse_setrange;
    input->suspend_mouse= mouse_suspend;
    input->resume_mouse = mouse_resume;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->suspend_keyboard = keyboard_suspend;
    input->resume_keyboard = keyboard_resume;
    input->set_leds = set_leds;

    input->wait_event = wait_event;
    
    return TRUE;
}

void TermNativeInput (void)
{
#ifndef _MGRM_THREADS

#ifdef _MGRM_PROCESSES
    if (mgIsServer)
#endif
        done_vtswitch (kbd_fd);
#endif

    if (mousedev)
        mousedev->Close();
    kbddev->Close();
}

