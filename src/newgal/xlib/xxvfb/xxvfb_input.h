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
#ifndef _XXVFB_INPUT_H
#define _XXVFB_INPUT_H

typedef int BOOL;
#define SCANCODE_USER                   (NR_KEYS + 1)

#define SCANCODE_ESCAPE                 1

#define SCANCODE_1                      2
#define SCANCODE_2                      3
#define SCANCODE_3                      4
#define SCANCODE_4                      5
#define SCANCODE_5                      6
#define SCANCODE_6                      7
#define SCANCODE_7                      8
#define SCANCODE_8                      9
#define SCANCODE_9                      10
#define SCANCODE_0                      11

#define SCANCODE_MINUS                  12
#define SCANCODE_EQUAL                  13

#define SCANCODE_BACKSPACE              14
#define SCANCODE_TAB                    15

#define SCANCODE_Q                      16
#define SCANCODE_W                      17
#define SCANCODE_E                      18
#define SCANCODE_R                      19
#define SCANCODE_T                      20
#define SCANCODE_Y                      21
#define SCANCODE_U                      22
#define SCANCODE_I                      23
#define SCANCODE_O                      24
#define SCANCODE_P                      25
#define SCANCODE_BRACKET_LEFT           26
#define SCANCODE_BRACKET_RIGHT          27

#define SCANCODE_ENTER                  28

#define SCANCODE_LEFTCONTROL            29

#define SCANCODE_A                      30
#define SCANCODE_S                      31
#define SCANCODE_D                      32
#define SCANCODE_F                      33
#define SCANCODE_G                      34
#define SCANCODE_H                      35
#define SCANCODE_J                      36
#define SCANCODE_K                      37
#define SCANCODE_L                      38
#define SCANCODE_SEMICOLON              39
#define SCANCODE_APOSTROPHE             40
#define SCANCODE_GRAVE                  41

#define SCANCODE_LEFTSHIFT              42
#define SCANCODE_BACKSLASH              43

#define SCANCODE_Z                      44
#define SCANCODE_X                      45
#define SCANCODE_C                      46
#define SCANCODE_V                      47
#define SCANCODE_B                      48
#define SCANCODE_N                      49
#define SCANCODE_M                      50
#define SCANCODE_COMMA                  51
#define SCANCODE_PERIOD                 52
#define SCANCODE_SLASH                  53

#define SCANCODE_RIGHTSHIFT             54
#define SCANCODE_KEYPADMULTIPLY         55

#define SCANCODE_LEFTALT                56
#define SCANCODE_SPACE                  57
#define SCANCODE_CAPSLOCK               58

#define SCANCODE_F1                     59
#define SCANCODE_F2                     60
#define SCANCODE_F3                     61
#define SCANCODE_F4                     62
#define SCANCODE_F5                     63
#define SCANCODE_F6                     64
#define SCANCODE_F7                     65
#define SCANCODE_F8                     66
#define SCANCODE_F9                     67
#define SCANCODE_F10                    68

#define SCANCODE_NUMLOCK                69
#define SCANCODE_SCROLLLOCK             70

#define SCANCODE_KEYPAD7                71
#define SCANCODE_CURSORUPLEFT           71
#define SCANCODE_KEYPAD8                72
#define SCANCODE_CURSORUP               72
#define SCANCODE_KEYPAD9                73
#define SCANCODE_CURSORUPRIGHT          73
#define SCANCODE_KEYPADMINUS            74
#define SCANCODE_KEYPAD4                75
#define SCANCODE_CURSORLEFT             75
#define SCANCODE_KEYPAD5                76
#define SCANCODE_KEYPAD6                77
#define SCANCODE_CURSORRIGHT            77
#define SCANCODE_KEYPADPLUS             78
#define SCANCODE_KEYPAD1                79
#define SCANCODE_CURSORDOWNLEFT         79
#define SCANCODE_KEYPAD2                80
#define SCANCODE_CURSORDOWN             80
#define SCANCODE_KEYPAD3                81
#define SCANCODE_CURSORDOWNRIGHT        81
#define SCANCODE_KEYPAD0                82
#define SCANCODE_KEYPADPERIOD           83

#define SCANCODE_LESS                   86

#define SCANCODE_F11                    87
#define SCANCODE_F12                    88

#define SCANCODE_KEYPADENTER            96
#define SCANCODE_RIGHTCONTROL           97
#define SCANCODE_CONTROL                97
#define SCANCODE_KEYPADDIVIDE           98
#define SCANCODE_PRINTSCREEN            99
#define SCANCODE_RIGHTALT               100
#define SCANCODE_BREAK                  101    /* Beware: is 119     */
#define SCANCODE_BREAK_ALTERNATIVE      119    /* on some keyboards! */

#define SCANCODE_HOME                   102
#define SCANCODE_CURSORBLOCKUP          103    /* Cursor key block */
#define SCANCODE_PAGEUP                 104
#define SCANCODE_CURSORBLOCKLEFT        105    /* Cursor key block */
#define SCANCODE_CURSORBLOCKRIGHT       106    /* Cursor key block */
#define SCANCODE_END                    107
#define SCANCODE_CURSORBLOCKDOWN        108    /* Cursor key block */
#define SCANCODE_PAGEDOWN               109
#define SCANCODE_INSERT                 110
#define SCANCODE_REMOVE                 111

#define SCANCODE_PAUSE                  119

#define SCANCODE_POWER                  120
#define SCANCODE_SLEEP                  121
#define SCANCODE_WAKEUP                 122

#define SCANCODE_LEFTWIN                125
#define SCANCODE_RIGHTWIN               126
#define SCANCODE_MENU                   127

#define SCANCODE_LEFTBUTTON             0x1000
#define SCANCODE_RIGHTBUTTON            0x2000
#define SCANCODE_MIDDLBUTTON            0x4000

void SendMouseData(int x, int y, int buttons);

void init_code_map (void);
void sendUnPressedKeys();
BOOL clearPressedKey(int keycode);
int setPressedKey(int keycode,int unicode);
int isAlpha(unsigned char keycode);
#endif /*_XXVFB_INPUT_H*/
