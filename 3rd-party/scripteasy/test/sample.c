/* 
** $Id: sample.c,v 1.38 2009-01-15 07:56:45 houhuihua Exp $
**
** sample.c: Sample program for komoxo scripteasy font.
**
** Copyright (C) 2008 ~ 2009 Feynman Software.
*/

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

static char welcome_text [10240];
static char msg_text [5120];
static RECT welcome_rc = {20, 100, 600, 400};
static RECT msg_rc = {10, 100, 600, 400};

static const char* syskey = "";

static int last_key = -1;
static int last_key_count = 0;

static void make_welcome_text (void)
{
    const char* sys_charset = GetSysCharset (TRUE);
    const char* format;

    if (sys_charset == NULL)
        sys_charset = GetSysCharset (FALSE);

    SetRect (&welcome_rc,  10, 20, g_rcScr.right - 10, g_rcScr.bottom / 1.5 - 10);
    SetRect (&msg_rc, 10, welcome_rc.bottom + 10, g_rcScr.right - 10, g_rcScr.bottom - 20);

    if (strcmp (sys_charset, FONT_CHARSET_GB2312_0) == 0 
            || strcmp (sys_charset, FONT_CHARSET_GBK) == 0) {
        format = "昨天,市工商局局长张志宽在市工商局工作会上透露,本市工商系统今年将推出一系列促进经济发展、保护消费者权益的监管措施。其中,将会把B2B交易平台中的网店、专业门户网站和综合门户网站中的经营主体纳入监控,重点监控网上传销、无照经营、售假和网上欺诈等4种行为。张志宽说,2008年网上违法情况主要表现在虚假宣传、网上欺诈、销售假冒产品等。针对这些违法行为,今年本市将把B2B交易平台上的网店、专业门户网站和综合门户网站中的经营主体信息,全部纳入监控。其中,网上传销、无照经营、销售假冒伪劣商品和网上欺诈等4种行为,将成为打击的重点,工商将研究推出新的监管手段。据了解,目前国务院已经将网上交易监管交由国家工重点打击无照经营商总局负责,工商总局正在研究监管意见。本市工商局将根据工商总局的监管意见,相应地完善监管制度。\
            \nA proprietary protocol VOIP system built using Peer-to-peer (P2P) techniques.Free for non commercial use when using softphones (PC to PC).Offers toll access to PSTN via SkypeOut and SkypeIn.\nFrom the company that created KaZaA Licenses the Global IP Sound VoiceEngine suite of codecs and related software including their wideband codecs (double the width of regular pstn.) If you find yourself asking, 'why does Skype sound so good?', the answer is primarily this wideband ability.";
    }
    else if (strcmp (sys_charset, FONT_CHARSET_BIG5) == 0) {
        format = "欢迎来到 MiniGUI 的世界! 如果您能看到该文本, 则说明 MiniGUI Version %d.%d.%d 可在该硬件上运行!";
    }
    else {
        format = "Welcome to the world of MiniGUI. \nIf you can see this text, MiniGUI Version %d.%d.%d can run on this hardware board.";
    }

        format = "昨天,市工商局局长张志宽在市工商局工作会上透露,本市工商系统今年将推出一系列促进经济发展、保护消费者权益的监管措施。其中,将会把B2B交易平台中的网店、专业门户网站和综合门户网站中的经营主体纳入监控,重点监控网上传销、无照经营、售假和网上欺诈等4种行为。张志宽说,2008年网上违法情况主要表现在虚假宣传、网上欺诈、销售假冒产品等。针对这些违法行为,今年本市将把B2B交易平台上的网店、专业门户网站和综合门户网站中的经营主体信息,全部纳入监控。其中,网上传销、无照经营、销售假冒伪劣商品和网上欺诈等4种行为,将成为打击的重点,工商将研究推出新的监管手段。据了解,目前国务院已经将网上交易监管交由国家工重点打击无照经营商总局负责,工商总局正在研究监管意见。本市工商局将根据工商总局的监管意见,相应地完善监管制度。\
            \nA proprietary protocol VOIP system built using Peer-to-peer (P2P) techniques.Free for non commercial use when using softphones (PC to PC).Offers toll access to PSTN via SkypeOut and SkypeIn.\nFrom the company that created KaZaA Licenses the Global IP Sound VoiceEngine suite of codecs and related software including their wideband codecs (double the width of regular pstn.) If you find yourself asking, 'why does Skype sound so good?', the answer is primarily this wideband ability.";
    sprintf (welcome_text, format, MINIGUI_MAJOR_VERSION, MINIGUI_MINOR_VERSION, MINIGUI_MICRO_VERSION);

    strcpy (msg_text, "No message so far, \nThis is Only a test.");
}


#if 0
static void dump_logfont(PLOGFONT logfont)
{
    if(logfont->sbc_devfont)
    {
        printf("sbc_devfont: (0x%08x %s)\n", (int)logfont->sbc_devfont, logfont->sbc_devfont->name);
    }
    if(logfont->mbc_devfont)
    {
        printf("mbc_devfont: (0x%08x %s)\n", (int)logfont->mbc_devfont, logfont->mbc_devfont->name);
    }
    printf("font_size: %d\n", logfont->size);
    printf("font_type: %s\tfamily: %s\tcharset: %s\t style: 0x%08x\n", logfont->type,
            logfont->family, logfont->charset, (int)logfont->style);
}
#endif

//#define UTF_8 1

const char* utf8 = "\xe8\xbf\x99\xe6\x98\xaf\x55\x54\x46\x2d\x38\xe7\x9a\x84\xe6\xb5\x8b\xe8\xaf\x95\x0";
static int HelloWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    syskey = "";

    switch (message) {
        case MSG_CREATE:
            make_welcome_text ();
            SetTimer (hWnd, 100, 100);
            break;

        case MSG_TIMER:
            sprintf (msg_text, "Timer expired, current tick count: %ul.", 
                            GetTickCount ());
            //InvalidateRect (hWnd, &msg_rc, TRUE);
            InvalidateRect (hWnd, NULL, TRUE);
            break;
            
        case MSG_LBUTTONDOWN:
            strcpy (msg_text, "The left button pressed.");
            InvalidateRect (hWnd, &msg_rc, TRUE);
            break;

        case MSG_LBUTTONUP:
            strcpy (msg_text, "The left button released.");
            InvalidateRect (hWnd, &msg_rc, TRUE);
            break;

        case MSG_RBUTTONDOWN:
            strcpy (msg_text, "The right button pressed.");
            InvalidateRect (hWnd, &msg_rc, TRUE);
            break;

        case MSG_RBUTTONUP:
            strcpy (msg_text, "The right button released.");
            InvalidateRect (hWnd, &msg_rc, TRUE);
            break;

        case MSG_PAINT:
            {
                PLOGFONT logfont;
                PLOGFONT bklogfont;
#ifdef UTF_8
                logfont = CreateLogFont("sef", 
                        "fzxhgb_yh", 
                        "UTF-8", 
                        FONT_WEIGHT_BOOK,
                        FONT_SLANT_ROMAN, 
                        0, 
                        0,
                        FONT_UNDERLINE_NONE,
                        FONT_STRUCKOUT_NONE,
                        16, 0);
#else
                logfont = CreateLogFont("sef", 
                        "fzxhgb_yh", 
                        "GB2312",
                        FONT_WEIGHT_BOOK,
                        FONT_SLANT_ROMAN, 
                        0, 
                        0,
                        FONT_UNDERLINE_NONE,
                        FONT_STRUCKOUT_NONE,
                        16, 0);
#endif

                hdc = BeginPaint (hWnd);
                bklogfont = SelectFont(hdc, logfont);
                SetBkColor(hdc, COLOR_blue);
#ifdef UTF_8
                DrawText (hdc, utf8, -1, &welcome_rc, DT_LEFT | DT_WORDBREAK);
#else
                DrawText (hdc, welcome_text, -1, &welcome_rc, DT_LEFT | DT_WORDBREAK);
#endif
                DrawText (hdc, msg_text, -1, &msg_rc, DT_LEFT | DT_WORDBREAK);
                SelectFont(hdc, bklogfont);
                EndPaint (hWnd, hdc);
                DestroyLogFont(logfont);
                return 0;
         }
        case MSG_SYSKEYDOWN:
            syskey = "sys";
        case MSG_KEYDOWN:
            if(last_key == wParam)
                last_key_count++;
            else
            {
                last_key = wParam;
                last_key_count = 1;
            }
            sprintf (msg_text, "The %d %skey pressed %d times", 
                            wParam, syskey, last_key_count);
            InvalidateRect (hWnd, &msg_rc, TRUE);
            return 0;

        case MSG_KEYUP:
            sprintf (msg_text, "The %d key released", wParam);
            InvalidateRect (hWnd, &msg_rc, TRUE);
            return 0;

        case MSG_CLOSE:
            KillTimer (hWnd, 100);
            DestroyMainWindow (hWnd);
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

    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "Hello, world!";
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

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif


