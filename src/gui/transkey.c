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
** transkey.c: The Key Messages Translator
**
** Create date: 1999.05 by Wei Yongming.
**
** Used abbreviations:
**  ime: input method
*/

#include <ctype.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"

//table1, translating depends on capslock & shift
static const char keymap11 [] = { 0,
27,'1','2','3','4','5','6','7','8','9','0',               //11
'-','=','\b','\t',                                        //4
'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,   //14   
'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',  //14
'z','x','c','v','b','n','m',',','.','/',0,'*',0,' ',0,    //15
};
static const char keymap12 [] = { 0,
27,'!','@','#','$','%','^','&','*','(',')','_','+','\b',
'\t',
'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',0,
'A','S','D','F','G','H','J','K','L',':','"','~',0,'|',
'Z','X','C','V','B','N','M','<','>','?',0,'*',0,' ',0,
};

#define TABLE_SPLIT_VAL 58

//table2 translating depends on numlock & shift
static const char keymap21 [] = { 0,
0,0,0,0,0,0,0,0,0,0,0,0,    // f1-f10 numlock & scrolllock
0,0,0,'-',0,0,0,'+',0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,
'\n',0,'/'
};
static const char keymap22 [] = { 0,
0,0,0,0,0,0,0,0,0,0,0,0,    // f1-f10 numlock & scrolllock
'7','8','9','-','4','5','6','+','1','2','3','0','.',0,0,0,
0,0,0,0,0,0,0,0,0,
'\n',0,'/'
};

#define TABLE_SPLIT_VAL2 98

unsigned char GUIAPI KeyMessageToChar (PMSG pMsg)
{
    short caplock = 0, numlock = 0, shift = 0;

    if (pMsg->message != MSG_KEYDOWN 
                    && pMsg->message != MSG_SYSKEYDOWN 
                    && pMsg->message != MSG_KEYUP 
                    && pMsg->message != MSG_SYSKEYUP 
                    && pMsg->message != MSG_KEYALWAYSPRESS 
                    && pMsg->message != MSG_KEYLONGPRESS) {
        return 0;
    }

    if (pMsg->lParam & KS_CTRL)
        return 0;

    if (pMsg->wParam <= TABLE_SPLIT_VAL2)
    {
        if (pMsg->lParam & KS_CAPSLOCK)
            caplock=1;
        if (pMsg->lParam & KS_NUMLOCK)
            numlock=1;
        if (pMsg->lParam & KS_SHIFT)
            shift=1;

        if (pMsg->wParam <= TABLE_SPLIT_VAL)  // translate using table1
        {
            // alpha chars, capslock affects
            if (isalpha(keymap11[pMsg ->wParam])) {

                // caps lock not on or shift & caps lock both on
                if (! (caplock ^ shift))
                    return keymap11 [pMsg -> wParam];
                else
                    return keymap12 [pMsg -> wParam];
            }
            // non-alpha, capslock doesn't matter
            else if (shift)
                return keymap12 [pMsg -> wParam];
            else 
                return keymap11 [pMsg -> wParam];
        }
        else
        {
            // caps lock not on or shift & caps lock both on
            if (!(numlock ^ shift))
                return keymap21 [pMsg -> wParam - TABLE_SPLIT_VAL];
            else
                return keymap22 [pMsg -> wParam - TABLE_SPLIT_VAL];
        }
    }
    else
        return 0;
}

BOOL GUIAPI TranslateMessage (PMSG pMsg)
{
    unsigned char iChar;

    if ((pMsg->hwnd != HWND_DESKTOP)
        && (pMsg->message == MSG_KEYDOWN || pMsg->message == MSG_SYSKEYDOWN)) {
        // can be transed to WM_CHAR
        if ((iChar = KeyMessageToChar (pMsg))) {
            if (pMsg->message == MSG_SYSKEYDOWN)  
                SendMessage (pMsg->hwnd, 
                            MSG_SYSCHAR, iChar, pMsg->lParam);
            else
                SendMessage (pMsg->hwnd,
                            MSG_CHAR, iChar, pMsg->lParam);

            return TRUE;
        }
        
    }

    return FALSE; 
}

