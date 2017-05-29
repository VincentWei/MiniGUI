/*
** $Id: transkey.c 10660 2008-08-14 09:30:39Z weiym $
**
** transkey.c: The Key Messages Translator
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** All rights reserved by Feynman Software.
**
** Current maintainer: Wei Yongming.
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

