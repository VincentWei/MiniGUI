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
 ** qemu.c: Low Level Input Engine for QEMU
 **         This driver can run on Linux.
 ** 
 ** Created by Libo Jiao, 2001/08/20
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGIAL_QEMU

#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <minigui.h>

#include "ial.h"

static unsigned char state [NR_KEYS];
static int btn_fd = -1;
static int mice_fd = -1;
static int key, old_key = -1;


#undef _DEBUG

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

static int keyboard_update(void)
{
    int status;

    int m,n;
    char buf[48];
    read (btn_fd, buf, 48);
    key = buf[26];
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

static char mice_info[3];
/*
 * the qemu mice info struct
 *  mice_info[0]: the button info
 *  	0x08: normal
 *  	0x09: left button down
 *  	0x0a: right button down
 *  	0x10: x-aixel decrement
 *  	0x20: y-aixel decrement
 *  mice_info[1]: x decriment
 *  mice_info[2]: y incriment 
 *
 */
static int mousex = 0;
static int mousey = 0;

static int qemu_mouse_update(void)
{
	int r;
	
	r = read(mice_fd, mice_info, sizeof(mice_info));

	if( r < 0 ) return -1;

	if(mice_info[1]<128)
		mousex += mice_info[1];
	else
		mousex += (mice_info[1]-256);

	
	if(mice_info[2]<128)
		mousey -= mice_info[2];
	else
		mousey += (256-mice_info[2]);

	if(mousex < 0 )
		mousex = 0;
	else if(mousex >= g_rcScr.right)
		mousex = g_rcScr.right;
	
	if(mousey < 0)
		mousey = 0;
	else if(mousey >= g_rcScr.bottom)
		mousey = g_rcScr.bottom;

	return 1;
}

static void qemu_mouse_getxy(int *x, int *y){
	*x = mousex;
	*y = mousey;
}

static int qemu_mouse_getbutton(void){
	return  ((mice_info[0]&0x01)?IAL_MOUSE_LEFTBUTTON:0)
		| ((mice_info[0]&0x02)?IAL_MOUSE_RIGHTBUTTON:0);
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

    if ((which & IAL_KEYEVENT) && btn_fd >= 0){
        FD_SET (btn_fd, in);
#ifndef _MGRM_THREADS
        if(btn_fd > maxfd) maxfd = btn_fd;
#endif
    }

	if ((which & IAL_MOUSEEVENT) && mice_fd >= 0){
		FD_SET ( mice_fd, in);
#ifndef _MGRM_THREADS 
		if(mice_fd > maxfd) maxfd = mice_fd;
#endif
	}

#ifndef _MGRM_THREADS
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

		if(mice_fd >= 0 && FD_ISSET(mice_fd, in)){
			FD_CLR(mice_fd, in);
			retvalue |= IAL_MOUSEEVENT;
		}
    }
    else if (e < 0) {
        return -1;
    }

    return retvalue;
}

BOOL InitQEMUInput (INPUT* input, const char* mdev, const char* mtype)
{

    btn_fd = open ("/dev/input/event1", O_RDONLY | O_NOCTTY);
    if (btn_fd < 0 ) {
        fprintf (stderr, "QEMU: Can not open keyboard!\n");
        return FALSE;
    }

	mice_fd = open("/dev/input/mice", O_RDONLY | O_NOCTTY);
	if(mice_fd < 0)
	{
		fprintf (stderr, "QEMU: Cannot open mice!\n");
		return FALSE;
	}
	
    input->update_mouse = qemu_mouse_update;
    input->get_mouse_xy = qemu_mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = qemu_mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->set_leds = NULL;

    input->wait_event = wait_event;

    return TRUE;
}

void TermQEMUInput (void)
{
    if (btn_fd >= 0)
        close(btn_fd);
	if(mice_fd >= 0)
		close(mice_fd);
}

#endif
