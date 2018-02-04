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
 ** figueroa.c: Low Level Input Engine for D2 FiguaOA
 **         This driver can run on Linux.
 ** 
 ** Created by Libo Jiao, 2001/08/20
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _FIGUEROA_IAL

#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ial.h"
#include "figueroa.h"

static unsigned char state [NR_KEYS];
static int btn_fd = -1;
static int key, old_key = -1;


#undef _DEBUG

//#define QEMU

#ifdef QEMU
static int key_mapper[][2] = {
	{2,SCANCODE_1},
	{3,SCANCODE_2},
	{4,SCANCODE_3},
	{5,SCANCODE_4},
	{6,SCANCODE_5},
	{7,SCANCODE_6},
	{8,SCANCODE_7},
	{9,SCANCODE_8},
	{10,SCANCODE_9},
	{11,SCANCODE_0},
	{59,SCANCODE_F1},
	{60,SCANCODE_F2},
	{61,SCANCODE_F3},
	{62,SCANCODE_F4},
	{63,SCANCODE_F5},
	{64,SCANCODE_F6},
	{65,SCANCODE_F7},
	{66,SCANCODE_F8},
	{67,SCANCODE_F9},
	{68,SCANCODE_F10},
	{103,SCANCODE_CURSORBLOCKUP},
	{105,SCANCODE_CURSORBLOCKLEFT},
	{106,SCANCODE_CURSORBLOCKRIGHT},
	{108,SCANCODE_CURSORBLOCKDOWN}
};
#else
static int key_mapper[][2] = {
	{1,SCANCODE_1},
	{2,SCANCODE_2},
	{3,SCANCODE_3},
	{4,SCANCODE_4},
	{5,SCANCODE_5},
	{6,SCANCODE_6},
	{7,SCANCODE_7},
	{8,SCANCODE_8},
	{9,SCANCODE_9},
	{10,SCANCODE_0},
	{11,SCANCODE_F1},
	{12,SCANCODE_F2},
	{17,SCANCODE_CURSORBLOCKUP},
	{18,SCANCODE_CURSORBLOCKDOWN},
	{19,SCANCODE_CURSORBLOCKLEFT},
	{20,SCANCODE_CURSORBLOCKRIGHT},
	{33,SCANCODE_F3},
	{34,SCANCODE_F4},
	{35,SCANCODE_F5},
	{36,SCANCODE_F6},
	{37,SCANCODE_F7},
	{39,SCANCODE_F8},
	{40,SCANCODE_F9},
	{41,SCANCODE_F10}
};
#endif

static int keyboard_update(void)
{
    int status;

    int m,n, nbytes =0;
#ifdef QEMU
    char buf[48];
    read (btn_fd, buf, 48);
    key = buf[26];
#else
    char buf[16];
    read (btn_fd, buf, 16);
    key = buf[10];
#endif
    status = (key == old_key)? 0 : 1;
    if (status == 0)
        old_key = -1;
    else 
        old_key = key;
	m = 0;
	n = sizeof(key_mapper)/sizeof(int*)-1;

	while(m<n){
		int half = (m+n)/2;
		if(key==key_mapper[half][0])
		{
//			printf("%d:getKey=%d\n",__LINE__,key);
			state[key_mapper[half][1]] = status;
			return NR_KEYS;
		}
		else if(key>key_mapper[half][0]){
			if(m==half) m++ ; else m = half;}
		else
			n = half;
	}
	if(m==n && key==key_mapper[m][0]){
//		printf("%d:getKey=%d\n",__LINE__,key);
		state[key_mapper[m][1]] = status;
	}
	else
	   	printf("undefined state\n");
	return NR_KEYS;
}

static const char* keyboard_getstate(void)
{
    return (char *)state;
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
    int    retvalue = 0;
    int    e;


    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if ((which & IAL_KEYEVENT) && btn_fd >= 0){
        FD_SET (btn_fd, in);
#ifdef _LITE_VERSION
        if(btn_fd > maxfd) maxfd = btn_fd;
#endif
    }


#ifdef _LITE_VERSION
    e = select (maxfd + 1, in, out, except, timeout) ;

#else
    e = select (FD_SETSIZE, in, out, except, timeout) ;
#endif

    if (e > 0)
    {
        if (btn_fd >= 0 && FD_ISSET(btn_fd, in))
        {
            FD_CLR(btn_fd, in);
            retvalue |= IAL_KEYEVENT;
        }
    }
    else if (e < 0) {
        return -1;
    }

    return retvalue;
}

BOOL InitFiguerOAInput (INPUT* input, const char* mdev, const char* mtype)
{

    btn_fd = open ("/dev/input/event1", O_RDONLY | O_NOCTTY);
    if (btn_fd < 0 ) {
        fprintf (stderr, "FiguerOA: Can not open keyboard!\n");
        return FALSE;
    }

    printf ("file descripts is %d\n", btn_fd);

    input->update_mouse = NULL;
    input->get_mouse_xy = NULL;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = NULL;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->set_leds = NULL;

    input->wait_event = wait_event;

    return TRUE;
}

void TermFiguerOAInput (void)
{
    if (btn_fd >= 0)
        close(btn_fd);
}

#endif
