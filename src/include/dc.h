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
** dc.h: this head file declares all internal types and data of GDI module.
**
** Create Date: 1999/xx/xx
*/

#ifndef _DC_H
    #define _DC_H
    
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#if defined (__NOUNIX__) || defined (__uClinux__)
    #define DCSLOTNUMBER        8
#else
  #ifndef _MGRM_THREADS
    #define DCSLOTNUMBER        8
  #else
    #define DCSLOTNUMBER        16
  #endif
#endif

struct tagDC;
typedef struct tagDC DC;
typedef struct tagDC* PDC;
#ifndef _MGRM_THREADS
#define INIT_LOCK(lock, attr)
#define LOCK(lock)
#define UNLOCK(lock)
#define DESTROY_LOCK(lock)
#else
#define INIT_LOCK(lock, attr)   pthread_mutex_init(lock, attr)
#define LOCK(lock)              pthread_mutex_lock(lock)
#define UNLOCK(lock)            pthread_mutex_unlock(lock)
#define DESTROY_LOCK(lock)      pthread_mutex_destroy(lock)
#endif

/* for support mGEff mini MiniGUI by humingming 2010.7.8 */
#ifndef _MG_MINIMALGDI
void __mg_lock_recalc_gcrinfo (PDC pdc);
void __mg_unlock_gcrinfo (PDC pdc);
#define LOCK_GCRINFO(pdc)       __mg_lock_recalc_gcrinfo (pdc)
#define UNLOCK_GCRINFO(pdc)     if (dc_IsGeneralDC(pdc)) __mg_unlock_gcrinfo (pdc)
#else
RECT g_rcScr;
PLOGFONT g_SysLogFont[1];
#define LOCK_GCRINFO(pdc)       
#define UNLOCK_GCRINFO(pdc)    
#define __mg_hwnd_desktop                     0xFFFFFFFD
#define __mg_dsk_win                          1
#define gui_Control(hwnd)                     NULL
#define DestroyDskMsgQueue()                  
#define salone_StandAloneStartup()            TRUE 
#define salone_StandAloneCleanup()            
#define salone_IdleHandler4StandAlone(qmsg)   NULL
#define kernel_GetGCRgnInfo(hwnd)             NULL
#define GetWindowFont(hwnd)                   NULL
#define mg_InitControlClass()                 TRUE
#define mg_InitAccel()                        TRUE
#define mg_InitDesktop()                      TRUE
#define mg_InitLWEvent()                      TRUE
#define mg_InitFreeQMSGList()                 TRUE
#define mg_InitSystemRes()                    TRUE
#define mg_TerminateLWEvent()                 TRUE
#define mg_TerminateLFManager()               TRUE
#define mg_TerminateDesktop()                 TRUE
#define PreDefMainWinProc                     NULL
#define PreDefControlProc                     NULL
#define PreDefDialogProc                      NULL
#define SendMessage(hwnd, message, wParam, lParam)    1 
#endif

BOOL dc_GenerateECRgn (PDC pdc, BOOL fForce);
PDC __mg_check_ecrgn (HDC hdc);
BOOL dc_GenerateMemDCECRgn(PDC pdc, BOOL fForce);

typedef void (* DC_STEP_X) (PDC pdc, int step);
typedef void (* DC_MOVE_TO) (PDC pdc, int x, int y);

/**
 * \var typedef void* (* CB_GET_LINE_BUFF) (void* context, int y, void** alpha_line_mask)
 * \brief Bitmap scaler's getting line buffer callback.
 */
typedef void* (* CB_GET_LINE_BUFF) (void* context, int y, void** alpha_line_mask);

/**
 * \var typedef void* (* CB_LINE_SCALED) (void* context, const void* line, int y)
 * \brief Bitmap scaler's getting line buffer callback.
 */
typedef void (* CB_LINE_SCALED) (void* context, const void* line, int y);

typedef BOOL (* CB_BITMAP_SCALER_FUNC )(
        void* context, 
        const BITMAP* srjjc_bmp, 
        int dst_w, int dst_h,
        CB_GET_LINE_BUFF cb_line_buff, 
        CB_LINE_SCALED cb_line_scaled,
        GAL_PixelFormat *format);

/* Device Context */
struct tagDC
{
    short DataType;  /* the data type, always be TYPE_HDC */
    short DCType;    /* the dc type */
        
    BOOL inuse;
    HWND hwnd;

    /* surface of this DC */
    GAL_Surface* surface;

    /* background color */
    gal_pixel bkcolor;

    /* pen color */
    gal_pixel pencolor;

    /* solid brush color */
    gal_pixel brushcolor;

    /* text color */
    gal_pixel textcolor;

    int bkmode;

    int tabstop;
    int cExtra;     /* Character extra */
    int alExtra;    /* Above line extra */
    int blExtra;    /* Bellow line extra */

    int mapmode;    /* mappping mode */

    int ta_flags;   /* Text alignment flags */

#ifdef _MGHAVE_ADV_2DAPI
    /* pen attributes */
    int pen_type;
    int pen_cap_style;
    int pen_join_style;
    unsigned int pen_width;

    /* brush attributes */
    int brush_type;

    POINT brush_orig;
    const BITMAP* brush_tile;
    const STIPPLE* brush_stipple;

    /* custom dash info */
    int dash_offset;
    const unsigned char* dash_list;
    size_t dash_list_len;
#endif

    PLOGFONT pLogFont;

    POINT CurPenPos;
    POINT CurTextPos;

    POINT ViewOrig;
    POINT ViewExtent;
    POINT WindowOrig;
    POINT WindowExtent;

    /* raster operation */
    int rop;

    /* used by the text rendering for anti-aliasing fonts. */
    gal_pixel gray_pixels [17];
    /* used by the text rendering for low-pass filtering. */
    gal_pixel filter_pixels [17];
    GAL_PixelFormat* alpha_pixel_format;

    /* pixel and line operation */
    CB_COMP_SETPIXEL draw_pixel;
    CB_COMP_SETHLINE draw_pixel_span;
    CB_COMP_PUTHLINE draw_src_span;
    DC_MOVE_TO move_to;
    DC_STEP_X  step_x;

    /* === context information. ============================================= */
    /* DK[01/22/10]:This segment is binary compatible with _COMP_CTXT struct */
    /* VW[01/18/18]:Adjust the fields sequence for 64-bit to ensure 8-byte alignment */
    gal_uint8*  cur_dst;
    void*       user_comp_ctxt;
    gal_pixel   skip_pixel;
    gal_pixel   cur_pixel;
    int         step;
    /* ====================================================================== */

    CLIPRECT* cur_ban;
    RECT rc_output;
    
    /* local clip region information */
    CLIPRGN  lcrgn;

    /* effective clip region information */
    CLIPRGN  ecrgn;

    /* device rect */
    BOOL bIsClient;
    RECT DevRC;

    PGCRINFO pGCRInfo;
    unsigned int oldage;

    CB_BITMAP_SCALER_FUNC bitmap_scaler;
};

#define PDC_TO_COMP_CTXT(pdc) ((COMP_CTXT* )(&pdc->cur_dst))

extern DC __mg_screen_dc;
extern DC __mg_screen_sys_dc;

/* This function convert HDC to PDC. */
static inline PDC dc_HDC2PDC (HDC hdc)
{
    if (hdc == HDC_SCREEN_SYS) 
      	return &__mg_screen_sys_dc;
    else if (hdc == HDC_SCREEN) 
      	return &__mg_screen_dc;
    return (PDC) hdc;
}

static inline BOOL dc_IsMemDC (PDC pdc)
{
    return (pdc->DCType == TYPE_MEMDC);
}

static inline BOOL dc_IsScreenDC (PDC pdc)
{
    return (pdc->DCType == TYPE_SCRDC);
}

static inline BOOL dc_IsGeneralDC (PDC pdc)
{
    return (pdc->DCType == TYPE_GENDC);
}

static inline BOOL dc_IsVisible (PDC pdc)
{
    PCONTROL pCtrl;
    
#if 0
    if (pdc->DCType != TYPE_GENDC)
        return TRUE;
#else
    if (pdc->DCType != TYPE_GENDC) {
        if (dc_IsMemDC(pdc)){
            if (!pdc->hwnd) return TRUE;
        }
        else return TRUE;
    }
#endif
    
    if (pdc->hwnd == HWND_DESKTOP)
        return TRUE;
 
    MG_CHECK_RET (MG_IS_WINDOW(pdc->hwnd), FALSE);

    pCtrl = (PCONTROL)(pdc->hwnd);

    do {
        if (!(pCtrl->dwStyle & WS_VISIBLE))
            return FALSE;

        pCtrl = pCtrl->pParent;
    } while (pCtrl);

    return TRUE;
}
static inline void coor_DP2SP(PDC pdc, int* x, int* y)
{
    *x += pdc->DevRC.left;
    *y += pdc->DevRC.top;
}

static inline void coor_SP2DP(PDC pdc, int* x, int* y)
{
    *x -= pdc->DevRC.left;
    *y -= pdc->DevRC.top;
}


static inline void coor_LP2SP(PDC pdc, int* x, int* y)
{
    if (dc_IsScreenDC (pdc))
        return;

    if (pdc->mapmode == MM_TEXT) {
        *x += pdc->DevRC.left;
        *y += pdc->DevRC.top;
    }
    else {
        *x = pdc->DevRC.left + (*x - pdc->WindowOrig.x)
             * pdc->ViewExtent.x / pdc->WindowExtent.x
             + pdc->ViewOrig.x;

        *y = pdc->DevRC.top + (*y - pdc->WindowOrig.y)
             * pdc->ViewExtent.y / pdc->WindowExtent.y
             + pdc->ViewOrig.y;
    }
}

static inline void coor_SP2LP(PDC pdc, int* x, int* y)
{
    if (dc_IsScreenDC (pdc))
        return;

    *x -= pdc->DevRC.left;
    *y -= pdc->DevRC.top;

    if (pdc->mapmode != MM_TEXT) {
        *x = (*x - pdc->ViewOrig.x)
             * pdc->WindowExtent.x / pdc->ViewExtent.x
             + pdc->WindowOrig.x;

        *y = (*y - pdc->ViewOrig.y)
             * pdc->WindowExtent.y / pdc->ViewExtent.y
             + pdc->WindowOrig.y;
    }
}

static inline void extent_x_LP2SP (PDC pdc, int* extent)
{
    if (dc_IsScreenDC (pdc) || pdc->mapmode == MM_TEXT)
        return;

    *extent = *extent * pdc->ViewExtent.x / pdc->WindowExtent.x;
}

static inline void extent_x_SP2LP (PDC pdc, int* extent)
{
    if (dc_IsScreenDC (pdc) || pdc->mapmode == MM_TEXT)
        return;

    *extent = *extent * pdc->WindowExtent.x / pdc->ViewExtent.x;
}

static inline void extent_y_LP2SP (PDC pdc, int* extent)
{
    if (dc_IsScreenDC (pdc) || pdc->mapmode == MM_TEXT)
        return;

    *extent = *extent * pdc->ViewExtent.y / pdc->WindowExtent.y;
}

static inline void extent_y_SP2LP (PDC pdc, int* extent)
{
    if (dc_IsScreenDC (pdc) || pdc->mapmode == MM_TEXT)
        return;

    *extent = *extent * pdc->WindowExtent.y / pdc->ViewExtent.y;
}

static inline gal_pixel _mem_get_pixel (Uint8 *dst, int bpp)
{
    switch (bpp) {
        case 1:
            return *dst;
        case 2:
            return *(Uint16*)dst;
        case 3:
        {
            gal_pixel pixel;
            if (GAL_BYTEORDER == GAL_LIL_ENDIAN) {
                pixel = dst[0] + (dst[1] << 8) + (dst[2] << 16);
            } else {
                pixel = (dst[0] << 16) + (dst[1] << 8) + dst[2];
            }
            return pixel;
        }
        case 4:
            return *(Uint32*)dst;
    }

    return 0;
}

static inline BYTE* _mem_set_pixel (BYTE* dst, int bpp, Uint32 pixel)
{
    switch (bpp) {
        case 1:
            *dst= pixel;
            break;
        case 2:
            *(Uint16 *) dst = pixel;
            break;
        case 3:
            if (GAL_BYTEORDER == GAL_LIL_ENDIAN) {
                dst [0] = pixel;
                dst [1] = pixel >> 8;
                dst [2] = pixel >> 16;
            } else {
                dst [0] = pixel >> 16;
                dst [1] = pixel >> 8;
                dst [2] = pixel;
            }
            break;
        case 4:
            *(Uint32 *) dst = pixel;
            break;
    }

    return dst + bpp;
}

#ifdef _MGRM_PROCESSES
#   include "ourhdr.h"
#   include "client.h"
#   include "sharedres.h"
#   include "drawsemop.h"
#endif

static inline BYTE* _dc_get_dst (PDC pdc, int x, int y)
{
    BYTE* dst = (BYTE*)pdc->surface->pixels + pdc->surface->pitch * y;
    switch (pdc->surface->format->BytesPerPixel) {
            case 1:
                    dst += x;
                    break;
            case 2:
                    dst += x<<1;
                    break;
            case 3:
                    dst += (x<<1) + x;
                    break;
            case 4:
                    dst += x<<2;
                    break;
    }
    return dst;
}

static inline void _dc_step_y (PDC pdc, int step)
{
    pdc->cur_dst += pdc->surface->pitch * step;
}

#ifndef _MGRM_THREADS

#ifdef _MGRM_STANDALONE

#define BLOCK_DRAW_SEM(pdc)
#define UNBLOCK_DRAW_SEM(pdc)

#ifndef _MG_MINIMALGDI
#define CHECK_DRAWING(pdc)      \
        (__mg_switch_away && pdc->surface == __gal_screen)
#else
#define CHECK_DRAWING(pdc)      \
        (FALSE)
#endif

#define CHECK_CLI_SCREEN(pdc, rcOutput) FALSE

#else

#define BLOCK_DRAW_SEM(pdc)     \
        if (!mgIsServer && pdc->surface == __gal_screen) lock_draw_sem ()
#define UNBLOCK_DRAW_SEM(pdc)   \
        if (!mgIsServer && pdc->surface == __gal_screen) unlock_draw_sem ()

#define CHECK_DRAWING(pdc)      \
        (((!mgIsServer && (SHAREDRES_TOPMOST_LAYER != __mg_layer)) \
            || __mg_switch_away) && pdc->surface == __gal_screen)

#define CHECK_CLI_SCREEN(pdc, rcOutput)  FALSE

#endif

#else

#define BLOCK_DRAW_SEM(pdc)
#define UNBLOCK_DRAW_SEM(pdc)

#endif 

int __mg_enter_drawing (PDC pdc);
void __mg_enter_drawing_nocheck (PDC pdc);
void __mg_leave_drawing (PDC pdc);

#define ENTER_DRAWING(pdc)               \
    if (__mg_enter_drawing(pdc) < 0)     \
        goto leave_drawing

#define LEAVE_DRAWING(pdc)           \
    __mg_leave_drawing(pdc);         \
leave_drawing:

#define ENTER_DRAWING_NOCHECK(pdc) __mg_enter_drawing_nocheck(pdc)

#define LEAVE_DRAWING_NOCHECK(pdc) __mg_leave_drawing(pdc)

#define SET_GAL_CLIPRECT(pdc, cliprect)                             \
    pdc->surface->clip_rect.x = cliprect.left;                      \
    pdc->surface->clip_rect.y = cliprect.top;                       \
    pdc->surface->clip_rect.w = cliprect.right - cliprect.left;     \
    pdc->surface->clip_rect.h = cliprect.bottom - cliprect.top

#if MGUI_BYTEORDER == MGUI_LIL_ENDIAN

    #define EQUAL_24BIT(ptr, val)  \
                ((*(BYTE*)(ptr)      == ((BYTE)(val))) \
                &&(*((BYTE*)(ptr)+1) == ((BYTE)((val)>>8))) \
                &&(*((BYTE*)(ptr)+2) == ((BYTE)((val)>>16))))

    #define SETVAL_24BIT(ptr, val) do { \
            *(BYTE*)(ptr)     = (BYTE)(val); \
            *((BYTE*)(ptr)+1) = (BYTE)((val)>>8); \
            *((BYTE*)(ptr)+2) = (BYTE)((val)>>16); \
        }while(0)

    #define ORVAL_24BIT(ptr, val)  do { \
            *(BYTE*)(ptr)     |= (BYTE)(val); \
            *((BYTE*)(ptr)+1) |= (BYTE)((val)>>8); \
            *((BYTE*)(ptr)+2) |= (BYTE)((val)>>16);  \
        }while(0)

    #define ANDVAL_24BIT(ptr, val) do { \
            *(BYTE*)(ptr)     &= (BYTE)(val); \
            *((BYTE*)(ptr)+1) &= (BYTE)((val)>>8); \
            *((BYTE*)(ptr)+2) &= (BYTE)((val)>>16);  \
        }while(0)

    #define XORVAL_24BIT(ptr, val) do { \
            *(BYTE*)(ptr)     ^= (BYTE)(val); \
            *((BYTE*)(ptr)+1) ^= (BYTE)((val)>>8); \
            *((BYTE*)(ptr)+2) ^= (BYTE)((val)>>16); \
        }while(0)

    #define READPTR_24BIT(val, ptr) ((val) = (*(BYTE*)ptr) \
                + ((*((BYTE*)(ptr)+1))<<8) + ((*((BYTE*)(ptr)+2))<<16))
#else

    #define EQUAL_24BIT(ptr, val)  \
        ((*(BYTE*)(ptr)      == ((BYTE)((val)>>16))) \
        &&(*((BYTE*)(ptr)+1) == ((BYTE)((val)>>8))) \
        &&(*((BYTE*)(ptr)+2) == ((BYTE)((val)))))

    #define SETVAL_24BIT(ptr, val) do { \
            *(BYTE*)(ptr)     = (BYTE)((val)>>16); \
            *((BYTE*)(ptr)+1) = (BYTE)((val)>>8); \
            *((BYTE*)(ptr)+2) = (BYTE)((val));  \
        }while(0)

    #define ORVAL_24BIT(ptr, val)  do { \
            *(BYTE*)(ptr)     |= (BYTE)((val)>>16); \
            *((BYTE*)(ptr)+1) |= (BYTE)((val)>>8); \
            *((BYTE*)(ptr)+2) |= (BYTE)((val));  \
        }while(0)

    #define ANDVAL_24BIT(ptr, val)do { \
            *(BYTE*)(ptr)     &= (BYTE)((val)>>16); \
            *((BYTE*)(ptr)+1) &= (BYTE)((val)>>8); \
            *((BYTE*)(ptr)+2) &= (BYTE)((val));  \
        }while(0)

    #define XORVAL_24BIT(ptr, val) do { \
            *(BYTE*)(ptr)     ^= (BYTE)((val)>>16); \
            *((BYTE*)(ptr)+1) ^= (BYTE)((val)>>8); \
            *((BYTE*)(ptr)+2) ^= (BYTE)((val)); \
        }while(0)

    #define READPTR_24BIT(val, ptr) ((val) = (((*(BYTE*)ptr)<<16) \
            + ((*((BYTE*)(ptr)+1))<<8) + (*((BYTE*)(ptr)+2))))

#endif // #if MGUI_BYTEORDER == MGUI_LIL_ENDIAN

MG_EXPORT HDC GetSecondarySubDC (HDC secondary_dc, HWND hwnd_child, BOOL client);
MG_EXPORT void ReleaseSecondarySubDC (HDC secondary_subdc);

static inline HDC get_valid_dc (PMAINWIN pWin, BOOL client)
{
#if 1
    if (!(pWin->dwExStyle & WS_EX_CTRLASMAINWIN) 
            && (pWin->pMainWin->secondaryDC)) {
#else
    if (pWin->pMainWin->secondaryDC) {
#endif
        if (client && (pWin->dwExStyle & WS_EX_USEPRIVATECDC)) {
            return pWin->privCDC;
        }
        else
            return GetSecondarySubDC (pWin->pMainWin->secondaryDC, 
                    (HWND)pWin, client);
    }
    else {
        if (client && (pWin->dwExStyle & WS_EX_USEPRIVATECDC)) {
            return pWin->privCDC;
        }
        if (client) {
            return GetClientDC ((HWND)pWin);
        }
        else {
            return GetDC ((HWND)pWin);
        }
    }
}

static inline void release_valid_dc (PMAINWIN pWin, HDC hdc)
{
    if (pWin->pMainWin->secondaryDC) {
        if (pWin->privCDC != hdc)
            ReleaseSecondarySubDC (hdc);
    }
    else {
        if (pWin->privCDC != hdc) {
            ReleaseDC (hdc);
        }
    }
} 

void update_secondary_dc (PMAINWIN pWin, HDC secondary_dc, 
        HDC real_dc, const RECT* rc, DWORD flags);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // _DC_H 
