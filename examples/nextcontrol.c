
/*
 * Sample code for GetNextChild:
 * Travles all child of a window \a hWnd.
 */

HWND child = HWND_DESKTOP;

do {
    child = GetNextChild (hWnd, child);
} while (child != HWND_DESKTOP);

