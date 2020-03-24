///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** cursor-lite.c: Cursor support module for MiniGUI-Processes.
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

static RECT cliprc = {0, 0, 0, 0};

#ifdef _MGHAVE_CURSOR

#define CSR_CURSORX     (((PG_RES)mgSharedRes)->cursorx)
#define CSR_CURSORY     (((PG_RES)mgSharedRes)->cursory)
#define CSR_OLDBOXLEFT  (((PG_RES)mgSharedRes)->oldboxleft)
#define CSR_OLDBOXTOP   (((PG_RES)mgSharedRes)->oldboxtop)

#define CSR_CURRENT     ((PCURSOR)((PG_RES)mgSharedRes)->csr_current)
#define CSR_CURRENT_SET (((PG_RES)mgSharedRes)->csr_current)
#define CSR_XHOTSPOT    (((PG_RES)mgSharedRes)->xhotspot)
#define CSR_YHOTSPOT    (((PG_RES)mgSharedRes)->yhotspot)
#define CSR_SHOW_COUNT  (((PG_RES)mgSharedRes)->csr_show_count)

static PCURSOR *_sys_cursors;
static HCURSOR _def_cursor;

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

#ifdef _MGSCHEMA_SHAREDFB

unsigned int __mg_csrimgsize;
unsigned int __mg_csrimgpitch;

#define CSR_SAVEDBITS   ((BYTE*)mgSharedRes + (((PG_RES)mgSharedRes)->svdbitsoffset))

static BYTE* _cursor_bits = NULL;
#endif

#ifdef _MGSCHEMA_COMPOSITING

/* Cursor creating and destroying. */
static HCURSOR srvCreateCursor (int xhotspot, int yhotspot, int w, int h,
                     const BYTE* pANDBits, const BYTE* pXORBits, int colornum)
{
    HDC dc;
    GAL_Surface* csr_surf;
    PCURSOR pcsr;
    gal_pixel trans_pixel;

    if ((dc = __mg_get_common_rgba8888_dc()) == HDC_INVALID)
        return 0;

    if (w != CURSORWIDTH || h != CURSORHEIGHT)
        return 0;

    if (!(pcsr = mg_slice_new (CURSOR)))
        return 0;

    csr_surf = GAL_CreateCursorSurface (NULL, w, h);
    if (!csr_surf) {
        mg_slice_delete (CURSOR, pcsr);
        return 0;
    }

    trans_pixel = GAL_MapRGBA (csr_surf->format, 0x00, 0x00, 0x00, 0x00);

    pcsr->xhotspot = xhotspot;
    pcsr->yhotspot = yhotspot;
    pcsr->surface = csr_surf;

    if (colornum == 1) {
        int x, y;
        BYTE and_byte = 0;
        BYTE xor_byte = 0;
        gal_pixel white_pixel = GAL_MapRGBA (csr_surf->format, 0xFF, 0xFF, 0xFF, 0xFF);
        gal_pixel black_pixel = GAL_MapRGBA (csr_surf->format, 0x00, 0x00, 0x00, 0xFF);

        pANDBits += MONOPITCH * (CURSORHEIGHT - 1);
        pXORBits += MONOPITCH * (CURSORHEIGHT - 1);
        Uint32* pixels = (Uint32*)csr_surf->pixels;

        for (y = 0; y < CURSORHEIGHT; y++) {
            const BYTE* and_bytes = pANDBits;
            const BYTE* xor_bytes = pXORBits;

            for (x = 0; x < CURSORWIDTH; x++) {
                if (x % 8 == 0) {
                    and_byte = *and_bytes++;
                    xor_byte = *xor_bytes++;
                }

                if (and_byte & (0x80 >> (x % 8))) {
                    pixels [x] = trans_pixel;
                }
                else {
                    if (xor_byte & (0x80 >> (x % 8))) {
                        pixels [x] = white_pixel;
                    }
                    else {
                        pixels [x] = black_pixel;
                    }
                }
            }

            pANDBits -= MONOPITCH;
            pXORBits -= MONOPITCH;
            pixels += (csr_surf->pitch >> 2);
        }
    }
    else if (colornum == 4) {
        int x, y;
        BYTE and_byte = 0;
        BYTE xor_byte = 0;
        int idx_16c = 0;
        const RGB* std16c_rgb = __mg_bmp_get_std_16c ();

        pANDBits += MONOPITCH * (CURSORHEIGHT - 1);
        pXORBits += MONOPITCH * 4 * (CURSORHEIGHT - 1);
        Uint32* pixels = (Uint32*)csr_surf->pixels;

        for (y = 0; y < CURSORHEIGHT; y++) {
            const BYTE* and_bytes = pANDBits;
            const BYTE* xor_bytes = pXORBits;

            for (x = 0; x < CURSORWIDTH; x++) {
                if (x % 8 == 0) {
                    and_byte = *and_bytes++;
                }
                if (x % 2 == 0) {
                    xor_byte = *xor_bytes++;
                    idx_16c = (xor_byte >> 4) & 0x0F;
                }
                else {
                    idx_16c = xor_byte & 0x0F;
                }

                if (and_byte & (0x80 >> (x % 8))) {
                    pixels [x] = trans_pixel;
                }
                else {
                    pixels [x] = GAL_MapRGBA (csr_surf->format,
                            std16c_rgb[idx_16c].r, std16c_rgb[idx_16c].g,
                            std16c_rgb[idx_16c].b, 0xFF);
                }
            }

            pANDBits -= MONOPITCH;
            pXORBits -= (MONOPITCH * 4);
            pixels += (csr_surf->pitch >> 2);
        }
    }

    return (HCURSOR)pcsr;
}

static HCURSOR srvCopyCursor (HCURSOR hcsr)
{
    PCURSOR pcsr = (PCURSOR)hcsr;
    PCURSOR pdcsr;
    GAL_Surface* dcsr_surf;
    GAL_Surface* scsr_surf;

    if (!(pdcsr = mg_slice_new (CURSOR)))
        return 0;

    scsr_surf = pcsr->surface;
    dcsr_surf = GAL_CreateCursorSurface (NULL, scsr_surf->w, scsr_surf->h);
    if (dcsr_surf == NULL) {
        mg_slice_delete (CURSOR, pdcsr);
        return 0;
    }

    pdcsr->xhotspot = pcsr->xhotspot;
    pdcsr->yhotspot = pcsr->yhotspot;

    memcpy(dcsr_surf->pixels, scsr_surf->pixels, scsr_surf->h * scsr_surf->pitch);
    return (HCURSOR) pdcsr;
}

static BOOL srvDestroyCursor (HCURSOR hcsr)
{
    int i;
    PCURSOR pcsr = (PCURSOR)hcsr;

    if (pcsr == NULL)
        return TRUE;

    for (i = 0; i < ((PG_RES)mgSharedRes)->csrnum; i++) {
        if (pcsr == _sys_cursors [i])
            return FALSE;
    }

    if (pcsr == CSR_CURRENT)
        SetCursor(_def_cursor);

    GAL_FreeCursorSurface (pcsr->surface);
    mg_slice_delete (CURSOR, pcsr);
    return TRUE;
}

static BOOL my_buff_allocator (void* context, BITMAP* bmp)
{
    PCURSOR* pcsr = context;
    GAL_Surface* csr_surf;

    if (bmp->bmWidth != CURSORWIDTH || bmp->bmHeight != CURSORHEIGHT)
        return FALSE;

    assert (bmp->bmAlphaPitch == 0);

    csr_surf = GAL_CreateCursorSurface (NULL, bmp->bmWidth, bmp->bmHeight);
    if (!csr_surf) {
        return FALSE;
    }

    bmp->bmBits = csr_surf->pixels;
    (*pcsr)->surface = csr_surf;

    return TRUE;
}

static PCURSOR srvLoadCursorFromPNG (MG_RWops* area)
{
    HDC dc;
    BITMAP bmp;
    PCURSOR pcsr;
    int ret;

    if ((dc = __mg_get_common_rgba8888_dc()) == HDC_INVALID)
        return NULL;

    if (!(pcsr = mg_slice_new (CURSOR)))
        return NULL;

    ret = LoadBitmapEx2 (dc, &bmp, area, "png", my_buff_allocator, &pcsr);
    if (ret) {
        goto error;
    }

    return (HCURSOR)pcsr;

error:
    if (pcsr->surface) {
        GAL_FreeCursorSurface(pcsr->surface);
    }
    mg_slice_delete (CURSOR, pcsr);
    return NULL;
}

static HCURSOR srvLoadCursorFromPNGFile (const char* filename,
        int xhotspot, int yhotspot)
{
    PCURSOR csr;
    MG_RWops* area;

    if (!(area = MGUI_RWFromFile (filename, "rb"))) {
        _WRN_PRINTF ("Failed to make RWOps\n");
        return NULL;
    }

    csr = srvLoadCursorFromPNG(area);
    if (csr) {
        csr->xhotspot = xhotspot;
        csr->yhotspot = yhotspot;
    }

    MGUI_RWclose (area);

    return csr;
}

HCURSOR GUIAPI LoadCursorFromPNGFile (const char* filename,
        int xhotspot, int yhotspot)
{
    if (mgIsServer) {
        return srvLoadCursorFromPNGFile (filename, xhotspot, yhotspot);
    }
    else {
        HCURSOR hcursor;
        REQUEST req;
        int hotspot[2] = { xhotspot, yhotspot };

        req.id = REQID_LOADCURSOR_PNG;
        req.data = hotspot;
        req.len_data = sizeof (hotspot);

        if (ClientRequestEx (&req, filename, strlen (filename) + 1,
                &hcursor, sizeof (HCURSOR)) < 0)
            return 0;

        return hcursor;
    }
}

static HCURSOR srvLoadCursorFromPNGMem (const void* mem, size_t size,
        int xhotspot, int yhotspot)
{
    PCURSOR csr;
    MG_RWops* area;

    if (!(area = MGUI_RWFromMem ((void*)mem, size))) {
        _WRN_PRINTF ("Failed to make RWOps\n");
        return NULL;
    }

    csr = srvLoadCursorFromPNG(area);

    if (csr) {
        csr->xhotspot = xhotspot;
        csr->yhotspot = yhotspot;
    }

    MGUI_RWclose (area);

    return csr;
}

HCURSOR GUIAPI LoadCursorFromPNGMem (const void* area, size_t size,
        int xhotspot, int yhotspot)
{
    if (mgIsServer) {
        return srvLoadCursorFromPNGMem (area, size, xhotspot, yhotspot);
    }
    else {
        HCURSOR hcursor;
        REQUEST req;
        int hotspot[2] = { xhotspot, yhotspot };

        req.id = REQID_LOADCURSOR_PNG_MEM;
        req.data = hotspot;
        req.len_data = sizeof(hotspot);

        if (ClientRequestEx (&req, area, size, &hcursor, sizeof (HCURSOR)) < 0)
            return 0;

        return hcursor;
    }
}

static BITMAP csr_bmp;
static GAL_Rect csr_rect = {0, 0, CURSORWIDTH, CURSORHEIGHT};

static void hidecursor (void)
{
    csr_rect.x = CSR_OLDBOXLEFT;
    csr_rect.y = CSR_OLDBOXTOP;

    GAL_SetClipRect (__gal_screen, NULL);
    GAL_PutBox (__gal_screen, &csr_rect, &csr_bmp);
    GAL_UpdateRects (__gal_screen, 1, &csr_rect);
}

static void showcursor (void)
{
    int x, y;
    GAL_Rect src_rect;

    x = boxleft ();
    y = boxtop ();

    csr_rect.x = x;
    csr_rect.y = y;

    GAL_SetClipRect (__gal_screen, NULL);
    GAL_GetBox (__gal_screen, &csr_rect, &csr_bmp);

    CSR_OLDBOXLEFT = x;
    CSR_OLDBOXTOP = y;

    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.w = CURSORWIDTH;
    src_rect.h = CURSORHEIGHT;

    GAL_SetClipRect (__gal_screen, NULL);
    {
        GAL_Rect scr_rect = { 0, 0, __gal_screen->w, __gal_screen->h };
        GAL_Rect inter_rect;

        if (GAL_IntersectRect (&csr_rect, &scr_rect, &inter_rect)) {
            src_rect.x = csr_rect.x - inter_rect.x;
            src_rect.y = csr_rect.y - inter_rect.y;
            src_rect.w = inter_rect.w;
            src_rect.h = inter_rect.h;

            GAL_BlitSurface (CSR_CURRENT->surface, &src_rect,
                    __gal_screen, &inter_rect);
            GAL_UpdateRects (__gal_screen, 1, &inter_rect);
        }
    }

#if 0 // debug code
    GAL_FillRect (__gal_screen, &csr_rect, 0xFFFFFFFF);
#endif
}

BOOL mg_InitCursor (void)
{
    if (mgIsServer) {
        CSR_CURRENT_SET = 0;
        CSR_SHOW_COUNT = 0;
        CSR_OLDBOXLEFT  = -100;
        CSR_OLDBOXTOP = -100;

        if (__mg_get_common_rgba8888_dc() == HDC_INVALID)
            return FALSE;

        if (!GAL_GetVideoInfo()->hw_cursor) {
            /* use software cursor */
            csr_bmp.bmType = BMP_TYPE_NORMAL;
            csr_bmp.bmBitsPerPixel = __gal_screen->format->BitsPerPixel;
            csr_bmp.bmBytesPerPixel = __gal_screen->format->BytesPerPixel;
            csr_bmp.bmWidth = CURSORWIDTH;
            csr_bmp.bmHeight = CURSORHEIGHT;
            csr_bmp.bmPitch = __gal_screen->format->BytesPerPixel * CURSORWIDTH;
            csr_bmp.bmBits = malloc (csr_bmp.bmPitch * CURSORHEIGHT);
            if (csr_bmp.bmBits == NULL) {
                return FALSE;
            }

            _WRN_PRINTF("We are using software cursor!\n");
        }
    }

    _sys_cursors = (PCURSOR*)((PG_RES)mgSharedRes)->sys_cursors;
    return TRUE;
}

/* The following function must be called at last.  */
void mg_TerminateCursor (void)
{
    int i;

    if (mgIsServer) {

        CSR_CURRENT_SET = 0;
        CSR_SHOW_COUNT = 0;

        for (i = 0; i < ((PG_RES)mgSharedRes)->csrnum; i++) {
            if (_sys_cursors [i]) {
                GAL_FreeCursorSurface (_sys_cursors[i]->surface);
                mg_slice_delete (CURSOR, _sys_cursors [i]);
                _sys_cursors [i] = NULL;
            }
        }

        if (csr_bmp.bmBits)
            free (csr_bmp.bmBits);
    }
}

#else /* not define _MGSCHEMA_COMPOSITING */

/* Cursor creating and destroying. */
static HCURSOR srvCreateCursor (int xhotspot, int yhotspot, int w, int h,
                     const BYTE* pANDBits, const BYTE* pXORBits, int colornum)
{
    PCURSOR pcsr;

    if (w != CURSORWIDTH || h != CURSORHEIGHT) return 0;

    if (!(pcsr = mg_slice_new (CURSOR))) return 0;
    if (!(pcsr->AndBits = malloc (__mg_csrimgsize))) {
        mg_slice_delete (CURSOR, pcsr);
        return 0;
    }
    if (!(pcsr->XorBits = malloc (__mg_csrimgsize))) {
        free (pcsr->AndBits);
        mg_slice_delete (CURSOR, pcsr);
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

static HCURSOR srvCopyCursor (HCURSOR hcsr)
{
    PCURSOR pcsr = (PCURSOR)hcsr;
    PCURSOR pdcsr;

    if (!(pdcsr = mg_slice_new (CURSOR))) return 0;
    if (!(pdcsr->AndBits = malloc (__mg_csrimgsize))) {
        mg_slice_delete (CURSOR, pdcsr);
        return 0;
    }
    if (!(pdcsr->XorBits = malloc (__mg_csrimgsize))) {
        free (pdcsr->AndBits);
        mg_slice_delete (CURSOR, pdcsr);
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

static BOOL srvDestroyCursor (HCURSOR hcsr)
{
    int i;
    PCURSOR pcsr = (PCURSOR)hcsr;

    if (pcsr == NULL)
        return TRUE;

    for (i = 0; i < ((PG_RES)mgSharedRes)->csrnum; i++) {
        if (pcsr == _sys_cursors [i])
            return FALSE;
    }

    if (pcsr == CSR_CURRENT)
        SetCursor(_def_cursor);

    free (pcsr->AndBits);
    free (pcsr->XorBits);
    mg_slice_delete (CURSOR, pcsr);
    return TRUE;
}

static void init_system_cursor (void)
{
    int csrid;
    unsigned char* temp;

    temp = mgSharedRes + ((PG_RES)mgSharedRes)->csroffset;
    temp += sizeof (PCURSOR) * ((PG_RES)mgSharedRes)->csrnum;

    for (csrid = 0; csrid < ((PG_RES)mgSharedRes)->csrnum; csrid++) {

        if (!(_sys_cursors [csrid] = mg_slice_new (CURSOR)))
            return;

        _sys_cursors [csrid]->xhotspot = ((PCURSOR)temp)->xhotspot;
        _sys_cursors [csrid]->yhotspot = ((PCURSOR)temp)->yhotspot;
        _sys_cursors [csrid]->width    = ((PCURSOR)temp)->width;
        _sys_cursors [csrid]->height   = ((PCURSOR)temp)->height;
        _sys_cursors [csrid]->AndBits  = temp + sizeof(CURSOR);
        _sys_cursors [csrid]->XorBits  = temp + sizeof(CURSOR) + __mg_csrimgsize;

        temp += (sizeof(CURSOR) + 2 * __mg_csrimgsize);
    }
}

static BITMAP csr_bmp;

BOOL mg_InitCursor (void)
{
    _sys_cursors = mgSharedRes + ((PG_RES)mgSharedRes)->csroffset;

    if (mgIsServer) {

        if (!(_cursor_bits = malloc (__mg_csrimgsize))) {
            return FALSE;
        }

        init_system_cursor ();

        CSR_CURRENT_SET = 0;
        CSR_SHOW_COUNT = 0;
        CSR_OLDBOXLEFT  = -100;
        CSR_OLDBOXTOP = -100;
    }

    /* NOTE: Always initialize the structue fields with explicit assignments */
    csr_bmp.bmType = BMP_TYPE_NORMAL;
    csr_bmp.bmBitsPerPixel = __gal_screen->format->BitsPerPixel;
    csr_bmp.bmBytesPerPixel = __gal_screen->format->BytesPerPixel;
    csr_bmp.bmWidth = CURSORWIDTH;
    csr_bmp.bmHeight = CURSORHEIGHT;
    csr_bmp.bmPitch = __gal_screen->format->BytesPerPixel * CURSORWIDTH;

    return TRUE;
}

/* The following function must be called at last.  */
void mg_TerminateCursor (void)
{
    int i;

    if (mgIsServer) {

        free (_cursor_bits);
        CSR_CURRENT_SET = 0;
        CSR_SHOW_COUNT = 0;

        for (i = 0; i < ((PG_RES)mgSharedRes)->csrnum; i++) {
            if (_sys_cursors [i]) {
                mg_slice_delete (CURSOR, _sys_cursors [i]);
                _sys_cursors [i] = NULL;
            }
        }
    }
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

#if 0 // Debug code
    if (!mgIsServer) {
        char filename [PATH_MAX + 1];
        struct timeval tv;

        gettimeofday (&tv, NULL);
        sprintf (filename, "saved-cursor-bits-%d.%d.bmp", (int)tv.tv_sec, (int)tv.tv_usec);
        SaveBitmapToFile(HDC_SCREEN, &csr_bmp, filename);
    }
#endif
}

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

    GAL_memcpy4 (_cursor_bits, CSR_SAVEDBITS, __mg_csrimgsize >> 2);

#ifdef ASM_memandcpy4
    ASM_memandcpy4 (_cursor_bits, CSR_CURRENT->AndBits, __mg_csrimgsize >> 2);
    ASM_memxorcpy4 (_cursor_bits, CSR_CURRENT->XorBits, __mg_csrimgsize >> 2);
#else
    {
        int i;
        Uint32* andbits = (Uint32*) CSR_CURRENT->AndBits;
        Uint32* xorbits = (Uint32*) CSR_CURRENT->XorBits;
        Uint32* dst = (Uint32*) _cursor_bits;

        for (i = 0; i < __mg_csrimgsize >> 2; i++) {
            dst [i] &= andbits [i];
            dst [i] ^= xorbits [i];
        }
    }
#endif

    csr_bmp.bmBits = _cursor_bits;
    GAL_PutBox (__gal_screen, &csr_rect, &csr_bmp);
    GAL_UpdateRects (__gal_screen, 1, &csr_rect);
}

#endif /* _MGSCHEMA_COMPOSITING */

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

#define _LEN_BITS   (MONOPITCH * CURSORHEIGHT)
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

#include "cursor-comm.c"

HCURSOR GUIAPI LoadCursorFromFile (const char* filename)
{
    if (mgIsServer) {
        return load_cursor_from_file (filename);
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
    return load_cursor_from_mem (area);
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

HCURSOR GUIAPI GetSystemCursor (int csrid)
{
    if (csrid >= ((PG_RES)mgSharedRes)->csrnum || csrid < 0)
        return 0;

    return (HCURSOR) (_sys_cursors [csrid]);
}

HCURSOR GUIAPI GetDefaultCursor (void)
{
    return _def_cursor;
}

#endif /* _MGHAVE_CURSOR */

/* The return value indicates whether mouse has moved.  */
/* TRUE for moved. */
/* NOTE that only server can call this function. */
/* Since 5.0.0, this function only changes the cursor position */
BOOL kernel_RefreshCursor (int* x, int* y, int* button)
{
    int curx, cury;
    BOOL moved = FALSE;

    if (!mgIsServer)
        return FALSE;

    IAL_GetMouseXY (x, y);
    curx = *x;
    cury = *y;
    if (button)
        SHAREDRES_BUTTON = *button = IAL_GetMouseButton ();

    if (SHAREDRES_MOUSEX != curx || SHAREDRES_MOUSEY != cury) {
        SHAREDRES_MOUSEX = curx;
        SHAREDRES_MOUSEY = cury;
        moved = TRUE;
    }

    return moved;
}

#ifdef _MGHAVE_CURSOR

/* Change cursor position and show cursor hidden by client GDI function */
void kernel_ReShowCursor (void)
{
#ifdef _MGSCHEMA_SHAREDFB
    LOCK_CURSOR_SEM ();

    if (CSR_SHOW_COUNT >= 0 && CSR_CURRENT) {
        if (get_hidecursor_sem_val ()) {
            reset_hidecursor_sem ();
            showcursor ();
            GAL_SyncUpdate (__gal_screen);
        }
        else if (CSR_CURSORX != SHAREDRES_MOUSEX ||
                CSR_CURSORY != SHAREDRES_MOUSEY) {
            CSR_CURSORX = SHAREDRES_MOUSEX;
            CSR_CURSORY = SHAREDRES_MOUSEY;
            hidecursor ();
            showcursor ();
            GAL_SyncUpdate (__gal_screen);
        }
    }

    UNLOCK_CURSOR_SEM ();
#else /* _MGSCHEMA_SHAREDFB */
    if (CSR_SHOW_COUNT >= 0 && CSR_CURRENT &&
            (CSR_CURSORX != SHAREDRES_MOUSEX ||
                CSR_CURSORY != SHAREDRES_MOUSEY)) {

        CSR_CURSORX = SHAREDRES_MOUSEX;
        CSR_CURSORY = SHAREDRES_MOUSEY;
        if (csr_bmp.bmBits) {
            hidecursor ();
            showcursor ();
            GAL_SyncUpdate (__gal_screen);
        }
        else {
            PCURSOR pcsr = (PCURSOR)CSR_CURRENT;
            GAL_MoveCursor (pcsr->surface, CSR_CURSORX, CSR_CURSORY);
        }

    }
#endif /* _MGSCHEMA_COMPOSITING */
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
        old = (PCURSOR) _def_cursor;
        _def_cursor = hcsr;
    }
    else
        old = CSR_CURRENT;

    if ((PCURSOR)hcsr == CSR_CURRENT) {
        return (HCURSOR) old;
    }

    pcsr = (PCURSOR)hcsr;

#ifdef _MGSCHEMA_COMPOSITING
    if (csr_bmp.bmBits) {
        if (CSR_CURRENT && CSR_SHOW_COUNT >= 0)
            hidecursor();

        CSR_CURRENT_SET = (HCURSOR)pcsr;
        CSR_XHOTSPOT = pcsr ? pcsr->xhotspot : -100;
        CSR_YHOTSPOT = pcsr ? pcsr->yhotspot : -100;

        if (CSR_CURRENT && CSR_SHOW_COUNT >= 0)
            showcursor();

        GAL_SyncUpdate (__gal_screen);
    }
    else {
        CSR_CURRENT_SET = (HCURSOR)pcsr;
        CSR_XHOTSPOT = pcsr ? pcsr->xhotspot : -100;
        CSR_YHOTSPOT = pcsr ? pcsr->yhotspot : -100;
        if (CSR_CURRENT && CSR_SHOW_COUNT >= 0)
            GAL_SetCursor (pcsr->surface, pcsr->xhotspot, pcsr->yhotspot);
    }
#else /* defined _MGSCHEMA_COMPOSITING */
    LOCK_CURSOR_SEM ();
    if (CSR_CURRENT && CSR_SHOW_COUNT >= 0
                    && get_hidecursor_sem_val () == 0)
        hidecursor();

    CSR_CURRENT_SET = (HCURSOR)pcsr;
    CSR_XHOTSPOT = pcsr ? pcsr->xhotspot : -100;
    CSR_YHOTSPOT = pcsr ? pcsr->yhotspot : -100;

    if (CSR_CURRENT && CSR_SHOW_COUNT >= 0
                    && get_hidecursor_sem_val () == 0)
        showcursor();

    GAL_SyncUpdate (__gal_screen);
    UNLOCK_CURSOR_SEM ();
#endif /* not defined _MGSCHEMA_COMPOSITING */

    return (HCURSOR) old;
}

HCURSOR GUIAPI GetCurrentCursor (void)
{
    return (HCURSOR)CSR_CURRENT;
}

static inline BOOL does_need_hide (const RECT* prc)
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

    if (intleft >= intright || inttop >= intbottom) {
        return FALSE;
    }

    return TRUE;
}

#ifdef _MGSCHEMA_SHAREDFB

/* version for _MGSCHEMA_SHAREDFB */
void kernel_ShowCursorForGDI (BOOL fShow, void* pdc)
{
    PDC cur_pdc = (PDC)pdc;
    const RECT* prc;

    prc = &cur_pdc->rc_output;
    if (cur_pdc->surface != __gal_screen) {
        if (fShow) {
            GAL_UpdateRect (cur_pdc->surface,
                            prc->left, prc->top, RECTWP(prc), RECTHP(prc));
            GAL_SyncUpdate (cur_pdc->surface);
        }
    }
    else {
        if (!mgIsServer && (SHAREDRES_TOPMOST_LAYER != __mg_layer)) {
            return;
        }

        if (!fShow) {
            LOCK_CURSOR_SEM ();
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
            // GAL_SyncUpdate (cur_pdc->surface);
            UNLOCK_CURSOR_SEM ();
        }
    }
}

#else   /* defined _MGSCHEMA_SHAREDFB */

/* Version for _MGSCHEMA_COMPOSITING */
void kernel_ShowCursorForGDI (BOOL fShow, void* pdc)
{
    PDC cur_pdc = (PDC)pdc;
    const RECT* prc;

    if (!mgIsServer) {
        return;
    }

    prc = &cur_pdc->rc_output;
    if (cur_pdc->surface == __gal_screen) {
        // Under compositing schema, we never call SyncUpdate in
        // kernel_ShowCursorForGDI for __gal_screen.
        // Only call showcursor/hidecursor for software cursor.
        if (!fShow) {
            if (csr_bmp.bmBits && CSR_SHOW_COUNT >= 0 && CSR_CURRENT &&
                    does_need_hide (prc)) {
                hidecursor ();
            }
        }
        else {
            if (csr_bmp.bmBits && CSR_CURRENT && CSR_SHOW_COUNT >= 0 &&
                    does_need_hide (prc)) {
                showcursor();
            }

            GAL_UpdateRect (cur_pdc->surface,
                            prc->left, prc->top, RECTWP(prc), RECTHP(prc));
        }
    }
    else {
        GAL_UpdateRect (cur_pdc->surface,
                        prc->left, prc->top, RECTWP(prc), RECTHP(prc));
        // For surface other than screen, we call SyncUpdate.
        GAL_SyncUpdate (cur_pdc->surface);
    }
}

#endif  /* not defined _MGSCHEMA_SHAREDFB */

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

#ifdef _MGSCHEMA_SHAREDFB
    LOCK_CURSOR_SEM ();
    if (fShow) {
        CSR_SHOW_COUNT++;
        if (CSR_SHOW_COUNT == 0 && CSR_CURRENT
                    && get_hidecursor_sem_val () == 0) {
           showcursor();
           GAL_SyncUpdate (__gal_screen);
        }
    }
    else {
        CSR_SHOW_COUNT--;
        if (CSR_SHOW_COUNT == -1 && CSR_CURRENT) {
           hidecursor();
           GAL_SyncUpdate (__gal_screen);
        }
    }
    GAL_SyncUpdate (__gal_screen);
    UNLOCK_CURSOR_SEM ();
#else /* defined _MGSCHEMA_SHAREDFB */
    if (fShow) {
        CSR_SHOW_COUNT++;
        if (CSR_SHOW_COUNT == 0 && CSR_CURRENT) {
            if (csr_bmp.bmBits) {
                showcursor();
                GAL_SyncUpdate (__gal_screen);
            }
            else {
                PCURSOR pcsr = (PCURSOR)CSR_CURRENT;
                GAL_SetCursor (pcsr->surface, pcsr->xhotspot, pcsr->yhotspot);
            }
        }
    }
    else {
        CSR_SHOW_COUNT--;
        if (CSR_SHOW_COUNT == -1 && CSR_CURRENT) {
            if (csr_bmp.bmBits) {
                hidecursor();
                GAL_SyncUpdate (__gal_screen);
            }
            else {
                // GAL_SetCursor (NULL, 0, 0);
                PCURSOR pcsr = (PCURSOR)CSR_CURRENT;
                GAL_MoveCursor (pcsr->surface, -100, -100);
            }
        }
    }
#endif /* not defined _MGSCHEMA_SHAREDFB */

    return CSR_SHOW_COUNT;
}

#else /* _MGHAVE_CURSOR */

/* version for no cursor support */
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

    IAL_SetMouseRange (cliprc.left, cliprc.top,
            cliprc.right - 1, cliprc.bottom - 1);
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
