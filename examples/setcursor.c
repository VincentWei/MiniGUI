/*
 * The following MSG_SETCURSOR handler set the cursor
 * shape to the arrow cursor when the window is disabled.
 */
case MSG_SETCURSOR:
    if (GetWindowStyle (hwnd) & WS_DISABLED) {
        SetCursor (GetSystemCursor (IDC_ARROW));
        return 0;
    }
    break;
