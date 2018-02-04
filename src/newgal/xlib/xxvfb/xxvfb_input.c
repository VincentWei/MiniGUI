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
#include <stdio.h>

#include "xxvfbhdr.h"
#include "xxvfb_input.h"

extern XXVFBInfo xInfo;
static unsigned char keycode_scancode [256];
static int unpressedKey[50][2];

void init_code_map (void)
{
    memset(unpressedKey,0,50*2);
    keycode_scancode [0x00] = SCANCODE_ESCAPE;

    keycode_scancode [0x29] = SCANCODE_0;
    keycode_scancode [0x21] = SCANCODE_1;
    keycode_scancode [0x40] = SCANCODE_2;
    keycode_scancode [0x23] = SCANCODE_3;
    keycode_scancode [0x24] = SCANCODE_4;
    keycode_scancode [0x25] = SCANCODE_5;
    keycode_scancode [0x5E] = SCANCODE_6;
    keycode_scancode [0x26] = SCANCODE_7;
    keycode_scancode [0x2A] = SCANCODE_8;
    keycode_scancode [0x28] = SCANCODE_9;

    keycode_scancode [0x30] = SCANCODE_0;
    keycode_scancode [0x31] = SCANCODE_1;
    keycode_scancode [0x32] = SCANCODE_2;
    keycode_scancode [0x33] = SCANCODE_3;
    keycode_scancode [0x34] = SCANCODE_4;
    keycode_scancode [0x35] = SCANCODE_5;
    keycode_scancode [0x36] = SCANCODE_6;
    keycode_scancode [0x37] = SCANCODE_7;
    keycode_scancode [0x38] = SCANCODE_8;
    keycode_scancode [0x39] = SCANCODE_9;

//    keycode_scancode [0x30] = SCANCODE_F1;
//    keycode_scancode [0x31] = SCANCODE_F2;
//    keycode_scancode [0x32] = SCANCODE_F3;
//    keycode_scancode [0x33] = SCANCODE_F4;
//    keycode_scancode [0x34] = SCANCODE_F5;
//    keycode_scancode [0x35] = SCANCODE_F6;
//    keycode_scancode [0x36] = SCANCODE_F7;
//    keycode_scancode [0x37] = SCANCODE_F8;
//    keycode_scancode [0x38] = SCANCODE_F9;
//    keycode_scancode [0x39] = SCANCODE_F10;

    keycode_scancode [0x2D] = SCANCODE_MINUS;
    keycode_scancode [0x5F] = SCANCODE_MINUS;
    keycode_scancode [0x3D] = SCANCODE_EQUAL;
    keycode_scancode [0x2B] = SCANCODE_EQUAL;
    keycode_scancode [0x03] = SCANCODE_BACKSPACE;
    keycode_scancode [0x01] = SCANCODE_TAB;
    keycode_scancode [0x51] = SCANCODE_Q;
    keycode_scancode [0x57] = SCANCODE_W;
    keycode_scancode [0x45] = SCANCODE_E;
    keycode_scancode [0x52] = SCANCODE_R;
    keycode_scancode [0x54] = SCANCODE_T;
    keycode_scancode [0x59] = SCANCODE_Y;
    keycode_scancode [0x55] = SCANCODE_U;
    keycode_scancode [0x49] = SCANCODE_I;
    keycode_scancode [0x4F] = SCANCODE_O;
    keycode_scancode [0x50] = SCANCODE_P;
    keycode_scancode [0x5B] = SCANCODE_BRACKET_LEFT;
    keycode_scancode [0x7B] = SCANCODE_BRACKET_LEFT;
    keycode_scancode [0x5D] = SCANCODE_BRACKET_RIGHT;
    keycode_scancode [0x7D] = SCANCODE_BRACKET_RIGHT;
    keycode_scancode [0x04] = SCANCODE_ENTER;
    keycode_scancode [0x41] = SCANCODE_A;
    keycode_scancode [0x53] = SCANCODE_S;
    keycode_scancode [0x44] = SCANCODE_D;
    keycode_scancode [0x46] = SCANCODE_F;
    keycode_scancode [0x47] = SCANCODE_G;
    keycode_scancode [0x48] = SCANCODE_H;
    keycode_scancode [0x4A] = SCANCODE_J;
    keycode_scancode [0x4B] = SCANCODE_K;
    keycode_scancode [0x4C] = SCANCODE_L;

    keycode_scancode [0x3A] = SCANCODE_SEMICOLON;
    keycode_scancode [0x3B] = SCANCODE_SEMICOLON;
//    keycode_scancode [0x3B] = SCANCODE_F12;


    keycode_scancode [0x27] = SCANCODE_APOSTROPHE;
    keycode_scancode [0x22] = SCANCODE_APOSTROPHE;
    keycode_scancode [0x60] = SCANCODE_GRAVE;
    keycode_scancode [0x7E] = SCANCODE_GRAVE;

    keycode_scancode [0x20] = SCANCODE_SPACE;
//    keycode_scancode [0x20] = SCANCODE_LEFTSHIFT;

    keycode_scancode [0x5C] = SCANCODE_BACKSLASH;
    keycode_scancode [0x7C] = SCANCODE_BACKSLASH;
    keycode_scancode [0x5A] = SCANCODE_Z;
    keycode_scancode [0x58] = SCANCODE_X;
    keycode_scancode [0x43] = SCANCODE_C;
    keycode_scancode [0x56] = SCANCODE_V;
    keycode_scancode [0x42] = SCANCODE_B;
    keycode_scancode [0x4E] = SCANCODE_N;
    keycode_scancode [0x4D] = SCANCODE_M;
    keycode_scancode [0x2C] = SCANCODE_COMMA;
    keycode_scancode [0x3C] = SCANCODE_COMMA;
    keycode_scancode [0x2E] = SCANCODE_PERIOD;
    keycode_scancode [0x3E] = SCANCODE_PERIOD;
    keycode_scancode [0x2F] = SCANCODE_SLASH;
    keycode_scancode [0x3F] = SCANCODE_SLASH;
//    keycode_scancode [0x2A] = SCANCODE_KEYPADMULTIPLY;

    keycode_scancode [0x09] = SCANCODE_PRINTSCREEN;
    keycode_scancode [0x08] = SCANCODE_BREAK;
    keycode_scancode [0x06] = SCANCODE_INSERT;
    keycode_scancode [0x07] = SCANCODE_REMOVE;
    keycode_scancode [0x10] = SCANCODE_HOME;
    keycode_scancode [0x11] = SCANCODE_END;
    keycode_scancode [0x16] = SCANCODE_PAGEUP;
    keycode_scancode [0x17] = SCANCODE_PAGEDOWN;
    keycode_scancode [0x13] = SCANCODE_CURSORBLOCKUP;
    keycode_scancode [0x12] = SCANCODE_CURSORBLOCKLEFT;
    keycode_scancode [0x14] = SCANCODE_CURSORBLOCKRIGHT;
    keycode_scancode [0x15] = SCANCODE_CURSORBLOCKDOWN;

//    keycode_scancode [0x23] = SCANCODE_RIGHTALT;
//    keycode_scancode [0x2F] = SCANCODE_KEYPADDIVIDE;
//    keycode_scancode [0x20] = SCANCODE_RIGHTSHIFT;
//    keycode_scancode [0x21] = SCANCODE_RIGHTCONTROL;
//    keycode_scancode [0x37] = SCANCODE_KEYPAD7;
//    keycode_scancode [0x38] = SCANCODE_KEYPAD8;
//    keycode_scancode [0x39] = SCANCODE_KEYPAD9;
//    keycode_scancode [0x2D] = SCANCODE_KEYPADMINUS;
//    keycode_scancode [0x34] = SCANCODE_KEYPAD4;
//    keycode_scancode [0x35] = SCANCODE_KEYPAD5;
//    keycode_scancode [0x36] = SCANCODE_KEYPAD6;
//    keycode_scancode [0x2B] = SCANCODE_KEYPADPLUS;
//    keycode_scancode [0x31] = SCANCODE_KEYPAD1;
//    keycode_scancode [0x32] = SCANCODE_KEYPAD2;
//    keycode_scancode [0x33] = SCANCODE_KEYPAD3;
//    keycode_scancode [0x30] = SCANCODE_KEYPAD0;
//    keycode_scancode [0x2E] = SCANCODE_KEYPADPERIOD;
//    keycode_scancode [] = SCANCODE_LESS;
//    keycode_scancode [] = SCANCODE_KEYPADENTER;
//    keycode_scancode [] = SCANCODE_PAUSE;
//    keycode_scancode [] = SCANCODE_LEFTWIN;
//    keycode_scancode [] = SCANCODE_RIGHTWIN;
//    keycode_scancode [] = SCANCODE_MENU;
}
static unsigned char keycode_to_scancode (unsigned char keycode, BOOL asscii)
{
    if(!asscii)
    {
        switch (keycode) {
            case 0x30:
            case 0x31:
            case 0x32:
            case 0x33:
            case 0x34:
            case 0x35:
            case 0x36:
            case 0x37:
            case 0x38:
            case 0x39:
                return SCANCODE_F1 + keycode - 0x30;
            case 0x3B:
                return SCANCODE_F12;
            case 0x20:
                return SCANCODE_LEFTSHIFT;
            case 0x21:
                return SCANCODE_LEFTCONTROL;
            case 0x23:
                return SCANCODE_LEFTALT;
            case 0x24:
                return SCANCODE_CAPSLOCK;
            case 0x25:
                return SCANCODE_NUMLOCK;
            case 0x26:
                return SCANCODE_SCROLLLOCK;
            case 0x3A:
                return SCANCODE_F11;
        }
    }

    return keycode_scancode [keycode];
}

void SendMouseData(int x, int y, int buttons )
{
    XXVFBEventData event;
    event.event_type            = MOUSE_TYPE;
	event.data.mouse.x 		    = x;
	event.data.mouse.y 		    = y;
	event.data.mouse.button     = buttons;
    
    write( xInfo.sockfd, &event, sizeof(XXVFBEventData) );
}

int isAlpha(unsigned char keycode)
{
	if (keycode > 0x41 && keycode < 0x5a || keycode == 0x20 || keycode == 0x21)
		return 1;
	else
		return 0;
}

void sendKeyboardData( int unicode, int keycode, int modifiers,
        BOOL press, BOOL repeat )
{
	XXVFBEventData event;
    unsigned char scancode;

    scancode = keycode_to_scancode (keycode, unicode);
    event.event_type         = KB_TYPE;
	event.data.key.key_code  = scancode;
	event.data.key.key_state = press | repeat << 8;

    write( xInfo.sockfd, &event, sizeof(XXVFBEventData) );
}


int setPressedKey(int keycode,int unicode)
{
    int i = 0;
	for(i=0; unpressedKey[i][0] && i < sizeof(unpressedKey)/sizeof(unpressedKey[0]); i++)
	{
		if(unpressedKey[i][0] == keycode)
			return 0;
	}
	unpressedKey[i][0] = keycode;
	unpressedKey[i][1] = unicode;
	unpressedKey[++i][0] = 0;
    return 1;
}
BOOL clearPressedKey(int keycode)
{
	int i = 0;
	int find_idx = -1;
	for(i=0; unpressedKey[i][0] && i < sizeof(unpressedKey)/sizeof(unpressedKey[0]); i++)
	{
		if(keycode == unpressedKey[i][0])
			find_idx = i;
	}
	//printf("--clean key code=%d, find:%d\n",keycode, find_idx);

	if(find_idx == -1)
		return 0;
	if(--i >= 0)
	{
		if(find_idx != i)
		{
			unpressedKey[find_idx][0] = unpressedKey[i][0];
			unpressedKey[find_idx][1] = unpressedKey[i][1];
		}
		unpressedKey[i][0] = 0;
	}

	return 1;
}
void sendUnPressedKeys()
{
	int i = 0;
	for(i=0; unpressedKey[i][0] && i < sizeof(unpressedKey)/sizeof(unpressedKey[0]); i++)
	{
		sendKeyboardData(unpressedKey[i][1], unpressedKey[i][0], 0, 0, 0);
		unpressedKey[i][0] = 0;
	}
}
