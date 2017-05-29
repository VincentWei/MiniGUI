/*
** $Id: ads7846.c 7335 2007-08-16 03:38:27Z xgwang $
**
** ads7846.c: Low Level Input Engine for Arca Tpanel Ads7846.
** 
** Copyright (C) 2004 ~ 2007, Feynman Software.
**
** Created by Peng Ke, 2004/05/217
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _ADS7846_IAL

#include <unistd.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ial.h"
#include "ads7846.h"


static int tpfd = -1;
static int mousex = 0;
static int mousey = 0;
static int mouse_button = 0;


static int mouse_update(void)
{
    return 1;
}

static void mouse_getxy(int *x, int* y)
{
    //fprintf(stderr, "getxy x=%4d, y=%4d\n", mousex, mousey);
    *x = mousex;
    *y = mousey;
}

static int mouse_getbutton(void)
{
    //fprintf(stderr, "getbutton %d\n", mouse_button);
    return mouse_button;
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

    if ((which & IAL_MOUSEEVENT) && tpfd >= 0) {
        FD_SET (tpfd, in);
#ifdef _LITE_VERSION
        if (tpfd > maxfd) maxfd = tpfd;
#endif
    }

#ifdef _LITE_VERSION
    if ( select (maxfd + 1, in, out, except, timeout) < 0 )
#else
    if ( select (FD_SETSIZE, in, out, except, timeout) < 0 )
#endif
	return -1;

    if (tpfd >= 0 && FD_ISSET (tpfd, in))
    {
        static POS pos = {0,0,0};
        FD_CLR (tpfd, in);

        if ( read (tpfd, &pos, sizeof (POS)) == sizeof(POS) ) {
            //fprintf(stderr, "read x=%4d, y=%4d, status=%d\n", pos.x, pos.y, pos.status);
            if(!(pos.status & TPANEL_DATA_IS_UNAVAILABLE)
                    && !(pos.status & TPANEL_DATA_IS_PENUP)
                    && (pos.x >= 0 && pos.x <= 1024 && pos.y >= 0 && pos.y <= 768) )
            {
                mousex = pos.x;
                mousey = pos.y;
                retvalue |= IAL_MOUSEEVENT;
            }
            if(pos.status & TPANEL_DATA_IS_PENDOWN)
            {
                mouse_button |= IAL_MOUSE_LEFTBUTTON;
            }
            else if(pos.status & TPANEL_DATA_IS_PENUP)
            {
                mouse_button &= ~IAL_MOUSE_LEFTBUTTON;
                retvalue |= IAL_MOUSEEVENT;
            }
        }
        else {
    	    fprintf (stderr, "read pos data error!\n");
    	    return -1;
        }
    }

    return retvalue;
}

BOOL InitAds7846Input (INPUT* input, const char* mdev, const char* mtype)
{
    tpfd = open ("/dev/tpanel", O_RDONLY);
    if (tpfd < 0) {
        fprintf (stderr, "Ads7846: Can not open touch screen!\n");
        return FALSE;
    }
    ioctl(tpfd, 14, 0, 0);
    
	input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = NULL;
    input->get_keyboard_state = NULL;
    input->set_leds = NULL;

    input->wait_event = wait_event;
    mousex = mousey = mouse_button = 0;
    
    return TRUE;
}

void TermAds7846Input (void)
{
    if (tpfd >= 0)
        close(tpfd);
}

#endif

