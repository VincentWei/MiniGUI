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
** hh2410r3.c: Low Level Input Engine for HH2410-R3, HHARM9-LCD-R4 Dev Boards.
*/

#include "common.h"
#ifdef _MGIAL_DAVINCI6446

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

#include "ial.h"
#include "davinci6446.h"

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

#define IAL_MOUSE_LEFTBUTTON    1
#define IAL_MOUSE_MIDDLEBUTTON  4
#define IAL_MOUSE_RIGHTBUTTON   2

#define BUTTON_L    IAL_MOUSE_LEFTBUTTON
#define BUTTON_M    IAL_MOUSE_MIDDLEBUTTON
#define BUTTON_R    IAL_MOUSE_RIGHTBUTTON

static int mousex = 0;
static int mousey = 0;
static ts_EVENT ts_event;
unsigned char buf[4];
int mouse_pressure = 0, mouse_x = 0,  mouse_y = 0;
int buttons[4] = { 0, BUTTON_L, BUTTON_R, BUTTON_L | BUTTON_R};


static int n = 0;
static int mouse_fd;

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
#if 0
	if (mousex < 0)
		mousex = 0;
	if (mousex > 720)
		mousex = 720;

	if (mousey < 0)
		mousey = 0;
	if (mousey > 576)
		mousey = 576;
#endif
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

    if ((which & IAL_MOUSEEVENT) && mouse_fd >= 0) {
        FD_SET (mouse_fd, in);
#ifndef _MGRM_THREADS_
        if (mouse_fd > maxfd) maxfd = mouse_fd;
#endif
    }

    timeout->tv_sec = 0;
    timeout->tv_usec = 50;

#ifndef _MGRM_THREADS_
    e = select (maxfd + 1, in, out, except, timeout) ;
#else
    e = select (FD_SETSIZE, in, out, except, timeout) ;
#endif

    if (e > 0) 
    { 
        if (FD_ISSET (mouse_fd, in))
        {
            n = read (mouse_fd, buf, 4);
            if (n == 3)
            {
                mouse_pressure = buttons[(buf[0] & 0x03)];
                mouse_x = (buf[0] & 0x10) ? buf[1] - 256 : buf[1];
                mouse_y = (buf[0] & 0x20) ? -(buf[2] - 256) : -buf[2];
                mousex += mouse_x;
                mousey += mouse_y;
                switch (mouse_pressure)
                {
                    case 0:
                        ts_event.pressure = 0;
                        break;
                    case 1:
                        ts_event.pressure = IAL_MOUSE_LEFTBUTTON ;
                        break;
                    case 2:
                        ts_event.pressure = IAL_MOUSE_RIGHTBUTTON;
                        break;
                }
                return IAL_MOUSEEVENT;
            }
            else
            {
                return -1;
            }
        }
    }
	else
	{
		return -1;
	}
}


BOOL InitDavinci6446Input(INPUT* input, const char* mdev, const char* mtype)
{
    mouse_fd = open (mdev, O_RDWR | O_NONBLOCK);
	if (mouse_fd < 0)
	{
		printf ("Can not open mouse !\n");
        return -1;
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

void TermDavinci6446Input(void)
{
    if (mouse_fd >= 0)
        close(mouse_fd);    
}

#endif /* _MGIAL_DAVINCI6446 */

