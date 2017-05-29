/*
** $Id: drawsemop.h 12871 2010-05-07 06:13:42Z wanzheng $
**
** drawsemop.h: operations for drawing semaphore.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
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

