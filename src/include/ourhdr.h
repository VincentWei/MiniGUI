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
** ourhdr.h: the head file from APUE.
**
** Create date: 2000/06/11
*/

#ifndef    __ourhdr_h
#define    __ourhdr_h

#ifndef WIN32
#include <sys/time.h>    /* required for some of our prototypes */
#include <sys/types.h>    /* required for some of our prototypes */
#endif
#include <stdio.h>        /* for convenience */
#include <stdlib.h>        /* for convenience */
#include <string.h>        /* for convenience */
#ifndef WIN32
#include <unistd.h>        /* for convenience */
#include <syslog.h>        /* for convenience */
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#endif

#ifdef WIN32
#include "mgsock.h"
#endif

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
    int val;                    /* value for SETVAL */
    struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
    unsigned short int *array;  /* array for GETALL, SETALL */
    struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#endif

#define MAXLINE             4096        /* max line length */

#ifdef _MGRM_PROCESSES
void err_dump(const char *, ...);    /* {App misc_source} */
void err_msg(const char *, ...);
void err_quit(const char *, ...);
void err_ret(const char *, ...);
void err_sys(const char *, ...);
#endif

#if 0
void log_msg(const char *, ...);        /* {App misc_source} */
void log_open(const char *, int, int);
void log_quit(const char *, ...);
void log_ret(const char *, ...);
void log_sys(const char *, ...);
#endif

typedef struct listen_fd {
    int type;
    int fd;
    void* hwnd;
    void* context;
} LISTEN_FD;

extern fd_set      mg_rfdset;
extern fd_set*     mg_wfdset;
extern fd_set*     mg_efdset;
extern int         mg_maxfd;
extern LISTEN_FD   mg_listen_fds [];

#endif    /* __ourhdr_h */

