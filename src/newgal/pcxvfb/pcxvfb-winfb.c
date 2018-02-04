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
/**
 ** pcxvfb-winfb.c: win32 virtual fb implementation and ipc operations
 **                 used by pcxvfb.
 */

#include "mgconfig.h"

#ifdef _MGGAL_PCXVFB

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "pcxvfb.h"

#ifdef WIN32
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "ws2_32.lib")
#endif


static HANDLE hMutex;
static HANDLE hScreen;
static LPVOID lpScreen;
static PROCESS_INFORMATION pi;

static int kill_wvfb(void);

int win_sleep(int usec)
{
	Sleep(usec);
	return 0;
}

/* decide if win fb is available */
int win_PCXVFbAvailable (void)
{
    hMutex = CreateMutex (NULL, FALSE, "WVFBScreenObject");
    if (hMutex == NULL)
        return 0;
	
    return 1;
}

void win_PCXVFbLock (void)
{
    WaitForSingleObject (hMutex, INFINITE);
}

void win_PCXVFbUnlock (void)
{
    ReleaseMutex (hMutex);
}


static char *_map_file = "WVFBScreenMap-";

static g_map_file[128];
HANDLE g_map_file_handle;

/* Shared map file intialize */
void* win_PCXVFbInit (char* execl_file, char* etc_param, const char *skin)
{
    int pid = 0, color_num = 0, pitch, datalen;
    int width = 640, height = 480, depth = 16;
    char mode[15];
    char caption[64] = "wvfb";
    char cmdline[512];
    STARTUPINFO si;
    WORD16 wVersionRequested;
    WSADATA wsaData;
    struct sockaddr_in ser_addr;
    struct sockaddr_in c_addr;
    int client_len;

    // Initial for the socket ......
    wVersionRequested = MAKEWORD16 (2, 2);
    if (0 != WSAStartup(wVersionRequested, &wsaData)) 
    {
        return NULL;
    }

    if(strlen(etc_param) != 0)
    {
        strncpy(caption, etc_param, strrchr(etc_param, ' ') - etc_param);
        strcpy(mode, strrchr(etc_param, ' ') + 1);
        width   = atoi(mode);
        height  = atoi(strchr(mode, 'x')+1);
        depth   = atoi (strrchr (mode, '-') + 1);
    }

    if ( depth == 1 )
        pitch = (width*depth+7)/8;
    else
        pitch = ((width*depth+31)/32)*4;

    if(depth <= 8)
        color_num = 1 << depth;

    ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));
    ZeroMemory(&si,sizeof(STARTUPINFO));   
    si.cb           = sizeof(STARTUPINFO);   
    si.wShowWindow  = SW_SHOW;
    si.dwFlags      = STARTF_USESHOWWINDOW;

    pid = GetCurrentProcessId();

    __mg_pcxvfb_server_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(pid); //port : use the pid
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind((SOCKET)__mg_pcxvfb_server_sockfd, 
            (struct sockaddr *)&ser_addr, sizeof(ser_addr));

    listen((SOCKET)__mg_pcxvfb_server_sockfd, 3);

    memset (cmdline, 0, sizeof (cmdline));
    /* sprintf(cmdline,"wvfb %d %s %dx%d-%dbpp", pid, caption, width, height, depth); */
    if (skin && skin[0]) {
        sprintf(cmdline,"%s %d %s %dx%d-%dbpp %s", execl_file, pid, caption, width, height, depth, skin);
    }else{
        sprintf(cmdline,"%s %d %s %dx%d-%dbpp", execl_file, pid, caption, width, height, depth);
    }

    /* start wvfb2.exe ...... */
    if (!CreateProcess(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("cannot CreateProcess %s (%d).\n", execl_file, GetLastError());
        return 0;
    }

    client_len = sizeof(c_addr);
    __mg_pcxvfb_client_sockfd = 
        accept ((SOCKET)__mg_pcxvfb_server_sockfd, 
                (struct sockaddr *)&c_addr, &client_len);

    datalen = pitch * height + color_num * sizeof(XVFBPalEntry) + sizeof(XVFBHeader);
    {
        char tmp[MAX_PATH];
        GetTempPath(sizeof(tmp)/sizeof(tmp[0]), tmp);
        sprintf (g_map_file, "%s/%s-%d", tmp, _map_file, pid);
    }

    g_map_file_handle = CreateFile(g_map_file,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE |FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

    if(g_map_file_handle == INVALID_HANDLE_VALUE){
        printf("cannot create file %s (%d).\n", g_map_file, GetLastError());
        return 0;
    }

    hScreen = CreateFileMapping(g_map_file_handle, NULL,PAGE_READWRITE,  0, datalen,  NULL);  

    lpScreen =  MapViewOfFile(hScreen, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (lpScreen == NULL) 
    { 
        fprintf(stderr, "WVFB NEWGAL Engine: Could not map view of file.");
        return NULL;
    }

    return (void*)lpScreen;
}

/* close map file */
void win_PCXVFbClose (void)
{
    TerminateProcess(pi.hProcess, 0);

    UnmapViewOfFile (lpScreen);
    CloseHandle (hScreen);
    CloseHandle (hMutex);
    CloseHandle(g_map_file_handle);
	DeleteFile(g_map_file);
	{
		DWORD err = GetLastError();
	}
    /*
    if (__mg_pcxvfb_server_sockfd >=0 ){
        closesocket((SOCKET)__mg_pcxvfb_server_sockfd);
        __mg_pcxvfb_server_sockfd = -1;
    }
    if (__mg_pcxvfb_client_sockfd >=0 ) {
        closesocket((SOCKET)__mg_pcxvfb_client_sockfd);
        __mg_pcxvfb_client_sockfd = -1;
    }
    */
	WSACleanup();
}

/* extern for pcxvfbial.c to close socket fd */
int win_PCXVFbCloseSocket (int sockfd)
{
	return closesocket((SOCKET)sockfd);
}
#endif /* _PCXFB_ENGINE*/
