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
** svgalib.c: Low Level Input Engine based on SVGALib
**
** Created by WEI Yongming, 2000/06/13
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/io.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <vga.h>
#include <vgamouse.h>
#include <vgakeyboard.h>

#include "common.h"
#include "misc.h"
#include "ial.h"
#include "svgalib.h"

/************************  Low Level Input Operations **********************/
// Mouse operations -- Event
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    return vga_waitevent (which, in, out, except, timeout);
}

#if 0
/********* the source bellow comes from MINIX PC/AT keyboard driver. ********/

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD           0x60    /* I/O port for keyboard data */

/* AT keyboard. */
#define KB_COMMAND      0x64    /* I/O port for commands on AT */
#define KB_GATE_A20     0x02    /* bit in output port to enable A20 line */
#define KB_PULSE_OUTPUT 0xF0    /* base for commands to pulse output port */
#define KB_RESET        0x01    /* bit in output port to reset CPU */
#define KB_STATUS       0x64    /* I/O port for status on AT */
#define KB_ACK          0xFA    /* keyboard ack response */
#define KB_BUSY         0x02    /* status bit set when KEYBD port ready */
#define LED_CODE        0xED    /* command to keyboard to set LEDs */
#define MAX_KB_ACK_RETRIES 0x1000       /* max #times to wait for kb ack */
#define MAX_KB_BUSY_RETRIES 0x1000      /* max #times to loop while kb busy */
#define KBIT            0x80    /* bit used to ack characters to keyboar */

/*==========================================================================*
 *                              kb_wait                                     *
 *==========================================================================*/
inline static int kb_wait(void)
{
/* Wait until the controller is ready; return zero if this times out. */

    int retries;

    retries = MAX_KB_BUSY_RETRIES + 1;
    while (--retries != 0 && inb(KB_STATUS) & KB_BUSY)
        ;                         /* wait until not busy */
    return(retries);              /* nonzero if ready */
}


/*==========================================================================*
 *                              kb_ack                                      *
 *==========================================================================*/
inline static int kb_ack(void)
{
/* Wait until kbd acknowledges last command; return zero if this times out. */

    int retries;

    retries = MAX_KB_ACK_RETRIES + 1;
    while (--retries != 0 && inb(KEYBD) != KB_ACK)
        ;                         /* wait for ack */
    return(retries);              /* nonzero if ack received */
}


/*===========================================================================*
 *                              set_leds                                     *
 *===========================================================================*/
static void set_leds (unsigned int leds)
{
/* Set the LEDs on the caps lock and num lock keys */

    kb_wait();                    /* wait for buffer empty  */
    outb(LED_CODE, KEYBD);        /* prepare keyboard to accept LED values */
    kb_ack();                     /* wait for ack response  */

    kb_wait();                    /* wait for buffer empty  */
    outb(leds, KEYBD);            /* give keyboard LED values */
    kb_ack();                     /* wait for ack response  */
}
/********* the source above comes from MINIX PC/AT keyboard driver. ********/

#else
static void set_leds (unsigned int leds)
{
    ioctl (0, KDSETLED, leds);
}
#endif

static void mouse_getxy (int* x, int* y)
{
    *x = mouse_getx ();
    *y = mouse_gety ();
}

static void mouse_setrange(int minx,int miny,int maxx,int maxy)
{
	mouse_setxrange(minx,maxx);
	mouse_setyrange(miny,maxy);
}

static int my_keyboard_update (void)
{
	if (keyboard_update ())
        return NR_KEYS;

    return 0;
}

/******************  Initialization and termination of SVGALib **************/
static int MouseType (const char* mtype)
{
    if (strcasecmp ("Microsoft", mtype) == 0)
        return MOUSE_MICROSOFT;
    if (strcasecmp ("MouseSystems", mtype) == 0)
        return MOUSE_MOUSESYSTEMS;
    if (strcasecmp ("MMSeries", mtype) == 0)
        return MOUSE_MMSERIES;
    if (strcasecmp ("Logitech", mtype) == 0)
        return MOUSE_LOGIMAN;
    if (strcasecmp ("Busmouse", mtype) == 0)
        return MOUSE_BUSMOUSE;
    if (strcasecmp ("PS2", mtype) == 0)
        return MOUSE_PS2;
    if (strcasecmp ("MouseMan", mtype) == 0)
        return MOUSE_IMPS2;
    if (strcasecmp ("Spaceball", mtype) == 0)
        return MOUSE_SPACEBALL;
    if (strcasecmp ("IntelliMouse", mtype) == 0)
        return MOUSE_INTELLIMOUSE;
    if (strcasecmp ("None", mtype) == 0)
        return MOUSE_NONE;

    return -1;
}

BOOL InitSVGALibInput (INPUT* input, const char* mdev, const char* mtype)
{
    int type;

    // Use raw keyboard.
    keyboard_translatekeys (TRANSLATE_KEYPADENTER
                            | DONT_CATCH_CTRLC);

#if 0
    // Get the keyboard port access permission.
    if (ioperm (KEYBD, 5, 1)) {
        fprintf (stderr, "IAL: Can not get keyboard I/O permission!\n");
        return FALSE;
    }
#endif

    if (keyboard_init ()) {
        fprintf (stderr, "IAL: Can not init keyboard!\n");
        return FALSE;
    }

    if ((type = MouseType (mtype)) == -1) {
        fprintf (stderr, "IAL: Get mouse type error! Please check your 'MiniGUI.cfg'\n");
        return FALSE;
    }

    if (mouse_init ((char*)mdev, type, MOUSE_DEFAULTSAMPLERATE)) {
        fprintf (stderr, "IAL: Can not init mouse!\n");
        return FALSE;
    }

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = mouse_setposition;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = mouse_setrange;

    input->update_keyboard = my_keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->set_leds = set_leds;

    input->wait_event = wait_event;

    return TRUE;
}

void TermSVGALibInput (void)
{
    keyboard_close ();
    mouse_close ();

#if 0
    ioperm (KEYBD, 5, FALSE);
#endif
}

