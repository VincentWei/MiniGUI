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
** ucb1x00.c: IAL Engine for UCB1x00 board.
** 
** Author: Zhong Shuyi (2003/12/28)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _UCB1X00_IAL

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <linux/kd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <termios.h>

//#include "misc.h"
#include "ial.h"
#include "ucb1x00.h"


#define _DEBUG_UCB1X00	0

/* ------------------------------------------------------------------------- */

#ifndef KEY_RELEASED
#define KEY_RELEASED	0x80
#endif

#ifndef KEY_NUM
#define KEY_NUM		0x7F
#endif

/* for data reading from /dev/tpanel */
typedef struct {
    short header;
    short x;
    short y;
} POS;
typedef struct {
  unsigned short header;
  unsigned short y;
  unsigned short x;
  unsigned short board;
  short time[4];
} RAW_POS;

static int ts = -1;
static int kb_fd = -1;

static unsigned char state [NR_KEYS];
static unsigned char btn_state=0;

static POS pos;
static RAW_POS raw_pos;

static int a, b, c, d, e, f, s;

/************************  Low Level Input Operations **********************/

static void transform(POS *pos, RAW_POS *raw_pos)
{
    pos->header = raw_pos->header;
    pos->x =  640-raw_pos->y; //(a*raw_pos->x + b*raw_pos->y + c)/s;
    pos->y =  480-raw_pos->x;  //(d*raw_pos->x + e*raw_pos->y + f)/s;
}

static int mouse_update(void)
{
    return 1;
}

static void mouse_getxy(int *x, int* y)
{
    *x = pos.x;
    *y = pos.y;
}

static int mouse_getbutton(void)
{
    printf ("return =%d\n", pos.header);
    return pos.header;

}

static int keyboard_update(void)
{
    int status;
    int key;

    status = (btn_state & KEY_RELEASED)? 0: 1;
    key = btn_state & KEY_NUM;

    if (key == SCANCODE_CURSORLEFT)
	key = SCANCODE_CURSORBLOCKLEFT;
    else if (key == SCANCODE_CURSORRIGHT)
	key = SCANCODE_CURSORBLOCKRIGHT;
    else if (key == SCANCODE_CURSORUP)
	key = SCANCODE_CURSORBLOCKUP;
    else if (key == SCANCODE_CURSORDOWN)
	key = SCANCODE_CURSORBLOCKDOWN;

    state[key] = status;

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

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if ((which & IAL_MOUSEEVENT) && ts >= 0) {
        FD_SET (ts, in);
#ifdef _LITE_VERSION
        if (ts > maxfd) maxfd = ts;
#endif
    }
    if ((which & IAL_KEYEVENT) && kb_fd >= 0){
        FD_SET (kb_fd, in);
#ifdef _LITE_VERSION
        if(kb_fd > maxfd) maxfd = kb_fd;
#endif
    }

#ifdef _LITE_VERSION
    if ( select (maxfd + 1, in, out, except, timeout) < 0 )
#else
    if ( select (FD_SETSIZE, in, out, except, timeout) < 0 )
#endif
	return -1;

    if (ts >= 0 && FD_ISSET (ts, in))
    {
        FD_CLR (ts, in);

        if ( read (ts, &raw_pos, sizeof (RAW_POS)) == sizeof(RAW_POS) ) {
            if (raw_pos.header != 0) {
	            transform(&pos, &raw_pos);
            }
            else 
                pos.header = raw_pos.header;
    	    pos.header = ( pos.header > 0 ? 4:0 );
	        printf("raw:header=%d x=%d y=%d\n", raw_pos.header, raw_pos.x, raw_pos.y);
	        printf("header=%d x=%d y=%d\n", pos.header, pos.x, pos.y);
            retvalue |= IAL_MOUSEEVENT;
        }
        else {
    	    fprintf (stderr, "read pos data error!\n");
    	    return -1;
        }
    }

    if (kb_fd >= 0 && FD_ISSET(kb_fd, in))
    {
        unsigned char scankey;

        FD_CLR(kb_fd, in);

        if (read(kb_fd, &scankey, sizeof(scankey)) == sizeof(scankey)) {
#if _DEBUG_UCB1X00
	    printf ("scankey = %d\n", scankey);
#endif
	    /* 224 where from ? */
	    if (scankey == 224) {
		return -1;
	    }

            btn_state = scankey;
            retvalue |= IAL_KEYEVENT;
        }
        else {
    	    fprintf (stderr, "read pos data error!\n");
    	    return -1;
        }
    }

    return retvalue;
}

BOOL InitUCB1X00Input (INPUT* input, const char* mdev, const char* mtype)
{
    int conffd;
    char buff[128];
    const char *p;
    int i = 0, j = 0;

    //read /etc/pointercal 
    conffd = open ("/etc/pointercal", O_RDONLY);
    if(conffd < 0) {
	    fprintf (stderr, "UCB1X00: Can not open calibrate configure\n");
	    return FALSE;	
    }
    read(conffd, buff, 128);
    while(j <7) {
	    p = &buff[i];
	    if(j == 0) a = atoi(p);
	    else if(j == 1) b = atoi(p);
	    else if(j == 2) c = atoi(p);
	    else if(j == 3) d = atoi(p);
	    else if(j == 4) e = atoi(p);
	    else if(j == 5) f = atoi(p);
	    else if(j == 6) s = atoi(p);
	    else break;
	    j ++;
	    while(buff[i] != 0x20 && buff[i] != 0) i++;
	    i ++;
    }
    close(conffd);

    ts = open ("/dev/h3600_tsraw", O_RDONLY);
    if (ts < 0) {
        fprintf (stderr, "UCB1X00: Can not open touch screen!\n");
        return FALSE;
    }

#if 0
    kb_fd = open ("/dev/kb_7289", O_RDONLY);
    if (kb_fd < 0 ) {
        fprintf (stderr, "PX255B: Can not open key board!\n");
        return FALSE;
    }
    if (ioctl(kb_fd, KDSKBMODE, K_MEDIUMRAW) < 0) {
        fprintf (stderr, "PX255B: key board ioctl error!\n");
        return FALSE;
    }
#endif

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->set_leds = NULL;

    input->wait_event = wait_event;
    pos.x = pos.y = pos.header = 0;
    
    return TRUE;
}

void TermUCB1X00Input (void)
{
    if (ts >= 0)
        close(ts);
    if (kb_fd >= 0)
	close(kb_fd);
}

#endif /* _UCB1X00_IAL */

