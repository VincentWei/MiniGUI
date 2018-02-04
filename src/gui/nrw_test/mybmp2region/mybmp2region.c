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
 * mybmp2region_test.c
 * wangjian
 * 2008-02-19.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define ID_NEXT 101

static int loop = 0;
static MYBITMAP mybmp;
static BITMAP bmp;
static RGB* pal;
static PCLIPRGN region;
static char *bmp_file[5] = { "11.bmp", "22.bmp", "33.bmp", "44.bmp", "55.bmp"};

/**
 * CalcXYBannedRects:
 *   this function calculates x-y-banned rectangles of MYBITMAP 
 *   which is 8 bits per pixels.
 *
 * param hwnd           : the window handle
 * param new_mask       : MYBITMAP with 8 bits per pixels
 * param rect_size(out) : number of the x-y-banned rectangles
 *
 * return               : return NULL on failure, return the x-y-banned 
 *                        rectangles on success.
 *
 * note                 : You are responsible to free the x-y-banned rectangles
 *
 * Author               : NuohuaZhou
 * Data                 : 2008-02-20
 */

#define XYBANNED_RECT_NR_INIT   10
#define XYBANNED_RECT_NR_INC    10

static RECT * CalcXYBannedRects (HWND hwnd, 
        const MYBITMAP * new_mask, int * rect_size)
{
    Uint32 x, y;
    Uint8 bitspp; 
    int max_index, max_calloced_nr;
    int prev_pos_from, prev_pos_to, next_pos_from, next_pos_to;
    int combined_index, i;

    POINT section;
    RECT * xybanned_rects;

    Uint32 pixel;
    BOOL ismask, combined;

    if (rect_size)
        *rect_size = 0;

    if (!(new_mask && new_mask->h > 0))
    {
        return NULL;
    }

    bitspp = new_mask->depth;
    if (bitspp != 8)
    {
        printf ("ERROR: MYBITMAP is not 8 bits per pixel\n");
        return NULL;
    }

    max_calloced_nr = XYBANNED_RECT_NR_INIT;
    xybanned_rects = (RECT *) calloc (max_calloced_nr, sizeof (RECT));
    if (!xybanned_rects)
    {
        printf ("ERROR: fail to calloc xybanned_rects\n");
        return NULL;
    }

    /** calculate mask section at first line */

    max_index   = 0;

    /** not transparent */
    ismask      = FALSE;

    /** get x-y-banned rectangles by scanning the first line */
    for (x = 0; x < new_mask->w; ++x)
    {
        pixel = new_mask->bits[x];

        /** compare transparent pixel */
        if (pixel != new_mask->transparent)
        {
            /** from transparent to mask */
            if (!ismask)
            {
                ismask = TRUE;
                xybanned_rects[max_index].left = x;
            }
        }
        else
        {
            /** from mask to transparent */
            if (ismask)
            {
                ismask = FALSE;

                /** add rectangle of first line to x-y- banned rects */

                xybanned_rects[max_index].right = x - 1;
                xybanned_rects[max_index].top = 0;
                xybanned_rects[max_index].bottom = 1;

                ++max_index;

                if (max_index >= max_calloced_nr)
                {
                    max_calloced_nr += XYBANNED_RECT_NR_INC;
                    xybanned_rects = (RECT *) realloc (xybanned_rects, 
                            max_calloced_nr * sizeof (RECT));
                    if (!xybanned_rects)
                    {
                        printf ("ERROR: not enought memory!\n");
                        return NULL;
                    }
                }
            }// if (ismask)
        }
    }

    if (ismask)
    {
        xybanned_rects[max_index].right = new_mask->w;
        xybanned_rects[max_index].top = 0;
        xybanned_rects[max_index].bottom = 1;
    }

    /** record index of x-y-banned rectangles at the first line */

    next_pos_from = 0;
    next_pos_to   = max_index;

    /** union x-y-banned rectangles from the second line to the last line */

    for (y = 1; y < new_mask->h; ++y)
    {
        ismask           = FALSE;
        section.x        = 0;
        section.y        = 0;
        combined         = FALSE;
        prev_pos_from    = next_pos_from;
        prev_pos_to      = next_pos_to;

        for (x = 0; x < new_mask->w; ++x)
        {
            pixel = new_mask->bits[y * new_mask->pitch + x];

            /** compare transparent pixel */
            if (pixel != new_mask->transparent)
            {
                /** from transparent to mask */
                if (!ismask)
                {
                    ismask = TRUE;
                    section.x = x;
                }
            }
            else
            {
                /** from mask to transparent */
                if (ismask)
                {
                    ismask = FALSE;
                    section.y = x - 1;
                    combined_index = -1;

                    /** combine x-y-banned rectangles */

                    if (prev_pos_from <= max_index && prev_pos_to <= max_index)
                    {
                        for (i = prev_pos_from; i <= prev_pos_to; ++i)
                        {
                            /** if joined */
                            if (y == xybanned_rects[i].bottom + 1)
                            {
                                /** if same left and right */
                                if (section.x == xybanned_rects[i].left &&
                                    section.y == xybanned_rects[i].right)
                                {
                                    ++xybanned_rects[i].bottom;
                                    combined_index = i;
                                    break;
                                }
                            }
                        }
                    }

                    /** add a new x-y-banned rectangle when failing to combine */
                    if (-1 == combined_index)
                    {
                        /** increase max index */
                        ++ max_index;

                        if (max_index >= max_calloced_nr)
                        {
                            max_calloced_nr += XYBANNED_RECT_NR_INC;
                            xybanned_rects = (RECT *) realloc (xybanned_rects, 
                                    max_calloced_nr * sizeof (RECT));
                            if (!xybanned_rects)
                            {
                                printf ("ERROR: not enought memory!\n");
                                return NULL;
                            }
                        }

                        xybanned_rects[max_index].left   = section.x;
                        xybanned_rects[max_index].top    = y - 1;
                        xybanned_rects[max_index].right  = section.y;
                        xybanned_rects[max_index].bottom = y;
                    }
                    else
                    {
                        /** record the minimum index when successing to combine */
                        if (!combined)
                        {
                            combined = TRUE;
                            next_pos_from = combined_index; 
                        }
                        else
                        {
                            if (combined_index < next_pos_from)
                                next_pos_from = combined_index;
                        }
                    }
                }// if (ismask)
            }

        }// for (x = 0; x < new_mask->w; ++x)
        
        /** adjust the minimum and maximum index */

        if ((section.y == 0) && (section.x == 0))
        {
            /** the scaned line is completely transparent */
            next_pos_to = next_pos_from = max_index + 1;
        }
        else
        {
            /** the scaned line is not transparent */
            next_pos_to = max_index;
        }
    }// for (y = 1; y < new_mask->h; ++y)

    if (rect_size)
        *rect_size = max_index;

    return xybanned_rects;
}


static int LoadMyBmpWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    int i;
    int nr_rect;
    RECT *rc_list;
    switch (message) 
    {
        case MSG_CREATE:
            {
                pal = (RGB *) malloc (256 * sizeof(RGB));
                region = CreateClipRgn();

                CreateWindow(CTRL_BUTTON, "Start",
                        WS_VISIBLE | BS_DEFPUSHBUTTON, 
                        ID_NEXT,
                        270, 310, 60, 30, 
                        hWnd, 0);

                if (LoadMyBitmap (&mybmp, pal, bmp_file[loop]))
                    return -1;

                return 0;
            }
        case MSG_COMMAND:
            {
                if (wParam == ID_NEXT) 
                {
                    ++loop;
                    if(loop > 4) loop = 0;

                    UnloadMyBitmap (&mybmp);
                    LoadMyBitmap (&mybmp, pal, bmp_file[loop]);
                    for(i = 0; i < 256; ++i)
                    {
                        if(pal[i].r == 0xFC && pal[i].g == 0xFF &&  pal[i].b == 0xFB)
                        {
                            mybmp.transparent = i;
                            break;
                        }
                    }

                    rc_list = CalcXYBannedRects(hWnd, &mybmp, &nr_rect);
                    if (!rc_list)
                    {
                        printf ("ERROR: fail to CalcXYBannedRects, loop = %d\n", loop);
                        break;
                    }
                    EmptyClipRgn (region);          
					HDC hdc = GetClientDC(hWnd);
                    for(i = 0; i < nr_rect; ++i)
                    {
                        printf("RECT :[%-3d %-3d %-3d %-3d]\n",
								rc_list[i].left, rc_list[i].top,
                                rc_list[i].right, rc_list[i].bottom);
						Rectangle(hdc, rc_list[i].left, rc_list[i].top,
                                rc_list[i].right, rc_list[i].bottom);
                        AddClipRect(region, &rc_list[i]);
                    }
					ReleaseDC(hdc);

                    free (rc_list);
                    rc_list = NULL;

                    printf ("number of x-y-banned rectangles is %d\n", nr_rect);
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
            break;

        
        case MSG_PAINT:
            {
                hdc = BeginPaint (hWnd);
                MoveTo(hdc, 0, 0);
                LineTo(hdc, 600, 0);
                MoveTo(hdc, 300, 0);
                LineTo(hdc, 300, 300);
                MoveTo(hdc, 0, 300);
                LineTo(hdc, 600, 300);
                TextOut(hdc, 100, 301, "Region"); 
                TextOut(hdc, 400, 301, "MyBitmap"); 

                ExpandMyBitmap (hdc, &bmp, &mybmp, pal, 0);
                printf("mybitmap->depth: %d.\n", mybmp.depth);
                FillBoxWithBitmap (hdc, 301, 1, bmp.bmWidth, bmp.bmHeight, &bmp);

                SelectClipRegion (hdc, region);
                SetBrushColor(hdc, PIXEL_blue); 
                FillBox(hdc, 0, 0, 300, 300);

                EndPaint (hWnd, hdc);
                return 0;
            }
        case MSG_CLOSE:
            {
                free (rc_list);
                rc_list = NULL;

                DestroyClipRgn (region);
                region = NULL;
                UnloadMyBitmap (&mybmp);
                DestroyMainWindow (hWnd);
                PostQuitMessage (hWnd);
                return 0;
            }
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "loadbmp" , 0 , 0);
#endif
    
    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "bmp2region testing ...";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = LoadMyBmpWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 600;
    CreateInfo.by = 400;
    CreateInfo.iBkColor = PIXEL_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow (hMainWnd, SW_SHOWNORMAL);

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

