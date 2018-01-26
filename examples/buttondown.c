
/*
 * This example trys to handle the event when the user presses 
 * left button and right button of the mouse at the same time.
 */
LRESULT MyWinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_LBUTTONDOWN:
            if (wParam & KS_RIGHTBUTTON) {
                // Left button and right button both are pressed.
                break;
            }
            break;

        case MSG_RBUTTONDOWN:
            if (wParam & KS_LEFTBUTTON) {
                // Left button and right button both are pressed.
                break;
            }
            break;

        case MSG_MOUSEMOVE:
            if (wParam & KS_CTRL) {
                // User moves the mouse as the <Ctrl> key is down.
                break;
            }
            break;

        ...
    }

    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

