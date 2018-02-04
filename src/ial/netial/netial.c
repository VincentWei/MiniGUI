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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>

#include "mgconfig.h"
#include "common.h"
#include "minigui.h"
#include "netial.h"
#ifdef _MGIAL_NET

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#include <sys/select.h>
#include <sys/time.h>


/* for data reading from /dev/temp_sock_descriptor */
typedef struct {
    unsigned short pressure;
    unsigned short x;
    unsigned short y;
    unsigned short pad;
} ts_EVENT;

typedef struct
{
	int x;
	int y;
	int pressure;
}MOUSE_EVENT;

static int mousex = 0;
static int mousey = 0;
static ts_EVENT ts_event;

static int port = 8800;

//static int screen_width;
//static int screen_high;

static struct sockaddr_in sin;
static struct sockaddr_in pin;
static int sock_descriptor;
static int temp_sock_descriptor;
static unsigned int address_size;
static int n = 0;
static MOUSE_EVENT *mouse_event;
static char event_buf[100];

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
	if (mousex < 0)
		mousex = 0;
	if (mousex > g_rcScr.right)
		mousex = g_rcScr.right;

	if (mousey < 0)
		mousey = 0;
	if (mousey > g_rcScr.bottom)
		mousey = g_rcScr.bottom;
    *x = mousex;
    *y = mousey;
}

static int mouse_getbutton(void)
{
    return ts_event.pressure;
}

static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    fd_set rfds;
    int    e;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if ((which & IAL_MOUSEEVENT) && temp_sock_descriptor >= 0) {
        FD_SET (temp_sock_descriptor, in);
#ifndef _MGRM_THREADS_
        if (temp_sock_descriptor > maxfd) maxfd = temp_sock_descriptor;
#endif
    }
#ifndef _MGRM_THREADS_
    e = select (maxfd + 1, in, out, except, timeout) ;
#else
    e = select (FD_SETSIZE, in, out, except, timeout) ;
#endif

    if (e > 0) 
	{ 
		if (temp_sock_descriptor >= 0 && FD_ISSET (temp_sock_descriptor, in)) 
		{
			FD_CLR (temp_sock_descriptor, in);
			n = recv (temp_sock_descriptor, event_buf, 100, 0);
			if (n == 0)
			{
				printf ("recv == 0\n");
				exit (1);
			}
			if (n == sizeof (MOUSE_EVENT)/sizeof (char))
			{
				mouse_event = (MOUSE_EVENT*)event_buf;
				mousex += mouse_event->x * 2;
				mousey += mouse_event->y * 2;
				switch (mouse_event->pressure)
				{
					case 1:
						ts_event.pressure = IAL_MOUSE_LEFTBUTTON;
						break;
					case 2:
						ts_event.pressure = IAL_MOUSE_RIGHTBUTTON;
						break;
					case 0:
						ts_event.pressure = 0;
						break;
				}
				return  IAL_MOUSEEVENT;
			}
		} 
	}
	else if (e < 0) 
	{
		return -1;
	}

	return 0;
}

BOOL InitNetInput (INPUT* input, const char* mdev, const char* mtype)
{
	sock_descriptor = socket (AF_INET, SOCK_STREAM, 0);
	if (sock_descriptor == -1)
	{
		perror ("call to socket");
		exit (1);
	}

	bzero (&sin, sizeof (sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons (port);

	if (bind (sock_descriptor, (struct sockaddr*)&sin, sizeof (sin)) == -1)
	{
		perror("call to bind");
		exit (1);
	}

	if (listen (sock_descriptor, 20) == -1)
	{
		perror ("call to listen");
		exit (1);
	}
	printf ("Please start the PC side of the client process.\n");

	temp_sock_descriptor = accept (sock_descriptor, (struct sockaddr *)&pin, &address_size);
	if (temp_sock_descriptor == -1)
	{
		perror ("call to accept");
		exit (1);
	}

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;
    
	input->update_keyboard = NULL;
    input->get_keyboard_state = NULL;

    input->wait_event = wait_event;
    
    return TRUE;
}

void TermNetInput(void)
{
    if (temp_sock_descriptor >= 0)
        close(temp_sock_descriptor);    
}

#endif /* _MGIAL_NET */

