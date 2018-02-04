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
** hi3510.c: Input Engine for hi3510
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _HI3510_IAL

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "minigui.h"
#include "ial.h"

/* socket relative */
#define SOCKET_MOUSE_IP      htonl (INADDR_ANY)
#define SOCKET_MOUSE_PORT    htons (5003)

#define SOCKET_KEYBOARD_IP    htonl (INADDR_ANY)
#define SOCKET_KEYBOARD_PORT  htons (5001)

#define SIM_INPUT_CONTENT_LEN   8
#define SIM_INPUT_TYPE_KEYPAD   0
#define SIM_INPUT_TYPE_MOUSE    1

#define NOBUTTON         0x0000
#define LEFTBUTTON       0x0001
#define RIGHTBUTTON      0x0002
#define MIDBUTTON        0x0004
#define MOUSEBUTTONMASK  0x00FF
#define KEYBOARD_NETWORK 0x01

typedef struct tagMapItem
{
    int  wSimKey;          /* simulate key code */
    BYTE    btVirtualKey1;    /* virtual keyborad keys mapping from sim-key */
    BYTE    btVirtualKey2;
} MAPITEM, *LPMAPITEM;

struct SimKeyData
{
    Uint16 keycode;
    BYTE   press;
    BYTE   repeat;
};

typedef struct tagSimInputMsg
{
    Uint32 ulInputType;
    Uint8  ucContent[SIM_INPUT_CONTENT_LEN];
}SIM_INPUT_MSG_S;

static int mouse_fd = -1;
static int kbd_fd = -1;
static int key_flag = 0;    /* to indicate which keyboard has data */
static POINT mouse_pt;
static int mouse_buttons;
static struct SimKeyData kbd_data = { 0 };
static unsigned char kbd_state [MGUI_NR_KEYS + 1] = { 0 };

/*custom keys difined below are used for STB, 
those macros will put in common.h*/
/*power*/
#define SCANCODE_V_POWER       (SCANCODE_USER)

/*mute*/
#define SCANCODE_V_MUTE        (SCANCODE_USER + 1)

/*num1*/
#define SCANCODE_V_NUM_1       (SCANCODE_1)

/*num2*/
#define SCANCODE_V_NUM_2       (SCANCODE_2)

/*num3*/
#define SCANCODE_V_NUM_3       (SCANCODE_3)

/*num4*/
#define SCANCODE_V_NUM_4       (SCANCODE_4)

/*num5*/
#define SCANCODE_V_NUM_5       (SCANCODE_5)

/*num6*/
#define SCANCODE_V_NUM_6       (SCANCODE_6)

/*num7*/
#define SCANCODE_V_NUM_7       (SCANCODE_7)

/*num8*/
#define SCANCODE_V_NUM_8       (SCANCODE_8)

/*num9*/
#define SCANCODE_V_NUM_9       (SCANCODE_9)

/*num0*/
#define SCANCODE_V_NUM_0       (SCANCODE_0)

/*input method*/
#define SCANCODE_V_IME         (SCANCODE_L)

#define SCANCODE_V_DIGITS      (SCANCODE_USER + 3)
#define SCANCODE_V_AUDIO       (SCANCODE_USER + 4)
#define SCANCODE_V_EPG         (SCANCODE_USER + 5)
#define SCANCODE_V_RECALL      (SCANCODE_USER + 6)
#define SCANCODE_V_CAPTION     (SCANCODE_USER + 7)
#define SCANCODE_V_CHANNEL_UP  (SCANCODE_USER + 8)
#define SCANCODE_V_CHANNEL_DOWN (SCANCODE_USER + 9)
#define SCANCODE_V_RED         (SCANCODE_USER + 10)
#define SCANCODE_V_GREEN       (SCANCODE_USER + 11)
#define SCANCODE_V_ORANGE      (SCANCODE_USER + 28)
#define SCANCODE_V_BLUE        (SCANCODE_USER + 29)
/*number switch*/
#define SCANCODE_V_SWITCH       (SCANCODE_USER + 12)

/*Back Space*/
#define SCANCODE_V_BACKSPACE   (SCANCODE_BACKSPACE)

/*channel*/
#define SCANCODE_V_CHANNEL     (SCANCODE_USER + 13)

/*menu*/
#define SCANCODE_V_MENU        (SCANCODE_USER + 14)

/*up*/
#define SCANCODE_V_UP          (SCANCODE_CURSORBLOCKUP)

/*left*/
#define SCANCODE_V_LEFT        (SCANCODE_CURSORBLOCKLEFT)

/*right*/
#define SCANCODE_V_RIGHT       (SCANCODE_CURSORBLOCKRIGHT)

/*down*/
#define SCANCODE_V_DOWN        (SCANCODE_CURSORBLOCKDOWN)

/*confirm*/
#define SCANCODE_V_CONFIRM     (SCANCODE_ENTER)

/*function Key*/
#define SCANCODE_V_F1          (SCANCODE_F1)
#define SCANCODE_V_F2          (SCANCODE_TAB)
#define SCANCODE_V_F3          (SCANCODE_F3)
#define SCANCODE_V_F4          (SCANCODE_F4)

/*go back*/
#define SCANCODE_V_GO_BACK     (SCANCODE_USER + 15)

/*favorite*/
#define SCANCODE_V_FAVORITE    (SCANCODE_USER + 16)

/*Escape*/
#define SCANCODE_V_ESCAPE      (SCANCODE_ESCAPE)

/*play*/
#define SCANCODE_V_PLAY        (SCANCODE_USER + 17)

/*pause*/
#define SCANCODE_V_PAUSE       (SCANCODE_BREAK)

/*stop*/
#define SCANCODE_V_STOP        (SCANCODE_USER + 18)

/*volume down*/
#define SCANCODE_V_VOLUME_DOWN (SCANCODE_USER + 19)

/*volume up*/
#define SCANCODE_V_VOLUME_UP   (SCANCODE_USER + 20)

/*quickly backward*/
#define SCANCODE_V_BACKWARD    (SCANCODE_USER + 21)

/*quickly forward*/
#define SCANCODE_V_FORWARD     (SCANCODE_USER + 22)

/*Locate*/
#define SCANCODE_V_LOCATE      (SCANCODE_USER + 23)

/*system*/
#define SCANCODE_V_SYSTEM      (SCANCODE_USER + 24)

/*setup*/
#define SCANCODE_V_CONFIG      (SCANCODE_USER + 25)

/*track*/
#define SCANCODE_V_TRACK       (SCANCODE_USER + 26)

/*info*/
#define SCANCODE_V_INFO        (SCANCODE_USER + 27)

/* key map table*/
static MAPITEM gs_stKeyMapTable [] = 
{    
    {0x0001,     SCANCODE_SPACE                 , 0}, // Key_Space    
    {0x0002,     SCANCODE_APOSTROPHE            , 0}, // Key_Apostrophe
    {0x0003,     SCANCODE_COMMA                 , 0}, // Key_Comma
    {0x0005,     SCANCODE_MINUS                 , 0}, // Key_Minus
    {0x0006,     SCANCODE_PERIOD                , 0}, // Key_Period
    {0x0007,     SCANCODE_SLASH                 , 0}, // Key_Slash
    {0x0008,     SCANCODE_SEMICOLON             , 0}, // Key_Semicolon
    {0x0009,     SCANCODE_EQUAL                 , 0}, // Key_Equal
    {0x000A,     SCANCODE_BACKSLASH             , 0}, // Key_BracketLeft
    {0x000B,     SCANCODE_GRAVE                 , 0}, // Key_Backslash 
    {0x000C,     SCANCODE_BRACKET_LEFT          , 0}, // Key_BracketRight 
    {0x000D,     SCANCODE_BRACKET_RIGHT         , 0}, // Key_QuoteLeft = 0x60 
    {0x000E,     SCANCODE_0                     , 0}, // Key_0
    {0x000F,     SCANCODE_1                     , 0}, // Key_1
    {0x0010,     SCANCODE_2                     , 0}, // Key_2
    {0x0011,     SCANCODE_3                     , 0}, // Key_3
    {0x0012,     SCANCODE_4                     , 0}, // Key_4
    {0x0013,     SCANCODE_5                     , 0}, // Key_5
    {0x0014,     SCANCODE_6                     , 0}, // Key_6
    {0x0015,     SCANCODE_7                     , 0}, // Key_7
    {0x0016,     SCANCODE_8                     , 0}, // Key_8
    {0x0017,     SCANCODE_9                     , 0}, // Key_9
    {0x0018,     SCANCODE_A                     , 0}, // Key_A
    {0x0019,     SCANCODE_B                     , 0}, // Key_B
    {0x001A,     SCANCODE_C                     , 0}, // Key_C
    {0x001B,     SCANCODE_D                     , 0}, // Key_D
    {0x001C,     SCANCODE_E                     , 0}, // Key_E
    {0x001D,     SCANCODE_F                     , 0}, // Key_F
    {0x001E,     SCANCODE_G                     , 0}, // Key_G
    {0x001F,     SCANCODE_H                     , 0}, // Key_H
    {0x0020,     SCANCODE_I                     , 0}, // Key_I
    {0x0021,     SCANCODE_J                     , 0}, // Key_J
    {0x0022,     SCANCODE_K                     , 0}, // Key_K
    {0x0023,     SCANCODE_L                     , 0}, // Key_L
    {0x0024,     SCANCODE_M                     , 0}, // Key_M
    {0x0025,     SCANCODE_N                     , 0}, // Key_N
    {0x0026,     SCANCODE_O                     , 0}, // Key_O
    {0x0027,     SCANCODE_P                     , 0}, // Key_P
    {0x0028,     SCANCODE_Q                     , 0}, // Key_Q
    {0x0029,     SCANCODE_R                     , 0}, // Key_R
    {0x002A,     SCANCODE_S                     , 0}, // Key_S
    {0x002B,     SCANCODE_T                     , 0}, // Key_T
    {0x002C,     SCANCODE_U                     , 0}, // Key_U
    {0x002D,     SCANCODE_V                     , 0}, // Key_V
    {0x002E,     SCANCODE_W                     , 0}, // Key_W
    {0x002F,     SCANCODE_X                     , 0}, // Key_X
    {0x0030,     SCANCODE_Y                     , 0}, // Key_Y
    {0x0031,     SCANCODE_Z                     , 0}, // Key_Z
    {0x1001,     SCANCODE_ESCAPE                , 0}, // Key_Escape
    {0x1002,     SCANCODE_TAB                   , 0}, // Key_Tab
    {0x1003,     SCANCODE_BACKSPACE             , 0}, // Key_Backspace 
    {0x1004,     SCANCODE_ENTER                 , 0}, // Key_Return  
    {0x1005,     SCANCODE_INSERT                , 0}, // Key_Insert
    {0x1006,     SCANCODE_REMOVE                , 0}, // Key_Delete
    {0x1007,     SCANCODE_PAUSE                 , 0}, // Key_Pause
    {0x1008,     SCANCODE_PRINTSCREEN           , 0}, // Key_Print    
    {0x1009,     SCANCODE_HOME                  , 0}, // Key_Home
    {0x100A,     SCANCODE_END                   , 0}, // Key_End
    {0x100B,     SCANCODE_CURSORBLOCKLEFT       , 0}, // Key_Left
    {0x100C,     SCANCODE_CURSORBLOCKUP         , 0}, // Key_Up
    {0x100D,     SCANCODE_CURSORBLOCKRIGHT      , 0}, // Key_Right
    {0x100E,     SCANCODE_CURSORBLOCKDOWN       , 0}, // Key_Down
    {0x100F,     SCANCODE_PAGEUP                , 0},  // Key_PageUp
    {0x1010,     SCANCODE_PAGEDOWN              , 0},  // Key_PageDown
    {0x1011,     SCANCODE_LEFTSHIFT             , 0}, // Key_Shift
    {0x1012,     SCANCODE_LEFTCONTROL           , 0},  // Key_Control 
    {0x1013,     SCANCODE_LEFTALT               , 0}, // Key_Alt
    {0x1014,     SCANCODE_CAPSLOCK              , 0}, // Key_CapsLock
    {0x1015,     SCANCODE_NUMLOCK               , 0},  // Key_NumLock
    {0x1016,     SCANCODE_SCROLLLOCK            , 0},  // Key_ScrollLock
    {0x1017,     SCANCODE_F1                    , 0},  // Key_F1
    {0x1018,     SCANCODE_F2                    , 0}, // Key_F2
    {0x1019,     SCANCODE_F3                    , 0}, // Key_F3
    {0x101A,     SCANCODE_F4                    , 0},  // Key_F4
    {0x101B,     SCANCODE_F5                    , 0},  // Key_F5
    {0x101C,     SCANCODE_F6                    , 0},  // Key_F6
    {0x101D,     SCANCODE_F7                    , 0},  // Key_F7
    {0x101E,     SCANCODE_F8                    , 0},  // Key_F8
    {0x101F,     SCANCODE_F9                    , 0},  // Key_F9
    {0x1020,     SCANCODE_F10                   , 0},  // Key_F10
    {0x1021,     SCANCODE_F11                   , 0},  // Key_F11
    {0x1022,     SCANCODE_F12                   , 0},  // Key_F12     
    {0x1023,     SCANCODE_MENU                  , 0},  // Key_Menu    
    {0x1101,     SCANCODE_V_POWER               , 0}, // 
    {0x1102,     SCANCODE_V_MUTE                , 0}, // 
    {0x1103,     SCANCODE_V_NUM_1               , 0}, // 
    {0x1104,     SCANCODE_V_NUM_2               , 0}, // 
    {0x1105,     SCANCODE_V_NUM_3               , 0}, //
    {0x1106,     SCANCODE_V_NUM_4               , 0}, // 
    {0x1107,     SCANCODE_V_NUM_5               , 0}, // 
    {0x1108,     SCANCODE_V_NUM_6               , 0}, // 
    {0x1109,     SCANCODE_V_NUM_7               , 0}, // 
    {0x1110,     SCANCODE_V_NUM_8               , 0}, // 
    {0x1111,     SCANCODE_V_NUM_9               , 0}, //
    {0x1112,     SCANCODE_V_NUM_0               , 0}, //
    {0x1113,     SCANCODE_V_SWITCH              , 0}, // 
    {0x1114,     SCANCODE_V_BACKSPACE           , 0}, // 
    {0x1115,     SCANCODE_V_CHANNEL             , 0}, // 
    {0x1116,     SCANCODE_V_MENU                , 0}, // 
    {0x1117,     SCANCODE_V_LEFT                , 0}, //
    {0x1118,     SCANCODE_V_RIGHT               , 0}, //
    {0x1119,     SCANCODE_V_UP                  , 0}, // 
    {0x1120,     SCANCODE_V_DOWN                , 0}, // 
    {0x1121,     SCANCODE_V_CONFIRM             , 0}, // 
    {0x1122,     SCANCODE_V_F1                  , 0}, // 
    {0x1123,     SCANCODE_V_F2                  , 0}, //
    {0x1124,     SCANCODE_V_F3                  , 0}, //
    {0x1125,     SCANCODE_V_F4                  , 0}, // 
    {0x1126,     SCANCODE_V_GO_BACK             , 0}, // 
    {0x1127,     SCANCODE_V_FAVORITE            , 0}, // 
    {0x1128,     SCANCODE_V_IME                 , 0}, // 
    {0x1129,     SCANCODE_V_ESCAPE              , 0}, //
    {0x1130,     SCANCODE_V_PLAY                , 0}, //
    {0x1131,     SCANCODE_V_PAUSE               , 0}, // 
    {0x1132,     SCANCODE_V_STOP                , 0}, // 
    {0x1133,     SCANCODE_V_BACKWARD            , 0}, // 
    {0x1134,     SCANCODE_V_FORWARD             , 0}, // 
    {0x1135,     SCANCODE_V_LOCATE              , 0}, //
    {0x1136,     SCANCODE_V_VOLUME_UP           , 0}, //
    {0x1137,     SCANCODE_V_VOLUME_DOWN         , 0}, // 
    {0x1138,     SCANCODE_V_SYSTEM              , 0}, // 
    {0x1139,     SCANCODE_V_CONFIG              , 0}, // 
    {0x1140,     SCANCODE_V_TRACK               , 0}, // 
    {0x1141,     SCANCODE_V_INFO                , 0}  //    
};

static int gs_iCurPanelKeys = sizeof (gs_stKeyMapTable) / sizeof (MAPITEM);

/*maxium scan code value of current mapping keys.*/
static int gs_iMaxScanCode = 0;
 
/*number of mapping keys's events to be fetched by up UI.*/
static int gs_iVKeysToBeHandled = 0; 

/*store last key map item.*/
static MAPITEM gs_stMapItem = {0xffff, 0, 0};

static LPMAPITEM GetMapItem (int wIndex)
{
    int low = 0;
    int hig = gs_iCurPanelKeys - 1;
    int mid;

    /* half-way finding.*/ 
    while (low <= hig)
    {
        mid = (low + hig) / 2;
        if (wIndex == gs_stKeyMapTable[mid].wSimKey)
        {
            return (gs_stKeyMapTable + mid);
        }
        else if (wIndex > gs_stKeyMapTable[mid].wSimKey)
        {
            low = mid + 1;
        }
        else
        {
            hig = mid - 1;
        }
    }

    return NULL;
}

static void init_code_map (void)
{ 
    int i,j;
    MAPITEM stItem;
    
    gs_iMaxScanCode = 0;

    for (i = 0; i < gs_iCurPanelKeys; ++i)
    {
        if (gs_iMaxScanCode < gs_stKeyMapTable[i].btVirtualKey1)
        {
            gs_iMaxScanCode = gs_stKeyMapTable[i].btVirtualKey1;
        }
        
        if (gs_iMaxScanCode < gs_stKeyMapTable[i].btVirtualKey2)
        {
            gs_iMaxScanCode = gs_stKeyMapTable[i].btVirtualKey2;
        }
    }


    /* Then sort the map table
       use bubble-sort*/
    for (i = 0; i < gs_iCurPanelKeys - 1; i++)
    {
        for (j = 0; j < gs_iCurPanelKeys - 1 - i; j++)
        {
            if (gs_stKeyMapTable [j].wSimKey > gs_stKeyMapTable [j + 1].wSimKey)
            {
                stItem = gs_stKeyMapTable [j];
                gs_stKeyMapTable[j] = gs_stKeyMapTable [j + 1];
                gs_stKeyMapTable [j + 1] = stItem;
            }
        }
    }
    
    /*maxium of scan code must greater than all keys' scan code*/
    gs_iMaxScanCode += 1; 
}

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update (void)
{
    int ret;
    SIM_INPUT_MSG_S struSimInputData;
    struct sockaddr_in from;
    socklen_t fromsize;

#ifdef _DEBUG
    vlog( "Enter mouse_update\n");
#endif

    fromsize  = sizeof(struct sockaddr);

    ret = recvfrom (mouse_fd, &struSimInputData,
        sizeof (SIM_INPUT_MSG_S), 0, (struct sockaddr *)&from, &fromsize);

    if (ret == sizeof (SIM_INPUT_MSG_S))
    {
        if( SIM_INPUT_TYPE_MOUSE == struSimInputData.ulInputType )
        {
            mouse_pt.x    = (struSimInputData.ucContent[0] << 8) 
                | struSimInputData.ucContent[1];
            mouse_pt.y    = (struSimInputData.ucContent[2] << 8) 
                | struSimInputData.ucContent[3];
            mouse_buttons = struSimInputData.ucContent[4];
#ifdef _DEBUG
            vlog( "recv mouse x=%d,y=%d,button=%x\n",
                     mouse_pt.x,
                     mouse_pt.y,
                     mouse_buttons );
#endif
        }
        else
        {
#ifdef _DEBUG
            vlog("Mouse type not match!\n");
#endif
            return 0;
        }
    }
    else
    {
#ifdef _DEBUG
        vlog("Mouse date size not match!\n");
#endif
        return 0;
    }

    return 1;
}

static void mouse_getxy (int* x, int* y)
{
    *x = mouse_pt.x;
    *y = mouse_pt.y;
}

static int mouse_getbutton (void)
{
    int buttons = 0;

    if (mouse_buttons & LEFTBUTTON)
        buttons |= IAL_MOUSE_LEFTBUTTON;
    if (mouse_buttons & RIGHTBUTTON)
        buttons |= IAL_MOUSE_RIGHTBUTTON;
    if (mouse_buttons & MIDBUTTON)
        buttons |= IAL_MOUSE_MIDDLEBUTTON;

    return buttons;
}

static int keyboard_update (void)
{
    static BYTE s_btShiftState = 0; // up
    SIM_INPUT_MSG_S struSimInputData;
    int ret;
    LPMAPITEM pItem;    
    int wKeyCode;
    struct sockaddr_in from;
    socklen_t fromsize;

#ifdef _DEBUG
    vlog( "Enter Key_update\n");
#endif
    fromsize  = sizeof(struct sockaddr);

    if (gs_iVKeysToBeHandled == 0)
    {
        /* if network keyboard */
        if( key_flag & KEYBOARD_NETWORK )
        {
            ret = recvfrom (kbd_fd, &struSimInputData,
                sizeof (SIM_INPUT_MSG_S), 0, (struct sockaddr *)&from, &fromsize);

            if (ret == sizeof (SIM_INPUT_MSG_S))
            {
                if( SIM_INPUT_TYPE_KEYPAD == struSimInputData.ulInputType )
                {
                    kbd_data.keycode = (struSimInputData.ucContent[0] << 8) | struSimInputData.ucContent[1];
                    kbd_data.press   = struSimInputData.ucContent[2];
                    kbd_data.repeat  = struSimInputData.ucContent[3];
#ifdef _DEBUG
                    vlog( "recv key = %d\n", kbd_data.keycode );
#endif
                }
                else
                {
#ifdef _DEBUG
                    vlog("Key type not match!\n");
#endif
                    return 0;
                }
            }
            else
            {
#ifdef _DEBUG
                vlog("Key date size not match!\n");
#endif
                return 0;
            }
        }

        if (kbd_data.repeat)
        {                      
            return 1;
        }
        
#ifdef _DEBUG
        vlog( "IAL::keyboard_update:receiv key code %x\n", kbd_data.keycode);
#endif /* _DEBUG */
        
        wKeyCode = kbd_data.keycode;

        if((pItem = GetMapItem(wKeyCode)) == NULL)
        {
#ifdef _DEBUG
            vlog( "keyboard_update:Can't find qt-key %x in map talbe:\n", wKeyCode);
#endif
            return 0;
        }
        
        gs_stMapItem = *pItem;

        if (gs_stMapItem.btVirtualKey1)
        {                
            ++gs_iVKeysToBeHandled;                     
        }
        if (gs_stMapItem.btVirtualKey2)
        {                
            ++gs_iVKeysToBeHandled;                     
        }
    }

    if (gs_iVKeysToBeHandled)
    {
        if (kbd_data.press)
        {
            if (gs_iVKeysToBeHandled == 2)
            {
#ifdef _DEBUG
                vlog("map virtual key(%u) down message.\n",gs_stMapItem.btVirtualKey2);
#endif /* _DEBUG */  
                if (gs_stMapItem.btVirtualKey2 == SCANCODE_LEFTSHIFT)
                {
                    s_btShiftState = kbd_data.press;
                }
                kbd_state [gs_stMapItem.btVirtualKey2] = 1;
            }
            else 
            {
#ifdef _DEBUG
                vlog("map virtual key(%u) down message.\n",gs_stMapItem.btVirtualKey1);
#endif /* _DEBUG */    
                if (gs_stMapItem.btVirtualKey1 == SCANCODE_LEFTSHIFT)
                {
                    s_btShiftState = kbd_data.press;
                }
                kbd_state [gs_stMapItem.btVirtualKey1] = 1;
            }
        }
        else
        {
            if (gs_iVKeysToBeHandled == 2)
            {
                if (gs_stMapItem.btVirtualKey2 == SCANCODE_LEFTSHIFT)                    
                {
                    if (s_btShiftState)
                    {
                        s_btShiftState = 0;
                    }
                    else
                    {
                        --gs_iVKeysToBeHandled;
                        return 0;
                    }
                }
                
#ifdef _DEBUG
                vlog("map virtual key(%u) up message.\n",gs_stMapItem.btVirtualKey2);
#endif /* _DEBUG */    
                
                kbd_state [gs_stMapItem.btVirtualKey2] = 0;
            }
            else 
            {
                if (gs_stMapItem.btVirtualKey1 == SCANCODE_LEFTSHIFT)
                {
                    if (s_btShiftState)
                    {
                        s_btShiftState = 0;
                    }
                    else
                    {
                        --gs_iVKeysToBeHandled;
                        return 0;
                    }
                }
#ifdef _DEBUG
                vlog( "map virtual key(%u) up message.\n",gs_stMapItem.btVirtualKey1);
#endif /* _DEBUG */  
                
                kbd_state [gs_stMapItem.btVirtualKey1] = 0;
            }
        }
        
        --gs_iVKeysToBeHandled;
        return gs_iMaxScanCode;
    }
    else
    {
#ifdef _DEBUG
        vlog( "keyboard_update:no message to be update\n");
#endif /* _DEBUG */
        return 0;
    }    
}

static const char* keyboard_getstate (void)
{
#ifdef _DEBUG
    vlog( "Enter key get start\n" );
#endif
    return kbd_state;
}

/* NOTE by weiym: Do not ignore the fd_set in, out, and except */
#ifdef _LITE_VERSION
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#endif /* _LITE_VERSION */ 
{
    fd_set rfds;
    int    retvalue = 0;
    int    fd, e;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    if (which & IAL_KEYEVENT && (kbd_fd >= 0) )
    {
        if (gs_iVKeysToBeHandled > 0)
        {        
#ifdef _DEBUG
            vlog( "There are still some mapping virtual key "
                "to be handled, return immediately\n");
#endif /* _DEBUG */   

            if (in)
            {
                FD_ZERO (in);
            }
            
            if (out)
            {
                FD_ZERO (out);
            }
            
            retvalue |= IAL_KEYEVENT;
            return retvalue;
        }

        if( kbd_fd >= 0 )
        {
            fd = kbd_fd;
            FD_SET (kbd_fd, in);
#ifdef _LITE_VERSION
            if (fd > maxfd) maxfd = fd;
#endif
        }     
    }
    
    if (which & IAL_MOUSEEVENT && mouse_fd >= 0) {
        fd = mouse_fd;
        FD_SET (fd, in);
#ifdef _LITE_VERSION
        if (fd > maxfd) maxfd = fd;
#endif
    }    

    /* FIXME: pass the real set size */
#ifdef _LITE_VERSION
    e = select (maxfd + 1, in, out, except, timeout) ;
#else
    e = select (FD_SETSIZE, in, out, except, timeout) ;
#endif

    if (e > 0) {
        fd = mouse_fd;
        /* If data is present on the mouse fd, service it: */
        if (fd >= 0 && FD_ISSET (fd, in)) {
            FD_CLR (fd, in);
            retvalue |= IAL_MOUSEEVENT;
        }

        key_flag = 0;

        fd = kbd_fd;
        /* If data is present on the keyboard fd, service it: */
        if (fd >= 0 && FD_ISSET (fd, in)) {
            FD_CLR (fd, in);
            retvalue |= IAL_KEYEVENT;
            key_flag |= KEYBOARD_NETWORK;
        }      
    } else if (e < 0) {        
        return -1;
    }

#ifdef _DEBUG
    if (retvalue)
        vlog( "return val = %d\n" , retvalue );
#endif

    return retvalue;
}

BOOL InitHI3510Input (INPUT* input, const char* mdev, const char* mtype)
{
    struct sockaddr_in addr;      
    int iRes;

    kbd_fd = socket(PF_INET, SOCK_DGRAM, 0);
    
    if (kbd_fd < 0)
    {
        return FALSE;
    } 

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = SOCKET_KEYBOARD_IP;
    addr.sin_port = SOCKET_KEYBOARD_PORT;

    iRes = bind(kbd_fd, (struct sockaddr *)&addr, sizeof(addr));

    if (iRes < 0)
    {
        close(kbd_fd);
        fprintf (stderr, "Simulate Key Input Engine: Can not bind socket.\n");
        return FALSE;
    }    

    mouse_fd = socket(PF_INET, SOCK_DGRAM, 0);
    
    if (mouse_fd < 0)
    {
        close(kbd_fd);
        return FALSE;
    } 

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = SOCKET_MOUSE_IP;
    addr.sin_port = SOCKET_MOUSE_PORT;

    iRes = bind(mouse_fd, (struct sockaddr *)&addr, sizeof(addr));

    if (iRes < 0)
    {
        close(kbd_fd);
        close(mouse_fd);
        fprintf (stderr, "Simulate Key Input Engine: Can not bind socket.\n");
        return FALSE;
    }    


    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;
    input->suspend_mouse= NULL;
    input->resume_mouse = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->suspend_keyboard = NULL;
    input->resume_keyboard = NULL;
    input->set_leds = NULL;

    input->wait_event = wait_event;

    init_code_map();

    return TRUE;
}

void TermHI3510Input (void)
{
    if (mouse_fd >= 0)
    {
        close (mouse_fd);
        mouse_fd = -1;
    }

    if (kbd_fd >= 0)
    {
        close (kbd_fd);
        kbd_fd = -1;
    }
}

#endif
