///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** linux-libinput.c: The implementation of the IAL engine based on libinput.
**
** Created by Wei Yongming, 2019/06/10
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGIAL_LIBINPUT

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libudev.h>
#include <libinput.h>

#include "minigui.h"
#include "misc.h"
#include "ial.h"
#include "linux-tty.h"
#include "linux-libinput.h"

#define LEN_SEAT_ID     127

struct _libinput_udev_context {
    char seat_id[LEN_SEAT_ID + 1];

    struct udev *udev;
    struct libinput *li;
    struct libinput_device *def_dev;
    int suspended;
    int vt_switch;

    int min_x, max_x, min_y, max_y;

    int li_fd;
    int mouse_x, mouse_y, mouse_button;
    int last_keycode;
    char kbd_state [NR_KEYS];
};

static struct _libinput_udev_context my_ctxt;

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static void mouse_setrange (int newminx, int newminy, int newmaxx, int newmaxy)
{
    my_ctxt.min_x = newminx;
    my_ctxt.max_x = newmaxx;
    my_ctxt.min_y = newminy;
    my_ctxt.max_y = newmaxy;

    if (my_ctxt.mouse_x < my_ctxt.min_x)
        my_ctxt.mouse_x = my_ctxt.min_x;
    if (my_ctxt.mouse_x > my_ctxt.max_x)
        my_ctxt.mouse_x = my_ctxt.max_x;
    if (my_ctxt.mouse_y < my_ctxt.min_y)
        my_ctxt.mouse_y = my_ctxt.min_y;
    if (my_ctxt.mouse_y > my_ctxt.max_y)
        my_ctxt.mouse_y = my_ctxt.max_y;

    my_ctxt.mouse_x = (my_ctxt.min_x + my_ctxt.max_x) / 2;
    my_ctxt.mouse_y = (my_ctxt.min_y + my_ctxt.max_y) / 2;
}

static void mouse_setxy (int newx, int newy)
{
    if (newx < my_ctxt.min_x)
        newx = my_ctxt.min_x;
    if (newx > my_ctxt.max_x)
        newx = my_ctxt.max_x;
    if (newy < my_ctxt.min_y)
        newy = my_ctxt.min_y;
    if (newy > my_ctxt.max_y)
        newy = my_ctxt.max_y;

    if (newx == my_ctxt.mouse_x && newy == my_ctxt.mouse_y)
        return;

    my_ctxt.mouse_x = newx;
    my_ctxt.mouse_x = newy;
}

static int mouse_update(void)
{
    return 1;
}

static void mouse_getxy (int* x, int* y)
{
    *x = my_ctxt.mouse_x;
    *y = my_ctxt.mouse_y;
}

static int mouse_getbutton(void)
{
    return my_ctxt.mouse_button;
}

static int keyboard_update(void)
{
    if (my_ctxt.last_keycode == 0)
        return 0;

    if (MG_UNLIKELY (my_ctxt.vt_switch &&
            my_ctxt.kbd_state[SCANCODE_LEFTCONTROL] &&
            my_ctxt.kbd_state[SCANCODE_LEFTALT] &&
            (my_ctxt.last_keycode >= SCANCODE_F1) &&
            (my_ctxt.last_keycode <= SCANCODE_F10) &&
            my_ctxt.kbd_state[my_ctxt.last_keycode])) {

        if (__mg_linux_tty_switch_vt(my_ctxt.last_keycode - SCANCODE_F1 + 1) == 0) {
            my_ctxt.kbd_state[my_ctxt.last_keycode] = 0;
            my_ctxt.kbd_state[SCANCODE_LEFTCONTROL] = 0;
            my_ctxt.kbd_state[SCANCODE_LEFTALT] = 0;
            return 0;
        }
    }

    return my_ctxt.last_keycode + 1;
}

static const char * keyboard_getstate (void)
{
    return my_ctxt.kbd_state;
}

#include <linux/input-event-codes.h>

#ifdef _DEBUG

static unsigned char linux_keycode_to_scancode_map[] = {
    SCANCODE_RESERVED, // KEY_RESERVED (0)
    SCANCODE_ESCAPE, // KEY_ESC (1)
    SCANCODE_1, // KEY_1 (2)
    SCANCODE_2, // KEY_2 (3)
    SCANCODE_3, // KEY_3 (4)
    SCANCODE_4, // KEY_4 (5)
    SCANCODE_5, // KEY_5 (6)
    SCANCODE_6, // KEY_6 (7)
    SCANCODE_7, // KEY_7 (8)
    SCANCODE_8, // KEY_8 (9)
    SCANCODE_9, // KEY_9 (10)
    SCANCODE_0, // KEY_0 (11)
    SCANCODE_MINUS, // KEY_MINUS (12)
    SCANCODE_EQUAL, // KEY_EQUAL (13)
    SCANCODE_BACKSPACE, // KEY_BACKSPACE (14))
    SCANCODE_TAB, // KEY_TAB (15)
    SCANCODE_Q, // KEY_Q (16)
    SCANCODE_W, // KEY_W (17)
    SCANCODE_E, // KEY_E (18)
    SCANCODE_R, // KEY_R (19)
    SCANCODE_T, // KEY_T (20)
    SCANCODE_Y, // KEY_Y (21)
    SCANCODE_U, // KEY_U (22)
    SCANCODE_I, // KEY_I (23)
    SCANCODE_O, // KEY_O (24)
    SCANCODE_P, // KEY_P (25)
    SCANCODE_LEFTBRACE, // KEY_LEFTBRACE (26)
    SCANCODE_RIGHTBRACE, // KEY_RIGHTBRACE (27)
    SCANCODE_ENTER, // KEY_ENTER (28)
    SCANCODE_LEFTCTRL, // KEY_LEFTCTRL (29)
    SCANCODE_A, // KEY_A (30)
    SCANCODE_S, // KEY_S (31)
    SCANCODE_D, // KEY_D (32)
    SCANCODE_F, // KEY_F (33)
    SCANCODE_G, // KEY_G (34)
    SCANCODE_H, // KEY_H (35)
    SCANCODE_J, // KEY_J (36)
    SCANCODE_K, // KEY_K (37)
    SCANCODE_L, // KEY_L (38)
    SCANCODE_SEMICOLON, // KEY_SEMICOLON (39)
    SCANCODE_APOSTROPHE, // KEY_APOSTROPHE (40)
    SCANCODE_GRAVE, // KEY_GRAVE (41)
    SCANCODE_LEFTSHIFT, // KEY_LEFTSHIFT (42)
    SCANCODE_BACKSLASH, // KEY_BACKSLASH (43)
    SCANCODE_Z, // KEY_Z (44)
    SCANCODE_X, // KEY_X (45)
    SCANCODE_C, // KEY_C (46)
    SCANCODE_V, // KEY_V (47)
    SCANCODE_B, // KEY_B (48)
    SCANCODE_N, // KEY_N (49)
    SCANCODE_M, // KEY_M (50)
    SCANCODE_COMMA, // KEY_COMMA (51)
    SCANCODE_DOT, // KEY_DOT (52)
    SCANCODE_SLASH, // KEY_SLASH (53)
    SCANCODE_RIGHTSHIFT, // KEY_RIGHTSHIFT (54)
    SCANCODE_KPASTERISK, // KEY_KPASTERISK (55)
    SCANCODE_LEFTALT, // KEY_LEFTALT (56)
    SCANCODE_SPACE, // KEY_SPACE (57)
    SCANCODE_CAPSLOCK, // KEY_CAPSLOCK (58)
    SCANCODE_F1, // KEY_F1 (59)
    SCANCODE_F2, // KEY_F2 (60)
    SCANCODE_F3, // KEY_F3 (61)
    SCANCODE_F4, // KEY_F4 (62)
    SCANCODE_F5, // KEY_F5 (63)
    SCANCODE_F6, // KEY_F6 (64)
    SCANCODE_F7, // KEY_F7 (65)
    SCANCODE_F8, // KEY_F8 (66)
    SCANCODE_F9, // KEY_F9 (67)
    SCANCODE_F10, // KEY_F10 (68)
    SCANCODE_NUMLOCK, // KEY_NUMLOCK (69)
    SCANCODE_SCROLLLOCK, // KEY_SCROLLLOCK (70)
    SCANCODE_KP7, // KEY_KP7 (71)
    SCANCODE_KP8, // KEY_KP8 (72)
    SCANCODE_KP9, // KEY_KP9 (73)
    SCANCODE_KPMINUS, // KEY_KPMINUS (74)
    SCANCODE_KP4, // KEY_KP4 (75)
    SCANCODE_KP5, // KEY_KP5 (76)
    SCANCODE_KP6, // KEY_KP6 (77)
    SCANCODE_KPPLUS, // KEY_KPPLUS (78)
    SCANCODE_KP1, // KEY_KP1 (79)
    SCANCODE_KP2, // KEY_KP2 (80)
    SCANCODE_KP3, // KEY_KP3 (81)
    SCANCODE_KP0, // KEY_KP0 (82)
    SCANCODE_KPDOT, // KEY_KPDOT (83)
    SCANCODE_RESERVED, // NOT DEFINED (84)
    SCANCODE_ZENKAKUHANKAKU, // KEY_ZENKAKUHANKAKU (85)
    SCANCODE_102ND, // KEY_102ND (86)
    SCANCODE_F11, // KEY_F11 (87)
    SCANCODE_F12, // KEY_F12 (88)
    SCANCODE_RO, // KEY_RO (89)
    SCANCODE_KATAKANA, // KEY_KATAKANA (90)
    SCANCODE_HIRAGANA, // KEY_HIRAGANA (91)
    SCANCODE_HENKAN, // KEY_HENKAN (92)
    SCANCODE_KATAKANAHIRAGANA, // KEY_KATAKANAHIRAGANA (93)
    SCANCODE_MUHENKAN, // KEY_MUHENKAN (94)
    SCANCODE_KPJPCOMMA, // KEY_KPJPCOMMA (95)
    SCANCODE_KPENTER, // KEY_KPENTER (96)
    SCANCODE_RIGHTCTRL, // KEY_RIGHTCTRL (97)
    SCANCODE_KPSLASH, // KEY_KPSLASH (98)
    SCANCODE_SYSRQ, // KEY_SYSRQ (99)
    SCANCODE_RIGHTALT, // KEY_RIGHTALT (100)
    SCANCODE_LINEFEED, // KEY_LINEFEED (101)
    SCANCODE_HOME, // KEY_HOME (102)
    SCANCODE_UP, // KEY_UP (103)
    SCANCODE_PAGEUP, // KEY_PAGEUP (104)
    SCANCODE_LEFT, // KEY_LEFT (105)
    SCANCODE_RIGHT, // KEY_RIGHT (106)
    SCANCODE_END, // KEY_END (107)
    SCANCODE_DOWN, // KEY_DOWN (108)
    SCANCODE_PAGEDOWN, // KEY_PAGEDOWN (109)
    SCANCODE_INSERT, // KEY_INSERT (110)
    SCANCODE_DELETE, // KEY_DELETE (111)
    SCANCODE_MACRO, // KEY_MACRO (112)
    SCANCODE_MUTE, // KEY_MUTE (113)
    SCANCODE_VOLUMEDOWN, // KEY_VOLUMEDOWN (114)
    SCANCODE_VOLUMEUP, // KEY_VOLUMEUP (115)
    SCANCODE_POWER, // KEY_POWER (116)    /* SC System Power Down */
    SCANCODE_KPEQUAL, // KEY_KPEQUAL (117)
    SCANCODE_KPPLUSMINUS, // KEY_KPPLUSMINUS (118)
    SCANCODE_PAUSE, // KEY_PAUSE (119)
    SCANCODE_SCALE, // KEY_SCALE (120)    /* AL Compiz Scale (Expose) */
    SCANCODE_KPCOMMA, // KEY_KPCOMMA (121)
    SCANCODE_HANGEUL, // KEY_HANGEUL (122)
    SCANCODE_HANJA, // KEY_HANJA (123)
    SCANCODE_YEN, // KEY_YEN (124)
    SCANCODE_LEFTMETA, // KEY_LEFTMETA (125)
    SCANCODE_RIGHTMETA, // KEY_RIGHTMETA (126)
    SCANCODE_COMPOSE, // KEY_COMPOSE (127)
    SCANCODE_STOP, // KEY_STOP (128)    /* AC Stop */
    SCANCODE_AGAIN, // KEY_AGAIN (129)
    SCANCODE_PROPS, // KEY_PROPS (130)    /* AC Properties */
    SCANCODE_UNDO, // KEY_UNDO (131)    /* AC Undo */
    SCANCODE_FRONT, // KEY_FRONT (132)
    SCANCODE_COPY, // KEY_COPY (133)    /* AC Copy */
    SCANCODE_OPEN, // KEY_OPEN (134)    /* AC Open */
    SCANCODE_PASTE, // KEY_PASTE (135)    /* AC Paste */
    SCANCODE_FIND, // KEY_FIND (136)    /* AC Search */
    SCANCODE_CUT, // KEY_CUT (137)    /* AC Cut */
    SCANCODE_HELP, // KEY_HELP (138)    /* AL Integrated Help Center */
    SCANCODE_MENU, // KEY_MENU (139)    /* Menu (show menu) */
    SCANCODE_CALC, // KEY_CALC (140)    /* AL Calculator */
    SCANCODE_SETUP, // KEY_SETUP (141)
    SCANCODE_SLEEP, // KEY_SLEEP (142)    /* SC System Sleep */
    SCANCODE_WAKEUP, // KEY_WAKEUP (143)    /* System Wake Up */
    SCANCODE_FILE, // KEY_FILE (144)    /* AL Local Machine Browser */
    SCANCODE_SENDFILE, // KEY_SENDFILE (145)
    SCANCODE_DELETEFILE, // KEY_DELETEFILE (146)
    SCANCODE_XFER, // KEY_XFER (147)
    SCANCODE_PROG1, // KEY_PROG1 (148)
    SCANCODE_PROG2, // KEY_PROG2 (149)
    SCANCODE_WWW, // KEY_WWW (150)    /* AL Internet Browser */
    SCANCODE_MSDOS, // KEY_MSDOS (151)
    SCANCODE_COFFEE, // KEY_COFFEE (152)    /* AL Terminal Lock/Screensaver */
    // SCANCODE_SCREENLOCK, // KEY_SCREENLOCK (KEY_COFFEE)
    SCANCODE_ROTATE_DISPLAY, // KEY_ROTATE_DISPLAY (153)    /* Display orientation for e.g. tablets */
    // SCANCODE_DIRECTION, // KEY_DIRECTION (KEY_ROTATE_DISPLAY)
    SCANCODE_CYCLEWINDOWS, // KEY_CYCLEWINDOWS (154)
    SCANCODE_MAIL, // KEY_MAIL (155)
    SCANCODE_BOOKMARKS, // KEY_BOOKMARKS (156)    /* AC Bookmarks */
    SCANCODE_COMPUTER, // KEY_COMPUTER (157)
    SCANCODE_BACK, // KEY_BACK (158)    /* AC Back */
    SCANCODE_FORWARD, // KEY_FORWARD (159)    /* AC Forward */
    SCANCODE_CLOSECD, // KEY_CLOSECD (160)
    SCANCODE_EJECTCD, // KEY_EJECTCD (161)
    SCANCODE_EJECTCLOSECD, // KEY_EJECTCLOSECD (162)
    SCANCODE_NEXTSONG, // KEY_NEXTSONG (163)
    SCANCODE_PLAYPAUSE, // KEY_PLAYPAUSE (164)
    SCANCODE_PREVIOUSSONG, // KEY_PREVIOUSSONG (165)
    SCANCODE_STOPCD, // KEY_STOPCD (166)
    SCANCODE_RECORD, // KEY_RECORD (167)
    SCANCODE_REWIND, // KEY_REWIND (168)
    SCANCODE_PHONE, // KEY_PHONE (169)    /* Media Select Telephone */
    SCANCODE_ISO, // KEY_ISO (170)
    SCANCODE_CONFIG, // KEY_CONFIG (171)    /* AL Consumer Control Configuration */
    SCANCODE_HOMEPAGE, // KEY_HOMEPAGE (172)    /* AC Home */
    SCANCODE_REFRESH, // KEY_REFRESH (173)    /* AC Refresh */
    SCANCODE_EXIT, // KEY_EXIT (174)    /* AC Exit */
    SCANCODE_MOVE, // KEY_MOVE (175)
    SCANCODE_EDIT, // KEY_EDIT (176)
    SCANCODE_SCROLLUP, // KEY_SCROLLUP (177)
    SCANCODE_SCROLLDOWN, // KEY_SCROLLDOWN (178)
    SCANCODE_KPLEFTPAREN, // KEY_KPLEFTPAREN (179)
    SCANCODE_KPRIGHTPAREN, // KEY_KPRIGHTPAREN (180)
    SCANCODE_NEW, // KEY_NEW (181)    /* AC New */
    SCANCODE_REDO, // KEY_REDO (182)    /* AC Redo/Repeat */
    SCANCODE_F13, // KEY_F13 (183)
    SCANCODE_F14, // KEY_F14 (184)
    SCANCODE_F15, // KEY_F15 (185)
    SCANCODE_F16, // KEY_F16 (186)
    SCANCODE_F17, // KEY_F17 (187)
    SCANCODE_F18, // KEY_F18 (188)
    SCANCODE_F19, // KEY_F19 (189)
    SCANCODE_F20, // KEY_F20 (190)
    SCANCODE_F21, // KEY_F21 (191)
    SCANCODE_F22, // KEY_F22 (192)
    SCANCODE_F23, // KEY_F23 (193)
    SCANCODE_F24, // KEY_F24 (194)
    SCANCODE_RESERVED, // 195
    SCANCODE_RESERVED, // 196
    SCANCODE_RESERVED, // 197
    SCANCODE_RESERVED, // 198
    SCANCODE_RESERVED, // 199
    SCANCODE_PLAYCD, // KEY_PLAYCD (200)
    SCANCODE_PAUSECD, // KEY_PAUSECD (201)
    SCANCODE_PROG3, // KEY_PROG3 (202)
    SCANCODE_PROG4, // KEY_PROG4 (203)
    SCANCODE_DASHBOARD, // KEY_DASHBOARD (204)    /* AL Dashboard */
    SCANCODE_SUSPEND, // KEY_SUSPEND (205)
    SCANCODE_CLOSE, // KEY_CLOSE (206)    /* AC Close */
    SCANCODE_PLAY, // KEY_PLAY (207)
    SCANCODE_FASTFORWARD, // KEY_FASTFORWARD (208)
    SCANCODE_BASSBOOST, // KEY_BASSBOOST (209)
    SCANCODE_PRINT, // KEY_PRINT (210)    /* AC Print */
    SCANCODE_HP, // KEY_HP (211)
    SCANCODE_CAMERA, // KEY_CAMERA (212)
    SCANCODE_SOUND, // KEY_SOUND (213)
    SCANCODE_QUESTION, // KEY_QUESTION (214)
    SCANCODE_EMAIL, // KEY_EMAIL (215)
    SCANCODE_CHAT, // KEY_CHAT (216)
    SCANCODE_SEARCH, // KEY_SEARCH (217)
    SCANCODE_CONNECT, // KEY_CONNECT (218)
    SCANCODE_FINANCE, // KEY_FINANCE (219)    /* AL Checkbook/Finance */
    SCANCODE_SPORT, // KEY_SPORT (220)
    SCANCODE_SHOP, // KEY_SHOP (221)
    SCANCODE_ALTERASE, // KEY_ALTERASE (222)
    SCANCODE_CANCEL, // KEY_CANCEL (223)    /* AC Cancel */
    SCANCODE_BRIGHTNESSDOWN, // KEY_BRIGHTNESSDOWN (224)
    SCANCODE_BRIGHTNESSUP, // KEY_BRIGHTNESSUP (225)
    SCANCODE_MEDIA, // KEY_MEDIA (226)
    SCANCODE_SWITCHVIDEOMODE, // KEY_SWITCHVIDEOMODE (227)    /* Cycle between available video outputs (Monitor/LCD/TV-out/etc) */
    SCANCODE_KBDILLUMTOGGLE, // KEY_KBDILLUMTOGGLE (228)
    SCANCODE_KBDILLUMDOWN, // KEY_KBDILLUMDOWN (229)
    SCANCODE_KBDILLUMUP, // KEY_KBDILLUMUP (230)
    SCANCODE_SEND, // KEY_SEND (231)    /* AC Send */
    SCANCODE_REPLY, // KEY_REPLY (232)    /* AC Reply */
    SCANCODE_FORWARDMAIL, // KEY_FORWARDMAIL (233)    /* AC Forward Msg */
    SCANCODE_SAVE, // KEY_SAVE (234)    /* AC Save */
    SCANCODE_DOCUMENTS, // KEY_DOCUMENTS (235)
    SCANCODE_BATTERY, // KEY_BATTERY (236)
    SCANCODE_BLUETOOTH, // KEY_BLUETOOTH (237)
    SCANCODE_WLAN, // KEY_WLAN (238)
    SCANCODE_UWB, // KEY_UWB (239)
    SCANCODE_UNKNOWN, // KEY_UNKNOWN (240)
    SCANCODE_VIDEO_NEXT, // KEY_VIDEO_NEXT (241)    /* drive next video source */
    SCANCODE_VIDEO_PREV, // KEY_VIDEO_PREV (242)    /* drive previous video source */
    SCANCODE_BRIGHTNESS_CYCLE, // KEY_BRIGHTNESS_CYCLE (243)    /* brightness up, after max is min */
    SCANCODE_BRIGHTNESS_AUTO, // KEY_BRIGHTNESS_AUTO (244)    /* Set Auto Brightness: manual brightness control is off, rely on ambient */
    // SCANCODE_BRIGHTNESS_ZERO, // KEY_BRIGHTNESS_ZERO (KEY_BRIGHTNESS_AUTO)
    SCANCODE_DISPLAY_OFF, // KEY_DISPLAY_OFF (245)    /* display device to off state */
    SCANCODE_WWAN, // KEY_WWAN (246)    /* Wireless WAN (LTE, UMTS, GSM, etc.) */
    // SCANCODE_WIMAX, // KEY_WWAN
    SCANCODE_RFKILL, // KEY_RFKILL (247)    /* Key that controls all radios */
    SCANCODE_MICMUTE, // KEY_MICMUTE (248)    /* Mute / unmute the microphone */
};

static int translate_libinput_keycode(uint32_t keycode)
{
    unsigned char scancode;
    if (keycode > TABLESIZE(linux_keycode_to_scancode_map))
        return 0;

    scancode = linux_keycode_to_scancode_map[keycode];
    _MG_PRINTF("translate keycode %u to scancode: %u\n", keycode, scancode);
    return scancode;
}

#else /* _DEBUG */

static inline int translate_libinput_keycode(uint32_t keycode)
{
    if (keycode > NR_KEYS)
        return 0;

    return (int)keycode;
}

#endif  /* !_DEBUG */

static void normalize_mouse_pos(int* new_x, int* new_y)
{
    if (*new_x < my_ctxt.min_x)
        *new_x = my_ctxt.min_x;
    if (*new_x > my_ctxt.max_x)
        *new_x = my_ctxt.max_x;
    if (*new_y < my_ctxt.min_y)
        *new_y = my_ctxt.min_y;
    if (*new_y > my_ctxt.max_y)
        *new_y = my_ctxt.max_y;
}

static BOOL on_mouse_moved (double dx, double dy)
{
    int new_x = (int)(my_ctxt.mouse_x + dx + 0.5);
    int new_y = (int)(my_ctxt.mouse_y + dy + 0.5);

    _DBG_PRINTF("%s: new mouse position: %d, %d (old: %d, %d; delta: %f, %f)\n",
            __FUNCTION__, new_x, new_y, my_ctxt.mouse_x, my_ctxt.mouse_y, dx, dy);

    normalize_mouse_pos(&new_x, &new_y);
    if (new_x == my_ctxt.mouse_x && new_y == my_ctxt.mouse_y)
        return FALSE;

    my_ctxt.mouse_x = new_x;
    my_ctxt.mouse_y = new_y;
    return TRUE;
}

static BOOL on_new_mouse_pos (double x, double y)
{
    int new_x = (int)(x + 0.5);
    int new_y = (int)(y + 0.5);

    _DBG_PRINTF("%s: new mouse position: %d, %d (old: %d, %d)\n",
            __FUNCTION__, new_x, new_y, my_ctxt.mouse_x, my_ctxt.mouse_y);

    normalize_mouse_pos(&new_x, &new_y);
    if (new_x == my_ctxt.mouse_x && new_y == my_ctxt.mouse_y)
        return FALSE;

    my_ctxt.mouse_x = new_x;
    my_ctxt.mouse_y = new_y;
    return TRUE;
}

static BOOL on_mouse_button_changed (uint32_t button,
        enum libinput_button_state state)
{
    int old_mouse_button = my_ctxt.mouse_button;

    switch (button) {
    case BTN_LEFT:
        if (state == LIBINPUT_BUTTON_STATE_RELEASED)
            my_ctxt.mouse_button &= ~IAL_MOUSE_LEFTBUTTON;
        else
            my_ctxt.mouse_button |= IAL_MOUSE_LEFTBUTTON;
        break;

    case BTN_RIGHT:
        if (state == LIBINPUT_BUTTON_STATE_RELEASED)
            my_ctxt.mouse_button &= ~IAL_MOUSE_RIGHTBUTTON;
        else
            my_ctxt.mouse_button |= IAL_MOUSE_RIGHTBUTTON;
        break;

    case BTN_MIDDLE:
        if (state == LIBINPUT_BUTTON_STATE_RELEASED)
            my_ctxt.mouse_button &= ~IAL_MOUSE_MIDDLEBUTTON;
        else
            my_ctxt.mouse_button |= IAL_MOUSE_MIDDLEBUTTON;
        break;

    default:
        // not a standard mouse button.
        return FALSE;
    }

    _DBG_PRINTF("%s: new mouse button: 0x%x (old: 0x%x)\n",
            __FUNCTION__, my_ctxt.mouse_button, old_mouse_button);
    if (old_mouse_button == my_ctxt.mouse_button)
        return FALSE;

    return TRUE;
}

static int wait_event_ex (int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout, EXTRA_INPUT_EVENT* extra)
{
    struct libinput_event *event;
    enum libinput_event_type type;
    int retval;

    event = libinput_get_event(my_ctxt.li);
    if (event == NULL) {
        fd_set rfds;

        if (!in) {
            in = &rfds;
            FD_ZERO(in);
        }

        FD_SET(my_ctxt.li_fd, in);

        if (my_ctxt.li_fd > maxfd)
            maxfd = my_ctxt.li_fd;

        retval = select (maxfd + 1, in, out, except, timeout);
        if (retval > 0 && FD_ISSET (my_ctxt.li_fd, in)) {
            libinput_dispatch(my_ctxt.li);
            event = libinput_get_event(my_ctxt.li);
            if (event == NULL) {
                return -1;
            }
        }
        else if (retval < 0) {
            _DBG_PRINTF("IAL>LIBINPUT: select returns < 0: %d\n", retval);
            return -1;
        }
        else {
            return 0;
        }
    }

    type = libinput_event_get_type(event);

    /* set default return value */
    retval = -1;
    switch (type) {
    case LIBINPUT_EVENT_NONE:
        _DBG_PRINTF("IAL>LIBINPUT: got a NONE event\n");
        break;

    case LIBINPUT_EVENT_DEVICE_ADDED: {
        struct libinput_device *device;
        struct libinput_seat *seat;
        const char *seat_name;

        device = libinput_event_get_device(event);
        seat = libinput_device_get_seat(device);
        seat_name = libinput_seat_get_logical_name(seat);
        _MG_PRINTF("IAL>LIBINPUT: a new event device added: %s\n", seat_name);
        break;
    }

    case LIBINPUT_EVENT_DEVICE_REMOVED: {
        struct libinput_device *device;
        struct libinput_seat *seat;
        const char *seat_name;

        device = libinput_event_get_device(event);
        seat = libinput_device_get_seat(device);
        seat_name = libinput_seat_get_logical_name(seat);
        _MG_PRINTF("IAL>LIBINPUT: an event device removed: %s\n", seat_name);
        break;
    }

    case LIBINPUT_EVENT_KEYBOARD_KEY: {
        struct libinput_event_keyboard* kbd_event;

        kbd_event = libinput_event_get_keyboard_event(event);
        if (kbd_event) {
            uint32_t keycode = libinput_event_keyboard_get_key(kbd_event);
            my_ctxt.last_keycode = translate_libinput_keycode(keycode);
            if (my_ctxt.last_keycode) {
                switch (libinput_event_keyboard_get_key_state(kbd_event)) {
                case LIBINPUT_KEY_STATE_RELEASED:
                    my_ctxt.kbd_state[my_ctxt.last_keycode] = 0;
                    break;
                case LIBINPUT_KEY_STATE_PRESSED:
                    my_ctxt.kbd_state[my_ctxt.last_keycode] = 1;
                    break;
                }

                retval = IAL_EVENT_KEY;
            }
        }
        break;
    }

    case LIBINPUT_EVENT_POINTER_MOTION: {
        struct libinput_event_pointer* ptr_event;
        double dx, dy;

        ptr_event = libinput_event_get_pointer_event(event);
        dx = libinput_event_pointer_get_dx(ptr_event);
        dy = libinput_event_pointer_get_dy(ptr_event);
        if (on_mouse_moved(dx, dy))
            retval = IAL_EVENT_MOUSE;
        break;
    }

    case LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE: {
        struct libinput_event_pointer* ptr_event;
        double x, y;

        ptr_event = libinput_event_get_pointer_event(event);
        x = libinput_event_pointer_get_absolute_x_transformed(ptr_event,
                my_ctxt.max_x - my_ctxt.min_x + 1) + my_ctxt.min_x;
        y = libinput_event_pointer_get_absolute_y_transformed(ptr_event,
                my_ctxt.max_y - my_ctxt.min_y + 1) + my_ctxt.min_y;
        if (on_new_mouse_pos(x, y))
            retval = IAL_EVENT_MOUSE;
        break;
    }

    case LIBINPUT_EVENT_POINTER_BUTTON: {
        struct libinput_event_pointer* ptr_event;
        uint32_t button;
        enum libinput_button_state state;

        ptr_event = libinput_event_get_pointer_event(event);
        button = libinput_event_pointer_get_button(ptr_event);
        state = libinput_event_pointer_get_button_state(ptr_event);

        retval = 0;
        if (button >= BTN_LEFT && button <= BTN_MIDDLE) {
            if (on_mouse_button_changed(button, state))
                retval |= IAL_EVENT_MOUSE;
        }

        if (state == LIBINPUT_BUTTON_STATE_PRESSED)
            extra->event = IAL_EVENT_BUTTONDOWN;
        else
            extra->event = IAL_EVENT_BUTTONUP;
        extra->wparam = button;
        extra->lparam = libinput_event_pointer_get_seat_button_count(ptr_event);
        retval |= IAL_EVENT_EXTRA;
        break;
    }

    case LIBINPUT_EVENT_POINTER_AXIS: {
        struct libinput_event_pointer* ptr_event;
        enum libinput_pointer_axis_source li_source;
        int scroll = AXIS_SCROLL_INVALID;
        int source = AXIS_SOURCE_INVALID;
        double tmp;
        int value = 0, value_discrete = 0;

        ptr_event = libinput_event_get_pointer_event(event);
        li_source = libinput_event_pointer_get_axis_source(ptr_event);
        if (li_source) {
            if (libinput_event_pointer_has_axis(ptr_event,
                    LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL)) {
                scroll = AXIS_SCROLL_VERTICAL;
                tmp = libinput_event_pointer_get_axis_value(ptr_event,
                        LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL);
                value = (int)(tmp + 0.5);
                tmp = libinput_event_pointer_get_axis_value_discrete(ptr_event,
                        LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL);
                value_discrete = (int)(tmp + 0.5);
            }
            else if (libinput_event_pointer_has_axis(ptr_event,
                    LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL)) {
                scroll = AXIS_SCROLL_HORIZONTAL;
                tmp = libinput_event_pointer_get_axis_value(ptr_event,
                        LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL);
                value = (int)(tmp + 0.5);
                tmp = libinput_event_pointer_get_axis_value_discrete(ptr_event,
                        LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL);
                value_discrete = (int)(tmp + 0.5);
            }

            switch (li_source) {
            case LIBINPUT_POINTER_AXIS_SOURCE_WHEEL:
                source = AXIS_SOURCE_WHEEL;
                break;
            case LIBINPUT_POINTER_AXIS_SOURCE_FINGER:
                source = AXIS_SOURCE_FINGER;
                break;
            case LIBINPUT_POINTER_AXIS_SOURCE_CONTINUOUS:
                source = AXIS_SOURCE_CONTINUOUS;
                break;
            case LIBINPUT_POINTER_AXIS_SOURCE_WHEEL_TILT:
                source = AXIS_SOURCE_WHEEL_TILT;
                break;
            }

            if (scroll == AXIS_SCROLL_INVALID || source == AXIS_SOURCE_INVALID)
                break;

            extra->event = IAL_EVENT_AXIS;
            extra->wparam = MAKELONG(scroll, source);
            extra->lparam = MAKELONG(value, value_discrete);
            retval = IAL_EVENT_EXTRA;
        }

        break;
    }

    case LIBINPUT_EVENT_TOUCH_DOWN: {
        struct libinput_event_touch* tch_event;
        double x, y;

        tch_event = libinput_event_get_touch_event(event);
        x = libinput_event_touch_get_x_transformed(tch_event,
                my_ctxt.max_x - my_ctxt.min_x + 1) + my_ctxt.min_x;
        y = libinput_event_touch_get_y_transformed(tch_event,
                my_ctxt.max_y - my_ctxt.min_y + 1) + my_ctxt.min_y;

        retval = 0;
        // emulate the mouse left button down and mouse move events
        if (on_mouse_button_changed(BTN_LEFT, LIBINPUT_BUTTON_STATE_PRESSED) ||
                on_new_mouse_pos(x, y))
            retval |= IAL_EVENT_MOUSE;

        extra->event = IAL_EVENT_TOUCH_DOWN;
        extra->wparam = 0;
        extra->lparam = MAKELONG(my_ctxt.mouse_x, my_ctxt.mouse_y);
        retval |= IAL_EVENT_EXTRA;
        break;
    }

    case LIBINPUT_EVENT_TOUCH_UP: {
        //struct libinput_event_touch* tch_event;
        //tch_event = libinput_event_get_touch_event(event);

        retval = 0;
        // emulate the mouse left button up and mouse move events
        if (on_mouse_button_changed(BTN_LEFT, LIBINPUT_BUTTON_STATE_RELEASED))
            retval |= IAL_EVENT_MOUSE;

        extra->event = IAL_EVENT_TOUCH_UP;
        extra->wparam = 0;
        extra->lparam = 0;
        retval |= IAL_EVENT_EXTRA;
        break;
    }

    case LIBINPUT_EVENT_TOUCH_MOTION: {
        struct libinput_event_touch* tch_event;
        double x, y;

        tch_event = libinput_event_get_touch_event(event);
        x = libinput_event_touch_get_x_transformed(tch_event,
                my_ctxt.max_x - my_ctxt.min_x + 1) + my_ctxt.min_x;
        y = libinput_event_touch_get_y_transformed(tch_event,
                my_ctxt.max_y - my_ctxt.min_y + 1) + my_ctxt.min_y;

        retval = 0;
        // emulate the mouse move event
        if (on_new_mouse_pos(x, y))
            retval |= IAL_EVENT_MOUSE;

        extra->event = IAL_EVENT_TOUCH_MOTION;
        extra->wparam = 0;
        extra->lparam = MAKELONG(my_ctxt.mouse_x, my_ctxt.mouse_y);
        retval |= IAL_EVENT_EXTRA;
        break;
    }

    case LIBINPUT_EVENT_TOUCH_CANCEL: {
        retval = 0;
        // emulate the mouse left button up and mouse move events
        if (on_mouse_button_changed(BTN_LEFT, LIBINPUT_BUTTON_STATE_RELEASED))
            retval |= IAL_EVENT_MOUSE;

        extra->event = IAL_EVENT_TOUCH_CANCEL;
        extra->wparam = 0;
        extra->lparam = 0;
        retval |= IAL_EVENT_EXTRA;
        break;
    }

    case LIBINPUT_EVENT_TOUCH_FRAME: {
        struct libinput_event_touch* tch_event;

        tch_event = libinput_event_get_touch_event(event);

        extra->event = IAL_EVENT_TOUCH_FRAME;
        extra->wparam = libinput_event_touch_get_slot(tch_event);
        extra->lparam = libinput_event_touch_get_seat_slot(tch_event);
        retval = IAL_EVENT_EXTRA;
        break;
    }

    case LIBINPUT_EVENT_TABLET_TOOL_AXIS:
    case LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY:
    case LIBINPUT_EVENT_TABLET_TOOL_TIP: {
        struct libinput_event_tablet_tool* tool_event;
        int state = 0;
        double tmp, mouse_x, mouse_y;

        tool_event = libinput_event_get_tablet_tool_event(event);

        /* always get x,y for mouse */
        mouse_x = libinput_event_tablet_tool_get_x_transformed(tool_event,
            my_ctxt.max_x - my_ctxt.min_x + 1) + my_ctxt.min_x;
        mouse_y = libinput_event_tablet_tool_get_y_transformed(tool_event,
            my_ctxt.max_y - my_ctxt.min_y + 1) + my_ctxt.min_y;

        if (on_new_mouse_pos(mouse_x, mouse_y))
            retval = IAL_EVENT_MOUSE;

        if (type == LIBINPUT_EVENT_TABLET_TOOL_AXIS)
            extra->event = IAL_EVENT_TABLET_TOOL_AXIS;
        else if (type == LIBINPUT_EVENT_TABLET_TOOL_PROXIMITY) {
            extra->event = IAL_EVENT_TABLET_TOOL_PROXIMITY;
            switch (libinput_event_tablet_tool_get_proximity_state(tool_event)) {
            case LIBINPUT_TABLET_TOOL_PROXIMITY_STATE_OUT:
                state = TABLET_TOOL_PROXIMITY_STATE_OUT;
                break;
            case LIBINPUT_TABLET_TOOL_PROXIMITY_STATE_IN:
                state = TABLET_TOOL_PROXIMITY_STATE_IN;
                break;
            }
        }
        else {
            extra->event = IAL_EVENT_TABLET_TOOL_TIP;
            switch (libinput_event_tablet_tool_get_tip_state(tool_event)) {
            case LIBINPUT_TABLET_TOOL_TIP_UP:
                state = TABLET_TOOL_TIP_UP;
                // emulate the mouse left button up event
                if (on_mouse_button_changed(BTN_LEFT, LIBINPUT_BUTTON_STATE_RELEASED))
                    retval = IAL_EVENT_MOUSE;
                break;
            case LIBINPUT_TABLET_TOOL_TIP_DOWN:
                state = TABLET_TOOL_TIP_DOWN;
                // emulate the mouse left button down event
                if (on_mouse_button_changed(BTN_LEFT, LIBINPUT_BUTTON_STATE_RELEASED))
                    retval = IAL_EVENT_MOUSE;
                break;
            }
        }

        tool_event = libinput_event_get_tablet_tool_event(event);

        extra->event = IAL_EVENT_TABLET_TOOL_AXIS;
        if (libinput_event_tablet_tool_x_has_changed(tool_event)) {
            extra->params_mask |= TABLET_TOOL_CHANGED_X;
            extra->wparams[TABLET_TOOL_X] = MAKELONG(TABLET_TOOL_X, state);
            extra->lparams[TABLET_TOOL_X] = (int)(mouse_x * 10);
        }

        if (libinput_event_tablet_tool_y_has_changed(tool_event)) {
            extra->params_mask |= TABLET_TOOL_CHANGED_X;
            extra->wparams[TABLET_TOOL_Y] = MAKELONG(TABLET_TOOL_X, state);
            extra->lparams[TABLET_TOOL_Y] = (int)(mouse_y * 10);
        }

        if (libinput_event_tablet_tool_pressure_has_changed(tool_event)) {
            extra->params_mask |= TABLET_TOOL_CHANGED_PRESSURE;
            tmp = libinput_event_tablet_tool_get_pressure(tool_event);
            extra->wparams[TABLET_TOOL_PRESSURE] = MAKELONG(TABLET_TOOL_PRESSURE, state);
            extra->lparams[TABLET_TOOL_PRESSURE] = (int)(tmp * 1000);
        }

        if (libinput_event_tablet_tool_distance_has_changed(tool_event)) {
            extra->params_mask |= TABLET_TOOL_CHANGED_DISTANCE;
            tmp = libinput_event_tablet_tool_get_distance(tool_event);
            extra->wparams[TABLET_TOOL_DISTANCE] = MAKELONG(TABLET_TOOL_DISTANCE, state);
            extra->lparams[TABLET_TOOL_DISTANCE] = (int)(tmp * 1000);
        }

        if (libinput_event_tablet_tool_tilt_x_has_changed(tool_event)) {
            extra->params_mask |= TABLET_TOOL_CHANGED_TILT_X;
            tmp = libinput_event_tablet_tool_get_tilt_x(tool_event);
            extra->wparams[TABLET_TOOL_TILT_X] = MAKELONG(TABLET_TOOL_TILT_X, state);
            extra->lparams[TABLET_TOOL_TILT_X] = (int)(tmp * 50);
        }

        if (libinput_event_tablet_tool_tilt_y_has_changed(tool_event)) {
            extra->params_mask |= TABLET_TOOL_CHANGED_TILT_Y;
            tmp = libinput_event_tablet_tool_get_tilt_y(tool_event);
            extra->wparams[TABLET_TOOL_TILT_Y] = MAKELONG(TABLET_TOOL_TILT_Y, state);
            extra->lparams[TABLET_TOOL_TILT_Y] = (int)(tmp * 50);
        }

        if (libinput_event_tablet_tool_rotation_has_changed(tool_event)) {
            extra->params_mask |= TABLET_TOOL_CHANGED_ROTATION;
            tmp = libinput_event_tablet_tool_get_rotation(tool_event);
            extra->wparams[TABLET_TOOL_ROTATION] = MAKELONG(TABLET_TOOL_ROTATION, state);
            extra->lparams[TABLET_TOOL_ROTATION] = (int)(tmp * 50);
        }

        if (libinput_event_tablet_tool_slider_has_changed(tool_event)) {
            extra->params_mask |= TABLET_TOOL_CHANGED_SLIDER;
            tmp = libinput_event_tablet_tool_get_slider_position(tool_event);
            extra->wparams[TABLET_TOOL_SLIDER] = MAKELONG(TABLET_TOOL_SLIDER, state);
            extra->lparams[TABLET_TOOL_SLIDER] = (int)(tmp * 1000);
        }

#if 0
        if (libinput_event_tablet_tool_size_major_has_changed(tool_event)) {
            extra->params_mask |= TABLET_TOOL_CHANGED_SIZE_MAJOR;
            tmp = libinput_event_tablet_tool_get_size_major(tool_event);
            extra->wparams[TABLET_TOOL_SIZE_MAJOR] = MAKELONG(TABLET_TOOL_SIZE_MAJOR, state);
            extra->lparams[TABLET_TOOL_SIZE_MAJOR] = (int)(tmp * 100);
        }

        if (libinput_event_tablet_tool_size_minor_has_changed(tool_event)) {
            extra->params_mask |= TABLET_TOOL_CHANGED_SIZE_MINOR;
            tmp = libinput_event_tablet_tool_get_size_minor(tool_event);
            extra->wparams[TABLET_TOOL_SIZE_MINOR] = MAKELONG(TABLET_TOOL_SIZE_MINOR, state);
            extra->lparams[TABLET_TOOL_SIZE_MINOR] = (int)(tmp * 100);
        }
#endif

        if (libinput_event_tablet_tool_wheel_has_changed(tool_event)) {
            extra->params_mask |= TABLET_TOOL_CHANGED_WHEEL;
            tmp = libinput_event_tablet_tool_get_wheel_delta(tool_event);
            extra->wparams[TABLET_TOOL_WHEEL] = MAKELONG(TABLET_TOOL_WHEEL, state);
            extra->lparams[TABLET_TOOL_WHEEL] = (int)(tmp * 100);
        }

        if (extra->params_mask) {
            if (retval > 0)
                retval |= IAL_EVENT_EXTRA;
            else
                retval = IAL_EVENT_EXTRA;
        }
        break;
    }

    case LIBINPUT_EVENT_TABLET_TOOL_BUTTON: {
        struct libinput_event_tablet_tool* tool_event;
        uint32_t button_code;
        int button_state = TABLET_BUTTON_STATE_INVALID;

        tool_event = libinput_event_get_tablet_tool_event(event);

        button_code = libinput_event_tablet_tool_get_button(tool_event);
        switch (libinput_event_tablet_tool_get_button_state(tool_event)) {
        case LIBINPUT_BUTTON_STATE_RELEASED:
            button_state = TABLET_BUTTON_STATE_RELEASED;
            break;
        case LIBINPUT_BUTTON_STATE_PRESSED:
            button_state = TABLET_BUTTON_STATE_PRESSED;
            break;
        }

        extra->event = IAL_EVENT_TABLET_TOOL_BUTTON;
        extra->wparam = button_code;
        extra->lparam = button_state;
        retval = IAL_EVENT_EXTRA;
        break;
    }

    case LIBINPUT_EVENT_TABLET_PAD_BUTTON: {
        struct libinput_event_tablet_pad* pad_event;
        uint32_t button_number;
        int button_state = TABLET_BUTTON_STATE_INVALID;

        pad_event = libinput_event_get_tablet_pad_event(event);

        button_number = libinput_event_tablet_pad_get_button_number(pad_event);
        switch (libinput_event_tablet_pad_get_button_state(pad_event)) {
        case LIBINPUT_BUTTON_STATE_RELEASED:
            button_state = TABLET_BUTTON_STATE_RELEASED;
            break;
        case LIBINPUT_BUTTON_STATE_PRESSED:
            button_state = TABLET_BUTTON_STATE_PRESSED;
            break;
        }

        extra->event = IAL_EVENT_TABLET_PAD_BUTTON;
        extra->wparam = libinput_event_tablet_pad_get_mode(pad_event);
        extra->lparam = MAKELONG(button_number, button_state);
        retval = IAL_EVENT_EXTRA;
        break;
    }

    case LIBINPUT_EVENT_TABLET_PAD_RING: {
        struct libinput_event_tablet_pad* pad_event;
        unsigned int mode;
        int source = TABLET_PAD_RING_SOURCE_UNKNOWN;
        uint32_t number;
        double tmp;
        int position;

        pad_event = libinput_event_get_tablet_pad_event(event);

        mode = libinput_event_tablet_pad_get_mode(pad_event);
        switch (libinput_event_tablet_pad_get_ring_source(pad_event)) {
        case LIBINPUT_TABLET_PAD_RING_SOURCE_UNKNOWN:
            source = TABLET_PAD_RING_SOURCE_UNKNOWN;
            break;
        case LIBINPUT_TABLET_PAD_RING_SOURCE_FINGER:
            source = TABLET_PAD_RING_SOURCE_FINGER;
            break;
        }

        number = libinput_event_tablet_pad_get_ring_number(pad_event);
        tmp = libinput_event_tablet_pad_get_ring_position(pad_event);
        position = (int)(tmp * 50);

        extra->event = IAL_EVENT_TABLET_PAD_RING;
        extra->wparam = MAKELONG(mode, source);
        extra->lparam = MAKELONG(position, number);
        retval = IAL_EVENT_EXTRA;
        break;
    }

    case LIBINPUT_EVENT_TABLET_PAD_STRIP: {
        struct libinput_event_tablet_pad* pad_event;
        unsigned int mode;
        int source = TABLET_PAD_STRIP_SOURCE_UNKNOWN;
        uint32_t number;
        double tmp;
        int position;

        pad_event = libinput_event_get_tablet_pad_event(event);

        mode = libinput_event_tablet_pad_get_mode(pad_event);
        switch (libinput_event_tablet_pad_get_strip_source(pad_event)) {
        case LIBINPUT_TABLET_PAD_STRIP_SOURCE_UNKNOWN:
            source = TABLET_PAD_STRIP_SOURCE_UNKNOWN;
            break;
        case LIBINPUT_TABLET_PAD_STRIP_SOURCE_FINGER:
            source = TABLET_PAD_STRIP_SOURCE_FINGER;
            break;
        }

        number = libinput_event_tablet_pad_get_strip_number(pad_event);
        tmp = libinput_event_tablet_pad_get_strip_position(pad_event);
        position = (int)(tmp * 100);

        extra->event = IAL_EVENT_TABLET_PAD_RING;
        extra->wparam = MAKELONG(mode, source);
        extra->lparam = MAKELONG(position, number);
        retval = IAL_EVENT_EXTRA;
        break;
    }

    case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN: {
        struct libinput_event_gesture* gst_event;
        int nr_figs;

        gst_event = libinput_event_get_gesture_event(event);
        nr_figs = libinput_event_gesture_get_finger_count(gst_event);

        extra->event = IAL_EVENT_GESTURE_SWIPE_BEGIN;
        extra->wparam = nr_figs;
        extra->lparam = 0;
        retval = IAL_EVENT_EXTRA;
        break;
    }

    case LIBINPUT_EVENT_GESTURE_SWIPE_UPDATE: {
        struct libinput_event_gesture* gst_event;
        int nr_figs;
        double dx, dy;
        int my_dx, my_dy;

        gst_event = libinput_event_get_gesture_event(event);

        nr_figs = libinput_event_gesture_get_finger_count(gst_event);
        dx = libinput_event_gesture_get_dx(gst_event);
        dy = libinput_event_gesture_get_dy(gst_event);
        my_dx = (int)(dx + 0.5);
        my_dy = (int)(dy + 0.5);

        extra->event = IAL_EVENT_GESTURE_SWIPE_UPDATE;
        extra->wparam = nr_figs;
        extra->lparam = MAKELONG(my_dx, my_dy);
        retval = IAL_EVENT_EXTRA;
        break;
    }

    case LIBINPUT_EVENT_GESTURE_SWIPE_END: {
        struct libinput_event_gesture* gst_event;
        int nr_figs;
        int is_cancelled;

        gst_event = libinput_event_get_gesture_event(event);

        nr_figs = libinput_event_gesture_get_finger_count(gst_event);
        is_cancelled = libinput_event_gesture_get_cancelled(gst_event);

        extra->event = IAL_EVENT_GESTURE_SWIPE_END;
        extra->wparam = nr_figs;
        extra->lparam = is_cancelled;
        retval = IAL_EVENT_EXTRA;
        break;
    }

    case LIBINPUT_EVENT_GESTURE_PINCH_BEGIN: {
        struct libinput_event_gesture* gst_event;
        int nr_figs;
        double tmp;
        unsigned int scale;

        gst_event = libinput_event_get_gesture_event(event);

        nr_figs = libinput_event_gesture_get_finger_count(gst_event);
        tmp = libinput_event_gesture_get_scale(gst_event);
        if (tmp < 0) tmp = 0;
        scale = (unsigned int)(tmp * 100);

        extra->event = IAL_EVENT_GESTURE_PINCH_BEGIN;
        extra->wparam = nr_figs;
        extra->lparam = scale;
        retval = IAL_EVENT_EXTRA;
        break;
    }

    case LIBINPUT_EVENT_GESTURE_PINCH_UPDATE: {
        struct libinput_event_gesture* gst_event;
        double tmp;
        //int nr_figs;
        //nr_figs = libinput_event_gesture_get_finger_count(gst_event);
        unsigned int scale;
        int dx, dy, da;

        gst_event = libinput_event_get_gesture_event(event);

        tmp = libinput_event_gesture_get_scale(gst_event);
        if (tmp < 0) tmp = 0;
        scale = (unsigned int)(tmp * 100);

        tmp = libinput_event_gesture_get_angle_delta(gst_event);
        da = (int)(tmp * 50);

        tmp = libinput_event_gesture_get_dx(gst_event);
        dx = (int)(tmp + 0.5);

        tmp = libinput_event_gesture_get_dy(gst_event);
        dy = (int)(tmp + 0.5);

        extra->event = IAL_EVENT_GESTURE_PINCH_UPDATE;
        extra->wparam = MAKELONG(scale, da);
        extra->lparam = MAKELONG(dx, dy);
        retval = IAL_EVENT_EXTRA;
        break;
    }

    case LIBINPUT_EVENT_GESTURE_PINCH_END: {
        struct libinput_event_gesture* gst_event;
        int nr_figs;
        double tmp;
        unsigned int scale;
        int is_cancelled;

        gst_event = libinput_event_get_gesture_event(event);

        nr_figs = libinput_event_gesture_get_finger_count(gst_event);
        tmp = libinput_event_gesture_get_scale(gst_event);
        if (tmp < 0) tmp = 0;
        scale = (unsigned int)(tmp * 100);
        is_cancelled = libinput_event_gesture_get_cancelled(gst_event);

        extra->event = IAL_EVENT_GESTURE_PINCH_END;
        extra->wparam = MAKELONG(nr_figs, is_cancelled);
        extra->lparam = scale;
        retval = IAL_EVENT_EXTRA;
        break;
    }

    case LIBINPUT_EVENT_SWITCH_TOGGLE: {
        struct libinput_event_switch* sch_event;
        int my_switch = SWITCH_INVALID;
        int my_state = SWITCH_STATE_INVALID;

        sch_event = libinput_event_get_switch_event(event);
        switch (libinput_event_switch_get_switch(sch_event)) {
        case LIBINPUT_SWITCH_LID:
            my_switch = SWITCH_LID;
            break;
        case LIBINPUT_SWITCH_TABLET_MODE:
            my_switch = SWITCH_TABLET_MODE;
            break;
        }

        if (my_switch) {
            switch (libinput_event_switch_get_switch_state(sch_event)) {
            case LIBINPUT_SWITCH_STATE_OFF:
                my_state = SWITCH_STATE_OFF;
                break;
            case LIBINPUT_SWITCH_STATE_ON:
                my_state = SWITCH_STATE_ON;
                break;
            }

            if (my_state) {
                extra->event = IAL_EVENT_SWITCH_TOGGLE;
                extra->wparam = my_switch;
                extra->lparam = my_state;
                retval = IAL_EVENT_EXTRA;
            }
        }
        break;
    }

#if 0
    default:
        _DBG_PRINTF("IAL>LIBINPUT: got a UNKNOWN event type: %d\n", type);
        break;
#endif
    }

    libinput_event_destroy(event);

    return retval;
}

static int open_restricted(const char *path, int flags, void *data)
{
    int fd;

    fd = open(path, flags);

#ifdef _DEBUG
    if (fd < 0) {
        _DBG_PRINTF("IAL>LIBINPUT: failed to open event device %s: %m\n",
            path);
    }
#endif

    return fd < 0 ? -errno : fd;
}

static void close_restricted(int fd, void *data)
{
    close(fd);
}

static const struct libinput_interface my_interface = {
    .open_restricted = open_restricted,
    .close_restricted = close_restricted,
};

static void update_default_device(void)
{
    struct libinput_event *event;
    int default_seat_found = 0;

    libinput_dispatch(my_ctxt.li);
    while ((event = libinput_get_event(my_ctxt.li))) {
        enum libinput_event_type type;
        struct libinput_device *device;
        struct libinput_seat *seat;
        const char *seat_name;

        type = libinput_event_get_type(event);
        _DBG_PRINTF("IAL>LIBINPUT: got a new event: %d\n", type);

        if (type != LIBINPUT_EVENT_DEVICE_ADDED) {
            libinput_event_destroy(event);
            continue;
        }

        device = libinput_event_get_device(event);
        seat = libinput_device_get_seat(device);
        seat_name = libinput_seat_get_logical_name(seat);

        _DBG_PRINTF("IAL>LIBINPUT: a new event device: %s\n", seat_name);

        if (!default_seat_found) {
            default_seat_found = (strcmp(seat_name, "default") == 0) ? 1 : 0;
            my_ctxt.def_dev = device;
        }

        libinput_event_destroy(event);
    }

    if (!default_seat_found) {
        _WRN_PRINTF("no default seat found for seat_id: %s\n", my_ctxt.seat_id);
    }

    my_ctxt.li_fd = libinput_get_fd(my_ctxt.li);
    _DBG_PRINTF("IAL>LIBINPUT: initialized successfully: %d\n",
            my_ctxt.li_fd);
}

static void input_suspend(void)
{
    if (!my_ctxt.suspended) {
        libinput_suspend(my_ctxt.li);
        my_ctxt.def_dev = NULL;
        my_ctxt.suspended = 1;
    }
}

static int input_resume(void)
{
    if (my_ctxt.suspended) {
        if (libinput_resume(my_ctxt.li) == 0) {
            update_default_device();
            my_ctxt.suspended = 0;
            return 0;
        }
    }

    return -1;
}

/*
**

NOTE: the led masks defined by libinput is not same as MiniGUI.

libinput:

    enum libinput_led {
        LIBINPUT_LED_NUM_LOCK = (1 << 0),
        LIBINPUT_LED_CAPS_LOCK = (1 << 1),
        LIBINPUT_LED_SCROLL_LOCK = (1 << 2)
    };

MiniGUI:

    IAL_SetLeds (slock | (numlock << 1) | (capslock << 2));

**
*/
static void set_leds (unsigned int leds)
{
    if (my_ctxt.def_dev) {
        enum libinput_led li_leds = 0;

        if (leds & (1 << 2))
            li_leds |= LIBINPUT_LED_CAPS_LOCK;
        if (leds & (1 << 1))
            li_leds |= LIBINPUT_LED_NUM_LOCK;
        if (leds & (1 << 0))
            li_leds |= LIBINPUT_LED_SCROLL_LOCK;

        libinput_device_led_update(my_ctxt.def_dev, li_leds);
    }
}

BOOL ial_InitLibInput (INPUT* input, const char* mdev, const char* mtype)
{
    if (__mg_linux_tty_enable_vt_switch() == 0) {
        _DBG_PRINTF("IAL>LIBINPUT: vt switch enabled\n");
        my_ctxt.vt_switch = 1;
    }

    my_ctxt.udev = udev_new();
    if (my_ctxt.udev == NULL)
        goto error;

    my_ctxt.li = libinput_udev_create_context(&my_interface,
            &my_ctxt, my_ctxt.udev);
    if (my_ctxt.li == NULL)
        goto error;

    if (GetMgEtcValue ("libinput", "seat",
            my_ctxt.seat_id, LEN_SEAT_ID) < 0) {
        strcpy(my_ctxt.seat_id, "seat0");
        _WRN_PRINTF("No libinput.seat defined, use the default 'seat0'\n");
    }

    if (libinput_udev_assign_seat(my_ctxt.li, my_ctxt.seat_id))
        goto error;

    update_default_device();

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = mouse_setxy;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = mouse_setrange;
    input->suspend_mouse= input_suspend;
    input->resume_mouse = input_resume;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->suspend_keyboard = input_suspend;
    input->resume_keyboard = input_resume;
    input->set_leds = set_leds;

    input->wait_event_ex = wait_event_ex;
    return TRUE;

error:
    _ERR_PRINTF("IAL>LIBINPUT: failed to initialize (udev: %p; li: %p)\n",
            my_ctxt.udev, my_ctxt.li);

    if (my_ctxt.li)
        libinput_unref(my_ctxt.li);
    if (my_ctxt.udev)
        udev_unref(my_ctxt.udev);

    my_ctxt.li = NULL;
    my_ctxt.udev = NULL;
    return FALSE;
}

void TermLibInput (void)
{
    if (my_ctxt.li)
        libinput_unref(my_ctxt.li);
    if (my_ctxt.udev)
        udev_unref(my_ctxt.udev);

    my_ctxt.li = NULL;
    my_ctxt.udev = NULL;

    __mg_linux_tty_disable_vt_switch();
}

#endif /* _MGIAL_LIBINPUT */

