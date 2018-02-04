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
** cursor.c: the Cursor Support module for MiniGUI-Threads.
**
** Current maintainer: Wei Yongming.
**
** Create date: 1999.01.06
*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "gal.h"
#include "dc.h"
#include "memops.h"
#include "inline.h"
#include "gal.h"
#include "cursor.h"
#include "ial.h"
#include "readbmp.h"
#include "misc.h"
#include "cursor.h"
#include "sysres.h"

#ifdef _MGRM_THREADS

/* mutex ensuring exclusive access to mouse. */
pthread_mutex_t __mg_mouselock;

static int __mg_cursor_x, __mg_cursor_y;
static int oldx = -1, oldy;

static RECT cliprc = { 0, 0, 0, 0};

#ifdef _MGHAVE_CURSOR

#if defined(__THREADX__) && defined(__TARGET_VFANVIL__)
#ifndef fgetc 
#define fgetc my_fgetc
#endif 
#endif

static PCURSOR SysCursor [MAX_SYSCURSORINDEX + 1];
static HCURSOR def_cursor;
static BYTE* savedbits = NULL;
static BYTE* cursorbits = NULL;
static unsigned int csrimgsize;
static unsigned int csrimgpitch;
 
static int oldboxleft = -100, oldboxtop = -100;
static int nShowCount = 0;
static PCURSOR pCurCsr = NULL;

Uint8* GetPixelUnderCursor (int x, int y, gal_pixel* pixel)
{
    Uint8* dst = NULL;
    
    pthread_mutex_lock (&__mg_mouselock);
    if (nShowCount >= 0 && pCurCsr
            && x >= oldboxleft && y >= oldboxtop
            && (x < oldboxleft + CURSORWIDTH)
            && (y < oldboxtop + CURSORHEIGHT)) {
        int _x = x - oldboxleft;
        int _y = y - oldboxtop;

        dst = savedbits + _y * csrimgpitch
                + _x * __gal_screen->format->BytesPerPixel;
        *pixel = _mem_get_pixel (dst, __gal_screen->format->BytesPerPixel);
    }
    pthread_mutex_unlock (&__mg_mouselock);

    return dst;
}

/* Cursor creating and destroying. */
#include "cursor-comm.c"

HCURSOR GUIAPI LoadCursorFromFile (const char* filename)
{
    return load_cursor_from_file (filename);
}

HCURSOR GUIAPI LoadCursorFromMem (const void* area)
{
    return load_cursor_from_mem (area);
}

static BITMAP csr_bmp = {
    BMP_TYPE_NORMAL, 0, 0, 0, 0, CURSORWIDTH, CURSORHEIGHT
};

/* Only called from InitCursor and client code. */
HCURSOR GUIAPI CreateCursor(int xhotspot, int yhotspot, int w, int h,
                     const BYTE* pANDBits, const BYTE* pXORBits, int colornum)
{
    PCURSOR pcsr;
    
    if( w != CURSORWIDTH || h != CURSORHEIGHT ) return 0;

    /* allocate memory. */
    if( !(pcsr = (PCURSOR)malloc(sizeof(CURSOR))) ) return 0;
    if( !(pcsr->AndBits = malloc(csrimgsize)) ) {
        free(pcsr);
        return 0;
    }
    if( !(pcsr->XorBits = malloc(csrimgsize)) ) {
        free(pcsr->AndBits);
        free(pcsr);
        return 0;
    }

    pcsr->xhotspot = xhotspot;
    pcsr->yhotspot = yhotspot;
    pcsr->width = w;
    pcsr->height = h;

    if (colornum == 1) {
        ExpandMonoBitmap (HDC_SCREEN_SYS, pcsr->AndBits, csrimgpitch, 
                        pANDBits, MONOPITCH, w, h, MYBMP_FLOW_UP,
                        RGBA2Pixel (HDC_SCREEN_SYS, 0, 0, 0, 0xFF), 
                        RGBA2Pixel (HDC_SCREEN_SYS, 0xFF, 0xFF, 0xFF, 0xFF));
        ExpandMonoBitmap (HDC_SCREEN_SYS, pcsr->XorBits, csrimgpitch, 
                        pXORBits, MONOPITCH,
                        w, h, MYBMP_FLOW_UP, 
                        RGBA2Pixel (HDC_SCREEN_SYS, 0, 0, 0, 0x00), 
                        RGBA2Pixel (HDC_SCREEN_SYS, 0xFF, 0xFF, 0xFF, 0x00));
    }
    else if (colornum == 4) {
        ExpandMonoBitmap (HDC_SCREEN_SYS, pcsr->AndBits, csrimgpitch, 
                        pANDBits, MONOPITCH,
                        w, h, MYBMP_FLOW_UP, 
                        RGBA2Pixel (HDC_SCREEN_SYS, 0, 0, 0, 0xFF), 
                        RGBA2Pixel (HDC_SCREEN_SYS, 0xFF, 0xFF, 0xFF, 0xFF));
        Expand16CBitmapEx (HDC_SCREEN_SYS, pcsr->XorBits, csrimgpitch, pXORBits, 
                        MONOPITCH*4, w, h, MYBMP_FLOW_UP, NULL, FALSE, 0x00);
    }

    return (HCURSOR)pcsr;
}

HCURSOR GUIAPI CopyCursor (HCURSOR hcsr)
{
    int i;
    PCURSOR pcsr = (PCURSOR)hcsr;
    PCURSOR pdcsr;

    if (pcsr == NULL)
        return 0;

    for(i = 0; i <= MAX_SYSCURSORINDEX; i++)
    {
        if(pcsr == SysCursor[i])
            return (HCURSOR) pcsr;
    }

    //copy a new cursor
    if (!(pdcsr = (PCURSOR)malloc (sizeof (CURSOR)))) return 0;
    if (!(pdcsr->AndBits = malloc (csrimgsize))) {
        free(pdcsr);
        return 0;
    }
    if (!(pdcsr->XorBits = malloc (csrimgsize))) {
        free (pdcsr->AndBits);
        free (pdcsr);
        return 0;
    }

    pdcsr->xhotspot = pcsr->xhotspot;
    pdcsr->yhotspot = pcsr->yhotspot;
    pdcsr->width = pcsr->width;
    pdcsr->height = pcsr->height;
    memcpy(pdcsr->XorBits, pcsr->XorBits, csrimgsize);
    memcpy(pdcsr->AndBits, pcsr->AndBits, csrimgsize);

    return (HCURSOR) pdcsr;
}

/* Only called from client code. */
BOOL GUIAPI DestroyCursor(HCURSOR hcsr)
{
    int i;
    PCURSOR pcsr = (PCURSOR)hcsr;

    if (pcsr == NULL)
        return TRUE;

    for(i = 0; i <= MAX_SYSCURSORINDEX; i++)
    {
        if(pcsr == SysCursor[i])
            return FALSE;
    }

    if (pcsr == pCurCsr)
        SetCursor(def_cursor);

    free(pcsr->AndBits);
    free(pcsr->XorBits);
    free(pcsr);
    return TRUE;
}

/* Only called from client code, and accessed items are not changable ones. */
HCURSOR GUIAPI GetSystemCursor(int csrid)
{
    if(csrid > MAX_SYSCURSORINDEX || csrid < 0)
        return 0; 

    return (HCURSOR)(SysCursor[csrid]);
}

HCURSOR GUIAPI GetDefaultCursor (void)
{
    return def_cursor;
}

#define CURSORSECTION   "cursorinfo"

BOOL realInitCursor(void)
{
    char szValue[MAX_NAME + 1];
    int number;
    int i;

    csrimgsize = GAL_GetBoxSize (__gal_screen, CURSORWIDTH, CURSORHEIGHT, 
                    &csrimgpitch);

    csr_bmp.bmType = BMP_TYPE_NORMAL;
    csr_bmp.bmWidth = CURSORWIDTH;
    csr_bmp.bmHeight = CURSORHEIGHT;
    
    csr_bmp.bmBitsPerPixel = __gal_screen->format->BitsPerPixel;
    csr_bmp.bmBytesPerPixel = __gal_screen->format->BytesPerPixel;
    csr_bmp.bmPitch = csrimgpitch;

    if( !(savedbits = malloc(csrimgsize)) )
        return FALSE;

    if( !(cursorbits = malloc(csrimgsize)) )
    {
        free(savedbits);
        savedbits = NULL;
        return FALSE;
    }

    if( GetMgEtcValue (CURSORSECTION, "cursornumber", szValue, 10) < 0 )
        goto error;
    
    number = atoi(szValue);

    if(number <= 0)
        return TRUE;

    number = number < (MAX_SYSCURSORINDEX + 1) ? 
             number : (MAX_SYSCURSORINDEX + 1);

    for(i = 0; i < number; i++) {
        if ( !(SysCursor[i] = sysres_load_system_cursor(i)) )
             goto error;
    }
	

    return TRUE;
error:
    mg_TerminateCursor();
    return FALSE;
}

/* The following function must be called at last.  */
void mg_TerminateCursor( void )
{
    int i;

    if (!savedbits ) return;

    pthread_mutex_destroy (&__mg_mouselock);

    free(savedbits);
    free(cursorbits);
    savedbits = NULL;
    pCurCsr = NULL;
    nShowCount = 0;
 
    for(i = 0; i<= MAX_SYSCURSORINDEX; i++)
    {
        if( SysCursor[i] ) {
            free(SysCursor[i]->AndBits);
            free(SysCursor[i]->XorBits);
            free(SysCursor[i]);
            SysCursor[i] = NULL;
       }
    }
}

HCURSOR GUIAPI GetCurrentCursor(void)
{
    HCURSOR hcsr;

    pthread_mutex_lock (&__mg_mouselock);
    
    hcsr = (HCURSOR)pCurCsr;
    pthread_mutex_unlock(&__mg_mouselock);

    return hcsr;
}

/* Cursor pointer shape and hiding and showing. */
static inline int boxleft(void)
{
    if(!pCurCsr) return -100;
    return __mg_cursor_x - pCurCsr->xhotspot;
}
static inline int boxtop(void)
{
    if(!pCurCsr) return -100;
    return __mg_cursor_y - pCurCsr->yhotspot;
}

static GAL_Rect csr_rect = {0, 0, CURSORWIDTH, CURSORHEIGHT};

static void hidecursor (void)
{
    csr_rect.x = oldboxleft;
    csr_rect.y = oldboxtop;

    csr_bmp.bmBits = savedbits;

    GAL_SetClipRect (__gal_screen, NULL);
    GAL_PutBox (__gal_screen, &csr_rect, &csr_bmp); 
    GAL_UpdateRects (__gal_screen, 1, &csr_rect);
}

void _dc_restore_alpha_in_bitmap (const GAL_PixelFormat* format,
                void* dst_bits, unsigned int nr_bytes);

static void showcursor (void)
{
    int x, y;

    x = boxleft ();
    y = boxtop ();

    csr_rect.x = x;
    csr_rect.y = y;
    csr_bmp.bmBits = savedbits;

    GAL_SetClipRect (__gal_screen, NULL);
    GAL_GetBox (__gal_screen, &csr_rect, &csr_bmp);

    oldboxleft = x;
    oldboxtop = y;

    GAL_memcpy4 (cursorbits, savedbits, csrimgsize >> 2);

#ifdef ASM_memandcpy4
    ASM_memandcpy4 (cursorbits, pCurCsr->AndBits, csrimgsize >> 2);
    ASM_memxorcpy4 (cursorbits, pCurCsr->XorBits, csrimgsize >> 2);
#else
    {
        int i;
        Uint32* andbits = (Uint32*) pCurCsr->AndBits;
        Uint32* xorbits = (Uint32*) pCurCsr->XorBits;
        Uint32* dst = (Uint32*) cursorbits;

        for (i = 0; i < csrimgsize >> 2; i++) {
            dst [i] &= andbits [i];
            dst [i] ^= xorbits [i];
        }
    }
#endif

    csr_bmp.bmBits = cursorbits;
    GAL_PutBox (__gal_screen, &csr_rect, &csr_bmp);
    GAL_UpdateRects (__gal_screen, 1, &csr_rect);
}

HCURSOR GUIAPI SetCursorEx (HCURSOR hcsr, BOOL setdef)
{
    PCURSOR old, pcsr;

    pthread_mutex_lock (&__mg_mouselock);

    if (setdef) {
        old = (PCURSOR) def_cursor;
        def_cursor = hcsr;
    }
    else
        old = pCurCsr;

    if ((PCURSOR)hcsr == pCurCsr) {
        pthread_mutex_unlock(&__mg_mouselock);
        return (HCURSOR) old;
    }
    pthread_mutex_unlock(&__mg_mouselock);

    pthread_mutex_lock (&__mg_gdilock);
    pthread_mutex_lock (&__mg_mouselock);

    pcsr = (PCURSOR)hcsr;

    if (nShowCount >= 0 && pCurCsr)
        hidecursor();

    pCurCsr = pcsr;

    if (nShowCount >= 0 && pCurCsr)
        showcursor();

    pthread_mutex_unlock(&__mg_mouselock);
    pthread_mutex_unlock(&__mg_gdilock);
    return (HCURSOR) old;
}

void kernel_ShowCursorForGDI (BOOL fShow, void *pdc)
{
    int csrleft, csrright, csrtop, csrbottom;
    int intleft, intright, inttop, intbottom;
    PDC cur_pdc = (PDC)pdc;
    const RECT* prc = NULL;

    prc = &cur_pdc->rc_output;

    if (cur_pdc->surface != __gal_screen) {
        if (fShow) {
            GAL_UpdateRect (cur_pdc->surface, 
                            prc->left, prc->top, RECTWP(prc), RECTHP(prc));
        }
    }
    else {
        if (!fShow)
            pthread_mutex_lock (&__mg_mouselock);

        csrleft = boxleft();
        csrright = csrleft + CURSORWIDTH;
        csrtop = boxtop();
        csrbottom = csrtop + CURSORHEIGHT;

        intleft = (csrleft > prc->left) ? csrleft : prc->left;
        inttop  = (csrtop > prc->top) ? csrtop : prc->top;
        intright = (csrright < prc->right) ? csrright : prc->right;
        intbottom = (csrbottom < prc->bottom) ? csrbottom : prc->bottom;

        if (intleft >= intright || inttop >= intbottom) {
            if (fShow) {
                GAL_UpdateRect (cur_pdc->surface, 
                                prc->left, prc->top, RECTWP(prc), RECTHP(prc));
                pthread_mutex_unlock(&__mg_mouselock);
            }
            return;
        }

        if (fShow && nShowCount >= 0 && pCurCsr) {
            showcursor();
        }
        if (!fShow && nShowCount >= 0 && pCurCsr) {
            hidecursor();
        }

        if (fShow) {
            GAL_UpdateRect (cur_pdc->surface, 
                            prc->left, prc->top, RECTWP(prc), RECTHP(prc));
            pthread_mutex_unlock(&__mg_mouselock);
        }
    }
}

int GUIAPI ShowCursor(BOOL fShow)
{
    int count;

    pthread_mutex_lock (&__mg_gdilock);
    pthread_mutex_lock (&__mg_mouselock);

    if(fShow) {
        nShowCount++;
        if (nShowCount == 0 && pCurCsr)
            showcursor();
    }
    else {
        nShowCount--;
        if (nShowCount == -1 && pCurCsr)
            hidecursor();
    }

    count = nShowCount;

    pthread_mutex_unlock(&__mg_mouselock);
    pthread_mutex_unlock(&__mg_gdilock);
    return count;
}

#else

void kernel_ShowCursorForGDI (BOOL fShow, void* pdc)
{
    PDC cur_pdc = (PDC)pdc;
    const RECT* prc = NULL;

    prc = &cur_pdc->rc_output;

    if (fShow)
        GAL_UpdateRect (cur_pdc->surface, 
                        prc->left, prc->top, RECTWP(prc), RECTHP(prc));
}

#endif /* _MGHAVE_CURSOR */


BOOL mg_InitCursor (void)
{

    __mg_cursor_x=0, __mg_cursor_y=0;
    oldx = -1, oldy=-1;
    memset(&cliprc,0,sizeof(RECT));
#ifdef _MGHAVE_CURSOR
    oldboxleft = -100, oldboxtop = -100;
    nShowCount = 0;
    pCurCsr = NULL;

    csr_rect.x = csr_rect.y = 0;
    csr_rect.w = CURSORWIDTH;
    csr_rect.h = CURSORHEIGHT;
#endif
    pthread_mutex_init (&__mg_mouselock, NULL);

#ifdef _MGHAVE_CURSOR
    return realInitCursor ();
#else
    return TRUE;
#endif
}

BOOL kernel_RefreshCursor(int* x, int* y, int* button)
{
    pthread_mutex_lock (&__mg_gdilock);
    pthread_mutex_lock (&__mg_mouselock);

    IAL_GetMouseXY (x, y);
    __mg_cursor_x = *x;
    __mg_cursor_y = *y;
    *button = IAL_GetMouseButton ();
    if (oldx != __mg_cursor_x || oldy != __mg_cursor_y) {
#ifdef _MGHAVE_CURSOR
        if(nShowCount >= 0 && pCurCsr) {
            hidecursor();
            showcursor();
        }
#endif
        oldx = __mg_cursor_x;
        oldy = __mg_cursor_y;

        pthread_mutex_unlock(&__mg_mouselock);
        pthread_mutex_unlock(&__mg_gdilock);
        return TRUE;
    }

    pthread_mutex_unlock(&__mg_mouselock);
    pthread_mutex_unlock(&__mg_gdilock);
    return FALSE;
}

/* Cursor position. */

void GUIAPI GetCursorPos(POINT* ppt)
{
    pthread_mutex_lock (&__mg_mouselock);
    ppt->x = __mg_cursor_x;
    ppt->y = __mg_cursor_y;
    pthread_mutex_unlock(&__mg_mouselock);
}

void GUIAPI SetCursorPos(int x, int y)
{
    pthread_mutex_lock (&__mg_gdilock);
    pthread_mutex_lock (&__mg_mouselock);

    IAL_SetMouseXY (x, y);
    IAL_GetMouseXY (&__mg_cursor_x, &__mg_cursor_y);

    if (oldx != __mg_cursor_x || oldy != __mg_cursor_y) {
#ifdef _MGHAVE_CURSOR
        if(nShowCount >= 0 && pCurCsr) {
            hidecursor();
            showcursor();
        }
#endif
        oldx = __mg_cursor_x;
        oldy = __mg_cursor_y;
    }

    pthread_mutex_unlock(&__mg_mouselock);
    pthread_mutex_unlock(&__mg_gdilock);
}

/* Cursor clipping support. */
void GUIAPI ClipCursor(const RECT* prc)
{
    RECT rc;

    pthread_mutex_lock (&__mg_mouselock);

    SetRect(&cliprc, 0, 0, WIDTHOFPHYGC - 1, HEIGHTOFPHYGC - 1);

    if(prc == NULL)
    {
        IAL_SetMouseRange (0,0,WIDTHOFPHYGC - 1,HEIGHTOFPHYGC - 1);
        pthread_mutex_unlock(&__mg_mouselock);
        return;
    }
        
    memcpy(&rc, prc, sizeof(RECT));
    NormalizeRect(&rc);
    IntersectRect(&cliprc, &rc, &cliprc);
    NormalizeRect(&cliprc);

    IAL_SetMouseRange (cliprc.left,cliprc.top, cliprc.right,cliprc.bottom);

    pthread_mutex_unlock(&__mg_mouselock);
}

void GUIAPI GetClipCursor(RECT* prc)
{
    pthread_mutex_lock (&__mg_mouselock);

    if( IsRectEmpty(&cliprc) )
        SetRect(&cliprc, 0, 0, WIDTHOFPHYGC - 1, HEIGHTOFPHYGC - 1);

    memcpy(prc, &cliprc, sizeof(RECT));
    pthread_mutex_unlock(&__mg_mouselock);
}

#endif /* _MGRM_THREADS */

