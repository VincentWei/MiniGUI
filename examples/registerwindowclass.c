#define STEP_CTRL_NAME "mystep"

#define MSG_SET_STEP_INFO   (MSG_USER + 1)
#define MSG_SET_CURR_STEP   (MSG_USER + 2)

static LRESULT StepControlProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    HELPWININFO* info;
    
    switch (message) {
    case MSG_PAINT:
        hdc = BeginPaint (hwnd);


        /* Get the step information. */
        info = (HELPWININFO*)GetWindowAdditionalData (hwnd);

        /* Draw the step information. */
        ......

        EndPaint (hwnd, hdc);
        break;

    /* A message defined by the control, used to set the step information. */
    case MSG_SET_STEP_INFO:
        SetWindowAdditionalData (hwnd, (DWORD)lParam);
        InvalidateRect (hwnd, NULL, TRUE);
        break;

    /*
     * A message defined by the control, used to set the information
     * of the current step.
     */
    case MSG_SET_CURR_STEP:
        InvalidateRect (hwnd, NULL, FALSE);
        break;

    case MSG_DESTROY:
        break;
    }

    return DefaultControlProc (hwnd, message, wParam, lParam);
}


static BOOL RegisterStepControl ()
{
    int result;
    WNDCLASS StepClass;

    StepClass.spClassName = STEP_CTRL_NAME;
    StepClass.dwStyle     = 0;
    StepClass.hCursor     = GetSystemCursor (IDC_ARROW);
    StepClass.iBkColor    = COLOR_lightwhite;
    StepClass.WinProc     = StepControlProc;

    return RegisterWindowClass (&StepClass);
}

static void UnregisterStepControl ()
{
    UnregisterWindowClass (STEP_CTRL_NAME);
}
