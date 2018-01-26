/*
 * The following code defines the dialog box callback procedure
 * and displays the dialog box by calling DialogBoxIndirectParam function.
 */
static LRESULT InitDialogBoxProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_INITDIALOG:
        return 1;
        
    case MSG_COMMAND:
        switch (wParam) {
        case IDOK:
        case IDCANCEL:
            EndDialog (hDlg, wParam);
            break;
        }
        break;
        
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}
static void InitDialogBox (HWND hWnd)
{
    /* Assoiciate the dialog with the controls */
    DlgInitProgress.controls = CtrlInitProgress;
    
    /* Display the dialog box and wait */
    DialogBoxIndirectParam (&DlgInitProgress, hWnd, InitDialogBoxProc, 0L);
}
