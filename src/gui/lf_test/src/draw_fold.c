/*
 * draw_fold.c
 * test sample for classic renderer draw_fold function.
 * wangjian<wangjian@minigui.org>
 * 2007-11-23
 */

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/mywindows.h>
/*
 * draw_fold:
 *  This function draw a fold by the color.
 * 
 * \param hdc : the HDC .
 * \param pRect : the point to the rectangle area to drawing.
 * \param color : the pen color.
 * \param is_opened : the status of the fold.
 * \return : 0 for succeed, other for failure.
 *
 * Author : wangjian<wangjian@minigui.org>
 * Date : 2007-11-23.
 */
static int draw_fold (HDC hdc, const RECT* pRect, DWORD color, BOOL is_opened)
{
    int i;
    int pen_width;
    int w, h;
    gal_pixel old_pen_color;

    w = pRect->right - pRect->left;
    h = pRect->bottom - pRect->top;
    if(w < 4 || h < 4) return -1;

    old_pen_color = SetPenColor(hdc, RGB2Pixel(hdc,GetRValue(color),
                GetGValue(color), GetBValue(color)));
    pen_width = (MIN(w,h)>>4) + 1;

    for(i = 0; i < pen_width; i++)
    {
        Rectangle(hdc, pRect->left + i, pRect->top + i, 
                    pRect->right - i, pRect->bottom - i);
        MoveTo(hdc, pRect->left + 2 * pen_width , 
                pRect->top + (h>>1) - (pen_width>>1) + i);
        LineTo(hdc, pRect->right - 2 * pen_width , 
                pRect->top + (h>>1) - (pen_width>>1) + i);
        if(!is_opened)
        {
            MoveTo(hdc, pRect->left + (w>>1) - (pen_width>>1) + i, 
                    pRect->top + 2 * pen_width);
            LineTo(hdc, pRect->left + (w>>1) - (pen_width>>1) + i,
                    pRect->bottom - 2 * pen_width);
        }
    }

    SetPenColor(hdc, old_pen_color);
    return 0;
}
 
static int TestProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    RECT rect;

    if(message == MSG_PAINT)
    {
        hdc = BeginPaint (hWnd);
        rect.left = 150;  rect.top = 190;
        rect.right = 166; rect.bottom = 206;
        Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
        draw_fold(hdc, &rect, 0x000000FF,TRUE);
        rect.left = 180;  rect.top = 190;
        rect.right = 204; rect.bottom = 214;
        Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
        draw_fold(hdc, &rect, 0x0000FF00,FALSE);
        rect.left = 210;  rect.top = 190;
        rect.right = 242; rect.bottom = 222;
        Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
        draw_fold(hdc, &rect, 0x00FF0000,FALSE);
        rect.left = 240;  rect.top = 190;
        rect.right = 304; rect.bottom = 254;
        Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
        draw_fold(hdc, &rect, 0x00000000,TRUE);
        EndPaint (hWnd, hdc);
        return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "classic lf : draw_fold testing ....!";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = TestProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = g_rcScr.right;
    CreateInfo.by = g_rcScr.bottom;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow (&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}


