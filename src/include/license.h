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
#ifndef _LICENSE_H
#define _LICENSE_H

#include "window.h"

#ifndef WIN32
#   define MG_LOCAL __attribute__((__visibility__("hidden")))
#else
#   define MG_LOCAL /* NULL */
#endif

/*
 * Common
 */
#if defined(_MG_ENABLE_SPLASH) || \
    defined(_MG_ENABLE_SCREENSAVER) || \
    defined(_MG_ENABLE_WATERMARK)

#define _MG_ENABLE_LICENSE 1

enum {
    SPLASH_MINIGUI,
    SPLASH_FMSOFT,
    SPLASH_FEIMAN,
    SPLASH_PROGRESSBAR,
    SPLASH_PROGRESSBAR_BK,
#if 0
    WATERMARK_1,
    WATERMARK_2,
    WATERMARK_3,
#endif /* _MG_ENABLE_WATERMARK */
    LICENSE_BITMAP_NR
};

extern MG_LOCAL BITMAP g_license_bitmaps[];

#define g_bitmap_minigui        g_license_bitmaps[SPLASH_MINIGUI]
#define g_bitmap_fmsoft         g_license_bitmaps[SPLASH_FMSOFT]
#define g_bitmap_feiman         g_license_bitmaps[SPLASH_FEIMAN]
#define g_bitmap_progressbar    g_license_bitmaps[SPLASH_PROGRESSBAR]
#define g_bitmap_progressbar_bk g_license_bitmaps[SPLASH_PROGRESSBAR_BK]

MG_LOCAL void license_create(void);
MG_LOCAL void license_destroy(void);

#else
#   define license_create() /* NULL */
#   define license_destroy() /* NULL */
#endif /* endif _MG_ENABLE_SPLASH or _MG_ENABLE_SCREENSAVER or _MG_ENABLE_WATERMARK*/


#ifdef _MG_ENABLE_SCREENSAVER
MG_LOCAL void screensaver_create(void);
MG_LOCAL void screensaver_destroy(void);
#else
#   define screensaver_create() /* NULL */
#   define screensaver_destroy() /* NULL */
#endif

/*
 * Splash
 */
#ifdef _MG_ENABLE_SPLASH 
MG_LOCAL void splash_draw_framework(void);
MG_LOCAL void splash_progress(void);
MG_LOCAL void splash_delay(void);
#else
#   define splash_draw_framework() /* NULL */
#   define splash_progress() /* NULL */
#   define splash_delay() /* NULL */
#endif /* endif _MG_ENABLE_SPLASH */

/*
 * ScreenSaver & Watermark
 */
#if defined(_MG_ENABLE_SCREENSAVER) || defined(_MG_ENABLE_WATERMARK)
MG_LOCAL void license_on_input(void);
MG_LOCAL void license_on_timeout(void);
#else
#   define license_on_input() /* NULL */
#   define license_on_timeout() /* NULL */
#endif /* _MG_ENABLE_SCREENSAVER || _MG_ENABLE_WATERMARK */

/*
 * Product ID
 */
#ifdef _MG_PRODUCTID
typedef struct _product_id {
    /* Fixed 8-byte code, help us to locate the struct in .so file */
    unsigned char prefix[8];
    /* the ID of the customer */
    int customer_id;
    /* svn version */
    int version;
    /* When does we compile the .so, in seconds */
    int compile_date;
    /* The size of the .so file */
    int file_size;
    /* The check sum of the .so file */
    unsigned char checksum[16];
} product_id_t;

#define PRODUCT_ID_PREFIX 0xca, 0x3f, 0x2b, 0x43, 0x00, 0x33, 0xb0, 0xc3

MG_LOCAL int license_get_customer_id(void);

#endif /* _MG_PRODUCTID */

/*
 * Tools to encrypt const variables
 */
MG_LOCAL unsigned int LICENSE_ENCRYPTED_CONST_INT(unsigned int);
MG_LOCAL char *LICENSE_ENCRYPTED_CONST_STRING(unsigned char *, int);

/*
 * Processor ID
 */
#ifdef _MG_PRODUCTID
#   define LICENSE_CHECK_CUSTIMER_ID() \
        do { \
            if (! license_get_customer_id()) { \
                return ERR_INVALID_ARGS; \
            } \
        } while(0)
#else
#   define LICENSE_CHECK_CUSTIMER_ID() /* NULL */
#endif

#ifdef __TARGET_UNKNOWN__

#   define license_get_processor_id() /* NULL */
#   define LICENSE_SET_MESSAGE_OFFSET() /* NULL */
#   define LICENSE_MODIFY_MESSAGE(x) /* NULL */

#else /* not __TARGET_UNKNOWN__ */

extern MG_LOCAL unsigned int g_license_processor_id; /* window.c */
MG_LOCAL void license_get_processor_id (void);
#define LICENSE_PROCESSOR_ID_NONE (0xFC1D8B2A)

extern MG_LOCAL unsigned int g_license_message_offset; /* zorder.c */

#if defined (_WITH_TARGET_S3C6410)
#    define REAL_PROCESSOR_ID LICENSE_ENCRYPTED_CONST_INT(0x1AE2FDFD /* 0x36410101 */)
#elif defined (_WITH_TARGET_S3C2440)
#    define REAL_PROCESSOR_ID LICENSE_ENCRYPTED_CONST_INT(0x3AA4FDFD /* 0x32440101 */)
#elif defined (_WITH_TARGET_S3C2410)
#    define REAL_PROCESSOR_ID LICENSE_ENCRYPTED_CONST_INT(0x3AE2FDFD /* 0x32410101 */)
#elif defined (_WITH_TARGET_HI3560A)
#    define REAL_PROCESSOR_ID LICENSE_ENCRYPTED_CONST_INT(0xFC38C9AD /* 0x35600200 */)
#else
#    define REAL_PROCESSOR_ID LICENSE_PROCESSOR_ID_NONE
#endif

#define LICENSE_SET_MESSAGE_OFFSET() do {g_license_message_offset=g_license_processor_id-(REAL_PROCESSOR_ID);}while (0)

#define LICENSE_MODIFY_MESSAGE(pMsg) \
    do { \
        pMsg->message += g_license_message_offset; \
        pMsg->hwnd -= g_license_message_offset / 2; \
    } while (0)

#endif /* __TARGET_UNKNOWN__ */

#endif /* LICENSE_H */
