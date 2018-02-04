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
** em8620.h:. the head file for IAL Engine of EM8520L
**
** Created by Zhang Xinhua, 2005/10/xx
*/

#ifndef GUI_IAL_EM8620_H
    #define GUI_IAL_EM8620_H

#define IR_IOCTL 0x00450000
#define IR_IOCTL_READ_KEY		(IR_IOCTL + 0x00)
#define IR_IOCTL_FLUSH_KEY		(IR_IOCTL + 0x01)

#define NEC_REPEAT_CODE			(0xffffffff)

#define MOUSE_MOVE_PIXEL    5
#define MOUSE_MOVE_PIXEL_F  15

#define MOUSE_MAX_X         639
#define MOUSE_MAX_Y         464

#define RM_ENTER            0
#define RM_HW_ON_OFF        1
#define RM_HW_PROGRAM       2
#define RM_HW_UP            3
#define RM_HW_DOWN          4
#define RM_HW_LEFT          5
#define RM_HW_RIGHT         6
#define RM_HW_SELECT        7
#define RM_HW_MENU          8
#define RM_HW_TITLE         9
#define RM_HW_OSD           10
#define RM_HW_LANGUAGE      11
#define RM_HW_ANGLE         12
#define RM_HW_SUB_TITLE     13
#define RM_HW_PAUSE         14
#define RM_HW_STOP          15
#define RM_HW_PAUSE_PLAY    16
#define RM_HW_EJECT         17
#define RM_HW_FAST_REWIND   18
#define RM_HW_FAST_FORWARD  19
#define RM_HW_SLOW_REVERSE  20
#define RM_HW_SLOW_FORWARD  21
#define RM_HW_REPEAT        22
#define RM_HW_AB_REPEAT     23
#define RM_HW_PREV_TRACK    24
#define RM_HW_NEXT_TRACK    25
#define RM_HW_VOL_PLUS      26
#define RM_HW_VOL_MINUS     27
#define RM_HW_MUTE          28
#define RM_HW_KEY_1         29
#define RM_HW_KEY_2         30
#define RM_HW_KEY_3         31
#define RM_HW_KEY_4         32
#define RM_HW_KEY_5         33
#define RM_HW_KEY_6         34
#define RM_HW_KEY_7         35
#define RM_HW_KEY_8         36
#define RM_HW_KEY_9         37
#define RM_HW_KEY_0         38
#define RM_HW_ENT           39
#define RM_HW_SETUP         40
#define RM_HW_CLEAR         41
#define RM_HW_TVMODE        42
#define RM_HW_PBC           43
#define RM_HW_RETURN        44
#define RM_HW_SHUFFLE       45
#define RM_HW_SEARCH        46
#define RM_HW_ZOOM          47

#define RM_HW_TIMEOUT       NR_KEYS

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitEm8620Input(INPUT * input, const char * mdev, const char * mtype);
void    TermEm8620Input(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_EM8620_H */


