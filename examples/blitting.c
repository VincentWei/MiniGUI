{
    int tox = 800, toy = 800;
    int count;
    BITMAP bitmap;
    unsigned int start_tick, end_tick;

    /* Load the bitmap from the file. */
    if (LoadBitmap (hdc, &bitmap, "res/icon.bmp"))
        return;

    bitmap.bmType = BMP_TYPE_ALPHACHANNEL;

    /* Fill a box with the bitmap with alpha channel. */
    start_tick = GetTickCount ();
    count = 1000;
    while (count--) {
        tox = rand() % 800;
        toy = rand() % 800;

        /* Set a random alpha channel. */
        bitmap.bmAlpha = rand() % 256;
        /* Fill the box. */
        FillBoxWithBitmap (hdc, tox, toy, 0, 0, &bitmap);
    }
    end_tick = GetTickCount ();
    TellSpeed (hwnd, start_tick, end_tick, "Alpha Blended Bitmap", 1000);

    bitmap.bmType = BMP_TYPE_ALPHACHANNEL | BMP_TYPE_COLORKEY;
    /* Set the color key (the transparent pixel) of the bitmap. */
    bitmap.bmColorKey = GetPixelInBitmap (&bitmap, 0, 0);

    /* Fill a box with the bitmap with alpha channel and color key. */
    start_tick = GetTickCount ();
    count = 1000;
    while (count--) {
        tox = rand() % 800;
        toy = rand() % 800;

        /* Set a random alpha channel. */
        bitmap.bmAlpha = rand() % 256;
        /* Fill the box. */
        FillBoxWithBitmap (hdc, tox, toy, 0, 0, &bitmap);
    }
    end_tick = GetTickCount ();
    TellSpeed (hwnd, start_tick, end_tick, "Alpha Blended Transparent Bitmap", 1000);

    UnloadBitmap (&bitmap);
}

{
    /* Create a memory DC which hanve alpha per-pixel. */
    mem_dc = CreateMemDC (400, 100, 16, MEMDC_FLAG_HWSURFACE | MEMDC_FLAG_SRCALPHA,
                    0x0000F000, 0x00000F00, 0x000000F0, 0x0000000F);

    /* Set brush color and fill a box. */
    SetBrushColor (mem_dc, RGBA2Pixel (mem_dc, 0xFF, 0xFF, 0x00, 0xFF));
    FillBox (mem_dc, 0, 0, 200, 50);

    /* Set another brush color and fill a box */
    SetBrushColor (mem_dc, RGBA2Pixel (mem_dc, 0xFF, 0xFF, 0x00, 0x40));
    FillBox (mem_dc, 200, 0, 200, 50);

    /* Set another brush color and fill a box */
    SetBrushColor (mem_dc, RGBA2Pixel (mem_dc, 0xFF, 0xFF, 0x00, 0x80));
    FillBox (mem_dc, 0, 50, 200, 50);

    /* Set another brush color and fill a box */
    SetBrushColor (mem_dc, RGBA2Pixel (mem_dc, 0xFF, 0xFF, 0x00, 0xC0));
    FillBox (mem_dc, 200, 50, 200, 50);
    SetBkMode (mem_dc, BM_TRANSPARENT);

    /* Set a text color and output the text */
    SetTextColor (mem_dc, RGBA2Pixel (mem_dc, 0x00, 0x00, 0x00, 0x80));
    TabbedTextOut (mem_dc, 0, 0, "Memory DC with alpha.\n"
                                 "The source DC have alpha per-pixel.");

    /* Blitting to a client DC. */
    start_tick = GetTickCount ();
    count = 100;
    while (count--) {
        BitBlt (mem_dc, 0, 0, 400, 100, hdc, rand () % 800, rand () % 800);
    }
    end_tick = GetTickCount ();
    TellSpeed (hwnd, start_tick, end_tick, "Alpha Blit", 100);

    /* delete the momory DC. */
    DeleteMemDC (mem_dc);
}
