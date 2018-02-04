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
** scrollwnd.c: scrollwnd control, a scrollable container window
**
** Create date: 2004/03/01
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

#ifdef _MGCTRL_SCROLLVIEW
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "ctrl/ctrlhelper.h"
#include "ctrl/scrollview.h"
#include "ctrl/button.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"

#include "ctrlmisc.h"
#include "scrolled.h"
#include "scrollwnd.h"

/* ---------------------------------------------------------------------------- */
/* internal used static function declarations */
static BOOL RegisterContainer (void);
static void container_reset_content (HWND hWnd, PCONTDATA pcontdata);

/* ---------------------------------------------------------------------------- */

/* adjust the position and size of the container window */
static void 
scrollwnd_set_container (HWND hWnd, PSCRDATA pscrdata, BOOL visChanged)
{
    PSWDATA pswdata = (PSWDATA) GetWindowAdditionalData2 (hWnd);

    MoveWindow (pswdata->hContainer, -pscrdata->nContX, -pscrdata->nContY,
                pscrdata->nContWidth,
                pscrdata->nContHeight, TRUE);
}

int scrollwnd_add_controls (HWND hWnd, PSWDATA pswdata, 
                             int ctrl_nr, PCTRLDATA pCtrlData)
{
    int i;
    HWND hCtrl;

    if (!pCtrlData || ctrl_nr <= 0)
        return -1;

    for (i = 0; i < ctrl_nr; i++) {
        hCtrl = CreateWindowEx2 (pCtrlData->class_name,
                                pCtrlData->caption,
                                pCtrlData->dwStyle | WS_CHILD,
                                pCtrlData->dwExStyle,
                                pCtrlData->id,
                                pCtrlData->x, pCtrlData->y,
                                pCtrlData->w, pCtrlData->h,
                                pswdata->hContainer,
                                pCtrlData->werdr_name,
                                pCtrlData->we_attrs,
                                pCtrlData->dwAddData);

        if (hCtrl == HWND_INVALID) {
            return FALSE;
        }

        pCtrlData ++;
    }

    return 0;
}

static void create_container (HWND hWnd, PSWDATA pswdata)
{
    PCONTAINERINFO cont_info = (PCONTAINERINFO) GetWindowAdditionalData(hWnd);

    pswdata->hContainer = CreateWindowEx (CTRL_CONTAINER, "", 
                            WS_VISIBLE, WS_EX_NONE,
                            IDC_STATIC,
                            pswdata->scrdata.nContX, 
                            pswdata->scrdata.nContY,
                            pswdata->scrdata.nContWidth,
                            pswdata->scrdata.nContHeight,
                            hWnd, 0);

    pswdata->pContdata = 
        (PCONTDATA) GetWindowAdditionalData2 (pswdata->hContainer);

    if (cont_info && cont_info->user_proc)
        SetWindowCallbackProc (pswdata->hContainer, cont_info->user_proc);

    if (cont_info && (cont_info->controlnr > 0))
        scrollwnd_add_controls (hWnd, pswdata, 
                cont_info->controlnr, cont_info->controls);

    SendMessage (pswdata->hContainer, 
                MSG_INITCONTAINER, 
                0, cont_info ? cont_info->dwAddData : 0);
}

/*
 * initialize scrollwnd internal structure
 */
static int svInitData (HWND hWnd, PSWDATA pswdata)
{
    scrolled_init (hWnd, &pswdata->scrdata, 0, 0);
    pswdata->scrdata.move_content = scrollwnd_set_container; 
    pswdata->flags = 0;

    create_container (hWnd, pswdata);
    return 0;
}

/* 
 * shoulded be called before scrollwnd is used
 * hWnd: the scrolled window
 */
int scrollwnd_init (HWND hWnd, PSWDATA pswdata)
{
    if (!pswdata)
        return -1;

    SetWindowAdditionalData2 (hWnd, 0);
    ShowScrollBar (hWnd, SB_HORZ, FALSE);
    ShowScrollBar (hWnd, SB_VERT, FALSE);

    svInitData (hWnd, pswdata);
    SetWindowAdditionalData2 (hWnd, (DWORD) pswdata);

    /* set scrollbar status */
    scrolled_set_hscrollinfo (hWnd, &pswdata->scrdata);
    scrolled_set_vscrollinfo (hWnd, &pswdata->scrdata);
                                    
    return 0;
}

/*
 * destroy a scrollwnd
 */
void scrollwnd_destroy (PSWDATA pswdata)
{
    DestroyWindow (pswdata->hContainer);
}

/* --------------------------------------------------------------------------------- */

void scrollwnd_reset_content (HWND hWnd, PSWDATA pswdata)
{
    /* delete all container content */
    container_reset_content (pswdata->hContainer, pswdata->pContdata);

    if (pswdata->scrdata.sbPolicy != SB_POLICY_ALWAYS) {
        ShowScrollBar (hWnd, SB_HORZ, FALSE);
        ShowScrollBar (hWnd, SB_VERT, FALSE);
    }

    /* reset content and viewport size */
    scrolled_init_contsize (hWnd, &pswdata->scrdata);
    /* reset container window */
    //FIXME
    scrollwnd_set_container (hWnd, &pswdata->scrdata, TRUE);
    /* reset viewport window */
    scrolled_set_visible (hWnd, &pswdata->scrdata);

    scrolled_set_hscrollinfo (hWnd, &pswdata->scrdata);
    scrolled_set_vscrollinfo (hWnd, &pswdata->scrdata);

    //InvalidateRect (pswdata->hContainer, NULL, TRUE);
    scrollwnd_refresh_content (pswdata);
}

/* --------------------------------------------------------------------------------- */

static LRESULT ScrollWndCtrlProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PSWDATA pswdata = NULL;

    if (message != MSG_CREATE)
        pswdata = (PSWDATA) GetWindowAdditionalData2 (hWnd);

    switch (message) {

    case MSG_CREATE:
    {
        pswdata = (PSWDATA) malloc(sizeof (SWDATA));
        if (!pswdata)
            return -1;
        scrollwnd_init (hWnd, pswdata);
        break;
    }

    case MSG_DESTROY:
        scrollwnd_destroy (pswdata);
        free (pswdata);
        break;
    
    case MSG_GETDLGCODE:
        return DLGC_WANTTAB | DLGC_WANTARROWS;

    case SVM_ADDCTRLS:
    {
        int ctrl_nr = (int)wParam;
        PCTRLDATA pCtrlData = (PCTRLDATA)lParam;
        scrollwnd_add_controls (hWnd, pswdata, ctrl_nr, pCtrlData);
        return 0;
    }

    case SVM_GETCTRL:
        return (LRESULT)GetDlgItem (pswdata->hContainer, wParam);

    case SVM_RESETCONTENT:
        scrollwnd_reset_content (hWnd, pswdata);
        return 0;

    case SVM_SETCONTAINERPROC:
        scrollwnd_set_container_proc (pswdata, (WNDPROC)lParam);
        return 0;

    case SVM_GETFOCUSCHILD:
        return (LRESULT)GetFocus (pswdata->hContainer);

    }/* end switch */

    return DefaultScrolledProc (hWnd, message, wParam, lParam);
}

BOOL RegisterScrollWndControl (void)
{
    WNDCLASS WndClass;

    if (!RegisterContainer ())
        return FALSE;

    WndClass.spClassName = CTRL_SCROLLWND;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = GetWindowElementPixel (HWND_NULL, WE_BGC_WINDOW);
    WndClass.WinProc     = ScrollWndCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

/* ------------------------------------ container --------------------------- */

static PCONTDATA container_init (HWND hWnd)
{
    PCONTDATA pcontdata;

    if ( !(pcontdata = (PCONTDATA) malloc (sizeof (CONTDATA))) )
        return NULL;

    pcontdata->hSV = GetParent(hWnd);
    pcontdata->flags = 0;

    SetWindowAdditionalData2 (hWnd, (DWORD) pcontdata);

    return pcontdata;
}

static void container_reset_content (HWND hWnd, PCONTDATA pcontdata)
{
    DestroyAllControls (hWnd);
}

static void container_destroy (HWND hWnd, PCONTDATA pcontdata)
{
    container_reset_content (hWnd, pcontdata);
    free (pcontdata);
}

/* ---------------------------------------------------------------------------- */

LRESULT GUIAPI 
DefaultContainerProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PCONTDATA pcontdata = NULL;

    switch (message) {

    case MSG_CREATE:
        if ( !container_init (hWnd) )
            return -1;
        break;

    case MSG_DESTROY:
        pcontdata = (PCONTDATA) GetWindowAdditionalData2 (hWnd);
        container_destroy (hWnd, pcontdata);
        break;

    case MSG_COMMAND:
        pcontdata = (PCONTDATA) GetWindowAdditionalData2 (hWnd);
        SendNotifyMessage (GetParent(pcontdata->hSV), 
                MSG_SVCONTCMD, wParam, lParam);
        break;

    } /* end switch */

    return DefaultPageProc (hWnd, message, wParam, lParam);
}

static BOOL RegisterContainer (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_CONTAINER;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = GetWindowElementPixel (HWND_NULL, WE_BGC_WINDOW);
    WndClass.WinProc     = DefaultContainerProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

#endif /* _MGCTRL_SCROLLVIEW */

