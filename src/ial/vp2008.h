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
** vp2008.h:. the head file of VP2008 IAL Engine.
**
** Created by Wei YongMing, 2000/09/13
*/

#ifndef GUI_IAL_VP2008_H
    #define GUI_IAL_VP2008_H

#define MY_NR_KEYS						   134

#define MY_SCANCODE_NOTEXIST			   0xffff

#define MY_SCANCODE_ESCAPE				   1

#define MY_SCANCODE_1                      2
#define MY_SCANCODE_2                      3
#define MY_SCANCODE_3                      4
#define MY_SCANCODE_4                      5
#define MY_SCANCODE_5                      6
#define MY_SCANCODE_6                      7
#define MY_SCANCODE_7                      8
#define MY_SCANCODE_8                      9
#define MY_SCANCODE_9                      10
#define MY_SCANCODE_0                      11

#define MY_SCANCODE_MINUS                  12
#define MY_SCANCODE_EQUAL                  13

#define MY_SCANCODE_BACKSPACE              14
#define MY_SCANCODE_TAB                    15

#define MY_SCANCODE_Q                      16
#define MY_SCANCODE_W                      17
#define MY_SCANCODE_E                      18
#define MY_SCANCODE_R                      19
#define MY_SCANCODE_T                      20
#define MY_SCANCODE_Y                      21
#define MY_SCANCODE_U                      22
#define MY_SCANCODE_I                      23
#define MY_SCANCODE_O                      24
#define MY_SCANCODE_P                      25
#define MY_SCANCODE_BRACKET_LEFT           26
#define MY_SCANCODE_BRACKET_RIGHT          27

#define MY_SCANCODE_ENTER                  28

#define MY_SCANCODE_LEFTCONTROL            29

#define MY_SCANCODE_A                      30
#define MY_SCANCODE_S                      31
#define MY_SCANCODE_D                      32
#define MY_SCANCODE_F                      33
#define MY_SCANCODE_G                      34
#define MY_SCANCODE_H                      35
#define MY_SCANCODE_J                      36
#define MY_SCANCODE_K                      37
#define MY_SCANCODE_L                      38
#define MY_SCANCODE_SEMICOLON              39
#define MY_SCANCODE_APOSTROPHE             40
#define MY_SCANCODE_GRAVE                  41

#define MY_SCANCODE_LEFTSHIFT              42
#define MY_SCANCODE_BACKSLASH              43

#define MY_SCANCODE_Z                      44
#define MY_SCANCODE_X                      45
#define MY_SCANCODE_C                      46
#define MY_SCANCODE_V                      47
#define MY_SCANCODE_B                      48
#define MY_SCANCODE_N                      49
#define MY_SCANCODE_M                      50
#define MY_SCANCODE_COMMA                  51
#define MY_SCANCODE_PERIOD                 52
#define MY_SCANCODE_SLASH                  53

#define MY_SCANCODE_RIGHTSHIFT             54
#define MY_SCANCODE_KEYPADMULTIPLY         55

#define MY_SCANCODE_LEFTALT                56
#define MY_SCANCODE_SPACE                  57
#define MY_SCANCODE_CAPSLOCK               58

#define MY_SCANCODE_F1                     59
#define MY_SCANCODE_F2                     60
#define MY_SCANCODE_F3                     61
#define MY_SCANCODE_F4                     62
#define MY_SCANCODE_F5                     63
#define MY_SCANCODE_F6                     64
#define MY_SCANCODE_F7                     65
#define MY_SCANCODE_F8                     66
#define MY_SCANCODE_F9                     67
#define MY_SCANCODE_F10                    68

#define MY_SCANCODE_NUMLOCK                69
#define MY_SCANCODE_SCROLLLOCK             70

#define MY_SCANCODE_KEYPAD7                71
#define MY_SCANCODE_CURSORUPLEFT           71
#define MY_SCANCODE_KEYPAD8                72
#define MY_SCANCODE_CURSORUP               72
#define MY_SCANCODE_KEYPAD9                73
#define MY_SCANCODE_CURSORUPRIGHT          73
#define MY_SCANCODE_KEYPADMINUS            74
#define MY_SCANCODE_KEYPAD4                75
#define MY_SCANCODE_CURSORLEFT             75
#define MY_SCANCODE_KEYPAD5                76
#define MY_SCANCODE_KEYPAD6                77
#define MY_SCANCODE_CURSORRIGHT            77
#define MY_SCANCODE_KEYPADPLUS             78
#define MY_SCANCODE_KEYPAD1                79
#define MY_SCANCODE_CURSORDOWNLEFT         79
#define MY_SCANCODE_KEYPAD2                80
#define MY_SCANCODE_CURSORDOWN             80
#define MY_SCANCODE_KEYPAD3                81
#define MY_SCANCODE_CURSORDOWNRIGHT        81
#define MY_SCANCODE_KEYPAD0                82
#define MY_SCANCODE_KEYPADPERIOD           83

#define MY_SCANCODE_LESS                   86

#define MY_SCANCODE_F11                    87
#define MY_SCANCODE_F12                    88

#define MY_SCANCODE_KEYPADENTER            96
#define MY_SCANCODE_RIGHTCONTROL           97
#define MY_SCANCODE_CONTROL                97
#define MY_SCANCODE_KEYPADDIVIDE           98
#define MY_SCANCODE_PRINTSCREEN            99
#define MY_SCANCODE_RIGHTALT               100
#define MY_SCANCODE_BREAK                  101    /* Beware: is 119     */
#define MY_SCANCODE_BREAK_ALTERNATIVE      119    /* on some keyboards! */

#define MY_SCANCODE_HOME                   102
#define MY_SCANCODE_CURSORBLOCKUP          103    /* Cursor key block */
#define MY_SCANCODE_PAGEUP                 104
#define MY_SCANCODE_CURSORBLOCKLEFT        105    /* Cursor key block */
#define MY_SCANCODE_CURSORBLOCKRIGHT       106    /* Cursor key block */
#define MY_SCANCODE_END                    107
#define MY_SCANCODE_CURSORBLOCKDOWN        108    /* Cursor key block */
#define MY_SCANCODE_PAGEDOWN               109
#define MY_SCANCODE_INSERT                 110
#define MY_SCANCODE_REMOVE                 111

#define MY_SCANCODE_PAUSE                  119

#define MY_SCANCODE_POWER                  120
#define MY_SCANCODE_SLEEP                  121
#define MY_SCANCODE_WAKEUP                 122

#define MY_SCANCODE_LEFTWIN                125
#define MY_SCANCODE_RIGHTWIN               126
#define MY_SCANCODE_MENU                   127

#define MY_SCANCODE_FN					   128
#define MY_SCANCODE_UP					   129
#define MY_SCANCODE_DOWN				   130
#define MY_SCANCODE_LEFT				   131
#define MY_SCANCODE_RIGHT				   132
#define MY_SCANCODE_OK					   133

#define WHEEL_UP    0x10
#define WHEEL_DOWN  0x08 

#define BUTTON_L    0x04
#define BUTTON_M    0x02
#define BUTTON_R    0x01

#define MIN_COORD   -32767
#define MAX_COORD   32767

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitVP2008Input (INPUT* input, const char* mdev, const char* mtype);
void    TermVP2008Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_VP2008_H */


