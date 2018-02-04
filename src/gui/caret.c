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
** caret.c: The Caret module.
**
** Create date: 1999.07.03
*/

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"

BOOL GUIAPI CreateCaret (HWND hWnd, PBITMAP pBitmap, int nWidth, int nHeight)
{
    PMAINWIN pWin;

    pWin = (PMAINWIN)hWnd;

    if (!pWin->pCaretInfo) {
        if (!(pWin->pCaretInfo = malloc (sizeof (CARETINFO))))
            return FALSE;
        
        pWin->pCaretInfo->pBitmap = pBitmap;
        if (pBitmap) {
            nWidth  = pBitmap->bmWidth;
            nHeight = pBitmap->bmHeight;
        }
        pWin->pCaretInfo->nWidth  = nWidth;
        pWin->pCaretInfo->nHeight = nHeight;
        pWin->pCaretInfo->caret_bmp.bmType = BMP_TYPE_NORMAL;
        pWin->pCaretInfo->caret_bmp.bmBitsPerPixel = BITSPERPHYPIXEL;
        pWin->pCaretInfo->caret_bmp.bmBytesPerPixel = BYTESPERPHYPIXEL;
        pWin->pCaretInfo->caret_bmp.bmWidth = nWidth;
        pWin->pCaretInfo->caret_bmp.bmHeight = nHeight;
        pWin->pCaretInfo->caret_bmp.bmAlphaMask = NULL;
        pWin->pCaretInfo->caret_bmp.bmAlphaPitch = 0;

        pWin->pCaretInfo->nBytesNr = GAL_GetBoxSize (__gal_screen, 
                        nWidth, nHeight, &pWin->pCaretInfo->caret_bmp.bmPitch);
        pWin->pCaretInfo->pNormal = malloc (pWin->pCaretInfo->nBytesNr);
        pWin->pCaretInfo->pXored  = malloc (pWin->pCaretInfo->nBytesNr);

        if (pWin->pCaretInfo->pNormal == NULL ||
            pWin->pCaretInfo->pXored == NULL) {
            free (pWin->pCaretInfo);
            pWin->pCaretInfo = NULL;
            return FALSE;
        }

        pWin->pCaretInfo->x = pWin->pCaretInfo->y = 0;
        
        pWin->pCaretInfo->fBlink  = FALSE;
        pWin->pCaretInfo->fShow   = FALSE;
        
        pWin->pCaretInfo->hOwner  = hWnd;
        pWin->pCaretInfo->uTime   = 500;
    }

    SendMessage (HWND_DESKTOP, MSG_CARET_CREATE, (WPARAM)hWnd, 0);

    return TRUE;
}

BOOL GUIAPI ActiveCaret (HWND hWnd)
{
    PMAINWIN pWin;

    pWin = (PMAINWIN)hWnd;

    if (!pWin->pCaretInfo)
        return FALSE;

    SendMessage (HWND_DESKTOP, MSG_CARET_CREATE, (WPARAM)hWnd, 0);

    return TRUE;
}

BOOL GUIAPI DestroyCaret (HWND hWnd)
{
    PMAINWIN pWin;

    pWin = (PMAINWIN)hWnd;

    if (!pWin->pCaretInfo)
        return FALSE;

    free (pWin->pCaretInfo->pNormal);
    free (pWin->pCaretInfo->pXored);
    
    free (pWin->pCaretInfo);
    pWin->pCaretInfo = NULL;

    SendMessage (HWND_DESKTOP, MSG_CARET_DESTROY, (WPARAM)hWnd, 0);

    return TRUE;
}

UINT GUIAPI GetCaretBlinkTime (HWND hWnd)
{
    PMAINWIN pWin;

    pWin = (PMAINWIN)hWnd;

    if (!pWin->pCaretInfo)
        return 0;
    
    return pWin->pCaretInfo->uTime;
}

#define MIN_BLINK_TIME      100     // ms

BOOL GUIAPI SetCaretBlinkTime (HWND hWnd, UINT uTime)
{
    PMAINWIN pWin;

    pWin = (PMAINWIN)hWnd;

    if (!pWin->pCaretInfo)
        return FALSE;

    if (uTime < MIN_BLINK_TIME)
        uTime = MIN_BLINK_TIME;

    pWin->pCaretInfo->uTime = uTime;

    SendMessage (HWND_DESKTOP, MSG_CARET_CREATE, (WPARAM)hWnd, 0);

    return TRUE;
}

BOOL GUIAPI HideCaretEx (HWND hWnd, BOOL ime)
{
    PMAINWIN pWin;

    pWin = (PMAINWIN)hWnd;

    if (!pWin->pCaretInfo)
        return FALSE;

    if (!pWin->pCaretInfo->fBlink)
        return FALSE;

    pWin->pCaretInfo->fBlink = FALSE;
    if (pWin->pCaretInfo->fShow) {
        HDC hdc;
        
        pWin->pCaretInfo->fShow = FALSE;

        // hide caret immediately
        hdc = GetClientDC (hWnd);
        pWin->pCaretInfo->caret_bmp.bmBits = pWin->pCaretInfo->pNormal;
        FillBoxWithBitmap (hdc,
                        pWin->pCaretInfo->x, pWin->pCaretInfo->y, 0, 0, 
                        &pWin->pCaretInfo->caret_bmp);
        ReleaseDC (hdc);
    }

    if (ime) {
        gui_open_ime_window (pWin, FALSE, (HWND)hWnd);
    }

    return TRUE;
}

void GetCaretBitmaps (PCARETINFO pCaretInfo)
{
    int i;
    int sx, sy;
        
    // convert to screen coordinates
    sx = pCaretInfo->x;
    sy = pCaretInfo->y;
    ClientToScreen (pCaretInfo->hOwner, &sx, &sy);

    // save normal bitmap first.
    pCaretInfo->caret_bmp.bmBits = pCaretInfo->pNormal;
    GetBitmapFromDC (HDC_SCREEN_SYS, sx, sy, 
                    pCaretInfo->caret_bmp.bmWidth,
                    pCaretInfo->caret_bmp.bmHeight,
                    &pCaretInfo->caret_bmp);
    // generate XOR bitmap.
    if (pCaretInfo->pBitmap) {
        BYTE* normal;
        BYTE* bitmap;
        BYTE* xored;
                
        normal = pCaretInfo->pNormal;
        bitmap = pCaretInfo->pBitmap->bmBits;
        xored  = pCaretInfo->pXored;
            
        for (i = 0; i < pCaretInfo->nBytesNr; i++)
            xored[i] = normal[i] ^ bitmap[i];
    }
    else {
        BYTE* normal;
        BYTE* xored;
        BYTE xor_byte;

        if (BITSPERPHYPIXEL < 8)
            xor_byte = 0x0F;
        else
            xor_byte = 0xFF;
                
        normal = pCaretInfo->pNormal;
        xored  = pCaretInfo->pXored;
            
        for (i = 0; i < pCaretInfo->nBytesNr; i++)
            xored[i] = normal[i] ^ xor_byte;
    }
}

BOOL BlinkCaret (HWND hWnd)
{
    PMAINWIN pWin;
    HDC hdc;

    pWin = (PMAINWIN)hWnd;

    if (!pWin->pCaretInfo)
        return FALSE;

    if (!pWin->pCaretInfo->fBlink)
        return FALSE;

    hdc = GetClientDC (hWnd);

    if (!pWin->pCaretInfo->fShow) {
        // show caret
        pWin->pCaretInfo->caret_bmp.bmBits = pWin->pCaretInfo->pXored;
        FillBoxWithBitmap (hdc,
                        pWin->pCaretInfo->x, pWin->pCaretInfo->y, 0, 0, 
                        &pWin->pCaretInfo->caret_bmp);

        pWin->pCaretInfo->fShow = TRUE;
    }
    else {
        // hide caret
        pWin->pCaretInfo->caret_bmp.bmBits = pWin->pCaretInfo->pNormal;
        FillBoxWithBitmap (hdc,
                        pWin->pCaretInfo->x, pWin->pCaretInfo->y, 0, 0, 
                        &pWin->pCaretInfo->caret_bmp);

        pWin->pCaretInfo->fShow = FALSE;
    }
    
    ReleaseDC (hdc);

    return TRUE;
}

BOOL GUIAPI ShowCaretEx (HWND hWnd, BOOL ime)
{
    PMAINWIN pWin;

    pWin = (PMAINWIN)hWnd;

    if (!pWin->pCaretInfo)
        return FALSE;

    /* XXX: In order to re-open ime window, we need to open ime window before checking fBlink flag
     */ 
    if (ime) {
        gui_open_ime_window (pWin, TRUE, (HWND)hWnd);
    }

    if (pWin->pCaretInfo->fBlink)
        return FALSE;

    pWin->pCaretInfo->fBlink = TRUE;
    GetCaretBitmaps (pWin->pCaretInfo);
    
    if (!pWin->pCaretInfo->fShow) {

        HDC hdc;
        
        // show caret immediately
        hdc = GetClientDC (hWnd);
        pWin->pCaretInfo->caret_bmp.bmBits = pWin->pCaretInfo->pXored;
        FillBoxWithBitmap (hdc,
                        pWin->pCaretInfo->x, pWin->pCaretInfo->y, 0, 0, 
                        &pWin->pCaretInfo->caret_bmp);
        ReleaseDC (hdc);

        pWin->pCaretInfo->fShow = TRUE;
    }

    return TRUE;
}

BOOL GUIAPI SetCaretPos (HWND hWnd, int x, int y)
{
    PMAINWIN pWin;
    IME_TARGET_INFO info;

    pWin = (PMAINWIN)hWnd;
    if (!pWin->pCaretInfo)
        return FALSE;

    info.ptCaret.x = x; 
    if (pWin->pLogFont)
        info.ptCaret.y = y + pWin->pLogFont->size;
    else
        info.ptCaret.y = y + 15;
    GetClientRect (hWnd, &info.rcEditBox);
    info.iEditBoxType = SendAsyncMessage ((HWND)pWin, MSG_DOESNEEDIME, 0, 0L);
    ClientToScreen(hWnd, &info.ptCaret.x, &info.ptCaret.y);
    SetIMETargetInfo(&info);

    if (pWin->pCaretInfo->x == x && pWin->pCaretInfo->y == y) {
        
        return TRUE;
    }

    if (pWin->pCaretInfo->fBlink) {
        if (pWin->pCaretInfo->fShow) {
            HDC hdc;

            // hide caret first
            hdc = GetClientDC (hWnd);
            pWin->pCaretInfo->caret_bmp.bmBits = pWin->pCaretInfo->pNormal;
            FillBoxWithBitmap (hdc,
                        pWin->pCaretInfo->x, pWin->pCaretInfo->y, 0, 0, 
                        &pWin->pCaretInfo->caret_bmp);

            // then update position
            pWin->pCaretInfo->x = x;
            pWin->pCaretInfo->y = y;

            // save normal bitmap first
            GetCaretBitmaps (pWin->pCaretInfo);
            
            // show caret again
            pWin->pCaretInfo->caret_bmp.bmBits = pWin->pCaretInfo->pXored;
            FillBoxWithBitmap (hdc,
                        pWin->pCaretInfo->x, pWin->pCaretInfo->y, 0, 0, 
                        &pWin->pCaretInfo->caret_bmp);

            ReleaseDC (hdc);
        }
        else {
            HDC hdc;
        
            // update position
            pWin->pCaretInfo->x = x;
            pWin->pCaretInfo->y = y;

            // save normal bitmap first
            GetCaretBitmaps (pWin->pCaretInfo);

            // show caret
            hdc = GetClientDC (hWnd);
            pWin->pCaretInfo->caret_bmp.bmBits = pWin->pCaretInfo->pXored;
            FillBoxWithBitmap (hdc,
                        pWin->pCaretInfo->x, pWin->pCaretInfo->y, 0, 0, 
                        &pWin->pCaretInfo->caret_bmp);

            ReleaseDC (hdc);

            pWin->pCaretInfo->fShow = TRUE;
        }
    }
    else {
        // update position
        pWin->pCaretInfo->x = x;
        pWin->pCaretInfo->y = y;
    }

    return TRUE;
}

BOOL GUIAPI ChangeCaretSize (HWND hWnd, int newWidth, int newHeight)
{
    PMAINWIN pWin;

    pWin = (PMAINWIN)hWnd;

    if (!pWin->pCaretInfo)
        return FALSE;
    
    if (newWidth == pWin->pCaretInfo->caret_bmp.bmWidth
            && newHeight == pWin->pCaretInfo->caret_bmp.bmHeight)
        return TRUE;
        
    if (newWidth <= 0 || newHeight <= 0)
        return FALSE;
    
    if (pWin->pCaretInfo->fBlink) {
        if (pWin->pCaretInfo->fShow) {

            HDC hdc;

            // hide caret first
            hdc = GetClientDC (hWnd);
            pWin->pCaretInfo->caret_bmp.bmBits = pWin->pCaretInfo->pNormal;
            FillBoxWithBitmap (hdc,
                        pWin->pCaretInfo->x, pWin->pCaretInfo->y, 0, 0, 
                        &pWin->pCaretInfo->caret_bmp);

            // then update size info
            pWin->pCaretInfo->caret_bmp.bmWidth = newWidth;
            pWin->pCaretInfo->caret_bmp.bmHeight= newHeight;
            pWin->pCaretInfo->nBytesNr = GAL_GetBoxSize (__gal_screen, 
                    newWidth, newHeight, &pWin->pCaretInfo->caret_bmp.bmPitch);

            // when the caret size big then original, re-malloc it
            if (newWidth > pWin->pCaretInfo->nWidth 
                    || newHeight > pWin->pCaretInfo->nHeight) {
                pWin->pCaretInfo->nWidth = newWidth;
                pWin->pCaretInfo->nHeight = newHeight;
                free (pWin->pCaretInfo->pNormal);
                free (pWin->pCaretInfo->pXored);
                pWin->pCaretInfo->pNormal = malloc (pWin->pCaretInfo->nBytesNr);
                pWin->pCaretInfo->pXored  = malloc (pWin->pCaretInfo->nBytesNr);
            }

            // save normal bitmap first
            GetCaretBitmaps (pWin->pCaretInfo);

            // show caret again
            pWin->pCaretInfo->caret_bmp.bmBits = pWin->pCaretInfo->pXored;
            FillBoxWithBitmap (hdc,
                        pWin->pCaretInfo->x, pWin->pCaretInfo->y, 0, 0, 
                        &pWin->pCaretInfo->caret_bmp);

            ReleaseDC (hdc);
        }
        else {

            HDC hdc;

            // then update size info
            pWin->pCaretInfo->caret_bmp.bmWidth = newWidth;
            pWin->pCaretInfo->caret_bmp.bmHeight= newHeight;
            pWin->pCaretInfo->nBytesNr = GAL_GetBoxSize (__gal_screen, 
                    newWidth, newHeight, &pWin->pCaretInfo->caret_bmp.bmPitch);

            // when the caret size big then original, re-malloc it
            if (newWidth > pWin->pCaretInfo->nWidth 
                    || newHeight > pWin->pCaretInfo->nHeight) {
                pWin->pCaretInfo->nWidth = newWidth;
                pWin->pCaretInfo->nHeight = newHeight;
                free (pWin->pCaretInfo->pNormal);
                free (pWin->pCaretInfo->pXored);
                pWin->pCaretInfo->pNormal = malloc (pWin->pCaretInfo->nBytesNr);
                pWin->pCaretInfo->pXored  = malloc (pWin->pCaretInfo->nBytesNr);
            }
            
            // save normal bitmap first
            GetCaretBitmaps (pWin->pCaretInfo);

            // show caret
            hdc = GetClientDC (hWnd);
            pWin->pCaretInfo->caret_bmp.bmBits = pWin->pCaretInfo->pXored;
            FillBoxWithBitmap (hdc,
                        pWin->pCaretInfo->x, pWin->pCaretInfo->y, 0, 0, 
                        &pWin->pCaretInfo->caret_bmp);

            ReleaseDC (hdc);

            pWin->pCaretInfo->fShow = TRUE;
        }
    }
    else {
        // update size info
        pWin->pCaretInfo->caret_bmp.bmWidth = newWidth;
        pWin->pCaretInfo->caret_bmp.bmHeight= newHeight;
        pWin->pCaretInfo->nBytesNr = GAL_GetBoxSize (__gal_screen,
                newWidth, newHeight, &pWin->pCaretInfo->caret_bmp.bmPitch);

        // when the caret size big then original, re-malloc it
        if (newWidth > pWin->pCaretInfo->nWidth
                || newHeight > pWin->pCaretInfo->nHeight) {
            pWin->pCaretInfo->nWidth = newWidth;
            pWin->pCaretInfo->nHeight = newHeight;
            free (pWin->pCaretInfo->pNormal);
            free (pWin->pCaretInfo->pXored);
            pWin->pCaretInfo->pNormal = malloc (pWin->pCaretInfo->nBytesNr);
            pWin->pCaretInfo->pXored  = malloc (pWin->pCaretInfo->nBytesNr);
        }
    }

    return TRUE;
}

BOOL GUIAPI GetCaretPos (HWND hWnd, PPOINT pPt)
{
    PMAINWIN pWin;

    pWin = (PMAINWIN)hWnd;

    if (!pWin->pCaretInfo)
        return FALSE;

    pPt->x = pWin->pCaretInfo->x;
    pPt->y = pWin->pCaretInfo->y;

    return TRUE;
}


