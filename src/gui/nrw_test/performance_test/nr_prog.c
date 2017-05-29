/*
 * nr_prog.c
 * wangjian
 * 2008-03-03.
 */

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>


static int NrProgProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    int i;
    static int pos = 0;
    static MYBITMAP mybmp;
    static RGB *pal;
    static HWND prog;
    switch (message) 
    {
        case MSG_CREATE:
            {
                pal = (RGB *)malloc (256 * sizeof (RGB));
                LoadMyBitmap (&mybmp, pal, "prog.bmp");
                for(i = 0; i < 256; ++i)
                {
                    if(pal[i].r == 0xFC && pal[i].g == 0xFF &&  pal[i].b == 0xFB)
                    {
                        mybmp.transparent = i;
                        break;
                    }
                }
                SetTimer(hWnd, 101, 30);
                
                CreateWindowEx ("static", 
                        "Init the systerm ...", 
                        WS_VISIBLE | SS_SIMPLE, 
                        WS_EX_USEPARENTCURSOR,
                        IDC_STATIC, 
                        10, 10, 380, 16, hWnd, 0);
                
                prog = CreateWindowEx(CTRL_PROGRESSBAR, "",
                        WS_VISIBLE , 
                        WS_EX_NONE,
                        0,
                        ((480-mybmp.w)>>1), ((180-mybmp.h)>>1), mybmp.w, mybmp.h, 
                        hWnd, 0);
                SendMessage (prog, PBM_SETRANGE, 0, 1000);

#if 1
                if (!SetWindowMask(prog, &mybmp))
                    return -1;
#endif


                return 0;
            }
        
        case MSG_TIMER:
            {
                if (wParam != 101)
                    return 0;
                pos++;
                if(pos >= 100) pos = 0;
                SendMessage (prog, PBM_SETPOS, 10*pos, 0L);
                break;
            }
        case MSG_CLOSE:
            {
                UnloadMyBitmap(&mybmp);
                free(pal);
                KillTimer (hWnd, 101);
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
    JoinLayer(NAME_DEF_LAYER , "nr_prog" , 0 , 0);
#endif
    
    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION; 
    //CreateInfo.dwExStyle = WS_EX_TROUNDCNS | WS_EX_BROUNDCNS;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "non-regular progressbar demo ...";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = NrProgProc;
    CreateInfo.lx = 10;
    CreateInfo.ty = 10;
    CreateInfo.rx = 510;
    CreateInfo.by = 210;
    CreateInfo.iBkColor = PIXEL_green;
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

