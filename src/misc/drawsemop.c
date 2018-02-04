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
** drawsemop.c: operations for drawing semaphore.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/12/31
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "common.h"
#include "minigui.h"
#include "ourhdr.h"
#include "sharedres.h"
#include "drawsemop.h"

#define SEM_TIMEOUT     500

void unlock_draw_sem (void)
{
    struct sembuf sb;

again:
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;

    if (semop (SHAREDRES_SEMID, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }
}

void lock_draw_sem (void)
{
    struct sembuf sb;

again:
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;

    if (semop (SHAREDRES_SEMID, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }
}

#ifdef _MGHAVE_CURSOR
void inc_hidecursor_sem (void)
{
    struct sembuf sb;

again:
    sb.sem_num = 2;
    sb.sem_op = 1;
    sb.sem_flg = 0;

    if (semop (SHAREDRES_SEMID, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }
}

#endif /* _MGHAVE_CURSOR */

