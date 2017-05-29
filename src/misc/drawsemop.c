/*
** $Id: drawsemop.c 10660 2008-08-14 09:30:39Z weiym $
**
** drawsemop.c: operations for drawing semaphore.
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** All rights reserved by Feynman Software.
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

