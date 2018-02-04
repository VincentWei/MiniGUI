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
#define dbg() printf("%s %d\n", __FUNCTION__, __LINE__)

/*
** cisco_touchpad.c: Input Engine for Cisco touchpad 
*/

#include "common.h"
#ifdef _MGIAL_CISCO_TOUCHPAD

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/ioctl.h> 
#include <errno.h>
#include "mgsock.h"
#include "minigui.h"
#include "ial.h" 
#include "native/native.h"
#include "cisco_touchpad.h"
#include "window.h"
#include "cliprect.h"
#include "../include/internals.h"

typedef unsigned int __u32;
typedef int __s32;
typedef short __s16;

#define NOBUTTON         0x0000
#define LEFTBUTTON       0x0001
#define MOUSEBUTTONMASK  0x00FF

#define PATHLEN          3
#define SEND_PACKET_LEN  6
#define RECV_PACKET_LEN  6

static int    xpos;        /* current x position of mouse */
static int    ypos;        /* current y position of mouse */
static int    minx;        /* minimum allowed x position */
static int    maxx;        /* maximum allowed x position */
static int    miny;        /* minimum allowed y position */
static int    maxy;        /* maximum allowed y position */
static KBDDEVICE * kbddev = &kbddev_event;
static MOUSEDEVICE * mousedev = &mousedev_IMPS2;
static int    scale;       /* acceleration scale factor */
static int    thresh;      /* acceleration threshhold */
static int buttons = 0; 
static int s_current_mode = -1; 
static int s_expect_mode = MICE_MODE;
static int kbd_fd = -1;
static int mouse_fd = -1;
static int app_fd = -1;
static char state[NR_KEYS];
static int s_enable_appmode;

void mg_ial_ioctl(unsigned int cmd, unsigned int value)
{ 
    if (s_enable_appmode) {
        if (cmd == CMD_SET_MODE) {
            switch (value) {
                case MICE_MODE:
                case APP_MODE:
                    s_expect_mode = value;
                    printf("expect mode=%d\n", value);
                    break;
                default:
                    fprintf(stderr, "unknown mode=%d\n", value);
            }
        }else{
            fprintf(stderr, "unknown cmd=%d\n", cmd);
        }
    }
}

static int touchpad_set_mode(int mode) {
    int ret = 0;
    unsigned char packet[] = { 0x00, 0x04,0x81,0x00 }; 
    if(mode == MICE_MODE) {
        packet[sizeof(packet)-1] = 0x00;
        ret = write(app_fd, packet, sizeof(packet));
    } else { // APP_MODE
        memset(state, 0, NR_KEYS);
        packet[sizeof(packet)-1] = 0x03;
        ret = write(app_fd, packet, sizeof(packet));
    } 
    if (ret < 0) {
        perror("write()");
    }
    /* printf("set to mode[%d], ret=%d\n", mode, ret); */
    return 0;
}

/************************  Low Level Input Operations **********************/ 
/* * Mouse operations -- Event */ 
static int mouse_update (void)
{
    if(MICE_MODE == s_current_mode) {
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
    return 0;
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

static int keyboard_update (void)
{
    int retvalue = 0;
    if(MICE_MODE == s_current_mode) { 
        unsigned char buf;
        int modifier;
        int ch;
        int is_pressed;

        retvalue = kbddev->Read (&buf, &modifier); 
        if ((retvalue == -1) || (retvalue == 0)) {
            return 0;
        } else { /* retvalue > 0 */
            is_pressed = !(buf & 0x80);
            ch         = buf & 0x7f;
            if (is_pressed) {
                state[ch] = 1;
            }
            else {
                state[ch] = 0;
            }
            // printf("is_pressed=%d, ch=%d, state[%d]=%d\n", is_pressed, ch, ch, state[ch]);
        } 
        return NR_KEYS;
    } else if(APP_MODE == s_current_mode) {
        unsigned char buf[16];
        static int last_key;
        if (read(app_fd, buf, sizeof(buf)) > 5) {
            if ((buf[1] & 0x80)) { /* HOME key pressed */
                mg_ial_ioctl(CMD_SET_MODE, MICE_MODE);
                if (last_key) {
                    /* make the last_key up */
                    printf("key up %d\n", last_key);
                    if (__mg_ime_wnd) {
                        SendNotifyMessage (__mg_ime_wnd, MSG_KEYUP, (WPARAM)last_key, 0);
                    }
                }
                /* notify the ime to hide */
                printf("key down %d\n", SCANCODE_HOME);
                if (__mg_ime_wnd) {
                    SendNotifyMessage (__mg_ime_wnd, MSG_KEYDOWN, (WPARAM)SCANCODE_HOME, 0);
                }
                last_key = 0;
            }else{
                if (buf[0] != last_key) {
                    if (buf[0]){
                        printf("key down %d\n", buf[0]);
                        if (__mg_ime_wnd) {
                            SendNotifyMessage (__mg_ime_wnd, MSG_KEYDOWN, (WPARAM)buf[0], 0);
                        }
                    }else{
                        printf("key up %d\n", last_key);
                        if (__mg_ime_wnd) {
                            SendNotifyMessage (__mg_ime_wnd, MSG_KEYUP, (WPARAM)last_key, 0);
                        }
                    }
                    last_key = buf[0];
                }
            }
        }
    }
    return 0;
} 

static const char* keyboard_getstate (void)
{
    return state;
}

static void appmode_sendkey(int status, int code) {
    printf("key %s %d\n", status == MSG_KEYUP ? "up" : "down", code);
    if (__mg_ime_wnd) {
        SendNotifyMessage(__mg_ime_wnd, status, code, 0);
    }
}

static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    int    retvalue = 0;
    fd_set rfds;
    int    fd, e;

    if (!in) {
        in = &rfds;
        mg_fd_zero (in);
    }

    if (s_enable_appmode) {
        if (s_current_mode != s_expect_mode) {
            printf("mode current=%d,expect=%d\n", s_current_mode, s_expect_mode);
            if (touchpad_set_mode(s_expect_mode) < 0) {
                fprintf(stderr, "Set mode[%d] failed.\n", s_expect_mode);
            }else{
                s_current_mode = s_expect_mode;
            }
        }

        static unsigned int last_time;
        unsigned int now = GetTickCount();
        if (now < last_time - 150 || now > last_time + 150) {
            touchpad_set_mode(s_current_mode);
            last_time = now;
        }
    }

    if( MICE_MODE == s_current_mode) { 
        if (which & IAL_MOUSEEVENT && mouse_fd >= 0) {
            fd = mouse_fd;
            mg_fd_set (fd, in);
            if (fd > maxfd) maxfd = fd;
        }

        if (which & IAL_KEYEVENT && kbd_fd >= 0) {
            fd = kbd_fd;
            mg_fd_set (fd, in);
            if (fd > maxfd) maxfd = fd;
        }

        e = mg_select (maxfd + 1, in, out, except, timeout) ;
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
    } else { 
        static int s_app_key;
        static int s_app_key_status = -1; /* -1: empty, 0:up, but not send to app yet, 1:down */
        unsigned char buf[16];
        if ((which & IAL_KEYEVENT) && app_fd >= 0) {
            mg_fd_set (app_fd, in);
            if (app_fd > maxfd) maxfd = app_fd;
        }

        e = mg_select (maxfd + 1, in, out, except, timeout) ;
        if (e > 0) {
            if (FD_ISSET(app_fd, in) && read(app_fd, buf, sizeof(buf)) > 5) {
                printf("%02x %02x %02x\n", buf[0], buf[1], buf[2]);
                if ((buf[1] & 0x80)) { /* HOME key pressed */
                    mg_ial_ioctl(CMD_SET_MODE, MICE_MODE);
                    if (s_app_key_status >= 0) {
                        /* make the last_key up */
                        appmode_sendkey(MSG_KEYUP, s_app_key);
                    }
                    /* notify the ime to hide */
                    appmode_sendkey(MSG_KEYDOWN, SCANCODE_HOME);

                    s_app_key = -1;
                    s_app_key_status = -1;
                }else if (buf[0] != 0) { /* Key pressed */
                    if (s_app_key_status == -1 || s_app_key != buf[0]){
                        s_app_key = buf[0];
                        s_app_key_status = 1;
                        appmode_sendkey(MSG_KEYDOWN, s_app_key);
                    }
                }else{ /* Key released */
                    if (s_app_key_status > 0) {
                        s_app_key_status = 0;
                        /* don't send immediately */
                    }else if (s_app_key_status == 0) {
                        s_app_key_status = -1;
                        appmode_sendkey(MSG_KEYUP, s_app_key);
                    }else{
                        /* ignore */
                    }
                }
            }
        }else if (e < 0) { 
            return -1;
        }else{ /* timedout */
            if (s_app_key_status == 0) {
                s_app_key_status = -1;
                appmode_sendkey(MSG_KEYUP, s_app_key);
            }
        }
        return 0;
    } 
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

BOOL InitCiscoTouchpadInput (INPUT* input, const char* mdev, const char* mtype)
{
    {
#       define KBD "/dev/input/event0"
        char keyboard_device[] = KBD;
        system("mknod "KBD" c 13 64 > /dev/null 2>&1");
        kbd_fd = keyboard_open(keyboard_device);
        if (kbd_fd < 0) {
            fprintf (stderr, "[WARNING] IAL Native Engine: Can not init keyboard right now, %s(%s).\nPlease plug in the KEYBOARD!\n", 
                    strerror(errno), keyboard_device);
        }
    }

    mouse_fd = mouse_open(mdev);
    if (mouse_fd <0) {
        fprintf (stderr, "[WARNING] IAL Native Engine: Can not init mouse right now, %s(%s).\nPlease plug in the MOUSE!\n", 
                strerror(errno), mdev);
    }

    if (getenv("MG_ENV_DISABLE_APP_MODE")) {
        s_enable_appmode = 0;
    }else{
        s_enable_appmode = 1;
    }

    if (s_enable_appmode) {
        system("mknod /dev/hidraw0 c 253 0 > /dev/null 2>&1");
        system("mknod /dev/hidraw1 c 253 1 > /dev/null 2>&1");
#       define HIDRAW "/dev/hidraw1"
        app_fd = open(HIDRAW, O_RDWR);
        if (app_fd < 0) {
            fprintf(stderr, "Fatal error> Failed to open %s: [errno=%d]%s", HIDRAW, errno, strerror(errno));
            return -1;
        }

        mg_ial_ioctl(CMD_SET_MODE, MICE_MODE);
        // mg_ial_ioctl(CMD_SET_MODE, APP_MODE);
    }else{
        s_current_mode = s_expect_mode = MICE_MODE;
    }

    xpos = 0;
    ypos = 0;
    buttons = 0;
    minx = 0;
    miny = 0;
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
    input->set_leds = NULL;

    input->wait_event = wait_event; 
    return TRUE;
}

void TermCiscoTouchpadInput (void)
{ 
    if (mousedev)
        mousedev->Close();

    kbddev->Close();

    if (app_fd >= 0)
    {
        close (app_fd);
        app_fd = -1;
    }
}
#endif // _MGIAL_CISCO_TOUCHPAD
