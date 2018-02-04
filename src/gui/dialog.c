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
** dialog.c: The Dialog module.
**
** Create date: 1999.09.10
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"

#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"

void GUIAPI DestroyAllControls (HWND hDlg)
{
    PMAINWIN pDlg = (PMAINWIN)hDlg;
    PCONTROL pCtrl = (PCONTROL)(pDlg->hFirstChild);
    PCONTROL pNext;

    while (pCtrl) {
        pNext = pCtrl->next;
        DestroyWindow ((HWND)pCtrl);
        pCtrl = pNext;
    }
}

#define dlgDestroyAllControls   DestroyAllControls

HWND GUIAPI CreateMainWindowIndirectParamEx (PDLGTEMPLATE pDlgTemplate,
        HWND hOwner, WNDPROC WndProc, LPARAM lParam,
        const char* werdr_name, WINDOW_ELEMENT_ATTR* we_attrs,
        const char* window_name, const char* layer_name)
{
    MAINWINCREATE CreateInfo;
    HWND hMainWin;
    int i;
    PCTRLDATA pCtrlData;
    HWND hCtrl;
    HWND hFocus;

    if (pDlgTemplate->controlnr > 0 && !pDlgTemplate->controls)
        return HWND_INVALID;

    hOwner = GetMainWindowHandle (hOwner);

    CreateInfo.dwReserved     = (DWORD)pDlgTemplate;

    CreateInfo.dwStyle        = pDlgTemplate->dwStyle & ~WS_VISIBLE;
    CreateInfo.dwExStyle      = pDlgTemplate->dwExStyle;
    CreateInfo.spCaption      = pDlgTemplate->caption;
    CreateInfo.hMenu          = pDlgTemplate->hMenu;
    CreateInfo.hCursor        = GetSystemCursor (IDC_ARROW);
    CreateInfo.hIcon          = pDlgTemplate->hIcon;
    CreateInfo.MainWindowProc = WndProc ? WndProc : DefaultMainWinProc;
    CreateInfo.lx             = pDlgTemplate->x;
    CreateInfo.ty             = pDlgTemplate->y;
    CreateInfo.rx             = pDlgTemplate->x + pDlgTemplate->w;
    CreateInfo.by             = pDlgTemplate->y + pDlgTemplate->h;
    CreateInfo.iBkColor       = 
                    GetWindowElementPixel (HWND_NULL, WE_MAINC_THREED_BODY);
    CreateInfo.dwAddData      = pDlgTemplate->dwAddData;
    CreateInfo.hHosting       = hOwner;
    
        
    hMainWin = CreateMainWindowEx (&CreateInfo, 
            werdr_name, we_attrs, window_name, layer_name);

    if (hMainWin == HWND_INVALID)
        return HWND_INVALID;

    for (i = 0; i < pDlgTemplate->controlnr; i++) {
        pCtrlData = pDlgTemplate->controls + i;
        hCtrl = CreateWindowEx2 (pCtrlData->class_name,
                              pCtrlData->caption,
                              pCtrlData->dwStyle | WS_CHILD,
                              pCtrlData->dwExStyle,
                              pCtrlData->id,
                              pCtrlData->x,
                              pCtrlData->y,
                              pCtrlData->w,
                              pCtrlData->h,
                              hMainWin,
                              pCtrlData->werdr_name,
                              pCtrlData->we_attrs,
                              pCtrlData->dwAddData);
                              
        if (hCtrl == HWND_INVALID) {
            DestroyMainWindow (hMainWin);
            MainWindowThreadCleanup (hMainWin);
            return HWND_INVALID;
        }
    }

#if 0
    hFocus = GetNextDlgTabItem (hMainWin, (HWND)0, FALSE);
#else
    /* houhh 20100706, set the forefront control as focus. */
    {
        PCONTROL pCtrl;
        for (pCtrl = (PCONTROL)(((PMAINWIN)hMainWin)->hFirstChild); 
                pCtrl && pCtrl->next; pCtrl = pCtrl->next);
        hFocus = (HWND)pCtrl;
    }
#endif
    if (SendMessage (hMainWin, MSG_INITDIALOG, (WPARAM)hFocus, lParam)) {
        if (hFocus)
            SetFocus (hFocus);
    }
    if (!(pDlgTemplate->dwExStyle & WS_EX_DLGHIDE))
        ShowWindow (hMainWin, SW_SHOWNORMAL);
    
    return hMainWin;
}

BOOL GUIAPI DestroyMainWindowIndirect (HWND hMainWin)
{
    DestroyMainWindow (hMainWin);
    MainWindowThreadCleanup (hMainWin);

    return TRUE;
}

int GUIAPI DialogBoxIndirectParamEx (PDLGTEMPLATE pDlgTemplate,
        HWND hOwner, WNDPROC DlgProc, LPARAM lParam,
        const char* werdr_name, WINDOW_ELEMENT_ATTR* we_attrs,
        const char* window_name, const char* layer_name)
{
    HWND hDlg;
    int  retCode = IDCANCEL;
    MSG Msg;

	if(hOwner && hOwner != HWND_INVALID && hOwner != HWND_DESKTOP)
		hOwner = GetMainWindowHandle(hOwner);

	hDlg = CreateMainWindowIndirectParamEx(pDlgTemplate,
		hOwner, DlgProc, lParam, 
		werdr_name, we_attrs,
		window_name, layer_name);

    if (hDlg == HWND_INVALID)
        return -1;

    //MiniGUI maybe change dialog owner in CreateMainWindow, so we 
    //should update its owner by GetHosting.
    hOwner = GetHosting (hDlg);
    SetWindowAdditionalData2 (hDlg, (DWORD)(&retCode));

    if (hOwner && hOwner != HWND_DESKTOP) {
        if (IsWindowEnabled (hOwner)) {
            EnableWindow (hOwner, FALSE);
            IncludeWindowExStyle (hOwner, WS_EX_MODALDISABLED);
        }
        /* Throw away MSG_KEYDOWN, MSG_CHAR and MSG_KEYUP to hOwner */
        while (PeekPostMessage (&Msg, hOwner, 
                                MSG_KEYDOWN, MSG_KEYUP, PM_REMOVE));
    }
       
    while (GetMessage (&Msg, hDlg)) {
        TranslateMessage (&Msg);
        DispatchMessage (&Msg);
    }

    MainWindowCleanup (hDlg);

    return retCode;
}

BOOL GUIAPI EndDialog (HWND hDlg, int endCode)
{
    HWND hOwner;
    BOOL isActive;
    DWORD dwAddData2;

    dwAddData2 = GetWindowAdditionalData2 (hDlg);

    if (dwAddData2 == 0)
        return FALSE;

    *((int *)(dwAddData2)) = endCode;

    hOwner = GetHosting (hDlg);
    isActive = (GetActiveWindow() == hDlg);

    DestroyMainWindow (hDlg);

    if (hOwner != HWND_DESKTOP && hOwner != HWND_NULL) {
        if (GetWindowExStyle (hOwner) & WS_EX_MODALDISABLED) {
            EnableWindow (hOwner, TRUE);
            ExcludeWindowExStyle (hOwner, WS_EX_MODALDISABLED);
            if (isActive && IsWindowVisible (hOwner)) {
                ShowWindow (hOwner, SW_SHOWNORMAL);
                SetActiveWindow (hOwner);
            }
        }
    }

    return TRUE;
}

LRESULT GUIAPI PreDefDialogProc (HWND hWnd, UINT message, 
                WPARAM wParam, LPARAM lParam)
{
    HWND hCurFocus;

    switch (message) {
#if 0
    case MSG_CREATE:
    {
        int i;
        PCTRLDATA pCtrlData;
        HWND hCtrl;
            
        PDLGTEMPLATE pDlgTmpl 
                    = (PDLGTEMPLATE)(((PMAINWINCREATE)lParam)->dwReserved);
            
        for (i = 0; i < pDlgTmpl->controlnr; i++) {
            pCtrlData = pDlgTmpl->controls + i;
            if (pCtrlData->class_name) {
                hCtrl = CreateWindowEx2 (pCtrlData->class_name,
                              pCtrlData->caption,
                              pCtrlData->dwStyle | WS_CHILD,
                              pCtrlData->dwExStyle,
                              pCtrlData->id,
                              pCtrlData->x,
                              pCtrlData->y,
                              pCtrlData->w,
                              pCtrlData->h,
                              hWnd,
                              pCtrlData->werdr_name,
                              pCtrlData->we_attrs,
                              pCtrlData->dwAddData);
            }
            else
                break;
                              
            if (hCtrl == HWND_INVALID) {
                dlgDestroyAllControls (hWnd);
                return -1;
            }
        }

        return 0;
    }
#endif
    case MSG_DLG_GETDEFID:
    {
        HWND hDef;

        hDef = GetDlgDefPushButton (hWnd);
        if (hDef)
            return GetDlgCtrlID (hDef);
        return 0;
    }
    
    case MSG_DLG_SETDEFID:
    {
        HWND hOldDef;
        HWND hNewDef;

        hNewDef = GetDlgItem (hWnd, wParam);
        if (SendMessage (hNewDef, MSG_GETDLGCODE, 0, 0L) & DLGC_PUSHBUTTON) {
            hOldDef = GetDlgDefPushButton (hWnd);
            if (hOldDef) {
                ExcludeWindowStyle (hOldDef, BS_DEFPUSHBUTTON);
                InvalidateRect (hOldDef, NULL, TRUE);
            }
            IncludeWindowStyle (hNewDef, BS_DEFPUSHBUTTON);
            InvalidateRect (hNewDef, NULL, TRUE);

            return (LRESULT)hOldDef;
        }
        break;
    }
        
    case MSG_COMMAND:
        if (wParam == IDCANCEL) {
            HWND hCancel;
            
            hCancel = GetDlgItem (hWnd, IDCANCEL);
            if (hCancel && IsWindowEnabled (hCancel) 
                    && IsWindowVisible (hCancel))
                EndDialog (hWnd, IDCANCEL);
        }
        break;

    case MSG_CLOSE:
    {
        HWND hCancel;

        hCancel = GetDlgItem (hWnd, IDCANCEL);
        if (hCancel && IsWindowEnabled (hCancel) 
                    && IsWindowVisible (hCancel))
            EndDialog (hWnd, IDCANCEL);

        return 0;
    }

    case MSG_ISDIALOG:
        return 1;

    case MSG_KEYDOWN:
        if ((hCurFocus = GetFocusChild (hWnd)) 
                && SendMessage (hCurFocus, MSG_GETDLGCODE, 0, 0L) & 
                DLGC_WANTALLKEYS)
            break;

        switch (wParam) {
        case SCANCODE_ESCAPE:
            SendMessage (hWnd, MSG_COMMAND, IDCANCEL, 0L);
            return 0;

        case SCANCODE_TAB:
        {
            HWND hNewFocus;
                
            if (hCurFocus && SendMessage (hCurFocus, MSG_GETDLGCODE, 0, 0L) & 
                            DLGC_WANTTAB)
                break;

            if (lParam & KS_SHIFT)
                hNewFocus = GetNextDlgTabItem (hWnd, hCurFocus, TRUE);
            else
                hNewFocus = GetNextDlgTabItem (hWnd, hCurFocus, FALSE);

            if (hNewFocus != hCurFocus) {
                SetNullFocus (hCurFocus);
                SetFocus (hNewFocus);
#if 0
                SendMessage (hWnd, MSG_DLG_SETDEFID, 
                                GetDlgCtrlID (hNewFocus), 0L);
#endif
            }

            return 0;
        }

        case SCANCODE_KEYPADENTER:
        case SCANCODE_ENTER:
        {
            HWND hDef;

            if (hCurFocus && SendMessage (hCurFocus, MSG_GETDLGCODE, 0, 0L) & 
                            DLGC_WANTENTER)
                break;

            if (SendMessage (hCurFocus, MSG_GETDLGCODE, 0, 0L) & 
                            DLGC_PUSHBUTTON)
                break;

            hDef = GetDlgDefPushButton (hWnd);
            /* DK[07/05/10]Fix Bug4798, Check the control if has WS_DISABLED property. */
            if (hDef && IsWindowEnabled(hDef)) {
                SendMessage (hWnd, MSG_COMMAND, GetDlgCtrlID (hDef), 0L);
                return 0;
            }
        }

        case SCANCODE_CURSORBLOCKDOWN:
        case SCANCODE_CURSORBLOCKRIGHT:
        case SCANCODE_CURSORBLOCKUP:
        case SCANCODE_CURSORBLOCKLEFT:
        {
            HWND hNewFocus;
                
            if (hCurFocus && SendMessage (hCurFocus, MSG_GETDLGCODE, 0, 0L) & 
                            DLGC_WANTARROWS)
                break;

            if (LOWORD (wParam) == SCANCODE_CURSORBLOCKDOWN
                    || LOWORD (wParam) == SCANCODE_CURSORBLOCKRIGHT)
                hNewFocus = GetNextDlgGroupItem (hWnd, hCurFocus, FALSE);
            else
                hNewFocus = GetNextDlgGroupItem (hWnd, hCurFocus, TRUE);
            
            if (hNewFocus != hCurFocus) {

                if (SendMessage (hCurFocus, MSG_GETDLGCODE, 0, 0L) & 
                                DLGC_STATIC)
                    return 0;

                SetFocus (hNewFocus);
#if 0
                SendMessage (hWnd, MSG_DLG_SETDEFID, 
                                GetDlgCtrlID (hNewFocus), 0L);
#endif

                if (SendMessage (hNewFocus, MSG_GETDLGCODE, 0, 0L)
                        & DLGC_RADIOBUTTON) {
                    SendMessage (hNewFocus, BM_CLICK, 0, 0L);
                    ExcludeWindowStyle (hCurFocus, WS_TABSTOP);
                    IncludeWindowStyle (hNewFocus, WS_TABSTOP);
                }
            }

            return 0;
        }
        }
        break;

    default:
        break;
    }
    
    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

LINT GUIAPI GetDlgCtrlID (HWND hwndCtl)
{
    PCONTROL pCtrl = (PCONTROL)hwndCtl;

    if (pCtrl) {
        return pCtrl->id;
    }

    return -1;
}

HWND GUIAPI GetDlgItem (HWND hDlg, LINT nIDDlgItem)
{
    PCONTROL pCtrl;
    PMAINWIN pMainWin = (PMAINWIN)hDlg;

    pCtrl = (PCONTROL)(pMainWin->hFirstChild);

    while (pCtrl) {
        if (pCtrl->id == nIDDlgItem)
            return (HWND)pCtrl;

        pCtrl = pCtrl->next;
    }
   
    return 0;
}

HWND GUIAPI GetDlgDefPushButton (HWND hWnd)
{
    PCONTROL pCtrl;
    PMAINWIN pMainWin = (PMAINWIN)hWnd;

    pCtrl = (PCONTROL)(pMainWin->hFirstChild);

    while (pCtrl) {
        if (SendMessage ((HWND)pCtrl, 
                MSG_GETDLGCODE, 0, 0L) & DLGC_DEFPUSHBUTTON)
            return (HWND)pCtrl;

        pCtrl = pCtrl->next;
    }
   
    return 0;
}

HWND GetNextDlgGroupItem (HWND hDlg, HWND hCtl, BOOL bPrevious)
{
    PCONTROL pStartCtrl, pCtrl;
    PMAINWIN pMainWin = (PMAINWIN)hDlg;

    if (hCtl)
        pStartCtrl = (PCONTROL)hCtl;
    else {
        pStartCtrl = (PCONTROL)(pMainWin->hFirstChild);
        if (!pStartCtrl)
            return hCtl;
        if (bPrevious) {
            while (pStartCtrl->next) {
                pStartCtrl = pStartCtrl->next;
            }
        }
    }

    if (bPrevious) {
        if (pStartCtrl->dwStyle & WS_GROUP) {
            if (pStartCtrl->next) {
                pCtrl = pStartCtrl->next;
                while (pCtrl && pCtrl->next) {
                    if (pCtrl->dwStyle & WS_GROUP)
                        break;
                    pCtrl = pCtrl->next;
                }
            }
            else
                pCtrl = pStartCtrl;
        }
        else
            pCtrl = pStartCtrl;
        
        if (pCtrl->dwStyle & WS_GROUP || pCtrl == pStartCtrl)
            pCtrl = pCtrl->prev; 

        while (pCtrl) {
            if (pCtrl->dwStyle & WS_VISIBLE
                    && !(pCtrl->dwStyle & WS_DISABLED) )
                return (HWND)pCtrl;

            pCtrl = pCtrl->prev;
        }
    }
    else {
        pCtrl = pStartCtrl->next;
        while (pCtrl) {
            if ( !(pCtrl->dwStyle & WS_GROUP) ) {
                if (pCtrl->dwStyle & WS_VISIBLE
                        && !(pCtrl->dwStyle & WS_DISABLED) )
                    return (HWND)pCtrl;
            }
            else
                break;

            pCtrl = pCtrl->next;
        }
        
        pCtrl = pStartCtrl->prev;
        while (pCtrl) {
            if (pCtrl->dwStyle & WS_GROUP) {
                if (pCtrl->dwStyle & WS_VISIBLE
                        && !(pCtrl->dwStyle & WS_DISABLED) )
                    return (HWND)pCtrl;
                else
                    break;
            }

            pCtrl = pCtrl->prev;
        }
        
        if (pCtrl) {        // pCtrl is the first control in group.
            pCtrl = pCtrl->next;
            while (pCtrl) {
                if ( !(pCtrl->dwStyle & WS_GROUP) ) {
                    if (pCtrl->dwStyle & WS_VISIBLE
                            && !(pCtrl->dwStyle & WS_DISABLED) )
                        return (HWND)pCtrl;
                }
                else
                    break;

                pCtrl = pCtrl->next;
            }
        }
    }

    return hCtl;
}

HWND GUIAPI GetNextDlgTabItem (HWND hDlg, HWND hCtl, BOOL bPrevious)
{
    PCONTROL pFirstCtrl, pLastCtrl, pCtrl;
    PMAINWIN pMainWin = (PMAINWIN)hDlg;

    pFirstCtrl = pCtrl = (PCONTROL)(pMainWin->hFirstChild);
    if (!pCtrl) return hCtl;

    while (pCtrl->next) {
        pCtrl = pCtrl->next;
    }
    pLastCtrl = pCtrl;
    
    if (hCtl) {
        pCtrl = (PCONTROL)hCtl;
        if (bPrevious)
            pCtrl = pCtrl->prev;
        else
            pCtrl = pCtrl->next;
    }
    else if (bPrevious)
        pCtrl = pLastCtrl;
    else
        pCtrl = pFirstCtrl;

    if (bPrevious) {
        while (pCtrl) {
            if (pCtrl->dwStyle & WS_TABSTOP 
                    && pCtrl->dwStyle & WS_VISIBLE
                    && !(pCtrl->dwStyle & WS_DISABLED) )
                return (HWND)pCtrl;

            pCtrl = pCtrl->prev;
        }
        
        pCtrl = pLastCtrl;
        while (pCtrl) {
            if (pCtrl->dwStyle & WS_TABSTOP 
                    && pCtrl->dwStyle & WS_VISIBLE
                    && !(pCtrl->dwStyle & WS_DISABLED) )
                return (HWND)pCtrl;

            pCtrl = pCtrl->prev;
        }
    }
    else {
        while (pCtrl) {
            if (pCtrl->dwStyle & WS_TABSTOP 
                    && pCtrl->dwStyle & WS_VISIBLE
                    && !(pCtrl->dwStyle & WS_DISABLED) )
                return (HWND)pCtrl;

            pCtrl = pCtrl->next;
        }

        pCtrl = pFirstCtrl;
        while (pCtrl) {
            if (pCtrl->dwStyle & WS_TABSTOP 
                    && pCtrl->dwStyle & WS_VISIBLE
                    && !(pCtrl->dwStyle & WS_DISABLED) )
                return (HWND)pCtrl;

            pCtrl = pCtrl->next;
        }
    }

    return hCtl;
}

LRESULT GUIAPI SendDlgItemMessage (HWND hDlg, LINT nIDDlgItem, 
            UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hCtrl;

    hCtrl = GetDlgItem (hDlg, nIDDlgItem);

    if (hCtrl)
        return SendMessage (hCtrl, message, wParam, lParam);

    return -1;
}

UINT GUIAPI GetDlgItemInt (HWND hDlg, LINT nIDDlgItem, BOOL *lpTranslated,
                    BOOL bSigned)
{
    HWND hCtrl;
    char buffer [101];
    unsigned long int result;

    if ( !(hCtrl = GetDlgItem (hDlg, nIDDlgItem)))
        goto error;

    if (SendMessage (hCtrl, MSG_GETTEXT, 100, (LPARAM)buffer) == 0)
        goto error;

    if (bSigned)
        result = strtoul (buffer, NULL, 0);
    else
        result = (unsigned long int)strtol (buffer, NULL, 0);
        
    if (lpTranslated)
        *lpTranslated = TRUE;

    return (UINT)result;
    
error:
    if (lpTranslated)
        *lpTranslated = FALSE;
    return 0;
}

BOOL GUIAPI SetDlgItemInt (HWND hDlg, LINT nIDDlgItem, UINT uValue, 
                    BOOL bSigned)
{
    HWND hCtrl;
    char buffer [101];

    if ( !(hCtrl = GetDlgItem (hDlg, nIDDlgItem)))
        return FALSE;

    if (bSigned)
        snprintf (buffer, 100, "%ld", (long)uValue);
    else
        snprintf (buffer, 100, "%u", uValue);
    buffer [100] = '\0';

    return SendMessage (hCtrl, MSG_SETTEXT, 0, (LPARAM)buffer) == 0;
}

int GUIAPI GetDlgItemText (HWND hDlg, LINT nIDDlgItem, char* lpString, 
                    int nMaxCount)
{
    HWND hCtrl;

    if ( !(hCtrl = GetDlgItem (hDlg, nIDDlgItem)))
        return 0;

    return SendMessage (hCtrl, 
            MSG_GETTEXT, (WPARAM)nMaxCount, (LPARAM)lpString);
}

char* GUIAPI GetDlgItemText2 (HWND hDlg, LINT id, int* lenPtr)
{
    int len;
    char* buff;

    len = SendDlgItemMessage (hDlg, id,
                        MSG_GETTEXTLENGTH, 0, 0L);
    if (len > 0) {
        buff = malloc (len + 1);
        if (buff)
           SendDlgItemMessage (hDlg, id,
               MSG_GETTEXT, len, (LPARAM)(buff));
    }
    else
        buff = NULL;

    if (lenPtr) *lenPtr = len;

    return buff;
}

BOOL GUIAPI SetDlgItemText (HWND hDlg, LINT nIDDlgItem, const char* lpString)
{
    HWND hCtrl;

    if ( !(hCtrl = GetDlgItem (hDlg, nIDDlgItem)))
        return FALSE;

    return SendMessage (hCtrl, 
            MSG_SETTEXT, 0, (LPARAM)lpString) == 0;
}

#ifdef _MGCTRL_BUTTON
void GUIAPI CheckDlgButton (HWND hDlg, LINT nIDDlgItem, int nCheck)
{
    HWND hCtrl;
    int DlgCode;

    if ( !(hCtrl = GetDlgItem (hDlg, nIDDlgItem)))
        return;
    
    DlgCode = SendMessage (hCtrl, MSG_GETDLGCODE, 0, 0L);

    if (DlgCode & DLGC_BUTTON)
        SendMessage (hCtrl, BM_SETCHECK, nCheck?BST_CHECKED:BST_UNCHECKED, 0L);
    else if (DlgCode & DLGC_3STATE) {
        SendMessage (hCtrl, BM_SETCHECK, nCheck, 0L);
    }
}

void GUIAPI CheckRadioButton (HWND hDlg, 
                LINT idFirstButton, LINT idLastButton, LINT idCheckButton)
{
    HWND hCtrl;

    if ( !(hCtrl = GetDlgItem (hDlg, idCheckButton)))
        return;
        
    SendMessage (hCtrl, BM_SETCHECK, BST_CHECKED, 0L);
}

int GUIAPI IsDlgButtonChecked (HWND hDlg, LINT idButton)
{
    HWND hCtrl;

    if ( !(hCtrl = GetDlgItem (hDlg, idButton)))
        return -1;
    
    return SendMessage (hCtrl, BM_GETCHECK, 0, 0L);
}

#ifdef _MGCTRL_STATIC
static LRESULT MsgBoxProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_INITDIALOG:
    {
        HWND hFocus = GetDlgDefPushButton (hWnd);
        if (hFocus)
             SetFocus (hFocus);

        SetWindowAdditionalData (hWnd, (DWORD)lParam);
        SetWindowFont (hWnd, 
            (PLOGFONT)GetWindowElementAttr (hWnd, WE_FONT_MESSAGEBOX));

        /* set the messagebox's body text font and color.*/
        hFocus = GetDlgItem(hWnd, IDC_STATIC+100);
        SetWindowFont (hFocus, 
            (PLOGFONT)GetWindowElementAttr (hFocus, WE_FONT_MESSAGEBOX));

        SetWindowElementAttr (hFocus,WE_FGC_WINDOW, 
            GetWindowElementAttr (hFocus, WE_FGC_MESSAGEBOX));
        return 0;
    }

    case MSG_COMMAND:
    {
        switch (wParam) {
        case IDOK:
        case IDCANCEL:
        case IDABORT:
        case IDRETRY:
        case IDIGNORE:
        case IDYES:
        case IDNO:
            if (GetDlgItem (hWnd, wParam))
                EndDialog (hWnd, wParam);
            break;
        }
        break;
    }

    case MSG_CHAR:
    {
        int id = 0;
        
        if (HIBYTE (wParam))
            break;
        switch (LOBYTE (wParam)) {
        case 'Y':
        case 'y':
            id = IDYES;
            break;
        case 'N':
        case 'n':
            id = IDNO;
            break;
        case 'A':
        case 'a':
            id = IDABORT;
            break;
        case 'R':
        case 'r':
            id = IDRETRY;
            break;
        case 'I':
        case 'i':
            id = IDIGNORE;
            break;
        }
        
        if (id != 0 && GetDlgItem (hWnd, id))
            EndDialog (hWnd, id);
        break;
    }

    case MSG_CLOSE:
        if (GetDlgItem (hWnd, IDCANCEL)) {
            EndDialog (hWnd, IDCANCEL);
        }
        else if (GetDlgItem (hWnd, IDIGNORE)) {
            EndDialog (hWnd, IDIGNORE);
        }
        else if (GetDlgItem (hWnd, IDNO)) {
            EndDialog (hWnd, IDNO);
        }
        else if (GetDlgItem (hWnd, IDOK)) {
            EndDialog (hWnd, IDOK);
        }
        break;

    default:
        break;
    }

    return DefaultDialogProc (hWnd, message, wParam, lParam);
}

static void get_box_xy (HWND hParentWnd, DWORD dwStyle, DLGTEMPLATE* MsgBoxData)
{
    RECT rcTemp;

    if (dwStyle & MB_BASEDONPARENT) {
        GetWindowRect (hParentWnd, &rcTemp);
    }
    else {
        rcTemp = g_rcDesktop;
    }

    switch (dwStyle & MB_ALIGNMASK) {
        case MB_ALIGNCENTER:
            MsgBoxData->x = rcTemp.left + (RECTW(rcTemp) - MsgBoxData->w)/2;
            MsgBoxData->y = rcTemp.top + (RECTH(rcTemp) - MsgBoxData->h)/2;
            break;

        case MB_ALIGNTOPLEFT:
            MsgBoxData->x = rcTemp.left;
            MsgBoxData->y = rcTemp.top;
            break;

        case MB_ALIGNBTMLEFT:
            MsgBoxData->x = rcTemp.left;
            MsgBoxData->y = rcTemp.bottom - MsgBoxData->h;
            break;

        case MB_ALIGNTOPRIGHT:
            MsgBoxData->x = rcTemp.right - MsgBoxData->w;
            MsgBoxData->y = rcTemp.top;
            break;

        case MB_ALIGNBTMRIGHT:
            MsgBoxData->x = rcTemp.right - MsgBoxData->w;
            MsgBoxData->y = rcTemp.bottom - MsgBoxData->h;
            break;
    }

    if ((MsgBoxData->x + MsgBoxData->w) > g_rcDesktop.right) {
        MsgBoxData->x = g_rcDesktop.right - MsgBoxData->w;
    }

    if ((MsgBoxData->y + MsgBoxData->h) > g_rcDesktop.bottom) {
        MsgBoxData->y = g_rcDesktop.bottom - MsgBoxData->h;
    }
}

int GUIAPI MessageBox (HWND hParentWnd, const char* pszText, 
                      const char* pszCaption, DWORD dwStyle)
{
    BOOL IsTiny;
    DLGTEMPLATE MsgBoxData = 
    {
        WS_CAPTION | WS_BORDER, 
        WS_EX_NONE, 0, 0, 0, 0, NULL, 0, 0, 0, NULL, 0L
    };
    CTRLDATA     CtrlData [5] = 
    {
        {"button", 
            BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP, 
            0, 0, 0, 0, 0, NULL, 0L},
        {"button", 
            BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE, 
            0, 0, 0, 0, 0, NULL, 0L},
        {"button",
            BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE, 
            0, 0, 0, 0, 0, NULL, 0L}
    };

    int i, nButtons, buttonx, iBorder;
    RECT rcText, rcButtons, rcIcon;
    int width, height;
    int mb_margin, mb_buttonw, mb_buttonh, mb_textw;

    IsTiny = !strcasecmp (GetDefaultWindowElementRenderer()->name, "tiny");
    if (IsTiny) {
        int font_size = GetSysFontHeight (SYSLOGFONT_CONTROL);
        mb_margin  =  2;
        mb_buttonw =  font_size * 3;
        mb_buttonh =  font_size + 6;
        mb_textw   =  120;
    }
    else {
        int font_size = GetSysFontHeight (SYSLOGFONT_CONTROL);
        mb_margin  = font_size;
        mb_buttonw = font_size * 4;
        mb_buttonh = (font_size < 8) ? (font_size + 6) : (font_size * 3 / 2);
        mb_textw   = 300;
    }

    /* use system font height to determine the width and height of button */

    if (pszCaption)
        MsgBoxData.caption  = pszCaption;
    else
        MsgBoxData.caption  = "MiniGUI";

    switch (dwStyle & MB_TYPEMASK) {
        case MB_OK:
            MsgBoxData.controlnr = 1;
            CtrlData [0].caption = GetSysText (IDS_MGST_OK);
            CtrlData [0].id      = IDOK;
            break;
        case MB_OKCANCEL:
            MsgBoxData.controlnr = 2;
            CtrlData [0].caption = GetSysText (IDS_MGST_OK);
            CtrlData [0].id      = IDOK;
            CtrlData [1].caption = (dwStyle & MB_CANCELASBACK) ? 
                GetSysText (IDS_MGST_PREV) : GetSysText (IDS_MGST_CANCEL);
            CtrlData [1].id      = IDCANCEL;
            break;
        case MB_YESNO:
            MsgBoxData.controlnr = 2;
            CtrlData [0].caption = GetSysText (IDS_MGST_YES);
            CtrlData [0].id      = IDYES;
            CtrlData [1].caption = GetSysText (IDS_MGST_NO);
            CtrlData [1].id      = IDNO;
            break;
        case MB_RETRYCANCEL:
            MsgBoxData.controlnr = 2;
            CtrlData [0].caption = GetSysText (IDS_MGST_RETRY);
            CtrlData [0].id      = IDRETRY;
            CtrlData [1].caption = (dwStyle & MB_CANCELASBACK) ? 
                GetSysText (IDS_MGST_PREV) : GetSysText (IDS_MGST_CANCEL);
            CtrlData [1].id      = IDCANCEL;
            break;
        case MB_ABORTRETRYIGNORE:
            MsgBoxData.controlnr = 3;
            CtrlData [0].caption = GetSysText (IDS_MGST_ABORT);
            CtrlData [0].id      = IDABORT;
            CtrlData [1].caption = GetSysText (IDS_MGST_RETRY);
            CtrlData [1].id      = IDRETRY;
            CtrlData [2].caption = GetSysText (IDS_MGST_IGNORE);
            CtrlData [2].id      = IDIGNORE;
            break;
        case MB_YESNOCANCEL:
            MsgBoxData.controlnr = 3;
            CtrlData [0].caption = GetSysText (IDS_MGST_YES);
            CtrlData [0].id      = IDYES;
            CtrlData [1].caption = GetSysText (IDS_MGST_NO);
            CtrlData [1].id      = IDNO;
            CtrlData [2].caption = (dwStyle & MB_CANCELASBACK) ?
                GetSysText (IDS_MGST_PREV) : GetSysText (IDS_MGST_CANCEL);
            CtrlData [2].id      = IDCANCEL;
            break;
    }

    switch (dwStyle & MB_DEFMASK) {
        case MB_DEFBUTTON1:
            CtrlData [0].dwStyle |= BS_DEFPUSHBUTTON;
            break;
        case MB_DEFBUTTON2:
            if (MsgBoxData.controlnr > 1)
                CtrlData [1].dwStyle |= BS_DEFPUSHBUTTON;
            break;
        case MB_DEFBUTTON3:
            if (MsgBoxData.controlnr > 2)
                CtrlData [2].dwStyle |= BS_DEFPUSHBUTTON;
            break;
    }

    nButtons = MsgBoxData.controlnr;
    rcButtons.left   = 0;
    rcButtons.top    = 0;
    rcButtons.bottom = mb_buttonh;
    rcButtons.right  = MsgBoxData.controlnr * mb_buttonw + 
        (MsgBoxData.controlnr - 1) * (mb_margin << 1);

    rcIcon.left   = 0;
    rcIcon.top    = 0;
    rcIcon.right  = 0;
    rcIcon.bottom = 0;
    if (dwStyle & MB_ICONMASK) {
        int id_icon = -1;
        i = MsgBoxData.controlnr;
        CtrlData [i].class_name= "static";
        CtrlData [i].dwStyle   = WS_VISIBLE | SS_ICON | WS_GROUP;
        CtrlData [i].x         = mb_margin;
        CtrlData [i].y         = mb_margin;

        if (IsTiny) {
            CtrlData [i].w     = 16;
            CtrlData [i].h     = 16;
        }
        else {
            CtrlData [i].w     = 32;
            CtrlData [i].h     = 32;
        }

        CtrlData [i].id        = IDC_STATIC;
        CtrlData [i].caption   = "";

        switch (dwStyle & MB_ICONMASK) {
            case MB_ICONSTOP:
                id_icon = IDI_STOP;
                break;
            case MB_ICONINFORMATION:
                id_icon = IDI_INFORMATION;
                break;
            case MB_ICONEXCLAMATION:
                id_icon = IDI_EXCLAMATION;
                break;
            case MB_ICONQUESTION:
                id_icon = IDI_QUESTION;
                break;
        }

        if (IsTiny) {
            if (id_icon != -1) {
                CtrlData [i].dwAddData = (DWORD)GetSmallSystemIcon (id_icon);
            }
            rcIcon.right  = 16;
            rcIcon.bottom = 16;
        }
        else {
            if (id_icon != -1) {
                CtrlData [i].dwAddData = (DWORD)GetLargeSystemIcon (id_icon);
                MsgBoxData.hIcon       = GetSmallSystemIcon (id_icon);
            }
            rcIcon.right  = 32;
            rcIcon.bottom = 32;
        }

        MsgBoxData.controlnr ++;
    }

    rcText.left = 0;
    rcText.top  = 0;
    rcText.right = rcButtons.right + (mb_margin << 1);
    rcText.right = MAX (rcText.right, mb_textw);
    rcText.bottom = GetSysCharHeight ();

    SelectFont (HDC_SCREEN, __mg_def_renderer->we_fonts[WE_MESSAGEBOX]);
    DrawText (HDC_SCREEN, pszText, -1, &rcText, 
            DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EXPANDTABS | DT_CALCRECT);

    if (IsTiny)
        rcText.right = MAX (rcText.right, mb_textw);

    i = MsgBoxData.controlnr;
    CtrlData [i].class_name= "static";
    CtrlData [i].dwStyle   = WS_VISIBLE | SS_LEFT | WS_GROUP;
    CtrlData [i].dwExStyle = 0;
    CtrlData [i].x         = RECTW (rcIcon) + (mb_margin << 1);
    CtrlData [i].y         = mb_margin;
    CtrlData [i].w         = RECTW (rcText);
    CtrlData [i].h         = RECTH (rcText);
    CtrlData [i].id        = IDC_STATIC+100;
    CtrlData [i].caption   = pszText;
    CtrlData [i].dwAddData = 0;
    MsgBoxData.controlnr ++;

    iBorder = 1 + 2*GetWindowElementAttr (hParentWnd, WE_METRICS_WND_BORDER);

    width = MAX (RECTW (rcText), RECTW (rcButtons)) + RECTW (rcIcon)
        + (mb_margin << 2)
        + (iBorder << 1);
    height = MAX (RECTH (rcText), RECTH (rcIcon)) + RECTH (rcButtons)
        + mb_margin + (mb_margin << 1) 
        + (iBorder << 1) 
        + GetWindowElementAttr (hParentWnd, WE_METRICS_CAPTION);

    buttonx = (width - RECTW (rcButtons)) >> 1;
    for (i = 0; i < nButtons; i++) {
        CtrlData[i].x = buttonx + i*(mb_buttonw + mb_margin);
        CtrlData[i].y = MAX (RECTH (rcIcon), RECTH (rcText)) + (mb_margin<<1);
        CtrlData[i].w = mb_buttonw;
        CtrlData[i].h = mb_buttonh;
    }

    MsgBoxData.w = width;
    MsgBoxData.h = height;
    get_box_xy (hParentWnd, dwStyle, &MsgBoxData);

    MsgBoxData.controls = CtrlData;

    return DialogBoxIndirectParam (&MsgBoxData, hParentWnd, MsgBoxProc, 
            (LPARAM)dwStyle);
}

#endif /* _MGCTRL_STATIC */
#endif /* _MGCTRL_BUTTON */

void GUIAPI MessageBeep (DWORD dwBeep)
{
    Beep ();
}

