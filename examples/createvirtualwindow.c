/*
 * The following code creates a virtual window.
 */
static LRESULT
my_virt_wnd_proc (HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message) {
        ...
    }

    return DefaultVirtualWinProc (hwnd, message, wparam, lparam);
}

...

    HWND new_wnd = CreateVirtualWindow (HWND_NULL, my_virt_wnd_proc,
            "A Virtual Window", 0, 0);

    if (new_wnd != HWND_INVALID) {
        // failed to create the virtual window.
    }

    // go on...
    ...
