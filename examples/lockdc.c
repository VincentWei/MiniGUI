    /*
     * lock the dc and draw the scan line with random color.
     */

    int i, width, height, pitch;
    RECT rc = {0, 0, 200, 200};
    int bpp = GetGDCapability (hdc, GDCAP_BPP);
    Uint8* frame_buffer = LockDC (hdc, &rc, &width, &height, &pitch);
    Uint8* row = frame_buffer;

    for (i = 0; i < *height; i++) {
        memset (row, rand ()%0x100, *width * bpp);
        row += *pitch;
    }

    UnlockDC (hdc);
