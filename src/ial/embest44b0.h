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
#ifndef GUI_IAL_HH44B0_H
#define GUI_IAL_HH44B0_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitHH44b0Input (INPUT* input, const char* mdev, const char* mtype);
void    TermHH44b0Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */


#define ZLG_SLAVE_ADDR	(0x70 >> 1)


#define REG_Sys		0x00
#define REG_Key		0x01
#define REG_Cnt		0x02
#define REG_Func	0x03
#define REG_Cmd0	0x07
#define REG_Cmd1	0x08
#define REG_Flas	0x0C
#define REG_Num		0x0D
#define REG_Dis0	0x10
#define REG_Dis1	0x11
#define REG_Dis2	0x12
#define REG_Dis3	0x13
#define REG_Dis4	0x14
#define REG_Dis5	0x15
#define REG_Dis6	0x16
#define REG_Dis7	0x17

#define SEG_A		(1<<7)
#define SEG_B		(1<<6)
#define SEG_C		(1<<5)
#define SEG_D		(1<<4)
#define SEG_E		(1<<3)
#define SEG_F		(1<<2)
#define SEG_G		(1<<1)
#define SEG_DIP		(1<<0)

#define KEY_RELEASED    0x80 

#define KEYPAD_UP       0x12
#define KEYPAD_DOWN     0x1a
#define KEYPAD_LEFT     0x19
#define KEYPAD_RIGHT    0x1b
#define KEYPAD_ENTER    0x1c
#define KEYPAD_ESCAPE   0x11

#define KEYPAD_F1        0x1
#define KEYPAD_F2        0x2
#define KEYPAD_F3        0x3
#define KEYPAD_F4        0x4
#define KEYPAD_F5        0x5
#define KEYPAD_F6        0x9
#define KEYPAD_F7        0xa
#define KEYPAD_F8        0xb
#define KEYPAD_F9        0xc
#define KEYPAD_F10       0xd

#define MAX_KEYPAD_CODE     0x21

#endif  /* GUI_IAL_HH44B0_H */
