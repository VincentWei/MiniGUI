/*
** $Id: winial.c 12945 2010-05-21 06:19:40Z dongjunjie $
**
** winial.c: IAL engine for Windows Virtual FrameBuffer
** 
** Copyright (C) 2004 ~ 2007 Feynman Software
**
** Created by 
*/

#include "mgconfig.h"

#ifdef WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "winial.h"

#pragma comment(lib, "ws2_32.lib")

win_fd_setsize = FD_SETSIZE;

static HANDLE hEventPipe;
static int IsPipeClose=0;

static int pipeClean (void);
static void notify_wvfb (void);

void ErrorExit(LPTSTR lpszFunction) 
{
    TCHAR szBuf[80]; 
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );
 
    fprintf (stderr, "%s", szBuf);

    LocalFree(lpMsgBuf);
}

int win_IALInit (void)
{
    char pipe_str[50];

    notify_wvfb ();
	_sleep (500);

    sprintf(pipe_str, "\\\\.\\pipe\\wvfbEventPipe");

    hEventPipe = CreateFile (pipe_str, GENERIC_READ, 0, NULL, 
                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hEventPipe == INVALID_HANDLE_VALUE) {
        ErrorExit ("wvfb ial");
        printf ("waiting for wvfb ..........................\n");
        if (!WaitNamedPipe(pipe_str, 10000)) 
        {
           printf("Could not open wvfb pipe\n"); 
           return -1;
        }
    }

    _onexit (pipeClean);

    return 0;
}

static void notify_wvfb (void)
{
#if 1
    HWND hWnd = FindWindow(NULL,"wvfb Vritual Frame Buffer");
    if (hWnd) {
        COPYDATASTRUCT cpd;
        cpd.cbData = strlen("close");
        cpd.lpData = (void*)"close";
        SendMessage (hWnd, WM_COPYDATA, 0, (LPARAM)&cpd);
    }
#endif
}

void win_IALClose (void)
{
    CloseHandle (hEventPipe);
    notify_wvfb ();
    printf ("win_IALClose :windows wvfb ial is closed\n");
	IsPipeClose = 1;
}

static int pipeClean (void)
{
	if (!IsPipeClose) {
        CloseHandle (hEventPipe);
        notify_wvfb ();
	}
    return 0;
}

int win_GetEventData (void* data, size_t len)
{
    DWORD r_len = 0;

    if(ReadFile (hEventPipe, data, len, &r_len, NULL) == FALSE)
    {
        win_IALClose();
        printf ("exit....................\n");
        exit(0);
    }
    return r_len;
}

int win_fd_set (int fd, void *set)
{
	FD_SET(fd, (fd_set *)set);
}

int win_fd_clr (int fd, void *set)
{
	FD_CLR(fd, (fd_set *)set);
}

int win_fd_isset (int fd, void *set)
{
	FD_ISSET(fd, (fd_set *)set);
}

int win_fd_zero (void *set)
{
	FD_ZERO((fd_set *)set);
}

int win_select(int nfds, void *readfds, void *writefds, void *exceptfds, const struct timeval *timeout)
{
	return select (nfds, (fd_set *)readfds, (fd_set *)writefds, (fd_set *)exceptfds, timeout);
}

int win_readsock(int s,  char *buf,   int len,  int flags)
{
	return recv( (SOCKET) s, buf, len, flags);
}

int win_writesock(int s, const char* buff, int len, int flags){
	return send( (SOCKET)s, buff, len, flags);
}

void win_closesock(int s)
{
	closesocket((SOCKET)s);
}

#endif /* WIN32 */
