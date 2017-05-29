#ifdef _USE_NEWGAL

static BLOCKHEAP my_cliprc_heap;
static BOOL ch_inited = FALSE;

static void GDIDemo_Region (HWND hWnd, HDC hdc)
{
    CLIPRGN my_cliprgn1;
    CLIPRGN my_cliprgn2;

    if (!ch_inited) {
        /* Init the heap used by our region. */
        InitFreeClipRectList (&my_cliprc_heap, 100);
        ch_inited = TRUE;
    }

    InitClipRgn (&my_cliprgn1, &my_cliprc_heap);
    InitClipRgn (&my_cliprgn2, &my_cliprc_heap);

    /* Create one circle region. */
    InitCircleRegion (&my_cliprgn1, 100, 100, 60);
    /* Create one ellipse region. */
    InitEllipseRegion (&my_cliprgn2, 100, 100, 50, 70);

    /* Fill a box to earase the background. */
    SetBrushColor (hdc, PIXEL_blue);
    FillBox (hdc, 0, 0, DEFAULT_WIDTH, 200);

    /* Get the difference between two regions. */
    SubtractRegion (&my_cliprgn1, &my_cliprgn1, &my_cliprgn2);
    /* Select the difference region as the current visible region of DC. */
    SelectClipRegion (hdc, &my_cliprgn1);

    /* Fill a box, but you will only see red color in the different region. */
    SetBrushColor (hdc, PIXEL_red);
    FillBox (hdc, 0, 0, 180, 200);

    /* Do more with XorRegion. */
    InitCircleRegion (&my_cliprgn1, 300, 100, 60);
    OffsetRegion (&my_cliprgn2, 200, 0);

    XorRegion (&my_cliprgn1, &my_cliprgn1, &my_cliprgn2);
    SelectClipRegion (hdc, &my_cliprgn1);

    FillBox (hdc, 200, 0, 180, 200);

    /* Do more with IntersectRegion. */
    InitCircleRegion (&my_cliprgn1, 500, 100, 60);
    OffsetRegion (&my_cliprgn2, 200, 0);

    IntersectRegion (&my_cliprgn1, &my_cliprgn1, &my_cliprgn2);
    SelectClipRegion (hdc, &my_cliprgn1);

    FillBox (hdc, 400, 0, 180, 200);

    /* 
     * Empty two clipping region to free the clipping rectangles used 
     * by them.
     */
    EmptyClipRgn (&my_cliprgn1);
    EmptyClipRgn (&my_cliprgn2);

    /* 
     * You should not forget to destroy the FreeClipRectList in your 
     * applications.
     * Here we do not destroy the heap because we will use it in next call.
     *
     * DestroyFreeClipRectList (&my_cliprc_heap);
     * ch_inited = FALSE;
     *
     */
}

#endif /* _USE_NEWGAL */
