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
** $Id$
**
** bmpf-test.c: Bitmap Font unit test.
**
** Copyright (C) 2003 ~ 2007 Feynman Software
**
** All right reserved by Feynman Software.
**
** Current maintainer: Liu Peng.
**
** Create date: 2007/09/23
*/

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/fixedmath.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "devfont.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "pixel_ops.h"
#include "cursor.h"
#include "drawtext.h"
#include "fixedmath.h"
#include "glyph.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
static PLOGFONT logfont[6];

static int GlyphTestWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    int x,y;
    PDC pdc;

    switch (message) {

        case MSG_CREATE:
            logfont[0] = CreateLogFont (FONT_TYPE_NAME_BITMAP_RAW, "SansSerif", "ISO8859-1", 
                        FONT_WEIGHT_REGULAR, FONT_SLANT_ITALIC, FONT_SETWIDTH_NORMAL,
                        FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_LINE, 
                        16, 0);
            logfont[1] = CreateLogFont (FONT_TYPE_NAME_BITMAP_VAR, "SansSerif", "ISO8859-1", 
                        FONT_WEIGHT_REGULAR, FONT_SLANT_ITALIC, FONT_SETWIDTH_NORMAL,
                        FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_LINE, 
                        16, 0);
            logfont[2] = CreateLogFont (FONT_TYPE_NAME_BITMAP_QPF, "SansSerif", "ISO8859-1", 
                        FONT_WEIGHT_REGULAR, FONT_SLANT_ITALIC, FONT_SETWIDTH_NORMAL,
                        FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_LINE, 
                        16, 0);
            logfont[3] = CreateLogFont (FONT_TYPE_NAME_BITMAP_UPF, "SansSerif", "ISO8859-1", 
                        FONT_WEIGHT_REGULAR, FONT_SLANT_ITALIC, FONT_SETWIDTH_NORMAL,
                        FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_LINE, 
                        16, 0);
            logfont[4] = CreateLogFont (FONT_TYPE_NAME_BITMAP_BMP, "SansSerif", "ISO8859-1", 
                        FONT_WEIGHT_REGULAR, FONT_SLANT_ITALIC, FONT_SETWIDTH_NORMAL,
                        FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_LINE, 
                        16, 0);
            logfont[5] = CreateLogFont (FONT_TYPE_NAME_SCALE_TTF, "SansSerif", "ISO8859-1", 
                        FONT_WEIGHT_REGULAR, FONT_SLANT_ITALIC, FONT_SETWIDTH_NORMAL,
                        FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_LINE, 
                        16, 0);
        break;

        case MSG_PAINT:
        {
            int ret, timespan, i;
            hdc = BeginPaint (hWnd);
            SetPenColor (hdc, PIXEL_red);

            pdc = __mg_check_ecrgn (hdc);
            pdc->rc_output.left = 5;
            pdc->rc_output.right = 1019;
            pdc->rc_output. top = 25;
            pdc->rc_output.bottom = 763;
            
#ifdef __USE_POSIX199309
            struct timespec start1, end1;
#else
            struct timeval start1, end1;
#endif


            //start rbf test
            SelectFont(hdc, logfont[0]);
#ifdef __USE_POSIX199309
            ret = clock_gettime (CLOCK_REALTIME, &start1);
#else
            ret = gettimeofday (&start1, NULL);
#endif
            for (i = 0; i < 1000; i++)
                _gdi_draw_one_glyph (pdc, 108, 1, 5, 39, &x, &y);

#ifdef __USE_POSIX199309
            ret = clock_gettime (CLOCK_REALTIME, &end1);
#else
            ret = gettimeofday (&end1, NULL);
#endif
            if (ret != 0)
            {
                printf ("ERROR: clock_gettime fail.\n");
                return -1;
            }

#ifdef __USE_POSIX199309
            timespan = (end1.tv_sec - start1.tv_sec) * 1000 +
                       (end1.tv_nsec - start1.tv_nsec) / 1000;
#else
            timespan = (end1.tv_sec - start1.tv_sec) * 1000 +
                       (end1.tv_usec - start1.tv_usec);
#endif

            printf ("FONT_TYPE_NAME_BITMAP_RAW, _gdi_draw_one_glyph time = %d\n",timespan);
            //end rbf test
            
            //start vbf test
            SelectFont(hdc, logfont[1]);
#ifdef __USE_POSIX199309
            ret = clock_gettime (CLOCK_REALTIME, &start1);
#else
            ret = gettimeofday (&start1, NULL);
#endif
            for (i = 0; i < 1000; i++)
                _gdi_draw_one_glyph (pdc, 108, 1, 25, 39, &x, &y);

#ifdef __USE_POSIX199309
            ret = clock_gettime (CLOCK_REALTIME, &end1);
#else
            ret = gettimeofday (&end1, NULL);
#endif
            if (ret != 0)
            {
                printf ("ERROR: clock_gettime fail.\n");
                return -1;
            }

#ifdef __USE_POSIX199309
            timespan = (end1.tv_sec - start1.tv_sec) * 1000 +
                       (end1.tv_nsec - start1.tv_nsec) / 1000;
#else
            timespan = (end1.tv_sec - start1.tv_sec) * 1000 +
                       (end1.tv_usec - start1.tv_usec);
#endif

            printf ("FONT_TYPE_NAME_BITMAP_VAR, _gdi_draw_one_glyph time = %d\n", timespan);
            //end vbf test

            //start qpf test
            SelectFont(hdc, logfont[2]);
#ifdef __USE_POSIX199309
            ret = clock_gettime (CLOCK_REALTIME, &start1);
#else
            ret = gettimeofday (&start1, NULL);
#endif
            for (i = 0; i < 1000; i++)
                _gdi_draw_one_glyph (pdc, 108, 1, 45, 39, &x, &y);

#ifdef __USE_POSIX199309
            ret = clock_gettime (CLOCK_REALTIME, &end1);
#else
            ret = gettimeofday (&end1, NULL);
#endif
            if (ret != 0)
            {
                printf ("ERROR: clock_gettime fail.\n");
                return -1;
            }

#ifdef __USE_POSIX199309
            timespan = (end1.tv_sec - start1.tv_sec) * 1000 +
                       (end1.tv_nsec - start1.tv_nsec) / 1000;
#else
            timespan = (end1.tv_sec - start1.tv_sec) * 1000 +
                       (end1.tv_usec - start1.tv_usec);
#endif

            printf ("FONT_TYPE_NAME_BITMAP_QPF, _gdi_draw_one_glyph time = %d\n", timespan);
            //end qpf test
            
            //start upf test
            SelectFont(hdc, logfont[3]);
#ifdef __USE_POSIX199309
            ret = clock_gettime (CLOCK_REALTIME, &start1);
#else
            ret = gettimeofday (&start1, NULL);
#endif
            for (i = 0; i < 1000; i++)
                _gdi_draw_one_glyph (pdc, 108, 1, 65, 39, &x, &y);

#ifdef __USE_POSIX199309
            ret = clock_gettime (CLOCK_REALTIME, &end1);
#else
            ret = gettimeofday (&end1, NULL);
#endif
            if (ret != 0)
            {
                printf ("ERROR: clock_gettime fail.\n");
                return -1;
            }

#ifdef __USE_POSIX199309
            timespan = (end1.tv_sec - start1.tv_sec) * 1000 +
                       (end1.tv_nsec - start1.tv_nsec) / 1000;
#else
            timespan = (end1.tv_sec - start1.tv_sec) * 1000 +
                       (end1.tv_usec - start1.tv_usec);
#endif

            printf ("FONT_TYPE_NAME_BITMAP_UPF, _gdi_draw_one_glyph time = %d\n", timespan);
            //end upf test
            
            //start bmp test
            SelectFont(hdc, logfont[4]);
#ifdef __USE_POSIX199309
            ret = clock_gettime (CLOCK_REALTIME, &start1);
#else
            ret = gettimeofday (&start1, NULL);
#endif
            for (i = 0; i < 1000; i++)
                _gdi_draw_one_glyph (pdc, 108, 1, 85, 39, &x, &y);

#ifdef __USE_POSIX199309
            ret = clock_gettime (CLOCK_REALTIME, &end1);
#else
            ret = gettimeofday (&end1, NULL);
#endif
            if (ret != 0)
            {
                printf ("ERROR: clock_gettime fail.\n");
                return -1;
            }

#ifdef __USE_POSIX199309
            timespan = (end1.tv_sec - start1.tv_sec) * 1000 +
                       (end1.tv_nsec - start1.tv_nsec) / 1000;
#else
            timespan = (end1.tv_sec - start1.tv_sec) * 1000 +
                       (end1.tv_usec - start1.tv_usec);
#endif

            printf ("FONT_TYPE_NAME_BITMAP_BMP, _gdi_draw_one_glyph time = %d\n", timespan);
            //end bmp test
            
            //start ttf test
            SelectFont(hdc, logfont[5]);
#ifdef __USE_POSIX199309
            ret = clock_gettime (CLOCK_REALTIME, &start1);
#else
            ret = gettimeofday (&start1, NULL);
#endif
            for (i = 0; i < 1000; i++)
                _gdi_draw_one_glyph (pdc, 108, 1, 105, 39, &x, &y);

#ifdef __USE_POSIX199309
            ret = clock_gettime (CLOCK_REALTIME, &end1);
#else
            ret = gettimeofday (&end1, NULL);
#endif
            if (ret != 0)
            {
                printf ("ERROR: clock_gettime fail.\n");
                return -1;
            }

#ifdef __USE_POSIX199309
            timespan = (end1.tv_sec - start1.tv_sec) * 1000 +
                       (end1.tv_nsec - start1.tv_nsec) / 1000;
#else
            timespan = (end1.tv_sec - start1.tv_sec) * 1000 +
                       (end1.tv_usec - start1.tv_usec);
#endif

            printf ("FONT_TYPE_NAME_SCALE_TTF, _gdi_draw_one_glyph time = %d\n", timespan);
            //end ttf test
            
            EndPaint (hWnd, hdc);
        }
            return 0;

        case MSG_KEYDOWN: {
            break;
        }

        case MSG_CLOSE:
            DestroyMainWindow (hWnd);
            DestroyLogFont(logfont[0]);
            DestroyLogFont(logfont[1]);
            DestroyLogFont(logfont[2]);
            DestroyLogFont(logfont[3]);
            DestroyLogFont(logfont[4]);
            DestroyLogFont(logfont[5]);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}


int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "helloworld" , 0 , 0);
#endif


    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Glyph font test";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = GlyphTestWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = g_rcScr.right;
    CreateInfo.by = g_rcScr.bottom;
    //CreateInfo.iBkColor = COLOR_blue;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

