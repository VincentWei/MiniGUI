/*
 * The following code initializes a MAINWINCREATE struct and then
 * creates a main window.
 */
{
    MAINWINCREATE CreateInfo;
    HWND hWnd;

    /* Initialize the MAINWINCREATE structure. */
    CreateInfo.dwStyle = WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_CAPTION;
    CreateInfo.spCaption= "MiniGUI step three";
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.hMenu = createmenu();
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = MainWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = 640;
    CreateInfo.by = 480;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    /* Create the main window. */
    hWnd = CreateMainWindow(&CreateInfo);
    if (hWnd == HWND_INVALID)
        return 0;
}
