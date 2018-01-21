
/*
 * This example trys to handle the event when the user
 * presses <C> key as the <Ctrl> key is down.
 */
LRESULT MyWinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_KEYDOWN:
            if (wParam ==SCANCODE_C && lParam & KS_CTRL) {
                // User pressed Ctrl+C.
                break;
            }
            break;
        ...
    }

    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

