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
** drawsemop.h: operations for drawing semaphore.
**
*/

#ifndef GUI_DRAWSEMOP_H
    #define GUI_DRAWSEMOP_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include <errno.h>

static inline void my_sem_op (int semid, int num, int op)
{
    struct sembuf sb;

again:
    sb.sem_num = num;
    sb.sem_op = op;
    sb.sem_flg = SEM_UNDO;

    if (semop (semid, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }
}

#define LOCK_DRAW_SEM()     my_sem_op(SHAREDRES_SEMID, _IDX_SEM_DRAW, -1)
#define UNLOCK_DRAW_SEM()   my_sem_op(SHAREDRES_SEMID, _IDX_SEM_DRAW, 1)

#define LOCK_SCREEN_SEM()   my_sem_op(SHAREDRES_SEMID, _IDX_SEM_SCR, -1)
#define UNLOCK_SCREEN_SEM() my_sem_op(SHAREDRES_SEMID, _IDX_SEM_SCR, 1)

#ifdef _MGSCHEMA_COMPOSITING
#   define LOCK_SURFACE_SEM(num)    my_sem_op(SHAREDRES_SEMID_SHARED_SURF, num, -1)
#   define UNLOCK_SURFACE_SEM(num)  my_sem_op(SHAREDRES_SEMID_SHARED_SURF, num, 1)
#endif

#ifdef _MGHAVE_CURSOR

#define LOCK_CURSOR_SEM()   my_sem_op(SHAREDRES_SEMID, _IDX_SEM_CURSOR, -1)
#define UNLOCK_CURSOR_SEM() my_sem_op(SHAREDRES_SEMID, _IDX_SEM_CURSOR, 1)

static inline int get_sem_pid (int semid, int num)
{
    int pid;
    union semun ignored;

    pid = semctl (semid, num, GETPID, ignored);
#ifdef _DEBUG
    if (pid == -1)
        perror ("get_sem_pid");
#endif

    return pid;
}

#define get_cursor_sem_pid() get_sem_pid(SHAREDRES_SEMID, _IDX_SEM_CURSOR)

static inline int get_hidecursor_sem_val (void)
{
    int val;
    union semun ignored;

    val = semctl (SHAREDRES_SEMID, _IDX_SEM_HIDECOUNT, GETVAL, ignored);
#ifdef _DEBUG
    if (val == -1)
        perror ("get_hidecursor_sem_val");
#endif

    return val;
}

static inline void reset_hidecursor_sem (void)
{
    int ret;
    union semun sunion;

    sunion.val = 0;
    ret = semctl (SHAREDRES_SEMID, _IDX_SEM_HIDECOUNT, SETVAL, sunion) == -1;
#ifdef _DEBUG
    if (ret == -1)
        perror ("reset_hidecursor_sem");
#else
    ret = ret + 0;
#endif
}

static inline void inc_hidecursor_sem (void)
{
    struct sembuf sb;

again:
    sb.sem_num = _IDX_SEM_HIDECOUNT;
    sb.sem_op = 1;
    sb.sem_flg = 0;

    if (semop (SHAREDRES_SEMID, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }
}

#endif /* _MGHAVE_CURSOR */

#ifdef _MGRM_PROCESSES
#   define LOCK_MOUSEMOVE_SEM()     my_sem_op(SHAREDRES_SEMID, _IDX_SEM_MOUSEMOVE, -1)
#   define UNLOCK_MOUSEMOVE_SEM()   my_sem_op(SHAREDRES_SEMID, _IDX_SEM_MOUSEMOVE, 1)
#endif /* _MGRM_PROCESSES */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_DRAWSEMOP_H

