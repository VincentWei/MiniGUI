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
** thor.c: Low Level Input Engine for ptsc MX1
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "common.h"

#ifdef _THOR_IAL

#ifdef USE_KEYPAD
#include "minigui.h"
#endif

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <linux/kd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "misc.h"
#include "ial.h"
#include "ipaq.h"

#define USE_KEYPAD 1

/* for data reading from /dev/hs3600_ts */
typedef struct {
    unsigned short b;
    unsigned short x;
    unsigned short y;
    unsigned short pad;
} POS;

static int ts = -1;
static int pad = -1;
static int mousex = 0;
static int mousey = 0;
static POS pos;

static float AX, AY, BX, BY;

static unsigned char state [NR_KEYS];
/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update(void)
{
    return 1;
}

static void mouse_getxy(int *x, int* y)
{
	if (mousex < 0) mousex = 0;
	if (mousey < 0) mousey = 0;
	if (mousex > 239) mousex = 239;
	if (mousey > 319) mousey = 319;

    *x = mousex;
    *y = mousey;
}

static int mouse_getbutton(void)
{
    return pos.b;
}

static int keyboard_update(void)
{
    unsigned char buf;
    //int modifier;
    int ch;
    int is_pressed;
    int retvalue;

    retvalue = read_keypad(&buf);
    
    if ((retvalue == -1) || (retvalue == 0))
        return 0;
    else { /* retvalue > 0 */
        is_pressed = !(buf & 0x80);
        ch         =   buf & 0x7f;
        if (is_pressed) {
            state[ch] = 1;
        }
        else 
            state[ch] = 0;
    }

    return NR_KEYS;

}

static const char* keyboard_getstate(void)
{
    return state;
}


#ifdef _LITE_VERSION 
#if 0
int IsSysPadKey(unsigned char scancode) {
	if ((scancode == SCANCODE_PHONE) || (scancode == SCANCODE_HANGUP))
		return 1;
	else
		return 0;
}

void FillSKBDBuffer(unsigned char ch) {
	*_input_buff_writepointer++ = ch;
	if ((_input_buff_writepointer - _input_buff) >= INPUTBUFF_LEN)
		_input_buff_writepointer = _input_buff;
}
#endif
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    fd_set rfds;
    int fd, e;
    int retvalue = 0;
    
    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if ((which & IAL_MOUSEEVENT) && ts >= 0) {
        FD_SET (ts, in);
        if (ts > maxfd) maxfd = ts;
    }
    
#ifdef USE_KEYPAD    
    if (which & IAL_KEYEVENT && pad >= 0){
        FD_SET (pad, in);
        if (pad > maxfd) maxfd = pad;
    }
#endif
    
    e = select (maxfd + 1, in, out, except, timeout) ;

    if (e > 0) { 
        if (ts >= 0 && FD_ISSET (ts, in))
        {
            FD_CLR (ts, in);
	    
            pos.x=-1;
            pos.y=-1;
            // FIXME: maybe failed due to the struct alignment.
            read (ts, &pos, sizeof (POS));    
            if (pos.x != 0 && pos.y != 0) {
    	    	//CASANOVA
	        	float tmpx, tmpy;
		
        		tmpx = pos.x;
		        tmpy = pos.y;
		
                tmpx /= AX;
        		tmpy /= AY;
				
		        tmpx -= BX;		
        		tmpy -= BY;
		
		        pos.x = (unsigned short)(tmpx < 0?0:tmpx);
        		pos.y = (unsigned short)(tmpy < 0?0:tmpy);
		    
                mousex = pos.x;
                mousey = pos.y;
            }
            pos.b = ( pos.b > 0 ? IAL_MOUSE_LEFTBUTTON:0);
            retvalue |= IAL_MOUSEEVENT;
        }

#ifdef USE_KEYPAD    
        if (pad >= 0 && FD_ISSET(pad, in)) {
            
            FD_CLR(pad, in);
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
    struct pollfd ufd;
    if ( (which & IAL_MOUSEEVENT) == IAL_MOUSEEVENT)
    {
        ufd.fd     = ts;
        ufd.events = POLLIN; 
        if ( poll (&ufd, 1, timeout) > 0)
        {
            // FIXME: maybe failed due to the struct alignment.
            read (ts, &pos, sizeof(POS));    
            return IAL_MOUSEEVENT;
        }
    }
    return 0;
}
#endif

BOOL InitTHORInput (INPUT* input, const char* mdev, const char* mtype)
{
    FILE* fp;

    ts = open ("/dev/h3600_ts", O_RDONLY);
    if ( ts < 0 ) {
        fprintf (stderr, "IAL: Can not open touch screen!\n");
        return FALSE;
    }

    fp = fopen("/tmp/calibrate.conf", "r");
    if (fp == NULL) {
	fprintf (stderr, "IAL: Can not read calibrate conf!\n");
	return FALSE;
    }
    fscanf(fp, "%f %f %f %f", &AX, &AY, &BX, &BY);    
    printf("DEBUG: AX=%f AY=%f BX=%f BY=%f\n", AX, AY, BX, BY);
    fclose(fp);
 
#ifdef USE_KEYPAD    
    pad = open ("/tmp/pad", O_RDONLY);
    if ( pad < 0 ) {
        pad = open ("/tmp/key", O_RDONLY);
        if ( pad < 0 ) {
            pad = open ("/dev/Thor_keypad", O_RDONLY);
            if ( pad < 0 ) {
                fprintf (stderr, "IAL: Can not open key pad device!\n");
                return FALSE;
            }
        }
    }
#else
    pad = -1;
#endif
     
    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->set_leds = NULL;
    input->suspend_keyboard = NULL;
    input->resume_keyboard = NULL;


    input->wait_event = wait_event;
    mousex = 0;
    mousey = 0;
    pos.x=pos.y=pos.b=0;
    
//#ifdef _LITE_VERSION
//    if (mgIsServer)
//        init_vtswitch (kbd_fd);
//#endif

   
    return TRUE;
}

void TermTHORInput (void)
{
    if ( ts >= 0 )
        close(ts);
}

static unsigned char KEYPAD_SCANCODE[10] = {
	0,
	SCANCODE_CURSORBLOCKUP, //SCANCODE_KEYPADF1,
	SCANCODE_KEYPADF2,
	SCANCODE_KEYPADF3,
	SCANCODE_KEYPADF4,
	SCANCODE_CURSORBLOCKRIGHT,//SCANCODE_KEYPADF5,
	SCANCODE_CURSORBLOCKDOWN,//SCANCODE_KEYPADF6,
	SCANCODE_CURSORBLOCKLEFT,//SCANCODE_KEYPADF7,
	SCANCODE_KEYPADF8,
	SCANCODE_ENTER//SCANCODE_KEYPADF9
};
static int read_keypad(unsigned char* key)
{
	int ret;
    unsigned char tmp;

	if (pad < 0) {
		*key = 0;
		return 0;
	}		

	ret = read(pad, &tmp, 1);
	if (ret <= 0) {
		*key = 0;
	} else if (tmp < 10 && tmp > 0) {
		*key = KEYPAD_SCANCODE[tmp];
	} else if (tmp < (0x80 + 10) && tmp > 0x80) {
        *key = KEYPAD_SCANCODE[tmp - 0x80] + 0x80;
    } else {
		fprintf(stderr, "IAL KEYPAD BUG: Illegal key = %d\n", tmp);
		*key = 0;
		ret = 0;
	}
	return ret;
}
#endif /* _THOR_IAL */
