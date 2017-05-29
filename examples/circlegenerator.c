/*
 * Draw a circle which is 4 pixels wide by using CircleGenerator.
 */
static void draw_circle_pixel (void* context, int x1, int x2, int y)
{
    HDC hdc = (HDC) context;

    FillCircle (hdc, x1, y, 2);
    FillCircle (hdc, x2, y, 2);
}

void DrawMyCircle (HDC hdc, int x, int y, int r, gal_pixel pixel)
{
    gal_pixel old_brush;

    old_bursh = SetBrushColor (hdc, pixle);

    CircleGenerator ((void*)hdc, x, y, r, draw_circle_pixel);
    
    SetBrushColor (hdc, old_brush);
}
