static LRESULT DepInfoBoxProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    struct _DepInfo *info;
   
    switch(message) {
    case MSG_INITDIALOG:
    {
        /* 
         * Get the lParam passed to this dialog box, and assign it
         * to the second private additional data assoiciated
         * with the dialog box.
         */
        info = (struct _DepInfo*)lParam;
        SetWindowAdditionalData2 (hDlg, (DWORD)lParam);
        break;
    }

    case MSG_COMMAND:
    {
        /* 
         * Get the parameter from the second private data assoiciated
         * with the dialog box.
         */
        info = (struct _DepInfo*) GetWindowAdditionalData2 (hDlg);

        switch(wParam) {
        case IDOK:
            /* Use the data in the parameter. */
            ......

        case IDCANCEL:
            EndDialog(hDlg,wParam);
            break;
        }
    }
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}
