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
#include "mgconfig.h"

#ifdef  WIN32

#include <windows.h>
#include "mgsock.h"

#pragma comment(lib, "ws2_32.lib")

win_fd_setsize = FD_SETSIZE;

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

int win_writesock(int s, char *buf, int len, int flags)
{
    return send( (SOCKET) s, buf, len, flags);
}

void win_closesock(int s)
{
    closesocket((SOCKET)s);
}

#endif /*WIN32*/
