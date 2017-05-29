/*
** $Id: sockio.h 7337 2007-08-16 03:44:29Z xgwang $
**
** sockio.h: routines for socket i/o.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Create date: 2000/12/xx
*/

#ifndef GUI_SOCKIO_H
    #define GUI_SOCKIO_H

#ifndef MINIGUI_H

#define SOCKERR_IO          -1
#define SOCKERR_CLOSED      -2
#define SOCKERR_INVARG      -3
#define SOCKERR_TIMEOUT     -4
#define SOCKERR_OK          0

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

int sock_write_t (int fd, const void* buff, int count, unsigned int timeout);
int sock_read_t (int fd, void* buff, int count, unsigned int timeout);

#define sock_write(fd, buff, count) sock_write_t(fd, buff, count, 0)
#define sock_read(fd, buff, count) sock_read_t(fd, buff, count, 0)

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // MINIGUI_H

#endif // GUI_SOCKIO_H

