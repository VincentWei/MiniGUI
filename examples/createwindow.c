
/*
 * The following code creates some controls when handling the MSG_CREATE message.
 */
#define IDC_STATIC1     100
#define IDC_STATIC2     150
#define IDC_BUTTON1     110
#define IDC_BUTTON2     120
#define IDC_EDIT1       130
#define IDC_EDIT2       140

LRESULT ControlTestWinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
        static HWND hStaticWnd1, hStaticWnd2, hButton1, hButton2, hEdit1, hEdit2;
        switch (message) {
        case MSG_CREATE:
        {
            hStaticWnd1 = CreateWindow (CTRL_STATIC, 
                                        "This is a static control", 
                                        WS_CHILD | SS_NOTIFY | SS_SIMPLE | WS_VISIBLE | WS_BORDER,
                                        IDC_STATIC1, 
                                        10, 10, 180, 300, hWnd, 0);
            hButton1    = CreateWindow (CTRL_BUTTON,
                                        "Button1", 
                                        WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 
                                        IDC_BUTTON1, 
                                        20, 20, 80, 20, hStaticWnd1, 0);
            hButton2    = CreateWindow (CTRL_BUTTON,
                                        "Button2", 
                                        WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 
                                        IDC_BUTTON2, 
                                        20, 50, 80, 20, hStaticWnd1, 0);
            hEdit1      = CreateWindow (CTRL_EDIT,
                                        "Edit Box 1", 
                                        WS_CHILD | WS_VISIBLE | WS_BORDER, 
                                        IDC_EDIT1, 
                                        20, 80, 100, 24, hStaticWnd1, 0);
            hStaticWnd2 = CreateWindow (CTRL_STATIC, 
                                        "This is child static control", 
                                        WS_CHILD | SS_NOTIFY | SS_SIMPLE | WS_VISIBLE | WS_BORDER,
                                        IDC_STATIC1, 
                                        20, 110, 100, 50, hStaticWnd1, 0);
            hEdit2      = CreateWindow (CTRL_EDIT,
                                        "Edit Box 2", 
                                        WS_CHILD | WS_VISIBLE | WS_BORDER, 
                                        IDC_EDIT2, 
                                        0, 20, 100, 24, hStaticWnd2, 0);
           break;
          }

.......

    }

    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}
