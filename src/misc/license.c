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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef __NOUNIX__
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#endif /* __NOUNIX__ */

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "misc.h"
#include "event.h"

#include "license.h"

#if defined(_MG_ENABLE_SPLASH) || defined(_MG_ENABLE_SCREENSAVER)
BITMAP g_license_bitmaps[LICENSE_BITMAP_NR];

#ifndef _MG_LICENSE_SCREENSAVER_TIMEOUT
#define _MG_LICENSE_SCREENSAVER_TIMEOUT (10*100)
#endif

#ifndef _MG_LICENSE_SCREENSAVER_UPDATE_TIME
#define _MG_LICENSE_SCREENSAVER_UPDATE_TIME (2*100)
#endif

/* step of splash */
#ifndef _MG_LICENSE_SPLASH_STEP
#define _MG_LICENSE_SPLASH_STEP (25)
#endif

/* one frame */
#ifndef _MG_LICENSE_SPLASH_MSEC
#define _MG_LICENSE_SPLASH_MSEC (40)
#endif

#if defined(_MG_ENABLE_SCREENSAVER)
static int  s_tick_last_input;
static BOOL screensaver_running;
extern int dskCreateTopZOrderNode (int cli, const RECT *rc);
extern int dskDestroyTopZOrderNode (int cli, int idx_znode);
extern void dskRefreshAllClient (const RECT* invrc);
#endif

#endif

/***********************************************************
 * Screensaver
 ***********************************************************/
#ifdef _MG_ENABLE_SCREENSAVER

static void screensaver_update(void)
{
    RECT rcScr = GetScreenRect();

    /* fixed size 200 * 99 */
    /* g_bitmap_minigui 200 * 55 at 0  * 44 */
    /* g_bitmap_feiman  118 * 41 at 61 * 0  */
    /* g_bitmap_fmsoft  50  * 11 at 4  * 30 */

    int pos_x = (RECTW(rcScr) > 200)
                ? (rand() % (RECTW(rcScr) - 200))
                : 0;
    int pos_y = (RECTH(rcScr) > 99)
                ? (rand() % (RECTH(rcScr) - 99))
                : 0;

    /* clean screen */
    SetBrushColor (HDC_SCREEN_SYS, PIXEL_black);
    FillBox (HDC_SCREEN_SYS, 0, 0, RECTW(rcScr), RECTH(rcScr));

    /* put picture to screen */
    /* g_bitmap_minigui */
    FillBoxWithBitmap (HDC_SCREEN_SYS,
            pos_x, pos_y + 44,
            0, 0,
            &g_bitmap_minigui);

    /* g_bitmap_feiman */
    FillBoxWithBitmap (HDC_SCREEN_SYS,
            pos_x + 61, pos_y,
            0, 0,
            &g_bitmap_feiman);

    /* g_bitmap_fmsoft */
    FillBoxWithBitmap (HDC_SCREEN_SYS,
            pos_x + 4, pos_y + 30,
            0, 0,
            &g_bitmap_fmsoft);
}
#else
#   define screensaver_update() /* NULL */
#endif /* endif _MG_ENABLE_SCREENSAVER */

/***********************************************************
 * Splash
 ***********************************************************/
#ifdef _MG_ENABLE_SPLASH
static int splash_adjust_x;
static int splash_adjust_y;
static int splash_bar_postion;
static int splash_bar_direction;

#define SPLASH_W         240
#define SPLASH_H         240
#define SPLASH_FEIMAN_X  (splash_adjust_x + 86)
#define SPLASH_FEIMAN_Y  (splash_adjust_y + 51)
#define SPLASH_FMSOFT_X  (splash_adjust_x + 29)
#define SPLASH_FMSOFT_Y  (splash_adjust_y + 81)
#define SPLASH_MINIGUI_X (splash_adjust_x + 25)
#define SPLASH_MINIGUI_Y (splash_adjust_y + 95)
#define SPLASH_BAR_X     (splash_adjust_x + 7)
#define SPLASH_BAR_Y     (splash_adjust_y + 168)
#define SPLASH_ID_X      (SPLASH_BAR_X)
#define SPLASH_ID_Y      (SPLASH_BAR_Y + 14)
#define SPLASH_ID_W      226
#define SPLASH_BAR_STEP  10
#define SPLASH_BAR_LEFT  0
#define SPLASH_BAR_RIGHT 1

static void splash_init(void)
{
    RECT rcScr = GetScreenRect();

    splash_bar_postion = 0;
    splash_bar_direction = SPLASH_BAR_RIGHT;

    splash_adjust_x = (RECTW(rcScr) - SPLASH_W) / 2;
    splash_adjust_y = (RECTH(rcScr) - SPLASH_H)*3 / 5;
}

void __mg_splash_draw_framework (void)
{
    char text[64];
    RECT rc_text;
    PLOGFONT font, old_font;
    RECT rcScr = GetScreenRect();

    gal_pixel old_brush = SetBrushColor(HDC_SCREEN_SYS, PIXEL_black);
    gal_pixel old_text = SetTextColor(HDC_SCREEN_SYS, RGB2Pixel(HDC_SCREEN_SYS, 0x32, 0xa0, 0xe4));

    font = CreateLogFont ("rbf", "vgaoem", "ISO8859-1",
            FONT_WEIGHT_REGULAR,
            FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
            8, 0);
    old_font = SelectFont(HDC_SCREEN_SYS, font);

    FillBox(HDC_SCREEN_SYS, 0 ,0, RECTW(rcScr), RECTH(rcScr));

    FillBoxWithBitmap(HDC_SCREEN_SYS, SPLASH_FEIMAN_X, SPLASH_FEIMAN_Y, 0, 0, &g_bitmap_feiman);
    FillBoxWithBitmap(HDC_SCREEN_SYS, SPLASH_FMSOFT_X, SPLASH_FMSOFT_Y, 0, 0, &g_bitmap_fmsoft);
    FillBoxWithBitmap(HDC_SCREEN_SYS, SPLASH_MINIGUI_X, SPLASH_MINIGUI_Y, 0, 0, &g_bitmap_minigui);
    FillBoxWithBitmap(HDC_SCREEN_SYS, SPLASH_BAR_X, SPLASH_BAR_Y, 0, 0, &g_bitmap_progressbar_bk);
    FillBoxWithBitmap(HDC_SCREEN_SYS, SPLASH_BAR_X, SPLASH_BAR_Y, 0, 0, &g_bitmap_progressbar);

    memset(text, 0, sizeof(text));
    SetRect(&rc_text, SPLASH_ID_X, SPLASH_ID_Y, SPLASH_ID_X+SPLASH_ID_W, RECTH(rcScr));

    sprintf(text, "SN: N/A");

    SetBkMode(HDC_SCREEN_SYS, BM_TRANSPARENT);
    DrawText(HDC_SCREEN_SYS, text, -1, &rc_text, DT_TOP | DT_RIGHT | DT_SINGLELINE);

    SelectFont(HDC_SCREEN_SYS, old_font);
    DestroyLogFont(font);

    SetBrushColor(HDC_SCREEN_SYS, old_brush);
    SetTextColor(HDC_SCREEN_SYS, old_text);
    SetBkMode(HDC_SCREEN_SYS, BM_OPAQUE);
    SyncUpdateDC(HDC_SCREEN_SYS);
}

void __mg_splash_progress (void)
{
    if (SPLASH_BAR_RIGHT == splash_bar_direction) {
        splash_bar_postion += SPLASH_BAR_STEP;
        if (splash_bar_postion + g_bitmap_progressbar.bmWidth > g_bitmap_progressbar_bk.bmWidth) {
            splash_bar_direction = SPLASH_BAR_LEFT;
            splash_bar_postion = g_bitmap_progressbar_bk.bmWidth - g_bitmap_progressbar.bmWidth;
        }
    } else {
        splash_bar_postion -= SPLASH_BAR_STEP;
        if (splash_bar_postion < 0) {
            splash_bar_direction = SPLASH_BAR_RIGHT;
            splash_bar_postion = 0;
        }
    }

    FillBoxWithBitmap(HDC_SCREEN_SYS, SPLASH_BAR_X, SPLASH_BAR_Y,
            0, 0, &g_bitmap_progressbar_bk);
    FillBoxWithBitmap(HDC_SCREEN_SYS, SPLASH_BAR_X+splash_bar_postion, SPLASH_BAR_Y,
            0, 0, &g_bitmap_progressbar);
    SyncUpdateDC(HDC_SCREEN_SYS);
}

void __mg_splash_delay (void)
{
    int i;

    for (i = 0; i < _MG_LICENSE_SPLASH_STEP; i++) {
        __mg_splash_progress();
        __mg_os_time_delay (_MG_LICENSE_SPLASH_MSEC);
    }
}

#else
#   define splash_init() /* NULL */
#endif /* _MG_ENABLE_SPLASH.*/

#if defined(_MG_ENABLE_SPLASH) || defined(_MG_ENABLE_SCREENSAVER)

#include "../sysres/license/c_files/_splash_inner_res.c"

void __mg_license_create(void) {
    int i;

    for (i=0; i<LICENSE_BITMAP_NR; ++i) {
        unsigned char *data;
        int len;

        data = (unsigned char *)malloc(__mg_splash_splash_inner_res[i].data_len);
        memcpy(data, __mg_splash_splash_inner_res[i].data, __mg_splash_splash_inner_res[i].data_len);
        len = __mg_splash_splash_inner_res[i].data_len;

        LoadBitmapFromMem(HDC_SCREEN_SYS, &g_license_bitmaps[i], data, len, "png");
        free(data);
    }

    splash_init();

#ifdef _MG_ENABLE_SCREENSAVER
    watermark_init();
#endif
}

void __mg_license_destroy (void) {
    int i;
    for (i = 0; i < LICENSE_BITMAP_NR; i++) {
        UnloadBitmap(&g_license_bitmaps[i]);
    }
}

#endif /* defined(_MG_ENABLE_SPLASH) || defined(_MG_ENABLE_SCREENSAVER) */

