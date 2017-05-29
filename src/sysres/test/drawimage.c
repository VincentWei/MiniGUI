/* 
** $Id: drawicon.c 465 2008-02-14 08:09:43Z wangjian $
**
** Listing 10.1
**
** drawicon.c: Sample program for MiniGUI Programming Guide
**      Demo of loading and drawing icon
**
** Copyright (C) 2004 ~ 2007 Feynman Software.
**
** License: GPL
*/

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define _MGLF_RDR_FLAT 1

static char* image_names [] = {
    "MiniGUI256.bmp", 
#ifdef _MGLF_RDR_TINY
    "bg_tiny.bmp",
#endif
    "classic_check_button.bmp", 
    "classic_radio_button.bmp", 
#ifdef _MGLF_RDR_FASHION
    "fashion_check_btn.bmp", 
    "fashion_radio_btn.bmp", 
#endif
#if defined (_MGLF_RDR_FLAT) || defined (_MGLF_RDR_TINY)
    "flat_check_button.bmp", 
    "flat_check_button.bmp", 
#endif
    "shurufa-flat.bmp",

#ifdef _MGLF_RDR_SKIN
    "skin_arrows.gif", 
    "skin_arrows_shell.bmp", 
    "skin_bborder.bmp", 
    "skin_bkgnd.bmp", 
    "skin_caption.gif", 
    "skin_checkbtn.bmp",  
    "skin_cpn_btn.gif", 
    "skin_header.bmp", 
    "skin_lborder.bmp",  
    "skin_pb_hchunk.bmp", 
    "skin_pb_htrack.gif", 
    "skin_pb_vchunk.bmp", 
    "skin_pb_vtrack.bmp", 
    "skin_pushbtn.gif", 
    "skin_radiobtn.bmp",  
    "skin_rborder.bmp", 
    "skin_sb_arrows.bmp", 
    "skin_sb_hshaft.bmp", 
    "skin_sb_hthumb.bmp", 
    "skin_sb_vshaft.bmp", 
    "skin_sb_vthumb.bmp", 
    "skin_tab.bmp", 
    "skin_tb_horz.gif", 
    "skin_tb_vert.gif", 
    "skin_tborder.bmp", 
    "skin_tbslider_h.bmp", 
    "skin_tbslider_v.bmp", 
    "skin_tree.bmp", 

#endif
    "test_error.bmp"


};

static PBITMAP bmps[TABLESIZE(image_names)];
static int left = 0, top = 0;

static int DrawiconWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message) {
    case MSG_CREATE: {
        int i = 0;
        for (i = 0; i < TABLESIZE(image_names); i++) {
            bmps[i] = LoadBitmapFromRes (HDC_SCREEN, image_names[i]);
            if (!bmps[i])
                fprintf (stderr, "Load image failure!");

        }
        break;
    }

    case MSG_PAINT: {
        int i;
        hdc = BeginPaint(hWnd);
        for (i = 0; i < TABLESIZE(image_names); i++) {
            if (bmps[i] != 0){
                if (left+bmps[i]->bmWidth > g_rcScr.right) {
                    left = 0;
                    top += bmps[i]->bmHeight; 
                }
                
                FillBoxWithBitmap(hdc, left , top, 0, 0, bmps[i]);
                left+=bmps[i]->bmWidth;
            }
            
        }
        EndPaint(hWnd, hdc);
        return 0;
    }
    case MSG_CLOSE: {
        int i = 0;
        for (i = 0; i < TABLESIZE(image_names); i++)
        {
            UnloadBitmap(bmps[i]);
            free (bmps[i]);
        }
        DestroyMainWindow (hWnd);
        PostQuitMessage (hWnd);
        return 0;
    }
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain (int args, const char* arg[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "test-image" , 0 , 0);
#endif
    
    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Test of image res";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = DrawiconWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 1024;
    CreateInfo.by = 768;
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

