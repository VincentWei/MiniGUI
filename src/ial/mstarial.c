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
#define ENTER()    printf(">>> entering %s %s\n", __FILE__, __FUNCTION__)
#define LEAVE()    printf("<<< leaving  %s %s\n", __FILE__, __FUNCTION__)
#define dbg() printf("%s %d\n", __FUNCTION__, __LINE__)

/*
** cisco_touchpad.c: Input Engine for Cisco touchpad 
*/

#include "common.h"
#ifdef _MGIAL_MSTAR

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
#include "window.h"
#include "cliprect.h"
#include "../include/internals.h"


#include <dirent.h>
#include <ctype.h> 
#include <sys/types.h>
#include <signal.h>
#include <linux/vt.h>
#include <linux/kd.h>
#include <linux/keyboard.h> 
#include <madp.h>
#include <madp/madp_ir.h>

#define NOBUTTON         0x0000
#define LEFTBUTTON       0x0001
#define MOUSEBUTTONMASK  0x00FF 

#define MOUSE_MOVE_PIXEL    50
#define MOUSE_MAX_X         1920 
#define MOUSE_MAX_Y         1080 

static unsigned short xiKeyValue[256] = 
{ /*      0          1        2             3           4          5          6          7          8           9 */ 
/*0*/   0x0030,    0x0031,    0x0032,    0x0033,        0x0034,    0x0035,    0x0036,    0x0037,    0x0038,    0x0039,
/*10*/  '*',       0x0122,    0x001B,    SCANCODE_F1,   0x0081,    0x0009,    0x0043,SCANCODE_SLASH,0x0111,    0x0112, 
/*20*/  0x0113,    0x0114,    0x008b,    SCANCODE_F2, SCANCODE_F3, 0x008e,SCANCODE_F4,   0x0123,    0x001B,    0x0091, 
/*30*/  0x0092,    SCANCODE_F5,0x0039,    0x0047,        0x0048,    0x0049,    0x004a,    0x0025,    0x0026,    0x0028,  
/*40*/  0x0028,    0x0029,    0x002a,    0x002b,        0x0033,    0x000c,    0x0034,    0x0035,    0x000b,    0x0002,  
/*50*/  0x0003,    0x0004,    0x0005,    0x0006,        0x0007,    0x0008,    0x0009,    0x000a,    0x000b,    0x0027,
/*60*/  0x003c,    0x000d,    0x003e,    0x003f,        0x0127, 0x0128,SCANCODE_CURSORUP,SCANCODE_CURSORDOWN,SCANCODE_CURSORLEFT,SCANCODE_CURSORRIGHT, 
/*70*/  SCANCODE_ENTER, SCANCODE_HOME,SCANCODE_END,0x0049,        0x0125,    0x0126,    0x004c,    0x004d, SCANCODE_F7,  0x004f,
/*80*/ SCANCODE_F6,0x0051, SCANCODE_F8,  0x0124,        0x0054,    0x0055,    0x0056,    0x0057,    0x0058,    0x0059,
/*90*/  0x005a,    0x001a,    0x002b,    0x001b,        0x005e,    0x005f,    0x0060,    0x001e,    0x0030,    0x002e,
/*100*/ 0x0020,    0x0012,    0x0021,    0x0022,        0x0023,    0x0017,    0x0024,    0x0025,    0x0026,    0x0032,
/*110*/ 0x0031,    0x0018,    0x0019,    0x0010,        0x0013,    0x001f,    0x0014,    0x0016,    0x002f,    0x0011,
/*120*/ 0x002d,    0x0015,    0x002c,    0x007b,        0x007c,    0x007d,    0x007e,    0x006f,    0x003b,    0x003c,
/*130*/ 0x003d,    0x003e,    0x003f,    0x0040,        0x0044,    0x0042,    0x0043,    0x0044,    0x0057,    0x0058,
/*140*/ 0x008c,    0x001d,    0x008e,    0x0038,        0x0036,    0x002a,    0x0092,    0x007f,    0x007d,    0x0095,
/*150*/ 0x003a,    0x0097,    0x006d,    0x0068,        0x0066,    0x006b,    0x009c,    0x0063,    0x006c,    0x0067,
/*160*/ 0x0069,    0x006a,    0x006e,    0x003f,        0x0092,    0x00a5,    0x00a6,    0x00a7,    0x00a8,    0x00a9,
/*170*/ 0x00aa,    0x00ab,    0x00ac,    0x00ad,        0x000e,    0x00af,    0x00b0,    0x00b1,    0x00b2,    0x00b3,
/*180*/ 0x00b4,    0x00b5,    0x00b6,    0x00b7,        0x00b8,    0x00b9,    0x00ba,    0x00bb,    0x00bc,    0x00bd,
/*190*/ 0x00be,    0x00bf,    0x00c0,    0x00c1,        0x00c2,    0x00c3,    0x00c4,    0x00c5,    0x00c6,    0x00c7,
/*200*/ 0x00c8,    0x00c9,    0x00ca,    0x00cb,        0x00cc,    0x00cd,    0x00ce,    0x00cf,    0x00d0,    0x00d1,
/*210*/ 0x00d2,    0x00d3,    0x00d4,    0x00d5,        0x00d6,    0x00d7,    0x00d8,    0x00d9,    0x00da,    0x00db,
/*220*/ 0x00dc,    0x00dd,    0x00de,    0x00df,        0x00e0,    0x00e1,    0x00e2,    0x00e3,    0x00e4,    0x00e5,
/*230*/ 0x00e6,    0x00e7,    0x00e8,    0x00e9,        0x00ea,    0x00eb,    0x00ec,    0x00ed,    0x00ee,    0x00ef,
/*240*/ 0x00f0,    0x00f1,    0x00f2,    0x00f3,        0x00f4,    0x00f5,    0x00f6,    0x00f7,    0x00f8,    0x00f9,
/*250*/ 0x00fa,    0x00fb,    0x00fc,    0x00fd,        0x00fe,    0x00ff
}; 
static unsigned int cur_key;
static char state[NR_KEYS]; 
static int    xpos;        /* current x position of mouse */
static int    ypos;        /* current y position of mouse */
static int    minx;        
static int    maxx;        
static int    miny;       
static int    maxy;       
static int buttons = 0; 
static int ir_fd = -1;

/************************  Low Level Input Operations **********************/ 
static int mouse_update (void)
{
    return 1;
}

static void mouse_getxy (int* x, int* y)
{
    *x = xpos;
    *y = ypos;
}

static int mouse_getbutton (void)
{
    return buttons;
}

static int keyboard_update (void)
{
    //ENTER();
    if (cur_key == 0) {
        memset (state, 0, sizeof(state));
    } else {
        state [cur_key] = 1;
    }
    //LEAVE();
    return NR_KEYS;
}

static const char * keyboard_get_state(void)
{
    return (char *)state;
}

/* NOTE by weiym: Do not ignore the fd_set in, out, and except */
#ifdef _LITE_VERSION
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#endif /* _LITE_VERSION */ 
{
    int e, fd;
    fd_set rfds;
    if (cur_key || buttons) {

        if (cur_key) {
            cur_key = 0;
            e = IAL_KEYEVENT;
        }

        if (buttons) {
            buttons = 0;
            e |= IAL_MOUSEEVENT;
        } 
        return e;
    }

    if (!in) {
        in = &rfds;
        mg_fd_zero (in);
    }
    fd = ir_fd;
    mg_fd_set (fd, in);
#ifdef _LITE_VERSION
    e = select (maxfd + 1, in, out, except, timeout);
#else
    e = select (FD_SETSIZE, in, out, except, timeout); 
#endif
    if(e > 0) {
        //printf("e > 0, key pressed\n");
        fd = ir_fd;
        if (fd >= 0 && FD_ISSET (fd, in)) {
            FD_CLR (fd, in);
        }

        int rawcode;
        int read_len = read (ir_fd, &rawcode, sizeof(unsigned int));
        if (read_len == sizeof(unsigned int)) {
            unsigned char code = (rawcode& 0x0000FF00) >> 8;	
            int key = xiKeyValue[code%MGUI_NR_KEYS];
            printf("read results: rawcode = %d, code = %d, key = 0x%x\n", rawcode ,code, key);
            switch (key) {
                case SCANCODE_CURSORLEFT: 
                    printf("SCANCODE_CURSORLEFT\n");
                    xpos -= MOUSE_MOVE_PIXEL;
                    if (xpos < 0)
                        xpos = 0;
                    return IAL_MOUSEEVENT;

                case SCANCODE_CURSORRIGHT:
                    printf("SCANCODE_CURSORRIGHT\n");
                    xpos += MOUSE_MOVE_PIXEL;
                    if (xpos > MOUSE_MAX_X)
                        xpos = MOUSE_MAX_X;
                    return IAL_MOUSEEVENT;

                case SCANCODE_CURSORUP:
                    ypos -= MOUSE_MOVE_PIXEL;
                    if (ypos < 0)
                        ypos = 0;
                    return IAL_MOUSEEVENT;

                case SCANCODE_CURSORDOWN:
                    ypos += MOUSE_MOVE_PIXEL;
                    if (ypos > MOUSE_MAX_Y)
                        ypos = MOUSE_MAX_Y;
                    return IAL_MOUSEEVENT;

                case SCANCODE_ENTER:
                    printf("SCANCODE_ENTER\n");
                    buttons = IAL_MOUSE_LEFTBUTTON;
                    return IAL_MOUSEEVENT;

                default:
                    cur_key = key;
                    return IAL_KEYEVENT;
            }
        } 
    }
    return 0;
}

BOOL InitMStarInput (INPUT* input, const char* mdev, const char* mtype)
{
#ifndef USE_MSTAR_CHAKRA
    //ENTER();
    int flag = 1; 
    ir_fd = open ("/dev/ir", O_RDWR);
    
    if ( ir_fd < 0 ) {
        fprintf (stderr, "IAL: Can not open xilleon ir device!\n");
        return -1;
    }   
    int masterIrPid;
    ioctl(ir_fd, MDRV_IR_GET_MASTER_PID, &masterIrPid);
    if (masterIrPid > 0) { 
        printf("kill master Ir Pid\n");
        kill(masterIrPid, 0);
    }

    masterIrPid = getpid();
    ioctl(ir_fd, MDRV_IR_INIT);
    ioctl(ir_fd, MDRV_IR_SET_MASTER_PID, &masterIrPid);
    ioctl(ir_fd, MDRV_IR_ENABLE_IR, &flag);

#else
    ir_fd = 1;
#endif

    xpos = 0;
    ypos = 0;
    buttons = 0;
    minx = 0;
    miny = 0;
    maxx = WIDTHOFPHYSCREEN;
    maxy = HEIGHTOFPHYSCREEN;

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;
    input->suspend_mouse= NULL;
    input->resume_mouse = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_get_state;
    input->set_leds = NULL;

    input->wait_event = wait_event; 
    //LEAVE();

    return TRUE;
}

void TermMStarInput (void)
{
    ENTER();
#ifndef USE_MSTAR_CHAKRA
    if ( ir_fd >= 0 )
        close(ir_fd);
#endif
    ir_fd = -1;
    LEAVE();
} 
#endif // _MGIAL_MSTAR
