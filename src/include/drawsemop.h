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
** drawsemop.h: operations for drawing semaphore.
**
*/

#ifndef GUI_DRAWSEMOP_H
    #define GUI_DRAWSEMOP_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include <errno.h>

static inline void my_sem_op (int num, int op)
{
    struct sembuf sb;

again:
    sb.sem_num = num;
    sb.sem_op = op;
    sb.sem_flg = SEM_UNDO;

    if (semop (SHAREDRES_SEMID, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }
}

#define lock_draw_sem()     my_sem_op(_IDX_SEM_DRAW, -1)
#define unlock_draw_sem()   my_sem_op(_IDX_SEM_DRAW, 1)

#define lock_scr_sem()      my_sem_op(_IDX_SEM_SCR, -1)
#define unlock_scr_sem()    my_sem_op(_IDX_SEM_SCR, 1)

#ifdef _MGHAVE_CURSOR

#define lock_cursor_sem()   my_sem_op(_IDX_SEM_CURSOR, -1)
#define unlock_cursor_sem() my_sem_op(_IDX_SEM_CURSOR, 1)

static inline int get_sem_pid (int num)
{
    int pid;
    union semun ignored;

    pid = semctl (SHAREDRES_SEMID, num, GETPID, ignored);
#ifdef _DEBUG
    if (pid == -1)
        perror ("get_sem_pid");
#endif

    return pid;
}

#define get_cursor_sem_pid() get_sem_pid(_IDX_SEM_CURSOR)

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
#   define lock_mousemove_sem()     my_sem_op(_IDX_SEM_MOUSEMOVE, -1)
#   define unlock_mousemove_sem()   my_sem_op(_IDX_SEM_MOUSEMOVE, 1)
#endif /* _MGRM_PROCESSES */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_DRAWSEMOP_H

