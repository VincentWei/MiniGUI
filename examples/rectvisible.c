/*
 * Use RectVisible to optimize the repaint of the window.
 */
case MSG_PAINT:
{
	HDC hdc = BeginPaint (hWnd);

	for (j = 0; j < 10; j ++) {
		if (RectVisible  (hdc, rcs + j)) {
			FillBox (hdc, rcs[j].left, rcs[j].top, rcs [j].right, rcs [j].bottom);
		}
	}

	EndPaint (hWnd, hdc);
	return 0;
}
