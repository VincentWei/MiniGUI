typedef struct TEST_REQ
{
   int a, b;
} TEST_REQ;

/* 
 * In the server, we define the request handler
 * and register it.
 */
static int ServerSendReply (int clifd, void* reply, int len)
{
    MSG reply_msg = {HWND_INVALID, 0};

    /* 
     * Sending a null message to client in order to indicate this is
     * a reply of a request.
     */
    if (sock_write (clifd, &reply_msg, sizeof (MSG)) < 0)
        return SOCKERR_IO;

    /* Send the result to the client. */
    if (sock_write (clifd, reply, len) < 0)
        return SOCKERR_IO;

    return SOCKERR_OK;
}

/*
 * This handler adds two integers and returns the sum
 * to the client.
 */
static int test_request (int cli, int clifd, void* buff, size_t len)
{
    int ret_value = 0;
    TEST_REQ* test_req = (TEST_REQ*)buff;

    ret_value = test_req.a + test_req.b;

    return ServerSendReply (clifd, &ret_value, sizeof (int));
}

...
     RegisterRequestHandler (MAX_SYS_REQID + 1, test_request);
...


/*
 * In the client, we can send a request to the server
 * to get the sum of two integers.
 */

        REQUEST req;
        TEST_REQ test_req = {5, 10};
        int ret_value;

        req.id = MAX_SYS_REQID + 1;
        req.data = &rest_req;
        req.len_data = sizeof (TEST_REQ);

        ClientRequest (&req, &ret_value, sizeof (int));
        /* ret_value shoudl be 15. */
        printf ("the returned value: %d\n", ret_value);
