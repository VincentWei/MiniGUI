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
** jz4740.c: Low Level Input Engine for HH2410-R3, HHARM9-LCD-R4 Dev Boards.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "common.h"

#ifdef _MGIAL_LIDE

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kd.h>

#include "ial.h"
#include "lide.h"

#define DIS_NUM 3

#define MINX 116
#define MAXX 3974

#define MINY 240
#define MAXY 3980

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480

static POINT src_pts[5]={{47,684},{422,688},{421,107},{46,118},{235,401}};
static POINT dst_pts[5]={{100,60},{699,60},{699,419},{100,419},{400,240}};

#define KBD_DEVICE  "/dev/input/mice"
#define TS_DEVICE  "/dev/ts"
/* for data reading from /dev/ts */
typedef struct {
  	struct timeval time;
	unsigned short type;
	unsigned short code;
    unsigned int  value;
} KBD_EVENT;


static int btn_fd = -1;
static unsigned char state[NR_KEYS];
static unsigned char btn_state=0;
static unsigned short btn_key=0;
static unsigned short btn_type=0;
static KBD_EVENT kbd_event;
static unsigned char keycode_scancode[MAX_KEYPAD_CODE + 1];
//static int numlock = 0;


//#define _DEBUG
#undef _DEBUG
//static struct tsdev *ts = NULL;
int touch_fd = -1;
static int mousex = 0;
static int mousey = 0;
static int button = 0;
static int nr_events = 0;

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update(void)
{
    return 1;
}

typedef struct {
	unsigned short status;
	unsigned short x;
	unsigned short y;
	unsigned short pressure;
	unsigned short pad;
} jz_ts_event; 

static  jz_ts_event ts_events[DIS_NUM];
static jz_ts_event ts_event;

static void mouse_getxy(int *y, int* x)
{
    int i;

    int max_x = ts_events [0].x;
    int min_x = ts_events [0].x;
    int max_event = 0, min_event = 0;
    int sum_x = 0, sum_y = 0;

#ifdef _DEBUG
    fprintf (stderr, "mousex = %d, mousey = %d\n", mousex, mousey);
#endif

#if 0

    *x = mousex;
    *y = mousey;
#endif
    if (nr_events != DIS_NUM) {
        *x = mousex;
        *y = mousey;
        return;
    }

    int tempx, tempy;
#ifdef _DEBUG
    fprintf(stderr, "nr_events%d\n", nr_events);
#endif
	for (i = 0; i < nr_events; i++) 
    {
        tempx =  (ts_events [i].x - MINX)*SCREEN_WIDTH/(MAXX-MINX);
        tempy =  (ts_events [i].y - MINY)*SCREEN_HEIGHT/(MAXY-MINY);
        
		if (tempx < 0) tempx = 0;
		if (tempy < 0) tempy = 0;

		ts_events [i].x = tempx;
		ts_events [i].y = tempy;

		if (ts_events [i].x > SCREEN_WIDTH-1) ts_events [i].x = SCREEN_WIDTH-1;
		
        if (ts_events [i].y > SCREEN_HEIGHT-1) ts_events [i].y = SCREEN_HEIGHT;

		if (ts_events [i].x > max_x) {
			max_x = ts_events [i].x;
			max_event = i;
		}

		if (ts_events [i].x < min_x) {
			min_x = ts_events [i].x;
			min_event = i;
		}
	}

    for (i = 0; i < nr_events; i++) {
        if (i != max_event && i != min_event) {
            sum_x += ts_events [i].x;
            sum_y += ts_events [i].y;
        }
    }
#ifdef _DEBUG
    fprintf(stderr, "max_event %d, min_event %d, minx=%d, miny=%d\n", max_event, min_event, ts_events[min_event].x, ts_events[min_event].y);
#endif
	if (max_event == min_event ) {
		*x = sum_x / (nr_events - 1);
		*y = sum_y / (nr_events - 1);
	}
	else {
		*x = sum_x / (nr_events - 2);
		*y = sum_y / (nr_events - 2);
	}
    if (ABS (mousex - *x) > 10 || ABS (mousey - *y) > 10) 
    {
        mousex = *x;
        mousey = *y;
    }
    *x = mousex;
    *y = mousey;
    nr_events = 0;

   // *x = mousex;
   // *y = mousey;
}

static int mouse_getbutton(void)
{
    return button;
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
    int    fd;
    int    retvalue = 0;
    int    ret = 0;
    int    e;
	static jz_ts_event jz_evt;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }


    if ((which & IAL_MOUSEEVENT) && touch_fd >= 0) {
        fd = touch_fd;
        FD_SET (fd, in);
#ifdef _LITE_VERSION
        if (fd > maxfd) maxfd = fd;
#endif
    }
    if (which & IAL_KEYEVENT && btn_fd >= 0){
        fd = btn_fd;          /* FIXME: keyboard fd may be changed in vt switch! */
        FD_SET (fd, in);
       
#ifdef _LITE_VERSION
        if (fd > maxfd) maxfd = fd;
#endif
    }
#ifdef _LITE_VERSION
    e = select (maxfd + 1, in, out, except, timeout) ;
#else
    e = select (FD_SETSIZE, in, out, except, timeout) ;
#endif

    if (e > 0 ) 
	{
       fd = touch_fd; 
        if (fd >= 0 && FD_ISSET (fd, in)) 
        {
            FD_CLR (fd, in);
            
	        retvalue = read(fd, &jz_evt, sizeof(jz_evt));
            
            if (retvalue > 0) {
                if (jz_evt.pressure > 0)
                {
                    ts_events [nr_events] = jz_evt;
                    nr_events ++;
                      if (nr_events >= DIS_NUM) 
                      {
                        button = 5;
                      }
                 }
                 else
                 {
				    nr_events = 0;
                    button = 0;
                 }
#ifdef _DEBUG
                {
                    fprintf (stderr, "mouse event:  button %d, pressure: %d  x= %d, y= %d\n", button, jz_evt.pressure, jz_evt.x, jz_evt.y);
                }
#endif
                ret |= IAL_MOUSEEVENT;
                
            }
        }
    }
    else
        {
            return -1;
        }


   return ret;
}

static void keycodetoscancode(void)
{
    int i = 0;
    for(i=0; i < NR_KEYS; i++)
        state[i] =0 ;
   keycode_scancode[KEY_1] = SCANCODE_HOME;
#if 1
   keycode_scancode[KEY_2] = SCANCODE_HOME + 1;
   keycode_scancode[KEY_3] = SCANCODE_HOME + 2;
   keycode_scancode[KEY_4] = SCANCODE_HOME + 3;
   keycode_scancode[KEY_5] = SCANCODE_HOME + 4;
   keycode_scancode[KEY_6] = SCANCODE_HOME + 5;
   keycode_scancode[KEY_7] = SCANCODE_HOME + 6;
#endif   
}


/*
 * 0 : no data
 * > 0 : the possible maximal key scancode
 */
static int keyboard_update(void)
{
    state[keycode_scancode[btn_key]] = btn_state;
    fprintf(stderr, "keyboard_update,btn_key=%d, KEY_1=%d, btn_state=%d, scancode=%d\n", btn_key, KEY_1, btn_state, keycode_scancode[btn_key]);
    return NR_KEYS;

}

static const char* keyboard_getstate(void)
{
   //fprintf(stderr, "keyboard_getstate\n");
    return (char*)state;
}

void rkbd(int sig)
{
    printf("keyboard even t\n");
}

BOOL InitlideInput(INPUT* input, const char* mdev, const char* mtype)
{
    btn_fd = -1;
    
    touch_fd = open (TS_DEVICE, O_NONBLOCK);

    if (!touch_fd) {
        _MG_PRINTF ("IAL>TSLib: can not open ts device\n");
        return FALSE;
    }
    
 
    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->wait_event = wait_event;
    mousex = 0;
    mousey = 0;
    
    ts_event.x = ts_event.y = ts_event.pressure =0;
    SetMouseCalibrationParameters (src_pts, dst_pts);

    return TRUE;
}

void TermlideInput(void)
{
    if (touch_fd >= 0)
        close(touch_fd);
    if (btn_fd >= 0)
        close(btn_fd);    
}

#endif /* _JZ4740_IAL */ 
