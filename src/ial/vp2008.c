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
** vp2008.c: The vp2008 IAL engine.
** 
** Created by Wei Yongming, 2001/09/13
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "common.h"

#ifdef _VP2008_IAL

#include "misc.h"
#include "ial.h"
#include "vp2008.h"

typedef struct InputEvent {
	unsigned int dummy1;
	unsigned int dummy2;
	unsigned short type;
	unsigned short code;
	unsigned int value;
} Myinputevent;

typedef struct KeyMap
{
	int  key_code;
	ushort unicode;
	ushort shift_unicode;
	ushort ctrl_unicode;
	ushort fn_key_code;
	ushort fn_unicode;
	ushort fn_shift_unicode;
	ushort fn_ctrl_unicode;
}yealinkKeyMap;

#define NOT_EXIST_KEY		{ MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff}

static const yealinkKeyMap yealinkKeyM[] = {
	/* 000 */	NOT_EXIST_KEY,
	/* 001 */	{ MY_SCANCODE_1, '1', 0xffff, 0xffff, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 002 */	{ MY_SCANCODE_4, '4', 0xffff, 0xffff, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 003 */	{ MY_SCANCODE_7, '7', 0xffff, 0xffff, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 004 */	{ MY_SCANCODE_NOTEXIST, '*', 0xffff, 0xffff, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 005 */	{ MY_SCANCODE_P, 'p', 'P', 'P'-64, MY_SCANCODE_7, '&', 0xffff, 0xffff},
	/* 006 */	{ MY_SCANCODE_BACKSPACE, 8, 8, 0xffff, MY_SCANCODE_BACKSPACE, 0xffff, 0xffff, 0xffff},
	/* 007 */	{ MY_SCANCODE_ENTER, 13, 13, 0xffff, MY_SCANCODE_ENTER, 13, 13, 0xffff},
	/* 008 */	{ MY_SCANCODE_CURSORBLOCKRIGHT, 0xffff , 0xffff , 0xffff , MY_SCANCODE_END, 0xffff, 0xffff, 0xffff},
	/* 009 */	{ MY_SCANCODE_2, '2', 0xffff, 0xffff, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 010 */	{ MY_SCANCODE_5, '5', 0xffff, 0xffff, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 011 */	{ MY_SCANCODE_8, '8', 0xffff, 0xffff, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 012 */	{ MY_SCANCODE_0, '0', 0xffff, 0xffff, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 013 */	{ MY_SCANCODE_O, 'o', 'O', 'O'-64, MY_SCANCODE_5, '%', 0xffff, 0xffff},
	/* 014 */	{ MY_SCANCODE_L, 'l', 'L', 'L'-64, MY_SCANCODE_0, ')', 0xffff, 0xffff},
	/* 015 */	{ MY_SCANCODE_CURSORBLOCKUP, 0xffff , 0xffff , 0xffff , MY_SCANCODE_PAGEUP, 0xffff, 0xffff, 0xffff},
	/* 016 */	{ MY_SCANCODE_CURSORBLOCKDOWN, 0xffff , 0xffff , 0xffff , MY_SCANCODE_PAGEDOWN, 0xffff, 0xffff, 0xffff},
	/* 017 */	{ MY_SCANCODE_3, '3', 0xffff, 0xffff, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 018 */	{ MY_SCANCODE_6, '6', 0xffff, 0xffff, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 019 */	{ MY_SCANCODE_9, '9', 0xffff, 0xffff, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 020 */	{ MY_SCANCODE_NOTEXIST, '#', 0xffff, 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 021 */	{ MY_SCANCODE_I, 'i', 'I', 'I'-64, MY_SCANCODE_4, '$', 0xffff, 0xffff},
	/* 022 */	{ MY_SCANCODE_K, 'k', 'K', 'K'-64, MY_SCANCODE_9, '(', 0xffff, 0xffff},
	/* 023 */	{ MY_SCANCODE_M, 'm', 'M', 'M'-64, MY_SCANCODE_SEMICOLON, ':', 0xffff, 0xffff},
	/* 024 */	{ MY_SCANCODE_CURSORBLOCKLEFT, 0xffff , 0xffff , 0xffff , MY_SCANCODE_HOME, 0xffff, 0xffff, 0xffff},
	/* 025 */	{ MY_SCANCODE_NOTEXIST, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 026 */	{ MY_SCANCODE_NOTEXIST, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 027 */	{ MY_SCANCODE_NOTEXIST, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 028 */	{ MY_SCANCODE_NOTEXIST, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 029 */	{ MY_SCANCODE_U, 'u', 'U', 'U'-64, MY_SCANCODE_2, '@', 0xffff, 0xffff},
	/* 030 */	{ MY_SCANCODE_J, 'j', 'J', 'J'-64, MY_SCANCODE_GRAVE, '`', 0xffff, 0xffff},
	/* 031 */	{ MY_SCANCODE_N, 'n', 'N', 'N'-64, MY_SCANCODE_BACKSLASH, '\\', 0xffff, 0xffff},
	/* 032 */	{ MY_SCANCODE_REMOVE, 0xffff , 0xffff , 0xffff , MY_SCANCODE_REMOVE, 0xffff, 0xffff, 0xffff},
	/* 033 */	{ MY_SCANCODE_NOTEXIST, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 034 */	{ MY_SCANCODE_NOTEXIST, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 035 */	{ MY_SCANCODE_NOTEXIST, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 036 */	{ MY_SCANCODE_F9, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 037 */	{ MY_SCANCODE_Y, 'y', 'Y', 'Y'-64, MY_SCANCODE_1, '!', 0xffff, 0xffff},
	/* 038 */	{ MY_SCANCODE_H, 'h', 'H', 'H'-64, MY_SCANCODE_6, '^', 0xffff, 0xffff},
	/* 039 */	{ MY_SCANCODE_B, 'b', 'B', 'B'-64, MY_SCANCODE_BACKSLASH, '|', 0xffff, 0xffff},
	/* 040 */	{ MY_SCANCODE_INSERT, 0xffff , 0xffff , 0xffff , MY_SCANCODE_APOSTROPHE, '"', 0xffff, 0xffff},
	/* 041 */	{ MY_SCANCODE_F1, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 042 */	{ MY_SCANCODE_F3, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 043 */	{ MY_SCANCODE_NOTEXIST, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 044 */	{ MY_SCANCODE_F10, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 045 */	{ MY_SCANCODE_T, 't', 'T', 'T'-64, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 046 */	{ MY_SCANCODE_G, 'g', 'G', 'G'-64, MY_SCANCODE_GRAVE, '~', 0xffff, 0xffff},
	/* 047 */	{ MY_SCANCODE_V, 'v', 'V', 'V'-64, MY_SCANCODE_EQUAL, '=', 0xffff, 0xffff},
	/* 048 */	{ MY_SCANCODE_SPACE, ' ', ' ', 0xffff, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 049 */	{ MY_SCANCODE_F2, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 050 */	{ MY_SCANCODE_F4, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 051 */	{ MY_SCANCODE_F6, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 052 */	{ MY_SCANCODE_F11, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 053 */	{ MY_SCANCODE_R, 'r', 'R', 'R'-64, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 054 */	{ MY_SCANCODE_D, 'd', 'D', 'D'-64, MY_SCANCODE_BRACKET_LEFT, '{', 0xffff, 0xffff},
	/* 055 */	{ MY_SCANCODE_F, 'f', 'F', 'F'-64, MY_SCANCODE_BRACKET_RIGHT, '}', 0xffff, 0xffff},
	/* 056 */	{ MY_SCANCODE_C, 'c', 'C', 'C'-64, MY_SCANCODE_SLASH, '/', 0xffff, 0xffff},
	/* 057 */	{ MY_SCANCODE_F7, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 058 */	{ MY_SCANCODE_F8, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 059 */	{ MY_SCANCODE_COMMA, ',', 0xffff, 0xffff, MY_SCANCODE_SEMICOLON, ';', 0xffff, 0xffff},
	/* 060 */	{ MY_SCANCODE_PERIOD, '.', 0xffff, 0xffff, MY_SCANCODE_SLASH, '?', 0xffff, 0xffff},
	/* 061 */	{ MY_SCANCODE_E, 'e', 'E', 'E'-64, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 062 */	{ MY_SCANCODE_S, 's', 'S', 'S'-64, MY_SCANCODE_BRACKET_RIGHT, ']', 0xffff, 0xffff},
	/* 063 */	{ MY_SCANCODE_X, 'x', 'X', 'X'-64, MY_SCANCODE_MINUS, '-', 0xffff, 0xffff},
	/* 064 */	{ MY_SCANCODE_MENU, 0xffff, 0xffff, 0xffff, MY_SCANCODE_MENU, 0xffff, 0xffff, 0xffff},
	/* 065 */	NOT_EXIST_KEY,
	/* 066 */	NOT_EXIST_KEY,
	/* 067 */	{ MY_SCANCODE_MINUS, '_', 0xffff, 0xffff, MY_SCANCODE_COMMA, '<', 0xffff, 0xffff},
	/* 068 */	{ MY_SCANCODE_APOSTROPHE, '\'', 0xffff, 0xffff, MY_SCANCODE_PERIOD, '>', 0xffff, 0xffff},
	/* 069 */	{ MY_SCANCODE_Q, 'q', 'Q', 'Q'-64, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 070 */	{ MY_SCANCODE_W, 'w', 'W', 'W'-64, MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 071 */	{ MY_SCANCODE_A, 'a', 'A', 'A'-64, MY_SCANCODE_BRACKET_LEFT, '[', 0xffff, 0xffff},
	/* 072 */	{ MY_SCANCODE_Z, 'z', 'Z', 'Z'-64, MY_SCANCODE_EQUAL, '+', 0xffff, 0xffff},
	/* 073 */	{ MY_SCANCODE_LEFTSHIFT, 0xffff, 0xffff, 0xffff, MY_SCANCODE_LEFTSHIFT, 0xffff, 0xffff, 0xffff},
	/* 074-Fn */{ MY_SCANCODE_FN, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 075 */	{ MY_SCANCODE_LEFTCONTROL, 0xffff, 0xffff, 0xffff, MY_SCANCODE_LEFTCONTROL, 0xffff, 0xffff, 0xffff},
	/* 076 */	{ MY_SCANCODE_LEFTALT, 0xffff , 0xffff , 0xffff , MY_SCANCODE_LEFTALT, 0xffff, 0xffff, 0xffff},
	/* 077 */	{ MY_SCANCODE_ESCAPE, 0x1b , 0x1b , 0xffff , MY_SCANCODE_ESCAPE, 0x1b, 0x1b, 0xffff},
	/* 078 */	{ MY_SCANCODE_TAB, 9, 9, 0xffff, MY_SCANCODE_TAB, 0xffff, 0xffff, 0xffff},
	/* 079 */	NOT_EXIST_KEY,
	/* 080 */	NOT_EXIST_KEY,
	/* 081 */	NOT_EXIST_KEY,
	/* 082 */	{ MY_SCANCODE_NOTEXIST, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
	/* 083 */	{ MY_SCANCODE_NOTEXIST, 0xffff , 0xffff , 0xffff , MY_SCANCODE_NOTEXIST, 0xffff, 0xffff, 0xffff},
};

static unsigned char state [MY_NR_KEYS];
static int btn_fd = -1;
static int mouse_fd = -1;
static int fn = 0;
static int shift = 0;
static int ctrl = 0;
static int alt = 0;

static unsigned char IMPS2_Param [] = {243,200,243,100,243,80};
static int    xpos;        /* current x position of mouse */
static int    ypos;        /* current y position of mouse */
static int    minx;        /* minimum allowed x position */
static int    maxx;        /* maximum allowed x position */
static int    miny;        /* minimum allowed y position */
static int    maxy;        /* maximum allowed y position */
static int    buttons;     /* current state of buttons */
static int    scale = 3;       /* acceleration scale factor */
static int    thresh = 5;      /* acceleration threshhold */

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */

static int Mouse_Read (int *dx, int *dy, int *dz, int *bp)
{
	static unsigned char buf[5];
	static int buttons[4] = { 0, 4, 1, 5};
	static int nbytes;
	int n;

	while ((n = read (mouse_fd, &buf [nbytes], 4 - nbytes))) {
		if (n < 0) {
			if (errno == EINTR)
				continue;
			else
				return -1;
		}

		nbytes += n;

		if (nbytes == 4) {
			int wheel;

			if ((buf[0] & 0xc0) != 0) {
				buf[0] = buf[1];
				buf[1] = buf[2];
				buf[2] = buf[3];
				nbytes = 3;

				return -1;
			}

			/* FORM XFree86 4.0.1 */
			*bp = buttons[(buf[0] & 0x03)];
			*dx = (buf[0] & 0x10) ? buf[1] - 256 : buf[1];
			*dy = (buf[0] & 0x20) ? -(buf[2] - 256) : -buf[2];

			/* Is a wheel event? */
			if ((wheel = buf[3]) != 0) {
				if(wheel > 0x7f) {
					*bp = WHEEL_UP;
				}
				else {
					*bp = WHEEL_DOWN;
				}
			}

			*dz = 0;
			nbytes = 0;
			return 1;
		}
	}
	return 0;
}


static int mouse_update(void)
{
	int dx,dy,dz;
	int r;
	int sign;

	dx = dy = 0;
	r = Mouse_Read(&dx, &dy, &dz, &buttons);
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
	if (mouse_fd > 0)
		close (mouse_fd);

	mouse_fd = -1;
}

static int mouse_resume(void)
{
	mouse_fd = open (IAL_MDev, O_RDWR | O_NONBLOCK);
	if (mouse_fd < 0) {
		mouse_fd = open (IAL_MDev, O_RDONLY | O_NONBLOCK);
		if (mouse_fd < 0)
			return -1;
	}
	else {
		write (mouse_fd, IMPS2_Param, sizeof (IMPS2_Param));
	}

	return mouse_fd;
}

static int keyboard_update(void)
{
	return MY_NR_KEYS;
}

static int read_key (void)
{
	Myinputevent event;
	if (read(btn_fd, &event, sizeof(Myinputevent)) != sizeof(Myinputevent))
	{
        return 0;
	}
	if (event.code == 0)
	{
		return 0;
	}

	int bIsPress = event.value != 0;
	if ( event.code < sizeof(yealinkKeyM) / sizeof(yealinkKeyMap))
	{
		const yealinkKeyMap currentKey = yealinkKeyM[event.code];
		if(currentKey.key_code == MY_SCANCODE_FN) // key_fn
		{
			fn = bIsPress;
			state[MY_SCANCODE_FN] = bIsPress;
		}
		int keyCode = fn ? currentKey.fn_key_code : currentKey.key_code;
		if(keyCode == MY_SCANCODE_NOTEXIST)
		{
			return 0;
		}
		state[keyCode] = bIsPress;
	}
	return 1;
}


static const char * keyboard_get_state (void)
{
	return (char *)state;
}

#ifdef _LITE_VERSION 
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
	fd_set rfds;
	int	e;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

	e = select (maxfd + 1, in, out, except, timeout) ;

    if (e < 0) {
		return -1;
	}
	return 0;
}
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
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
	}

	if ((which & IAL_KEYEVENT) && btn_fd >= 0){
		fd = btn_fd;
		FD_SET (btn_fd, in);
	}

	e = select (FD_SETSIZE, in, out, except, timeout) ;

	if (e > 0) {
		fd = mouse_fd;
		/* If data is present on the mouse fd, service it: */
		if (fd >= 0 && FD_ISSET (fd, in)) {
			FD_CLR (fd, in);
			retvalue |= IAL_MOUSEEVENT;
		}

		fd = btn_fd;
		if (fd >= 0 && FD_ISSET(fd, in)) {
			FD_CLR(fd, in);
			if (read_key())
			{
				retvalue |= IAL_KEYEVENT;
			}
			else 
			{  
				return -1;
			}
		}

	} else if (e < 0) {
		return -1;
	}

	return retvalue;
}
#endif

BOOL InitVP2008Input (INPUT* input, const char* mdev, const char* mtype)
{
	mouse_fd = open (mdev, O_RDWR | O_NONBLOCK);
	if (mouse_fd < 0) {
		mouse_fd = open (mdev, O_RDONLY | O_NONBLOCK);
		if (mouse_fd < 0){
			fprintf (stderr, "VP2008: Can not open mouse!\n");
			return FALSE;
		}
	}
	else {
		write (mouse_fd, IMPS2_Param, sizeof (IMPS2_Param));
	}

	btn_fd = open ("/dev/input/event0", O_RDONLY);
	if (btn_fd < 0 ) {
		fprintf (stderr, "VP2008: Can not open button key!\n");
		return FALSE;
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
    input->get_keyboard_state = keyboard_get_state;
    input->set_leds = NULL;

    input->wait_event = wait_event;

    return TRUE;
}

void TermVP2008Input (void)
{
	if (mouse_fd >= 0)
		close(mouse_fd);

	if (btn_fd >= 0)
		close(btn_fd);
}

#endif /* _VP2008_IAL */

