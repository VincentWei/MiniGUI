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
** sockio.c: routines for socket i/o.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/12/26
*/

#include <stdio.h>
#include <string.h>

#include "common.h"

#ifdef _MGRM_PROCESSES

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#include "minigui.h"
#include "ourhdr.h"
#include "sharedres.h"
#include "sockio.h"

ssize_t sock_write_t (int fd, const void* buff, size_t count, DWORD timeout)
{
    const void* pts = buff;
    ssize_t status = 0, n;
    DWORD start_tick = SHAREDRES_TIMER_COUNTER;

    if (count == 0)
        return SOCKERR_OK;

    while (status != count) {
        n = write (fd, pts + status, count - status);
        if (n < 0) {
            if (errno == EPIPE)
                return SOCKERR_CLOSED;
            else if (errno == EINTR) {
                if (timeout && (SHAREDRES_TIMER_COUNTER > start_tick + timeout)) {
                    return SOCKERR_TIMEOUT;
                }
                continue;
            }
            else
                return SOCKERR_IO;
        }
        status += n;
    }

    return status;
}

ssize_t sock_read_t (int fd, void* buff, size_t count, DWORD timeout)
{
    void* pts = buff;
    ssize_t status = 0, n;
    DWORD start_tick = SHAREDRES_TIMER_COUNTER;

    if (count <= 0) return SOCKERR_OK;
    while (status != count) {
        n = read (fd, pts + status, count - status);
        if (n < 0) {
            if (errno == EINTR) {
                if (timeout && (SHAREDRES_TIMER_COUNTER > start_tick + timeout))
                    return SOCKERR_TIMEOUT;
                continue;
            }
            else
                return SOCKERR_IO;
        }

        if (n == 0)
            return SOCKERR_CLOSED;

        status += n;
    }

    return status;
}

ssize_t sock_sendmsg_t (int fd, const struct msghdr *msg, int flags, DWORD timeout)
{
    ssize_t n;
    DWORD start_tick = SHAREDRES_TIMER_COUNTER;

    if (msg == NULL)
        return SOCKERR_OK;

    do {
        n = sendmsg (fd, msg, flags);

        if (n < 0) {
            if (errno == EPIPE)
                return SOCKERR_CLOSED;
            else if (errno == EINTR) {
                if (timeout && (SHAREDRES_TIMER_COUNTER > start_tick + timeout)) {
                    return SOCKERR_TIMEOUT;
                }
            }
            else
                return SOCKERR_IO;
        }
    } while (n < 0);

    return n;
}

ssize_t sock_recvmsg_t (int fd, struct msghdr *msg, int flags, DWORD timeout)
{
    ssize_t n;
    DWORD start_tick = SHAREDRES_TIMER_COUNTER;

    if (msg == NULL)
        return SOCKERR_OK;

    do {
        n = recvmsg (fd, msg, flags);

        if (n < 0) {
            if (errno == EPIPE)
                return SOCKERR_CLOSED;
            else if (errno == EINTR) {
                if (timeout && (SHAREDRES_TIMER_COUNTER > start_tick + timeout)) {
                    return SOCKERR_TIMEOUT;
                }
            }
            else
                return SOCKERR_IO;
        }
    } while (n < 0);

    return n;
}

#endif
