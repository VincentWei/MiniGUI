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
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

//#define ARABIC_DEBUG
#ifdef ARABIC_DEBUG
    #define DBGLOG(s)      do { if (1) { fprintf(stderr, s); } } while (0)
    #define DBGLOG2(s, t1)  do { if (1) { fprintf(stderr, s, t1); } } while (0)
    #define DBGLOG3(s, t1,t2)  do { if (1) { fprintf(stderr, s, t1,t2); } } while (0)
    #define DBGLOG4(s, t1,t2,t3)  do { if (1) { fprintf(stderr, s, t1,t2,t3); } } while (0)
#else
    #define DBGLOG(s)
    #define DBGLOG2(s, t1)
    #define DBGLOG3(s, t1, t2)
    #define DBGLOG4(s, t1, t2, t3)
#endif

char linebuf[4096];
char filebuf[4096*4];
char filename[256];
char fontname[50];

BOOL textout = TRUE;
RECT arabic_rc = {0, 0, 640, 480};

static char msg_text [256] = "Show text with TextOut.";

static FILE *fp = NULL;
static PLOGFONT logfont_iso8859_6;
#ifdef HEXSTRING
//static char arabic_text[] = {0xc7,0xe4,0xe1,0xd1,0xea,0xe2,0xa0,0xc7,0xe4,0xca,0xe2,0xe6,0xea,0x00};
//static char arabic_text[] = {0xd9, 0xe2,0xe4,0xc7,0xcf, 0x0};
//static char arabic_text[] = {0xcb, 0xca, 0xc7, 0xd9, 0x64, 0x68, 0x20,0x0};
static char arabic_text[] = {0xf1, 0xee, 0x0};
#endif
static int line_num = 0;

static int reset_fp_pos()
{
    arabic_rc.left   = 0;
    arabic_rc.top    = 10;
    arabic_rc.right  = g_rcScr.right - 24;
    arabic_rc.bottom = g_rcScr.bottom;
    line_num = 0;

    if(!fp){
        DBGLOG2("============================= arabic_test %s===============================\n", filename);
        fp = fopen(filename, "rb");
        if (!fp) {
            DBGLOG2("cannot open %s\n", filename);
            return 0;
        }
    }
    if(fp){
        fseek(fp, 0, SEEK_SET);
    }
    return 1;
}

static void print_hexstr(const unsigned char* str, int len)
{
    int m = 0;
    DBGLOG2("\n========================%d============================\n", ++line_num);
    DBGLOG("Read one line from file start.\n");
    DBGLOG("   ");
    for(m = 0; m < len; m++){
        if(m && !(m%16)) DBGLOG("\n   ");
        DBGLOG2("0x%02x ", str[m]);
    }
    DBGLOG("\nRead one line from file end.\n");
    DBGLOG("====================================================\n");
}

static char* read_one_line(BOOL* read_over)
{
    int i = 0;
    //char c = 0;
    int c = 0;
    if(fp == NULL){
        *read_over = TRUE;
        return NULL;
    }
    memset(linebuf, 0, 4096);
    //while((c=fgetc(fp))!=EOF){
    while(!(*read_over = feof(fp))) {
        c = fgetc(fp);
        if (c == -1 || c == 0xa || c == 0xd) {
            break;
        }
        linebuf[i] = c;
        i++;
    }
    if(i > 0) {
        print_hexstr((BYTE*)linebuf, i);
        return linebuf;
    }
    else{
    //    fseek(fp, 0, SEEK_SET);
        return NULL;
    }
}

void read_total_file( void )
{
    FILE* fp = fopen(filename, "rb");
    long pos;
    fseek (fp, 0, SEEK_END);
    pos = ftell(fp);
    fseek (fp, 0, SEEK_SET);
    fread (filebuf, pos, 1, fp);
    fclose (fp);
}

static int HelloWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    switch (message) {
        case MSG_CREATE:
#ifndef ARABIC_UNICODE
            logfont_iso8859_6 = CreateLogFont("vbf", fontname, "ISO8859-6", FONT_WEIGHT_REGULAR,
                    FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
                    FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE,
                    FONT_STRUCKOUT_NONE, 18+8, 0);
#else
            logfont_iso8859_6 =
                CreateLogFont("ttf", "DejaVuSans", "ISO8859-6", FONT_WEIGHT_REGULAR,
                        FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
                        FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE,
                        FONT_STRUCKOUT_NONE, 20, 0);

#endif
            strcat(msg_text, filename);
            break;
        case MSG_LBUTTONDOWN:
            textout = TRUE;
            strcpy (msg_text, "Show text with TextOut.");
            strcat(msg_text, filename);
            InvalidateRect (hWnd, NULL, TRUE);
            break;

        case MSG_MOUSEMOVE:
            {
                /*
                int x_pos = LOSWORD (lParam);
                int y_pos = HISWORD (lParam);
                DBGLOG3("mouse in <%d, %d>\n", x_pos, y_pos);
                */
            }
            break;
        case MSG_RBUTTONDOWN:
            textout = FALSE;
            strcpy (msg_text, "Show text with DrawText.");
            strcat(msg_text, filename);
            InvalidateRect (hWnd, NULL, TRUE);
            break;

        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            //SetBkColor (hdc, COLOR_green);
            SelectFont(hdc, logfont_iso8859_6);
            SetWindowCaption(hWnd, msg_text);
            SetTextAlign(hdc, TA_RIGHT);
            //SetTextAlign(hdc, TA_LEFT);
            reset_fp_pos();
            {
                char* pline = NULL;
                int i = 0;
                int line_height = GetFontHeight(hdc);
                BOOL read_over = FALSE;
                while(1){
                    pline = read_one_line(&read_over);
                    if(!pline && !read_over) continue;
                    line_height = line_height; 
                    if(textout == TRUE) {
#ifdef HEXSTRING
                        TextOut(hdc, 100, 10, arabic_text);
#else
                        if(GetTextAlign(hdc) == TA_RIGHT)
                            TextOut(hdc, arabic_rc.right, 10+i*line_height, pline);
                        else
                            TextOut(hdc, arabic_rc.left, 10+i*line_height, pline);
#endif
                    }
                    else{
                        int height = DrawText(hdc, pline, -1, &arabic_rc, DT_RIGHT);
                        arabic_rc.top += height;
                    }
                    i++;
                    if(read_over) break;
                }
            }
            EndPaint (hWnd, hdc);
            return 0;
        case MSG_CLOSE:
            if(fp) fclose(fp);
            DestroyMainWindow (hWnd);
            DestroyLogFont(logfont_iso8859_6);
            PostQuitMessage (hWnd);
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

    if(argc >= 2){
        strcpy(filename, argv[1]);
        if(!reset_fp_pos()) return -1;
    }
    else{
        //DBGLOG("usage:./arabic_test arabic_text.txt\n");
        fprintf(stderr, "usage:./arabic_test arabic_text.txt\n");
        return -1;
    }
    if(argc >= 3){
        if(argv[2])
            strcpy(fontname, argv[2]);
    }
    else
        strcpy(fontname, "naskhi");

    CreateInfo.dwStyle = 
        WS_VISIBLE | WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    CreateInfo.dwExStyle = WS_EX_NONE;
    //CreateInfo.spCaption = "Hello, world!";
    CreateInfo.spCaption = "";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = HelloWinProc;
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
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup (hMainWnd);

    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

