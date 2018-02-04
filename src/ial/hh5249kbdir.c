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
** hh5249kbdir.c: IAL Engine for HH5249 PS/2 keyboard and IrDA.
** 
** Author: Wei Yongming (2004/05/19)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common.h"

#ifdef _HH5249KBDIR_IAL

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "ial.h"
#include "hh5249kbdir.h"
#include "ps2-keycode.h"

#define PS2KBD_DEV  "/dev/ps2"
#define IRKBD_DEV   "/dev/irc"

static int ps2kbd_fd = -1;
static int irkbd_fd = -1;

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */

static int mouse_update (void)
{
    return 0;
}

static void mouse_getxy(int *x, int *y)
{
    *x = 0; *y = 0;
}

static int mouse_getbutton(void)
{
    return 0;
}
static struct _irkey_scancode_map {
    unsigned short ir_key;
    unsigned char  scancode;
} irkey_scancode_map [] =
{
#if 0
    {IRKEY_MUTE,        SCANCODE_TAB},
    {IRKEY_POWEROFF,    SCANCODE_USER+1},
    {IRKEY_1,           SCANCODE_1},
    {IRKEY_2,           SCANCODE_2},
    {IRKEY_3,           SCANCODE_3},
    {IRKEY_4,           SCANCODE_4},
    {IRKEY_5,           SCANCODE_5},
    {IRKEY_6,           SCANCODE_6},
    {IRKEY_7,           SCANCODE_7},
    {IRKEY_8,           SCANCODE_8},
    {IRKEY_9,           SCANCODE_9},
    {IRKEY_0,           SCANCODE_0},
    {IRKEY_SND_INC,     SCANCODE_CURSORBLOCKUP},
    {IRKEY_SND_DEC,     SCANCODE_CURSORBLOCKDOWN},
    {IRKEY_CHNN_INC,    SCANCODE_CURSORBLOCKLEFT},
    {IRKEY_CHNN_DEC,    SCANCODE_CURSORBLOCKRIGHT},
    {IRKEY_CLOCK,       SCANCODE_ENTER},
    {IRKEY_AV,          SCANCODE_USER+17},
    {IRKEY_BACK,        SCANCODE_USER+18},
    {IRKEY_DISP_CHNN,   SCANCODE_USER+19},
    {IRKEY_LUM_HUE,     SCANCODE_USER+20},
    {IRKEY_LH_INC,      SCANCODE_USER+21},
    {IRKEY_LH_DEC,      SCANCODE_USER+22},
#else
	{IRKEY_1,   SCANCODE_ESCAPE},
	{IRKEY_2,   SCANCODE_1},
	{IRKEY_3,   SCANCODE_2},
	{IRKEY_4,   SCANCODE_3},
	{IRKEY_5,   SCANCODE_4},
	{IRKEY_6,   SCANCODE_5},
	{IRKEY_7,   SCANCODE_6},
	{IRKEY_8,   SCANCODE_7},
	{IRKEY_9,   SCANCODE_8},
	{IRKEY_10,  SCANCODE_9},
	{IRKEY_11,  SCANCODE_0},
	{IRKEY_12,  SCANCODE_USER+2},
	{IRKEY_13,  SCANCODE_USER+3},
	{IRKEY_14,  SCANCODE_USER+4},
	{IRKEY_15,  SCANCODE_USER+5},
	{IRKEY_16,  SCANCODE_USER+6},
	{IRKEY_17,  SCANCODE_TAB},
	{IRKEY_18,  SCANCODE_USER+8},
	{IRKEY_19,  SCANCODE_ENTER},
	{IRKEY_20,  SCANCODE_USER+10},
	{IRKEY_21,  SCANCODE_USER+11},
	{IRKEY_22,  SCANCODE_BACKSPACE},
	{IRKEY_23,  SCANCODE_USER+9},
	{IRKEY_24,  SCANCODE_USER+14},
	{IRKEY_25,  SCANCODE_USER+15},
	{IRKEY_26,  SCANCODE_USER+16},
	{IRKEY_27,  SCANCODE_USER+17},
	{IRKEY_28,  SCANCODE_USER+18},
	{IRKEY_29,  SCANCODE_USER+19},
	{IRKEY_30,  SCANCODE_USER+20},
	{IRKEY_31,  SCANCODE_USER+21},
	{IRKEY_32,  SCANCODE_USER+22},
	{IRKEY_33,  SCANCODE_USER+23},
#endif
};

static unsigned char find_ir_key (unsigned short key)
{
    int i;

    for (i = 0; i < TABLESIZE (irkey_scancode_map); i++) {
        if (irkey_scancode_map [i].ir_key == key)
            return irkey_scancode_map [i].scancode;
    }

    return K_NONE;
}

static unsigned char cur_ir_scancode;
static unsigned char key_state [NR_KEYS + TABLESIZE (irkey_scancode_map)];

static int keyboard_update (void)
{
    return NR_KEYS + TABLESIZE (irkey_scancode_map);
}

static const char * keyboard_get_state(void)
{
    return (char *)key_state;
}

#ifdef  _LITE_VERSION
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#endif
{
    fd_set rfds;
    int retvalue = 0;
    int e = 0;
    unsigned short cur_ps2_key;

    if (cur_ir_scancode) {

#ifdef _LITE_VERSION
        select (maxfd + 1, in, out, except, timeout);
#else
        select (FD_SETSIZE, in, out, except, timeout);
#endif
        if (cur_ir_scancode) {
            key_state [cur_ir_scancode] = 0;
            cur_ir_scancode = 0;
            e = IAL_KEYEVENT;
        }

        return e;
    }

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if ((which & IAL_KEYEVENT)){
        if (ps2kbd_fd >= 0)
            FD_SET (ps2kbd_fd, in);
        if (irkbd_fd >= 0)
            FD_SET (irkbd_fd, in);
#ifdef _LITE_VERSION
        if (MAX (ps2kbd_fd, irkbd_fd) > maxfd) maxfd = MAX (ps2kbd_fd, irkbd_fd);
#endif
    }

#ifdef _LITE_VERSION
    e = select (maxfd + 1, in, out, except, timeout) ;
#else
    e = select (FD_SETSIZE, in, out, except, timeout) ;
#endif

    if (e > 0) { 
        if (ps2kbd_fd >= 0 && FD_ISSET (ps2kbd_fd, in)) {
            unsigned char scancode;

            cur_ps2_key = mg_ps2_converter (ps2kbd_fd);
            scancode = cur_ps2_key & 0xFF;
            if (scancode != K_NONE) {
                if (cur_ps2_key & KS_DOWN) {
                    key_state [scancode] = 1;
                }
                else if (cur_ps2_key & KS_UP) {
                    key_state [scancode] = 0;
                }
                else {
                    key_state [scancode] = (key_state [scancode]?0:1);
                }
                retvalue |= IAL_KEYEVENT;
            }

            FD_CLR (ps2kbd_fd, in);
        }

        if (irkbd_fd >= 0 && FD_ISSET(irkbd_fd, in)) {
            unsigned short ir_key;

            read (irkbd_fd, &ir_key, sizeof (ir_key));

            cur_ir_scancode = find_ir_key (ir_key);

            if (cur_ir_scancode == K_NONE) {
                fprintf (stderr, "HH5249KbdIr IAL engine: Unrecognized key from IrDA.\n");
                cur_ir_scancode = 0;
            }
            else {
                key_state [cur_ir_scancode] = 1;
                retvalue |= IAL_KEYEVENT;
            }

            FD_CLR (irkbd_fd, in);
        }
    } else if (e < 0) {
        return -1;
    }

    return retvalue;
}

BOOL InitHH5249KbdIrInput (INPUT* input, const char* mdev, const char* mtype)
{
    if ((ps2kbd_fd = open (PS2KBD_DEV, O_RDONLY)) < 0) {
        printf ("HH5249KbdIr IAL engine: Cannot open device: %s (%s)", PS2KBD_DEV, strerror (errno));
        return FALSE;
    }

    if ((irkbd_fd = open (IRKBD_DEV, O_RDONLY)) < 0) {
        close (ps2kbd_fd);
        ps2kbd_fd = -1;
        printf ("HH5249KbdIr IAL engine: Cannot open device: %s (%s)", IRKBD_DEV, strerror (errno));
        return FALSE;
    }

    memset (key_state, 0, sizeof (key_state));

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_get_state;
    input->set_leds = NULL;
    input->wait_event = wait_event;

    return TRUE;
}

void TermHH5249KbdIrInput (void)
{
    if (ps2kbd_fd >= 0)
        close (ps2kbd_fd);
    if (irkbd_fd >= 0)
        close (irkbd_fd);
}

#endif /* _HH5249KBDIR_IAL */

