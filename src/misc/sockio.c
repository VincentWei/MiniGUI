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

int sock_write_t (int fd, const void* buff, int count, unsigned int timeout)
{
    const void* pts = buff;
    int status = 0, n;
    unsigned int start_tick = SHAREDRES_TIMER_COUNTER;

    if (count < 0) return SOCKERR_OK;

    while (status != count) {
        n = write (fd, pts + status, count - status);
        if (n < 0) {
            if (errno == EPIPE)
                return SOCKERR_CLOSED;
            else if (errno == EINTR) {
                if (timeout && (SHAREDRES_TIMER_COUNTER 
                                        > start_tick + timeout)) {
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

int sock_read_t (int fd, void* buff, int count, unsigned int timeout)
{
    void* pts = buff;
    int status = 0, n;
    unsigned int start_tick = SHAREDRES_TIMER_COUNTER;

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

#endif 
