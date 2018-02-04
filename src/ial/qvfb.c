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
** qvfb.c: Input Engine for Qt Virtual FrameBuffer
** 
** Created by Wei Yongming, 2001/11/26
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGIAL_QVFB

#ifdef __ECOS__
/* for eCos Linux Synthetic target */
#include <cyg/hal/hal_io.h>
#else
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#endif

#include "minigui.h"
#include "misc.h"

#ifdef __ECOS__

#define O_RDONLY CYG_HAL_SYS_O_RDONLY

#undef FD_SET
#define FD_SET CYG_HAL_SYS_FD_SET
#undef FD_ZERO
#define FD_ZERO CYG_HAL_SYS_FD_ZERO
#undef FD_CLR
#define FD_CLR CYG_HAL_SYS_FD_CLR
#undef FD_ISSET
#define FD_ISSET CYG_HAL_SYS_FD_ISSET

#define fd_set struct cyg_hal_sys_fd_set
#define read cyg_hal_sys_read
#define open(name, flags) cyg_hal_sys_open (name, flags, 0)
#define close cyg_hal_sys_close
#define select cyg_hal_sys__newselect
#define timeval cyg_hal_sys_timeval
#define tv_sec hal_tv_sec
#define tv_usec hal_tv_usec
#else
#include <fcntl.h>
#endif

#define __NONEED_FD_SET_TYPE
#include "ial.h"
#include "qvfb.h"

#define NOBUTTON        0x0000
#define LEFTBUTTON      0x0001
#define RIGHTBUTTON     0x0002
#define MIDBUTTON       0x0004
#define MOUSEBUTTONMASK 0x00FF

#define SHIFTBUTTON     0x0100
#define CONTROLBUTTON   0x0200
#define ALTBUTTON       0x0400
#define METABUTTON      0x0800
#define KEYBUTTONMASK   0x0FFF
#define KEYPAD          0x4000

static int mouse_fd = -1;
static int kbd_fd = -1;
static POINT mouse_pt;
static int mouse_buttons;
static struct QVFbKeyData kbd_data;
static unsigned char kbd_state [NR_KEYS];
static unsigned char keycode_scancode [256];
#include "auto-protocol.h"
static int record_fd = -1;
static long long int record_time_start = 0;

static long long int getcurtime()
{
    struct timeval tv;
    gettimeofday( &tv, NULL );
    return (long long int)tv.tv_sec * 1000 + (long long int)tv.tv_usec / 1000;
}

static void init_code_map (void)
{
    keycode_scancode [0x00] = SCANCODE_ESCAPE;

    keycode_scancode [0x29] = SCANCODE_0;
    keycode_scancode [0x21] = SCANCODE_1;
    keycode_scancode [0x40] = SCANCODE_2;
    keycode_scancode [0x23] = SCANCODE_3;
    keycode_scancode [0x24] = SCANCODE_4;
    keycode_scancode [0x25] = SCANCODE_5;
    keycode_scancode [0x5E] = SCANCODE_6;
    keycode_scancode [0x26] = SCANCODE_7;
    keycode_scancode [0x2A] = SCANCODE_8;
    keycode_scancode [0x28] = SCANCODE_9;

    keycode_scancode [0x30] = SCANCODE_0;
    keycode_scancode [0x31] = SCANCODE_1;
    keycode_scancode [0x32] = SCANCODE_2;
    keycode_scancode [0x33] = SCANCODE_3;
    keycode_scancode [0x34] = SCANCODE_4;
    keycode_scancode [0x35] = SCANCODE_5;
    keycode_scancode [0x36] = SCANCODE_6;
    keycode_scancode [0x37] = SCANCODE_7;
    keycode_scancode [0x38] = SCANCODE_8;
    keycode_scancode [0x39] = SCANCODE_9;

//    keycode_scancode [0x30] = SCANCODE_F1;
//    keycode_scancode [0x31] = SCANCODE_F2;
//    keycode_scancode [0x32] = SCANCODE_F3;
//    keycode_scancode [0x33] = SCANCODE_F4;
//    keycode_scancode [0x34] = SCANCODE_F5;
//    keycode_scancode [0x35] = SCANCODE_F6;
//    keycode_scancode [0x36] = SCANCODE_F7;
//    keycode_scancode [0x37] = SCANCODE_F8;
//    keycode_scancode [0x38] = SCANCODE_F9;
//    keycode_scancode [0x39] = SCANCODE_F10;

    keycode_scancode [0x2D] = SCANCODE_MINUS;
    keycode_scancode [0x5F] = SCANCODE_MINUS;
    keycode_scancode [0x3D] = SCANCODE_EQUAL;
    keycode_scancode [0x2B] = SCANCODE_EQUAL;
    keycode_scancode [0x03] = SCANCODE_BACKSPACE;
    keycode_scancode [0x01] = SCANCODE_TAB;
    keycode_scancode [0x51] = SCANCODE_Q;
    keycode_scancode [0x57] = SCANCODE_W;
    keycode_scancode [0x45] = SCANCODE_E;
    keycode_scancode [0x52] = SCANCODE_R;
    keycode_scancode [0x54] = SCANCODE_T;
    keycode_scancode [0x59] = SCANCODE_Y;
    keycode_scancode [0x55] = SCANCODE_U;
    keycode_scancode [0x49] = SCANCODE_I;
    keycode_scancode [0x4F] = SCANCODE_O;
    keycode_scancode [0x50] = SCANCODE_P;
    keycode_scancode [0x5B] = SCANCODE_BRACKET_LEFT;
    keycode_scancode [0x7B] = SCANCODE_BRACKET_LEFT;
    keycode_scancode [0x5D] = SCANCODE_BRACKET_RIGHT;
    keycode_scancode [0x7D] = SCANCODE_BRACKET_RIGHT;
    keycode_scancode [0x04] = SCANCODE_ENTER;
    keycode_scancode [0x41] = SCANCODE_A;
    keycode_scancode [0x53] = SCANCODE_S;
    keycode_scancode [0x44] = SCANCODE_D;
    keycode_scancode [0x46] = SCANCODE_F;
    keycode_scancode [0x47] = SCANCODE_G;
    keycode_scancode [0x48] = SCANCODE_H;
    keycode_scancode [0x4A] = SCANCODE_J;
    keycode_scancode [0x4B] = SCANCODE_K;
    keycode_scancode [0x4C] = SCANCODE_L;

    keycode_scancode [0x3A] = SCANCODE_SEMICOLON;
    keycode_scancode [0x3B] = SCANCODE_SEMICOLON;
//    keycode_scancode [0x3B] = SCANCODE_F12;


    keycode_scancode [0x27] = SCANCODE_APOSTROPHE;
    keycode_scancode [0x22] = SCANCODE_APOSTROPHE;
    keycode_scancode [0x60] = SCANCODE_GRAVE;
    keycode_scancode [0x7E] = SCANCODE_GRAVE;

    keycode_scancode [0x20] = SCANCODE_SPACE;
//    keycode_scancode [0x20] = SCANCODE_LEFTSHIFT;

    keycode_scancode [0x5C] = SCANCODE_BACKSLASH;
    keycode_scancode [0x7C] = SCANCODE_BACKSLASH;
    keycode_scancode [0x5A] = SCANCODE_Z;
    keycode_scancode [0x58] = SCANCODE_X;
    keycode_scancode [0x43] = SCANCODE_C;
    keycode_scancode [0x56] = SCANCODE_V;
    keycode_scancode [0x42] = SCANCODE_B;
    keycode_scancode [0x4E] = SCANCODE_N;
    keycode_scancode [0x4D] = SCANCODE_M;
    keycode_scancode [0x2C] = SCANCODE_COMMA;
    keycode_scancode [0x3C] = SCANCODE_COMMA;
    keycode_scancode [0x2E] = SCANCODE_PERIOD;
    keycode_scancode [0x3E] = SCANCODE_PERIOD;
    keycode_scancode [0x2F] = SCANCODE_SLASH;
    keycode_scancode [0x3F] = SCANCODE_SLASH;
//    keycode_scancode [0x2A] = SCANCODE_KEYPADMULTIPLY;

    keycode_scancode [0x09] = SCANCODE_PRINTSCREEN;
    keycode_scancode [0x08] = SCANCODE_BREAK;
    keycode_scancode [0x06] = SCANCODE_INSERT;
    keycode_scancode [0x07] = SCANCODE_REMOVE;
    keycode_scancode [0x10] = SCANCODE_HOME;
    keycode_scancode [0x11] = SCANCODE_END;
    keycode_scancode [0x16] = SCANCODE_PAGEUP;
    keycode_scancode [0x17] = SCANCODE_PAGEDOWN;
    keycode_scancode [0x13] = SCANCODE_CURSORBLOCKUP;
    keycode_scancode [0x12] = SCANCODE_CURSORBLOCKLEFT;
    keycode_scancode [0x14] = SCANCODE_CURSORBLOCKRIGHT;
    keycode_scancode [0x15] = SCANCODE_CURSORBLOCKDOWN;

//    keycode_scancode [0x23] = SCANCODE_RIGHTALT;
//    keycode_scancode [0x2F] = SCANCODE_KEYPADDIVIDE;
//    keycode_scancode [0x20] = SCANCODE_RIGHTSHIFT;
//    keycode_scancode [0x21] = SCANCODE_RIGHTCONTROL;
//    keycode_scancode [0x37] = SCANCODE_KEYPAD7;
//    keycode_scancode [0x38] = SCANCODE_KEYPAD8;
//    keycode_scancode [0x39] = SCANCODE_KEYPAD9;
//    keycode_scancode [0x2D] = SCANCODE_KEYPADMINUS;
//    keycode_scancode [0x34] = SCANCODE_KEYPAD4;
//    keycode_scancode [0x35] = SCANCODE_KEYPAD5;
//    keycode_scancode [0x36] = SCANCODE_KEYPAD6;
//    keycode_scancode [0x2B] = SCANCODE_KEYPADPLUS;
//    keycode_scancode [0x31] = SCANCODE_KEYPAD1;
//    keycode_scancode [0x32] = SCANCODE_KEYPAD2;
//    keycode_scancode [0x33] = SCANCODE_KEYPAD3;
//    keycode_scancode [0x30] = SCANCODE_KEYPAD0;
//    keycode_scancode [0x2E] = SCANCODE_KEYPADPERIOD;
//    keycode_scancode [] = SCANCODE_LESS;
//    keycode_scancode [] = SCANCODE_KEYPADENTER;
//    keycode_scancode [] = SCANCODE_PAUSE;
//    keycode_scancode [] = SCANCODE_LEFTWIN;
//    keycode_scancode [] = SCANCODE_RIGHTWIN;
//    keycode_scancode [] = SCANCODE_MENU;
}

static unsigned char keycode_to_scancode (unsigned char keycode, BOOL asscii)
{
    if(!asscii)
    {
        switch (keycode) {
            case 0x30 ... 0x39:
                return SCANCODE_F1 + keycode - 0x30;
            case 0x3B:
                return SCANCODE_F12;
            case 0x20:
                return SCANCODE_LEFTSHIFT;
            case 0x21:
                return SCANCODE_LEFTCONTROL;
            case 0x23:
                return SCANCODE_LEFTALT;
            case 0x24:
                return SCANCODE_CAPSLOCK;
            case 0x25:
                return SCANCODE_NUMLOCK;
            case 0x26:
                return SCANCODE_SCROLLLOCK;
            case 0x3A:
                return SCANCODE_F11;
            default:
                /* NULL */
                break;
        }
    }

    return keycode_scancode [keycode];
}

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static ssize_t xread(int fd, void *buf, size_t count)
{
    ssize_t ret;
    size_t nread = 0;
    while (nread < count)
    {
        ret = read(fd, buf, count-nread);
        if (ret == -1)
        {
            if (errno == EAGAIN)
            {
                continue;
            }
            else
            {
                perror("read()");
                return -1;
            }
        }
        else if (ret == 0)
        {
            break;
        }
        else
        {
            nread += ret;
        }
    }
    return nread;
}

static void mouse_getxy (int *x, int* y)
{
    *x = mouse_pt.x;
    *y = mouse_pt.y;
}

static int mouse_getbutton (void)
{
    int buttons = 0;

    if (mouse_buttons & LEFTBUTTON)
        buttons |= IAL_MOUSE_LEFTBUTTON;
    if (mouse_buttons & RIGHTBUTTON)
        buttons |= IAL_MOUSE_RIGHTBUTTON;
    if (mouse_buttons & MIDBUTTON)
        buttons |= IAL_MOUSE_MIDDLEBUTTON;

    return buttons;
}

static void do_record(AUTO_IAL_INPUT_EVENT *event)
{
#define MAX_COUNT 50
#define MAX_TIME 3000
    static int record_count = 0;
    static long long int record_last_time = 0;

#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    event->timestamp = ArchSwap64(event->timestamp);
    if (event->type == 0)
    {
        event->u.key_event.scancode = ArchSwap32(event->u.key_event.scancode);
        event->u.key_event.type = ArchSwap32(event->u.key_event.type);
    }
    else
    {
        event->u.mouse_event.x = ArchSwap32(event->u.mouse_event.x);
        event->u.mouse_event.y = ArchSwap32(event->u.mouse_event.y);
        event->u.mouse_event.buttons = ArchSwap32(event->u.mouse_event.buttons);
    }
    event->type = ArchSwap32(event->type);
#endif

    write(record_fd, event, sizeof(*event));
    if (record_count++ >= MAX_COUNT || (event->timestamp - record_last_time) >= MAX_TIME)
    {
        record_count = 0;
        record_last_time = event->timestamp;
        fsync(record_fd);
    }
}

static int mouse_update (void)
{
    POINT l_mouse_pt;
    int l_mouse_buttons;
#if QT_VERSION>=4
    int l_mouse_wheel;
#endif

    if (0
            || xread (mouse_fd, &l_mouse_pt, sizeof (POINT)) != sizeof (POINT)
            || xread (mouse_fd, &l_mouse_buttons, sizeof (l_mouse_buttons)) != sizeof (l_mouse_buttons)
#if QT_VERSION>=4
            || xread (mouse_fd, &l_mouse_wheel, sizeof(l_mouse_wheel)) != sizeof (l_mouse_wheel)
#endif
            || l_mouse_buttons >= 0x08)
    {
        /* houhh 20081218, if close qvfb, then exit minigui app.*/
        static int count = 0;
        if (errno == ESPIPE && count++ > 5)
        {
            fprintf(stderr, "Has `qvfb' exited? I'm going to quit now\n");
            ExitGUISafely (-1);
        }
        return 0;
    }
    else
    {
        mouse_pt = l_mouse_pt;
        mouse_buttons = l_mouse_buttons;

        if (record_fd >= 0){
            AUTO_IAL_INPUT_EVENT event;
            event.timestamp = getcurtime() - record_time_start;
            event.type = 1;
            mouse_getxy(&event.u.mouse_event.x, &event.u.mouse_event.y);
            event.u.mouse_event.buttons = mouse_getbutton();
            do_record(&event);
        }

        return 1;
    }
}

static unsigned char nr_changed_keys = 0;

static int read_key (void)
{
    static unsigned char last;
    struct QVFbKeyData l_kbd_data;
    int ret;
    unsigned char scancode;
    char press;

    ret = read (kbd_fd, &l_kbd_data, sizeof (struct QVFbKeyData));

    if (ret == sizeof (struct QVFbKeyData)) {
        kbd_data = l_kbd_data;
    }

    press = kbd_data.press;

    if (kbd_data.repeat) {
        return 1;
    }

    if (kbd_data.unicode == 0 && !press) {
        kbd_state [last] = 0;
        if (record_fd >= 0){
            AUTO_IAL_INPUT_EVENT event;
            event.timestamp = getcurtime() - record_time_start;
            event.type = 0;
            event.u.key_event.scancode = last;
            event.u.key_event.type = press ? 1 : 0;
            do_record(&event);
        }
    }
    else {
        scancode = keycode_to_scancode (HIWORD (kbd_data.unicode) & 0x00FF, 
                    LOWORD (kbd_data.unicode));
        kbd_state [scancode] = (press) ? 1 : 0;
        last = scancode;
        if (record_fd >= 0){
            AUTO_IAL_INPUT_EVENT event;
            event.timestamp = getcurtime() - record_time_start;
            event.type = 0;
            event.u.key_event.scancode = scancode;
            event.u.key_event.type = press ? 1 : 0;
            do_record(&event);
        }
    }

    nr_changed_keys = last + 1;
    return 1;
}

static int keyboard_update (void)
{
    if (read_key())
    {
        return nr_changed_keys;
    }
    else
    {
        return 0;
    }
}

static const char* keyboard_getstate (void)
{
    return (char*)kbd_state;
}

/* NOTE by weiym: Do not ignore the fd_set in, out, and except */
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    fd_set rfds;
    int    retvalue = 0;
    int    fd, e;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if (which & IAL_MOUSEEVENT && mouse_fd >= 0) {
        fd = mouse_fd;
        FD_SET (fd, in);
#ifndef _MGRM_THREADS
        if (fd > maxfd) maxfd = fd;
#endif
    }

    if (which & IAL_KEYEVENT && kbd_fd >= 0) {
        fd = kbd_fd;
        FD_SET (kbd_fd, in);
#ifndef _MGRM_THREADS
        if (fd > maxfd) maxfd = fd;
#endif
    }

    /* FIXME: pass the real set size */
#ifndef _MGRM_THREADS
    e = select (maxfd + 1, in, out, except, timeout) ;
#else
    e = select (FD_SETSIZE, in, out, except, timeout) ;
#endif

    if (e > 0) { 
        fd = mouse_fd;
        /* If data is present on the mouse fd, service it: */
        if (fd >= 0 && FD_ISSET (fd, in)) {
            FD_CLR (fd, in);
            retvalue |= IAL_MOUSEEVENT;
        }

        fd = kbd_fd;
        /* If data is present on the keyboard fd, service it: */
        if (fd >= 0 && FD_ISSET (fd, in)) {
            FD_CLR (fd, in);
            retvalue |= IAL_KEYEVENT;
        }

    } else if (e < 0) {
        return -1;
    }

    return retvalue;
}

BOOL InitQVFBInput (INPUT* input, const char* mdev, const char* mtype)
{
    char file [50];
    int display;

   /* get display number */
    if (GetMgEtcIntValue ("qvfb", "display", &display) < 0)
        return FALSE;

    /* open mouse pipe */
    sprintf (file, QT_VFB_MOUSE_PIPE, display);
    if ((mouse_fd = open (file, O_RDONLY)) < 0) {
        fprintf (stderr, "QVFB IAL engine: can not open mouse pipe.\n");
        return FALSE;
    }
    /* open keyboard pipe */
    sprintf (file, QT_VFB_KEYBOARD_PIPE, display);
    if ((kbd_fd = open (file, O_RDONLY)) < 0) {
        fprintf (stderr, "QVFB IAL engine: can not open keyboard pipe.\n");
        return FALSE;
    }

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;
    input->suspend_mouse= NULL;
    input->resume_mouse = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->suspend_keyboard = NULL;
    input->resume_keyboard = NULL;
    input->set_leds = NULL;

    input->wait_event = wait_event;

    init_code_map ();

    {
        char path[PATH_MAX], *template;
        template = getenv("MG_ENV_RECORD_IAL");
        if (template)
        {
#if 0
            snprintf(path, sizeof(path), "%s%ld", template, time(NULL));
#else
            snprintf(path, sizeof(path), "%s", template);
#endif
            path[sizeof(path)/sizeof(path[0])-1] = 0;
            record_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            record_time_start = getcurtime();
        }
    }

    return TRUE;
}

void TermQVFBInput (void)
{
    if (mouse_fd >= 0)
        close (mouse_fd);
    if (kbd_fd >= 0)
        close (kbd_fd);
}

#endif /* _MGIAL_QVFB */

