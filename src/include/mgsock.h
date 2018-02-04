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
#ifndef _MGSOCK_H
#define _MGSOCK_H

#ifndef WIN32

#define mg_fd_set(fd, set)     FD_SET(fd, set)
#define mg_fd_clr(fd, set)     FD_CLR(fd, set)
#define mg_fd_isset(fd, set)   FD_ISSET(fd, set)
#define mg_fd_zero(set)        FD_ZERO(set)
#define mg_select(nfds, readfds, writefds, exceptfds, timeout) \
    select(nfds, readfds, writefds, exceptfds, timeout)
#define mg_readsocket(s, buf, len)     read(s, buf, len)
#define mg_writesocket(s, buf, len)    write(s, buf, len)

#else

#define mg_fd_set(fd, set)     win_fd_set(fd, set)
#define mg_fd_clr(fd, set)     win_fd_clr(fd, set)
#define mg_fd_isset(fd, set)   win_fd_isset(fd, set)
#define mg_fd_zero(set)        win_fd_zero(set)
#define mg_select(nfds, readfds, writefds, exceptfds, timeout) \
    win_select(nfds, readfds, writefds, exceptfds, timeout)
#define mg_readsocket(s, buf, len)     win_readsock(s, buf, len, 0)
#define mg_writesocket(s, buf, len)    win_writesock(s, buf, len, 0)

#endif /* WIN32 */

#ifdef WIN32
#   ifndef FD_SETSIZE
#       define FD_SETSIZE      64
typedef unsigned int u_int;
typedef int SOCKET;
typedef struct __fd_set {
    u_int   fd_count;  /* how many are SET? */
    int  fd_array[FD_SETSIZE];        /* an array of SOCKETs */
} _fd_set;
typedef struct __fd_set fd_set;
#   endif /* FD_SETSIZE */

int win_fd_setsize;
int win_fd_set (int fd, void *set);
int win_fd_clr (int fd, void *set);
int win_fd_isset (int fd, void *set);
int win_fd_zero (void *set);
int win_select (int nfds, void *readfds, void *writefds, void *exceptfds, const struct timeval *timeout);
int win_readsock (int s, char *buf, int len, int flags);
int win_writesock (int s, char *buf, int len, int flags);
void win_closesock (int s);
#endif

#endif /* _MGSOCK_H */
