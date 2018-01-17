/*
** $Id: cursor-lite.c 7188 2007-05-16 11:01:59Z weiym $
**
** cursor-lite.c: Cursor support module for MiniGUI-Processes.
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Wei Yongming.
**
** Create date: 1999/01/06
*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common.h"

#ifdef _MGRM_PROCESSES

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
#include "sharedres.h"
#include "ourhdr.h"
#include "client.h"
#include "server.h"
#include "drawsemop.h"
#include "readbmp.h"

unsigned int __mg_csrimgsize;
unsigned int __mg_csrimgpitch;

static int oldx = -1, oldy;
static RECT cliprc = {0, 0, 0, 0};

#ifdef _MGHAVE_CURSOR

#define CSR_SAVEDBITS   ((BYTE*)mgSharedRes + (((PG_RES)mgSharedRes)->svdbitsoffset))
#define CSR_CURSORX     (((PG_RES)mgSharedRes)->cursorx)
#define CSR_CURSORY     (((PG_RES)mgSharedRes)->cursory)
#define CSR_OLDBOXLEFT  (((PG_RES)mgSharedRes)->oldboxleft)
#define CSR_OLDBOXTOP   (((PG_RES)mgSharedRes)->oldboxtop)

#define CSR_CURRENT     ((PCURSOR)((PG_RES)mgSharedRes)->csr_current)
#define CSR_CURRENT_SET (((PG_RES)mgSharedRes)->csr_current)
#define CSR_XHOTSPOT    (((PG_RES)mgSharedRes)->xhotspot)
#define CSR_YHOTSPOT    (((PG_RES)mgSharedRes)->yhotspot)
#define CSR_SHOW_COUNT  (((PG_RES)mgSharedRes)->csr_show_count)

static PCURSOR *sys_cursors;
static HCURSOR def_cursor;
static BYTE* cursorbits = NULL;

#if 0 /* FIXME: I do not know why this can not work :( */
Uint8* GetPixelUnderCursor (int x, int y, gal_pixel* pixel)
{
    Uint8* dst = NULL;
    
    lock_cursor_sem ();
    if (CSR_SHOW_COUNT >= 0 && CSR_CURRENT
            && x >= CSR_OLDBOXLEFT && y >= CSR_OLDBOXTOP
            && (x < CSR_OLDBOXLEFT + CURSORWIDTH)
            && (y < CSR_OLDBOXTOP + CURSORHEIGHT)
            && get_hidecursor_sem_val () == 0) {
        int _x = x - CSR_OLDBOXLEFT;
        int _y = y - CSR_OLDBOXTOP;

        dst = CSR_SAVEDBITS + _y * __mg_csrimgpitch
                + _x * __gal_screen->format->BytesPerPixel;
        *pixel = _mem_get_pixel (dst, __gal_screen->format->BytesPerPixel);
    }
    unlock_cursor_sem ();

    return dst;
}
#endif

/* Cursor creating and destroying. */
static HCURSOR srvCreateCursor (int xhotspot, int yhotspot, int w, int h,
                     const BYTE* pANDBits, const BYTE* pXORBits, int colornum)
{
    PCURSOR pcsr;
    
    if (w != CURSORWIDTH || h != CURSORHEIGHT) return 0;

    if (!(pcsr = (PCURSOR)malloc (sizeof (CURSOR)))) return 0;
    if (!(pcsr->AndBits = malloc (__mg_csrimgsize))) {
        free(pcsr);
        return 0;
    }
    if (!(pcsr->XorBits = malloc (__mg_csrimgsize))) {
        free (pcsr->AndBits);
        free (pcsr);
        return 0;
    }

    pcsr->xhotspot = xhotspot;
    pcsr->yhotspot = yhotspot;
    pcsr->width = w;
    pcsr->height = h;

    if (colornum == 1) {
        ExpandMonoBitmap (HDC_SCREEN_SYS, pcsr->AndBits, __mg_csrimgpitch, 
                        pANDBits, MONOPITCH, w, h, MYBMP_FLOW_UP, 
                        RGBA2Pixel (HDC_SCREEN_SYS, 0, 0, 0, 0xFF), 
                        RGBA2Pixel (HDC_SCREEN_SYS, 0xFF, 0xFF, 0xFF, 0xFF));
        ExpandMonoBitmap (HDC_SCREEN_SYS, pcsr->XorBits, __mg_csrimgpitch, 
                        pXORBits, MONOPITCH, w, h, MYBMP_FLOW_UP, 
                        RGBA2Pixel (HDC_SCREEN_SYS, 0, 0, 0, 0x00), 
                        RGBA2Pixel (HDC_SCREEN_SYS, 0xFF, 0xFF, 0xFF, 0x00));
    }
    else if (colornum == 4) {
        ExpandMonoBitmap (HDC_SCREEN_SYS, pcsr->AndBits, __mg_csrimgpitch, 
                        pANDBits, MONOPITCH, w, h, MYBMP_FLOW_UP, 
                        RGBA2Pixel (HDC_SCREEN_SYS, 0, 0, 0, 0xFF), 
                        RGBA2Pixel (HDC_SCREEN_SYS, 0xFF, 0xFF, 0xFF, 0xFF));
        Expand16CBitmapEx (HDC_SCREEN_SYS, pcsr->XorBits, __mg_csrimgpitch, 
                        pXORBits, MONOPITCH*4, w, h, MYBMP_FLOW_UP, NULL, FALSE, 0x00);
    }

    return (HCURSOR)pcsr;
}

#define _LEN_BITS   (MONOPITCH * CURSORHEIGHT)
static HCURSOR GUIAPI srvCopyCursor (HCURSOR hcsr)
{
    PCURSOR pcsr = (PCURSOR)hcsr;
    PCURSOR pdcsr;
    
    if (!(pdcsr = (PCURSOR)malloc (sizeof (CURSOR)))) return 0;
    if (!(pdcsr->AndBits = malloc (__mg_csrimgsize))) {
        free(pdcsr);
        return 0;
    }
    if (!(pdcsr->XorBits = malloc (__mg_csrimgsize))) {
        free (pdcsr->AndBits);
        free (pdcsr);
        return 0;
    }

    pdcsr->xhotspot = pcsr->xhotspot;
    pdcsr->yhotspot = pcsr->yhotspot;
    pdcsr->width = pcsr->width;
    pdcsr->height = pcsr->height;
    memcpy(pdcsr->XorBits, pcsr->XorBits, __mg_csrimgsize);
    memcpy(pdcsr->AndBits, pcsr->AndBits, __mg_csrimgsize);

    return (HCURSOR) pdcsr;
}

HCURSOR GUIAPI CopyCursor (HCURSOR hcsr)
{
    if (mgIsServer) {
        return srvCopyCursor(hcsr);
    }
    else {
        HCURSOR hcursor;
        REQUEST req;

        req.id = REQID_COPYCURSOR;
        req.len_data = sizeof(HCURSOR);
        req.data = &hcsr;

        if (ClientRequest (&req, &hcursor, sizeof (HCURSOR)) < 0)
            hcursor = 0;

        DEALLOCATE_LOCAL (tmp);
        return hcursor;
    }
}

HCURSOR GUIAPI CreateCursor (int xhotspot, int yhotspot, int w, int h,
                     const BYTE* pANDBits, const BYTE* pXORBits, int colornum)
{
    if (mgIsServer) {
        return srvCreateCursor (xhotspot, yhotspot, w, h, 
                        pANDBits, pXORBits, colornum);
    }
    else {
        HCURSOR hcursor;
        REQUEST req;
        int len_and_bits, len_xor_bits;
        int* tmp;

        if (w != CURSORWIDTH || h != CURSORHEIGHT)
            return 0;

        len_and_bits = _LEN_BITS;
        len_xor_bits = _LEN_BITS * ((colornum == 1) ? 1 : 4);

        req.id = REQID_CREATECURSOR;
        req.len_data = sizeof (int) * 6 + len_and_bits + len_xor_bits;

        if ((tmp = (int*) ALLOCATE_LOCAL (req.len_data)) == NULL)
                return 0;

        tmp [0] = xhotspot; tmp [1] = yhotspot;
        tmp [2] = w;        tmp [3] = h;
        tmp [4] = colornum;
        tmp [5] = _LEN_BITS;
        memcpy (tmp + 6, pANDBits, len_and_bits);
        memcpy ((BYTE*)(tmp + 6) + len_and_bits, pXORBits, len_xor_bits);

        req.data = tmp;

        if (ClientRequest (&req, &hcursor, sizeof (HCURSOR)) < 0)
            hcursor = 0;

        DEALLOCATE_LOCAL (tmp);
        return hcursor;
    }
}

static HCURSOR srvLoadCursorFromFile (const char* filename)
{
    FILE* fp;
    WORD16 wTemp;
    int ret = 0;
    int  w, h, xhot, yhot, colornum;
    DWORD32 size, offset;
    DWORD32 imagesize, imagew, imageh;
    BYTE* image;
    
    if( !(fp = fopen(filename, "rb")) ) return 0;

    fseek(fp, sizeof(WORD16), SEEK_SET);

    /* the cbType of struct CURSORDIR. */
    wTemp = MGUI_ReadLE16FP (fp);
    if(wTemp != 2) goto error;

    /* skip the cdCount of struct CURSORDIR, we always use the first cursor. */
    fseek(fp, sizeof(WORD16), SEEK_CUR);
    
    /* cursor info, read the members of struct CURSORDIRENTRY. */
    w = fgetc (fp);  /* the width of first cursor. */
    h = fgetc (fp);  /* the height of first cursor. */
    if (w != CURSORWIDTH || h != CURSORHEIGHT) goto error;
    fseek(fp, sizeof(BYTE)*2, SEEK_CUR); /* skip bColorCount and bReserved. */
    wTemp = MGUI_ReadLE16FP (fp);
    xhot = wTemp;
    wTemp = MGUI_ReadLE16FP (fp);
    yhot = wTemp;
    size = MGUI_ReadLE32FP (fp);
    offset = MGUI_ReadLE32FP (fp);

    /* read the cursor image info. */
    fseek(fp, offset, SEEK_SET);
    fseek(fp, sizeof(DWORD32), SEEK_CUR); /* skip biSize member. */
    imagew = MGUI_ReadLE32FP (fp);
    imageh = MGUI_ReadLE32FP (fp);
    /* check the biPlanes member; */
    wTemp = MGUI_ReadLE16FP (fp);
    if(wTemp != 1) goto error;
    /* check the biBitCount member; */
    wTemp = MGUI_ReadLE16FP (fp);
    if(wTemp > 4) goto error;
    colornum = (int)wTemp;
    fseek(fp, sizeof(DWORD32), SEEK_CUR); /* skip the biCompression members. */
    imagesize = MGUI_ReadLE32FP (fp);

    /* skip the rest members and the color table. */
    fseek(fp, sizeof(DWORD32)*4 + sizeof(BYTE)*(4<<colornum), SEEK_CUR);
    
    /* allocate memory for image. */
    if ((image = (BYTE*)ALLOCATE_LOCAL (imagesize)) == NULL)
        goto error;

    /* read image */
    fread (image, imagesize, 1, fp);
    
    ret = srvCreateCursor (xhot, yhot, w, h, 
                        image + (imagesize - MONOSIZE), image, colornum);
    DEALLOCATE_LOCAL (image);

error:
    fclose (fp);
    return ret;
}

HCURSOR GUIAPI LoadCursorFromFile (const char* filename)
{
    if (mgIsServer) {
        return srvLoadCursorFromFile (filename);
    }
    else {
        HCURSOR hcursor;
        REQUEST req;

        req.id = REQID_LOADCURSOR;
        req.data = filename;
        req.len_data = strlen (filename) + 1;

        if (ClientRequest (&req, &hcursor, sizeof (HCURSOR)) < 0)
            return 0;

        return hcursor;
    }
}

HCURSOR GUIAPI LoadCursorFromMem (const void* area)
{
    const Uint8* p = (Uint8*)area;
    WORD16 wTemp;

    int  w, h, xhot, yhot, colornum;
    DWORD32 size, offset;
    DWORD32 imagesize, imagew, imageh;
    
    p += sizeof (WORD16);
    wTemp = MGUI_ReadLE16Mem (&p);
    if(wTemp != 2) goto error;

    /* skip the cdCount of struct CURSORDIR, we always use the first cursor. */
    p += sizeof (WORD16);
    
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
    p += sizeof (DWORD32);    
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
    p += sizeof (DWORD32);    
    imagesize = MGUI_ReadLE32Mem (&p);

    /* skip the rest members and the color table. */
    p += sizeof(DWORD32)*4 + sizeof(BYTE)*(4<<colornum);
    
    return CreateCursor (xhot, yhot, w, h, 
                        p + (imagesize - MONOSIZE), p, colornum);

error:
    return 0;
}

static BOOL srvDestroyCursor (HCURSOR hcsr)
{
    int i;
    PCURSOR pcsr = (PCURSOR)hcsr;

    if (pcsr == NULL)
        return TRUE;

    for (i = 0; i < ((PG_RES)mgSharedRes)->csrnum; i++) {
        if (pcsr == sys_cursors [i])
            return FALSE;
    }

    if (pcsr == CSR_CURRENT)
        SetCursor(def_cursor);

    free(pcsr->AndBits);
    free(pcsr->XorBits);
    free(pcsr);
    return TRUE;
}

BOOL GUIAPI DestroyCursor (HCURSOR hcsr)
{
    if (mgIsServer) {
        return srvDestroyCursor (hcsr);
    }
    else {
        REQUEST req;
        BOOL ret_value;

        req.id = REQID_DESTROYCURSOR;
        req.data = &hcsr;
        req.len_data = sizeof (HCURSOR);

        if (ClientRequest (&req, &ret_value, sizeof (BOOL)) < 0)
            return FALSE;

        return ret_value;
    }
}

static void init_system_cursor (void)
{
    int csrid;
    unsigned char* temp;

    temp = mgSharedRes + ((PG_RES)mgSharedRes)->csroffset;
    temp += sizeof (PCURSOR) * ((PG_RES)mgSharedRes)->csrnum;

    for (csrid = 0; csrid < ((PG_RES)mgSharedRes)->csrnum; csrid++) {

        if (!(sys_cursors [csrid] = (PCURSOR) malloc (sizeof(CURSOR))))
            return;

        sys_cursors [csrid]->xhotspot = ((PCURSOR)temp)->xhotspot;
        sys_cursors [csrid]->yhotspot = ((PCURSOR)temp)->yhotspot;
        sys_cursors [csrid]->width    = ((PCURSOR)temp)->width;
        sys_cursors [csrid]->height   = ((PCURSOR)temp)->height;
        sys_cursors [csrid]->AndBits  = temp + sizeof(CURSOR);
        sys_cursors [csrid]->XorBits  = temp + sizeof(CURSOR) + __mg_csrimgsize;

        temp += (sizeof(CURSOR) + 2 * __mg_csrimgsize);
    }
}

HCURSOR GUIAPI GetSystemCursor (int csrid)
{
    if (csrid >= ((PG_RES)mgSharedRes)->csrnum || csrid < 0)
        return 0; 

    return (HCURSOR) (sys_cursors [csrid]);
}

HCURSOR GUIAPI GetDefaultCursor (void)
{
    return def_cursor;
}

static BITMAP csr_bmp = {
    BMP_TYPE_NORMAL, 0, 0, 0, 0, CURSORWIDTH, CURSORHEIGHT
};

BOOL mg_InitCursor (void)
{
    sys_cursors = mgSharedRes + ((PG_RES)mgSharedRes)->csroffset;

    if (mgIsServer) {

        if (!(cursorbits = malloc (__mg_csrimgsize))) {
            return FALSE;
        }

        init_system_cursor ();

        CSR_CURRENT_SET = 0;
        CSR_SHOW_COUNT = 0;
        CSR_OLDBOXLEFT  = -100;
        CSR_OLDBOXTOP = -100;
    }

    csr_bmp.bmBitsPerPixel = __gal_screen->format->BitsPerPixel;
    csr_bmp.bmBytesPerPixel = __gal_screen->format->BytesPerPixel;
    csr_bmp.bmPitch = __gal_screen->format->BytesPerPixel * CURSORWIDTH;
    
    return TRUE;
}

/* The following function must be called at last.  */
void mg_TerminateCursor (void)
{
    int i;

    if (mgIsServer) {

        free (cursorbits);

        CSR_CURRENT_SET = 0;
        CSR_SHOW_COUNT = 0;
 
        for (i = 0; i < ((PG_RES)mgSharedRes)->csrnum; i++) {
            if (sys_cursors [i]) {
                free (sys_cursors [i]);
                sys_cursors [i] = NULL;
            }
        }
    }
}

/* Cursor pointer shape and hiding and showing. */
inline static int boxleft (void)
{
    if (!CSR_CURRENT) return -100;
    return CSR_CURSORX - CSR_XHOTSPOT;
}

inline static int boxtop (void)
{
    if (!CSR_CURRENT) return -100;
    return CSR_CURSORY - CSR_YHOTSPOT;
}

static GAL_Rect csr_rect = {0, 0, CURSORWIDTH, CURSORHEIGHT};

static void hidecursor (void)
{
    csr_rect.x = CSR_OLDBOXLEFT;
    csr_rect.y = CSR_OLDBOXTOP;

    csr_bmp.bmBits = CSR_SAVEDBITS;

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
    csr_bmp.bmBits = CSR_SAVEDBITS;

    GAL_SetClipRect (__gal_screen, NULL);
    GAL_GetBox (__gal_screen, &csr_rect, &csr_bmp);

    CSR_OLDBOXLEFT = x;
    CSR_OLDBOXTOP = y;

    GAL_memcpy4 (cursorbits, CSR_SAVEDBITS, __mg_csrimgsize >> 2);

#ifdef ASM_memandcpy4
    ASM_memandcpy4 (cursorbits, CSR_CURRENT->AndBits, __mg_csrimgsize >> 2);
    ASM_memxorcpy4 (cursorbits, CSR_CURRENT->XorBits, __mg_csrimgsize >> 2);
#else
    {
        int i;
        Uint32* andbits = (Uint32*) CSR_CURRENT->AndBits;
        Uint32* xorbits = (Uint32*) CSR_CURRENT->XorBits;
        Uint32* dst = (Uint32*) cursorbits;

        for (i = 0; i < __mg_csrimgsize >> 2; i++) {
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
    int curx, cury;
    BOOL moved = FALSE;

    if (!mgIsServer) return FALSE;

    IAL_GetMouseXY (x, y);
    SHAREDRES_MOUSEX = curx = *x;
    SHAREDRES_MOUSEY = cury = *y;
    if (button)
        SHAREDRES_BUTTON = *button = IAL_GetMouseButton ();

    if (oldx != curx || oldy != cury) {

#ifdef _MGHAVE_CURSOR
        lock_cursor_sem ();
        CSR_CURSORX = curx;
        CSR_CURSORY = cury;
        if (CSR_SHOW_COUNT >= 0 && CSR_CURRENT) {
            if (get_hidecursor_sem_val ()) {
                reset_hidecursor_sem ();
            }
            else
                hidecursor ();
            showcursor ();
        }
        unlock_cursor_sem ();
#endif /* _MGHAVE_CURSOR */

        oldx = curx;
        oldy = cury;
        moved = TRUE;
    }

    return moved;
}

#ifdef _MGHAVE_CURSOR

/* show cursor hidden by client GDI function */
void kernel_ReShowCursor (void)
{
    lock_cursor_sem ();
    if (CSR_SHOW_COUNT >= 0 && CSR_CURRENT) {
        if (get_hidecursor_sem_val ()) {
            reset_hidecursor_sem ();
            showcursor ();
        }
    }
    unlock_cursor_sem ();
}

/* Always call with "setdef = FALSE" for clients at server side. */
HCURSOR GUIAPI SetCursorEx (HCURSOR hcsr, BOOL setdef)
{
    PCURSOR old, pcsr;

    if (!mgIsServer) {
        REQUEST req;

        req.id = REQID_SETCURSOR;
        req.data = &hcsr;
        req.len_data = sizeof (HCURSOR);

        if (ClientRequest (&req, &old, sizeof (PCURSOR)) < 0)
            return 0;

        return (HCURSOR) old;
    }

    if (setdef) {
        old = (PCURSOR) def_cursor;
        def_cursor = hcsr;
    }
    else
        old = CSR_CURRENT;

    if ((PCURSOR)hcsr == CSR_CURRENT) {
        return (HCURSOR) old;
    }

    pcsr = (PCURSOR)hcsr;

    lock_cursor_sem ();
    if (CSR_CURRENT && CSR_SHOW_COUNT >= 0
                    && get_hidecursor_sem_val () == 0)
        hidecursor();

    CSR_CURRENT_SET = (HCURSOR)pcsr;
    CSR_XHOTSPOT = pcsr ? pcsr->xhotspot : -100;
    CSR_YHOTSPOT = pcsr ? pcsr->yhotspot : -100;

    if (CSR_CURRENT && CSR_SHOW_COUNT >= 0
                    && get_hidecursor_sem_val () == 0)
        showcursor();
    unlock_cursor_sem ();

    return (HCURSOR) old;
}

HCURSOR GUIAPI GetCurrentCursor (void)
{
#if 1
    return (HCURSOR)CSR_CURRENT;
#else
    HCURSOR hcsr;

    if (!mgIsServer) {
        REQUEST req;

        req.id = REQID_GETCURRENTCURSOR;
        req.data = 0;
        req.len_data = 0;

        if (ClientRequest (&req, &hcsr, sizeof (HCURSOR)) < 0)
            return 0;

        return hcsr;
    }

    return (HCURSOR)CSR_CURRENT;
#endif
}

inline static BOOL does_need_hide (const RECT* prc)
{
    int csrleft, csrright, csrtop, csrbottom;
    int intleft, intright, inttop, intbottom;

    csrleft = boxleft();
    csrright = csrleft + CURSORWIDTH;
    csrtop = boxtop();
    csrbottom = csrtop + CURSORHEIGHT;

    intleft = (csrleft > prc->left) ? csrleft : prc->left;
    inttop  = (csrtop > prc->top) ? csrtop : prc->top;
    intright = (csrright < prc->right) ? csrright : prc->right;
    intbottom = (csrbottom < prc->bottom) ? csrbottom : prc->bottom;

    if (intleft >= intright || inttop >= intbottom)
        return FALSE;

    return TRUE;
}

void kernel_ShowCursorForGDI (BOOL fShow, void* pdc)
{
    PDC cur_pdc = (PDC)pdc;
    const RECT* prc = NULL;

    prc = &cur_pdc->rc_output;

    if (cur_pdc->surface != __gal_screen) {
        if (fShow)
            GAL_UpdateRect (cur_pdc->surface, 
                            prc->left, prc->top, RECTWP(prc), RECTHP(prc));
    }
    else {
        if (!mgIsServer && (SHAREDRES_TOPMOST_LAYER != __mg_layer)) {
            return;
        }

        if (!fShow) {
            lock_cursor_sem ();
            if (CSR_SHOW_COUNT >= 0 && CSR_CURRENT && does_need_hide (prc)) {
                if (get_hidecursor_sem_val () == 0) {
                    inc_hidecursor_sem ();
                    hidecursor ();
                }
            }
        }
        else {
            GAL_UpdateRect (cur_pdc->surface, 
                            prc->left, prc->top, RECTWP(prc), RECTHP(prc));
            unlock_cursor_sem ();
        }
    }
}

int GUIAPI ShowCursor (BOOL fShow)
{
    if (!mgIsServer) {
        REQUEST req;
        int ret_value;

        req.id = REQID_SHOWCURSOR;
        req.data = &fShow;
        req.len_data = sizeof (BOOL);

        ClientRequest (&req, &ret_value, sizeof (int));
        return ret_value;
    }

    lock_cursor_sem ();
    if (fShow) {
        CSR_SHOW_COUNT++;
        if (CSR_SHOW_COUNT == 0 && CSR_CURRENT
                    && get_hidecursor_sem_val () == 0)
           showcursor();
    }
    else {
        CSR_SHOW_COUNT--;
        if (CSR_SHOW_COUNT == -1 && CSR_CURRENT)
           hidecursor();
    }
    unlock_cursor_sem ();

    return CSR_SHOW_COUNT;
}

#else

void kernel_ShowCursorForGDI (BOOL fShow, void* pdc)
{
    PDC cur_pdc = (PDC)pdc;
    const RECT* prc = NULL;

    prc = &cur_pdc->rc_output;

    /* houhh20080827, if --disable-cursor, mginit can not update auto(qvfb). */
#if 0
    if (cur_pdc->surface == __gal_screen
            && mgIsServer && (SHAREDRES_TOPMOST_LAYER != __mg_layer))
        return;

    if (fShow)
        GAL_UpdateRect (cur_pdc->surface, 
                        prc->left, prc->top, RECTWP(prc), RECTHP(prc));
#else
    if (!mgIsServer && (SHAREDRES_TOPMOST_LAYER != __mg_layer)) {
        return;
    }

    if (fShow)
        GAL_UpdateRect (cur_pdc->surface, 
                prc->left, prc->top, RECTWP(prc), RECTHP(prc));
#endif
}

#endif /* _MGHAVE_CURSOR */

void GUIAPI GetCursorPos (POINT* ppt)
{
    ppt->x = SHAREDRES_MOUSEX;
    ppt->y = SHAREDRES_MOUSEY;
}

void GUIAPI SetCursorPos (int x, int y)
{
    if (mgIsServer) {
        IAL_SetMouseXY (x, y);
        kernel_RefreshCursor (&x, &y, NULL);
    }
    else {
        REQUEST req;
        POINT pt = {x, y};

        req.id = REQID_SETCURSORPOS;
        req.data = &pt;
        req.len_data = sizeof (POINT);

        ClientRequest (&req, NULL, 0);
    }
}

/* Cursor clipping support. */
void GUIAPI ClipCursor (const RECT* prc)
{
    RECT rc;

    if (!mgIsServer) {
        REQUEST req;

        req.id = REQID_CLIPCURSOR;

        if (prc)
            req.data = prc;
        else {
            SetRect (&rc, 0, 0, WIDTHOFPHYGC - 1, HEIGHTOFPHYGC - 1);
            req.data = &rc;
        }
        req.len_data = sizeof (RECT);

        ClientRequest (&req, NULL, 0);
        return;
    }

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
    if (!mgIsServer) {
        REQUEST req;

        req.id = REQID_GETCLIPCURSOR;
        req.data = NULL;
        req.len_data = 0;

        ClientRequest (&req, prc, sizeof (RECT));
        return;
    }

    if (IsRectEmpty (&cliprc))
        SetRect (&cliprc, 0, 0, WIDTHOFPHYGC - 1, HEIGHTOFPHYGC - 1);

    memcpy (prc, &cliprc, sizeof(RECT));
}

#endif /* _MGRM_PROCESSES */
