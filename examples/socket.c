#define LISTEN_SOCKET    "/var/tmp/mysocket"

static int listen_fd;

BOOL listen_socket (HWND hwnd)
{
    if ((listen_fd = serv_listen (LISTEN_SOCKET)) < 0)
        return FALSE;
    return RegisterListenFD (fd, POLL_IN, hwnd, NULL);
}

/* 
 * When the server receives the request to connect from a client,
 * the window hwnd will receive a MSG_FDEVENT message.
 * Now the server can accept the request.
 */
LRESULT MyWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        
        ...
        
        case MSG_FDEVENT:
             if (LOWORD (wParam) == listen_fd) { 
                  /* This message comes from the listen socket fd. */
                  pid_t pid;
                  uid_t uid;
                  int conn_fd;
                  conn_fd = serv_accept (listen_fd, &pid, &uid);
                  if (conn_fd >= 0) {
                       RegisterListenFD (conn_fd, POLL_IN, hwnd, NULL);
                  }
             }
             else { 
                  /* Client send a request. */
                  int fd = LOWORD(wParam);
                  /* Handle the request from client. */
                  sock_read_t (fd, ...);
                  sock_write_t (fd, ....);
             }
        break;
        
        ...
        
    }
}

/*
 * Clients can use the following code to connect itself to the server.
 */

    int conn_fd;
    
    if ((conn_fd  = cli_conn (LISTEN_SOCKET, 'b')) >= 0) {
        /* Send a request to the server. */
        sock_write_t (fd, ....);
        /* Get the reply from the server. */
        sock_read_t (fd, ....);
    }
