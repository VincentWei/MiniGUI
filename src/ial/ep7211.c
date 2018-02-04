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
** ep7211.c: Low Level Input Engine for EP7211.
** 
** Created by Wei Yongming, 2000/08/24
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _EP7211_IAL

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ep7211_internal.h"
#include "ial.h"
#include "ep7211.h"

#define TSTHRESHOLD	0x545e

static int ts;
static int mousex = 0;
static int mousey = 0;
static POS pos,truepos;
static TSP tsp;
/************************  Low Level Input Operations **********************/

/*
 * Mouse operations -- Event
 */
void convert(POS * tempos)
{
    int x0, y0;
    tempos->x -= tsp.xbase; 
    tempos->y -= tsp.ybase;
    x0 = tempos->x; 
    y0 = tempos->y;
    tempos->x -= y0 * (tsp.xdlt + x0 * tsp.xdlt2 / tsp.xsize) / tsp.ysize;
    tempos->y -= x0 * (tsp.ydlt + y0 * tsp.ydlt2 / tsp.ysize) / tsp.xsize;
    tempos->x = tempos->x*320/tsp.xsize; 
    tempos->y = tempos->y*240/tsp.ysize;
}

static int mouse_update(void)
{
	if ( (pos.x + pos.y) != 0 )
		convert(&pos);
	if ( pos.x < 0 || pos.y < 0 )
		return 0;

    truepos.x = pos.x;
    truepos.y = pos.y;
	return 1;
}

static void mouse_getxy(int* x, int* y)
{
	*x = truepos.x;
	*y = truepos.x;
}

static int mouse_getbutton(void)
{
	if ( (truepos.x + truepos.y) != 0 && (mousex + mousey) == 0 )
	{
		mousex = truepos.x;
		mousey = truepos.y;
		return IAL_MOUSE_LEFTBUTTON;
	}
	else if ( (truepos.x + truepos.y) == 0 && (mousex + mousey) != 0 )
	{
		mousex = 0;
		mousey = 0;
		return 0;
	}
	return 0;
}

static int keyboard_update(void)
{
	return 0;
}

static const char* keyboard_getstate(void)
{
	return NULL;
}

#ifdef _LITE_VERSION 
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    fd_set  rfds;
    int e;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if (which & IAL_MOUSEEVENT) {
        FD_SET (ts, in);
#ifdef _LITE_VERSION
        if (ts > maxfd) maxfd = ts;
#endif
    }

    e = select (maxfd+1, in, out, except, timeout);

    if (e > 0) {
        /* If data is present on the touchsreen fd, service it: */
        if (FD_ISSET (ts, in))
        {
	    read (ts, &pos, sizeof(POS));
            FD_CLR (ts, in);
            return IAL_MOUSEEVENT;
        }

    } else if (e < 0) {
        return -1;
    }

    return 0;
}

#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    int timeout_ms;
	struct pollfd ufd;

    if (timeout)
        timeout_ms = timeout->tv_sec * 1000 + timeout->tv_usec / 1000;
    else
        timeout_ms = -1;

	if ( (which & IAL_MOUSEEVENT) == IAL_MOUSEEVENT)
	{
		ufd.fd     = ts;
		ufd.events = POLLIN; 
		if ( poll (&ufd, 1, timeout_ms) > 0)
		{
			read (ts, &pos, sizeof(POS));	
			return IAL_MOUSEEVENT;
		}
	}
	return 0;
}
#endif

BOOL InitEP7211Input (INPUT* input, const char* mdev, const char* mtype)
{
	int fs_data;
	THRESHOLD  threshold;
    if(!access("/etc/ts.conf",R_OK))
	{
    	read(fs_data=open("/etc/ts.conf",0),&tsp,sizeof(tsp));
	    close(fs_data);
		threshold.x = tsp.xthrsd;
  	    threshold.y = tsp.ythrsd;	
	}
	else 
	{
		fprintf(stderr,"IAL: Can not set touch screet threshold!\n");
		return FALSE;
	}
	ts = open ("/dev/ts",O_RDONLY);
    if ( ts < 0 ) {
        fprintf (stderr, "IAL: Can not open touch screen!\n");
        return FALSE;
    }

	ioctl(ts,TSTHRESHOLD,&threshold);

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->set_leds = NULL;

    input->wait_event = wait_event;
	mousex = 0;
	mousey = 0;
    return TRUE;
}

void TermEP7211Input (void)
{
	close(ts);
}

#endif /* _EP7211_IAL */

