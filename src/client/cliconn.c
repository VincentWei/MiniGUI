/*
** $Id: cliconn.c 10660 2008-08-14 09:30:39Z weiym $
** 
** cliconn.c: Create a client endpoint and connect to a server.
** 
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** All rights reserved by Feynman Software.
**
** Current maintainer: Wei Yongming.
**
** NOTE: The idea comes from sample code in APUE.
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

