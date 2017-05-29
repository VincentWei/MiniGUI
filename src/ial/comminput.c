/*
** $Id: comminput.c 8944 2007-12-29 08:29:16Z xwyan $
**
** comminput.c: Common Input Engine for eCos, uC/OS-II, VxWorks, ...
** 
** Copyright (C) 2004 ~ 2007 Feynman Software
**
** Created by Zhong Shuyi, 2004/02/29
*/

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#ifdef _MGIAL_COMM

#include "minigui.h"
#include "misc.h"
#include "ial.h"

#include "comminput.h"

#define COMM_MOUSEINPUT    0x01
#define COMM_KBINPUT       0x02

/* ----------------------------------------------------------------------- */
// OS input driver or application must implement these input functions
// hardware must be initialized before this engine can be used.

extern int __comminput_init (void);
extern int __comminput_ts_getdata (short *x, short *y, short *button);
extern int __comminput_kb_getdata (short *key, short *status);
extern int __comminput_wait_for_input (void);
extern void __comminput_deinit (void);

/* ----------------------------------------------------------------------- */

static int MOUSEX = 0, MOUSEY = 0, MOUSEBUTTON = 0;
static short KEYCODE = 0, KEYSTATUS = 0;

/************************  Low Level Input Operations **********************/
static int mouse_update (void)
{
    return 1;
}

static void mouse_getxy (int *x, int* y)
{
    *x = MOUSEX;
    *y = MOUSEY;
}

static int mouse_getbutton (void)
{
    int button = 0;
    if (MOUSEBUTTON & 0x01)
    	button |= IAL_MOUSE_LEFTBUTTON;
    else if (MOUSEBUTTON & 0x04)
    	button |= IAL_MOUSE_RIGHTBUTTON;

    MOUSEBUTTON = 0;
    return button;
}

static unsigned char kbd_state [NR_KEYS];

static int keyboard_update (void)
{
    kbd_state[KEYCODE] = KEYSTATUS;
    return KEYCODE + 1;
}

static const char* keyboard_getstate (void)
{
    return (const char*)kbd_state;
}

static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    int retvalue;

    retvalue = __comminput_wait_for_input ();
    
    if (retvalue & COMM_MOUSEINPUT) {
        short x, y, button;

        __comminput_ts_getdata (&x, &y, &button);
        if (1) {
            MOUSEX = x;
            MOUSEY = y;
        }

        //MOUSEBUTTON = (button ? 4 : 0);
        MOUSEBUTTON = button;
        retvalue = IAL_MOUSEEVENT;
    }
    else if (retvalue & COMM_KBINPUT) {
        __comminput_kb_getdata (&KEYCODE, &KEYSTATUS);

#if defined (__THREADX__) && defined (__TARGET_VFANVIL__)
        	if (kbd_state[KEYCODE] == KEYSTATUS)
        		return -1;
#endif
        retvalue = IAL_KEYEVENT;
    }
    else {
        retvalue = -1;
    }
        
    return retvalue;
}

BOOL InitCOMMInput (INPUT* input, const char* mdev, const char* mtype)
{
    /* input hardware should be initialized before this function is called */

    if (__comminput_init ())
        return FALSE;

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;
    input->suspend_mouse= NULL;
    input->resume_mouse = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->suspend_keyboard = NULL;
    input->resume_keyboard = NULL;
    input->set_leds = NULL;

    input->wait_event = wait_event;

    return TRUE;
}

void TermCOMMInput (void)
{
    __comminput_deinit ();
}

#endif /* _MGIAL_COMM */

