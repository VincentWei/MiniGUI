/*
 * The following code loads a BITMAP object from a file and then
 * fills a box in a DC.
 *
 * You should note that the code assigns 'bmType' field of the BITMAP object directly.
 */
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
