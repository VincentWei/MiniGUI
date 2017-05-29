case MSG_PAINT:
{
    HDC hdc;

    hdc = BeginPaint (hWnd);

    // Draw a line.
    MoveTo (hdc, 0, 0);
    LineTo (hdc, 100, 100);

    EndPaint (hWnd, hdc);
    return 0;
}
