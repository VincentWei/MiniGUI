/*
 * A typical handling of timer.
 */
LRESULT FlyingGUIWinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_CREATE:
        /* create a timer which expires every 100 ms, and whose id is 100. */
#ifdef _MGTIMER_UNIT_10MS
            SetTimer (hWnd, 100, 10);
#else
            SetTimer (hWnd, 100, 100);
#endif
        break;

        /* handling the MSG_TIMER message. */
        case MSG_TIMER:
            if (wParam == 100) /* if it is the timer whose id is 100. */
                InvalidateRect (hWnd, NULL, FALSE);
        break;

        case MSG_CLOSE:
            /* kill the timer whose id is 100. */
            KillTimer (hWnd, 100);
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
        return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

