#ifndef _MGSOCK_H
#define _MSSOCK_H

#ifndef WIN32

#define mg_fd_set(fd, set)     FD_SET(fd, set)
#define mg_fd_clr(fd, set)     FD_CLR(fd, set)
#define mg_fd_isset(fd, set)   FD_ISSET(fd, set)
#define mg_fd_zero(set)        FD_ZERO(set)
#define mg_select(nfds, readfds, writefds, exceptfds, timeout) \
    select(nfds, readfds, writefds, exceptfds, timeout)
#define FD_READ(s, buf, len)     read(s, buf, len)
#define FD_WRITE(s, buf, len)    write(s, buf, len)

#else

#define mg_fd_set(fd, set)     win_fd_set(fd, set)
#define mg_fd_clr(fd, set)     win_fd_clr(fd, set)
#define mg_fd_isset(fd, set)   win_fd_isset(fd, set)
#define mg_fd_zero(set)        win_fd_zero(set)
#define mg_select(nfds, readfds, writefds, exceptfds, timeout) \
    win_select(nfds, readfds, writefds, exceptfds, timeout)
#define FD_READ(s, buf, len)     win_readsock(s, buf, len, 0)
#define FD_WRITE(s, buf, len)    win_writesock(s, buf, len, 0)

#endif /* WIN32 */

#ifndef FD_SETSIZE
#define FD_SETSIZE      64
#endif /* FD_SETSIZE */

#ifdef WIN32
typedef struct __fd_set {
    unsigned int   fd_count;  /* how many are SET? */
    int  fd_array[FD_SETSIZE];        /* an array of SOCKETs */
} _fd_set;
#endif

#ifdef WIN32
int win_fd_setsize;
int win_fd_set (int fd, void *set);
int win_fd_clr (int fd, void *set);
int win_fd_isset (int fd, void *set);
int win_fd_zero (void *set);
int win_select (int nfds, void *readfds, void *writefds, void *exceptfds, const struct timeval *timeout);
int win_readsock (int s, char *buf, int len, int flags);
int win_writesock (int s, char *buf, int len, int flags);
void win_closesock (int s);

#endif

#endif /* _MGSOCK_H */
