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
** ps2-conv.c: convert scancode from set2 to set1.
** 
** Author: Wei Yongming (2004/05/19)
*/

#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include "common.h"

#ifdef _HH5249KBDIR_IAL

#include "ps2-keycode.h"

/* 
 * we use this map to convert scancode from set2 to set1,
 * because minigui use set1 and ps/2 keyboard gives set2.
 */
 
static unsigned char normal_keymap [0x83+1] =
{
/* 00 */  K_NONE, SCANCODE_F9, K_NONE, SCANCODE_F5, SCANCODE_F3, SCANCODE_F1, SCANCODE_F2, SCANCODE_F12,
/* 08 */  K_NONE, SCANCODE_F10 , SCANCODE_F8  , SCANCODE_F6  , SCANCODE_F4  , SCANCODE_TAB , SCANCODE_GRAVE, K_NONE,
/* 10 */  K_NONE, SCANCODE_LEFTALT, SCANCODE_LEFTSHIFT, K_NONE, SCANCODE_LEFTCONTROL, SCANCODE_Q, SCANCODE_1, K_NONE,
/* 18 */  K_NONE, K_NONE, SCANCODE_Z, SCANCODE_S, SCANCODE_A, SCANCODE_W, SCANCODE_2, K_NONE,
/* 20 */  K_NONE, SCANCODE_C, SCANCODE_X, SCANCODE_D, SCANCODE_E, SCANCODE_4, SCANCODE_3, K_NONE,
/* 28 */  K_NONE, SCANCODE_SPACE, SCANCODE_V, SCANCODE_F, SCANCODE_T, SCANCODE_R, SCANCODE_5, K_NONE,
/* 30 */  K_NONE, SCANCODE_N, SCANCODE_B, SCANCODE_H, SCANCODE_G, SCANCODE_Y, SCANCODE_6, K_NONE,
/* 38 */  K_NONE, K_NONE, SCANCODE_M, SCANCODE_J, SCANCODE_U   , SCANCODE_7, SCANCODE_8, K_NONE,
/* 40 */  K_NONE, SCANCODE_COMMA, SCANCODE_K, SCANCODE_I, SCANCODE_O, SCANCODE_0, SCANCODE_9, K_NONE,
/* 48 */  K_NONE, SCANCODE_PERIOD, SCANCODE_SLASH, SCANCODE_L, SCANCODE_SEMICOLON, SCANCODE_P, SCANCODE_MINUS, K_NONE,
/* 50 */  K_NONE, K_NONE, SCANCODE_APOSTROPHE, K_NONE, SCANCODE_BRACKET_LEFT, SCANCODE_EQUAL, K_NONE, K_NONE,
/* 58 */  SCANCODE_CAPSLOCK, SCANCODE_RIGHTSHIFT, SCANCODE_ENTER, SCANCODE_BRACKET_RIGHT, K_NONE, SCANCODE_BACKSLASH, K_NONE, K_NONE,
/* 60 */  K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, SCANCODE_BACKSPACE, K_NONE,
/* 68 */  K_NONE, SCANCODE_KEYPAD1, K_NONE, SCANCODE_KEYPAD4, SCANCODE_KEYPAD7, K_NONE, K_NONE, K_NONE,
/* 70 */  SCANCODE_KEYPAD0, SCANCODE_KEYPADPERIOD, SCANCODE_KEYPAD2, SCANCODE_KEYPAD5, SCANCODE_KEYPAD6, SCANCODE_KEYPAD8, SCANCODE_ESCAPE, SCANCODE_NUMLOCK,
/* 78 */  SCANCODE_F11, SCANCODE_KEYPADPLUS, SCANCODE_KEYPAD3, SCANCODE_KEYPADMINUS, SCANCODE_KEYPADMULTIPLY, SCANCODE_KEYPAD9, SCANCODE_SCROLLLOCK, K_NONE,
/* 80 */  K_NONE, K_NONE, K_NONE, SCANCODE_F7
};

static struct _e0_pckey_map {
    unsigned char e0_key;
    unsigned char scancode;
} e0_pckey_map [] =
{
    {0x11, SCANCODE_RIGHTALT},
    {0x14, SCANCODE_RIGHTCONTROL},
    {0x37, SCANCODE_POWER},
    {0x3f, SCANCODE_SLEEP},
    {0x4a, SCANCODE_KEYPADDIVIDE},
    {0x5a, SCANCODE_KEYPADENTER},
    {0x5e, SCANCODE_WAKEUP},
    {0x69, SCANCODE_END},
    {0x6b, SCANCODE_CURSORBLOCKLEFT},
    {0x6c, SCANCODE_HOME},
    {0x70, SCANCODE_INSERT},
    {0x71, SCANCODE_REMOVE},
    {0x72, SCANCODE_CURSORBLOCKDOWN},
    {0x74, SCANCODE_CURSORBLOCKRIGHT},
    {0x75, SCANCODE_CURSORBLOCKUP},
    {0x7a, SCANCODE_PAGEDOWN},
    {0x7c, SCANCODE_PRINTSCREEN},
    {0x7d, SCANCODE_PAGEUP},
};

static unsigned char find_e0_key (unsigned char key)
{
    int i;

    for (i = 0; i < TABLESIZE (e0_pckey_map); i++) {
        if (e0_pckey_map [i].e0_key == key)
            return e0_pckey_map [i].scancode;
    }

    return K_NONE;
}

static struct _e1_pckey_map {
    unsigned char e1_key;
    unsigned char scancode;
} e1_pckey_map [] =
{
    {0x77, SCANCODE_BREAK},
};

static unsigned char find_e1_key (unsigned char key)
{
    int i;

    for (i = 0; i < TABLESIZE (e1_pckey_map); i++) {
        if (e1_pckey_map [i].e1_key == key)
            return e1_pckey_map [i].scancode;
    }

    return K_NONE;
}

static int my_read (int fd, unsigned char* key, int count)
{
    int n;

again:
    if ((n = read (fd, key, 1)) < 0) {
        if (errno == EINTR) {
            goto again;
        }
    }

    fprintf (stderr, "read key: 0x%x", *key);
    return n;
}

unsigned short mg_ps2_converter (int fd)
{
    unsigned char key;
    unsigned short ks;
    unsigned short scancode;

    my_read (fd, &key, sizeof (unsigned char));

    switch (key) {
    case 0xf0:
        ks = KS_UP;
        my_read (fd, &key, sizeof (unsigned char));
        if (key < TABLESIZE (normal_keymap))
            scancode = normal_keymap [key];
        else
            scancode = K_NONE;
        break;

    case 0xe0:
        my_read (fd, &key, sizeof (unsigned char));
        if (key == 0xe0) {
            ks = KS_DOWN;
            my_read (fd, &key, sizeof (unsigned char));
            scancode = find_e0_key (key);
        }
        else if (key < 0x60) {
            ks = KS_UNKNOWN;
            scancode = find_e0_key (key);
        }
        else {
            ks = KS_UP;
            scancode = find_e0_key (key);
            my_read (fd, &key, sizeof (unsigned char));
            if (key != 0x12) {
                fprintf (stderr, "PS2 Converter: Bad 0xe0 data from driver.\n");
            }
        }
        break;

    case 0xe1:
        ks = KS_UNKNOWN;
        my_read (fd, &key, sizeof (unsigned char));
        scancode = find_e1_key (key);
        if (key == 0x77) {
            my_read (fd, &key, sizeof (unsigned char));
            my_read (fd, &key, sizeof (unsigned char));
            scancode = K_NONE;
        }
        break;

    default:
        ks = KS_DOWN;
        if (key < TABLESIZE (normal_keymap))
            scancode = normal_keymap [key];
        else
            scancode = K_NONE;
        break;
    }

    if (scancode == K_NONE) {
        fprintf (stderr, "PS2 Converter: Unrecognized data from driver.\n");
    }
    else {
        fprintf (stderr, "Key %s: %d\n", (ks == KS_DOWN)?"down":"up", scancode);
    }

    return ks | scancode;
}

#endif /* _HH5249KBDIR_IAL */

