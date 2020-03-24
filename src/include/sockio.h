///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
 *   This file is part of MiniGUI, a mature cross-platform windowing
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 *
 *   Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** sockio.h: routines for socket i/o.
**
** Create date: 2000/12/xx
*/

#ifndef GUI_SOCKIO_H
    #define GUI_SOCKIO_H

#include <sys/types.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifndef MINIGUI_H

#define SOCKERR_IO          -1
#define SOCKERR_CLOSED      -2
#define SOCKERR_INVARG      -3
#define SOCKERR_TIMEOUT     -4
#define SOCKERR_OK          0

ssize_t sock_write_t (int fd, const void* buff, size_t count, DWORD timeout);
ssize_t sock_read_t (int fd, void* buff, size_t count, DWORD timeout);

#define sock_write(fd, buff, count) sock_write_t(fd, buff, count, 0)
#define sock_read(fd, buff, count) sock_read_t(fd, buff, count, 0)

#endif // MINIGUI_H

ssize_t sock_sendmsg_t (int fd, const struct msghdr *msg, int flags, DWORD timeout);
ssize_t sock_recvmsg_t (int fd, struct msghdr *msg, int flags, DWORD timeout);

#define sock_sendmsg(fd, msg, flags) sock_sendmsg_t(fd, msg, flags, 0)
#define sock_recvmsg(fd, msg, flags) sock_recvmsg_t(fd, msg, flags, 0)

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_SOCKIO_H

