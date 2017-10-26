/*
** $Id: cursor-sa.c 13674 2010-12-06 06:45:01Z wanzheng $
**
** cursor-sa.c: Cursor support module for MiniGUI-Standalone.
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2003/08/15
*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common.h"

#ifdef _MGRM_STANDALONE

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "inline.h"
#include "memops.h"
#include "gal.h"
#include "dc.h"
#include "cursor.h"
#include "ial.h"
#include "ourhdr.h"
#include "readbmp.h"
#include "misc.h"
#include "sysres.h"

static int curx, cury;
static int oldx = -1, oldy;
static RECT cliprc = {0, 0, 0, 0};

#ifdef _MGHAVE_CURSOR

static PCURSOR SysCursor [MAX_SYSCURSORINDEX + 1];
static HCURSOR def_cursor;
static BYTE* savedbits = NULL;
static BYTE* cursorbits = NULL;
static int csrimgsize;
static unsigned int csrimgpitch;
 
static int oldboxleft = -100, oldboxtop = -100;
static int nShowCount = 0;
static PCURSOR pCurCsr = NULL;

/* Cursor creating and destroying. */
HCURSOR GUIAPI LoadCursorFromFile(const char* filename)
{
    FILE* fp;
    WORD wTemp;
    int  w, h, xhot, yhot, colornum;
    DWORD size, offset;
    DWORD imagesize, imagew, imageh;
    BYTE* image;
    HCURSOR csr = 0;
    
    if( !(fp = fopen(filename, "rb")) ) return 0;

    fseek(fp, sizeof(WORD), SEEK_SET);

    /* the cbType of struct CURSORDIR. */
    wTemp = MGUI_ReadLE16FP (fp);
    if(wTemp != 2) goto error;

    /* skip the cdCount of struct CURSORDIR, we always use the first cursor. */
    fseek(fp, sizeof(WORD), SEEK_CUR);
    
    /* cursor info, read the members of struct CURSORDIRENTRY. */
    w = fgetc (fp);  /* the width of first cursor. */
    h = fgetc (fp);  /* the height of first cursor. */
    if(w != CURSORWIDTH || h != CURSORHEIGHT) goto error;
    fseek(fp, sizeof(BYTE)*2, SEEK_CUR); /* skip bColorCount and bReserved. */
    wTemp = MGUI_ReadLE16FP (fp);
    xhot = wTemp;
    wTemp = MGUI_ReadLE16FP (fp);
    yhot = wTemp;
    size = MGUI_ReadLE32FP (fp);
    offset = MGUI_ReadLE32FP (fp);

    /* read the cursor image info. */
    fseek(fp, offset, SEEK_SET);
    fseek(fp, sizeof(DWORD), SEEK_CUR); /* skip the biSize member. */
    imagew = MGUI_ReadLE32FP (fp);
    imageh = MGUI_ReadLE32FP (fp);
    /* check the biPlanes member; */
    wTemp = MGUI_ReadLE16FP (fp);
    if(wTemp != 1) goto error;
    /* check the biBitCount member; */
    wTemp = MGUI_ReadLE16FP (fp);
    if(wTemp > 4) goto error;
    colornum = (int)wTemp;
    fseek(fp, sizeof(DWORD), SEEK_CUR); /* skip the biCompression members. */
    imagesize = MGUI_ReadLE32FP (fp);

    /* skip the rest members and the color table. */
    fseek(fp, sizeof(DWORD)*4 + sizeof(BYTE)*(4<<colornum), SEEK_CUR);
    
    /* allocate memory for image. */
    if ((image = (BYTE*)ALLOCATE_LOCAL (imagesize)) == NULL)
        goto error;

    /* read image */
    fread (image, imagesize, 1, fp);
    
    csr = CreateCursor(xhot, yhot, w, h, 
                        image + (imagesize - MONOSIZE), image, colornum);

    DEALLOCATE_LOCAL (image);

error:
    fclose (fp);
    return csr;
}

HCURSOR GUIAPI LoadCursorFromMem (const void* area)
{
    const Uint8* p = (Uint8*)area;
    WORD wTemp;

    int  w, h, xhot, yhot, colornum;
    DWORD size, offset;
    DWORD imagesize, imagew, imageh;
    
    p += sizeof (WORD);
    wTemp = MGUI_ReadLE16Mem (&p);
    if(wTemp != 2) goto error;

    /* skip the cdCount of struct CURSORDIR, we always use the first cursor. */
    p += sizeof (WORD);
    
    /* cursor info, read the members of struct CURSORDIRENTRY. */
    w = *p++;  /* the width of first cursor. */
    h = *p++;  /* the height of first cursor. */
    if (w != CURSORWIDTH || h != CURSORHEIGHT)
        goto error;

    /* skip the bColorCount and bReserved. */
    p += sizeof(BYTE)*2;
    xhot = MGUI_ReadLE16Mem (&p);
    yhot = MGUI_ReadLE16Mem (&p);
    size = MGUI_ReadLE32Mem (&p);
    offset = MGUI_ReadLE32Mem (&p);

    /* read the cursor image info. */
    p = (Uint8*)area + offset;

    /* skip the biSize member. */
    p += sizeof (DWORD);    
    imagew = MGUI_ReadLE32Mem (&p);
    imageh = MGUI_ReadLE32Mem (&p);

    /* check the biPlanes member; */
    wTemp = MGUI_ReadLE16Mem (&p);
    if (wTemp != 1) goto error;

    /* check the biBitCount member; */
    wTemp = MGUI_ReadLE16Mem (&p);
    if (wTemp > 4) goto error;
    colornum = wTemp;

    /* skip the biCompression members. */
    p += sizeof (DWORD);    
    imagesize = MGUI_ReadLE32Mem (&p);

    /* skip the rest members and the color table. */
    p += sizeof(DWORD)*4 + sizeof(BYTE)*(4<<colornum);
    
    return CreateCursor (xhot, yhot, w, h, 
                        p + (imagesize - MONOSIZE), p, colornum);

error:
    return 0;
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
                        pXORBits, MONOPITCH, w, h, MYBMP_FLOW_UP, 
                        RGBA2Pixel (HDC_SCREEN_SYS, 0, 0, 0, 0x00), 
                        RGBA2Pixel (HDC_SCREEN_SYS, 0xFF, 0xFF, 0xFF, 0x00));
    }
    else if (colornum == 4) {
        ExpandMonoBitmap (HDC_SCREEN_SYS, pcsr->AndBits, csrimgpitch, 
                        pANDBits, MONOPITCH, w, h, MYBMP_FLOW_UP, 
                        RGBA2Pixel (HDC_SCREEN_SYS, 0, 0, 0, 0xFF), 
                        RGBA2Pixel (HDC_SCREEN_SYS, 0xFF, 0xFF, 0xFF, 0xFF));
        Expand16CBitmapEx (HDC_SCREEN_SYS, pcsr->XorBits, csrimgpitch, 
                        pXORBits, MONOPITCH*4, w, h, MYBMP_FLOW_UP, NULL, FALSE, 0x00);
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
            return pcsr;
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

    return pdcsr;
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

BOOL mg_InitCursor(void)
{
    char szValue[MAX_NAME + 1];
    int number;
    int i;

    csrimgsize = GAL_GetBoxSize (__gal_screen, CURSORWIDTH, CURSORHEIGHT, &csrimgpitch);

    csr_bmp.bmBitsPerPixel = __gal_screen->format->BitsPerPixel;
    csr_bmp.bmBytesPerPixel = __gal_screen->format->BytesPerPixel;
    csr_bmp.bmPitch = csrimgpitch;

    if( !(savedbits = malloc(csrimgsize)) )
        return FALSE;

    if( !(cursorbits = malloc(csrimgsize)) ) {
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

    free(savedbits);
    free(cursorbits);
    savedbits = NULL;
    pCurCsr = NULL;
    nShowCount = 0;
 
    for(i = 0; i<= MAX_SYSCURSORINDEX; i++) {
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
    return (HCURSOR)pCurCsr;
}

/* Cursor pointer shape and hiding and showing. */
static inline int boxleft(void)
{
    if(!pCurCsr) return -100;
    return curx - pCurCsr->xhotspot;
}
static inline int boxtop(void)
{
    if(!pCurCsr) return -100;
    return cury - pCurCsr->yhotspot;
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
            dst [i] ^= xorbits[i];
        }
    }
#endif

    csr_bmp.bmBits = cursorbits;
    GAL_PutBox (__gal_screen, &csr_rect, &csr_bmp);
    GAL_UpdateRects (__gal_screen, 1, &csr_rect);
}

#endif /* _MGHAVE_CURSOR */

/* The return value indicates whether mouse has moved.  */
/* TRUE for moved. */
/* NOTE that only server can call this function. */
BOOL kernel_RefreshCursor (int* x, int* y, int* button)
{
    BOOL moved = FALSE;

    IAL_GetMouseXY (x, y);
    curx = *x;
    cury = *y;
    if (button)
        *button = IAL_GetMouseButton ();

    if (oldx != curx || oldy != cury) {

#ifdef _MGHAVE_CURSOR
        if (nShowCount >= 0 && pCurCsr) {
            hidecursor ();
            showcursor ();
        }
#endif /* _MGHAVE_CURSOR */

        oldx = curx;
        oldy = cury;
        moved = TRUE;
    }

    return moved;
}

#ifdef _MGHAVE_CURSOR

/* Always call with "setdef = FALSE" for clients at server side. */
HCURSOR GUIAPI SetCursorEx (HCURSOR hcsr, BOOL setdef)
{
    PCURSOR old, pcsr;

    if (setdef) {
        old = (PCURSOR) def_cursor;
        def_cursor = hcsr;
    }
    else
        old = pCurCsr;

    if ((PCURSOR)hcsr == pCurCsr) {
        return (HCURSOR) old;
    }

    pcsr = (PCURSOR)hcsr;

    if (pCurCsr && nShowCount >= 0)
        hidecursor();

    pCurCsr = pcsr;

    if (pCurCsr && nShowCount >= 0)
        showcursor();

    return (HCURSOR) old;
}

void kernel_ShowCursorForGDI (BOOL fShow, void* pdc)
{
    int csrleft, csrright, csrtop, csrbottom;
    int intleft, intright, inttop, intbottom;
    PDC cur_pdc = (PDC)pdc;
    const RECT* prc = NULL;

    prc = &cur_pdc->rc_output;

    if (cur_pdc->surface != __gal_screen) {
        if (fShow)
            GAL_UpdateRect (cur_pdc->surface, 
                            prc->left, prc->top, RECTWP(prc), RECTHP(prc));
    }
    else {
        csrleft = boxleft();
        csrright = csrleft + CURSORWIDTH;
        csrtop = boxtop();
        csrbottom = csrtop + CURSORHEIGHT;

        intleft = (csrleft > prc->left) ? csrleft : prc->left;
        inttop  = (csrtop > prc->top) ? csrtop : prc->top;
        intright = (csrright < prc->right) ? csrright : prc->right;
        intbottom = (csrbottom < prc->bottom) ? csrbottom : prc->bottom;

        if (intleft >= intright || inttop >= intbottom) {
            if (fShow)
                GAL_UpdateRect (cur_pdc->surface, 
                                prc->left, prc->top, RECTWP(prc), RECTHP(prc));
            return;
        }

        if (fShow && nShowCount >= 0 && pCurCsr) {
            showcursor();
        }
        if (!fShow && nShowCount >= 0 && pCurCsr) {
            hidecursor();
        }

        if (fShow)
            GAL_UpdateRect (cur_pdc->surface, prc->left, prc->top, RECTWP(prc), RECTHP(prc));
    }
}

int GUIAPI ShowCursor (BOOL fShow)
{
    if (fShow) {
        nShowCount++;
        if (nShowCount == 0 && pCurCsr)
           showcursor();
    }
    else {
        nShowCount--;
        if (nShowCount == -1 && pCurCsr)
           hidecursor();
    }

    return nShowCount;
}

#else

void kernel_ShowCursorForGDI(BOOL fShow, void* pdc)
{
    PDC cur_pdc = (PDC)pdc;
    const RECT* prc = NULL;

    prc = &cur_pdc->rc_output;

    if (fShow)
        GAL_UpdateRect (cur_pdc->surface, 
                        prc->left, prc->top, RECTWP(prc), RECTHP(prc));
}

#endif /* _MGHAVE_CURSOR */

void GUIAPI GetCursorPos (POINT* ppt)
{
    ppt->x = curx;
    ppt->y = cury;
}

void GUIAPI SetCursorPos (int x, int y)
{
    IAL_SetMouseXY (x, y);
    kernel_RefreshCursor (&x, &y, NULL);
}

/* Cursor clipping support. */
void GUIAPI ClipCursor (const RECT* prc)
{
    RECT rc;

    SetRect (&cliprc, 0, 0, WIDTHOFPHYGC - 1, HEIGHTOFPHYGC - 1);

    if (prc == NULL) {
        IAL_SetMouseRange (0,0,WIDTHOFPHYGC - 1, HEIGHTOFPHYGC - 1);
        return;
    }

    memcpy (&rc, prc, sizeof(RECT));
    NormalizeRect (&rc);
    IntersectRect (&cliprc, &rc, &cliprc);
    NormalizeRect (&cliprc);

    IAL_SetMouseRange (cliprc.left, cliprc.top, cliprc.right, cliprc.bottom);
}

void GUIAPI GetClipCursor (RECT* prc)
{
    if (IsRectEmpty (&cliprc))
        SetRect (&cliprc, 0, 0, WIDTHOFPHYGC - 1, HEIGHTOFPHYGC - 1);

    memcpy (prc, &cliprc, sizeof(RECT));
}

#endif /* _MGRM_STANDALONE */

