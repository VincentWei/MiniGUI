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
** cliconn.c: Create a client endpoint and connect to a server.
**
** The idea comes from sample code in APUE.
** Thank Mr. Richard Stevens for his perfect work.
**
** Create date: 2000/12/20
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ourhdr.h"

#define CLI_PATH    "/var/tmp/"        /* +5 for pid = 14 chars */
#define CLI_PERM    S_IRWXU            /* rwx for user only */

/* returns fd if all OK, -1 on error */
int cli_conn (const char *name, char project)
{
    int                fd, len;
    struct sockaddr_un unix_addr;

    /* create a Unix domain stream socket */
    if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        return(-1);

    /* fill socket address structure w/our address */
    memset(&unix_addr, 0, sizeof(unix_addr));
    unix_addr.sun_family = AF_UNIX;
    sprintf(unix_addr.sun_path, "%s%05d-%c", CLI_PATH, getpid(), project);
    len = /* th sizeof(unix_addr.sun_len) + */ sizeof(unix_addr.sun_family) +
          strlen(unix_addr.sun_path) /* th + 1 */;
    /* th unix_addr.sun_len = len; */

    unlink (unix_addr.sun_path);        /* in case it already exists */
    if (bind(fd, (struct sockaddr *) &unix_addr, len) < 0)
        goto error;
    if (chmod(unix_addr.sun_path, CLI_PERM) < 0)
        goto error;

    /* fill socket address structure w/server's addr */
    memset(&unix_addr, 0, sizeof(unix_addr));
    unix_addr.sun_family = AF_UNIX;
    strcpy(unix_addr.sun_path, name);
    len = /* th sizeof(unix_addr.sun_len) + */ sizeof(unix_addr.sun_family) +
          strlen(unix_addr.sun_path) /* th + 1*/ ;
    /* th unix_addr.sun_len = len; */

    if (connect (fd, (struct sockaddr *) &unix_addr, len) < 0)
        goto error;

    return (fd);

error:
    close (fd);
    return(-1);
}

