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
** scrollbar_test.c: The scrollbar control unit test program.
**
** Create date: 2008/1/26
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __NOUNIX__
#include <unistd.h>
#include <dirent.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "resource.h"

static int 
PageProc1 (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{

    switch (message) {
    case MSG_INITPAGE:
        break;
    case MSG_SHOWPAGE:
        return 1;

    /*case MSG_SHEETCMD:
        if (wParam == IDOK) {
            char buffer [20];
            GetDlgItemText (hDlg, IDC_EDIT1, buffer, 18);
            buffer [18] = '\0';

            if (buffer [0] == '\0') {
                MessageBox (hDlg, 
                            "Please input something in the first edit box.", 
                            "Warning!", 
                            MB_OK | MB_ICONEXCLAMATION | MB_BASEDONPARENT);
                return -1;
            }
        }
        return 0;*/

    case MSG_COMMAND:
        switch (wParam) {
        case IDOK:
        case IDCANCEL:
            MessageBox (hDlg, "Button pushed", "OK", 
                            MB_OK | MB_ICONINFORMATION | MB_BASEDONPARENT);
            break;
        }
        break;
    }
    
    return DefaultPageProc (hDlg, message, wParam, lParam);
}

static int 
PageProc2 (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    static HWND  hScrollbar=HWND_INVALID;
    static int   delta=2;

    switch (message) {
    case MSG_INITPAGE:
        {
            hScrollbar =CreateWindowEx(CTRL_SCROLLBAR,"scroll bar test", \
                        WS_VISIBLE|SBS_HORZ, \
                        WS_EX_NONE,IDC_SCROLLBAR,100, 30, 100, 20, hDlg, 0);
        }
        break;

    case MSG_SHOWPAGE:
        return 1;
    case MSG_TIMER:
        {
            if(100 == wParam)
            {
                MoveWindow(hScrollbar,100+delta,30,100,20,1);
            }
        }
        break;        
    case MSG_COMMAND:
        switch (wParam) {
        case IDOK:
            {
#ifdef _TIMER_UNIT_10MS
                SetTimer (hDlg, 100, 10);
#else
                SetTimer (hDlg, 100, 100);
#endif
            }
        case IDCANCEL:
            {
                KillTimer(hDlg,100);
                DestroyWindow(hScrollbar);
            }
            break;
        }
        break;
        
    }
    
    return DefaultPageProc (hDlg, message, wParam, lParam);
}

static int 
PageProc3 (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    //int i;
    //char temp [20];

    switch (message) {
    case MSG_INITPAGE:
        /*for (i = 0; i < 20; i++) { 
            sprintf (temp, "%d-Welcome", i);
            SendDlgItemMessage (hDlg, IDC_BOX1, CB_ADDSTRING, 0, (LPARAM)temp) ;
            SendDlgItemMessage (hDlg, IDC_BOX2, CB_ADDSTRING, 0, (LPARAM)temp) ;
            SendDlgItemMessage (hDlg, IDC_BOX4, CB_ADDSTRING, 0, (LPARAM)temp) ;
        }*/ 
        break;

    case MSG_SHOWPAGE:
        return 1;

    case MSG_GETDLGCODE:
        return DLGC_WANTARROWS;

    case MSG_COMMAND:
        switch (wParam) {
        case IDOK:
        case IDCANCEL:
            {
                HWND hScrollbar =HWND_INVALID;

                hScrollbar =CreateWindowEx(CTRL_SCROLLBAR,"scroll bar test", \
                        WS_VISIBLE|SBS_HORZ, \
                        WS_EX_NONE,IDC_SCROLLBAR,100, 30, 400, 200, hDlg, 0);
            }
            break;
        }
        break;
        
    }
    
    return DefaultPageProc (hDlg, message, wParam, lParam);
}


static int  style_hv[2]={SBS_HORZ,SBS_VERT},i=0;
static int  style_tb[3]={SBS_BOTTOMALIGN,SBS_TOPALIGN,0},j=0;
static int  style_lr[3]={SBS_LEFTALIGN,SBS_RIGHTALIGN,0},m=0;
static int  style_arrow[2]={SBS_NOARROW,0},n=0;
static int  style_shaft[2]={SBS_NOSHAFT,0},k=0;
static int  ydelta, pos,min,max;

static int  message_array[8]={SBM_GETPOS,SBM_SETPOS,SBM_GETSCROLLINFO,SBM_SETSCROLLINFO,SBM_GETRANGE, \
                             SBM_SETRANGE,SBM_SETRANGEREDRAW,SBM_ENABLE_ARROW},mIndex=0;

static int ScrollbarMessageTest(HWND hScrollbar, HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    HWND  hStyleedit,hMessageButton,hMessageTestRe;
    char  messagebuffer[100]={0};
    int   curpos;
    static int arrow_enable_flag =0;

    if(HWND_INVALID ==hScrollbar)
        return 0;

    if(mIndex >=8)
    {
        hMessageButton =GetDlgItem(hDlg,IDC_SCROLLBAR_MESSAGE_BUTTON);
        SetWindowText(hMessageButton,"消息及通知码测试完毕");
        return 0;
    }

    hStyleedit =GetDlgItem(hDlg,IDC_PROGRESS);
    hMessageTestRe =GetDlgItem(hDlg,IDC_PROGRESS2);
    switch(message_array[mIndex])
    {
        case SBM_GETPOS:
            {
                strcpy(messagebuffer,"send message:SBM_GETPOS");
                SetWindowText(hStyleedit,messagebuffer);
                curpos =SendMessage(hScrollbar,SBM_GETPOS,0,0);
                sprintf(messagebuffer,"message SBM_GETPOS :%d",curpos);
                SetWindowText(hMessageTestRe,messagebuffer);
                mIndex +=1;
            }
            break;
        case SBM_SETPOS:
            {
                strcpy(messagebuffer,"send message:SBM_SETPOS");
                SetWindowText(hStyleedit,messagebuffer);
                SendMessage(hScrollbar,SBM_SETPOS,0,1);
                strcpy(messagebuffer,"message SBM_GETPOS wParam=0,lParam=1");
                SetWindowText(hMessageTestRe,messagebuffer);
                mIndex +=1;
            }
            break;
        case SBM_GETSCROLLINFO:
            {
                SCROLLINFO   infoScrollbar;
                strcpy(messagebuffer,"send message:SBM_GETSCROLLINFO");
                SetWindowText(hStyleedit,messagebuffer);
                SendMessage(hScrollbar,SBM_GETSCROLLINFO,(WPARAM)&infoScrollbar,0);
                sprintf(messagebuffer,"message SBM_GETPOS :nmin=%d,nmax=%d,npage=%d,npos=%d",infoScrollbar.nMin,infoScrollbar.nMax,infoScrollbar.nPage,infoScrollbar.nPos);
                SetWindowText(hMessageTestRe,messagebuffer);
                mIndex +=1;
            }
            break;
        case SBM_SETSCROLLINFO:
            {
                SCROLLINFO   infoScrollbar;
                SendMessage(hScrollbar,SBM_GETSCROLLINFO,(WPARAM)&infoScrollbar,0);
                strcpy(messagebuffer,"send message:SBM_SETSCROLLINFO");
                SetWindowText(hStyleedit,messagebuffer);
                infoScrollbar.nMin=0;
                infoScrollbar.nMax +=1;
                SendMessage(hScrollbar,SBM_SETSCROLLINFO,(WPARAM)&infoScrollbar,1);
                sprintf(messagebuffer,"message SBM_SETSCROLLINFO :nMin=%d,nMax=%d,nPage=%d,nPos=%d",infoScrollbar.nMin,infoScrollbar.nMax,infoScrollbar.nPage,infoScrollbar.nPos);
                SetWindowText(hMessageTestRe,messagebuffer);
                mIndex +=1;
            }
            break;
        case SBM_GETRANGE:
            {
                int min,max;

                strcpy(messagebuffer,"send message:SBM_GETRANGE");
                SetWindowText(hStyleedit,messagebuffer);
                SendMessage(hScrollbar,SBM_GETRANGE,(WPARAM)&min,(LPARAM)&max);
                sprintf(messagebuffer,"message SBM_GETRANGE :min=%d,max=%d",min,max);
                SetWindowText(hMessageTestRe,messagebuffer);
                mIndex +=1;
            }
            break;
        case SBM_SETRANGE:
            {
                int min,max;

                SendMessage(hScrollbar,SBM_GETRANGE,(WPARAM)&min,(LPARAM)&max);
                strcpy(messagebuffer,"send message:SBM_SETRANGE");
                SetWindowText(hStyleedit,messagebuffer);
                min +=1;
                max +=1;
                SendMessage(hScrollbar,SBM_SETRANGE,min,max);
                sprintf(messagebuffer,"message SBM_SETRANGE :min=%d,max=%d",min,max);
                SetWindowText(hMessageTestRe,messagebuffer);
                mIndex +=1;
            }
            break;
        case SBM_SETRANGEREDRAW:
            {
                int min,max;

                SendMessage(hScrollbar,SBM_GETRANGE,(WPARAM)&min,(LPARAM)&max);
                strcpy(messagebuffer,"send message:SBM_SETRANGEREDRAW");
                SetWindowText(hStyleedit,messagebuffer);
                min +=1;
                max +=1;
                SendMessage(hScrollbar,SBM_SETRANGEREDRAW,min,max);
                sprintf(messagebuffer,"message SBM_SETRANGEREDRAW :min=%d,max=%d",min,max);
                SetWindowText(hMessageTestRe,messagebuffer);
                mIndex +=1;
            }
            break;
        case SBM_ENABLE_ARROW:
            {
                strcpy(messagebuffer,"send message:SBM_ENABLE_ARROW");
                SetWindowText(hStyleedit,messagebuffer);
                if(!arrow_enable_flag){
                    curpos =SendMessage(hScrollbar,SBM_ENABLE_ARROW,SB_ARROW_LTUP,0);
                    arrow_enable_flag =!arrow_enable_flag;
                    strcpy(messagebuffer,"message SBM_ENABLE_ARROW :SB_ARROW_LTUP");
                }
                else{
                    curpos =SendMessage(hScrollbar,SBM_ENABLE_ARROW,SB_ARROW_BTDN,0);
                    arrow_enable_flag =!arrow_enable_flag;
                    strcpy(messagebuffer,"message SBM_ENABLE_ARROW :SB_ARROW_BTDN");
                    mIndex +=1;
                }
                
                SetWindowText(hMessageTestRe,messagebuffer);
            }
    }

    return 0;
}

static int 
PageProc4 (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    static HWND  hScrollbar=HWND_INVALID;
    static char  stylebuffer[150]={0};

    switch (message) {
    case MSG_INITPAGE:
        break;
    case MSG_SHOWPAGE:
        return 1;
    case MSG_HSCROLL:
         {
             HWND hStyleTestRe;
            hStyleTestRe = GetDlgItem (hDlg, IDC_PROGRESS2);
            switch(wParam){
                case SB_LINEUP:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_HSCROLL : SB_LINEUP");
                    }
                    break;
                case SB_LINEDOWN:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_HSCROLL : SB_LINEDOWN");
                    }
                    break;
                case SB_PAGEUP:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_HSCROLL : SB_PAGEUP");
                    }
                    break;
                case SB_PAGEDOWN:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_HSCROLL : SB_PAGEDOWN");
                    }
                    break;
                case SB_THUMBPOSITION:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_HSCROLL : SB_THUMBPOSITION");
                    }
                    break;
                case SB_THUMBTRACK:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_HSCROLL : SB_THUMBTRACK");
                    }
                    break;
                case SB_ENDSCROLL:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_HSCROLL : SB_ENDSCROLL");
                    }
                    /*break;
                case SB_TOP:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_HSCROLL : SB_TOP");
                    }
                    break;
                case SB_BOTTOM:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_HSCROLL : SB_BOTTOM");
                    }*/
            }
        }
        break;
    case MSG_VSCROLL:
        {
            HWND  hStyleTestRe;
            switch(wParam){
                case SB_LINELEFT:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_VSCROLL : SB_LINELEFT");
                    }
                    break;
                case SB_LINERIGHT:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_VSCROLL : SB_LINERIGHT");
                    }
                    break;
                case SB_PAGELEFT:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_VSCROLL : SB_PAGELEFT");
                    }
                    break;
                case SB_PAGERIGHT:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_VSCROLL : SB_PAGERIGHT");
                    }
                    break;
                case SB_THUMBPOSITION:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_VSCROLL : SB_THUMBPOSITION");
                    }
                    break;
                case SB_THUMBTRACK:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_VSCROLL : SB_THUMBTRACK");
                    }
                    break;
                case SB_ENDSCROLL:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_VSCROLL : SB_ENDSCROLL");
                    }
                /*case SB_TOP:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_VSCROLL : SB_TOP");
                    }
                    break;
                case SB_BOTTOM:
                    {
                        SetWindowText(hStyleTestRe,"get notify MSG_VSCROLL : SB_BOTTOM");
                    }*/
            }
        }
    case MSG_COMMAND:
        switch (wParam) {
            case IDC_SCROLLBAR_MESSAGE_BUTTON:
            {
                ScrollbarMessageTest(hScrollbar,hDlg,message,wParam,lParam);
            }
            break;
            case IDOK:
            {
                 HWND  hStyleedit,hStyleTestRe,hStyleButton;
                 if(i>=2){
                hStyleButton =GetDlgItem(hDlg,IDOK);
                SetWindowText(hStyleButton,"风格组合测试结束");
                break;
            }
            switch(i){
                case 0:
                    strcpy(stylebuffer,"SBS_HORZ");
                    break;
                case 1:
                    strcpy(stylebuffer,"SBS_VERT");
            }
            switch(j){
                case 0:
                    strcat(stylebuffer,"|SBS_BOTTOMALIGN");
                    break;
                case 1:
                    strcat(stylebuffer,"|SBS_TOPALIGN");
            }
            switch(m){
                case 0:
                    strcat(stylebuffer,"|SBS_LEFTALIGN");
                    break;
                case 1:
                    strcat(stylebuffer,"|SBS_RIGHTALIGN");
            }
            if(!n)
                strcat(stylebuffer,"|SBS_NOARROW");
            if(!k)
                strcat(stylebuffer,"|SBS_NOSHAFT");
            hStyleedit = GetDlgItem (hDlg, IDC_PROGRESS);
            //printf("stylebuffer=%s\n",stylebuffer);
            if(HWND_INVALID ==hStyleedit)
                printf("the hStyleedit is INVALID\n");
            SetWindowText(hStyleedit,stylebuffer);
            //SendMessage(hStyleedit,EM_INSERTTEXT,strlen(stylebuffer),(LPARAM)stylebuffer);
            if(hScrollbar){
                DestroyWindow(hScrollbar);
            }
            /*hScrollbar =CreateWindowEx2(CTRL_BUTTON,"button",WS_VISIBLE|BS_PUSHBUTTON,WS_EX_NONE, \
                        IDC_SCROLLBAR,400,50,100,20,hDlg,"classic",0,0);*/
            /*hScrollbar =CreateWindowEx2(CTRL_SCROLLBAR,"scroll bar test", \
                        WS_VISIBLE, \
                        /*WS_VISIBLE|style_hv[i]|style_tb[j]|style_lr[m]|style_arrow[n]|style_shaft[k], \
                        WS_EX_NONE,IDC_SCROLLBAR,400, 50, 100, 20, hDlg, \
                        "classic", 0, 0);*/
              hScrollbar =CreateWindowEx(CTRL_SCROLLBAR,"scroll bar test", \
                        WS_VISIBLE|SBS_HORZ, \
                        /*WS_VISIBLE|style_hv[i]|style_tb[j]|style_lr[m]|style_arrow[n]|style_shaft[k], \*/
                        WS_EX_NONE,IDC_SCROLLBAR,400, 90, 100, 20, hDlg, 0);
              hStyleTestRe = GetDlgItem (hDlg, IDC_PROGRESS2);
              if(hScrollbar !=HWND_INVALID){
                  SetWindowText(hStyleTestRe,"Create Scrollbar ok\n");
                  mIndex =0;
                  hStyleButton =GetDlgItem(hDlg,IDC_SCROLLBAR_MESSAGE_BUTTON);
                  SetWindowText(hStyleButton,"消息及通知码测试");
              }
              else{
                  SetWindowText(hStyleTestRe,"Create Scrollbar error\n");
              }
            k +=1;
            if(k>=2){
                k=0;
                n +=1;
                if(n >=2){
                    n =0;
                    m +=1;
                    if(m >=3){
                        m =0;
                        j +=1;
                        if(j >=3){
                            j =0;
                            i +=1;
                        }
                    }
                }
            }
        }
        break;
        }
    }
    
    return DefaultPageProc (hDlg, message, wParam, lParam);
}


static int 
PageProc5 (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_INITPAGE:
        break;
    case MSG_SHOWPAGE:
        return 1;

    case MSG_COMMAND:
        switch (wParam) {
        case IDOK:
        case IDCANCEL:
            MessageBox (hDlg, "Button pushed", "OK", 
                            MB_OK | MB_ICONINFORMATION | MB_BASEDONPARENT);
            break;
        }
    }
    
    return DefaultPageProc (hDlg, message, wParam, lParam);
}


static int PropSheetProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_INITDIALOG:
        {
            HWND pshwnd = GetDlgItem (hDlg, IDC_PROPSHEET);

            DlgInitProgress.controls = CtrlInitProgress;
            SendMessage ( pshwnd, PSM_ADDPAGE, 
                            (WPARAM)&DlgInitProgress, (LPARAM) PageProc4);

            DlgStartupMode.controls = CtrlStartupMode;
            SendMessage ( pshwnd, PSM_ADDPAGE, 
                            (WPARAM)&DlgStartupMode,(LPARAM)PageProc3);
            
            DlgPassword.controls = CtrlPassword;
            SendMessage ( pshwnd, PSM_ADDPAGE, 
                            (WPARAM)&DlgPassword,(LPARAM) PageProc2);
            
            DlgStructParams.controls = CtrlStructParams;
            SendMessage (pshwnd, PSM_ADDPAGE,
                            (WPARAM)&DlgStructParams, (LPARAM)PageProc1);

            Dlgutmost.controls =Ctrlutmost;
            SendMessage ( pshwnd, PSM_ADDPAGE, 
                            (WPARAM)&Dlgutmost, (LPARAM) PageProc5);


            break;
        }

        case MSG_COMMAND:
        switch (wParam) 
        {
            /*case IDC_APPLY:
            break;

            case IDOK:
            {
                int index = SendDlgItemMessage (hDlg, IDC_PROPSHEET, 
                                PSM_SHEETCMD, IDOK, 0);
                if (index) {
                    SendDlgItemMessage (hDlg, IDC_PROPSHEET, 
                                    PSM_SETACTIVEINDEX, index - 1, 0);
                }
                else 
                    EndDialog (hDlg, wParam);

                break;
            }*/
            case IDCANCEL:
                EndDialog (hDlg, wParam);
            break;
        }
        break;
    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

static void ScrollbarTest (void)
{
    DlgPropertySheet.controls = CtrlPropertySheet;
    DialogBoxIndirectParam (&DlgPropertySheet, HWND_DESKTOP, PropSheetProc, 0L);
}



int MiniGUIMain (int args, const char* arg[])
{
    //MSG Msg;
    //MAINWINCREATE CreateInfo;
    //HWND hMainWnd;

#ifdef _MGRM_PROCESSES
    int i;
    const char* layer = NULL;

    for (i = 1; i < args; i++) {
        if (strcmp (arg[i], "-layer") == 0) {
            layer = arg[i + 1];
            break;
        }
    }

    GetLayerInfo (layer, NULL, NULL, NULL);

    if (JoinLayer (layer, arg[0], 0, 0) == INV_LAYER_HANDLE) {
        printf ("JoinLayer: invalid layer handle.\n");
        return 1;
    }

    
#endif

    ScrollbarTest();

    return 0;
}
