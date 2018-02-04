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
** vxi386_input.c: IAL engine for VxWorks i386 PC target.
**
** Author: Yan Xiaowei
**
** Create Date: 2005-04-20
**/

#include <common.h>

/*WINML2.0 reference*/
#if defined(__VXWORKS__) && defined(__TARGET_PPC__)
#include <ugl/uglinput.h>
#include <ugl/uglugi.h>
#include <ugl/ugl.h>
#include <ugl/uglucode.h>

#define VX_MOUSEINPUT    0x01
#define VX_KBINPUT       0x02

static UGL_EVENT_SERVICE_ID eventServiceId;
static UGL_INPUT_EVENT *pInputEvent;
static UGL_EVENT event;

int ascii_to_scancode(int asc2);

int comm_ts_getdata (short *x, short *y, short *button)
{   
    int tmp;

   	*x = pInputEvent->x;
   	*y = pInputEvent->y;

    *x<1024 ? *x : 1024 ;
    *y<768 ? *y : 768 ;
    
   	*button = (pInputEvent->type.pointer.buttonState) ;

    return 0;
}
int comm_kb_getdata (short *key, short *status)
{
   *key = ascii_to_scancode (pInputEvent->type.keyboard.key);
   if (pInputEvent->modifiers & UGL_KEYBOARD_KEYDOWN)
   	   *status = 1;
   else
   	   *status = 0;
   
   return 0; 
}
int comm_wait_for_input (void)
{
    UGL_STATUS status;
    __mg_os_time_delay(10);
    
    status = uglInputEventGet (eventServiceId, UGL_NO_WAIT, (UGL_EVENT *)pInputEvent, sizeof (UGL_EVENT));
    
    if (status == UGL_STATUS_OK)
    {
        if (pInputEvent->header.type == UGL_EVENT_TYPE_KEYBOARD){
            pInputEvent->header.type = 0;
            return VX_KBINPUT;
        }else if (pInputEvent->header.type == UGL_EVENT_TYPE_POINTER) {
            pInputEvent->header.type = 0;
            return VX_MOUSEINPUT;
        }
    }
    return 0;
}

static int scancode_table[SCANCODE_USER]=
{ 
0,		         //0
0,
0,
0,
0,
0,
0,
0,
SCANCODE_BACKSPACE,	//8
SCANCODE_TAB,
0,
0,
0,
SCANCODE_ENTER,		//13
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
SCANCODE_ESCAPE,	//27
0,
0,
0,
0,
0,
SCANCODE_1, /*sign (!) 33 */
SCANCODE_APOSTROPHE, /*sign (")*/
SCANCODE_3, /*sign(#)*/
SCANCODE_4, /*sign($)*/
SCANCODE_5, /*sign(%)*/
SCANCODE_7, /*sign (&)*/
SCANCODE_APOSTROPHE, //sign (')
SCANCODE_9, /*sign (()*/
SCANCODE_0, /*sign ())*/
SCANCODE_8, /* sign(*) */
SCANCODE_EQUAL, /*sign (+)*/
SCANCODE_COMMA,			//44,
SCANCODE_MINUS,     	//45
SCANCODE_PERIOD,
SCANCODE_SLASH, // /
SCANCODE_0,		//48                     
SCANCODE_1,		//49     
SCANCODE_2,		//50
SCANCODE_3,		//51
SCANCODE_4,             //52         
SCANCODE_5,		//53                    
SCANCODE_6,		//54
SCANCODE_7,		//55                      
SCANCODE_8,		//56                      
SCANCODE_9,		//57
SCANCODE_SEMICOLON,	//58:
SCANCODE_SEMICOLON,	//59;
0,
SCANCODE_EQUAL,		//61
0,
0,
SCANCODE_2,     /* @ */
SCANCODE_A,		//65
SCANCODE_B,
SCANCODE_C,
SCANCODE_D,
SCANCODE_E,		
SCANCODE_F,
SCANCODE_G,
SCANCODE_H,
SCANCODE_I,	
SCANCODE_J,
SCANCODE_K,
SCANCODE_L,
SCANCODE_M,
SCANCODE_N,
SCANCODE_O,		
SCANCODE_P,		                    
SCANCODE_Q,		
SCANCODE_R,		
SCANCODE_S,
SCANCODE_T,		
SCANCODE_U,		
SCANCODE_V,
SCANCODE_W,		
SCANCODE_X,
SCANCODE_Y,		
SCANCODE_Z,		//90
SCANCODE_BRACKET_LEFT,  /* [ */
SCANCODE_BACKSLASH,  /* \ */
SCANCODE_BRACKET_RIGHT, /* ] */
SCANCODE_6,
SCANCODE_MINUS,     	/* _ (95)*/
0,
SCANCODE_A,		//97
SCANCODE_B,
SCANCODE_C,
SCANCODE_D,
SCANCODE_E,		
SCANCODE_F,
SCANCODE_G,
SCANCODE_H,
SCANCODE_I,	
SCANCODE_J,
SCANCODE_K,
SCANCODE_L,
SCANCODE_M,
SCANCODE_N,
SCANCODE_O,		
SCANCODE_P,		                    
SCANCODE_Q,		
SCANCODE_R,		
SCANCODE_S,
SCANCODE_T,		
SCANCODE_U,		
SCANCODE_V,
SCANCODE_W,		
SCANCODE_X,
SCANCODE_Y,		
SCANCODE_Z	,	//122
};

//0xE000
static int scancode_private1_table[22] = 
{
SCANCODE_HOME,
SCANCODE_END,
SCANCODE_INSERT,
SCANCODE_PAGEUP,
SCANCODE_PAGEDOWN,
SCANCODE_CURSORBLOCKLEFT,
SCANCODE_CURSORBLOCKRIGHT,
SCANCODE_CURSORBLOCKUP,
SCANCODE_CURSORBLOCKDOWN,
SCANCODE_PRINTSCREEN,
SCANCODE_PAUSE,
SCANCODE_CAPSLOCK,
SCANCODE_NUMLOCK,
SCANCODE_SCROLLLOCK,

SCANCODE_LEFTSHIFT,
SCANCODE_RIGHTSHIFT,
SCANCODE_LEFTCONTROL,
SCANCODE_RIGHTCONTROL,
SCANCODE_LEFTALT,
SCANCODE_RIGHTALT,
0,
0,
};

//0xEf00
static int scancode_private2_table[13] = 
{
0,//0
SCANCODE_F1,                     
SCANCODE_F2,                     
SCANCODE_F3,                     
SCANCODE_F4,                     
SCANCODE_F5,                     
SCANCODE_F6,                     
SCANCODE_F7,                     
SCANCODE_F8,                     
SCANCODE_F9,                     
SCANCODE_F10,
SCANCODE_F11,
SCANCODE_F12,
};

int ascii_to_scancode (int asc2)
{
    unsigned int ret, index=0;
    
    ret = (asc2) & 0xef00;
    index = asc2 & 0x00ff;
    
    if (ret==0) {
        return scancode_table[index]; 
    } else if (ret == 0xe000) {
        return scancode_private1_table [index];
    } else if (ret == 0xef00) {
        return scancode_private2_table[index];
    }      
    return 0;
}

#endif /*(__VXWORKS__) && (__TARGET_PPC__)*/


