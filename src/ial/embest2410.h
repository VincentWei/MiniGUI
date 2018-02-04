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
** embest2410.h:. the head file of Low Level Input Engine for EMBEST ARM2410 
**
** Created by Cheng Jiangang
*/

#ifndef GUI_IAL_EMBEST2410_H
    #define GUI_IAL_EMBEST2410_H

#define KEY_RELEASED    0x80 

#define KEYPAD_0        0x0B
#define KEYPAD_1        0x02
#define KEYPAD_2        0x0A
#define KEYPAD_3        0x12
#define KEYPAD_4        0x01
#define KEYPAD_5        0x09
#define KEYPAD_6        0x11
#define KEYPAD_7        0x00
#define KEYPAD_8        0x08
#define KEYPAD_9        0x10
#define KEYPAD_PERIOD      0x19
#define KEYPAD_DIVIDE      0x0C
#define KEYPAD_MULTIPLY    0x14
#define KEYPAD_MINUS       0x15
#define KEYPAD_PLUS        0x18     
#define KEYPAD_ENTER       0x1A
#define KEYPAD_NUMLOCK     0x04

#define MAX_KEYPAD_CODE     0x1C

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitEMBEST2410Input (INPUT* input, const char* mdev, const char* mtype);
void    TermEMBEST2410Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* GUI_IAL_EMBEST2410_H */
