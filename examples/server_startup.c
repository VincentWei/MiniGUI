/*
 * This program which should be named "mginit" calls "ServerStartup" 
 * to start the server of MiniGUI-Processes.
 */
int MiniGUIMain (int args, const char* arg[])
{
    if (!ServerStartup (0, 0, 0)) {
        fprintf (stderr, "Can not the MiniGUI server: mginit.\n");
        return 1;
    }

    if (!InitMiniGUIExt ()) {
        fprintf (stderr, "Can not init mgext library.\n");
        return 1;
    }

    /* Enter message loop */
    while (GetMessage (&msg, HWND_DESKTOP)) {
        DispatchMessage (&msg);
    }

    MiniGUIExtCleanUp ();

    return 0;
}
