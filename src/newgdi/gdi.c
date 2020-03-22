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
 *   Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
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
** The graphics display interface module of MiniGUI.
**
** Current maintainer: Wei Yongming.
**
** Create date: 1999.01.03
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "cursor.h"
#include "internals.h"
#include "inline.h"
#include "memops.h"
#include "ctrlclass.h"
#include "dc.h"
#include "sysfont.h"
#include "devfont.h"
#include "drawtext.h"
#include "debug.h"

/************************* global data define ********************************/
DC __mg_screen_dc;
DC __mg_screen_sys_dc;

gal_pixel SysPixelIndex [17];

/* This should be the standard EGA palette. */
const RGB SysPixelColor [] = {
    {0x00, 0x00, 0x00, 0x00},     /* transparent   --0 */
    {0x00, 0x00, 0x80, 0xFF},     /* dark blue     --1 */
    {0x00, 0x80, 0x00, 0xFF},     /* dark green    --2 */
    {0x00, 0x80, 0x80, 0xFF},     /* dark cyan     --3 */
    {0x80, 0x00, 0x00, 0xFF},     /* dark red      --4 */
    {0x80, 0x00, 0x80, 0xFF},     /* dark magenta  --5 */
    {0x80, 0x80, 0x00, 0xFF},     /* dark yellow   --6 */
    {0x80, 0x80, 0x80, 0xFF},     /* dark gray     --7 */
    {0xC0, 0xC0, 0xC0, 0xFF},     /* light gray    --8 */
    {0x00, 0x00, 0xFF, 0xFF},     /* blue          --9 */
    {0x00, 0xFF, 0x00, 0xFF},     /* green         --10 */
    {0x00, 0xFF, 0xFF, 0xFF},     /* cyan          --11 */
    {0xFF, 0x00, 0x00, 0xFF},     /* red           --12 */
    {0xFF, 0x00, 0xFF, 0xFF},     /* magenta       --13 */
    {0xFF, 0xFF, 0x00, 0xFF},     /* yellow        --14 */
    {0xFF, 0xFF, 0xFF, 0xFF},     /* light white   --15 */
    {0x00, 0x00, 0x00, 0xFF}      /* black         --16 */
};

#ifdef _MGRM_THREADS
/* mutex ensuring exclusive access to gdi.  */
pthread_mutex_t __mg_gdilock;
#endif

/**************************** static data ************************************/
/* General DC */
static DC DCSlot [DCSLOTNUMBER];

#ifdef _MGRM_THREADS
/* mutex ensuring exclusive access to DC slot.  */
static pthread_mutex_t dcslot;
#endif

BLOCKHEAP __mg_FreeClipRectList;

/************************* static functions declaration **********************/
static void dc_InitClipRgnInfo (void);
static void dc_DeinitClipRgnInfo(void);
static void dc_InitDC (PDC pdc, HWND hWnd, BOOL bIsClient);
static void dc_InitMemDCFrom (PDC pdc, const PDC pdc_ref);
static void dc_InitScreenDC (PDC pdc, GAL_Surface* surface);

#ifndef _MGSCHEMA_COMPOSITING
static BOOL RestrictControlECRGNEx (RECT* minimal,
        PCONTROL pCtrl, CLIPRGN * ergn)
{
    RECT rc;
    PCONTROL pRoot = (PCONTROL) (pCtrl->pMainWin);
    int off_x = 0, off_y = 0;
    int idx;
    MASKRECT * maskrect;

    do {
        PCONTROL pParent = pCtrl;

        rc.left = pRoot->cl + off_x;
        rc.top  = pRoot->ct + off_y;
        rc.right = pRoot->cr + off_x;
        rc.bottom = pRoot->cb + off_y;

        if (!IntersectRect (minimal, minimal, &rc)) {
            SetRect (minimal, 0, 0, 0, 0);
            return FALSE;
        }
        off_x += pRoot->cl;
        off_y += pRoot->ct;

        if (pRoot == pCtrl->pParent)
            break;

        while (TRUE) {
            if (pRoot->children == pParent->pParent->children) {
                pRoot = pParent;
                break;
            }
            pParent = pParent->pParent;
        }
    } while (TRUE);

    /** intersect with maskrects */
    maskrect = pCtrl->mask_rects;
    if (maskrect) {
        idx = 0;
        do {
            rc.left = (maskrect + idx)->left + pCtrl->left + off_x;
            rc.top  = (maskrect + idx)->top + pCtrl->top + off_y;
            rc.right = (maskrect + idx)->right + pCtrl->left + off_x;
            rc.bottom = (maskrect + idx)->bottom + pCtrl->top + off_y;
            AddClipRect (ergn, &rc);
            idx = (maskrect + idx)->next;
        }while (idx != 0);

        IntersectClipRect (ergn, minimal);
    }
    else {
        rc.left = pCtrl->left + off_x;
        rc.top  = pCtrl->top + off_y;
        rc.right = pCtrl->right + off_x;
        rc.bottom = pCtrl->bottom + off_y;
        AddClipRect (ergn, &rc);
        IntersectClipRect (ergn, minimal);
    }

    return TRUE;
}
#endif /* not defined _MGSCHEMA_COMPOSITING */

static BOOL RestrictControlMemDCECRGNEx (RECT* minimal,
        PCONTROL pCtrl, CLIPRGN * ergn)
{
    RECT rc;
    PCONTROL pRoot = (PCONTROL) (pCtrl->pMainWin);
    int off_x = 0, off_y = 0;
    int idx;
    MASKRECT * maskrect;

    do {
        PCONTROL pParent = pCtrl;

        rc.left = pRoot->cl + off_x;
        rc.top  = pRoot->ct + off_y;
        rc.right = pRoot->cr + off_x;
        rc.bottom = pRoot->cb + off_y;

        OffsetRect (&rc, -pCtrl->pMainWin->left, -pCtrl->pMainWin->top);

        if (!IntersectRect (minimal, minimal, &rc)) {
            SetRect (minimal, 0, 0, 0, 0);
            return FALSE;
        }
        off_x += pRoot->cl;
        off_y += pRoot->ct;

        if (pRoot == pCtrl->pParent)
            break;

        while (TRUE) {
            if (pRoot->children == pParent->pParent->children) {
                pRoot = pParent;
                break;
            }
            pParent = pParent->pParent;
        }
    } while (TRUE);

    /** intersect with maskrects */
    maskrect = pCtrl->mask_rects;
    if (maskrect) {
        idx = 0;
        do {
            rc.left = (maskrect + idx)->left + pCtrl->left + off_x;
            rc.top  = (maskrect + idx)->top + pCtrl->top + off_y;
            rc.right = (maskrect + idx)->right + pCtrl->left + off_x;
            rc.bottom = (maskrect + idx)->bottom + pCtrl->top + off_y;

            OffsetRect (&rc, -pCtrl->pMainWin->left, -pCtrl->pMainWin->top);
            AddClipRect (ergn, &rc);
            idx = (maskrect + idx)->next;
        }while (idx != 0);

        IntersectClipRect (ergn, minimal);
    }
    else {
        rc.left = pCtrl->left + off_x;
        rc.top  = pCtrl->top + off_y;
        rc.right = pCtrl->right + off_x;
        rc.bottom = pCtrl->bottom + off_y;

        OffsetRect (&rc, -pCtrl->pMainWin->left, -pCtrl->pMainWin->top);

        AddClipRect (ergn, &rc);
        IntersectClipRect (ergn, minimal);
    }

    return TRUE;
}

/******************* Initialization and termination of GDI *******************/
BOOL mg_InitScreenDC (void)
{
    InitFreeClipRectList (&__mg_FreeClipRectList, SIZE_CLIPRECTHEAP);

    INIT_LOCK (&__mg_gdilock, NULL);
    INIT_LOCK (&dcslot, NULL);

    dc_InitClipRgnInfo ();
#ifdef _MGSCHEMA_COMPOSITING
    if (mgIsServer) {
        // use wallpaper pattern for HDC_SCREEN
        dc_InitScreenDC (&__mg_screen_dc, __gal_fake_screen);
        __mg_screen_dc.DCType = TYPE_MEMDC; // mark mem dc

        // use real screen surface for HDC_SCREEN_SYS
        dc_InitScreenDC (&__mg_screen_sys_dc, __gal_screen);
    }
    else {
        dc_InitScreenDC (&__mg_screen_dc, __gal_screen);
        __mg_screen_dc.DCType = TYPE_MEMDC;     // mark mem dc
        dc_InitScreenDC (&__mg_screen_sys_dc, __gal_screen);
        __mg_screen_sys_dc.DCType = TYPE_MEMDC; // mark mem dc
    }
#else
    dc_InitScreenDC (&__mg_screen_dc, __gal_screen);
    dc_InitScreenDC (&__mg_screen_sys_dc, __gal_screen);
#endif

    return TRUE;
}

void mg_TerminateScreenDC (void)
{
#if 0   /* deprecated code */
    /* Since 5.0.0, do not call any GDI functions here */
    RECT rcScr = GetScreenRect ();

    SelectClipRect (HDC_SCREEN, &rcScr);
    SetBrushColor (HDC_SCREEN, 0);
#ifdef _MGRM_PROCESSES
    if (mgIsServer)
#endif
        FillBox (HDC_SCREEN, rcScr.left, rcScr.top,
                       rcScr.right, rcScr.bottom);
    SelectClipRect (HDC_SCREEN_SYS, &rcScr);
#endif  /* deprecated code */

    EmptyClipRgn (&__mg_screen_dc.lcrgn);
    EmptyClipRgn (&__mg_screen_dc.ecrgn);
    if (__mg_screen_dc.alpha_pixel_format)
        free (__mg_screen_dc.alpha_pixel_format);

    EmptyClipRgn (&__mg_screen_sys_dc.lcrgn);
    EmptyClipRgn (&__mg_screen_sys_dc.ecrgn);
    if (__mg_screen_sys_dc.alpha_pixel_format)
        free (__mg_screen_sys_dc.alpha_pixel_format);

    dc_DeinitClipRgnInfo();

    DESTROY_LOCK (&__mg_gdilock);
    DESTROY_LOCK (&dcslot);

    /* [2010/06/02] DongJunJie : fix a dead-lock bug of mgncs. */
    DestroyFreeClipRectList (&__mg_FreeClipRectList);
}

#define INIT_SPECIFICAL_FONTS(etc_section) \
{ \
    if (!font_InitSpecificalFonts (etc_section)) { \
        _WRN_PRINTF ("Can not initialize fonts defined in section %s!\n", etc_section); \
        goto error; \
    } \
}

#ifndef _MG_MINIMALGDI
BOOL mg_InitGDI (void)
{
    if (!gdi_InitTextBitmapBuffer ()) {
        _WRN_PRINTF ("Can not initialize text bitmap buffer!\n");
        goto error;
    }

#ifdef _MGFONT_RBF
    INIT_SPECIFICAL_FONTS (FONT_ETC_SECTION_NAME_RBF);
#endif

#ifdef _MGFONT_VBF
    INIT_SPECIFICAL_FONTS (FONT_ETC_SECTION_NAME_VBF);
#endif

#ifdef _MGFONT_QPF
    INIT_SPECIFICAL_FONTS (FONT_ETC_SECTION_NAME_QPF);
#endif

#ifdef _MGFONT_UPF
    INIT_SPECIFICAL_FONTS (FONT_ETC_SECTION_NAME_UPF);
#endif

#if defined (_MGFONT_TTF) || defined (_MGFONT_FT2)
    if (!font_InitFreetypeLibrary ()) {
        _WRN_PRINTF ("Can not initialize freetype fonts!\n");
        goto error;
    }
    INIT_SPECIFICAL_FONTS (FONT_ETC_SECTION_NAME_TTF);
#endif

#ifdef _MGFONT_SEF
    if(!font_InitializeScripteasy()) {
        _WRN_PRINTF ("Can not initialize ScriptEasy fonts!\n");
        goto error;
    }
#endif

    /* XXX: add other font support here */

    if (!font_InitIncoreFonts ()) {
        _WRN_PRINTF ("Can not initialize incore fonts!\n");
        goto error;
    }

#ifdef _DEBUG_DEVFONT
    dbg_dumpDevFonts ();
#endif

    if (!mg_InitSysFont ()) {
        _WRN_PRINTF ("Can not create system fonts!\n");
        goto error;
    }

    return TRUE;

error:
    return FALSE;
}

void mg_TerminateGDI( void )
{
    mg_TermSysFont ();

#if defined(_MGFONT_TTF) || defined(_MGFONT_FT2)
    font_TermSpecificalFonts (FONT_ETC_SECTION_NAME_TTF);
    font_TermFreetypeLibrary ();
#endif

#ifdef _MGFONT_QPF
    font_TermSpecificalFonts (FONT_ETC_SECTION_NAME_QPF);
#endif

#ifdef _MGFONT_UPF
    font_TermSpecificalFonts (FONT_ETC_SECTION_NAME_UPF);
#endif

#ifdef _MGFONT_VBF
    font_TermSpecificalFonts (FONT_ETC_SECTION_NAME_VBF);
#endif

#ifdef _MGFONT_RBF
    font_TermSpecificalFonts (FONT_ETC_SECTION_NAME_RBF);
#endif

#ifdef _MGFONT_SEF
    font_UninitializeScripteasy();
#endif

    font_TerminateIncoreFonts ();
    font_ResetDevFont ();

    TermTextBitmapBuffer ();
}
#endif

/*
 * Function: Uint32 GUIAPI GetGDCapability( int iItem)
 *      This Function return DC parameters.
 * Parameters:
 *      The element want to retrive.
 * Return:
 *      The parameter.
 */
Uint32 GUIAPI GetGDCapability (HDC hdc, int iItem)
{
    PDC pdc;
    Uint32 iret = 0xFFFFFFFF;

    pdc = dc_HDC2PDC (hdc);

    LOCK (&__mg_gdilock);

    switch (iItem)
    {
        case GDCAP_DEPTH:
            iret = GAL_BitsPerPixel (pdc->surface);
/*
 * FIXME
 * for 1555 pixel format, color depth should be 15
 */
            if (iret == 16 && GAL_RMask(pdc->surface) == 0x7c00 &&
                              GAL_GMask(pdc->surface) == 0x3e00)
                iret = 15;
            break;

        case GDCAP_BITSPP:
            iret = GAL_BitsPerPixel (pdc->surface);
            break;

        case GDCAP_BPP:
            iret = GAL_BytesPerPixel (pdc->surface);
            break;

        case GDCAP_PITCH:
            iret = pdc->surface->pitch;
            break;

        case GDCAP_RMASK:
            iret = GAL_RMask (pdc->surface);
            break;
        case GDCAP_GMASK:
            iret = GAL_GMask (pdc->surface);
            break;
        case GDCAP_BMASK:
            iret = GAL_BMask (pdc->surface);
            break;
        case GDCAP_AMASK:
            iret = GAL_AMask (pdc->surface);
            break;

        case GDCAP_COLORNUM:
            iret = GAL_BitsPerPixel (pdc->surface);
            if (iret < 32)
                iret = 1 << iret;
            else
                iret = 0xFFFFFFFF;
            break;

        case GDCAP_HPIXEL:
            iret = GAL_Width (pdc->surface);
            break;

        case GDCAP_VPIXEL:
            iret = GAL_Height (pdc->surface);
            break;

        case GDCAP_MAXX:
            iret = RECTW (pdc->DevRC) - 1;
            break;

        case GDCAP_MAXY:
            iret = RECTH (pdc->DevRC) - 1;
            break;

        case GDCAP_DPI:
            iret = pdc->surface->dpi;
            break;
    }

    UNLOCK(&__mg_gdilock);
    return iret;
}

/* This function init clip region in all DC slots. */
static void dc_InitClipRgnInfo(void)
{
    int i;

    for (i=0; i<DCSLOTNUMBER; i++) {
        /* Local clip region */
        InitClipRgn (&DCSlot[i].lcrgn, &__mg_FreeClipRectList);
        MAKE_REGION_INFINITE(&DCSlot[i].lcrgn);

#ifndef _MGSCHEMA_COMPOSITING
        /* Global clip region info */
        DCSlot[i].pGCRInfo = NULL;
        DCSlot[i].oldage = 0;
#endif

        /* Effective clip region */
        InitClipRgn (&DCSlot[i].ecrgn, &__mg_FreeClipRectList);
    }
}

static void dc_DeinitClipRgnInfo(void)
{
    int i;

    for (i=0; i<DCSLOTNUMBER; i++) {
        EmptyClipRgn (&DCSlot[i].lcrgn);
        EmptyClipRgn (&DCSlot[i].ecrgn);
    }
}


static inline void dc_CalculateDevRC4GenDC (PDC pdc);

/*
 * This function generates effective clip region from
 * local clip region and global clip region.
 * if the global clip region has a new age,
 * this function empty effective clip region first,
 * and then intersect local clip region and global clip region.
 */
BOOL dc_GenerateECRgn(PDC pdc, BOOL fForce)
{
#ifdef _MGSCHEMA_COMPOSITING
    assert (0); //  never touch here
#else   /* not defined _MGSCHEMA_COMPOSITING */

    /* is global clip region is empty? */
    if ((!fForce) && (!dc_IsVisible (pdc)))
        return FALSE;

#ifdef _MG_MINIMALGDI
    return TRUE;
#endif

    /* need regenerate? */
    if (fForce || (pdc->oldage != pdc->pGCRInfo->age)) {
        PCLIPRECT pcr;
        PCONTROL pCtrl;
        RECT minimal;
        CLIPRGN ergn;

        /* update pdc->DevRC, and restrict the effective */
        dc_CalculateDevRC4GenDC (pdc);

        /* copy local clipping region to effective clipping region. */
        ClipRgnCopy (&pdc->ecrgn, &pdc->lcrgn);

        /* transfer device coordinates to screen coordinates. */
        pcr = pdc->ecrgn.head;
        while (pcr) {
            coor_DP2SP (pdc, &pcr->rc.left, &pcr->rc.top);
            coor_DP2SP (pdc, &pcr->rc.right, &pcr->rc.bottom);

            pcr = pcr->next;
        }

        /* intersect with global clipping region. */
        {
            coor_DP2SP (pdc, &pdc->ecrgn.rcBound.left,
                            &pdc->ecrgn.rcBound.top);
            coor_DP2SP (pdc, &pdc->ecrgn.rcBound.right,
                            &pdc->ecrgn.rcBound.bottom);
            ClipRgnIntersect (&pdc->ecrgn, &pdc->ecrgn, &pdc->pGCRInfo->crgn);
#ifdef _DEBUG_REGION
            dbg_dumpRegion (&pdc->pGCRInfo->crgn);
            dbg_dumpRegion (&pdc->ecrgn);
#endif
        }

        /*
         * clipping region more with pdc->DevRC.
         */
        minimal = pdc->DevRC;

        /* restrict control's effective region. */
        pCtrl = gui_Control (pdc->hwnd);

        if (pCtrl && !(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN)) {
            InitClipRgn (&ergn, &__mg_FreeClipRectList);
            if (RestrictControlECRGNEx (&minimal, pCtrl, &ergn)) {
                ClipRgnIntersect (&pdc->ecrgn, &pdc->ecrgn, &ergn);
                EmptyClipRgn (&ergn);
            }
            else {
                EmptyClipRgn (&pdc->ecrgn);
            }
        }
        else {
            IntersectClipRect (&pdc->ecrgn, &minimal);
        }

        pdc->oldage = pdc->pGCRInfo->age;
    }
#endif   /* not defined _MGSCHEMA_COMPOSITING */

    return TRUE;
}

static void dc_CalculateDevRC4MemDC (PDC pdc);

PDC __mg_check_ecrgn (HDC hdc)
{
    PDC pdc = dc_HDC2PDC(hdc);

    if (pdc->surface->w <= 0 || pdc->surface->h <= 0)
        return NULL;

    if (dc_IsGeneralDC (pdc)) {
        LOCK_GCRINFO (pdc);
        if (!dc_GenerateECRgn (pdc, FALSE)) {
            UNLOCK_GCRINFO (pdc);
            return NULL;
        }
    }
    else if (dc_IsMemDC (pdc) && !dc_GenerateMemDCECRgn (pdc, FALSE)) {
        return NULL;
    }

    return pdc;
}

int __mg_enter_drawing (PDC pdc)
{
    BLOCK_DRAW_SEM (pdc);

    if (WITHOUT_DRAWING (pdc)) {
        goto fail;
    }

    if (!IntersectRect (&pdc->rc_output, &pdc->rc_output, &pdc->ecrgn.rcBound)) {
        goto fail;
    }

    LOCK (&__mg_gdilock);
    if (!dc_IsMemDC (pdc))
        kernel_ShowCursorForGDI (FALSE, pdc);

    return 0;

fail:
    UNBLOCK_DRAW_SEM (pdc);
    return -1;
}

void __mg_enter_drawing_nocheck (PDC pdc)
{
    BLOCK_DRAW_SEM (pdc);
    LOCK (&__mg_gdilock);

    if (!dc_IsMemDC (pdc))
        kernel_ShowCursorForGDI (FALSE, pdc);
}

void __mg_leave_drawing(PDC pdc)
{
    if (!dc_IsMemDC (pdc))
        kernel_ShowCursorForGDI (TRUE, pdc);
#ifdef _MGSCHEMA_COMPOSITING
    else if (pdc->surface->dirty_info) {
        GAL_UpdateRect (pdc->surface,
                pdc->rc_output.left, pdc->rc_output.top,
                RECTW(pdc->rc_output), RECTH(pdc->rc_output));
    }
#endif /* defined _MGSCHEMA_COMPOSITING */

    UNLOCK (&__mg_gdilock);
    UNBLOCK_DRAW_SEM (pdc);
}

static void _dc_draw_pixel_span_set_0 (COMP_CTXT* comp_ctxt, int bytes_per_pixel, int w)
{
    int n = w * bytes_per_pixel;
    if (!comp_ctxt->cur_pixel && !(n & 3) && !((long)comp_ctxt->cur_dst & 3) && (n > 3)) {
        n = n >> 2;
        GAL_memset4 (comp_ctxt->cur_dst, 0, n);
    }
    else {
        memset (comp_ctxt->cur_dst, comp_ctxt->cur_pixel, n);
    }
}

static void _dc_draw_pixel_span_set_1 (COMP_CTXT* comp_ctxt, int w)
{
    if (comp_ctxt->step == 1) {
        _dc_draw_pixel_span_set_0 (comp_ctxt, 1, w);
    }
    else {
        Uint8* row = comp_ctxt->cur_dst;
        while (w > 0) {
            *row = (Uint8) comp_ctxt->cur_pixel;
            row += comp_ctxt->step;
            w -= comp_ctxt->step;
        }
    }
}

static void _dc_draw_pixel_span_set_2 (COMP_CTXT* comp_ctxt, int w)
{
    int step = comp_ctxt->step;
    if (comp_ctxt->cur_pixel != 0 || comp_ctxt->step != 1) {
        Uint16 * dest16 = (Uint16 *)comp_ctxt->cur_dst;
        if (w < 5 || step != 1)
        {
            do {
                *dest16 = (Uint16)comp_ctxt->cur_pixel;
                dest16 += step;
                w -= step;
            } while (w > 0);
        }
        else
        {
            Uint32 mixture = (((Uint16)comp_ctxt->cur_pixel << 16)
                    | (Uint16)comp_ctxt->cur_pixel);
            Uint32 * dest32;
            int count;
            if ((UINT_PTR)comp_ctxt->cur_dst & 3) // Ensure 4-byte alignment.
            {
                *dest16++ = (Uint16)comp_ctxt->cur_pixel;
                --w;
            }
            count = (w >> 1);
            dest32 = (Uint32 *)dest16;
            do {
                *dest32++ = mixture;
            } while (--count > 0); // Please make sure 3 pixels to processed at least!
            if (0 != (w & 1)) // Dealing with the extra pixel.
            {
                *(Uint16 *)dest32 = comp_ctxt->cur_pixel;
            }
        }
    }
#ifdef ASM_memset2
    else if (comp_ctxt->step == 1 && !((long)comp_ctxt->cur_dst & 2)) {
        ASM_memset2 (comp_ctxt->cur_dst, comp_ctxt->cur_pixel, w);
    }
#endif
    else {
        _dc_draw_pixel_span_set_0 (comp_ctxt, 2, w);
    }
}

static void  _dc_draw_pixel_span_set_3 (COMP_CTXT* comp_ctxt, int w)
{
    if (comp_ctxt->step == 1 && comp_ctxt->cur_pixel == 0) {
        _dc_draw_pixel_span_set_0 (comp_ctxt, 3, w);
    }
#ifdef ASM_memset3
    else if (comp_ctxt->step == 1) {
        ASM_memset3 (comp_ctxt->cur_dst, comp_ctxt->cur_pixel, w);
    }
#endif
    else {
        Uint8* row = comp_ctxt->cur_dst;
        int step = (comp_ctxt->step << 1) + comp_ctxt->step;

        while (w > 0) {
            SETVAL_24BIT (row, comp_ctxt->cur_pixel);
            row += step;
            w -= comp_ctxt->step;
        }
    }
}

static void  _dc_draw_pixel_span_set_4 (COMP_CTXT* comp_ctxt, int w)
{
    Uint32* row = (Uint32*)comp_ctxt->cur_dst;

    if (comp_ctxt->step == 1) {
        GAL_memset4 (row, comp_ctxt->cur_pixel, w);
    }
    else {
        while (w > 0) {
            *row = comp_ctxt->cur_pixel;
            row += comp_ctxt->step;
            w -= comp_ctxt->step;
        }
    }
}

static void  _dc_draw_pixel_span_and_1 (COMP_CTXT* comp_ctxt, int w)
{
    Uint8* row = comp_ctxt->cur_dst;
#ifdef ASM_memandset4
    if (comp_ctxt->step == 1 && !((Uint32)row & 3)
            && !(w & 3) && (w > 3)) {
        Uint16 _w = MAKEWORD16 (comp_ctxt->cur_pixel, comp_ctxt->cur_pixel);
        Uint32 _u = MAKELONG32 (_w, _w);
        ASM_memandset4 (row, _u, w >> 2);
        return;
    }
#endif

    while (w > 0) {
        *row &= (Uint8)comp_ctxt->cur_pixel;
        row += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

static void  _dc_draw_pixel_span_and_2 (COMP_CTXT* comp_ctxt, int w)
{
    Uint16* row = (Uint16*)comp_ctxt->cur_dst;
#ifdef ASM_memandset4
    if (comp_ctxt->step == 1 && !((Uint32)row & 3)
            && !(w & 1) && (w > 1)) {
        Uint32 u = MAKELONG (comp_ctxt->cur_pixel, comp_ctxt->cur_pixel);
        ASM_memandset4 (row, u, w >> 1);
        return;
    }
#endif

    while (w > 0) {
        *row &= (Uint16)comp_ctxt->cur_pixel;
        row += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

static void  _dc_draw_pixel_span_and_3 (COMP_CTXT* comp_ctxt, int w)
{
    Uint8* row = comp_ctxt->cur_dst;
    int step = (comp_ctxt->step << 1) + comp_ctxt->step;
#ifdef ASM_memandset3
    if (comp_ctxt->step == 1) {
        ASM_memandset3 (row, comp_ctxt->cur_pixel, w);
        return;
    }
#endif

    while (w > 0) {
        ANDVAL_24BIT (row, comp_ctxt->cur_pixel);
        row += step;
        w -= comp_ctxt->step;
    }
}

static void  _dc_draw_pixel_span_and_4 (COMP_CTXT* comp_ctxt, int w)
{
    Uint32* row = (Uint32*)comp_ctxt->cur_dst;
#ifdef ASM_memandset4
    if (comp_ctxt->step == 1) {
        ASM_memandset4 (row, comp_ctxt->cur_pixel, w);
        return;
    }
#endif

    while (w > 0) {
        *row &= comp_ctxt->cur_pixel;
        row += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

static void  _dc_draw_pixel_span_or_1 (COMP_CTXT* comp_ctxt, int w)
{
    Uint8* row = comp_ctxt->cur_dst;
#ifdef ASM_memorset4
    if (comp_ctxt->step == 1 && !((Uint32)row & 3)
            && !(w & 3) && (w > 3)) {
        Uint16 _w = MAKEWORD16 (comp_ctxt->cur_pixel, comp_ctxt->cur_pixel);
        Uint32 _u = MAKELONG32 (_w, _w);
        ASM_memorset4 (row, _u, w >> 2);
        return;
    }
#endif

    while (w > 0) {
        *row |= (Uint8) comp_ctxt->cur_pixel;
        row += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

static void  _dc_draw_pixel_span_or_2 (COMP_CTXT* comp_ctxt, int w)
{
    Uint16* row = (Uint16*)comp_ctxt->cur_dst;
#ifdef ASM_memorset4
    if (comp_ctxt->step == 1 && !((Uint32)row & 3)
            && !(w & 1) && (w > 1)) {
        Uint32 u = MAKELONG (comp_ctxt->cur_pixel, comp_ctxt->cur_pixel);
        ASM_memorset4 (row, u, w>>1);
        return;
    }
#endif

    while (w > 0) {
        *row |= (Uint16) comp_ctxt->cur_pixel;
        row += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

static void  _dc_draw_pixel_span_or_3 (COMP_CTXT* comp_ctxt, int w)
{
    Uint8* row = comp_ctxt->cur_dst;
    int step = (comp_ctxt->step << 1) + comp_ctxt->step;

#ifdef ASM_memorset3
    if (comp_ctxt->step == 1) {
        ASM_memorset3 (row, comp_ctxt->cur_pixel, w);
        return;
    }
#endif

    while (w > 0) {
        ORVAL_24BIT (row, comp_ctxt->cur_pixel);
        row += step;
        w -= comp_ctxt->step;
    }
}

static void  _dc_draw_pixel_span_or_4 (COMP_CTXT* comp_ctxt, int w)
{
    Uint32* row = (Uint32*)comp_ctxt->cur_dst;
#ifdef ASM_memorset4
    if (comp_ctxt->step == 1) {
        ASM_memorset4 (row, comp_ctxt->cur_pixel, w);
        return;
    }
#endif

    while (w > 0) {
        *row |= comp_ctxt->cur_pixel;
        row += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

static void  _dc_draw_pixel_span_xor_1 (COMP_CTXT* comp_ctxt, int w)
{
    Uint8* row = comp_ctxt->cur_dst;
#ifdef ASM_memxorset4
    if (comp_ctxt->step == 1 && !((Uint32)comp_ctxt->cur_dst & 3)
            && !(w & 3) && (w > 3)) {
        Uint16 _w = MAKEWORD16 (comp_ctxt->cur_pixel, comp_ctxt->cur_pixel);
        Uint32 _u = MAKELONG32 (_w, _w);
        ASM_memxorset4 (comp_ctxt->cur_dst, _u, w >> 2);
        return;
    }
#endif

    while (w > 0) {
        *row ^= (Uint8) comp_ctxt->cur_pixel;
        row += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

static void  _dc_draw_pixel_span_xor_2 (COMP_CTXT* comp_ctxt, int w)
{
    Uint16* row = (Uint16*)comp_ctxt->cur_dst;
#ifdef ASM_memxorset4
    if (comp_ctxt->step == 1 && !((Uint32)comp_ctxt->cur_dst & 3)
            && !(w & 1) && (w > 1)) {
        Uint32 u = MAKELONG (comp_ctxt->cur_pixel, comp_ctxt->cur_pixel);
        ASM_memxorset4 (comp_ctxt->cur_dst, u, w>>1);
        return;
    }
#endif

    while (w > 0) {
        *row ^= (Uint16) comp_ctxt->cur_pixel;
        row += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

static void  _dc_draw_pixel_span_xor_3 (COMP_CTXT* comp_ctxt, int w)
{
    Uint8* row = comp_ctxt->cur_dst;
    int step = (comp_ctxt->step << 1) + comp_ctxt->step;

#ifdef ASM_memxorset3
    if (comp_ctxt->step == 1) {
        ASM_memxorset3 (comp_ctxt->cur_dst, comp_ctxt->cur_pixel, w);
        return;
    }
#endif

    while (w > 0) {
        XORVAL_24BIT (row, comp_ctxt->cur_pixel);
        row += step;
        w -= comp_ctxt->step;
    }
}

static void  _dc_draw_pixel_span_xor_4 (COMP_CTXT* comp_ctxt, int w)
{
    Uint32* row = (Uint32*)comp_ctxt->cur_dst;
#ifdef ASM_memxorset4
    if (comp_ctxt->step == 1) {
        ASM_memxorset4 (comp_ctxt->cur_dst, comp_ctxt->cur_pixel, w);
        return;
    }
#endif

    while (w > 0) {
        *row ^= comp_ctxt->cur_pixel;
        row += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

static void _dc_move_to_1 (PDC pdc, int x, int y)
{
    pdc->cur_dst = (BYTE*)pdc->surface->pixels + pdc->surface->pitch * y;
    pdc->cur_dst += x;
}

static void _dc_move_to_2 (PDC pdc, int x, int y)
{
    pdc->cur_dst = (BYTE*)pdc->surface->pixels + pdc->surface->pitch * y;
    pdc->cur_dst += x << 1;
}

static void _dc_move_to_3 (PDC pdc, int x, int y)
{
    pdc->cur_dst = (BYTE*)pdc->surface->pixels + pdc->surface->pitch * y;
    pdc->cur_dst += (x << 1) + x;
}

static void _dc_move_to_4 (PDC pdc, int x, int y)
{
    pdc->cur_dst = (BYTE*)pdc->surface->pixels + pdc->surface->pitch * y;
    pdc->cur_dst += x << 2;
}

static void _dc_step_x_1 (PDC pdc, int step)
{
    pdc->cur_dst += step;
}

static void _dc_step_x_2 (PDC pdc, int step)
{
    pdc->cur_dst += step << 1;
}

static void _dc_step_x_3 (PDC pdc, int step)
{
    pdc->cur_dst += (step << 1) + step;
}

static void _dc_step_x_4 (PDC pdc, int step)
{
    pdc->cur_dst += step << 2;
}

static void _dc_draw_src_span_set_0
(COMP_CTXT* comp_ctxt, const gal_uint8* src, int bytes_per_pixel, int w)
{
    int n = w * bytes_per_pixel;

    if (!((long)comp_ctxt->cur_dst&3) && !(n&3) && !((long)src&3) && (n > 3)) {
        GAL_memcpy4 (comp_ctxt->cur_dst, src, n >> 2);
    }
    else
        ASM_memcpy (comp_ctxt->cur_dst, src, n);
}

static void _dc_draw_src_span_set_1
(COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w)
{
    if (comp_ctxt->step == 1 && bkmode != BM_TRANSPARENT) {
        _dc_draw_src_span_set_0 (comp_ctxt, src, 1, w);
    }
    else {
        Uint8* row = comp_ctxt->cur_dst;

        if (bkmode == BM_TRANSPARENT) {
            while (w > 0) {
                if (*src != comp_ctxt->skip_pixel)
                    *row = *src;
                row += comp_ctxt->step;
                src += comp_ctxt->step;
                w -= comp_ctxt->step;
            }
        }
        else while (w > 0) {
            *row = *src;
            row += comp_ctxt->step;
            src += comp_ctxt->step;
            w -= comp_ctxt->step;
        }
    }
}

static void _dc_draw_src_span_set_2
(COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w)
{
    if (comp_ctxt->step == 1 && bkmode != BM_TRANSPARENT) {
        _dc_draw_src_span_set_0 (comp_ctxt, src, 2, w);
    }
    else {
        Uint16* dstrow = (Uint16*)comp_ctxt->cur_dst;
        Uint16* srcrow = (Uint16*)src;

        if (bkmode == BM_TRANSPARENT) {
            while (w > 0) {
                if (*srcrow != comp_ctxt->skip_pixel)
                    *dstrow = *srcrow;
                dstrow += comp_ctxt->step;
                srcrow += comp_ctxt->step;
                w -= comp_ctxt->step;
            }
        }
        else while (w > 0) {
            *dstrow = *srcrow;
            dstrow += comp_ctxt->step;
            srcrow += comp_ctxt->step;
            w -= comp_ctxt->step;
        }
    }
}

static void _dc_draw_src_span_set_3
(COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w)
{
    if (comp_ctxt->step == 1 && bkmode != BM_TRANSPARENT) {
        _dc_draw_src_span_set_0 (comp_ctxt, src, 3, w);
    }
    else {
        Uint8* row = (Uint8*)comp_ctxt->cur_dst;
        int step = (comp_ctxt->step << 1) + comp_ctxt->step;

        if (bkmode == BM_TRANSPARENT) {
            while (w > 0) {
#if 0
                if (((* (Uint32*)row) & 0x00FFFFFF) != comp_ctxt->skip_pixel) {
#else
                if (!EQUAL_24BIT (row, comp_ctxt->skip_pixel)) {
#endif
                    *row = *src;
                    *(row + 1) = *(src + 1);
                    *(row + 2) = *(src + 2);
                }
                row += step;
                src += step;
                w -= comp_ctxt->step;
            }
        }
        else while (w > 0) {
            *row = *src;
            *(row + 1) = *(src + 1);
            *(row + 2) = *(src + 2);
            row += step;
            src += step;
            w -= comp_ctxt->step;
        }
    }
}

static void _dc_draw_src_span_set_4
(COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w)
{
    Uint32* dstrow = (Uint32*)comp_ctxt->cur_dst;
    Uint32* srcrow = (Uint32*)src;

    if (comp_ctxt->step == 1 && bkmode != BM_TRANSPARENT) {
        _dc_draw_src_span_set_0 (comp_ctxt, src, 4, w);
    }
    else {
        if (bkmode == BM_TRANSPARENT) {
            while (w > 0) {
                if (*srcrow != comp_ctxt->skip_pixel)
                    *dstrow = *srcrow;
                dstrow += comp_ctxt->step;
                srcrow += comp_ctxt->step;
                w -= comp_ctxt->step;
            }
        }
        else while (w > 0) {
            *dstrow = *srcrow;
            dstrow += comp_ctxt->step;
            srcrow += comp_ctxt->step;
            w -= comp_ctxt->step;
        }
    }
}

static void _dc_draw_src_span_and_1
(COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w)
{
    Uint8* row = comp_ctxt->cur_dst;

#ifdef ASM_memandcpy4
    if (comp_ctxt->step == 1 && bkmode != BM_TRANSPARENT
            && !((Uint32)row & 3) && !((Uint32)src & 3)
            && !(w & 3) && (w > 3)) {
        ASM_memandcpy4 (row, src, w >> 2);
        return;
    }
#endif

    if (bkmode == BM_TRANSPARENT) {
        while (w > 0) {
            if (*src != comp_ctxt->skip_pixel)
                *row &= *src;
            row += comp_ctxt->step;
            src += comp_ctxt->step;
            w -= comp_ctxt->step;
        }
    }
    else while (w > 0) {
        *row &= *src;
        row += comp_ctxt->step;
        src += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

static void _dc_draw_src_span_and_2
(COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w)
{
    Uint16* dstrow = (Uint16*)comp_ctxt->cur_dst;
    Uint16* srcrow = (Uint16*)src;

#ifdef ASM_memandcpy4
    if (comp_ctxt->step == 1 && bkmode != BM_TRANSPARENT
            && !((Uint32)dstrow & 3) && !((Uint32)srcrow & 3)
            && !(w & 1) && (w > 1)) {
        ASM_memandcpy4 (dstrow, srcrow, w >> 1);
        return;
    }
#endif

    if (bkmode == BM_TRANSPARENT) {
        while (w > 0) {
            if (*srcrow != comp_ctxt->skip_pixel)
                *dstrow &= *srcrow;
            dstrow += comp_ctxt->step;
            srcrow += comp_ctxt->step;
            w -= comp_ctxt->step;
        }
    }
    else while (w > 0) {
        *dstrow &= *srcrow;
        dstrow += comp_ctxt->step;
        srcrow += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

static void _dc_draw_src_span_and_3
(COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w)
{
    Uint8* row = comp_ctxt->cur_dst;

    int step = (comp_ctxt->step << 1) + comp_ctxt->step;

#ifdef ASM_memandcpy4
    if (comp_ctxt->step == 1 && bkmode != BM_TRANSPARENT) {
        int i;
        int n = (w << 1) + w;
        int len = n >> 2;
        if (!((Uint32)row & 3) && !((Uint32)src & 3) && len)
            ASM_memandcpy4 (row, src, len);

        len <<= 2;
        row += len; src += len;
        for (i = 0; i < (n % 4); i++) {
            *row++ &= *src++;
        }
        return;
    }
#endif

    if (bkmode == BM_TRANSPARENT) {
        while (w > 0) {
#if 0
            if (((* (Uint32*)row) & 0x00FFFFFF) != comp_ctxt->skip_pixel) {
#else
            if (!EQUAL_24BIT (row, comp_ctxt->skip_pixel)) {
#endif
               *row &= *src;
               *(row + 1) &= *(src + 1);
               *(row + 2) &= *(src + 2);
            }
            row += step;
            src += step;
            w -= comp_ctxt->step;
        }
    }
    else while (w > 0) {
        *row &= *src;
        *(row + 1) &= *(src + 1);
        *(row + 2) &= *(src + 2);
        row += step;
        src += step;
        w -= comp_ctxt->step;
    }
}

static void _dc_draw_src_span_and_4
(COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w)
{
    Uint32* dstrow = (Uint32*)comp_ctxt->cur_dst;
    Uint32* srcrow = (Uint32*)src;

#ifdef ASM_memandcpy4
    if (comp_ctxt->step == 1 && bkmode != BM_TRANSPARENT) {
        ASM_memandcpy4 (dstrow, srcrow, w);
        return;
    }
#endif

    if (bkmode == BM_TRANSPARENT) {
        while (w > 0) {
            if (*srcrow != comp_ctxt->skip_pixel)
                *dstrow &= *srcrow;
            dstrow += comp_ctxt->step;
            srcrow += comp_ctxt->step;
            w -= comp_ctxt->step;
        }
    }
    else while (w > 0) {
        *dstrow &= *srcrow;
        dstrow += comp_ctxt->step;
        srcrow += comp_ctxt->step;
        w -= comp_ctxt->step;
    }

}

static void _dc_draw_src_span_or_1
(COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w)
{
    Uint8* row = comp_ctxt->cur_dst;

#ifdef ASM_memorcpy4
    if (comp_ctxt->step == 1 && bkmode != BM_TRANSPARENT
            && !((Uint32)row & 3) && !((Uint32)src & 3)
            && !(w & 3) && (w > 3)) {
        ASM_memorcpy4 (row, src, w >> 2);
        return;
    }
#endif

    if (bkmode == BM_TRANSPARENT) {
        while (w > 0) {
            if (*src != comp_ctxt->skip_pixel)
                *row |= *src;
            row += comp_ctxt->step;
            src += comp_ctxt->step;
            w -= comp_ctxt->step;
        }
    }
    else while (w > 0) {
        *row |= *src;
        row += comp_ctxt->step;
        src += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

static void _dc_draw_src_span_or_2
(COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w)
{
    Uint16* dstrow = (Uint16*)comp_ctxt->cur_dst;
    Uint16* srcrow = (Uint16*)src;

#ifdef ASM_memorcpy4
    if (comp_ctxt->step == 1 && bkmode != BM_TRANSPARENT
            && !((Uint32)dstrow & 3) && !((Uint32)srcrow & 3)
            && !(w & 1) && (w > 1)) {
        ASM_memorcpy4 (dstrow, srcrow, w >> 1);
        return;
    }
#endif

    if (bkmode == BM_TRANSPARENT) {
        while (w > 0) {
            if (*srcrow != comp_ctxt->skip_pixel)
                *dstrow |= *srcrow;
            dstrow += comp_ctxt->step;
            srcrow += comp_ctxt->step;
            w -= comp_ctxt->step;
        }
    }
    else while (w > 0) {
        *dstrow |= *srcrow;
        dstrow += comp_ctxt->step;
        srcrow += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

static void _dc_draw_src_span_or_3
(COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w)
{
    Uint8* row = comp_ctxt->cur_dst;

    int step = (comp_ctxt->step << 1) + comp_ctxt->step;

#ifdef ASM_memorcpy4
    if (comp_ctxt->step == 1 && bkmode != BM_TRANSPARENT) {
        int i;
        int n = (w << 1) + w;
        int len = n >> 2;
        if (!((Uint32)row & 3) && !((Uint32)src & 3) && len)
            ASM_memorcpy4 (row, src, len);

        len <<= 2;
        row += len; src += len;
        for (i = 0; i < (n % 4); i++) {
            *row++ |= *src++;
        }
        return;
    }
#endif

    if (bkmode == BM_TRANSPARENT) {
        while (w > 0) {
#if 0
            if (((* (Uint32*)row) & 0x00FFFFFF) != comp_ctxt->skip_pixel) {
#else
            if (!EQUAL_24BIT (row, comp_ctxt->skip_pixel)) {
#endif
                *row |= *src;
                *(row + 1) |= *(src + 1);
                *(row + 2) |= *(src + 2);
            }
            row += step;
            src += step;
            w -= comp_ctxt->step;
        }
    }
    else while (w > 0) {
        *row |= *src;
        *(row + 1) |= *(src + 1);
        *(row + 2) |= *(src + 2);
        row += step;
        src += step;
        w -= comp_ctxt->step;
    }
}

static void _dc_draw_src_span_or_4
(COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w)
{
    Uint32* dstrow = (Uint32*)comp_ctxt->cur_dst;
    Uint32* srcrow = (Uint32*)src;

#ifdef ASM_memorcpy4
    if (comp_ctxt->step == 1 && bkmode != BM_TRANSPARENT) {
        ASM_memorcpy4 (dstrow, srcrow, w);
        return;
    }
#endif

    if (bkmode == BM_TRANSPARENT) {
        while (w > 0) {
            if (*srcrow != comp_ctxt->skip_pixel)
                *dstrow |= *srcrow;
            dstrow += comp_ctxt->step;
            srcrow += comp_ctxt->step;
            w -= comp_ctxt->step;
        }
    }
    while (w > 0) {
        *dstrow |= *srcrow;
        dstrow += comp_ctxt->step;
        srcrow += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

static void _dc_draw_src_span_xor_1
(COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w)
{
    Uint8* row = comp_ctxt->cur_dst;

#ifdef ASM_memxorcpy4
    if (comp_ctxt->step == 1 && bkmode != BM_TRANSPARENT
            && !((Uint32)row & 3) && !((Uint32)src & 3)
            && !(w & 3) && (w > 3)) {
        ASM_memxorcpy4 (row, src, w >> 2);
        return;
    }
#endif

    if (bkmode == BM_TRANSPARENT) {
        while (w > 0) {
            if (*src != comp_ctxt->skip_pixel)
                *row ^= *src;
            row += comp_ctxt->step;
            src += comp_ctxt->step;
            w -= comp_ctxt->step;
        }
    }
    else while (w > 0) {
        *row ^= *src;
        row += comp_ctxt->step;
        src += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

static void _dc_draw_src_span_xor_2
(COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w)
{
    Uint16* dstrow = (Uint16*)comp_ctxt->cur_dst;
    Uint16* srcrow = (Uint16*)src;

#ifdef ASM_memxorcpy4
    if (comp_ctxt->step == 1 && bkmode != BM_TRANSPARENT
            && !((Uint32)dstrow & 3) && !((Uint32)srcrow & 3)
            && !(w & 1) && (w > 1)) {
        ASM_memxorcpy4 (dstrow, srcrow, w >> 1);
        return;
    }
#endif

    if (bkmode == BM_TRANSPARENT) {
        while (w > 0) {
            if (*srcrow != comp_ctxt->skip_pixel)
                *dstrow ^= *srcrow;
            dstrow += comp_ctxt->step;
            srcrow += comp_ctxt->step;
            w -= comp_ctxt->step;
        }
    }
    else while (w > 0) {
        *dstrow ^= *srcrow;
        dstrow += comp_ctxt->step;
        srcrow += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

static void _dc_draw_src_span_xor_3
(COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w)
{
    Uint8* row = comp_ctxt->cur_dst;

    int step = (comp_ctxt->step << 1) + comp_ctxt->step;

#ifdef ASM_memxorcpy4
    if (comp_ctxt->step == 1 && bkmode != BM_TRANSPARENT) {
        int i;
        int n = (w << 1) + w;
        int len = n >> 2;
        if (!((Uint32)row & 3) && !((Uint32)src & 3) && len)
            ASM_memxorcpy4 (row, src, len);

        len <<= 2;
        row += len; src += len;
        for (i = 0; i < (n % 4); i++) {
            *row++ ^= *src++;
        }
        return;
    }
#endif

    if (bkmode == BM_TRANSPARENT) {
        while (w > 0) {
#if 0
            if (((* (Uint32*)row) & 0x00FFFFFF) != comp_ctxt->skip_pixel) {
#else
            if (!EQUAL_24BIT (row, comp_ctxt->skip_pixel)) {
#endif
                *row ^= *src;
                *(row + 1) ^= *(src + 1);
                *(row + 2) ^= *(src + 2);
            }
            row += step;
            src += step;
            w -= comp_ctxt->step;
        }
    }
    while (w > 0) {
        *row ^= *src;
        *(row + 1) ^= *(src + 1);
        *(row + 2) ^= *(src + 2);
        row += step;
        src += step;
        w -= comp_ctxt->step;
    }
}

static void _dc_draw_src_span_xor_4
(COMP_CTXT* comp_ctxt, gal_uint8* src, int bkmode, int w)
{
    Uint32* dstrow = (Uint32*)comp_ctxt->cur_dst;
    Uint32* srcrow = (Uint32*)src;

#ifdef ASM_memxorcpy4
    if (comp_ctxt->step == 1 && bkmode != BM_TRANSPARENT) {
        ASM_memxorcpy4 (dstrow, srcrow, w);
        return;
    }
#endif

    if (bkmode == BM_TRANSPARENT) {
        while (w > 0) {
            if (*srcrow != comp_ctxt->skip_pixel)
                *dstrow ^= *srcrow;
            dstrow += comp_ctxt->step;
            srcrow += comp_ctxt->step;
            w -= comp_ctxt->step;
        }
    }
    while (w > 0) {
        *dstrow ^= *srcrow;
        dstrow += comp_ctxt->step;
        srcrow += comp_ctxt->step;
        w -= comp_ctxt->step;
    }
}

#define NR_ROPS         5
#define NR_PIXEL_LEN    5

static CB_COMP_SETHLINE draw_pixel_span_ops [NR_ROPS][NR_PIXEL_LEN] =
{

    {
        _dc_draw_pixel_span_set_1,
        _dc_draw_pixel_span_set_2,
        _dc_draw_pixel_span_set_3,
        _dc_draw_pixel_span_set_4
    },
    {
        _dc_draw_pixel_span_and_1,
        _dc_draw_pixel_span_and_2,
        _dc_draw_pixel_span_and_3,
        _dc_draw_pixel_span_and_4
    },
    {
        _dc_draw_pixel_span_or_1,
        _dc_draw_pixel_span_or_2,
        _dc_draw_pixel_span_or_3,
        _dc_draw_pixel_span_or_4
    },
    {
        _dc_draw_pixel_span_xor_1,
        _dc_draw_pixel_span_xor_2,
        _dc_draw_pixel_span_xor_3,
        _dc_draw_pixel_span_xor_4
    },
    {
        _dc_draw_pixel_span_set_1,
        _dc_draw_pixel_span_set_2,
        _dc_draw_pixel_span_set_3,
        _dc_draw_pixel_span_set_4
    }
};

static CB_COMP_PUTHLINE draw_src_span_ops [NR_ROPS][NR_PIXEL_LEN] =
{
    {
        _dc_draw_src_span_set_1,
        _dc_draw_src_span_set_2,
        _dc_draw_src_span_set_3,
        _dc_draw_src_span_set_4
    },
    {
        _dc_draw_src_span_and_1,
        _dc_draw_src_span_and_2,
        _dc_draw_src_span_and_3,
        _dc_draw_src_span_and_4
    },
    {
        _dc_draw_src_span_or_1,
        _dc_draw_src_span_or_2,
        _dc_draw_src_span_or_3,
        _dc_draw_src_span_or_4
    },
    {
        _dc_draw_src_span_xor_1,
        _dc_draw_src_span_xor_2,
        _dc_draw_src_span_xor_3,
        _dc_draw_src_span_xor_4
    },
    {
        _dc_draw_src_span_set_1,
        _dc_draw_src_span_set_2,
        _dc_draw_src_span_set_3,
        _dc_draw_src_span_set_4
    }
};

static DC_MOVE_TO move_to_ops [NR_PIXEL_LEN] =
{
    _dc_move_to_1,
    _dc_move_to_2,
    _dc_move_to_3,
    _dc_move_to_4
};

static DC_STEP_X step_x_ops [NR_PIXEL_LEN] =
{
    _dc_step_x_1,
    _dc_step_x_2,
    _dc_step_x_3,
    _dc_step_x_4
};

/* ===================================================================== */
static void _dc_set_pixel_1 (COMP_CTXT* comp_ctxt)
{
    *comp_ctxt->cur_dst = (Uint8) comp_ctxt->cur_pixel;
}

static void _dc_set_pixel_2 (COMP_CTXT* comp_ctxt)
{
    *(Uint16 *) comp_ctxt->cur_dst = (Uint16) comp_ctxt->cur_pixel;
}

static void _dc_set_pixel_3 (COMP_CTXT* comp_ctxt)
{
    SETVAL_24BIT (comp_ctxt->cur_dst, comp_ctxt->cur_pixel);
}

static void _dc_set_pixel_4 (COMP_CTXT* comp_ctxt)
{
    *(Uint32 *) comp_ctxt->cur_dst = (Uint32) comp_ctxt->cur_pixel;
}

static void _dc_and_pixel_1 (COMP_CTXT* comp_ctxt)
{
    *comp_ctxt->cur_dst &= (Uint8) comp_ctxt->cur_pixel;
}

static void _dc_and_pixel_2 (COMP_CTXT* comp_ctxt)
{
    *(Uint16 *) comp_ctxt->cur_dst &= (Uint16) comp_ctxt->cur_pixel;
}

static void _dc_and_pixel_3 (COMP_CTXT* comp_ctxt)
{
    ANDVAL_24BIT (comp_ctxt->cur_dst, comp_ctxt->cur_pixel);
}

static void _dc_and_pixel_4 (COMP_CTXT* comp_ctxt)
{
    *(Uint32 *) comp_ctxt->cur_dst &= (Uint32) comp_ctxt->cur_pixel;
}

static void _dc_or_pixel_1 (COMP_CTXT* comp_ctxt)
{
    *comp_ctxt->cur_dst |= (Uint8) comp_ctxt->cur_pixel;
}

static void _dc_or_pixel_2 (COMP_CTXT* comp_ctxt)
{
    *(Uint16 *) comp_ctxt->cur_dst |= (Uint16) comp_ctxt->cur_pixel;
}

static void _dc_or_pixel_3 (COMP_CTXT* comp_ctxt)
{
    ORVAL_24BIT (comp_ctxt->cur_dst, comp_ctxt->cur_pixel);
}

static void _dc_or_pixel_4 (COMP_CTXT* comp_ctxt)
{
    *(Uint32 *) comp_ctxt->cur_dst |= (Uint32) comp_ctxt->cur_pixel;
}

static void _dc_xor_pixel_1 (COMP_CTXT* comp_ctxt)
{
    *comp_ctxt->cur_dst ^= (Uint8) comp_ctxt->cur_pixel;
}

static void _dc_xor_pixel_2 (COMP_CTXT* comp_ctxt)
{
    *(Uint16 *) comp_ctxt->cur_dst ^= (Uint16) comp_ctxt->cur_pixel;
}

static void _dc_xor_pixel_3 (COMP_CTXT* comp_ctxt)
{
    XORVAL_24BIT (comp_ctxt->cur_dst, comp_ctxt->cur_pixel);
}

static void _dc_xor_pixel_4 (COMP_CTXT* comp_ctxt)
{
    *(Uint32 *) comp_ctxt->cur_dst ^= (Uint32) comp_ctxt->cur_pixel;
}
static CB_COMP_SETPIXEL draw_pixel_ops [NR_ROPS][NR_PIXEL_LEN] =
{
    {_dc_set_pixel_1, _dc_set_pixel_2, _dc_set_pixel_3, _dc_set_pixel_4},
    {_dc_and_pixel_1, _dc_and_pixel_2, _dc_and_pixel_3, _dc_and_pixel_4},
    {_dc_or_pixel_1, _dc_or_pixel_2, _dc_or_pixel_3, _dc_or_pixel_4},
    {_dc_xor_pixel_1, _dc_xor_pixel_2, _dc_xor_pixel_3, _dc_xor_pixel_4},
    {_dc_set_pixel_1, _dc_set_pixel_2, _dc_set_pixel_3, _dc_set_pixel_4}
};
/* ===================================================================== */

/* This function calculate the DevRC for the dc */

static inline void dc_CalculateDevRC4GenDC (PDC pdc)
{
    if (pdc->bIsClient)
        gui_WndClientRect (pdc->hwnd, &pdc->DevRC);
    else
        gui_WndRect (pdc->hwnd, &pdc->DevRC);
}

static void dc_CalculateDevRC4MemDC (PDC pdc)
{
    PMAINWIN pWin = (PMAINWIN)pdc->hwnd;

    assert (pWin);

#ifdef _MGSCHEMA_COMPOSITING
    if (pWin->WinType == TYPE_CONTROL &&
            (pWin->dwExStyle & WS_EX_CTRLASMAINWIN)) {
        if (pdc->bIsClient) {
            pdc->DevRC.left = pWin->cl - pWin->left;
            pdc->DevRC.top  = pWin->ct - pWin->top;
            pdc->DevRC.right = pWin->cr - pWin->left;
            pdc->DevRC.bottom = pWin->cb - pWin->top;
        }
        else {
            pdc->DevRC.left = 0;
            pdc->DevRC.top  = 0;
            pdc->DevRC.right = pWin->right - pWin->left;
            pdc->DevRC.bottom = pWin->bottom - pWin->top;
        }
    }
    else
#endif  /* defined _MGSCHEMA_COMPOSITING */
    {
        PMAINWIN pMainWin = pWin->pMainWin;
        RECT rc1, rc2;
        int off_x, off_y;

        if (pdc->bIsClient) {
            /* 1.get client rc relative to screen. */
            gui_WndClientRect (pdc->hwnd, &rc1);
        }
        else {
            /* 2.get window rc relative to screen.*/
            gui_WndRect (pdc->hwnd, &rc1);
        }
        /* 3.main window's rc relative to screen.*/
        gui_WndRect ((HWND)pMainWin, &rc2);

        /* 4.get the DC's offset relative to the main window.*/
        off_x = rc1.left - rc2.left;
        off_y = rc1.top  - rc2.top;

        pdc->DevRC.left   = off_x;
        pdc->DevRC.top    = off_y;
        pdc->DevRC.right  = MIN((pdc->DevRC.left + RECTW(rc1)), RECTW(rc2));
        pdc->DevRC.bottom = MIN((pdc->DevRC.top  + RECTH(rc1)), RECTH(rc2));
    }
}

/* This function initializes a DC: set the default parameters. */
static void dc_InitDC (PDC pdc, HWND hWnd, BOOL bIsClient)
{
    PCONTROL pCtrl;

    pdc->hwnd = hWnd;

    memset (pdc->gray_pixels, 0, sizeof (pdc->gray_pixels));
    memset (pdc->filter_pixels, 0, sizeof (pdc->filter_pixels));

    pdc->bkcolor = GAL_MapRGB (pdc->surface->format, 0xFF, 0xFF, 0xFF);
    pdc->bkmode = 0;

    pdc->pencolor = GAL_MapRGB (pdc->surface->format, 0x00, 0x00, 0x00);
    pdc->brushcolor = GAL_MapRGB (pdc->surface->format, 0xFF, 0xFF, 0xFF);

    pdc->textcolor = GAL_MapRGB (pdc->surface->format, 0x00, 0x00, 0x00);
    if (!(pdc->pLogFont = GetWindowFont (pdc->hwnd)))
        pdc->pLogFont = GetSystemFont (SYSLOGFONT_WCHAR_DEF);
    pdc->tabstop = 8;
    pdc->CurTextPos.x = pdc->CurTextPos.y = 0;
    pdc->cExtra = pdc->wExtra = pdc->alExtra = pdc->blExtra = 0;

    pdc->mapmode = MM_TEXT;
    pdc->ta_flags = TA_LEFT | TA_TOP | TA_NOUPDATECP;
    pdc->bidi_flags = BIDI_FLAG_LTR;

    pdc->ViewOrig.x = pdc->ViewOrig.y = 0;
    pdc->ViewExtent.x = pdc->ViewExtent.y = 1;
    pdc->WindowOrig.x = pdc->WindowOrig.y = 0;
    pdc->WindowExtent.x = pdc->WindowExtent.y = 1;

#ifdef _MGHAVE_ADV_2DAPI
    pdc->pen_type = PT_SOLID;
    pdc->pen_cap_style = PT_CAP_BUTT;
    pdc->pen_join_style = PT_JOIN_MITER;
    pdc->pen_width = 0;
    pdc->dash_offset = 0;
    pdc->dash_list = NULL;
    pdc->dash_list_len = 0;

    pdc->brush_type = BT_SOLID;
    pdc->brush_orig.x = pdc->brush_orig.y = 0;
    pdc->brush_tile = NULL;
    pdc->brush_stipple = NULL;
#endif

    pdc->bIsClient = bIsClient;

    /* Assume that the local clip region is empty. */
    /* Get global clip region info and generate effective clip region. */
    if (dc_IsGeneralDC (pdc)) {
#ifdef _MGSCHEMA_COMPOSITING
        assert (0); // never touch here.
#else   /* not defined _MGSCHEMA_COMPOSITING */
        RECT minimal;

        pdc->pGCRInfo = ((PMAINWIN)pdc->hwnd)->pGCRInfo;

        /* initialize the local and effective clipping regions */
        InitClipRgn (&pdc->lcrgn, &__mg_FreeClipRectList);
        MAKE_REGION_INFINITE(&pdc->lcrgn);
        InitClipRgn (&pdc->ecrgn, &__mg_FreeClipRectList);

        LOCK_GCRINFO (pdc);

        pdc->oldage = pdc->pGCRInfo->age;
        ClipRgnCopy (&pdc->ecrgn, &pdc->pGCRInfo->crgn);

        dc_CalculateDevRC4GenDC (pdc);

        minimal = pdc->DevRC;

        pCtrl = gui_Control (pdc->hwnd);
        if (pCtrl && !(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN)) {
            CLIPRGN ergn;
            InitClipRgn (&ergn, &__mg_FreeClipRectList);

            if (RestrictControlECRGNEx (&minimal, pCtrl, &ergn)) {
                ClipRgnIntersect (&pdc->ecrgn, &pdc->ecrgn, &ergn);
                EmptyClipRgn (&ergn);
            }
            else {
                EmptyClipRgn (&pdc->ecrgn);
            }
        }
        else {
            IntersectClipRect (&pdc->ecrgn, &minimal);
        }

        UNLOCK_GCRINFO (pdc);
#endif  /* not defined _MGSCHEMA_COMPOSITING */
    }
    else if (dc_IsMemDC (pdc) && pdc->hwnd != HWND_NULL) {
        RECT rc_surface, minimal;

        rc_surface.left     = 0;
        rc_surface.top      = 0;
        rc_surface.right    = pdc->surface->w;
        rc_surface.bottom   = pdc->surface->h;

        dc_CalculateDevRC4MemDC (pdc);

        /* initialize the local and effective clipping regions */
        InitClipRgn (&pdc->lcrgn, &__mg_FreeClipRectList);
        MAKE_REGION_INFINITE(&pdc->lcrgn);
        InitClipRgn (&pdc->ecrgn, &__mg_FreeClipRectList);

        /* clipping region more with pdc->DevRC and rc_surface. */
        IntersectRect (&minimal, &pdc->DevRC, &rc_surface);
        SetClipRgn (&pdc->ecrgn, &rc_surface);
        IntersectClipRect (&pdc->lcrgn, &rc_surface);

        pCtrl = gui_Control (pdc->hwnd);
        /* restrict control's effective region. */
        if (pCtrl && !(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN)) {
            CLIPRGN ergn;
            InitClipRgn (&ergn, &__mg_FreeClipRectList);

            if (RestrictControlMemDCECRGNEx (&minimal, pCtrl, &ergn)) {
                ClipRgnIntersect (&pdc->ecrgn, &pdc->ecrgn, &ergn);
            }
            else {
                EmptyClipRgn (&pdc->ecrgn);
            }
            EmptyClipRgn (&ergn);
        }
        else {
            IntersectClipRect (&pdc->ecrgn, &minimal);
        }
    }
    else {
        /* a memory DC not bound with a window */
    }

    /* context info and raster operations. */
    pdc->CurPenPos.x = pdc->CurTextPos.x = 0;
    pdc->CurPenPos.y = pdc->CurTextPos.y = 0;

    pdc->rop = ROP_SET;
    pdc->step = 1;

    pdc->draw_pixel = draw_pixel_ops [pdc->rop]
        [pdc->surface->format->BytesPerPixel - 1];
    pdc->draw_pixel_span = draw_pixel_span_ops [pdc->rop]
        [pdc->surface->format->BytesPerPixel - 1];
    pdc->draw_src_span = draw_src_span_ops [pdc->rop]
        [pdc->surface->format->BytesPerPixel - 1];
    pdc->user_comp_ctxt = pdc->surface->format;

    pdc->cur_dst = (BYTE*)pdc->surface->pixels
            + pdc->surface->pitch * pdc->DevRC.top
            + pdc->surface->format->BytesPerPixel * pdc->DevRC.left;

    pdc->move_to = move_to_ops [pdc->surface->format->BytesPerPixel - 1];
    pdc->step_x = step_x_ops [pdc->surface->format->BytesPerPixel - 1];

    pdc->alpha_pixel_format = NULL;
    SetBitmapScalerType((HDC)pdc, BITMAP_SCALER_DDA);
}

/* This function initilaizes a memory DC from a compliant reference dc
 * and copies the DC attributes from the refrence DC. */
static void dc_InitMemDCFrom (PDC pdc, const PDC pdc_ref)
{
    memset (pdc->gray_pixels, 0, sizeof (pdc->gray_pixels));
    memset (pdc->filter_pixels, 0, sizeof (pdc->filter_pixels));

    /* copy attributes from reference DC
     * gal_pixel bkcolor, pencolor, brushcolor, textcolor;
     * int bkmode, tabstop, cExtra, wExtra, alExtra, blExtra, mapmode, ta_flags, bidi_flags;
     */
    memcpy (&pdc->bkcolor, &pdc_ref->bkcolor,
                sizeof (gal_pixel)*4 + sizeof (int)*9);
#ifdef _MGHAVE_ADV_2DAPI
    memcpy (&pdc->pen_type, &pdc_ref->pen_type,
                (sizeof(int)*7) + sizeof(POINT) + (sizeof (void*)*3));
#endif
    pdc->pLogFont = pdc_ref->pLogFont;

    /* reset view point info. */
    pdc->ViewOrig.x = pdc->ViewOrig.y = 0;
    pdc->ViewExtent.x = pdc->ViewExtent.y = 1;
    pdc->WindowOrig.x = pdc->WindowOrig.y = 0;
    pdc->WindowExtent.x = pdc->WindowExtent.y = 1;

    /* reset context info. */
    pdc->CurPenPos.x = pdc->CurTextPos.x = 0;
    pdc->CurPenPos.y = pdc->CurTextPos.y = 0;

    /* reset raster operations. */
    pdc->rop = pdc_ref->rop;
    pdc->step = 1;
    pdc->hwnd = 0;

    pdc->draw_pixel = draw_pixel_ops [pdc->rop]
        [pdc->surface->format->BytesPerPixel - 1];
    pdc->draw_pixel_span = draw_pixel_span_ops [pdc->rop]
        [pdc->surface->format->BytesPerPixel - 1];
    pdc->draw_src_span = draw_src_span_ops [pdc->rop]
        [pdc->surface->format->BytesPerPixel - 1];
    pdc->user_comp_ctxt = pdc->surface->format;

    pdc->cur_dst = (BYTE*)pdc->surface->pixels
            + pdc->surface->pitch * pdc->DevRC.top
            + pdc->surface->format->BytesPerPixel * pdc->DevRC.left;

    pdc->move_to = move_to_ops [pdc->surface->format->BytesPerPixel - 1];
    pdc->step_x = step_x_ops [pdc->surface->format->BytesPerPixel - 1];

    pdc->alpha_pixel_format = NULL;

    /* Init bitmap scaler ALG */
    //SetBitmapScalerType((HDC)pdc, BITMAP_SCALER_DDA);
    pdc->bitmap_scaler = pdc_ref->bitmap_scaler;
}

static void dc_InitScreenDC (PDC pdc, GAL_Surface *surface)
{
    pdc->DataType = TYPE_HDC;
    pdc->DCType   = TYPE_SCRDC;
    pdc->hwnd = 0;
    pdc->surface = surface;

    pdc->bkcolor = PIXEL_lightwhite;
    pdc->bkmode = 0;

    pdc->brushcolor = PIXEL_lightwhite;

    pdc->pencolor = PIXEL_black;
    pdc->CurPenPos.x = pdc->CurPenPos.y = 0;

    pdc->textcolor = PIXEL_black;
    pdc->pLogFont = GetSystemFont (SYSLOGFONT_WCHAR_DEF);
    pdc->tabstop = 8;
    pdc->CurTextPos.x = pdc->CurTextPos.y = 0;
    pdc->cExtra = pdc->wExtra = pdc->alExtra = pdc->blExtra = 0;

    pdc->mapmode = MM_TEXT;
    pdc->ta_flags = TA_LEFT | TA_TOP | TA_NOUPDATECP;
    pdc->bidi_flags = BIDI_FLAG_LTR;

    pdc->ViewOrig.x = pdc->ViewOrig.y = 0;
    pdc->ViewExtent.x = pdc->ViewExtent.y = 1;
    pdc->WindowOrig.x = pdc->WindowOrig.y = 0;
    pdc->WindowExtent.x = pdc->WindowExtent.y = 1;

#ifdef _MGHAVE_ADV_2DAPI
    pdc->pen_type = PT_SOLID;
    pdc->pen_cap_style = PT_CAP_BUTT;
    pdc->pen_join_style = PT_JOIN_MITER;
    pdc->pen_width = 0;
    pdc->dash_offset = 0;
    pdc->dash_list = NULL;
    pdc->dash_list_len = 0;

    pdc->brush_type = BT_SOLID;
    pdc->brush_orig.x = pdc->brush_orig.y = 0;
    pdc->brush_tile = NULL;
    pdc->brush_stipple = NULL;
#endif

    pdc->bIsClient = FALSE;

    /* init local clippping region */
    InitClipRgn (&pdc->lcrgn, &__mg_FreeClipRectList);
    MAKE_REGION_INFINITE(&pdc->lcrgn);
    /* init effective clippping region */
    InitClipRgn (&pdc->ecrgn, &__mg_FreeClipRectList);

#ifndef _MGSCHEMA_COMPOSITING
    /* init global clip region information */
    pdc->pGCRInfo = NULL;
    pdc->oldage = 0;
#endif

    pdc->DevRC.left = 0;
    pdc->DevRC.top  = 0;
    pdc->DevRC.right = surface->w;
    pdc->DevRC.bottom = surface->h;

    /* Set effetive clippping region to the screen. */
    /* SetClipRgn (&pdc->lcrgn, &pdc->DevRC); */
    SetClipRgn (&pdc->ecrgn, &pdc->DevRC);

    /* context info and raster operations. */
    pdc->rop = ROP_SET;
    pdc->step = 1;

    pdc->draw_pixel = draw_pixel_ops [pdc->rop]
        [pdc->surface->format->BytesPerPixel - 1];
    pdc->draw_pixel_span = draw_pixel_span_ops [pdc->rop]
        [pdc->surface->format->BytesPerPixel - 1];
    pdc->draw_src_span = draw_src_span_ops [pdc->rop]
        [pdc->surface->format->BytesPerPixel - 1];
    pdc->user_comp_ctxt = pdc->surface->format;

    pdc->cur_dst = pdc->surface->pixels;
    pdc->move_to = move_to_ops [pdc->surface->format->BytesPerPixel - 1];
    pdc->step_x = step_x_ops [pdc->surface->format->BytesPerPixel - 1];

    memset (pdc->gray_pixels, 0, sizeof (pdc->gray_pixels));
    memset (pdc->filter_pixels, 0, sizeof (pdc->filter_pixels));
    pdc->alpha_pixel_format = NULL;
    /* Init bitmap scaler ALG */
    SetBitmapScalerType((HDC)pdc, BITMAP_SCALER_DDA);
}

int GUIAPI GetRasterOperation (HDC hdc)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);
    return pdc->rop;
}

int GUIAPI SetRasterOperation (HDC hdc, int rop)
{
    PDC pdc;
    int old;

    pdc = dc_HDC2PDC (hdc);
    old = pdc->rop;

    if (rop >= 0 && rop < NR_ROPS) {
        pdc->rop = rop;

        pdc->draw_pixel = draw_pixel_ops [rop]
                [pdc->surface->format->BytesPerPixel - 1];
        pdc->draw_pixel_span = draw_pixel_span_ops [rop]
                [pdc->surface->format->BytesPerPixel - 1];
        pdc->draw_src_span = draw_src_span_ops [rop]
                [pdc->surface->format->BytesPerPixel - 1];
        pdc->user_comp_ctxt = pdc->surface->format;
    }

    return old;
}

#ifdef _MGSCHEMA_COMPOSITING
static GAL_Surface* get_window_surface (HWND hwnd)
{
    PCONTROL ctrl;
    PMAINWIN mainwin;

    if (hwnd == HWND_DESKTOP) {
        return __gal_screen;
    }

    mainwin = ((PMAINWIN)hwnd)->pMainWin;
    ctrl = gui_Control (hwnd);
    if (ctrl && (ctrl->dwExStyle & WS_EX_CTRLASMAINWIN)) {
        return ctrl->surf;
    }

    return mainwin->surf;
}
#endif  /* defined _MGSCHEMA_COMPOSITING */

#if 0   /* deprecated code */
HDC GUIAPI GetClientDC (HWND hWnd)
{
    int i;
    PMAINWIN pWin, pMainWin;
    PDC pdc;

    MG_CHECK_RET (MG_IS_GRAPHICS_WINDOW(hWnd), HDC_INVALID);
    pWin = (PMAINWIN)hWnd;
    pMainWin = pWin->pMainWin;

    /* allocate an empty dc slot exclusively */
    LOCK (&dcslot);
    for (i = 0; i < DCSLOTNUMBER; i++) {
        if (!DCSlot[i].bInUse) {
            break;
        }
    }
    UNLOCK(&dcslot);

    if (i >= DCSLOTNUMBER)
        return HDC_SCREEN;

    pdc = DCSlot + i;

    pdc->bInUse   = TRUE;
    pdc->DataType = TYPE_HDC;
    /* since 5.0.0, use surface of secondary DC if possible */
    if (0 && pMainWin->secondaryDC && pWin->WinType == TYPE_CONTROL
            && !(pWin->dwExStyle & WS_EX_CTRLASMAINWIN)) {
        PDC pdc_secondary = dc_HDC2PDC (pMainWin->secondaryDC);
        pdc->DCType = TYPE_MEMDC;
        pdc->surface = pdc_secondary->surface;
    }
    else {
#ifdef _MGSCHEMA_COMPOSITING
        pdc->DCType   = TYPE_MEMDC;
        pdc->surface  = get_window_surface (hWnd);
#else
        pdc->DCType   = TYPE_GENDC;
        pdc->surface  = __gal_screen;
#endif
    }

    dc_InitDC (pdc, hWnd, FALSE);
    return (HDC)(pdc);
}
#endif  /* deprecated code */

HDC GUIAPI GetDCEx (HWND hWnd, BOOL bClient)
{
    int i;
    PDC pdc;

    MG_CHECK_RET (MG_IS_GRAPHICS_WINDOW(hWnd), HDC_INVALID);

    /* allocate an empty dc slot exclusively */
    LOCK (&dcslot);
    for (i = 0; i < DCSLOTNUMBER; i++) {
        if (!DCSlot[i].bInUse) {
            pdc = DCSlot + i;
            pdc->bInUse = TRUE;
            break;
        }
    }
    UNLOCK(&dcslot);

    if (i >= DCSLOTNUMBER)
        return HDC_SCREEN;

    pdc->DataType = TYPE_HDC;
#ifdef _MGSCHEMA_COMPOSITING
    pdc->DCType   = TYPE_MEMDC;
    pdc->surface  = get_window_surface (hWnd);
#else
    pdc->DCType   = TYPE_GENDC;
    pdc->surface  = __gal_screen;
#endif

    dc_InitDC (pdc, hWnd, bClient);
    return (HDC)(pdc);
}

/*
 * Function: void GUIAPI ReleaseDC(HDC hDC)
 *     This function release the specified DC.
 * Parameter:
 *     HDC hDC: The DC handle want to release.
 * Return:
 *     None.
 */
void GUIAPI ReleaseDC (HDC hDC)
{
    PMAINWIN pWin;
    PDC pdc;

    /* HDC_SCREEN and HDC_SCREEN_SYS is release by TerminateScreenDC ();*/
    if (hDC == HDC_SCREEN || hDC == HDC_SCREEN_SYS)
        return;

    pdc = dc_HDC2PDC(hDC);

    /* Only call SyncUpdateDC when releasing a DC.
       This will emilinate flickers effectively. */
    LOCK (&__mg_gdilock);
    GAL_SyncUpdate (pdc->surface);
    UNLOCK (&__mg_gdilock);

    pWin = (PMAINWIN)(pdc->hwnd);
    if (pWin && pWin->privCDC == hDC) {
        /* for private DC, we reset the clip region info. */
        /* houhh 20090113, for private dc with secondaryDC, pGCRInfo is NULL. */
#ifdef _MGSCHEMA_COMPOSITING
        RECT rc_surface, minimal;
        PCONTROL pCtrl;

        /* initialize the local and effective clipping regions */
        EmptyClipRgn (&pdc->lcrgn);
        MAKE_REGION_INFINITE(&pdc->lcrgn);
        EmptyClipRgn (&pdc->ecrgn);

        rc_surface.left     = 0;
        rc_surface.top      = 0;
        rc_surface.right    = pdc->surface->w;
        rc_surface.bottom   = pdc->surface->h;

        /* clipping region more with pdc->DevRC and rc_surface. */
        IntersectRect (&minimal, &pdc->DevRC, &rc_surface);
        SetClipRgn (&pdc->ecrgn, &rc_surface);
        IntersectClipRect (&pdc->lcrgn, &rc_surface);

        /* restrict control's effective region. */
        pCtrl = gui_Control (pdc->hwnd);
        if (pCtrl && !(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN)) {
            CLIPRGN ergn;
            InitClipRgn (&ergn, &__mg_FreeClipRectList);

            if (RestrictControlMemDCECRGNEx (&minimal, pCtrl, &ergn)) {
                ClipRgnIntersect (&pdc->ecrgn, &pdc->ecrgn, &ergn);
            }
            else {
                EmptyClipRgn (&pdc->ecrgn);
            }
            EmptyClipRgn (&ergn);
        }
        else {
            IntersectClipRect (&pdc->ecrgn, &minimal);
        }
#else   /* defined _MGSCHEMA_COMPOSITING */
        if (pdc->pGCRInfo) {
            RECT minimal;
            PCONTROL pCtrl;
            CLIPRGN ergn;

            LOCK (&pdc->pGCRInfo->lock);

            pdc->oldage = pdc->pGCRInfo->age;
            ClipRgnCopy (&pdc->ecrgn, &pdc->pGCRInfo->crgn);

            dc_CalculateDevRC4GenDC (pdc);

            minimal = pdc->DevRC;

            pCtrl = gui_Control (pdc->hwnd);
            if (pCtrl && !(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN)) {
                InitClipRgn (&ergn, &__mg_FreeClipRectList);
                if (RestrictControlECRGNEx (&minimal, pCtrl, &ergn)) {
                    ClipRgnIntersect (&pdc->ecrgn, &pdc->ecrgn, &ergn);
                    EmptyClipRgn (&ergn);
                }
                else {
                    EmptyClipRgn (&pdc->ecrgn);
                }
            }
            else {
                IntersectClipRect (&pdc->ecrgn, &minimal);
            }

            UNLOCK (&pdc->pGCRInfo->lock);
        }
#endif /* not defined _MGSCHEMA_COMPOSITING */
    }
    else {
        /* since 5.0.0, only free pixel format for dc from slots */
        if (pdc->alpha_pixel_format) {
            GAL_FreeFormat (pdc->alpha_pixel_format);
            pdc->alpha_pixel_format = NULL;
        }

        EmptyClipRgn (&pdc->lcrgn);
        EmptyClipRgn (&pdc->ecrgn);
#ifndef _MGSCHEMA_COMPOSITING
        pdc->pGCRInfo = NULL;
        pdc->oldage = 0;
#endif
        LOCK (&dcslot);
        pdc->bInUse = FALSE;
        UNLOCK(&dcslot);
    }
}

static BOOL InitSubDC (HDC hdcDest, HDC hdc, int off_x, int off_y,
        int width, int height)
{
    int parent_width,parent_height;
    PDC pdc;
    PDC pdc_parent;

    if(hdcDest == 0) return FALSE;

    pdc_parent = dc_HDC2PDC (hdc);
    if (pdc_parent == NULL || pdc_parent->DataType != TYPE_HDC
            || pdc_parent->DCType == TYPE_SCRDC)
        return FALSE;

    parent_width = RECTW (pdc_parent->DevRC);
    parent_height = RECTH (pdc_parent->DevRC);

    if (off_x < 0 && pdc_parent->DCType != TYPE_MEMDC) off_x = 0;
    else if(off_x > parent_width) off_x = parent_width;
    if (off_y < 0 && pdc_parent->DCType != TYPE_MEMDC) off_y = 0;
    else if(off_y > parent_height) off_y = parent_height;
    if (width < 0) width = 0;
    if (height < 0) height = 0;

    if (off_x + width > parent_width)
        width =  parent_width - off_x;
    if (off_y+height > parent_height)
        height = parent_height - off_y;

    pdc = (PDC)hdcDest;
    pdc->bIsClient    = FALSE;
    pdc->DevRC.left   = pdc_parent->DevRC.left + off_x;
    pdc->DevRC.top    = pdc_parent->DevRC.top + off_y;
    pdc->DevRC.right  = pdc->DevRC.left + width;
    pdc->DevRC.bottom = pdc->DevRC.top + height;
    pdc->surface      = pdc_parent->surface;
#ifndef _MGSCHEMA_COMPOSITING
    pdc->pGCRInfo     = pdc_parent->pGCRInfo;
#endif

    dc_InitMemDCFrom(pdc, pdc_parent);
    /* should set after InitMemDC.*/
    pdc->hwnd         = pdc_parent->hwnd;

    CopyRegion (&pdc->lcrgn, &pdc_parent->lcrgn);
    CopyRegion (&pdc->ecrgn, &pdc_parent->ecrgn);
    IntersectClipRect (&pdc->ecrgn, &pdc->DevRC);

    return TRUE;
}

/*
 * Function: HDC GUIAPI GtSubDC(HDC hdc,int off_x,int off_y,int width,int height);
 *     This function get the specified window's DC compatible with hdc.
 * Parameter:
 *      HDC hdc: The client DC of window. cannot be 0
 *      int off_x: the x-start pos of sub dc. if x<0  x=0. Device coordinate
 *      int off_y: the y-start pos of sub dc. if y<0 y=0. Device coordinate
 *      int width: the width of sub dc. if width>x+windows.width,width = window.width-x
 *      int height: the height of sub dc. if height>y+window.height,height=window.height-y
 */
HDC GUIAPI GetSubDC (HDC hdc, int off_x, int off_y, int width, int height)
{
    int i;
    int parent_width,parent_height;
    PDC pdc;
    PDC pdc_parent;

    pdc_parent = dc_HDC2PDC (hdc);
    if (pdc_parent == NULL || pdc_parent->DataType != TYPE_HDC
            || pdc_parent->DCType == TYPE_SCRDC)
        return HDC_INVALID;

    parent_width = RECTW (pdc_parent->DevRC);
    parent_height = RECTH (pdc_parent->DevRC);
    if ((off_x < 0 && pdc_parent->DCType != TYPE_MEMDC) ||
            off_x  >=  parent_width ||
            (off_y < 0 && pdc_parent->DCType != TYPE_MEMDC) ||
            off_y >= parent_height || width <= 0 || height <= 0)
        return HDC_INVALID;

    if (off_x + width > parent_width)
        width =  parent_width - off_x;
    if (off_y+height > parent_height)
        height = parent_height - off_y;

    pdc = NULL;
    LOCK (&dcslot);
    for (i = 0; i < DCSLOTNUMBER; i++) {
        if (!DCSlot[i].bInUse) {
            pdc = &DCSlot [i];
            pdc->bInUse   = TRUE;
            pdc->DataType = pdc_parent->DataType;
            pdc->DCType   = pdc_parent->DCType;
            break;
        }
    }
    UNLOCK (&dcslot);

    if (pdc == NULL)
        return HDC_INVALID;

    if (!InitSubDC((HDC)pdc, hdc, off_x, off_y, width, height)) {
        pdc->bInUse = FALSE;
        return HDC_INVALID;
    }
    return (HDC)pdc;
}

HDC GUIAPI CreatePrivateDC (HWND hwnd)
{
    PMAINWIN pWin, pMainWin;
    PDC pdc;

    MG_CHECK_RET (MG_IS_GRAPHICS_WINDOW(hwnd), HDC_INVALID);
    pWin = (PMAINWIN)hwnd;
    pMainWin = pWin->pMainWin;

    if (!(pdc = malloc (sizeof(DC))))
        return HDC_INVALID;

    pdc->bInUse   = TRUE;
    pdc->DataType = TYPE_HDC;
    /* since 5.0.0, use surface of secondary DC if possible */
    if (pMainWin->secondaryDC && !(pWin->WinType == TYPE_CONTROL
            && (pWin->dwExStyle & WS_EX_CTRLASMAINWIN))) {
        PDC pdc_secondary = dc_HDC2PDC (pMainWin->secondaryDC);
        pdc->DCType = TYPE_MEMDC;
        pdc->surface = pdc_secondary->surface;
    }
    else {
#ifdef _MGSCHEMA_COMPOSITING
        pdc->DCType   = TYPE_MEMDC;
        pdc->surface  = get_window_surface (hwnd);
#else
        pdc->DCType   = TYPE_GENDC;
        pdc->surface  = __gal_screen;
#endif
    }

    dc_InitDC (pdc, hwnd, FALSE);
    return (HDC)(pdc);
}

HDC GUIAPI CreatePrivateClientDC(HWND hwnd)
{
    PMAINWIN pWin, pMainWin;
    PDC pdc;

    MG_CHECK_RET (MG_IS_GRAPHICS_WINDOW(hwnd), HDC_INVALID);
    pWin = (PMAINWIN)hwnd;
    pMainWin = pWin->pMainWin;

    if (!(pdc = malloc (sizeof(DC))))
        return HDC_INVALID;

    pdc->bInUse   = TRUE;
    pdc->DataType = TYPE_HDC;

    /* since 5.0.0, use surface of secondary DC if possible */
    if (pMainWin->secondaryDC && !(pWin->WinType == TYPE_CONTROL
            && (pWin->dwExStyle & WS_EX_CTRLASMAINWIN))) {
        PDC pdc_secondary = dc_HDC2PDC (pMainWin->secondaryDC);
        pdc->DCType = TYPE_MEMDC;
        pdc->surface = pdc_secondary->surface;
    }
    else {
#ifdef _MGSCHEMA_COMPOSITING
        pdc->DCType   = TYPE_MEMDC;
        pdc->surface  = get_window_surface (hwnd);
#else
        pdc->DCType   = TYPE_GENDC;
        pdc->surface  = __gal_screen;
#endif
    }

    dc_InitDC (pdc, hwnd, TRUE);
    return (HDC)(pdc);
}

HDC GUIAPI CreatePrivateSubDC (HDC hdc, int off_x, int off_y,
        int width, int height)
{
    PDC pdc, pdc_parent;

    pdc_parent = dc_HDC2PDC (hdc);
    if (pdc_parent == NULL)
        return HDC_INVALID;

    if (!(pdc = malloc(sizeof(DC))))
        return HDC_INVALID;

    InitClipRgn (&pdc->lcrgn, &__mg_FreeClipRectList);
    MAKE_REGION_INFINITE(&pdc->lcrgn);
    InitClipRgn (&pdc->ecrgn, &__mg_FreeClipRectList);

    pdc->bInUse   = TRUE;
    pdc->DataType = TYPE_HDC;
    /* XXX: use the parent's type */
    pdc->DCType   = pdc_parent->DCType;

    if (!InitSubDC ((HDC)pdc, hdc, off_x, off_y, width, height)) {
        free (pdc);
        return HDC_INVALID;
    }

    return (HDC)pdc;
}

void GUIAPI DeletePrivateDC(HDC hdc)
{
    PDC pdc;

    pdc = (PDC)hdc;

    if (pdc->alpha_pixel_format)
        GAL_FreeFormat (pdc->alpha_pixel_format);

    EmptyClipRgn (&pdc->lcrgn);
    EmptyClipRgn (&pdc->ecrgn);

    free (pdc);
}

/* Since 5.0.0, returns HDC_INVALID if no private CDC */
HDC GUIAPI GetPrivateClientDC (HWND hwnd)
{
    PMAINWIN pWin = (PMAINWIN)hwnd;
    MG_CHECK_RET (MG_IS_GRAPHICS_WINDOW(hwnd), HDC_INVALID);

    if (pWin->privCDC)
        return pWin->privCDC;

    return HDC_INVALID;
}

HDC GUIAPI CreateSecondaryDC(HWND hwnd)
{
    PMAINWIN pWin;
    int width, height;
    HDC hdc, hdc_ref;
    PDC pdc;

    MG_CHECK_RET (MG_IS_MAIN_WINDOW(hwnd), HDC_INVALID);
    pWin = MG_GET_WINDOW_PTR (hwnd);

    width  = pWin->right - pWin->left;
    height = pWin->bottom - pWin->top;

    hdc_ref = GetDC (hwnd);

    if ((width <= 0) || ( height <= 0)) {
        hdc = CreateCompatibleDCEx (hdc_ref, 128, 128);
    }
    else {
        hdc = CreateCompatibleDCEx (hdc_ref, width, height);
    }

    if (hdc != HDC_INVALID) {
        pdc = dc_HDC2PDC (hdc);
        pdc->hwnd = hwnd;
    }

    ReleaseDC (hdc_ref);
    return hdc;
}

void __mg_delete_secondary_dc (PMAINWIN pMainWin)
{
    if (pMainWin->secondaryDC) {
        DeleteMemDC (pMainWin->secondaryDC);
        pMainWin->secondaryDC = 0;
    }
}

void GUIAPI DeleteSecondaryDC (HWND hwnd)
{
    PMAINWIN pWin = (PMAINWIN) hwnd;

    __mg_delete_secondary_dc (pWin);

    /* Since 5.0.0, update private CDC of controls */
    __mg_update_dc_on_secondary_dc_changed (pWin);
}

HDC GUIAPI SetSecondaryDC (HWND hwnd, HDC secondary_dc,
        ON_UPDATE_SECONDARYDC on_update_secondarydc)
{
    PMAINWIN pWin;
    HDC hdc;
    PDC pdc = dc_HDC2PDC(secondary_dc);

    if (secondary_dc == HDC_INVALID)
        return HDC_INVALID;

    MG_CHECK_RET (MG_IS_MAIN_WINDOW(hwnd), HDC_INVALID);
    pWin = MG_GET_WINDOW_PTR (hwnd);

    if (secondary_dc == HDC_SCREEN) {
        if (!pWin->secondaryDC) {
            return HDC_SCREEN;
        }
        else {
            hdc = pWin->secondaryDC;
            if (pWin->dwExStyle & WS_EX_AUTOSECONDARYDC) {
                DeleteSecondaryDC (hwnd);
                pWin->dwExStyle &= ~WS_EX_AUTOSECONDARYDC;
            }
            pWin->secondaryDC = 0;
            pWin->update_secdc = NULL;
            return hdc;
        }
    }

    if ((pWin->secondaryDC != secondary_dc) &&
            (pWin->dwExStyle & WS_EX_AUTOSECONDARYDC)) {
        DeleteSecondaryDC (hwnd);
        pWin->dwExStyle &= ~WS_EX_AUTOSECONDARYDC;
        hdc = HDC_SCREEN;
    }
    else {
        hdc = pWin->secondaryDC;
    }

    __mg_update_dc_on_secondary_dc_changed (pWin);

    pdc->hwnd = hwnd;
    pWin->secondaryDC = secondary_dc;
    pWin->update_secdc = on_update_secondarydc;

    return hdc;
}

/* Since 5.0.0, this function always returns the secondary DC
   of the main window contains the window. */
HDC GUIAPI GetSecondaryDC (HWND hwnd)
{
    PMAINWIN pWin;

    pWin = MG_GET_WINDOW_PTR (hwnd);
    if (pWin == NULL)
        return HDC_INVALID;

    if (MG_IS_MAIN_WINDOW(hwnd) && pWin->secondaryDC) {
        return pWin->secondaryDC;
    }
    else if (pWin->pMainWin->secondaryDC) {
        return pWin->pMainWin->secondaryDC;
        // deprecated since 5.0.0.
        // return GetDCInSecondarySurface (hwnd, FALSE);
    }

    return HDC_SCREEN;
}

#if 0   /* deprecated code */
HDC GUIAPI GetSecondaryClientDC (HWND hwnd)
{
    PMAINWIN pWin;
    pWin = MG_GET_WINDOW_PTR (hwnd);

    return get_effective_dc (pWin, TRUE);
}

void GUIAPI ReleaseSecondaryDC (HWND hwnd, HDC hdc)
{
    PMAINWIN pWin;

    pWin = MG_GET_WINDOW_PTR (hwnd);

    if (MG_IS_MAIN_WINDOW(hwnd) && pWin->secondaryDC == hdc)
        return;

    release_effective_dc (pWin, hdc);
}

/**
* Function: Release a secondary sub DC.
*
* secondary_subdc: The handle to the secondary sub DC.
* hwnd_child: The handle to the child.
*/
void ReleaseSecondarySubDC (HDC secondary_subdc)
{
    ReleaseDC (secondary_subdc);
}

#endif  /* deprecated code */

HDC GUIAPI GetDCInSecondarySurface (HWND hwnd, BOOL client)
{
    int i;
    PDC pdc = NULL, pdc_secondary;
    PCONTROL pCtrl;
    RECT minimal;
    PMAINWIN pMainWin;

    MG_CHECK_RET (MG_IS_GRAPHICS_WINDOW(hwnd), HDC_INVALID);
    pCtrl = (PCONTROL)hwnd;
    pMainWin = pCtrl->pMainWin;

    if (pMainWin->secondaryDC &&
            !(pCtrl->WinType == TYPE_CONTROL &&
                (pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN))) {
        pdc_secondary = dc_HDC2PDC (pMainWin->secondaryDC);
    }
    else {
        return GetDCEx (hwnd, client);
    }

    /* allocate an empty dc slot exclusively */
    LOCK (&dcslot);
    for (i = 0; i < DCSLOTNUMBER; i++) {
        if (!DCSlot[i].bInUse) {
            pdc = DCSlot + i;
            pdc->bInUse = TRUE;
            break;
        }
    }
    UNLOCK(&dcslot);

    if (i >= DCSLOTNUMBER) {
        _WRN_PRINTF ("No DC slot.\n");
        return HDC_INVALID;
    }

    pdc->DataType  = TYPE_HDC;
    pdc->DCType    = TYPE_MEMDC;
    pdc->hwnd      = hwnd;
    pdc->bIsClient = client;
    pdc->surface   = pdc_secondary->surface;

    dc_CalculateDevRC4MemDC (pdc);

    dc_InitMemDCFrom (pdc, pdc_secondary);
    pdc->hwnd = hwnd;
    pdc->bIsClient = client;

    if (!(pdc->pLogFont = GetWindowFont (hwnd)))
        pdc->pLogFont = GetSystemFont (SYSLOGFONT_WCHAR_DEF);

#ifndef _MGSCHEMA_COMPOSITING
    pdc->pGCRInfo = NULL;
    pdc->oldage = 0;
#endif

    InitClipRgn (&pdc->lcrgn, &__mg_FreeClipRectList);
    MAKE_REGION_INFINITE(&pdc->lcrgn);
    InitClipRgn (&pdc->ecrgn, &__mg_FreeClipRectList);
    CopyRegion (&pdc->ecrgn, &pdc_secondary->ecrgn);

    /* clipping region more with pdc->DevRC. */
    minimal = pdc->DevRC;

    /* restrict control's effective region. */
    if (pCtrl && pCtrl->WinType == TYPE_CONTROL &&
            !(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN)) {
        CLIPRGN ergn;

        InitClipRgn (&ergn, &__mg_FreeClipRectList);

        if (RestrictControlMemDCECRGNEx (&minimal, pCtrl, &ergn)) {
            ClipRgnIntersect (&pdc->ecrgn, &pdc->ecrgn, &ergn);
            EmptyClipRgn (&ergn);
        }
        else {
            EmptyClipRgn (&pdc->ecrgn);
        }
        EmptyClipRgn (&ergn);
    }
    else {
        IntersectClipRect (&pdc->ecrgn, &minimal);
    }

    return (HDC)pdc;
}

HDC GUIAPI GetEffectiveCDC (HWND hwnd)
{
    PMAINWIN pWin = (PMAINWIN)hwnd;
    MG_CHECK_RET (MG_IS_GRAPHICS_WINDOW(hwnd), HDC_INVALID);

    if (pWin->privCDC)
        return pWin->privCDC;

    return GetDCInSecondarySurface (hwnd, TRUE);
}

BOOL dc_GenerateMemDCECRgn (PDC pdc, BOOL fForce)
{
    PCONTROL pCtrl;
    RECT minimal;
    PCLIPRECT pcr;

    /* is invisible? */
    if ((!fForce) && (!dc_IsVisible (pdc)))
        return FALSE;

    if (fForce) {
        /* copy local clipping region to effective clipping region. */
        ClipRgnCopy (&pdc->ecrgn, &pdc->lcrgn);

        /* transfer device coordinates to screen coordinates. */
        pcr = pdc->ecrgn.head;
        while (pcr) {
            coor_DP2SP (pdc, &pcr->rc.left, &pcr->rc.top);
            coor_DP2SP (pdc, &pcr->rc.right, &pcr->rc.bottom);

            pcr = pcr->next;
        }

        if (pdc->lcrgn.head) {
            coor_DP2SP (pdc, &pdc->ecrgn.rcBound.left,
                    &pdc->ecrgn.rcBound.top);
            coor_DP2SP (pdc, &pdc->ecrgn.rcBound.right,
                    &pdc->ecrgn.rcBound.bottom);
        }

        /* clipping region more with pdc->DevRC.  */
        minimal = pdc->DevRC;
        if (pdc->lcrgn.type == NULLREGION)
            EmptyClipRgn(&pdc->ecrgn);

        /* restrict control's effective region. */
        pCtrl = gui_Control (pdc->hwnd);
        if (pCtrl && !(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN)) {
            CLIPRGN ergn;
            InitClipRgn (&ergn, &__mg_FreeClipRectList);

            if (RestrictControlMemDCECRGNEx (&minimal, pCtrl, &ergn)) {
                ClipRgnIntersect (&pdc->ecrgn, &pdc->ecrgn, &ergn);
                EmptyClipRgn (&ergn);
            }
            else {
                EmptyClipRgn (&pdc->ecrgn);
            }
            EmptyClipRgn (&ergn);
        }
        else {
            IntersectClipRect (&pdc->ecrgn, &minimal);
        }
    }

    return TRUE;
}

/* LockDC/UnlockDC to get direct access to the pixels in a DC. */
Uint8* GUIAPI LockDC (HDC hdc, const RECT* rw_rc, int* width, int* height,
                int* pitch)
{
    PDC pdc;
    Uint8* pixels;

    if (!(pdc = __mg_check_ecrgn (hdc)))
        return NULL;

    if (rw_rc) {
        pdc->rc_output = *rw_rc;

        /* Transfer device to device to screen here. */
        coor_DP2SP (pdc, &pdc->rc_output.left, &pdc->rc_output.top);
        coor_DP2SP (pdc, &pdc->rc_output.right, &pdc->rc_output.bottom);
        if (!IntersectRect (&pdc->rc_output, &pdc->rc_output, &pdc->DevRC))
            goto fail;
    }
    else
        pdc->rc_output = pdc->DevRC;

    BLOCK_DRAW_SEM (pdc);

    /* LockDC return the pointer to the upper-left corner of the requested
     * rectangle*/
    /*
    if (!IntersectRect (&pdc->rc_output, &pdc->rc_output, &pdc->ecrgn.rcBound))
        goto fail1;
    */
    LOCK (&__mg_gdilock);
    if (!dc_IsMemDC (pdc))
        kernel_ShowCursorForGDI (FALSE, pdc);

    if (width) *width = RECTW(pdc->rc_output);
    if (height) *height = RECTH(pdc->rc_output);
    if (pitch) *pitch = pdc->surface->pitch;

    pixels = pdc->surface->pixels;
    pixels += pdc->surface->pitch * pdc->rc_output.top;
    pixels += pdc->surface->format->BytesPerPixel * pdc->rc_output.left;
    return pixels;

    UNBLOCK_DRAW_SEM (pdc);
fail:
    if (dc_IsGeneralDC (pdc))
        UNLOCK_GCRINFO (pdc);
    return NULL;
}

static void DefaultDirectDrawFunc (HDC hdc, Uint8 * pixels,
        int pitch, int bytesPerPixel, const RECT* rc, void* context)
{
    GAL_Rect gal_rect;
    PDC pdc = dc_HDC2PDC(hdc);
    pdc->cur_pixel = pdc->brushcolor;
    gal_rect.x = rc->left;
    gal_rect.y = rc->top;
    gal_rect.w = rc->right - rc->left;
    gal_rect.h = rc->bottom - rc->top;

    pdc->surface->clip_rect = gal_rect;
    GAL_FillRect (pdc->surface, &gal_rect, pdc->cur_pixel);
}

BOOL GUIAPI LockDCEx (HDC hdc, const PCLIPRGN region,
        void* context, CB_DIRECT_DRAW_RECT cb)
{
    PDC pdc;
    Uint8* pixels;
    PCLIPRECT  cliprc_src, cliprc_pdc;
    int pitch, bpp;

    if (!region->head || !(pdc = __mg_check_ecrgn (hdc))) {
        return FALSE;
    }

    /* check the invalid rectangle */
    pdc->rc_output = region->rcBound;

    /* Transfer device to device to screen here. */
    coor_DP2SP (pdc, &pdc->rc_output.left, &pdc->rc_output.top);
    coor_DP2SP (pdc, &pdc->rc_output.right, &pdc->rc_output.bottom);
    if (!IntersectRect (&pdc->rc_output, &pdc->rc_output, &pdc->DevRC)) {
        if (dc_IsGeneralDC (pdc)) UNLOCK_GCRINFO (pdc);
        return FALSE;
    }

    /* get callback function */
    if (cb == NULL)
        cb = DefaultDirectDrawFunc;

    BLOCK_DRAW_SEM (pdc);

    LOCK (&__mg_gdilock);

    if (!dc_IsMemDC (pdc))
        kernel_ShowCursorForGDI (FALSE, pdc);

    pitch = pdc->surface->pitch;
    bpp = pdc->surface->format->BytesPerPixel;

    cliprc_src = region->head;
    while (cliprc_src != NULL) {
        RECT rc_output;
        RECT dest_rc;

        rc_output = cliprc_src->rc;
        coor_DP2SP (pdc, &rc_output.left, &rc_output.top);
        coor_DP2SP (pdc, &rc_output.right, &rc_output.bottom);
        if (!IntersectRect (&rc_output, &rc_output, &pdc->DevRC)) {
            cliprc_src = cliprc_src->next;
            continue;
        }

        cliprc_pdc = pdc->ecrgn.head;
        while (cliprc_pdc != NULL) {
            if (!IntersectRect (&dest_rc, &rc_output, &cliprc_pdc->rc)) {
                cliprc_pdc = cliprc_pdc->next;
                continue;
            }

            pixels = (Uint8 *)pdc->surface->pixels + pitch * dest_rc.top;

            /* direct draw invalid rectangle */
            cb (hdc, pixels, pitch, bpp, &dest_rc, context);

            cliprc_pdc = cliprc_pdc->next;
        }

        cliprc_src = cliprc_src->next;
    }
    return TRUE;
}

void GUIAPI UnlockDC (HDC hdc)
{
    PDC pdc = dc_HDC2PDC (hdc);

    /* houhh 20090622, in multi thread if another thread
     * set window not visible, this will be error and dead lock.*/
    if (!dc_IsVisible(pdc))
        return;

    if (!dc_IsMemDC (pdc)) {
        kernel_ShowCursorForGDI (TRUE, pdc);
    }
#ifdef _MGSCHEMA_COMPOSITING
    else if (pdc->surface->dirty_info) {
        GAL_UpdateRect (pdc->surface,
                pdc->rc_output.left, pdc->rc_output.top,
                RECTW(pdc->rc_output), RECTH(pdc->rc_output));
    }
#endif /* defined _MGSCHEMA_COMPOSITING */

    UNLOCK (&__mg_gdilock);

    UNBLOCK_DRAW_SEM (pdc);

    if (dc_IsGeneralDC (pdc)) {
        UNLOCK_GCRINFO (pdc);
    }
}

/******************************* Memory DC ***********************************/
HDC GUIAPI CreateCompatibleDCEx (HDC hdc, int width, int height)
{
    PDC pdc;
    PDC pmem_dc = NULL;
    GAL_Surface* surface;
    DWORD flags;

    pdc = dc_HDC2PDC (hdc);

    if (!(pmem_dc = malloc (sizeof(DC))))
        return HDC_INVALID;

    if (width <= 0 || height <= 0) {
        width = RECTW (pdc->DevRC);
        height = RECTH (pdc->DevRC);
    }

    if ((pdc->surface->flags & GAL_HWSURFACE) == GAL_HWSURFACE)
        flags = GAL_HWSURFACE;
    else
        flags = GAL_SWSURFACE;

    LOCK (&__mg_gdilock);
    surface = GAL_CreateRGBSurface (flags,
                    width, height,
                    pdc->surface->format->BitsPerPixel,
                    pdc->surface->format->Rmask,
                    pdc->surface->format->Gmask,
                    pdc->surface->format->Bmask,
                    pdc->surface->format->Amask);
    UNLOCK (&__mg_gdilock);

    if (!surface) {
        free (pmem_dc);
        return HDC_INVALID;
    }

    /* Set surface attributes */
    if (pdc->surface->format->BitsPerPixel <= 8) {
        GAL_SetPalette (surface, GAL_LOGPAL,
            (GAL_Color*) pdc->surface->format->palette->colors,
            0, 1<<pdc->surface->format->BitsPerPixel);
    }

    if (pdc->surface->flags & GAL_SRCALPHA) {
        GAL_SetAlpha (surface, GAL_SRCALPHA,
                    pdc->surface->format->alpha);
    }

    if (pdc->surface->flags & GAL_SRCCOLORKEY) {
        GAL_SetColorKey (surface, GAL_SRCCOLORKEY,
                pdc->surface->format->colorkey);
    }

    pmem_dc->DataType = TYPE_HDC;
    pmem_dc->DCType   = TYPE_MEMDC;
    pmem_dc->bInUse   = TRUE;
    pmem_dc->surface  = surface;

    pmem_dc->DevRC.left   = 0;
    pmem_dc->DevRC.top    = 0;
    pmem_dc->DevRC.right  = width;
    pmem_dc->DevRC.bottom = height;

    dc_InitMemDCFrom (pmem_dc, pdc);

    /* clip region info */
#ifndef _MGSCHEMA_COMPOSITING
    pmem_dc->pGCRInfo = NULL;
    pmem_dc->oldage = 0;
#endif
    InitClipRgn (&pmem_dc->lcrgn, &__mg_FreeClipRectList);
    MAKE_REGION_INFINITE(&pmem_dc->lcrgn);
    InitClipRgn (&pmem_dc->ecrgn, &__mg_FreeClipRectList);
    SetClipRgn (&pmem_dc->ecrgn, &pmem_dc->DevRC);
    IntersectClipRect(&pmem_dc->lcrgn, &pmem_dc->DevRC);

    return (HDC)pmem_dc;
}

BOOL GUIAPI IsCompatibleDC (HDC hdc1, HDC hdc2)
{
    PDC pdc1, pdc2;

    pdc1 = dc_HDC2PDC (hdc1);
    pdc2 = dc_HDC2PDC (hdc2);

    if (pdc1 == NULL || pdc2 == NULL)
        return FALSE;

    if ((pdc1->surface->format->Rmask == pdc2->surface->format->Rmask)
            && (pdc1->surface->format->Gmask == pdc2->surface->format->Gmask)
            && (pdc1->surface->format->Bmask == pdc2->surface->format->Bmask)
            && (pdc1->surface->format->Amask == pdc2->surface->format->Amask))
        return TRUE;

    return FALSE;
}

HDC GUIAPI CreateMemDCEx (int width, int height, int depth, DWORD flags,
        Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask,
        void* bits, int pitch)
{
    PDC pmem_dc = NULL;
    GAL_Surface* surface;

    if (!(pmem_dc = malloc (sizeof(DC))))
        return HDC_INVALID;

    LOCK (&__mg_gdilock);
    if (bits) {
        surface = GAL_CreateRGBSurfaceFrom (bits, width, height, depth, pitch,
                Rmask, Gmask, Bmask, Amask);
    } else {
        surface = GAL_CreateRGBSurface (flags, width, height, depth,
                Rmask, Gmask, Bmask, Amask);
    }
    UNLOCK (&__mg_gdilock);

    if (!surface) {
        free (pmem_dc);
        return HDC_INVALID;
    }

    if (bits) {
        if (Amask) {
            surface->flags |= GAL_SRCPIXELALPHA;
        }
    }

    pmem_dc->DataType = TYPE_HDC;
    pmem_dc->DCType   = TYPE_MEMDC;
    pmem_dc->bInUse   = TRUE;
    pmem_dc->surface  = surface;

    dc_InitDC (pmem_dc, HWND_NULL, FALSE);

    InitClipRgn (&pmem_dc->lcrgn, &__mg_FreeClipRectList);
    MAKE_REGION_INFINITE(&pmem_dc->lcrgn);
    InitClipRgn (&pmem_dc->ecrgn, &__mg_FreeClipRectList);
#ifndef _MGSCHEMA_COMPOSITING
    pmem_dc->pGCRInfo = NULL;
    pmem_dc->oldage = 0;
#endif

    pmem_dc->DevRC.left = 0;
    pmem_dc->DevRC.top  = 0;
    pmem_dc->DevRC.right = width;
    pmem_dc->DevRC.bottom = height;

    SetClipRgn (&pmem_dc->ecrgn, &pmem_dc->DevRC);
    IntersectClipRect(&pmem_dc->lcrgn, &pmem_dc->DevRC);

    return (HDC)pmem_dc;
}

/* Since 5.0.0 */
HDC CreateMemDCFromSurface (GAL_Surface* surface)
{
    PDC pmem_dc = NULL;

    if (surface == NULL)
        return HDC_INVALID;

    if (!(pmem_dc = malloc (sizeof(DC))))
        return HDC_INVALID;

    pmem_dc->DataType = TYPE_HDC;
    pmem_dc->DCType   = TYPE_MEMDC;
    pmem_dc->bInUse   = TRUE;
    pmem_dc->surface  = surface;

    dc_InitDC (pmem_dc, HWND_NULL, FALSE);

    InitClipRgn (&pmem_dc->lcrgn, &__mg_FreeClipRectList);
    MAKE_REGION_INFINITE(&pmem_dc->lcrgn);
    InitClipRgn (&pmem_dc->ecrgn, &__mg_FreeClipRectList);

#ifndef _MGSCHEMA_COMPOSITING
    pmem_dc->pGCRInfo = NULL;
    pmem_dc->oldage = 0;
#endif

    pmem_dc->DevRC.left = 0;
    pmem_dc->DevRC.top  = 0;
    pmem_dc->DevRC.right = surface->w;
    pmem_dc->DevRC.bottom = surface->h;

    SetClipRgn (&pmem_dc->ecrgn, &pmem_dc->DevRC);
    IntersectClipRect (&pmem_dc->lcrgn, &pmem_dc->DevRC);

    return (HDC)pmem_dc;
}

struct GAL_Surface* GetSurfaceFromDC (HDC hdc)
{
    PDC pdc;
    pdc = dc_HDC2PDC (hdc);
    return pdc->surface;
}

HDC GUIAPI CreateSubMemDC (HDC parent, int off_x, int off_y,
                int width, int height, BOOL comp_to_parent)
{
    PDC pdc_parent;
    PDC pdc_sub;
    GAL_Surface* surface;
    BYTE* sub_pixels;
    int parent_width;
    int parent_height;

    pdc_parent = dc_HDC2PDC (parent);
    if (pdc_parent->DCType != TYPE_MEMDC)
        return HDC_INVALID;

    parent_width  = RECTW (pdc_parent->DevRC);
    parent_height = RECTH (pdc_parent->DevRC);

    if (off_x < 0)  off_x = 0;
    else if(off_x > parent_width)  off_x = parent_width;
    if (off_y < 0)  off_y = 0;
    else if(off_y > parent_height) off_y = parent_height;
    if (width < 0)  width = 0;
    if (height < 0) height = 0;


    if (off_x + width > pdc_parent->DevRC.right)
        width = pdc_parent->DevRC.right - off_x;
    if (off_y + height > pdc_parent->DevRC.bottom)
        height = pdc_parent->DevRC.bottom - off_y;

    if (!(pdc_sub = malloc (sizeof(DC))))
        return HDC_INVALID;

    sub_pixels = (BYTE*)(pdc_parent->surface->pixels) +
                pdc_parent->surface->pitch * off_y +
                pdc_parent->surface->format->BytesPerPixel * off_x;

    surface = GAL_CreateRGBSurfaceFrom (sub_pixels,
                    width, height, pdc_parent->surface->format->BitsPerPixel,
                    pdc_parent->surface->pitch,
                    pdc_parent->surface->format->Rmask,
                    pdc_parent->surface->format->Gmask,
                    pdc_parent->surface->format->Bmask,
                    pdc_parent->surface->format->Amask);

    if (!surface) {
        free (pdc_sub);
        return HDC_INVALID;
    }

    pdc_sub->DataType = TYPE_HDC;
    pdc_sub->DCType   = TYPE_MEMDC;
    pdc_sub->bInUse   = TRUE;
    pdc_sub->surface  = surface;

    pdc_sub->DevRC.left = 0;
    pdc_sub->DevRC.top  = 0;
    pdc_sub->DevRC.right = width;
    pdc_sub->DevRC.bottom = height;

    if (comp_to_parent) {
        dc_InitMemDCFrom (pdc_sub, pdc_parent);

        /* Set surface attributes */
        if (pdc_parent->surface->format->BitsPerPixel <= 8) {
            GAL_SetPalette (surface, GAL_LOGPAL,
                        pdc_parent->surface->format->palette->colors,
                        0, 1<<pdc_parent->surface->format->BitsPerPixel);
        }

        if (pdc_parent->surface->flags & GAL_SRCALPHA) {
            GAL_SetAlpha (surface, GAL_SRCALPHA,
                        pdc_parent->surface->format->alpha);
        }

        if (pdc_parent->surface->flags & GAL_SRCCOLORKEY) {
            GAL_SetColorKey (surface, GAL_SRCCOLORKEY,
                    pdc_parent->surface->format->colorkey);
        }
    }
    else {
        dc_InitDC (pdc_sub, HWND_NULL, FALSE);
    }

#ifndef _MGSCHEMA_COMPOSITING
    pdc_sub->pGCRInfo = NULL;
    pdc_sub->oldage = 0;
#endif

    InitClipRgn (&pdc_sub->lcrgn, &__mg_FreeClipRectList);
    MAKE_REGION_INFINITE(&pdc_sub->lcrgn);
    InitClipRgn (&pdc_sub->ecrgn, &__mg_FreeClipRectList);

    if (comp_to_parent) {
        CopyRegion (&pdc_sub->ecrgn, &pdc_parent->ecrgn);
        if (off_x || off_y)
            OffsetRegion (&pdc_sub->ecrgn, -off_x, -off_y);
        IntersectClipRect (&pdc_sub->ecrgn, &pdc_sub->DevRC);
    }
    else {
        SetClipRgn (&pdc_sub->ecrgn, &pdc_sub->DevRC);
    }

    return (HDC)pdc_sub;
}

HDC GUIAPI CreateMemDCFromBitmap (HDC hdc, const BITMAP* bmp)
{
    PDC pdc, pmem_dc = NULL;
    GAL_Surface* surface;
    Uint32 Rmask = 0, Gmask = 0, Bmask = 0, Amask = 0;

    pdc = dc_HDC2PDC (hdc);

    Rmask = pdc->surface->format->Rmask;
    Gmask = pdc->surface->format->Gmask;
    Bmask = pdc->surface->format->Bmask;
    Amask = pdc->surface->format->Amask;

    if (!(pmem_dc = malloc (sizeof(DC))))
        return HDC_INVALID;

    surface = GAL_CreateRGBSurfaceFrom (bmp->bmBits,
                    bmp->bmWidth, bmp->bmHeight, bmp->bmBitsPerPixel,
                    bmp->bmPitch, Rmask, Gmask, Bmask, Amask);

    if (!surface) {
        free (pmem_dc);
        return HDC_INVALID;
    }

    /* Set surface attributes */
    if (bmp->bmBitsPerPixel <= 8) {
        GAL_SetPalette (surface, GAL_LOGPAL,
                        pdc->surface->format->palette->colors, 0,
                        1<<bmp->bmBitsPerPixel);
    }

    if (bmp->bmType & BMP_TYPE_ALPHACHANNEL) {
        GAL_SetAlpha (surface, GAL_SRCALPHA, bmp->bmAlpha);
    }

    if (bmp->bmType & BMP_TYPE_COLORKEY) {
        GAL_SetColorKey (surface, GAL_SRCCOLORKEY, bmp->bmColorKey);
    }

    pmem_dc->DataType = TYPE_HDC;
    pmem_dc->DCType   = TYPE_MEMDC;
    pmem_dc->bInUse   = TRUE;
    pmem_dc->surface  = surface;

    dc_InitDC (pmem_dc, HWND_NULL, FALSE);

    InitClipRgn (&pmem_dc->lcrgn, &__mg_FreeClipRectList);
    MAKE_REGION_INFINITE(&pmem_dc->lcrgn);
    InitClipRgn (&pmem_dc->ecrgn, &__mg_FreeClipRectList);

#ifndef _MGSCHEMA_COMPOSITING
    pmem_dc->pGCRInfo = NULL;
    pmem_dc->oldage = 0;
#endif

    pmem_dc->DevRC.left = 0;
    pmem_dc->DevRC.top  = 0;
    pmem_dc->DevRC.right = bmp->bmWidth;
    pmem_dc->DevRC.bottom = bmp->bmHeight;

    SetClipRgn (&pmem_dc->ecrgn, &pmem_dc->DevRC);
    return (HDC)pmem_dc;
}

HDC GUIAPI CreateMemDCFromMyBitmap (const MYBITMAP* my_bmp, const RGB* pal)
{
    PDC pmem_dc;
    GAL_Surface* surface;
    Uint32 Rmask = 0, Gmask = 0, Bmask = 0, Amask = 0;

    if (my_bmp->flags & MYBMP_FLOW_UP || my_bmp->depth < 8)
        return HDC_INVALID;

    switch (my_bmp->depth) {
        case 8:
            break;

        case 24:
            if (my_bmp->flags & MYBMP_ALPHA) {
                Amask = 0x0000003F;
                if ((my_bmp->flags & MYBMP_TYPE_MASK) == MYBMP_TYPE_RGB) {
                    Rmask = 0x00FC0000;
                    Gmask = 0x0003F000;
                    Bmask = 0x00000FC0;
                }
                else {
                    Rmask = 0x00000FC0;
                    Gmask = 0x0003F000;
                    Bmask = 0x00FC0000;
                }
            }
            else {
                Amask = 0x00000000;
                if ((my_bmp->flags & MYBMP_TYPE_MASK) == MYBMP_TYPE_RGB) {
                    Rmask = 0x000000FF;
                    Gmask = 0x0000FF00;
                    Bmask = 0x00FF0000;
                }
                else {
                    Rmask = 0x00FF0000;
                    Gmask = 0x0000FF00;
                    Bmask = 0x000000FF;
                }
            }
            break;

        case 32:
            if ((my_bmp->flags & MYBMP_TYPE_MASK) == MYBMP_TYPE_RGB) {
                Rmask = 0x00FF0000;
                Gmask = 0x0000FF00;
                Bmask = 0x000000FF;
            }
            else {
                Rmask = 0x000000FF;
                Gmask = 0x0000FF00;
                Bmask = 0x00FF0000;
            }
            if (my_bmp->flags & MYBMP_ALPHA)
                Amask = 0xFF000000;
            else
                Amask = 0x00000000;
            break;

        default:
            return HDC_INVALID;
    }

    if (!(pmem_dc = malloc (sizeof(DC))))
        return HDC_INVALID;

    surface = GAL_CreateRGBSurfaceFrom (my_bmp->bits,
                    my_bmp->w, my_bmp->h, my_bmp->depth, my_bmp->pitch,
                    Rmask, Gmask, Bmask, Amask);

    if (!surface) {
        free (pmem_dc);
        return HDC_INVALID;
    }

    /* Set surface attributes */
    if (my_bmp->depth <= 8) {
        GAL_SetPalette (surface, GAL_LOGPAL, (GAL_Color*) pal, 0,
                1<<my_bmp->depth);
    }

    if (my_bmp->flags & MYBMP_ALPHACHANNEL) {
        GAL_SetAlpha (surface, GAL_SRCALPHA, my_bmp->alpha);
    }

    if (my_bmp->flags & MYBMP_TRANSPARENT) {
        GAL_SetColorKey (surface, GAL_SRCCOLORKEY,
                my_bmp->transparent);
    }

    pmem_dc->DataType = TYPE_HDC;
    pmem_dc->DCType   = TYPE_MEMDC;
    pmem_dc->bInUse   = TRUE;
    pmem_dc->surface  = surface;

    dc_InitDC (pmem_dc, HWND_NULL, FALSE);

    InitClipRgn (&pmem_dc->lcrgn, &__mg_FreeClipRectList);
    MAKE_REGION_INFINITE(&pmem_dc->lcrgn);
    InitClipRgn (&pmem_dc->ecrgn, &__mg_FreeClipRectList);

#ifndef _MGSCHEMA_COMPOSITING
    pmem_dc->pGCRInfo = NULL;
    pmem_dc->oldage = 0;
#endif

    pmem_dc->DevRC.left = 0;
    pmem_dc->DevRC.top  = 0;
    pmem_dc->DevRC.right = my_bmp->w;
    pmem_dc->DevRC.bottom = my_bmp->h;

    SetClipRgn (&pmem_dc->ecrgn, &pmem_dc->DevRC);
    return (HDC)pmem_dc;
}

BOOL GUIAPI ConvertMemDC (HDC mem_dc, HDC ref_dc, DWORD flags)
{
    PDC pmem_dc, pref_dc;
    GAL_Surface* new_surface;

    pmem_dc = dc_HDC2PDC (mem_dc);
    pref_dc = dc_HDC2PDC (ref_dc);

    new_surface = GAL_ConvertSurface (pmem_dc->surface,
                pref_dc->surface->format, flags);
    if (!new_surface)
        return FALSE;

    GAL_FreeSurface (pmem_dc->surface);
    pmem_dc->surface = new_surface;
    return TRUE;
}

BOOL GUIAPI SetMemDCAlpha (HDC mem_dc, DWORD flags, Uint8 alpha)
{
    BOOL ret;
    DWORD tmp_flag = flags;
    PDC pmem_dc = dc_HDC2PDC (mem_dc);

    ret = !GAL_SetAlpha (pmem_dc->surface, flags, alpha);

    if ((tmp_flag & MEMDC_FLAG_SRCALPHA)
            && (tmp_flag & MEMDC_FLAG_SRCPIXELALPHA))
        ret = !GAL_SetAlpha (pmem_dc->surface,
                tmp_flag & (~MEMDC_FLAG_SRCALPHA), alpha);

    return ret;
}

BOOL GUIAPI SetMemDCColorKey (HDC mem_dc, DWORD flags, Uint32 color_key)
{
    PDC pmem_dc = dc_HDC2PDC (mem_dc);

    return !GAL_SetColorKey (pmem_dc->surface, flags, color_key);
}

void GUIAPI DeleteMemDC (HDC hdc)
{
    PDC pmem_dc;

    pmem_dc = dc_HDC2PDC(hdc);

    GAL_FreeSurface (pmem_dc->surface);

    if (pmem_dc->alpha_pixel_format) {
        GAL_FreeFormat (pmem_dc->alpha_pixel_format);
    }

    EmptyClipRgn (&pmem_dc->lcrgn);
    EmptyClipRgn (&pmem_dc->ecrgn);

    free (pmem_dc);
}

HDC GUIAPI InitSlaveScreenEx (const char* name, const char* mode, int dpi)
{
    PDC pmem_dc = NULL;
    GAL_Surface* surface = NULL;

    /* create slavescreen memory dc */
    if (!(pmem_dc = malloc (sizeof(DC))))
        return HDC_INVALID;

    if ((surface = gal_SlaveVideoInit(name, mode, dpi))) {
        dc_InitScreenDC (pmem_dc, surface);
        return (HDC)pmem_dc;
    }
    else {
        free (pmem_dc);
        _WRN_PRINTF ("Can not init the slave screen: %s (%s)\n",
                        name, mode);
        return HDC_INVALID;
    }
}

void TerminateSlaveScreen (HDC hdc)
{
    PDC pmem_dc;

    if (hdc == HDC_INVALID)
        return;

    pmem_dc = dc_HDC2PDC (hdc);

    if (pmem_dc->DataType != TYPE_HDC || pmem_dc->DCType != TYPE_SCRDC)
        return;

    if (pmem_dc->alpha_pixel_format)
        GAL_FreeFormat (pmem_dc->alpha_pixel_format);

    EmptyClipRgn (&pmem_dc->ecrgn);
    gal_SlaveVideoQuit (pmem_dc->surface);

    free (pmem_dc);
}

HWND GUIAPI WindowFromDC (HDC hdc)
{
    PDC pdc;

    if (hdc == HDC_INVALID || hdc == HDC_SCREEN || hdc == HDC_SCREEN_SYS)
        return HWND_NULL;

    pdc = dc_HDC2PDC (hdc);

    if (pdc->DCType == TYPE_GENDC) {
        return pdc->hwnd;
    }

    return HWND_NULL;
}

typedef struct _DCSTATE
{
    GAL_Color bkcolor, pencolor, brushcolor, textcolor;
    /* bkmode, tabstop, cExtra, wExtra, alExtra, blExtra, mapmode, ta_flags, bidi_flags */
    char attrs_g1 [sizeof(int)*9];

#ifdef _MGHAVE_ADV_2DAPI
    /*
     * int: pen_type, pen_cap_style, pen_join_style, pen_width, brush_type;
     * POINT: brush_orig;
     * void*: brush_tile, brush_stipple;
     * int: dash_offset;
     * void* dash_list;
     * size_t: dash_list_len.
     */
    char attrs_adv [(sizeof(int)*7) + sizeof(POINT) + (sizeof (void*)*3)];
#endif

    /*
     * void*: pLogFont;
     * POINT: CurPenPos, CurTextPos;
     * POINT: ViewOrig, ViewExtent, WindowOrig, WindowExtent;
     * int rop.
     */
    char attrs_g2 [sizeof (void*) + sizeof(POINT) * 6 + sizeof (int)];

    CLIPRGN lcrgn;

    GAL_Palette pal;

    CB_BITMAP_SCALER_FUNC bitmap_scaler;
    /* DK[23/01/10]: Add for save user composition operation function */
    CB_COMP_SETPIXEL user_set_pixel;
    CB_COMP_SETHLINE user_set_hline;
    CB_COMP_PUTHLINE user_put_hline;
    /* DK[23/01/10]: Add for save user composition context */
    void * user_comp_ctxt;

    struct _DCSTATE *next, *prev;
} DCSTATE;

static DCSTATE* dc_state_stack = NULL;
static int nr_dc_states = 0;

int GUIAPI SaveDC (HDC hdc)
{
    DCSTATE* dc_state;
    PDC pdc;

    dc_state = calloc (1, sizeof (DCSTATE));
    if (dc_state == NULL)
        return 0;

    dc_state->next = NULL;
    dc_state->prev = NULL;

    dc_state->pal.ncolors = 0;
    dc_state->pal.colors = NULL;

    pdc = dc_HDC2PDC (hdc);
    LOCK (&__mg_gdilock);

    InitClipRgn (&dc_state->lcrgn, &__mg_FreeClipRectList);

    if (pdc->surface->format->palette) {
        dc_state->pal.ncolors = pdc->surface->format->palette->ncolors;
        dc_state->pal.colors = calloc (dc_state->pal.ncolors,
                sizeof (GAL_Color));

        if (dc_state->pal.colors == NULL)
            goto fail;
        memcpy (dc_state->pal.colors, pdc->surface->format->palette->colors,
                sizeof (GAL_Color) * dc_state->pal.ncolors);
    }

    if (!ClipRgnCopy (&dc_state->lcrgn, &pdc->lcrgn))
        goto fail;

    GAL_GetRGBA (pdc->bkcolor, pdc->surface->format,
                &dc_state->bkcolor.r, &dc_state->bkcolor.g,
                &dc_state->bkcolor.b, &dc_state->bkcolor.a);
    GAL_GetRGBA (pdc->pencolor, pdc->surface->format,
                &dc_state->pencolor.r, &dc_state->pencolor.g,
                &dc_state->pencolor.b, &dc_state->pencolor.a);
    GAL_GetRGBA (pdc->brushcolor, pdc->surface->format,
                &dc_state->brushcolor.r, &dc_state->brushcolor.g,
                &dc_state->brushcolor.b, &dc_state->brushcolor.a);
    GAL_GetRGBA (pdc->textcolor, pdc->surface->format,
                &dc_state->textcolor.r, &dc_state->textcolor.g,
                &dc_state->textcolor.b, &dc_state->textcolor.a);

    memcpy (dc_state->attrs_g1, &pdc->bkmode, sizeof (dc_state->attrs_g1));
#ifdef _MGHAVE_ADV_2DAPI
    memcpy (dc_state->attrs_adv, &pdc->pen_type, sizeof (dc_state->attrs_adv));
#endif
    memcpy (dc_state->attrs_g2, &pdc->pLogFont, sizeof (dc_state->attrs_g2));

    dc_state->bitmap_scaler = pdc->bitmap_scaler;
    /* Store user color composition operator functions and context */
    if (ROP_COMP_USER) {
        dc_state->user_set_pixel = pdc->draw_pixel;
        dc_state->user_set_hline = pdc->draw_pixel_span;
        dc_state->user_put_hline = pdc->draw_src_span;
        dc_state->user_comp_ctxt = pdc->user_comp_ctxt;
    }

    nr_dc_states ++;
    if (dc_state_stack == NULL) {
        dc_state_stack = dc_state;
    }
    else {
        dc_state_stack->next = dc_state;
        dc_state->prev = dc_state_stack;
        dc_state_stack = dc_state;
    }

    UNLOCK (&__mg_gdilock);

    return nr_dc_states;

fail:
    UNLOCK (&__mg_gdilock);

    EmptyClipRgn (&dc_state->lcrgn);
    if (dc_state->pal.colors)
        free (dc_state->pal.colors);
    free (dc_state);
    return 0;
}

static void destroy_dc_state (DCSTATE* dc_state, BOOL empty_crgn)
{
    if (empty_crgn)
        EmptyClipRgn (&dc_state->lcrgn);
    if (dc_state->pal.colors)
        free (dc_state->pal.colors);
    free (dc_state);
}

BOOL GUIAPI RestoreDC (HDC hdc, int saved_dc)
{
    DCSTATE* dc_state;
    PDC pdc;

    if ((saved_dc == 0) ||
                (saved_dc > nr_dc_states) || (-saved_dc > nr_dc_states))
        return FALSE;

    pdc = dc_HDC2PDC (hdc);
    LOCK (&__mg_gdilock);

    if (saved_dc > 0) {
        int n = nr_dc_states - saved_dc;
        while (n--) {
            dc_state = dc_state_stack->prev;
            destroy_dc_state (dc_state_stack, TRUE);
            dc_state_stack = dc_state;
            dc_state_stack->next = NULL;
            nr_dc_states --;
        }
    }
    else { /* saved_dc < 0 */
        int n = -saved_dc - 1;
        while (n--) {
            dc_state = dc_state_stack->prev;
            destroy_dc_state (dc_state_stack, TRUE);
            dc_state_stack = dc_state;
            dc_state_stack->next = NULL;
            nr_dc_states --;
        }
    }

    dc_state = dc_state_stack;
    if (pdc->surface->format->palette
            && dc_state->pal.ncolors == pdc->surface->format->palette->ncolors){
        memcpy (pdc->surface->format->palette->colors, dc_state->pal.colors,
                sizeof (GAL_Color) * dc_state->pal.ncolors);
    }

    EmptyClipRgn (&pdc->lcrgn);
    pdc->lcrgn = dc_state->lcrgn;

    pdc->bkcolor = GAL_MapRGBA (pdc->surface->format,
                dc_state->bkcolor.r, dc_state->bkcolor.g,
                dc_state->bkcolor.b, dc_state->bkcolor.a);
    pdc->pencolor = GAL_MapRGBA (pdc->surface->format,
                dc_state->pencolor.r, dc_state->pencolor.g,
                dc_state->pencolor.b, dc_state->pencolor.a);
    pdc->brushcolor = GAL_MapRGBA (pdc->surface->format,
                dc_state->brushcolor.r, dc_state->brushcolor.g,
                dc_state->brushcolor.b, dc_state->brushcolor.a);
    pdc->textcolor = GAL_MapRGBA (pdc->surface->format,
                dc_state->textcolor.r, dc_state->textcolor.g,
                dc_state->textcolor.b, dc_state->textcolor.a);

    memcpy (&pdc->bkmode, dc_state->attrs_g1, sizeof (dc_state->attrs_g1));
#ifdef _MGHAVE_ADV_2DAPI
    memcpy (&pdc->pen_type, dc_state->attrs_adv, sizeof (dc_state->attrs_adv));
#endif
    memcpy (&pdc->pLogFont, dc_state->attrs_g2, sizeof (dc_state->attrs_g2));

    /* calculate gray_pixels, filter_pixels, and alpha_pixel_format */
    SetDCAttr (hdc, DC_ATTR_TEXT_COLOR, pdc->textcolor);

    /* Restore the pixel operation methods */
    if (pdc->rop >= 0 && pdc->rop < NR_ROPS) {
        pdc->draw_pixel = draw_pixel_ops [pdc->rop]
            [pdc->surface->format->BytesPerPixel - 1];
        pdc->draw_pixel_span = draw_pixel_span_ops [pdc->rop]
            [pdc->surface->format->BytesPerPixel - 1];
        pdc->draw_src_span = draw_src_span_ops [pdc->rop]
            [pdc->surface->format->BytesPerPixel - 1];
        pdc->user_comp_ctxt = pdc->surface->format;
    }
    else if (ROP_COMP_USER == pdc->rop) {
        pdc->draw_pixel = dc_state->user_set_pixel;
        pdc->draw_pixel_span = dc_state->user_set_hline;
        pdc->draw_src_span = dc_state->user_put_hline;
        pdc->user_comp_ctxt = dc_state->user_comp_ctxt;
    }

    pdc->move_to = move_to_ops [pdc->surface->format->BytesPerPixel - 1];
    pdc->step_x = step_x_ops [pdc->surface->format->BytesPerPixel - 1];

    pdc->bitmap_scaler = dc_state->bitmap_scaler;

    if (dc_IsGeneralDC (pdc)) {
        /* force to regenerate the ecrgn */
        LOCK_GCRINFO (pdc);
        dc_GenerateECRgn (pdc, TRUE);
        UNLOCK_GCRINFO (pdc);
    }
    else {
        dc_GenerateMemDCECRgn(pdc, TRUE);
    }

    dc_state_stack = dc_state->prev;
    if (dc_state_stack)
        dc_state_stack->next = NULL;
    nr_dc_states --;

    destroy_dc_state (dc_state, FALSE);

    UNLOCK (&__mg_gdilock);
    return TRUE;
}

int GUIAPI SetUserCompositionOps (HDC hdc, CB_COMP_SETPIXEL comp_setpixel,
        CB_COMP_SETHLINE comp_sethline, CB_COMP_PUTHLINE comp_puthline, void* user_ctxt)
{
    PDC pdc = dc_HDC2PDC (hdc);
    int old_rop = pdc->rop;
    if (NULL == comp_setpixel || NULL == comp_sethline || NULL == comp_puthline)
        return old_rop;
    pdc->draw_pixel = comp_setpixel;
    pdc->draw_pixel_span = comp_sethline;
    pdc->draw_src_span = comp_puthline;
    pdc->user_comp_ctxt= user_ctxt;
    return old_rop;
}

BOOL GUIAPI SyncUpdateDC (HDC hdc)
{
    BOOL rc;
    PDC pdc = dc_HDC2PDC (hdc);

    LOCK (&__mg_gdilock);
    rc = GAL_SyncUpdate (pdc->surface);
    UNLOCK (&__mg_gdilock);

    return rc;
}

BOOL GUIAPI SyncUpdateSurface (HWND hwnd)
{
    BOOL rc = FALSE;

#ifdef _MGSCHEMA_COMPOSITING
    PMAINWIN main_win = getMainWindowPtr (hwnd);

    if (main_win && main_win->surf) {
        LOCK (&__mg_gdilock);
        rc = GAL_SyncUpdate (main_win->surf);
        UNLOCK (&__mg_gdilock);
    }
#else
    LOCK (&__mg_gdilock);
    rc = GAL_SyncUpdate (__gal_screen);
    UNLOCK (&__mg_gdilock);
#endif

    return rc;
}

BOOL GUIAPI IsMemDC (HDC hdc)
{
    PDC pdc = dc_HDC2PDC(hdc);
    return dc_IsMemDC (pdc);
}

BOOL GUIAPI IsScreenDC (HDC hdc)
{
    PDC pdc = dc_HDC2PDC(hdc);
    return dc_IsScreenDC (pdc);
}

BOOL GUIAPI IsWindowDC (HDC hdc)
{
    PDC pdc = dc_HDC2PDC(hdc);
    return dc_IsGeneralDC (pdc);
}

GHANDLE GetVideoHandle (HDC hdc)
{
    PDC pdc = dc_HDC2PDC (hdc);
    if (pdc->surface)
        return (GHANDLE)pdc->surface->video;

    return NULL;
}

#ifdef _MGGAL_DRM

BOOL drmGetSurfaceInfo (GHANDLE video, HDC hdc, DrmSurfaceInfo* info)
{
    PDC pdc = dc_HDC2PDC (hdc);
    if (pdc->surface->video != (GHANDLE)video)
        return FALSE;

    return __drm_get_surface_info(pdc->surface, info);
}

HDC drmCreateDCFromNameEx (GHANDLE video, uint32_t name, uint32_t drm_format,
        off_t offset, uint32_t width, uint32_t height, uint32_t pitch)
{
    PDC pmem_dc = NULL;
    GAL_Surface* surface;

    if (!(pmem_dc = malloc (sizeof(DC))))
        return HDC_INVALID;

    LOCK (&__mg_gdilock);
    surface = __drm_create_surface_from_name (video, name,
            drm_format, offset, width, height, pitch);
    UNLOCK (&__mg_gdilock);

    if (!surface) {
        free (pmem_dc);
        return HDC_INVALID;
    }

    if (surface->format->Amask) {
        surface->flags |= GAL_SRCPIXELALPHA;
    }

    pmem_dc->DataType = TYPE_HDC;
    pmem_dc->DCType   = TYPE_MEMDC;
    pmem_dc->bInUse   = TRUE;
    pmem_dc->surface  = surface;

    dc_InitDC (pmem_dc, HWND_NULL, FALSE);

    InitClipRgn (&pmem_dc->lcrgn, &__mg_FreeClipRectList);
    MAKE_REGION_INFINITE(&pmem_dc->lcrgn);
    InitClipRgn (&pmem_dc->ecrgn, &__mg_FreeClipRectList);
#ifndef _MGSCHEMA_COMPOSITING
    pmem_dc->pGCRInfo = NULL;
    pmem_dc->oldage = 0;
#endif

    pmem_dc->DevRC.left = 0;
    pmem_dc->DevRC.top  = 0;
    pmem_dc->DevRC.right = width;
    pmem_dc->DevRC.bottom = height;

    SetClipRgn (&pmem_dc->ecrgn, &pmem_dc->DevRC);
    IntersectClipRect(&pmem_dc->lcrgn, &pmem_dc->DevRC);

    return (HDC)pmem_dc;
}

HDC drmCreateDCFromHandleEx (GHANDLE video, uint32_t handle, size_t size,
        uint32_t drm_format, off_t offset,
        uint32_t width, uint32_t height, uint32_t pitch)
{
    PDC pmem_dc = NULL;
    GAL_Surface* surface;

    if (!(pmem_dc = malloc (sizeof(DC))))
        return HDC_INVALID;

    LOCK (&__mg_gdilock);
    surface = __drm_create_surface_from_handle (video, handle, size,
                drm_format, offset, width, height, pitch);
    UNLOCK (&__mg_gdilock);

    if (!surface) {
        free (pmem_dc);
        return HDC_INVALID;
    }

    if (surface->format->Amask) {
        surface->flags |= GAL_SRCPIXELALPHA;
    }

    pmem_dc->DataType = TYPE_HDC;
    pmem_dc->DCType   = TYPE_MEMDC;
    pmem_dc->bInUse   = TRUE;
    pmem_dc->surface  = surface;

    dc_InitDC (pmem_dc, HWND_NULL, FALSE);

    InitClipRgn (&pmem_dc->lcrgn, &__mg_FreeClipRectList);
    MAKE_REGION_INFINITE(&pmem_dc->lcrgn);
    InitClipRgn (&pmem_dc->ecrgn, &__mg_FreeClipRectList);
#ifndef _MGSCHEMA_COMPOSITING
    pmem_dc->pGCRInfo = NULL;
    pmem_dc->oldage = 0;
#endif

    pmem_dc->DevRC.left = 0;
    pmem_dc->DevRC.top  = 0;
    pmem_dc->DevRC.right = width;
    pmem_dc->DevRC.bottom = height;

    SetClipRgn (&pmem_dc->ecrgn, &pmem_dc->DevRC);
    IntersectClipRect(&pmem_dc->lcrgn, &pmem_dc->DevRC);

    return (HDC)pmem_dc;
}

HDC drmCreateDCFromPrimeFdEx (GHANDLE video, int prime_fd, size_t size,
        uint32_t drm_format, off_t offset,
        uint32_t width, uint32_t height, uint32_t pitch)
{
    PDC pmem_dc = NULL;
    GAL_Surface* surface;

    if (!(pmem_dc = malloc (sizeof(DC))))
        return HDC_INVALID;

    if (size == 0) {
    }

    LOCK (&__mg_gdilock);
    surface =__drm_create_surface_from_prime_fd (video, prime_fd, size,
            drm_format, offset, width, height, pitch);
    UNLOCK (&__mg_gdilock);

    if (!surface) {
        free (pmem_dc);
        return HDC_INVALID;
    }

    if (surface->format->Amask) {
        surface->flags |= GAL_SRCPIXELALPHA;
    }

    pmem_dc->DataType = TYPE_HDC;
    pmem_dc->DCType   = TYPE_MEMDC;
    pmem_dc->bInUse   = TRUE;
    pmem_dc->surface  = surface;

    dc_InitDC (pmem_dc, HWND_NULL, FALSE);

    InitClipRgn (&pmem_dc->lcrgn, &__mg_FreeClipRectList);
    MAKE_REGION_INFINITE(&pmem_dc->lcrgn);
    InitClipRgn (&pmem_dc->ecrgn, &__mg_FreeClipRectList);
#ifndef _MGSCHEMA_COMPOSITING
    pmem_dc->pGCRInfo = NULL;
    pmem_dc->oldage = 0;
#endif

    pmem_dc->DevRC.left = 0;
    pmem_dc->DevRC.top  = 0;
    pmem_dc->DevRC.right = width;
    pmem_dc->DevRC.bottom = height;

    SetClipRgn (&pmem_dc->ecrgn, &pmem_dc->DevRC);
    IntersectClipRect(&pmem_dc->lcrgn, &pmem_dc->DevRC);

    return (HDC)pmem_dc;
}

#endif /* defined _MGGAL_DRM */

/* Since 5.0.0 */
HDC __mg_get_common_rgba8888_dc (void)
{
    static HDC _dc_common_rgba8888 = HDC_INVALID;
    static char fake_bits [4];

    if (_dc_common_rgba8888 == HDC_INVALID) {
        _dc_common_rgba8888 = CreateMemDCEx (1, 1, 32,
                MEMDC_FLAG_SWSURFACE | MEMDC_FLAG_SRCALPHA,
                0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000,
                fake_bits, sizeof (fake_bits));
        if (_dc_common_rgba8888 == HDC_INVALID) {
            _WRN_PRINTF ("failed to create common RGBA8888 dc\n");
        }
    }

    return _dc_common_rgba8888;
}

BOOL __mg_reset_common_rgba8888_dc (int width, int height, int pitch,
        void* pixels)
{
    HDC hdc;
    PDC pdc;

    hdc = __mg_get_common_rgba8888_dc ();
    pdc = dc_HDC2PDC (hdc);
    if (pdc == NULL)
        return FALSE;

    assert (width > 0 && height > 0);
    pdc->surface->w      = width;
    pdc->surface->h      = height;
    pdc->surface->pitch  = pitch;
    pdc->surface->pixels = pixels;

    pdc->DevRC.left   = 0;
    pdc->DevRC.top    = 0;
    pdc->DevRC.right  = width;
    pdc->DevRC.bottom = height;

    /* reset local and effective clipping region */
    MAKE_REGION_INFINITE(&pdc->lcrgn);
    SetClipRgn (&pdc->ecrgn, &pdc->DevRC);

    /* reset context info. */
    pdc->CurPenPos.x = pdc->CurTextPos.x = 0;
    pdc->CurPenPos.y = pdc->CurTextPos.y = 0;

    /* reset destination position */
    pdc->cur_dst = (BYTE*)pdc->surface->pixels
            + pdc->surface->pitch * pdc->DevRC.top
            + pdc->surface->format->BytesPerPixel * pdc->DevRC.left;

    return TRUE;
}

struct _travel_context;
typedef void (*on_control_fn) (struct _travel_context* ctxt, PCONTROL child);

struct _travel_context {
    int             nr;
    PMAINWIN        main_win;
    PCONTROL        parent;
    GAL_Surface*    surface;
    on_control_fn   cb;
};

static void on_surface_changed_for_control (struct _travel_context* ctxt,
        PCONTROL ctrl)
{
    RECT rc_surface, rc_minimal;
    PDC pdc = dc_HDC2PDC (ctrl->privCDC);

    _WRN_PRINTF ("called\n");

    pdc->surface = ctxt->surface;

    if (pdc->surface != __gal_screen) {
        /* recalate the device rectangle of the DC */
        dc_CalculateDevRC4MemDC (pdc);

        /* regnerate the effective clipping regions */
        EmptyClipRgn (&pdc->ecrgn);
        InitClipRgn (&pdc->ecrgn, &__mg_FreeClipRectList);

        rc_surface.left     = 0;
        rc_surface.top      = 0;
        rc_surface.right    = pdc->surface->w;
        rc_surface.bottom   = pdc->surface->h;

        /* clipping region more with pdc->DevRC and rc_surface. */
        IntersectRect (&rc_minimal, &pdc->DevRC, &rc_surface);
        SetClipRgn (&pdc->ecrgn, &rc_surface);
        IntersectClipRect (&pdc->lcrgn, &rc_surface);

        /* restrict control's effective region. */
        if (ctrl && !(ctrl->dwExStyle & WS_EX_CTRLASMAINWIN)) {
            CLIPRGN ergn;
            InitClipRgn (&ergn, &__mg_FreeClipRectList);

            if (RestrictControlMemDCECRGNEx (&rc_minimal, ctrl, &ergn)) {
                ClipRgnIntersect (&pdc->ecrgn, &pdc->ecrgn, &ergn);
            }
            else {
                EmptyClipRgn (&pdc->ecrgn);
            }
            EmptyClipRgn (&ergn);
        }
        else {
            IntersectClipRect (&pdc->ecrgn, &rc_minimal);
        }
    }
    else {
#ifdef _MGSCHEMA_COMPOSITING
        assert (0); // never touch here.
#else   /* defined _MGSCHEMA_COMPOSITING */
        RECT rc_minimal;

        /* the DC type changed to general DC when failed to create
           the secondary DC */
        pdc->DCType = TYPE_GENDC;

        pdc->pGCRInfo = ((PMAINWIN)pdc->hwnd)->pGCRInfo;

        LOCK_GCRINFO (pdc);

        pdc->oldage = pdc->pGCRInfo->age;
        ClipRgnCopy (&pdc->ecrgn, &pdc->pGCRInfo->crgn);

        dc_CalculateDevRC4GenDC (pdc);

        rc_minimal = pdc->DevRC;

        if (ctrl && !(ctrl->dwExStyle & WS_EX_CTRLASMAINWIN)) {
            CLIPRGN ergn;
            InitClipRgn (&ergn, &__mg_FreeClipRectList);

            if (RestrictControlECRGNEx (&rc_minimal, ctrl, &ergn)) {
                ClipRgnIntersect (&pdc->ecrgn, &pdc->ecrgn, &ergn);
                EmptyClipRgn (&ergn);
            }
            else {
                EmptyClipRgn (&pdc->ecrgn);
            }
        }
        else {
            IntersectClipRect (&pdc->ecrgn, &rc_minimal);
        }

        UNLOCK_GCRINFO (pdc);
#endif  /* not defined _MGSCHEMA_COMPOSITING */
    }

    /* reset the current destination */
    pdc->cur_dst = (BYTE*)pdc->surface->pixels
            + pdc->surface->pitch * pdc->DevRC.top
            + pdc->surface->format->BytesPerPixel * pdc->DevRC.left;
}

static void travel_children_with_priv_cdc (struct _travel_context* ctxt)
{
    PCONTROL child;

    child = (PCONTROL)(ctxt->parent->children);
    while (child) {

        if (child->privCDC) {
            ctxt->cb (ctxt, child);
            ctxt->nr++;
        }

        ctxt->parent = child;
        travel_children_with_priv_cdc (ctxt);

        child = child->next;
    }
}

#ifdef _MGSCHEMA_COMPOSITING
void __mg_update_dc_on_surface_changed (PMAINWIN pWin, GAL_Surface* surf)
{
    struct _travel_context ctxt = { 0, pWin, (PCONTROL)pWin, surf,
        on_surface_changed_for_control };

    if (pWin->privCDC) {
        on_surface_changed_for_control (&ctxt, (PCONTROL)pWin);
    }

    travel_children_with_priv_cdc (&ctxt);
}
#endif  /* defined _MGSCHEMA_COMPOSITING */

void __mg_update_dc_on_secondary_dc_changed (PMAINWIN pMainWin)
{
    struct _travel_context ctxt = { 0, pMainWin, (PCONTROL)pMainWin,
        NULL, on_surface_changed_for_control };

    if (pMainWin->secondaryDC) {
        ctxt.surface = dc_HDC2PDC(pMainWin->secondaryDC)->surface;
    }
    else {
        /* if failed to create the secondary DC */
#ifdef _MGSCHEMA_COMPOSITING
        ctxt.surface = pMainWin->surf;
#else
        ctxt.surface = __gal_screen;
#endif
    }

    if (pMainWin->privCDC) {
        on_surface_changed_for_control (&ctxt, (PCONTROL)pMainWin);
    }

    travel_children_with_priv_cdc (&ctxt);
}

