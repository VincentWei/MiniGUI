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
** tf-stb.c: Low Level Input Engine for Tongfang STB.
** 
** Author: Liu Jianping (RedFlag Software).
**
** Revised by snig, 2001/09/21
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _TFSTB_IAL

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <linux/kd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "misc.h"
#include "ial.h"
#include "tf-stb.h"
#include "tf-code.h"

#define PANEL		1
#define REMOTER 	2
#define KEYBOARD	3

BYTE __mg_panel_funcmap[2][16] = {
{
	SCANCODE_0,
	SCANCODE_1,
	SCANCODE_2,
	SCANCODE_3,
	SCANCODE_4,
	SCANCODE_5,
	SCANCODE_6,
	SCANCODE_7,
	SCANCODE_8,
	SCANCODE_9,
	SCANCODE_ENTER,
	FUNCCODE_VIDEOMODE,
	FUNCCODE_MENU,
	FUNCCODE_DISCEXCH,
	FUNCCODE_VOLUP,
	FUNCCODE_VOLDOWN
},
{
	FUNCCODE_PLAYPAUSE,
	FUNCCODE_STOP,
	FUNCCODE_SF,
	FUNCCODE_EJECT,
	FUNCCODE_FB,
	FUNCCODE_FF,
	FUNCCODE_PREVIOUS,
	FUNCCODE_NEXT,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE
}
};

BYTE __mg_remoter_funcmap[6][16] = {
{
 	SCANCODE_0,
	SCANCODE_1,
	SCANCODE_2,
	SCANCODE_3,
	SCANCODE_4,
	SCANCODE_5,
	SCANCODE_6,
	SCANCODE_7,
	SCANCODE_8,
	SCANCODE_9,
	SCANCODE_ENTER,
	FUNCCODE_CLEAR, // ?
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE
},
{
	FUNCCODE_PLAYPAUSE,
	FUNCCODE_STOP,
	FUNCCODE_SF,
	FUNCCODE_EJECT,
	FUNCCODE_FB,
	FUNCCODE_FF,
	FUNCCODE_PREVIOUS,
	FUNCCODE_NEXT,
	FUNCCODE_SR,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE
},
{
	FUNCCODE_POWER,
	FUNCCODE_VIDEOMODE,
	FUNCCODE_OPMODE,
	FUNCCODE_UP,
	FUNCCODE_DOWN,
	FUNCCODE_LEFT,
	FUNCCODE_RIGHT,
	FUNCCODE_SELECT,
	FUNCCODE_DISCEXCH,
	FUNCCODE_SETUP,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE
},
{
	FUNCCODE_VOLUP,
	FUNCCODE_VOLDOWN,
	FUNCCODE_MUTE,
	FUNCCODE_LR,
	FUNCCODE_AUDIOMODE,
	FUNCCODE_3D,
	FUNCCODE_EQ,
	FUNCCODE_KARAOKE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE
},
{
	FUNCCODE_PROG,
	FUNCCODE_TITLE,
	FUNCCODE_ANGLE,
	FUNCCODE_AB,
	FUNCCODE_REPEAT,
	FUNCCODE_ZOOM,
	FUNCCODE_OSD,
	FUNCCODE_LANG,
	FUNCCODE_SUBTITLE,
	FUNCCODE_MENU,
	FUNCCODE_DISPLAY,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE
},
{
	FUNCCODE_EMAIL,
	FUNCCODE_WEB,
	FUNCCODE_START,
	FUNCCODE_REFRESH,
	FUNCCODE_ENLARGE,
	FUNCCODE_FAVORITE,
	FUNCCODE_ESC,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE
}
};

BYTE __mg_kb_funcmap[7][16] = {
{
	SCANCODE_7,
	SCANCODE_8,
	SCANCODE_9,
	SCANCODE_0,
	SCANCODE_U,
	SCANCODE_I,
	SCANCODE_O,
	SCANCODE_P,
	SCANCODE_J,
	SCANCODE_K,
	SCANCODE_L,
	SCANCODE_SEMICOLON,
	SCANCODE_M,
	SCANCODE_PERIOD,
	SCANCODE_SLASH,
	SCANCODE_ENTER
},
{
	SCANCODE_F1,
	SCANCODE_F2,
	SCANCODE_F3,
	SCANCODE_F4,
	SCANCODE_F5,
	SCANCODE_F6,
	SCANCODE_F7,
	SCANCODE_F8,
	SCANCODE_F9,
	SCANCODE_F10,
	SCANCODE_F11,
	SCANCODE_F12,
	SCANCODE_ESCAPE,
	SCANCODE_NUMLOCK,
	SCANCODE_PRINTSCREEN,
	SCANCODE_SCROLLLOCK
},
{
	SCANCODE_BREAK,
	SCANCODE_GRAVE,
	SCANCODE_1,
	SCANCODE_2,
	SCANCODE_3,
	SCANCODE_4,
	SCANCODE_5,
	SCANCODE_6,
	SCANCODE_MINUS,
	SCANCODE_EQUAL,
	SCANCODE_BACKSPACE,
	SCANCODE_TAB,
	SCANCODE_Q,	
	SCANCODE_W,
	SCANCODE_E,
	SCANCODE_R
},
{
	SCANCODE_T,
	SCANCODE_Y,
	SCANCODE_CAPSLOCK,
	SCANCODE_A,
	SCANCODE_S,
	SCANCODE_D,
	SCANCODE_F,
	SCANCODE_G,
	SCANCODE_H,
	SCANCODE_Z,
	SCANCODE_X,
	SCANCODE_C,
	SCANCODE_V,
	SCANCODE_B,
	SCANCODE_N,
	SCANCODE_LEFTSHIFT,
},
{
	FUNCCODE_LEFTWIN,
	SCANCODE_CURSORBLOCKUP,
	SCANCODE_CURSORBLOCKRIGHT,
	SCANCODE_LEFTALT,
	FUNCCODE_RIGHTWIN,
	SCANCODE_PAGEUP,
	SCANCODE_HOME,
	SCANCODE_RIGHTALT,
	FUNCCODE_APP,
	SCANCODE_PAGEDOWN,
	SCANCODE_END,
	SCANCODE_RIGHTCONTROL,
	SCANCODE_INSERT,
	SCANCODE_LEFTCONTROL,
	SCANCODE_CURSORBLOCKDOWN,
	SCANCODE_CURSORBLOCKLEFT,
},
{
	SCANCODE_REMOVE,
	SCANCODE_RIGHTCONTROL,
	SCANCODE_PAGEDOWN,
	SCANCODE_END,
	SCANCODE_RIGHTSHIFT,
	SCANCODE_SPACE,
	SCANCODE_BRACKET_LEFT,
	SCANCODE_BRACKET_RIGHT,
	SCANCODE_BACKSLASH,
	SCANCODE_APOSTROPHE,
	SCANCODE_COMMA,
	// TODO
	FUNCCODE_NONE, 
	FUNCCODE_NONE, 
	FUNCCODE_NONE, 
	FUNCCODE_NONE, 
	FUNCCODE_NONE 
},
{
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_MACRO,
	FUNCCODE_POWER,
	FUNCCODE_NONE,
	FUNCCODE_SLEEP,
	FUNCCODE_NONE,
	FUNCCODE_WAKEUP,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE,
	FUNCCODE_NONE
}
};

static int serial = -1;
static int mousex = 0, mousey = 0;
static int button;

static BYTE type;
static unsigned char state[MGUI_NR_KEYS];

/************************  code table transferation functions **************/ 
/* 
 * get position code by serial code received 
 */

static BYTE get_poscode (BYTE serial_code, int code)
{
	switch (code) {
	case PANEL:
	case REMOTER:
	case KEYBOARD:
	}

	return serial_code;
}


/*
 * get function code by serial code received
 */ 

static BYTE get_funccode (BYTE serial_code, int code)
{
	int i, j;
	BYTE pos_code;

	pos_code = get_poscode (serial_code, code);

	i = pos_code & 0xF0;
	j = pos_code & 0x0F;

	switch (code) {
	case PANEL:
		return __mg_panel_funcmap[i][j];
	case REMOTER:
		return __mg_remoter_funcmap[i][j];
	case KEYBOARD:
		return __mg_kb_funcmap[i][j];
	default:
		return -1;
	}

}

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */

static int mouse_update (void)
{
    BYTE mouse_data[2];
    BYTE data1, data2;
    
    read (serial, mouse_data, 2);
    data1 = mouse_data[0];
    data2 = mouse_data[1];
    
    // mouse button state
    button = 0; 
    if (mouse_data[1] & 0x10)
        button |= IAL_MOUSE_LEFTBUTTON;
    if (mouse_data[1] & 0x20)
	button |= IAL_MOUSE_RIGHTBUTTON;  
    
    // x move left or right
    if (mouse_data[1] & 0x40)      
 	mousex -= mouse_data[0] & 0x3F;
    else  
 	mousex += mouse_data[0] & 0x3F;
 
    // get y data
    data1 >>= 2; data1 &= 0x30;
    data2 &= 0x0F; 
    data1 &= data2;		

    // y move up or down
    if (mouse_data[1] & 0x80)   
 	mousey += data1;
    else 
 	mousey -= data1;
    
    if (mousex >= 640)
        mousex = 639;
    if (mousex <= 0)
        mousex = 0 ;

    if (mousey >= 480)
        mousey = 480;
    if (mousey <= 0)
        mousey = 0;
    	
    return 0;
}

static void mouse_getxy(int *x, int *y)
{
    *x = mousex;
    *y = mousey;
}

static int mouse_getbutton(void)
{
    return button;
}

static void mouse_setrange(int minx,int miny,int maxx,int maxy)
{
}

static int keyboard_update(void)
{
    BYTE serial_code[2];    
    
    read (serial, serial_code, 2);
    if (type == 1) {
	state[get_funccode (serial_code[0] & 0x7F, KEYBOARD)] = 1;
	if (serial_code[1] & 0x80)   //win-r  
	     state[FUNCCODE_RIGHTWIN] = 1; 	
	if (serial_code[1] & 0x40)    //win-l
	     state[FUNCCODE_LEFTWIN] = 1;
	if (serial_code[1] & 0x20)    //shift-r
	      state[SCANCODE_RIGHTSHIFT] = 1;
	if (serial_code[1] & 0x10)    //shift-l
	      state[SCANCODE_LEFTSHIFT] = 1;	
	if (serial_code[1] & 0x08)    //ctrl-r
	      state[SCANCODE_RIGHTCONTROL] = 1;
	if (serial_code[1] & 0x04)    //ctrl-l
              state[SCANCODE_LEFTCONTROL] = 1;
	if (serial_code[1] & 0x02)    //alt-r
	      state[SCANCODE_RIGHTALT] = 1;
	if (serial_code[1] & 0x01)    //alt-l	
	      state[SCANCODE_LEFTALT] = 1;	
    }
    else if (type == 2) {
	 state[get_funccode (serial_code[0] & 0x7F, KEYBOARD)] = 0;
         if (!serial_code[1] & 0x80)   //win-r
            state[FUNCCODE_RIGHTWIN] = 0;
	 if (!serial_code[1] & 0x40)    //win-l
            state[FUNCCODE_LEFTWIN] = 0;
	 if (!serial_code[1] & 0x20)    //shift-r
             state[SCANCODE_RIGHTSHIFT] = 0;
	 if (!serial_code[1] & 0x10)    //shift-l
             state[SCANCODE_LEFTSHIFT] = 0;
	 if (!serial_code[1] & 0x08)    //ctrl-r
             state[SCANCODE_RIGHTCONTROL] = 0;
	 if (!serial_code[1] & 0x04)    //ctrl-l
              state[SCANCODE_LEFTCONTROL] = 0;
	 if (!serial_code[1] & 0x02)    //alt-r
	      state[SCANCODE_RIGHTALT] = 0;
	 if (!serial_code[1] & 0x01)    //alt-l
	      state[SCANCODE_LEFTALT] = 0;	
    }
    else if (type == 5) { // from front panel
	   state[get_funccode(serial_code[0] & 0x7F, PANEL)] = 1;	
    }
    else if (type == 6) { // from remote control
	   state[get_funccode(serial_code[0] & 0x7F, REMOTER)] = 1;	
    }	
    return 0;
}

static const char * keyboard_geserialstate(void)
{
    return (char *)state;
}

#ifdef  _LITE_VERSION

static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    fd_set rfds;
    int    e;
    BYTE   esc;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if ((which & IAL_MOUSEEVENT || which & IAL_KEYEVENT) && serial >= 0) {
        FD_SET (serial, in);
        if (serial > maxfd) maxfd = serial;
    }

    e = select (maxfd + 1, in, out, except, timeout) ;

    if (e < 0) 
	return -1;

    if (serial >= 0 && FD_ISSET (serial, in)) {
            FD_CLR (serial, in);

	    read (serial, &esc, 1); 
	    if (esc != 0x1B) {
		fprintf (stderr, "Read input data error.\n");
		return -1;
	    }
	    
	    read (serial, &type, 1);
	    switch (type) {
	    case 1: // keyboard
	    case 2: // keyboard
	    	return IAL_KEYEVENT;
	    case 3: // mouse
		return IAL_MOUSEEVENT;
	    case 5: // front panel keypad 
		return IAL_KEYEVENT; 	  
	    case 6: // remote control			 
	  	return IAL_KEYEVENT; 	  
	    default:
		fprintf (stderr, "Not supported ESC code.\n");
		return 0;
	    }
    }

    return 0;
}

#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
	       	struct timeval *timeout)
{
    // do nothing
    return 0;
}
#endif

static void set_leds ( unsigned int leds )
{
}

static int init_serial (void)
{
    struct termios term; 
 
    serial = open ("/dev/ttyS0", O_RDONLY | O_NOCTTY);     // FIXME while VFD included

    if (serial < 0) {
        fprintf (stderr, "IAL: Can not open /dev/ttyS0 for Tongfang STB!\n");
	return -1;
    }
    
    if ( tcgetattr (serial, &term) < 0 ) {
	perror ("TFSTB tcgetattr");	   
	return -1;
    }   

    cfsetispeed (&term, B9600);
    cfsetospeed (&term, B9600);

    if ( tcsetattr (serial, TCSANOW, &term)  < 0 ) {
	perror ("TFSTB tcsetattr");	   
	return -1;
    }	

    return 0;
}

BOOL InitTFSTBInput (INPUT* input, const char* mdev, const char* mtype)
{
    int i ;

    if (init_serial () < 0) 
	return FALSE;

    for (i = 0 ; i < MGUI_NR_KEYS; i++)
        state[i] = 0;

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = mouse_setrange;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_geserialstate;
    input->set_leds = set_leds;
    input->wait_event = wait_event;
    return TRUE;
}

void TermTFSTBInput (void)
{
    if (serial >= 0)
   	close (serial);
	 
}

#endif /* _TFSTB_IAL */

