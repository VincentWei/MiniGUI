/*
 * The following code uses SetWindowCallbackProc function to
 * change the default window callback procedures of some controls.
 */
#define IDC_CTRL1     100
#define IDC_CTRL2     110
#define IDC_CTRL3     120
#define IDC_CTRL4     130

#define MY_ES_DIGIT_ONLY    0x0001
#define MY_ES_ALPHA_ONLY    0x0002
static WNDPROC old_edit_proc;
static LRESULT RestrictedEditBox (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == MSG_CHAR) {
        DWORD my_style = GetWindowAdditionalData (hwnd);

        /* Determine whether the character should be masked. */
        if ((my_style & MY_ES_DIGIT_ONLY) && (wParam < '0' || wParam > '9'))
            return 0;
        else if (my_style & MY_ES_ALPHA_ONLY)
            if (!((wParam >= 'A' && wParam <= 'Z') || (wParam >= 'a' && wParam <= 'z')))
                /* It is not the desired character, just return. */
                return 0;
    }

    /* Calling old procedure to handle the messages using default handler. */
    return (*old_edit_proc) (hwnd, message, wParam, lParam);
}

static LRESULT ControlTestWinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_CREATE:
    {
        HWND hWnd1, hWnd2, hWnd3;

        CreateWindow (CTRL_STATIC, "Digit-only box:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 0, 
                    10, 10, 180, 24, hWnd, 0);
        hWnd1 = CreateWindow (CTRL_EDIT, "", WS_CHILD | WS_VISIBLE | WS_BORDER, IDC_CTRL1, 
                    200, 10, 180, 24, hWnd, MY_ES_DIGIT_ONLY);
        CreateWindow (CTRL_STATIC, "Alpha-only box:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 0, 
                    10, 40, 180, 24, hWnd, 0);
        hWnd2 = CreateWindow (CTRL_EDIT, "", WS_CHILD | WS_BORDER | WS_VISIBLE, IDC_CTRL2, 
                    200, 40, 180, 24, hWnd, MY_ES_ALPHA_ONLY);
        CreateWindow (CTRL_STATIC, "Normal edit box:", WS_CHILD | WS_VISIBLE | SS_RIGHT, 0, 
                    10, 70, 180, 24, hWnd, 0);
        hWnd3 = CreateWindow (CTRL_EDIT, "", WS_CHILD | WS_BORDER | WS_VISIBLE, IDC_CTRL2, 
                    200, 70, 180, 24, hWnd, MY_ES_ALPHA_ONLY);

        CreateWindow ("button", "Close", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, IDC_CTRL4, 
                    100, 100, 60, 24, hWnd, 0);

        /* Set new window procedure and save the old procedure. */
        old_edit_proc = SetWindowCallbackProc (hWnd1, RestrictedEditBox);
        SetWindowCallbackProc (hWnd2, RestrictedEditBox);
        break;
    }

    ......

    }

    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}
