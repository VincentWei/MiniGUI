#include "mgconfig.h"

#ifdef  WIN32

#include <windows.h>
#include "mgsock.h"

#pragma comment(lib, "ws2_32.lib")

win_fd_setsize = FD_SETSIZE;

int win_fd_set (int fd, void *set)
{
    FD_SET(fd, (fd_set *)set);
}

int win_fd_clr (int fd, void *set)
{
    FD_CLR(fd, (fd_set *)set);
}

int win_fd_isset (int fd, void *set)
{
    FD_ISSET(fd, (fd_set *)set);
}

int win_fd_zero (void *set)
{
    FD_ZERO((fd_set *)set);
}

int win_select(int nfds, void *readfds, void *writefds, void *exceptfds, const struct timeval *timeout)
{
    return select (nfds, (fd_set *)readfds, (fd_set *)writefds, (fd_set *)exceptfds, timeout);
}

int win_readsock(int s,  char *buf,   int len,  int flags)
{
    return recv( (SOCKET) s, buf, len, flags);
}

int win_writesock(int s, char *buf, int len, int flags)
{
    return send( (SOCKET) s, buf, len, flags);
}

void win_closesock(int s)
{
    closesocket((SOCKET)s);
}

#endif /*WIN32*/
