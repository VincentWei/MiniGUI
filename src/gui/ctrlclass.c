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
** ctrlclass.c: the Control Class module.
**
** Create date: 1999/5/21
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"

#ifdef _MGCTRL_STATIC
extern BOOL RegisterStaticControl (void);
#endif
#ifdef _MGCTRL_BUTTON
extern BOOL RegisterButtonControl (void);
#endif
#ifdef _MGCTRL_SLEDIT
extern BOOL RegisterSLEditControl (void);
#endif
#ifdef _MGCTRL_BIDISLEDIT
extern BOOL RegisterBIDISLEditControl (void);
#endif
#ifdef _MGCTRL_PROGRESSBAR
extern BOOL RegisterProgressBarControl (void);
#endif
#ifdef _MGCTRL_LISTBOX
extern BOOL RegisterListboxControl (void);
#endif
#ifdef _MGCTRL_NEWTOOLBAR
extern BOOL RegisterNewToolbarControl (void);
#endif
#ifdef _MGCTRL_MENUBUTTON
extern BOOL RegisterMenuButtonControl (void);
#endif
#ifdef _MGCTRL_TRACKBAR
extern BOOL RegisterTrackBarControl (void);
#endif
#ifdef _MGCTRL_COMBOBOX
extern BOOL RegisterComboBoxControl (void);
#endif
#ifdef _MGCTRL_PROPSHEET
extern BOOL RegisterPropSheetControl (void);
#endif
#ifdef _MGCTRL_SCROLLVIEW
extern BOOL RegisterScrollViewControl (void);
extern BOOL RegisterScrollWndControl (void);
#endif
#ifdef _MGCTRL_TEXTEDIT
extern BOOL RegisterTextEditControl (void);
#endif

#ifdef _MGCTRL_MONTHCAL
extern BOOL RegisterMonthCalendarControl (void);
#endif

#ifdef _MGCTRL_TREEVIEW
extern BOOL RegisterTreeViewControl (void);
#endif

#ifdef _MGCTRL_TREEVIEW_RDR
extern BOOL RegisterTreeViewRdrControl (void);
#endif

#ifdef _MGCTRL_SPINBOX
extern BOOL RegisterSpinControl (void);
#endif

#ifdef _MGCTRL_COOLBAR
extern BOOL RegisterCoolBarControl (void);
#endif

#ifdef _MGCTRL_LISTVIEW
extern BOOL RegisterListViewControl (void);
#endif

#ifdef _MGCTRL_GRIDVIEW
extern BOOL RegisterGridViewControl (void);
#endif

#ifdef _MGCTRL_ICONVIEW
extern BOOL RegisterIconViewControl (void);
#endif

#ifdef _MGCTRL_ANIMATION
extern BOOL RegisterAnimationControl (void);
#endif

#ifdef _MGCTRL_SCROLLBAR
extern BOOL RegisterScrollBarControl (void);
#endif

#define LEN_CCITABLE    26

static PCTRLCLASSINFO ccitable[LEN_CCITABLE];

PCONTROL gui_Control (HWND hWnd)
{
    PCONTROL pCtrl;

    pCtrl = (PCONTROL) hWnd;

    if (pCtrl && pCtrl->WinType == TYPE_CONTROL)
        return pCtrl;

    return NULL;
}

BOOL mg_InitControlClass ()
{
    int i;
    
    for (i=0; i<LEN_CCITABLE; i++)
        ccitable[i] = NULL;

    // Register system controls here.
#ifdef _MGCTRL_STATIC
    if (!RegisterStaticControl ())
        return FALSE;
#endif

#ifdef _MGCTRL_BUTTON
    if (!RegisterButtonControl())
        return FALSE;
#endif

#ifdef _MGCTRL_SLEDIT
    if (!RegisterSLEditControl())
        return FALSE;
#endif

#ifdef _MGCTRL_BIDISLEDIT
    if (!RegisterBIDISLEditControl())
        return FALSE;
#endif

#ifdef _MGCTRL_PROGRESSBAR
    if (!RegisterProgressBarControl())
        return FALSE;
#endif

#ifdef _MGCTRL_LISTBOX
    if (!RegisterListboxControl())
        return FALSE;
#endif

#ifdef _MGCTRL_NEWTOOLBAR
    if (!RegisterNewToolbarControl())
        return FALSE;
#endif

#ifdef _MGCTRL_MENUBUTTON
    if (!RegisterMenuButtonControl())
        return FALSE;
#endif

#ifdef _MGCTRL_TRACKBAR
    if (!RegisterTrackBarControl())
        return FALSE;
#endif

#ifdef _MGCTRL_COMBOBOX
    if (!RegisterComboBoxControl())
        return FALSE;
#endif

#ifdef _MGCTRL_PROPSHEET
    if (!RegisterPropSheetControl())
        return FALSE;
#endif

#ifdef _MGCTRL_SCROLLVIEW
    if (!RegisterScrollViewControl ())
        return FALSE;
    if (!RegisterScrollWndControl ())
        return FALSE;
#endif

#ifdef _MGCTRL_TEXTEDIT
    if (!RegisterTextEditControl ())
        return FALSE;
#endif

#ifdef _MGCTRL_TREEVIEW
    if (!RegisterTreeViewControl ()) {
        _MG_PRINTF ("Init MiniGUI control error: TreeView.\n");
        return FALSE;
    }
#endif

#ifdef _MGCTRL_TREEVIEW_RDR
    if (!RegisterTreeViewRdrControl ()) {
        _MG_PRINTF ("Init MiniGUI control error: TreeView_rdr.\n");
        return FALSE;
    }
#endif

#ifdef _MGCTRL_MONTHCAL
    if (!RegisterMonthCalendarControl ()) {
        _MG_PRINTF ("Init MiniGUI control error: MonthCalendar.\n");
        return FALSE;
    }
#endif

#ifdef _MGCTRL_SPINBOX
    if (!RegisterSpinControl ()) {
        _MG_PRINTF ("Init MiniGUI control error: Spin.\n");
        return FALSE;
    }
#endif

#ifdef _MGCTRL_COOLBAR
    if (!RegisterCoolBarControl ()) {
        _MG_PRINTF ("Init MiniGUI control error: CoolBar.\n");
        return FALSE;
    }
#endif

#ifdef _MGCTRL_LISTVIEW
    if (!RegisterListViewControl ()) {
        _MG_PRINTF ("Init MiniGUI control error: ListView.\n");
        return FALSE;
    }
#endif

#ifdef _MGCTRL_GRIDVIEW
    if (!RegisterGridViewControl ()) {
        _MG_PRINTF ("Init MiniGUI control error: GridView.\n");
        return FALSE;
    }
#endif

#ifdef _MGCTRL_ICONVIEW
    if (!RegisterIconViewControl ()) {
        _MG_PRINTF ("Init MiniGUI control error: IconView.\n");
        return FALSE;
    }
#endif

#ifdef _MGCTRL_ANIMATION
    if (!RegisterAnimationControl ()) {
        _MG_PRINTF ("Init MiniGUI control error: Animation.\n");
        return FALSE;
    }
#endif

#ifdef _MGCTRL_SCROLLBAR
    if (!RegisterScrollBarControl ()) {
        _MG_PRINTF ("Init MiniGUI control error: ScrollBar.\n");
        return FALSE;
    }
#endif

    return TRUE;
}

void mg_TerminateControlClass ()
{
    gui_EmptyControlClassInfoTable ();
}

PCTRLCLASSINFO gui_GetControlClassInfo (const char* szClassName)
{
    PCTRLCLASSINFO cci;
    int i=0;
    char szName [MAXLEN_CLASSNAME + 1];

    if (szClassName == NULL) return NULL;

    strncpy (szName, szClassName, MAXLEN_CLASSNAME);
    szName[MAXLEN_CLASSNAME] = '\0';

    if (!isalpha ((int)szName[0])) return NULL;
    
    while (szName[i]) {
        szName[i] = toupper(szName[i]);

        i++;
    }
    
    cci = ccitable [szName[0] - 'A'];

    while (cci) {
    
        if (strcmp (cci->name, szName) == 0)
            break;

        cci = cci->next;
    }
    
    return cci;
}

int gui_ControlClassDataOp (int Operation, PWNDCLASS pWndClass)
{
    PCTRLCLASSINFO cci;

    cci = gui_GetControlClassInfo (pWndClass->spClassName);

    if (!cci)
        return ERR_CTRLCLASS_INVNAME;

    if (Operation ==  CCDOP_GETCCI) {
        if (pWndClass->opMask & COP_STYLE) {
            pWndClass->dwStyle      = cci->dwStyle;
            pWndClass->dwExStyle    = cci->dwExStyle;
        }
        if (pWndClass->opMask & COP_HCURSOR)
            pWndClass->hCursor      = cci->hCursor;
        if (pWndClass->opMask & COP_BKCOLOR)
            pWndClass->iBkColor     = cci->iBkColor;
        if (pWndClass->opMask & COP_WINPROC)
            pWndClass->WinProc      = cci->ControlProc;
        if (pWndClass->opMask & COP_ADDDATA)
            pWndClass->dwAddData    = cci->dwAddData;
    }
    else {
        if (pWndClass->opMask & COP_STYLE) {
            cci->dwStyle            = pWndClass->dwStyle;
            cci->dwExStyle          = pWndClass->dwExStyle;
        }
        if (pWndClass->opMask & COP_HCURSOR)
            cci->hCursor            = pWndClass->hCursor;
        if (pWndClass->opMask & COP_BKCOLOR)
            cci->iBkColor           = pWndClass->iBkColor;
        if (pWndClass->opMask & COP_WINPROC)
            cci->ControlProc        = pWndClass->WinProc;
        if (pWndClass->opMask & COP_ADDDATA)
            cci->dwAddData          = pWndClass->dwAddData;
    }

    return ERR_OK;
}

int GetCtrlClassAddData (const char* szClassName, DWORD* pAddData)
{
    PCTRLCLASSINFO cci;

    cci = gui_GetControlClassInfo (szClassName);

    if (cci) {
        *pAddData = cci->dwAddData;
        return ERR_OK;
    }
    
    return ERR_CTRLCLASS_INVNAME;
}

int SetCtrlClassAddData (const char* szClassName, DWORD dwAddData)
{
    PCTRLCLASSINFO cci;

    cci = gui_GetControlClassInfo (szClassName);

    if (cci) {
        cci->dwAddData = dwAddData;
        return ERR_OK;
    }
    
    return ERR_CTRLCLASS_INVNAME;
}

int AddNewControlClass (PWNDCLASS pWndClass)
{
    PCTRLCLASSINFO cci, newcci;
    char szClassName [MAXLEN_CLASSNAME + 2];
    int i=0;

    strncpy (szClassName, pWndClass->spClassName, MAXLEN_CLASSNAME);

    if (!isalpha ((int)szClassName[0])) return ERR_CTRLCLASS_INVNAME;
    
    while (szClassName[i]) {
        szClassName[i] = toupper(szClassName[i]);

        i++;
        if (i > MAXLEN_CLASSNAME)
            return ERR_CTRLCLASS_INVLEN;
    }
    
    i = szClassName[0] - 'A';
    cci = ccitable [i];
    if (cci) {

        while (cci) {
            if (strcmp (szClassName, cci->name) == 0)
                return ERR_CTRLCLASS_INVNAME;

            cci = cci->next;
        }
    }
    cci = ccitable[i];

    newcci = malloc (sizeof (CTRLCLASSINFO));
    
    if (newcci == NULL) return ERR_CTRLCLASS_MEM;

    newcci->next = NULL;
    strcpy (newcci->name, szClassName);
    newcci->dwStyle     = pWndClass->dwStyle;
    newcci->dwExStyle   = pWndClass->dwExStyle;
    newcci->hCursor     = pWndClass->hCursor;
    newcci->iBkColor    = pWndClass->iBkColor;
    newcci->ControlProc = pWndClass->WinProc;
    newcci->dwAddData   = pWndClass->dwAddData;
    newcci->nUseCount   = 0;

    if (cci) {
        while (cci->next)
            cci = cci->next;

        cci->next = newcci;
    }
    else
        ccitable [i] = newcci;
    
    return ERR_OK;
}

int gui_DeleteControlClass (const char* szClassName)
{
    PCTRLCLASSINFO head, cci, prev;
    int i=0;
    char szName [MAXLEN_CLASSNAME + 1];

    if (szClassName == NULL) return ERR_CTRLCLASS_INVNAME;
    
    strncpy (szName, szClassName, MAXLEN_CLASSNAME);

    if (!isalpha ((int)szName[0])) return ERR_CTRLCLASS_INVNAME;
    
    while (szName[i]) {
        szName[i] = toupper(szName[i]);

        i++;
    }
    
    i = szName[0] - 'A';
    head = ccitable [i];
    
    cci = head;
    prev = head;
    while (cci) {
    
        if (strcmp (cci->name, szName) == 0)
            break;

        prev = cci;
        cci = cci->next;
    }

    if (!cci)
        return ERR_CTRLCLASS_INVNAME;

    if (cci->nUseCount != 0)
        return ERR_CTRLCLASS_INVNAME;

    if (cci == head) {
        ccitable [i] = cci->next;
        free (cci);
    }
    else {
        prev->next = cci->next;
        free (cci);
    }

    return ERR_OK;
}

void gui_EmptyControlClassInfoTable ()
{
    PCTRLCLASSINFO cci, temp;
    int i;

    for (i = 0; i<LEN_CCITABLE; i++) {
        cci = ccitable [i];
        while (cci) {
            temp = cci->next;

            free (cci);

            cci = temp;
        }
    }
}

BOOL SetWindowExStyle (HWND hWnd, DWORD dwExStyle)
{
    PMAINWIN pWin;
    PCONTROL pCtrl;

    pWin = (PMAINWIN)hWnd;

    if (pWin->WinType == TYPE_MAINWIN)
         pWin->dwExStyle=dwExStyle;
    else if (pWin->WinType == TYPE_CONTROL) {
        pCtrl = (PCONTROL)hWnd;
        pCtrl->dwExStyle=dwExStyle;
    }
    else
        return FALSE;

    return TRUE;
}

#ifdef _DEBUG

void mnuDumpCtrlClassInfo (PCTRLCLASSINFO cci)
{
    printf ("\tClass Name:             %s\n", cci->name);
    printf ("\tClass Cursor:           %p\n", cci->hCursor);
    printf ("\tClass Background color: %d\n", cci->iBkColor);
    printf ("\tClass Control Proc:     %p\n", cci->ControlProc);
    printf ("\tClass Use Count:        %d\n", cci->nUseCount);
}

void DumpCtrlClassInfoTable()
{
    PCTRLCLASSINFO cci;
    int i;

    for (i = 0; i<LEN_CCITABLE; i++) {
        cci = ccitable [i];

        printf ("CCI Table Element: %d\n", i);
        while (cci) {
       
            mnuDumpCtrlClassInfo (cci);

            cci = cci->next;
        }
    }
}

#endif

