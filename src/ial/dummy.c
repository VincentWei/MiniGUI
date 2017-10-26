/*
** $Id: dummy.c 13830 2017-10-26 05:27:43Z weiym $
**
** dummy.c: The dummy IAL engine.
** 
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** Created by Wei Yongming, 2001/09/13
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGIAL_DUMMY

#include "misc.h"
#include "ial.h"
#include "dummy.h"

static int mouse_x, mouse_y, mouse_button;

typedef struct tagPOS
{
	short x;
	short y;
	short b;
} POS;

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update(void)
{
	return 1;
}

static void mouse_getxy (int* x, int* y)
{
	*x = mouse_x;
    *y = mouse_y;
}

static int mouse_getbutton(void)
{
	return mouse_button;
}

static int keyboard_update(void)
{
	return 0;
}

static const char * keyboard_get_state (void)
{
	return NULL;
}

static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
#ifdef _MGRM_THREADS
    __mg_os_time_delay (300);
#else
	fd_set rfds;
	int	e;

    if (!in) {
        in = &rfds;
        mg_fd_zero (in);
    }

	e = mg_select (maxfd + 1, in, out, except, timeout) ;

    if (e < 0) {
		return -1;
	}
#endif

	return 0;
}

BOOL InitDummyInput (INPUT* input, const char* mdev, const char* mtype)
{
    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_get_state;
    input->set_leds = NULL;

    input->wait_event = wait_event;
	mouse_x = 0;
	mouse_y = 0;
	mouse_button= 0;
    return TRUE;
}

void TermDummyInput (void)
{
}

#endif /* _MGIAL_DUMMY */

