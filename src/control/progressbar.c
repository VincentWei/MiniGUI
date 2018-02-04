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
** progressbar.c: the Progress Bar Control module.
**
** Create date: 1999/8/29
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGCTRL_PROGRESSBAR
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "ctrl/ctrlhelper.h"
#include "ctrl/progressbar.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"

#include "ctrlmisc.h"
#include "progressbar_impl.h"

static void pbarNormalizeParams (const CONTROL* pCtrl, 
                PROGRESSDATA* pData, BOOL fNotify)
{
    if (pData->nPos >= pData->nMax) {
        if (fNotify)
            NotifyParent ((HWND)pCtrl, pCtrl->id, PBN_REACHMAX);
        pData->nPos = pData->nMax;
    }

    if (pData->nPos <= pData->nMin) {
        if (fNotify)
            NotifyParent ((HWND)pCtrl, pCtrl->id, PBN_REACHMIN);
        pData->nPos = pData->nMin;
    }
}

static void 
pbarOnNewPos (const CONTROL* pCtrl, 
        PROGRESSDATA* pData, unsigned int new_pos)
{
    unsigned int old_pos;
    int range = pData->nMax - pData->nMin;

    if (range == 0 || new_pos == pData->nPos)
        return;

    old_pos = pData->nPos;
    pData->nPos = new_pos;
    pbarNormalizeParams (pCtrl, pData, pCtrl->dwStyle & PBS_NOTIFY);
    if (old_pos == pData->nPos)
        return;

    InvalidateRect ((HWND)pCtrl, NULL, FALSE);
}

static LRESULT
ProgressBarCtrlProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC           hdc;
    PCONTROL      pCtrl;
    PROGRESSDATA* pData;
    
    pCtrl = gui_Control (hwnd); 
    
    switch (message)
    {
        case MSG_CREATE:
            if (!(pData = malloc (sizeof (PROGRESSDATA)))) {
                _MG_PRINTF ("CONTROL>ProgressBar: Create control failure!\n");
                return -1;
            }
            
            pData->nMax     = 100;
            pData->nMin     = 0;
            pData->nPos     = 0;
            pData->nStepInc = 10;
            
            pCtrl->dwAddData2 = (DWORD)pData;
        break;
    
        case MSG_DESTROY:
            free ((void *)(pCtrl->dwAddData2));
        break;

        case MSG_NCPAINT:
            return 0;
        
        case MSG_GETDLGCODE:
            return DLGC_STATIC;

        case MSG_GETTEXTLENGTH:
        case MSG_GETTEXT:
        case MSG_SETTEXT:
            return -1;

        case MSG_PAINT:
        {
            PROGRESSDATA *data = (PROGRESSDATA *)pCtrl->dwAddData2;
            hdc = BeginPaint (hwnd);
            GetWindowInfo (hwnd)->we_rdr->draw_progress (hwnd, 
                            hdc, data->nMax, data->nMin, 
                            data->nPos, pCtrl->dwStyle & PBS_VERTICAL);

            EndPaint (hwnd, hdc);
            return 0;
        }

        case PBM_SETRANGE:
            pData = (PROGRESSDATA *)pCtrl->dwAddData2;
            if (wParam == lParam)
                return PB_ERR;

            pData->nMin = MIN (wParam, lParam);
            pData->nMax = MAX (wParam, lParam);
            if (pData->nPos > pData->nMax)
                pData->nPos = pData->nMax;
            if (pData->nPos < pData->nMin)
                pData->nPos = pData->nMin;

            if (pData->nStepInc > (pData->nMax - pData->nMin))
                pData->nStepInc = pData->nMax - pData->nMin;

            InvalidateRect (hwnd, NULL, TRUE);
            return PB_OKAY;
        
        case PBM_SETSTEP:
            pData = (PROGRESSDATA *)pCtrl->dwAddData2;
            if ((int)wParam > (int)(pData->nMax - pData->nMin))
                return PB_ERR;

            pData->nStepInc = wParam;
            return PB_OKAY;
        
        case PBM_SETPOS:
            pData = (PROGRESSDATA *)pCtrl->dwAddData2;
            
            if (pData->nPos == wParam)
                return PB_OKAY;

            pbarOnNewPos (pCtrl, pData, wParam);
            return PB_OKAY;
        
        case PBM_DELTAPOS:
            pData = (PROGRESSDATA *)pCtrl->dwAddData2;

            if (wParam == 0)
                return PB_OKAY;
            
            pbarOnNewPos (pCtrl, pData, pData->nPos + wParam);
            return PB_OKAY;
        
        case PBM_STEPIT:
            pData = (PROGRESSDATA *)pCtrl->dwAddData2;
            
            if (pData->nStepInc == 0)
                return PB_OKAY;

            pbarOnNewPos (pCtrl, pData, pData->nPos + pData->nStepInc);
            return PB_OKAY;
            
        case MSG_FONTCHANGED:
            InvalidateRect (hwnd, NULL, TRUE);
            break;
    }
    
    return DefaultControlProc (hwnd, message, wParam, lParam);
}

BOOL RegisterProgressBarControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_PROGRESSBAR;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (0);
    WndClass.iBkColor    = 
        GetWindowElementPixel (HWND_NULL, WE_MAINC_THREED_BODY);
    WndClass.WinProc     = ProgressBarCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}
#endif /* _MGCTRL_PROGRESSBAR */

