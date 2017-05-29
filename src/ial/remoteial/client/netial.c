#include <stdio.h>
#include <string.h>
#include "netial_ctrlwin.h"

#define IPEB_X 50
#define IPEB_Y 70
#define IPEBWIDE 30
#define IPEBHEIGHT 20
#define IPEB_D 10

static WNDPROC old_edit_proc;
static HWND hIPAddrEB[4] = {0,0,0,0};
static HWND hPortEB= 0;

enum _idc_
{
    IDC_MAIN_ST = 1,
    IDC_START_BT,
    IDC_IPADDR_EB,
    IDC_PORT_EB = IDC_IPADDR_EB+4, 
    IDC_IPADDRDOT_ST,
    IDC_NOUSED = IDC_IPADDRDOT_ST+3,
};

static CTRLDATA IPAddrInputProgress []=
{
    {
        CTRL_STATIC,
        WS_VISIBLE,
        20,20,400,100,
        IDC_MAIN_ST,
        "This is netial, set your ip to ctrl.",
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP ,
        IPEB_X, IPEB_Y, IPEBWIDE, IPEBHEIGHT,
        IDC_IPADDR_EB,
        "192",
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP ,
        IPEB_X+IPEBWIDE + IPEB_D, IPEB_Y, IPEBWIDE, IPEBHEIGHT,
        IDC_IPADDR_EB+1,
        "168",
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP ,
        IPEB_X+2*IPEBWIDE+IPEB_D*2, IPEB_Y, IPEBWIDE, IPEBHEIGHT,
        IDC_IPADDR_EB+2,
        "1",
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP,
        IPEB_X+3*IPEBWIDE+IPEB_D*3, IPEB_Y, IPEBWIDE, IPEBHEIGHT,
        IDC_IPADDR_EB+3,
        "77",
        0
    },
    {
        CTRL_EDIT,
        WS_VISIBLE | WS_TABSTOP,
        IPEB_X+IPEBWIDE*4+IPEB_D*4+3, 70, 2*IPEBWIDE, IPEBHEIGHT,
        IDC_PORT_EB,
        "10086",
        0
    },
    {
        CTRL_BUTTON,
        WS_VISIBLE |WS_TABSTOP ,//| BS_PUSHBUTTON,
        IPEB_X, IPEB_Y+IPEBHEIGHT+20, IPEB_X+IPEBWIDE*4+IPEB_D*4+3, 20,
        IDC_START_BT,
        "START",
        0,
    },
};

static DLGTEMPLATE IPAddrInputPlate =
{
    WS_BORDER | WS_CAPTION,
    WS_EX_NONE,
    20, 100, 400, 200,
    "netial",
    0,0, 7,NULL,
    0,
};

static int PressStartButton(HWND hWnd)
{
    int i;
    char szEBbuf[5];
    char szURL[sizeof("xxx.xxx.xxx.xxx")];
    int nPort;
    int tmp;

    memset (szURL, 0, sizeof(szURL));
    for(i=0; i<4; i++)
    {
        memset (szEBbuf, 0, sizeof(szEBbuf));
        GetWindowText(hIPAddrEB[i], szEBbuf, sizeof(szEBbuf));
        tmp = atoi(szEBbuf);
        printf("tmp:%d\n", tmp);

        if ((tmp < 0) || (tmp > 256))
        {
            MessageBox(hWnd, "Ip address error.", "error!", MB_OK);
            return -1;
        }
        else
        {
            strcat(szURL, szEBbuf);
        }
        if (i<3)
        {
            strcat(szURL, ".");
        }

    }

    GetWindowText(hPortEB, szEBbuf, sizeof(szEBbuf));
    tmp = atoi(szEBbuf);
    if(tmp < 0 || tmp > 65376)
    {
        MessageBox(hWnd, "Port error.", "error!", MB_OK);
        return -1;
    }
    else
    {
        nPort = tmp;
    }
    printf("URL: %s port:%d\n", szURL, nPort);

    CreateCtrlWin(szURL, nPort);

    return 0;
}

static int NumberEditBox(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    if (message == MSG_CHAR)
    {
        if ((wParam < '0' || wParam > '9') && \
                wParam != '\t' && wParam != 127)
        {
            return 0;
        }
    }
    
    return (*old_edit_proc) (hDlg, message, wParam, lParam);
}

static int NetIalDialogBoxProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    int i;
    
    switch(message)
    {
        case MSG_INITDIALOG:
            for (i=0; i<4; i++)
            {
                hIPAddrEB[i] = GetDlgItem(hDlg, IDC_IPADDR_EB+i);
                SendDlgItemMessage (hDlg, IDC_IPADDR_EB+i, EM_LIMITTEXT, 3, 0);
                SetWindowCallbackProc (hIPAddrEB[i], NumberEditBox);
            }

            hPortEB = GetDlgItem (hDlg, IDC_PORT_EB);
            SendDlgItemMessage (hDlg, IDC_IPADDR_EB+i, EM_LIMITTEXT, 5, 0);
            old_edit_proc = SetWindowCallbackProc (hPortEB, NumberEditBox);

            return 1;
        case MSG_COMMAND:
            if (wParam == IDC_START_BT)
            {
                printf("Start remote control page.\n");
                PressStartButton(hDlg);
            }
            return 0;
        case MSG_CLOSE:
            EndDialog (hDlg, IDCANCEL);
            break;
    }
    return DefaultDialogProc(hDlg, message, wParam, lParam);
}

static void InitIPAddrInputDialog(HWND hWnd)
{
    IPAddrInputPlate.controls = IPAddrInputProgress;

    DialogBoxIndirectParam(&IPAddrInputPlate, hWnd, NetIalDialogBoxProc, 0L);
}

int MiniGUIMain(int argc, const char* argv[])
{

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER, "netial", 0, 0);
#endif

    InitIPAddrInputDialog(HWND_DESKTOP);

    return 0;
}



