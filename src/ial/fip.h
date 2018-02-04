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
** fip.h:. the head file for IAL Engine of EM85xx Front Panel and Remote Controller
**
** Created by Wei Yongming, 2003/12/28
*/

#ifndef GUI_IAL_FIP_H
    #define GUI_IAL_FIP_H

#define MOUSE_MOVE_PIXEL    5
#define MOUSE_MAX_X         639
#define MOUSE_MAX_Y         464

#define PLAYER_NEXTBASE 1

#ifdef PLAYER_NEXTBASE
    #define KHT_IR
#elif defined PLAYER_V
    #define V_IR
#elif defined PLAYER_MOKOH
    #define MOKOH_IR
#elif defined PLAYER_KISS
    #error WE DO NOT HAVE KISS IR DEFINITION...
#elif defined PLAYER_PLAYMATE
    #define PLAYMATE_IR
#elif defined PLAYER_MOMITSU
    #define JX2002_IR
#elif
    //custom remote control...
#endif

/* FIP ioctls */
#define FIP_IOCTL               0x00450000
#define FIP_IOCTL_FLUSH_FIFO    (FIP_IOCTL+0x00)
#define FIP_IOCTL_DIMMER        (FIP_IOCTL+0x01)
#define FIP_IOCTL_DISPLAY       (FIP_IOCTL+0x02)
#define FIP_IOCTL_READ_KEYS     (FIP_IOCTL+0x03)
#define FIP_IOCTL_LED           (FIP_IOCTL+0x04)

/* command pseudo scancodes (e.g. for command from fileplayer) */
#define CMD_EOF_SCANCODE        0x7f000001

/* FIP ir scancode for PLAYER_NEXTBASE */

/* 
 * INVALID_SCANCODE should be defined for any remote control
 * this means a value that is not valid for any ir OR vfd code
 */

#if defined(NORCENT_IR)
    /* Norcent remote control */
    #define ZERO_SCANCODE        0x00ff00ff
    #define ENTER_SCANCODE        0x00ff02fd
    #define ZOOM_SCANCODE        0x00ff08f7
    #define CANCEL_SCANCODE        0x00ff10ef
    #define OSD_SCANCODE        0x00ff18e7
    #define STOP_SCANCODE        0x00ff20df
    #define ANGLE_SCANCODE        0x00ff28d7
    #define RETURN_SCANCODE        0x00ff30cf
    #define MUTE_SCANCODE        0x00ff38c7
    #define PREV_SCANCODE        0x00ff40bf
    #define FOUR_SCANCODE        0x00ff42bd
    #define    POWER_SCANCODE        0x00ff48b7
    #define NEXT_SCANCODE        0x00ff50af
    #define REPEAT_SCANCODE        0x00ff52ad
    #define SLOW_SCANCODE        0x00ff58a7
    #define PBC_SCANCODE        0x00ff609f
    #define SIX_SCANCODE        0x00ff629d
    #define OPENCLOSE_SCANCODE    0x00ff6897
    #define FWD_SCANCODE        0x00ff708f
    #define FIVE_SCANCODE        0x00ff728d
    #define TITLE_SCANCODE        0x00ff7887
    #define UP_SCANCODE            0x00ff807f
    #define ONE_SCANCODE        0x00ff827d
    #define RIGHT_SCANCODE        0x00ff8877
    #define DOWN_SCANCODE        0x00ff906f
    #define PROGRAM_SCANCODE    0x00ff926d
    #define LEFT_SCANCODE        0x00ff9867
    #define THREE_SCANCODE        0x00ffa25d
    #define SEARCH_SCANCODE        0x00ffa857
    #define VOLUP_SCANCODE        0x00ffb04f
    #define TWO_SCANCODE        0x00ffb24d
    #define SUBTITLE_SCANCODE    0x00ffb847
    #define AUDIO_SCANCODE        0x00ffc03f
    #define SEVEN_SCANCODE        0x00ffc23d
    #define VOLDOWN_SCANCODE    0x00ffc837
    #define SETUP_SCANCODE        0x00ffd02f
    #define ABREPEAT_SCANCODE    0x00ffd22d
    #define THREED_SCANCODE        0x00ffd827
    #define PAUSESTEP_SCANCODE    0x00ffe01f
    #define NINE_SCANCODE        0x00ffe21d
    #define PLAY_SCANCODE        0x00ffe817
    #define REV_SCANCODE        0x00fff00f
    #define EIGHT_SCANCODE        0x00fff20d
    #define MENU_SCANCODE        0x00fff807
    #define INVALID_SCANCODE    0x12345678
#elif defined KHT_IR
    #define INVALID_SCANCODE    0x12345678
    #define PAGEUP_SCANCODE     0x807f00ff
    #define DOWN_SCANCODE       0x807f02fd
    #define ZERO_SCANCODE       0x807f10ef
    #define THREED_SCANCODE     0x807f12ed
    #define THREE_SCANCODE      0x807f12ed
    #define SEVEN_SCANCODE      0x807f20df
    #define TITLE_SCANCODE      0x807f28d7
    #define TWO_SCANCODE        0x807f2ad5
    #define ONE_SCANCODE        0x807f30cf
    #define LEFT_SCANCODE       0x807f38c7
    #define FIVE_SCANCODE       0x807f40bf
    #define UP_SCANCODE         0x807f4ab5
    #define MUTE_SCANCODE       0x807f58a7
    #define NINE_SCANCODE       0x807f609f
    #define PAUSESTEP_SCANCODE  0x807f629d
    #define PAGEDOWN_SCANCODE   0x807f6897
    #define FOUR_SCANCODE       0x807f807f
    #define TAB_SCANCODE        0x807f8a75
    #define BACKSPACE_SCANCODE  0x807f906f
    #define STOP_SCANCODE       0x807f9867
    #define EIGHT_SCANCODE      0x807fa05f
    #define PLAY_SCANCODE       0x807fb04f
    #define ENTER_SCANCODE      0x807fb847
    #define SIX_SCANCODE        0x807fc03f
    #define RIGHT_SCANCODE      0x807fca35
    #define POWER_SCANCODE      0x807fd02f
    #define ZOOM_SCANCODE       0x807fd22d
    #define FWD_SCANCODE        0x807fda25
    #define CAPSNUM_SCANCODE    0x807fe01f
    #define NEXT_SCANCODE       0x807fea15
    #define PREV_SCANCODE       0x807ff20d
#elif
    #error NO RMOTE CONTROL DEFINIED!!!
#endif

#define SC_PANEL_RELEASE        0x00000000
#define SC_PANEL_REV            0x00200000
#define SC_PANEL_FWD            0x00800000
#define SC_PANEL_PREV           0x10000010
#define SC_PANEL_NEXT           0x01000001
#define SC_PANEL_EJECT          0x00100000
#define SC_PANEL_PLAY           0x00010000
#define SC_PANEL_STOP           0x00000100

#undef SC_PANEL_PAUSE
#undef SC_PANEL_NAV_UP
#undef SC_PANEL_NAV_RIGHT
#undef SC_PANEL_NAV_DOWN
#undef SC_PANEL_NAV_LEFT
#undef SC_PANEL_POWER

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitFIPInput (INPUT* input, const char* mdev, const char* mtype);
void    TermFIPInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_FIP_H */


