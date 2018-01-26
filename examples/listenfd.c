
    ...

    /* Register to listen the read event of the master pty. */
    RegisterListenFD (pConInfo->masterPty, POLLIN, hMainWnd, 0);

    /* Enter the message loop. */
    while (!pConInfo->terminate && GetMessage (&Msg, hMainWnd)) {
        DispatchMessage (&Msg);
    }
    /* Unregister the listening fd. */
    UnregisterListenFD (pConInfo->masterPty);

    ...

/* The window procedure. */
static LRESULT VCOnGUIMainWinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PCONINFO pConInfo;

    pConInfo = (PCONINFO)GetWindowAdditionalData (hWnd);
    switch (message) {

         ...

        /* MSG_FDEVENT sent by MiniGUI indicates that you should to read the data. */
        case MSG_FDEVENT:
            ReadMasterPty (pConInfo);
        break;

        ...
    }

    /* Calling default window procedure. */
    if (pConInfo->DefWinProc)
        return (*pConInfo->DefWinProc)(hWnd, message, wParam, lParam);
    else
        return DefaultMainWinProc (hWnd, message, wParam, lParam);
}
