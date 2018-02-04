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

#undef _MG_ENABLE_WATERMARK /* XXX: Always disable watermark */

#if defined(_MG_ENABLE_SPLASH) || \
    defined(_MG_ENABLE_SCREENSAVER) || \
    defined(_MG_ENABLE_WATERMARK)
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

#if defined(_MG_ENABLE_SCREENSAVER) || \
    defined(_MG_ENABLE_WATERMARK)
static int  s_tick_last_input;
static BOOL screensaver_running;
extern int dskCreateTopZOrderNode (int cli, const RECT *rc);
extern int dskDestroyTopZOrderNode (int cli, int idx_znode);
extern int dskSetTopForEver(int cli, int idx_znode, BOOL show);
extern void dskRefreshAllClient (const RECT* invrc);
#endif
#endif

/***********************************************************
 * Product ID
 ***********************************************************/
#ifdef _MG_PRODUCTID

static unsigned char product_id_buffer[40] = {
    PRODUCT_ID_PREFIX,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* use this for splash display customer id by humingming 2010.8.12 */ 
int license_get_customer_id(void) {
    return ((product_id_t*)product_id_buffer)->customer_id;
}

#else
//#   warning PRODUCT_ID_is_DISABLED
#endif

/***********************************************************
 * Watermark
 ***********************************************************/
#ifdef _MG_ENABLE_WATERMARK

#define MARGINSPACE 5

typedef struct _CornerInfo{
    POINT   pos;
    HWND    hwnd;
    BITMAP* bmp;
}CornerInfo;

static CornerInfo fourcorner_info[4] = {
    {{MARGINSPACE, MARGINSPACE} },
    {{-MARGINSPACE, MARGINSPACE} },
    {{MARGINSPACE,  -MARGINSPACE} },
    {{-MARGINSPACE, -MARGINSPACE} }
};

static void watermark_update(void)
{
    int i = 0;
    for (i = 0; i < TABLESIZE(fourcorner_info); i++)
        FillBoxWithBitmap (HDC_SCREEN_SYS, 
                fourcorner_info[i].pos.x, fourcorner_info[i].pos.y, 
                0, 0, fourcorner_info[i].bmp);
}

static void watermark_init(void)
{
    fourcorner_info[0].bmp = &g_license_bitmaps[WATERMARK_1];
    fourcorner_info[1].bmp = &g_license_bitmaps[WATERMARK_2];
    fourcorner_info[2].bmp = &g_license_bitmaps[WATERMARK_3];
    fourcorner_info[3].bmp = &g_license_bitmaps[WATERMARK_3];

    /* right-top.*/
    fourcorner_info[1].pos.x += RECTW(g_rcScr) - fourcorner_info[1].bmp->bmWidth;
    /* left-bottom.*/
    fourcorner_info[2].pos.y += RECTH(g_rcScr) - fourcorner_info[2].bmp->bmHeight;
    /* right-bottom.*/
    fourcorner_info[3].pos.x += RECTW(g_rcScr) - fourcorner_info[3].bmp->bmWidth;
    fourcorner_info[3].pos.y += RECTH(g_rcScr) - fourcorner_info[3].bmp->bmHeight;
}

#else
#   define watermark_update() /* NULL */
#   define watermark_init() /* NULL */
#endif /* _MG_ENABLE_WATERMARK */

/***********************************************************
 * Screensaver
 ***********************************************************/
#ifdef _MG_ENABLE_SCREENSAVER
static int s_screensaver_node;

void screensaver_show(void)
{
    dskSetTopForEver(0, s_screensaver_node, TRUE);
}

void screensaver_hide(void)
{
    dskSetTopForEver(0, s_screensaver_node, FALSE);
}

void screensaver_create(void)
{
    /* create screensaver node. */
    if (!s_screensaver_node) {
        RECT rc = g_rcScr;
        s_screensaver_node  = dskCreateTopZOrderNode(0, &rc);
        dskSetTopForEver(0, s_screensaver_node, TRUE);
        screensaver_hide();
    }
}

void screensaver_destroy(void)
{
    /* destroy screensaver node. */
    if (s_screensaver_node) {
        dskDestroyTopZOrderNode(0, s_screensaver_node);
        s_screensaver_node = 0;
    }
}

static void screensaver_update(void)
{
    /* fixed size 200 * 99 */
    /* g_bitmap_minigui 200 * 55 at 0  * 44 */
    /* g_bitmap_feiman  118 * 41 at 61 * 0  */
    /* g_bitmap_fmsoft  50  * 11 at 4  * 30 */

    int pos_x = (RECTW(g_rcScr) > 200)
                ? (rand() % (RECTW(g_rcScr) - 200))
                : 0;
    int pos_y = (RECTH(g_rcScr) > 99)
                ? (rand() % (RECTH(g_rcScr) - 99))
                : 0;

    /* clean screen */
    SetBrushColor (HDC_SCREEN_SYS, PIXEL_black);
    FillBox (HDC_SCREEN_SYS, 0, 0, RECTW(g_rcScr), RECTH(g_rcScr));

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

static void splash_init(void) {
    splash_bar_postion = 0;
    splash_bar_direction = SPLASH_BAR_RIGHT;

    splash_adjust_x = (RECTW(g_rcScr) - SPLASH_W) / 2;
    splash_adjust_y = (RECTH(g_rcScr) - SPLASH_H)*3 / 5;
}

void splash_draw_framework (void)
{
    char text[64];
    RECT rc_text;
    PLOGFONT font, old_font;

    gal_pixel old_brush = SetBrushColor(HDC_SCREEN_SYS, PIXEL_black);
    gal_pixel old_text = SetTextColor(HDC_SCREEN_SYS, RGB2Pixel(HDC_SCREEN_SYS, 0x32, 0xa0, 0xe4));

    font = CreateLogFont ("rbf", "vgaoem", "ISO8859-1", 
            FONT_WEIGHT_REGULAR, 
            FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
            FONT_OTHER_AUTOSCALE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,      
            8, 0); 
    old_font = SelectFont(HDC_SCREEN_SYS, font);

    FillBox(HDC_SCREEN_SYS, 0 ,0, RECTW(g_rcScr), RECTH(g_rcScr));

    FillBoxWithBitmap(HDC_SCREEN_SYS, SPLASH_FEIMAN_X, SPLASH_FEIMAN_Y, 0, 0, &g_bitmap_feiman);
    FillBoxWithBitmap(HDC_SCREEN_SYS, SPLASH_FMSOFT_X, SPLASH_FMSOFT_Y, 0, 0, &g_bitmap_fmsoft);
    FillBoxWithBitmap(HDC_SCREEN_SYS, SPLASH_MINIGUI_X, SPLASH_MINIGUI_Y, 0, 0, &g_bitmap_minigui);
    FillBoxWithBitmap(HDC_SCREEN_SYS, SPLASH_BAR_X, SPLASH_BAR_Y, 0, 0, &g_bitmap_progressbar_bk);
    FillBoxWithBitmap(HDC_SCREEN_SYS, SPLASH_BAR_X, SPLASH_BAR_Y, 0, 0, &g_bitmap_progressbar);

    memset(text, 0, sizeof(text));
    SetRect(&rc_text, SPLASH_ID_X, SPLASH_ID_Y, SPLASH_ID_X+SPLASH_ID_W, RECTH(g_rcScr));

#ifdef _MG_PRODUCTID
    int customer_id = license_get_customer_id();
    if (customer_id <= 0) {
        sprintf(text, "SN: N/A");
    } else {
        sprintf(text, "SN: %d", customer_id);
    }
#else
    sprintf(text, "SN: N/A");
#endif

    SetBkMode(HDC_SCREEN_SYS, BM_TRANSPARENT);
    DrawText(HDC_SCREEN_SYS, text, -1, &rc_text, DT_TOP | DT_RIGHT | DT_SINGLELINE); 

    SelectFont(HDC_SCREEN_SYS, old_font);
    DestroyLogFont(font);

    SetBrushColor(HDC_SCREEN_SYS, old_brush);
    SetTextColor(HDC_SCREEN_SYS, old_text);
}

void splash_progress (void)
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
}

void splash_delay (void)
{
    int i;

    for (i = 0;
        __mg_quiting_stage > 0 && i < _MG_LICENSE_SPLASH_STEP;
        i++) {
        splash_progress();
        __mg_os_time_delay (_MG_LICENSE_SPLASH_MSEC);
    }
}

#else
#   define splash_init() /* NULL */
#endif /* _MG_ENABLE_SPLASH.*/

/***********************************************************
 * License common
 ***********************************************************/
static unsigned char const_trans_table[] = {
    0x16, 0xfe, 0xaf, 0x06, 0xd6, 0x9d, 0x54, 0x98,
    0x77, 0x36, 0xc1, 0xc7, 0x29, 0xf8, 0x64, 0x93,
    0x2e, 0x6b, 0x2a, 0x3c, 0x12, 0x13, 0x23, 0xc9,
    0xe2, 0xa3, 0xbd, 0xbe, 0xac, 0xdf, 0x91, 0xe5,
    0xdc, 0x4a, 0xc0, 0xb8, 0xd3, 0x2c, 0x61, 0x60,
    0x5b, 0x32, 0x37, 0x7e, 0xb7, 0x8e, 0x2b, 0x0b,
    0x19, 0x50, 0x40, 0xf3, 0x5f, 0x5d, 0x0d, 0x59,
    0x21, 0xb1, 0xf6, 0xb5, 0xe3, 0xb3, 0x94, 0xa6,
    0x31, 0x78, 0x84, 0xcc, 0x9c, 0x1b, 0x7b, 0x2f,
    0x46, 0xa9, 0x9b, 0xda, 0xee, 0xd0, 0xc5, 0x7f,
    0x4f, 0xc2, 0xa0, 0x9f, 0xe8, 0xb6, 0x3e, 0xbc,
    0xf5, 0xae, 0x1c, 0x7d, 0xca, 0xf2, 0x65, 0xe0,
    0x5a, 0x33, 0xfc, 0x3f, 0x49, 0x6d, 0xa1, 0x83,
    0xcb, 0x4d, 0x0f, 0xf4, 0x55, 0xec, 0x8a, 0x04,
    0x03, 0x7c, 0x86, 0x0e, 0xe4, 0xb4, 0x30, 0x99,
    0xa2, 0xdb, 0x43, 0x8d, 0x92, 0x11, 0xef, 0x8f,
    0x3a, 0xfb, 0xc6, 0x74, 0x0c, 0xb2, 0x97, 0x57,
    0x48, 0x69, 0x70, 0x15, 0x22, 0x67, 0x18, 0x24,
    0xf0, 0x82, 0xe7, 0x44, 0x9e, 0x53, 0x26, 0x81,
    0x1f, 0xc8, 0x63, 0x96, 0x00, 0x20, 0x4e, 0xe1,
    0x45, 0xd2, 0x75, 0x4c, 0x1d, 0x95, 0xbf, 0x52,
    0x05, 0xd8, 0x09, 0x1e, 0x73, 0x1a, 0xd5, 0xab,
    0x76, 0x08, 0xaa, 0xc4, 0x85, 0xb9, 0xd7, 0x8c,
    0x02, 0x58, 0x4b, 0x47, 0x89, 0xdd, 0x62, 0xa5,
    0xbb, 0x51, 0x10, 0xeb, 0x80, 0x79, 0xce, 0xfa,
    0x87, 0xf1, 0xcf, 0xfd, 0x6a, 0x07, 0x2d, 0x38,
    0xde, 0x41, 0x90, 0xb0, 0x25, 0x5e, 0xf7, 0xe6,
    0x42, 0x9a, 0xe9, 0xed, 0x6c, 0xc3, 0x88, 0xd1,
    0x6f, 0xd9, 0x7a, 0x0a, 0x68, 0x3d, 0x34, 0xcd,
    0xff, 0x28, 0x3b, 0x35, 0x01, 0xea, 0x39, 0x8b,
    0x14, 0x27, 0x56, 0xd4, 0x17, 0x72, 0xad, 0xa4,
    0xf9, 0x5c, 0x66, 0xa8, 0x6e, 0xba, 0xa7, 0x71,
};

static unsigned char decrypt_byte(unsigned int ch) {
    return const_trans_table[(ch-17)%256];
}

static void decrypt_mem(void *_data, int len) {
    unsigned char *data = (unsigned char *)_data;
    int i;
    for (i=0; i<len; ++i) {
        data[i] = decrypt_byte(data[i]);
    }
}

unsigned int LICENSE_ENCRYPTED_CONST_INT(unsigned int src){
    decrypt_mem(&src, sizeof(src));
    return src;
}

char *LICENSE_ENCRYPTED_CONST_STRING(unsigned char *src, int len){
    decrypt_mem(src, len);
    src[len] = 0;
    return (char *)src;
}

#if defined(_MG_ENABLE_SPLASH) || \
    defined(_MG_ENABLE_SCREENSAVER) || \
    defined(_MG_ENABLE_WATERMARK)

/* disable cryptool */
#undef ENCRYPTED

#ifdef ENCRYPTED
#   include "../sysres/license/c_files/key.c"
#endif

#include "../sysres/license/c_files/_splash_inner_res.c"
/* XXX: Other resource files are included in desktop.c event.c message.c zorder.c and window.c */
#if 0 /* XXX: WaterMark is REMOVED */
#ifdef _MG_ENABLE_LICENSE
#   include "../sysres/license/c_files/05_watermark-fmsoft.dat.c"
#   include "../sysres/license/c_files/06_watermark-minigui.dat.c"
#   include "../sysres/license/c_files/07_watermark-evaluation.dat.c"
#endif /* _MG_ENABLE_SPLASH */
#endif

#ifdef ENCRYPTED
#   include "rc4.c"
#   define FRAGMENT_SIZE 512
static unsigned char calc_sum(int len, const unsigned char *data) {
    unsigned char sum;
    int i;

    for (i=0, sum=0; i<len; ++i) {
        sum -= data[i];
    }

    return sum;
}

static int decrypted_len(int len) {
    return len - ((len-1) / FRAGMENT_SIZE + 1) * sizeof(unsigned char);
}

static int my_decrypt(RC4_KEY *key, int in_len, unsigned char *in) {
    int out_len = 0;
    int i;
    unsigned char sum;

    for (i=0; i<in_len; /* NULL */) {
        int n;

        RC4(key, sizeof(unsigned char), &in[i], (unsigned char *)&sum);
        i += sizeof(unsigned char);

        n = (in_len - i > FRAGMENT_SIZE - sizeof(unsigned char)) ? (FRAGMENT_SIZE - sizeof(unsigned char)) : (in_len - i);
        RC4(key, n, &in[i], &in[out_len]);

        if (calc_sum(n, &in[out_len]) != sum) {
            assert(0);
            return -1;
        }

        i += n;
        out_len += n;
    }

    assert(out_len == decrypted_len(in_len));
    return out_len;
}
#endif /* ENCRYPTED */

void license_create(void) {
    int i;

    for (i=0; i<LICENSE_BITMAP_NR; ++i) {
        unsigned char *data;
        int len;
#ifdef ENCRYPTED
        RC4_KEY rc4key;
#endif

        data = (unsigned char *)malloc(__mg_splash_splash_inner_res[i].data_len);
        memcpy(data, __mg_splash_splash_inner_res[i].data, __mg_splash_splash_inner_res[i].data_len);
        len = __mg_splash_splash_inner_res[i].data_len;

#ifdef ENCRYPTED
        RC4_set_key(&rc4key, sizeof(splash_crypto_key), splash_crypto_key);
        len = my_decrypt(&rc4key, len, data);
#endif

        LoadBitmapFromMem(HDC_SCREEN_SYS, &g_license_bitmaps[i], data, len, "png");
        free(data);
    }

    splash_init();

    watermark_init();
}

void license_destroy (void) {
    int i;
    for (i = 0; i < LICENSE_BITMAP_NR; i++) {
        UnloadBitmap(&g_license_bitmaps[i]);
    }
}

#endif /* _MG_ENABLE_SPLASH || _MG_ENABLE_SCREENSAVER || _MG_ENABLE_WATERMARK */

#if defined(_MG_ENABLE_SCREENSAVER) || defined(_MG_ENABLE_WATERMARK)
void license_on_input(void)
{
    s_tick_last_input = GetTickCount();

    if (screensaver_running == TRUE) {
        screensaver_running = FALSE;

#if defined(_MGRM_THREADS)
        {
            HWND activeWnd = GetActiveWindow();
            if (activeWnd) {
                SendNotifyMessage(activeWnd, MSG_CANCELSCREENSAVER, 0, 0);
            } else {
                screensaver_hide();
                SendNotifyMessage (HWND_DESKTOP, MSG_PAINT, 0, 0);
            }
        }
#else
        screensaver_hide();
#if defined(_MGRM_PROCESSES)
        dskRefreshAllClient (&g_rcScr);
#elif defined(_MGRM_STANDALONE)
        SendNotifyMessage (HWND_DESKTOP, MSG_PAINT, 0, 0);
#else
#   error
#endif

#endif

    }
}

void license_on_timeout(void) 
{
    static unsigned int old_tick_count;
    unsigned int current_tick_count;

#ifdef _MGRM_PROCESSES
    if (!mgIsServer) {
        return;
    }
#endif

    current_tick_count = GetTickCount();

    if (screensaver_running == TRUE) {
        /* screensaver is running, update the picture */
        if ((current_tick_count - old_tick_count) > _MG_LICENSE_SCREENSAVER_UPDATE_TIME) {
            old_tick_count = current_tick_count;
            screensaver_update();
        }
    } else {
        /* screensaver is not running, show it */
        if ((current_tick_count - s_tick_last_input) > _MG_LICENSE_SCREENSAVER_TIMEOUT) {
            screensaver_running = TRUE;
            screensaver_show();
        }
    }
}
#endif /* _MG_ENABLE_SCREENSAVER || _MG_ENABLE_WATERMARK */

/***********************************************************
 * Processor ID
 ***********************************************************/
#ifndef __TARGET_UNKNOWN__
void license_get_processor_id (void) {
    g_license_processor_id = LICENSE_PROCESSOR_ID_NONE;
#if defined(_WITH_TARGET_S3C6410) || defined(_WITH_TARGET_S3C2440) || defined(_WITH_TARGET_S3C2410)  \
    || defined (_WITH_TARGET_HI3560A)
    {
        int fd;
        unsigned char *addr;
        off_t SFR_offest;
        size_t cpuid_offest;
        unsigned char path[8] = "\x58\x1f\x6f\xc1\x58\x76\x6f\x76" /* "/dev/mem" */;

        fd = open(LICENSE_ENCRYPTED_CONST_STRING(path, sizeof(path)), O_RDWR);
        if (fd <= 0) {
            return;
        } 

#ifdef _WITH_TARGET_S3C6410
        SFR_offest = LICENSE_ENCRYPTED_CONST_INT(0x3CADA1AD /* 0x7E00F000 */);
        cpuid_offest = LICENSE_ENCRYPTED_CONST_INT(0xADADFD9F /* 0x00000118 */);
#elif _WITH_TARGET_HI3560A
        SFR_offest = LICENSE_ENCRYPTED_CONST_INT(0xD3BCADAD /* 0x101E0000 */);
        cpuid_offest = LICENSE_ENCRYPTED_CONST_INT(0xADAD8470 /* 0x00000EE0 */);
#else /* 2440, 2410 */
        SFR_offest = LICENSE_ENCRYPTED_CONST_INT(0x03ADADAD /* 0x56000000 */);
        cpuid_offest = LICENSE_ENCRYPTED_CONST_INT(0xADADADE4 /* 0x000000B0 */);
#endif

        addr = mmap(NULL, (4<<10), PROT_READ, MAP_SHARED, fd, SFR_offest); 

#if _WITH_TARGET_HI3560A
        g_license_processor_id = *(unsigned int*)(addr+cpuid_offest+12) << 24 |
                                 *(unsigned int*)(addr+cpuid_offest+8) << 16  |
                                 *(unsigned int*)(addr+cpuid_offest+4) << 8   |
                                 *(unsigned int*)(addr+cpuid_offest);
#else
        g_license_processor_id = *(unsigned int*)(addr+cpuid_offest);
#endif
        munmap(addr, (4<<10));
        close(fd);
    }
#else
//#   warning PROCESSOR_ID_verfication_is_DISABLED_on_this_target
#endif
}
#endif /* __TARGET_UNKNOWN__ */
