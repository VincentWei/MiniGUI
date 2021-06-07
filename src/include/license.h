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
#ifndef _LICENSE_H
#define _LICENSE_H

#include "window.h"

#ifndef WIN32
#   define MG_LOCAL __attribute__((__visibility__("hidden")))
#else
#   define MG_LOCAL /* NULL */
#endif

#define _MG_ENABLE_LICENSE _MG_ENABLE_SPLASH

enum {
    SPLASH_MINIGUI,
    SPLASH_FMSOFT,
    SPLASH_FEIMAN,
    SPLASH_PROGRESSBAR,
    SPLASH_PROGRESSBAR_BK,
    LICENSE_BITMAP_NR
};

extern MG_LOCAL BITMAP g_license_bitmaps[];

#define g_bitmap_minigui        g_license_bitmaps[SPLASH_MINIGUI]
#define g_bitmap_fmsoft         g_license_bitmaps[SPLASH_FMSOFT]
#define g_bitmap_feiman         g_license_bitmaps[SPLASH_FEIMAN]
#define g_bitmap_progressbar    g_license_bitmaps[SPLASH_PROGRESSBAR]
#define g_bitmap_progressbar_bk g_license_bitmaps[SPLASH_PROGRESSBAR_BK]

MG_LOCAL void __mg_license_create(void);
MG_LOCAL void __mg_license_destroy(void);

#ifdef _MG_ENABLE_SCREENSAVER
MG_LOCAL void __mg_screensaver_create(void);
MG_LOCAL void __mg_screensaver_destroy(void);
#else
#   define __mg_screensaver_create() /* NULL */
#   define __mg_screensaver_destroy() /* NULL */
#endif

/*
 * Splash
 */
#ifdef _MG_ENABLE_SPLASH
MG_LOCAL void __mg_splash_draw_framework(void);
MG_LOCAL void __mg_splash_progress(void);
MG_LOCAL void __mg_splash_delay(void);
#else
#   define __mg_splash_draw_framework() /* NULL */
#   define __mg_splash_progress() /* NULL */
#   define __mg_splash_delay() /* NULL */
#endif /* endif _MG_ENABLE_SPLASH */

#define __mg_license_on_input() /* NULL */
#define __mg_license_on_timeout() /* NULL */
#define LICENSE_CHECK_CUSTIMER_ID() /* NULL */
#define license_get_processor_id() /* NULL */
#define LICENSE_SET_MESSAGE_OFFSET() /* NULL */
#define LICENSE_MODIFY_MESSAGE(x) /* NULL */

#endif /* LICENSE_H */
