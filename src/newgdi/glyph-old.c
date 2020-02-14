///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////

static BITMAP char_bmp;
static void* char_bits;
static size_t char_bits_size;

BOOL gdi_InitTextBitmapBuffer (void)
{
    char_bits = NULL;
    char_bmp.bmBits = NULL;
    return TRUE;
}

void TermTextBitmapBuffer (void)
{
    free (char_bits);
    char_bits = NULL;
    char_bmp.bmBits = NULL;

    char_bits_size = 0;
}

static void prepare_bitmap (PDC pdc, int w, int h)
{
    Uint32 size;
    size = GAL_GetBoxSize (pdc->surface, w + 2, h + 3, &char_bmp.bmPitch);
    char_bmp.bmType = 0;
    char_bmp.bmBitsPerPixel = pdc->surface->format->BitsPerPixel;
    char_bmp.bmBytesPerPixel = pdc->surface->format->BytesPerPixel;
    char_bmp.bmWidth = w;
    char_bmp.bmHeight = h;
    char_bmp.bmAlphaPixelFormat = NULL;

    /*char_bmp.bmBits may be setted to filtered_bits
     * recover it*/
    char_bmp.bmBits = char_bits;

    if (size <= char_bits_size) {
        return;
    }
    char_bits_size = ((size + 31) >> 5) << 5;

    char_bits = realloc(char_bmp.bmBits, char_bits_size);
    char_bmp.bmBits = char_bits;
}

static Uint32 allocated_size = 0;
static BYTE* filtered_bits;

static BYTE* do_low_pass_filtering (PDC pdc)
{
    int x, y;
    int bpp = GAL_BytesPerPixel (pdc->surface);
    BYTE *curr_sline, *prev_sline, *next_sline;
    BYTE *dst_line, *dst_pixel;

    if (allocated_size < char_bits_size) {

        filtered_bits = realloc(filtered_bits, char_bits_size);
        allocated_size = char_bits_size;
    }

    if (filtered_bits == NULL)
    {
        return NULL;
    }

    curr_sline = char_bmp.bmBits;
    next_sline = curr_sline + char_bmp.bmPitch;
    dst_line = filtered_bits;
    dst_pixel = dst_line;;

    /* For the first line, set the background and foreground color directly. */
    for (x = 0; x < char_bmp.bmWidth; x++) {
        int weight = 0;

        if (curr_sline [x]) {
            weight = 16;
        }
        else {
            if (x > 0) {
                if (curr_sline [x - 1]) weight += 3;
                if (next_sline [x - 1]) weight += 1;
            }
            if (x < char_bmp.bmWidth - 1) {
                if (curr_sline [x + 1]) weight += 3;
                if (next_sline [x + 1]) weight += 1;
            }
            if (next_sline [x]) weight += 3;
        }

        dst_pixel = _mem_set_pixel (dst_pixel,
                        bpp, pdc->filter_pixels [weight]);
    }

    prev_sline = curr_sline;
    curr_sline = curr_sline + char_bmp.bmPitch;
    next_sline = curr_sline + char_bmp.bmPitch;
    dst_line += char_bmp.bmPitch;

    if (char_bmp.bmHeight > 2)
    for (y = 1; y < char_bmp.bmHeight - 1; y++) {
        int weight;

        dst_pixel = dst_line;

        for (x = 0; x < char_bmp.bmWidth; x++) {
            weight = 0;

            /*
             * For the first pixel per line,
             * set the background and foreground color directly.
             */
            if (curr_sline [x]) {
                weight = 16;
            }
            else {
                if (x == 0) {
                    if (curr_sline [x + 1]) weight += 3;

                    if (prev_sline [x]) weight += 3;
                    if (prev_sline [x + 1]) weight += 1;

                    if (next_sline [x]) weight += 3;
                    if (next_sline [x + 1]) weight += 1;
                }
                else if (x == (char_bmp.bmWidth - 1)) {
                    if (curr_sline [x - 1]) weight += 3;

                    if (prev_sline [x - 1]) weight += 1;
                    if (prev_sline [x]) weight += 3;

                    if (next_sline [x - 1]) weight += 1;
                    if (next_sline [x]) weight += 3;
                }
                else {
                    if (curr_sline [x - 1]) weight += 3;
                    if (curr_sline [x + 1]) weight += 3;

                    if (prev_sline [x - 1]) weight += 1;
                    if (prev_sline [x]) weight += 3;
                    if (prev_sline [x + 1]) weight += 1;

                    if (next_sline [x - 1]) weight += 1;
                    if (next_sline [x]) weight += 3;
                    if (next_sline [x + 1]) weight += 1;
                }
            }

            /* set destination pixel according to the weight */
            dst_pixel = _mem_set_pixel (dst_pixel,
                            bpp, pdc->filter_pixels [weight]);
        }

        prev_sline += char_bmp.bmPitch;
        curr_sline += char_bmp.bmPitch;
        next_sline += char_bmp.bmPitch;
        dst_line += char_bmp.bmPitch;
    }

    dst_pixel = dst_line;
    /* For the last line, set the background and foreground color directly. */
    for (x = 0; x < char_bmp.bmWidth; x++) {
        int weight = 0;

        if (curr_sline [x]) {
            weight = 16;
        }
        else {
            if (x > 0) {
                if (prev_sline [x - 1]) weight += 1;
                if (curr_sline [x - 1]) weight += 3;
            }
            if (x < char_bmp.bmWidth - 1) {
                if (prev_sline [x + 1]) weight += 1;
                if (curr_sline [x + 1]) weight += 3;
            }
            if (prev_sline [x]) weight += 3;
        }

        dst_pixel = _mem_set_pixel (dst_pixel,
                        bpp, pdc->filter_pixels [weight]);
    }

    return filtered_bits;
}

static BYTE* do_inflate_filtering (PDC pdc)
{
    int x, y;
    int bpp = GAL_BytesPerPixel (pdc->surface);
    BYTE *curr_sline, *prev_sline, *next_sline;
    BYTE *dst_line, *dst_pixel;
    gal_pixel pixel;

    if (allocated_size < char_bits_size) {
        filtered_bits = realloc (filtered_bits, char_bits_size);
        allocated_size = char_bits_size;
    }

    if (filtered_bits == NULL)
    {
        return NULL;
    }

    curr_sline = char_bmp.bmBits;
    next_sline = curr_sline + char_bmp.bmPitch;
    dst_line = filtered_bits;
    dst_pixel = dst_line;;

    for (x = 0; x < char_bmp.bmWidth; x++) {
        if (curr_sline [x]) {
            pixel = pdc->filter_pixels [2];
        }
        else if ((x > 0 && curr_sline [x - 1]) ||
                        (x < char_bmp.bmWidth - 1 && curr_sline [x + 1]) ||
                        next_sline [x])
            pixel = pdc->filter_pixels [1];
        else
            pixel = pdc->filter_pixels [0];

        dst_pixel = _mem_set_pixel (dst_pixel, bpp, pixel);
    }

    prev_sline = curr_sline;
    curr_sline = curr_sline + char_bmp.bmPitch;
    next_sline = curr_sline + char_bmp.bmPitch;
    dst_line += char_bmp.bmPitch;

    for (y = 1; y < char_bmp.bmHeight - 1; y++) {

        dst_pixel = dst_line;
        for (x = 0; x < char_bmp.bmWidth; x++) {
            if (curr_sline [x]) {
                pixel = pdc->filter_pixels [2];
            }
            else if ((x > 0 && curr_sline [x - 1]) ||
                            (x < char_bmp.bmWidth - 1 && curr_sline [x + 1]) ||
                            prev_sline [x] || next_sline [x]) {
                pixel = pdc->filter_pixels [1];
            }
            else
                pixel = pdc->filter_pixels [0];

            dst_pixel = _mem_set_pixel (dst_pixel, bpp, pixel);
        }

        prev_sline += char_bmp.bmPitch;
        curr_sline += char_bmp.bmPitch;
        next_sline += char_bmp.bmPitch;
        dst_line += char_bmp.bmPitch;
    }

    dst_pixel = dst_line;
    for (x = 0; x < char_bmp.bmWidth; x++) {
        if (curr_sline [x]) {
            pixel = pdc->filter_pixels [2];
        }
        else if ((x > 0 && curr_sline [x - 1]) ||
                        (x < char_bmp.bmWidth - 1 && curr_sline [x + 1]) ||
                        prev_sline [x])
            pixel = pdc->filter_pixels [1];
        else
            pixel = pdc->filter_pixels [0];

        dst_pixel = _mem_set_pixel (dst_pixel, bpp, pixel);
    }

    return filtered_bits;
}

static void expand_char_pixmap (PDC pdc, int w, int h, const BYTE* bits,
            BYTE* expanded, int bold, int italic, int cols,
            unsigned short scale)
{
    int x, y, w_loop;
    int b = 0;
    BYTE* line;
    unsigned short s_loop;

    w_loop = w / scale;
    line = expanded;
    switch (GAL_BytesPerPixel (pdc->surface)) {
    case 1:
        for (y = 0; y < h; y += scale) {
            Uint8* dst = line;
            memset (dst , pdc->gray_pixels [0], w + bold + italic);

            if (italic)
                dst += (h - y) >> 1;

            for (x = 0; x < w_loop; x++) {

                b = *(bits+x);
                if (b == 255) b = 16;
                else b >>= 4;

                memset (dst, pdc->gray_pixels [b], scale);
                dst += scale;

                if (bold)
                    *dst = pdc->gray_pixels [b];
            }

            for (s_loop = 0; s_loop < (scale - 1); s_loop++) {
                memcpy (line + char_bmp.bmPitch, line, char_bmp.bmPitch);
                line += char_bmp.bmPitch;
            }
            line += char_bmp.bmPitch;

            bits += cols;
        }
    break;

    case 2:
        for (y = 0; y < h; y += scale) {
            Uint16* dst = (Uint16*) line;
            for (x = 0; x < (w + bold + italic); x ++) {
                dst [x] = pdc->gray_pixels [0];
            }

            if (italic)
                dst += ((h - y) >> 1);

            for (x = 0; x < w_loop; x++) {


                b = *(bits+x);

                if (b == 255) b = 16;
                else b >>= 4;

                for (s_loop = 0; s_loop < scale; s_loop ++) {
                    *dst = pdc->gray_pixels [b];
                    dst ++;
                }

                if (bold)
                    *dst = pdc->gray_pixels [b];
            }

            for (s_loop = 0; s_loop < (scale - 1); s_loop++) {
                memcpy (line + char_bmp.bmPitch, line, char_bmp.bmPitch);
                line += char_bmp.bmPitch;
            }
            line += char_bmp.bmPitch;

            bits += cols;
        }
    break;

    case 3:
        for (y = 0; y < h; y += scale) {
            Uint8* expanded = line;
            for (x = 0; x < (w + bold + italic) * 3; x += 3) {
#if 0
                *(Uint16 *) (expanded + x) = pdc->gray_pixels [0];
                *(expanded + x + 2) = pdc->gray_pixels [0] >> 16;
#else
                _mem_set_pixel (expanded + x, 3, pdc->gray_pixels [0]);
#endif
            }

            if (italic)
                expanded += ((h - y) >> 1) * 3;

            for (x = 0; x < w_loop; x++) {
                b = *(bits+x);
                if (b == 255) b = 16;
                else b >>= 4;

                for (s_loop = 0; s_loop < scale; s_loop ++) {
#if 0
                    *(Uint16 *) expanded = pdc->gray_pixels[b];
                    *(expanded + 2) = pdc->gray_pixels[b] >> 16;
                    expanded += 3;
#else
                    expanded = _mem_set_pixel (expanded, 3, pdc->gray_pixels [b]);
#endif
                }

                if (bold) {
#if 0
                    *(Uint16 *)expanded = pdc->gray_pixels[b];
                    *(expanded + 2) = pdc->gray_pixels[b] >> 16;
#else
                    _mem_set_pixel (expanded, 3, pdc->gray_pixels [b]);
#endif
                }

            }

            for (s_loop = 0; s_loop < (scale - 1); s_loop++) {
                memcpy (line + char_bmp.bmPitch, line, char_bmp.bmPitch);
                line += char_bmp.bmPitch;
            }
            line += char_bmp.bmPitch;

            bits += cols;
        }
    break;

    case 4:
        for (y = 0; y < h; y += scale) {
            Uint32* dst = (Uint32*)line;

            for (x = 0; x < (w + bold + italic); x ++) {
                dst [x] = pdc->gray_pixels [0];
            }

            if (italic)
                dst += ((h - y) >> 1);

            for (x = 0; x < w_loop; x++) {

                b = *(bits+x);
                if (b == 255) b = 16;
                else b >>= 4;

                for (s_loop = 0; s_loop < scale; s_loop ++) {
                    *dst = pdc->gray_pixels [b];
                    dst ++;
                }
                if (bold)
                    *dst = pdc->gray_pixels[b];
            }

            for (s_loop = 0; s_loop < (scale - 1); s_loop++) {
                memcpy (line + char_bmp.bmPitch, line, char_bmp.bmPitch);
                line += char_bmp.bmPitch;
            }
            line += char_bmp.bmPitch;

            bits += cols;
        }
    }
}

static void expand_char_bitmap (int w, int h,
            const BYTE* bits, int bits_pitch, int bpp, BYTE* expanded,
            int bg, int fg, int bold, int italic)
{
    int x, y;
    int b = 0;
    BYTE* line;
    const BYTE* line_bits = bits;

    line = expanded;
    switch (bpp) {
    case 1:
        for (y = 0; y < h; y++) {
            expanded = line;
            bits = line_bits;
            for (x = 0; x < (w + bold + italic); x++) {
                *(expanded + x) = bg;
            }

            if (italic)
                expanded += (h - y) >> 1;
            for (x = 0; x < w; x++) {
                if (x % 8 == 0)
                    b = *bits++;
                if ((b & (128 >> (x % 8)))) {
                    *expanded = fg;
                    if (bold)
                        *(expanded + 1) = fg;
                }

                expanded++;
            }
            line += char_bmp.bmPitch;
            line_bits += bits_pitch;
        }
    break;

    case 2:
        for (y = 0; y < h; y++) {
            expanded = line;
            bits = line_bits;
            for (x = 0; x < (w + bold + italic) << 1; x += 2) {
                *(Uint16 *) (expanded + x) = bg;
            }

            if (italic)
                expanded += ((h - y) >> 1) << 1;
            for (x = 0; x < w; x++) {
                if (x % 8 == 0)
                    b = *bits++;
                if ((b & (128 >> (x % 8)))) {
                    *(Uint16 *) expanded = fg;
                    if (bold)
                        *(Uint16 *)(expanded + 2) = fg;
                }

                expanded += 2;
            }
            line += char_bmp.bmPitch;
            line_bits += bits_pitch;
        }
    break;

    case 3:
        for (y = 0; y < h; y++) {
            expanded = line;
            bits = line_bits;
            for (x = 0; x < (w + bold + italic) * 3; x += 3) {
#if 0
                *(Uint16 *) (expanded + x) = bg;
                *(expanded + x + 2) = bg >> 16;
#else
                _mem_set_pixel (expanded + x, 3, bg);
#endif
            }

            if (italic)
                expanded += 3 * ((h - y) >> 1);

            for (x = 0; x < w; x++) {
                if (x % 8 == 0)
                    b = *bits++;
                if ((b & (128 >> (x % 8)))) {
#if 0
                    *(Uint16 *) expanded = fg;
                    *(expanded + 2) = fg >> 16;
#else
                    _mem_set_pixel (expanded, 3, fg);
#endif
                    if (bold) {
#if 0
                        *(Uint16 *)(expanded + 3) = fg;
                        *(expanded + 5) = fg >> 16;
#else
                        _mem_set_pixel (expanded + 3, 3, fg);
#endif
                    }
                }

                expanded += 3;
            }
            line += char_bmp.bmPitch;
            line_bits += bits_pitch;
        }
    break;

    case 4:
        for (y = 0; y < h; y++) {
            expanded = line;
            bits = line_bits;
            for (x = 0; x < (w + bold + italic) << 2; x += 4) {
                *(Uint32 *) (expanded + x)= bg;
            }

            if (italic)
                expanded += ((h - y) >> 1) << 2;

            for (x = 0; x < w; x++) {
                if (x % 8 == 0)
                    b = *bits++;
                if ((b & (128 >> (x % 8)))) {
                    *(Uint32 *) expanded = fg;
                    if (bold)
                        *(Uint32 *) (expanded + 4) = fg;
                }

                expanded += 4;
            }
            line += char_bmp.bmPitch;
            line_bits += bits_pitch;
        }
    }
}

static void expand_char_bitmap_scale (int w, int h,
            const BYTE* bits, int bits_pitch, int bpp, BYTE* expanded,
            int bg, int fg, int bold, int italic, unsigned short scale)
{
    int x, y, w_loop, b = 0;
    unsigned short s_loop;
    BYTE* line;
    const BYTE* line_bits = bits;

    w_loop = w / scale;
    line = expanded;
    switch (bpp) {
    case 1:
        for (y = 0; y < h; y+=scale) {
            expanded = line;
            bits = line_bits;

            memset (expanded, bg, w + bold + italic);

            if (italic)
                expanded += (h - y) >> 1;

            for (x = 0; x < w_loop; x++) {
                if (x % 8 == 0)
                    b = *bits++;

                if ((b & (128 >> (x % 8)))) {
                    memset (expanded, fg, scale);
                    expanded += scale;

                    if (bold)
                        *expanded = fg;
                }
                else
                    expanded += scale;
            }
            for (s_loop = 0; s_loop < (scale - 1); s_loop++) {
                memcpy (line + char_bmp.bmPitch, line, char_bmp.bmPitch);
                line += char_bmp.bmPitch;
            }
            line += char_bmp.bmPitch;
            line_bits += bits_pitch;
        }
    break;

    case 2:
        for (y = 0; y < h; y+=scale) {
            expanded = line;
            bits = line_bits;

            for (x = 0; x < (w + bold + italic) << 1; x += 2) {
                *(Uint16 *) (expanded + x) = bg;
            }

            if (italic)
                expanded += ((h - y) >> 1) << 1;

            for (x = 0; x < w_loop; x++) {
                if (x % 8 == 0)
                    b = *bits++;
                if ((b & (128 >> (x % 8)))) {
                    for (s_loop = 0; s_loop < scale; s_loop++) {
                        *(Uint16 *) expanded = fg;
                        expanded += 2;
                    }
                    if (bold)
                        *(Uint16 *)expanded = fg;
                }
                else
                    expanded += 2 * scale;
            }

            for (s_loop = 0; s_loop < (scale - 1); s_loop++) {
                memcpy (line + char_bmp.bmPitch, line, char_bmp.bmPitch);
                line += char_bmp.bmPitch;
            }
            line += char_bmp.bmPitch;
            line_bits += bits_pitch;
        }
    break;

    case 3:
        for (y = 0; y < h; y+=scale) {
            expanded = line;
            bits = line_bits;

            for (x = 0; x < (w + bold + italic) * 3; x += 3) {
#if 0
                *(Uint16 *) (expanded + x) = bg;
                *(expanded + x + 2) = bg >> 16;
#else
                _mem_set_pixel (expanded + x, 3, bg);
#endif
            }

            if (italic)
                expanded += 3 * ((h - y) >> 1);

            for (x = 0; x < w_loop; x++) {
                if (x % 8 == 0)
                    b = *bits++;
                if ((b & (128 >> (x % 8)))) {
                    for (s_loop = 0; s_loop < scale; s_loop++) {
#if 0
                        *(Uint16 *) expanded = fg;
                        *(expanded + 2) = fg >> 16;
                        expanded += 3;
#else
                        expanded = _mem_set_pixel (expanded, 3, fg);
#endif
                    }
                    if (bold) {
#if 0
                        *(Uint16 *)expanded = fg;
                        *(expanded + 2) = fg >> 16;
#else
                        _mem_set_pixel (expanded, 3, fg);
#endif
                    }
                }
                else
                    expanded += 3 * scale;
            }
            for (s_loop = 0; s_loop < (scale - 1); s_loop++) {
                memcpy (line + char_bmp.bmPitch, line, char_bmp.bmPitch);
                line += char_bmp.bmPitch;
            }
            line += char_bmp.bmPitch;
            line_bits += bits_pitch;
        }
    break;

    case 4:
        for (y = 0; y < h; y+=scale) {
            expanded = line;
            bits = line_bits;

            for (x = 0; x < (w + bold + italic) << 2; x += 4) {
                *(Uint32 *) (expanded + x)= bg;
            }

            if (italic)
                expanded += ((h - y) >> 1) << 2;

            for (x = 0; x < w_loop; x++) {

                if (x % 8 == 0)
                    b = *bits++;

                if ((b & (128 >> (x % 8)))) {
                    for (s_loop = 0; s_loop < scale; s_loop++) {
                        *(Uint32 *) expanded = fg;
                        expanded += 4;
                    }

                    if (bold)
                        *(Uint32 *) expanded = fg;
                }
                else {
                    expanded += 4 * scale;
                }
            }

            for (s_loop = 0; s_loop < (scale - 1); s_loop++) {
                memcpy (line + char_bmp.bmPitch, line, char_bmp.bmPitch);
                line += char_bmp.bmPitch;
            }
            line += char_bmp.bmPitch;
            line_bits += bits_pitch;
        }
    }
}


static void expand_char_bitmap_to_rgba_scale (int w, int h,
            const BYTE* bits, int bits_pitch, RGB* expanded,
            RGB fg, int bold, int italic, unsigned short scale)
{
    int x;
    int y;
    int font_w;
    int font_h;
    int font_x;
    int font_y;
    int italic_blank = 0;

    int b;

    RGB* line_start_rgba;
    BYTE* line_head_bits;

    font_w = (w-italic-bold) / scale;
    font_h = h / scale;

    /*expand font_h line*/
    for (font_y = 0; font_y < font_h; font_y++) {

        y = font_y * scale;
        line_head_bits = (BYTE*)bits + bits_pitch * font_y;


        /*expand a font line*/
        for ( ; y < (font_y+1)* scale; y++) {
            if (italic)
            {
                italic_blank = (h - y) >> 1;
            }

            line_start_rgba = expanded + y * w + italic_blank;

            /*expand a font point*/
            for (font_x = 0; font_x < font_w; font_x++) {

                b = line_head_bits[font_x/8];

                if ((b & (128 >> (font_x % 8)))) {
                    /*a font point => scale bmp point*/
                    for (x = font_x*scale; x < (font_x+1)*scale; x++) {
                        line_start_rgba[x] = fg;
                    }

                    if (bold)
                        line_start_rgba[x] = fg;
                }
                else
                {
                }
            }
        }
    }
}

#define ADJUST_SUBPEXIL_WIEGHT
#ifdef ADJUST_SUBPEXIL_WIEGHT
static void do_subpixel_line(RGB* dist_buf, RGB* pre_line,
        RGB* cur_line, RGB* next_line, int w, RGB rgba_fg)
{
    int x;
    int weight;
    Uint32 sub_val;

    if (cur_line[0].a == 0)
    {
        weight = 0;
        if (cur_line[1].a != 0)
            weight += NEIGHBOR_WGHT;
        if (pre_line[0].a != 0)
            weight += NEIGHBOR_WGHT;
        if (next_line[0].a != 0)
            weight += NEIGHBOR_WGHT;

        if (pre_line[1].a != 0)
            weight += NEAR_WGHT;
        if (next_line[1].a != 0)
            weight += NEAR_WGHT;

            sub_val = (cur_line[0].r * (ALL_WGHT-weight) + rgba_fg.r * weight)/ALL_WGHT;
            dist_buf[0].r = sub_val;

            sub_val = (cur_line[0].g * (ALL_WGHT-weight) + rgba_fg.g * weight)/ALL_WGHT;
            dist_buf[0].g = sub_val;

            sub_val = (cur_line[0].b * (ALL_WGHT-weight) + rgba_fg.b * weight)/ALL_WGHT;
            dist_buf[0].b = sub_val;

            dist_buf[0].a = 255;
    }
    else
    {
        sub_val = cur_line[0].r * (ALL_WGHT-ROUND_WGHT+ NEIGHBOR_WGHT + 2*NEAR_WGHT);
        sub_val += cur_line[1].r * NEIGHBOR_WGHT;

        sub_val += pre_line[0].r * NEIGHBOR_WGHT;
        sub_val += next_line[0].r * NEIGHBOR_WGHT;

        sub_val += pre_line[1].r * NEAR_WGHT;
        sub_val += next_line[1].r * NEAR_WGHT;

        dist_buf[0].r = sub_val/ALL_WGHT;


        sub_val = cur_line[0].g * (ALL_WGHT-ROUND_WGHT+ NEIGHBOR_WGHT + 2*NEAR_WGHT);
        sub_val += cur_line[1].g * NEIGHBOR_WGHT;
        sub_val += pre_line[0].g * NEIGHBOR_WGHT;
        sub_val += next_line[0].g * NEIGHBOR_WGHT;

        sub_val += pre_line[1].g * NEAR_WGHT;
        sub_val += next_line[1].g * NEAR_WGHT;

        dist_buf[0].g = sub_val/ALL_WGHT;

        sub_val = cur_line[0].b * (ALL_WGHT-ROUND_WGHT+ NEIGHBOR_WGHT + 2*NEAR_WGHT);
        sub_val += cur_line[1].b * NEIGHBOR_WGHT;
        sub_val += pre_line[0].b * NEIGHBOR_WGHT;
        sub_val += next_line[0].b * NEIGHBOR_WGHT;

        sub_val += pre_line[1].b * NEAR_WGHT;
        sub_val += next_line[1].b * NEAR_WGHT;

        dist_buf[0].b = sub_val/ALL_WGHT;

        dist_buf[0].a = 255;
    }

    for (x=1; x<w-1; x++)
    {
        weight = 0;
        if (cur_line[x].a == 0)
        {
            if (cur_line[x-1].a != 0)
                weight += NEIGHBOR_WGHT;
            if (cur_line[x+1].a != 0)
                weight += NEIGHBOR_WGHT;
            if (pre_line[ x ].a != 0)
                weight += NEIGHBOR_WGHT;
            if (next_line[ x ].a != 0)
                weight += NEIGHBOR_WGHT;

            if (pre_line[x-1].a != 0)
                weight += NEAR_WGHT;
            if (pre_line[x+1].a != 0)
                weight += NEAR_WGHT;
            if (next_line[x-1].a != 0)
                weight += NEAR_WGHT;
            if (next_line[x+1].a != 0)
                weight += NEAR_WGHT;

                sub_val = (cur_line[x].r * (ALL_WGHT-weight) + rgba_fg.r * weight)/ALL_WGHT;
                dist_buf[x].r = sub_val;

                sub_val = (cur_line[x].g * (ALL_WGHT-weight) + rgba_fg.g * weight)/ALL_WGHT;
                dist_buf[x].g = sub_val;

                sub_val = (cur_line[x].b * (ALL_WGHT-weight) + rgba_fg.b * weight)/ALL_WGHT;
                dist_buf[x].b = sub_val;

                dist_buf[x].a = 255;
        }
        else
        {
            sub_val = cur_line[ x ].r * (ALL_WGHT-ROUND_WGHT);

            sub_val += cur_line[x-1].r * NEIGHBOR_WGHT;
            sub_val += cur_line[x+1].r * NEIGHBOR_WGHT;
            sub_val += pre_line[ x ].r * NEIGHBOR_WGHT;
            sub_val += next_line[ x ].r * NEIGHBOR_WGHT;


            sub_val += pre_line[x-1].r * NEAR_WGHT;
            sub_val += next_line[x-1].r * NEAR_WGHT;
            sub_val += pre_line[x+1].r * NEAR_WGHT;
            sub_val += next_line[x+1].r * NEAR_WGHT;

            dist_buf[x].r = sub_val/ALL_WGHT;


            sub_val = cur_line[ x ].g * (ALL_WGHT-ROUND_WGHT);

            sub_val += cur_line[x-1].g * NEIGHBOR_WGHT;
            sub_val += cur_line[x+1].g * NEIGHBOR_WGHT;
            sub_val += pre_line[ x ].g * NEIGHBOR_WGHT;
            sub_val += next_line[ x ].g * NEIGHBOR_WGHT;


            sub_val += pre_line[x-1].g * NEAR_WGHT;
            sub_val += next_line[x-1].g * NEAR_WGHT;
            sub_val += pre_line[x+1].g * NEAR_WGHT;
            sub_val += next_line[x+1].g * NEAR_WGHT;

            dist_buf[x].g = sub_val/ALL_WGHT;


            sub_val = cur_line[ x ].b * (ALL_WGHT-ROUND_WGHT);

            sub_val += cur_line[x-1].b * NEIGHBOR_WGHT;
            sub_val += cur_line[x+1].b * NEIGHBOR_WGHT;
            sub_val += pre_line[ x ].b * NEIGHBOR_WGHT;
            sub_val += next_line[ x ].b * NEIGHBOR_WGHT;


            sub_val += pre_line[x-1].b * NEAR_WGHT;
            sub_val += next_line[x-1].b * NEAR_WGHT;
            sub_val += pre_line[x+1].b * NEAR_WGHT;
            sub_val += next_line[x+1].b * NEAR_WGHT;

            dist_buf[x].b = sub_val/ALL_WGHT;

            dist_buf[x].a = 255;
        }
    }

    weight = 0;
    if (cur_line[x].a == 0)
    {
        if (cur_line[x-1].a != 0)
            weight += NEIGHBOR_WGHT;
        if (pre_line[ x ].a != 0)
            weight += NEIGHBOR_WGHT;
        if (next_line[ x ].a != 0)
            weight += NEIGHBOR_WGHT;

        if (pre_line[x-1].a != 0)
            weight += NEAR_WGHT;
        if (next_line[x-1].a != 0)
            weight += NEAR_WGHT;

            sub_val = (cur_line[x].r * (ALL_WGHT-weight) + rgba_fg.r * weight)/ALL_WGHT;
            dist_buf[x].r = sub_val;

            sub_val = (cur_line[x].g * (ALL_WGHT-weight) + rgba_fg.g * weight)/ALL_WGHT;
            dist_buf[x].g = sub_val;

            sub_val = (cur_line[x].b * (ALL_WGHT-weight) + rgba_fg.b * weight)/ALL_WGHT;
            dist_buf[x].b = sub_val;

            dist_buf[x].a = 255;

    }
    else
    {
        sub_val = cur_line[x].r * (ALL_WGHT-ROUND_WGHT+ NEIGHBOR_WGHT + 2*NEAR_WGHT);
        sub_val += cur_line[x-1].r * NEIGHBOR_WGHT;
        sub_val += pre_line[x].r * NEIGHBOR_WGHT;
        sub_val += next_line[x].r * NEIGHBOR_WGHT;

        sub_val += pre_line[x-1].r * NEAR_WGHT;
        sub_val += next_line[x-1].r * NEAR_WGHT;

        dist_buf[x].r = sub_val/ALL_WGHT;

        sub_val = cur_line[x].g * (ALL_WGHT-ROUND_WGHT+ NEIGHBOR_WGHT + 2*NEAR_WGHT);
        sub_val += cur_line[x-1].g * NEIGHBOR_WGHT;
        sub_val += pre_line[x].g * NEIGHBOR_WGHT;
        sub_val += next_line[x].g * NEIGHBOR_WGHT;

        sub_val += pre_line[x-1].g * NEAR_WGHT;
        sub_val += next_line[x-1].g * NEAR_WGHT;

        dist_buf[x].g = sub_val/ALL_WGHT;

        sub_val = cur_line[x].b * (ALL_WGHT-ROUND_WGHT+ NEIGHBOR_WGHT + 2*NEAR_WGHT);
        sub_val += cur_line[x-1].b * NEIGHBOR_WGHT;
        sub_val += pre_line[x].b * NEIGHBOR_WGHT;
        sub_val += next_line[x].b * NEIGHBOR_WGHT;

        sub_val += pre_line[x-1].b * NEAR_WGHT;
        sub_val += next_line[x-1].b * NEAR_WGHT;

        dist_buf[x].b = sub_val/ALL_WGHT;

        dist_buf[x].a = 255;
    }
}

#else /* ADJUST_SUBPEXIL_WIEGHT */

static void do_subpixel_line(RGB* dist_buf, RGB* pre_line,
        RGB* cur_line, RGB* next_line, int w, RGB rgba_fg)
{
    int x;
    int weight;
    Uint32 sub_val;

    if (cur_line[0].a == 0)
    {
        weight = 0;
        if (cur_line[1].a != 0)
            weight += 3;
        if (pre_line[0].a != 0)
            weight += 3;
        if (next_line[0].a != 0)
            weight += 3;

        if (pre_line[1].a != 0)
            weight++;
        if (next_line[1].a != 0)
            weight++;

            sub_val = (cur_line[0].r * (32-weight) + rgba_fg.r * weight) >> 5;
            dist_buf[0].r = sub_val;

            sub_val = (cur_line[0].g * (32-weight) + rgba_fg.g * weight) >> 5;
            dist_buf[0].g = sub_val;

            sub_val = (cur_line[0].b * (32-weight) + rgba_fg.b * weight) >> 5;
            dist_buf[0].b = sub_val;

            dist_buf[0].a = 255;

    }
    else
    {
        sub_val = cur_line[0].r * 21; /* (32-16+ 3 + 2*1); */
        sub_val += cur_line[1].r * 3;

        sub_val += pre_line[0].r * 3;
        sub_val += next_line[0].r * 3;

        sub_val += pre_line[1].r;
        sub_val += next_line[1].r;

        dist_buf[0].r = sub_val >> 5;


        sub_val = cur_line[0].g * 21;/* (32-16+ 3 + 2*1); */
        sub_val += cur_line[1].g * 3;
        sub_val += pre_line[0].g * 3;
        sub_val += next_line[0].g * 3;

        sub_val += pre_line[1].g;
        sub_val += next_line[1].g;

        dist_buf[0].g = sub_val >> 5;

        sub_val = cur_line[0].b * 21; /* (32-16+ 3 + 2*1); */
        sub_val += cur_line[1].b * 3;
        sub_val += pre_line[0].b * 3;
        sub_val += next_line[0].b * 3;

        sub_val += pre_line[1].b;
        sub_val += next_line[1].b;

        dist_buf[0].b = sub_val >> 5;

        dist_buf[0].a = 255;
    }

    for (x=1; x<w-1; x++)
    {
        weight = 0;
        if (cur_line[x].a == 0)
        {
            if (cur_line[x-1].a != 0)
                weight += 3;
            if (cur_line[x+1].a != 0)
                weight += 3;
            if (pre_line[ x ].a != 0)
                weight += 3;
            if (next_line[ x ].a != 0)
                weight += 3;

            if (pre_line[x-1].a != 0)
                weight ++;
            if (pre_line[x+1].a != 0)
                weight ++;
            if (next_line[x-1].a != 0)
                weight ++;
            if (next_line[x+1].a != 0)
                weight ++;

                sub_val = (cur_line[x].r * (32-weight) + rgba_fg.r * weight) >> 5;
                dist_buf[x].r = sub_val;

                sub_val = (cur_line[x].g * (32-weight) + rgba_fg.g * weight) >> 5;
                dist_buf[x].g = sub_val;

                sub_val = (cur_line[x].b * (32-weight) + rgba_fg.b * weight) >> 5;
                dist_buf[x].b = sub_val;

                dist_buf[x].a = 255;

        }
        else
        {
            sub_val = cur_line[ x ].r << 4;

            sub_val += cur_line[x-1].r * 3;
            sub_val += cur_line[x+1].r * 3;
            sub_val += pre_line[ x ].r * 3;
            sub_val += next_line[ x ].r * 3;


            sub_val += pre_line[x-1].r;
            sub_val += next_line[x-1].r;
            sub_val += pre_line[x+1].r;
            sub_val += next_line[x+1].r;

            dist_buf[x].r = sub_val >> 5;


            sub_val = cur_line[ x ].g << 4 ;

            sub_val += cur_line[x-1].g * 3;
            sub_val += cur_line[x+1].g * 3;
            sub_val += pre_line[ x ].g * 3;
            sub_val += next_line[ x ].g * 3;


            sub_val += pre_line[x-1].g;
            sub_val += next_line[x-1].g;
            sub_val += pre_line[x+1].g;
            sub_val += next_line[x+1].g;

            dist_buf[x].g = sub_val >> 5;


            sub_val = cur_line[ x ].b << 4;

            sub_val += cur_line[x-1].b * 3;
            sub_val += cur_line[x+1].b * 3;
            sub_val += pre_line[ x ].b * 3;
            sub_val += next_line[ x ].b * 3;


            sub_val += pre_line[x-1].b;
            sub_val += next_line[x-1].b;
            sub_val += pre_line[x+1].b;
            sub_val += next_line[x+1].b;

            dist_buf[x].b = sub_val >> 5;



            dist_buf[x].a = 255;
        }
    }


    weight = 0;
    if (cur_line[x].a == 0)
    {
        if (cur_line[x-1].a != 0)
            weight += 3;
        if (pre_line[ x ].a != 0)
            weight += 3;
        if (next_line[ x ].a != 0)
            weight += 3;

        if (pre_line[x-1].a != 0)
            weight ++;
        if (next_line[x-1].a != 0)
            weight ++;

            sub_val = (cur_line[x].r * (32-weight) + rgba_fg.r * weight) >> 5;
            dist_buf[x].r = sub_val;

            sub_val = (cur_line[x].g * (32-weight) + rgba_fg.g * weight) >> 5;
            dist_buf[x].g = sub_val;

            sub_val = (cur_line[x].b * (32-weight) + rgba_fg.b * weight) >> 5;
            dist_buf[x].b = sub_val;

            dist_buf[x].a = 255;

    }
    else
    {
        sub_val = cur_line[x].r * 21; /* (32-16+ 3 + 2*1); */
        sub_val += cur_line[x-1].r * 3;
        sub_val += pre_line[x].r * 3;
        sub_val += next_line[x].r * 3;

        sub_val += pre_line[x-1].r;
        sub_val += next_line[x-1].r;

        dist_buf[x].r = sub_val >> 5;

        sub_val = cur_line[x].g * 21;
        sub_val += cur_line[x-1].g * 3;
        sub_val += pre_line[x].g * 3;
        sub_val += next_line[x].g * 3;

        sub_val += pre_line[x-1].g;
        sub_val += next_line[x-1].g;

        dist_buf[x].g = sub_val >> 5;

        sub_val = cur_line[x].b * 21;
        sub_val += cur_line[x-1].b * 3;
        sub_val += pre_line[x].b * 3;
        sub_val += next_line[x].b * 3;

        sub_val += pre_line[x-1].b;
        sub_val += next_line[x-1].b;

        dist_buf[x].b = sub_val >> 5;

        dist_buf[x].a = 255;
    }

}
#endif /* !ADJUST_SUBPEXIL_WIEGHT */

static void do_subpixel_filter (RGB* rgba_buf, int w, int h, RGB rgba_fg)
{
    RGB* cur_dist_buf = rgba_buf+ w * h;
    RGB* pre_dist_buf = cur_dist_buf + w;

    RGB* pre_src_line;
    RGB* cur_src_line;
    RGB* next_src_line;
    RGB* tmp;

    int y;

    /* the first line */
    pre_src_line = rgba_buf;
    cur_src_line = rgba_buf;
    next_src_line = rgba_buf + w;

    do_subpixel_line(pre_dist_buf, pre_src_line, cur_src_line,
        next_src_line, w, rgba_fg);

    /* inner point */
    for (y=1; y<h-1; y++)
    {
        pre_src_line = cur_src_line;
        cur_src_line = next_src_line;
        next_src_line += w;

        do_subpixel_line(cur_dist_buf, pre_src_line, cur_src_line,
            next_src_line, w, rgba_fg);

        /*save result of pre_line*/
        memcpy(pre_src_line, pre_dist_buf, w*4);

        /*move roll queue*/
        tmp = cur_dist_buf;
        cur_dist_buf = pre_dist_buf;
        pre_dist_buf = tmp;
    }

    /*cur_line:h-2=>h-1, next_line:h-1=>h-1*/
    pre_src_line = cur_src_line;
    cur_src_line = next_src_line;
    /*next_src_line is itself*/
    do_subpixel_line (cur_dist_buf, pre_src_line, cur_src_line,
            next_src_line, w, rgba_fg);

    memcpy(pre_src_line, pre_dist_buf, w*4);
    memcpy(cur_src_line, cur_dist_buf, w*4);
}

#ifdef _MGFONT_FT2
static void expand_subpixel_freetype (int w, int h,
                const BYTE* ft_a_buf, int ft_a_pitch,
                RGB* rgba_buf, RGB rgba_fg, int bold, int italic)
{
    int x;
    int y;
    int font_w = w - italic;
    const BYTE* sub_a_cur;
    RGB*  rgba_cur;
    Uint32 sub_val;
    int fg_alpha;

    for (y=0; y<h; y++)
    {
        sub_a_cur = ft_a_buf + y * ft_a_pitch;

        rgba_cur = rgba_buf + y * w;
        if (italic)
        {
            rgba_cur += (h - y) / 2;
        }

        for (x=0; x<font_w; x++)
        {
#if 0
            sub_val = rgba_fg.r * *sub_a_cur + rgba_cur->r * (255-*sub_a_cur);
            rgba_cur->r = sub_val /255;
            sub_a_cur++;

            sub_val = rgba_fg.g * *sub_a_cur + rgba_cur->g * (255-*sub_a_cur);
            rgba_cur->g = sub_val /255;
            sub_a_cur++;

            sub_val = rgba_fg.b * *sub_a_cur + rgba_cur->b * (255-*sub_a_cur);
            rgba_cur->b = sub_val /255;
            sub_a_cur++;
#else
            if (*sub_a_cur++)
            {
                fg_alpha = *(sub_a_cur-1) + 1;
                sub_val = rgba_fg.r * fg_alpha + rgba_cur->r * (256-fg_alpha);
                rgba_cur->r = sub_val >> 8;
            }

            if (*sub_a_cur++)
            {
                fg_alpha = *(sub_a_cur-1) + 1;
                sub_val = rgba_fg.g * fg_alpha + rgba_cur->g * (256-fg_alpha);
                rgba_cur->g = sub_val >> 8;
            }

            if (*sub_a_cur++)
            {
                fg_alpha = *(sub_a_cur-1) + 1;
                sub_val = rgba_fg.b * fg_alpha + rgba_cur->b * (256-fg_alpha);
                rgba_cur->b = sub_val >> 8;
            }
#endif

            rgba_cur->a = 255;

            if (bold) {
                /* FIXME FIXME */
            }

            rgba_cur++;
        }
    }
}

#endif /* _MGFONT_FT2 */

static void* expand_bkbmp_to_rgba (PDC pdc, DEVFONT* devfont,
            BITMAP* bk_bmp, const BYTE* bits,
            int pitch, int bold, int italic, unsigned short scale)
{
    static RGB* rgba_buf = NULL;
    static int    rgba_buf_size = 0;

    int w = bk_bmp->bmWidth;
    int h = bk_bmp->bmHeight;
    int x;
    int y;
    int bpp = bk_bmp->bmBytesPerPixel;
    int needed_rgba_buf_size = w * h * 4 + w*4*2;

    BYTE* src_pixel;
    RGB* dist_line_rgba;

    RGB* src_line_rgba;
    BYTE* dist_pixel;

    gal_pixel pixel;
    RGB rgba_fg;


    /*for char_bmp, and two line used by filter*/
    if (rgba_buf_size < needed_rgba_buf_size)
    {
        rgba_buf_size = needed_rgba_buf_size;
        rgba_buf = realloc(rgba_buf, rgba_buf_size);
    }

    if (rgba_buf == NULL)
        return NULL;

    for (y=0; y<h; y++)
    {
        src_pixel = bk_bmp->bmBits + bk_bmp->bmPitch * y;
        dist_line_rgba = rgba_buf + w * y;

        for (x=0; x<w; x++)
        {
            pixel = _mem_get_pixel(src_pixel, bpp);

            GAL_GetRGBA (pixel, pdc->surface->format, &(dist_line_rgba[x].r),
                    &(dist_line_rgba[x].g), &(dist_line_rgba[x].b),
                    &(dist_line_rgba[x].a));

            dist_line_rgba[x].a = 0;

            src_pixel += bpp;
        }
    }

    /* expand char to rgba_buf; */
    GAL_GetRGBA (pdc->textcolor, pdc->surface->format, &(rgba_fg.r),
            &(rgba_fg.g), &(rgba_fg.b), &(rgba_fg.a));
    rgba_fg.a = 255;

#ifdef _MGFONT_FT2
    if (ft2IsFreeTypeDevfont (devfont) &&
            ft2GetLcdFilter (devfont) != 0) {
        expand_subpixel_freetype (w, h, bits, pitch, rgba_buf, rgba_fg,
                bold, italic);
    }
    else
#endif
    {
        expand_char_bitmap_to_rgba_scale (w, h, bits, pitch, rgba_buf, rgba_fg,
                bold, italic, scale);
        /*draw rgba_fg to old rgba*/
        do_subpixel_filter (rgba_buf, w, h, rgba_fg);
    }

    /*change old_rgba_buf to dc format*/
    for (y=0; y<h; y++)
    {
        src_line_rgba = rgba_buf + w * y;
        dist_pixel = bk_bmp->bmBits + bk_bmp->bmPitch * y;

        for (x=0; x<w; x++)
        {
            pixel = GAL_MapRGB (pdc->surface->format, src_line_rgba[x].r,
                    src_line_rgba[x].g, src_line_rgba[x].b);
            dist_pixel = _mem_set_pixel(dist_pixel, bpp, pixel);
        }
    }

    return bk_bmp->bmBits;
}

//*******************************************************{
static BOOL get_subpixel_bmp (PDC pdc, Glyph32 glyph_value, SIZE* bbx_size,
        GAL_Rect* fg_gal_rc, int bold, int italic)
{
    LOGFONT* logfont;
    DEVFONT* devfont;
    unsigned short scale;
    int data_pitch = 0;
    const BYTE* data = NULL;
    bold = 0;

    logfont = pdc->pLogFont;
    devfont = SELECT_DEVFONT (logfont, glyph_value);

#ifdef _MGFONT_FT2
    if (ft2IsFreeTypeDevfont (devfont) &&
            ft2GetLcdFilter (devfont) &&
            *devfont->font_ops->get_glyph_greybitmap) {
        /* the returned bits will be the subpixled pixmap */
        data = (*devfont->font_ops->get_glyph_greybitmap) (logfont, devfont,
                REAL_GLYPH(glyph_value), &data_pitch, &scale);
    }
#endif

    if (data == NULL)
    {
        data = (*devfont->font_ops->get_glyph_monobitmap) (logfont, devfont,
                REAL_GLYPH(glyph_value), &data_pitch, &scale);
    }

    if (data == NULL)
        return 0;

    GAL_GetBox (pdc->surface, fg_gal_rc, &char_bmp);

    if (logfont->style & FS_FLIP_HORZ) {
        HFlipBitmap (&char_bmp,char_bmp.bmBits +
                char_bmp.bmPitch * char_bmp.bmHeight);
    }
    if (logfont->style & FS_FLIP_VERT) {
        VFlipBitmap (&char_bmp, char_bmp.bmBits +
                char_bmp.bmPitch * char_bmp.bmHeight);
    }

    expand_bkbmp_to_rgba (pdc, devfont, &char_bmp, (const BYTE*)data ,
            data_pitch, bold, italic, scale);
    return TRUE;
}

static BOOL get_book_bmp (PDC pdc, Glyph32 glyph_value, SIZE* bbx_size,
        int bold, int italic)
{
    LOGFONT* logfont;
    DEVFONT* devfont;
    unsigned short scale;
    int data_pitch;

    int bpp;
    gal_pixel bgcolor;
    gal_pixel fgcolor;


    const BYTE* data;

    logfont = pdc->pLogFont;
    devfont = SELECT_DEVFONT (logfont, glyph_value);
    bgcolor = pdc->bkcolor;

    data = NULL;

    /*get preybitmap and expand*/
    if (devfont->font_ops->get_glyph_greybitmap) {
        data = (*devfont->font_ops->get_glyph_greybitmap) (logfont, devfont,
                REAL_GLYPH(glyph_value), &data_pitch, &scale);
    }

    if (data)
    {
#if 1
        if (pdc->bkmode == BM_TRANSPARENT) {
#endif
            if (pdc->alpha_pixel_format && pdc->rop == ROP_SET) {
                /*pdc->gray_pixels -- alpha(0-max)*/
                char_bmp.bmType = BMP_TYPE_PRIV_PIXEL | BMP_TYPE_ALPHA;
                char_bmp.bmAlphaPixelFormat = pdc->alpha_pixel_format;
            }
            else {
                /*pdc->gray_pixels -- from bkcolor to textcolor*/
                char_bmp.bmType = BMP_TYPE_COLORKEY;
                char_bmp.bmColorKey = pdc->gray_pixels [0];
                bgcolor = pdc->gray_pixels [0];
            }
#if 1
        }
        else
        {
            /*FIXME can be delete ? */
            char_bmp.bmType = BMP_TYPE_COLORKEY;
            char_bmp.bmColorKey = bgcolor;
        }
#endif

        /* draw data to char_bmp.bmBits by pdc->gray_pixels*/
        expand_char_pixmap (pdc, bbx_size->cx, bbx_size->cy, data, char_bmp.bmBits,
                bold, italic, data_pitch, scale);

        return TRUE;
    }

    data = (*devfont->font_ops->get_glyph_monobitmap) (logfont, devfont,
            REAL_GLYPH(glyph_value), &data_pitch, &scale);

    if (data == NULL)
        return FALSE;

    if (pdc->alpha_pixel_format)
    {
        char_bmp.bmType = BMP_TYPE_PRIV_PIXEL | BMP_TYPE_ALPHA;
        char_bmp.bmAlphaPixelFormat = pdc->alpha_pixel_format;
        bpp = 1;
        bgcolor = 0;
        fgcolor = 1;
    }
    else
    {
        fgcolor = pdc->textcolor;
        bgcolor = pdc->bkcolor;
        bpp = GAL_BytesPerPixel (pdc->surface);
        char_bmp.bmType = BMP_TYPE_COLORKEY;

        if (bgcolor == fgcolor)
            bgcolor ^= 1;

        char_bmp.bmColorKey = bgcolor;
    }

    if (scale < 2) {
        expand_char_bitmap (bbx_size->cx, bbx_size->cy, data, data_pitch, bpp,
                char_bmp.bmBits, bgcolor, fgcolor, bold, italic);
    }
    else {
        expand_char_bitmap_scale (bbx_size->cx, bbx_size->cy, data, data_pitch, bpp,
                char_bmp.bmBits, bgcolor, fgcolor,
                bold, italic, scale);
    }

    if (bpp == 1)
        char_bmp.bmBits = do_low_pass_filtering (pdc);
    return TRUE;
}


static BOOL get_light_bmp (PDC pdc, Glyph32 glyph_value, SIZE* bbx_size,
        GAL_Rect* fg_rect, int bold, int italic)
{
    LOGFONT* logfont;
    DEVFONT* devfont;
    unsigned short scale;
    int pitch;

    gal_pixel bgcolor;
    gal_pixel fgcolor;
    int bpp;

    const BYTE* data;

    logfont = pdc->pLogFont;
    devfont = SELECT_DEVFONT (logfont, glyph_value);
    bgcolor = pdc->bkcolor;

    data = NULL;

    data = (*devfont->font_ops->get_glyph_monobitmap) (logfont, devfont,
            REAL_GLYPH(glyph_value), &pitch, &scale);

    if (data == NULL)
        return FALSE;

    if (pdc->textcolor != pdc->bkcolor)
    {
        char_bmp.bmType = BMP_TYPE_COLORKEY;
        char_bmp.bmColorKey = pdc->filter_pixels [0];

        /* special handle for light style */
        memset (char_bmp.bmBits, 0, char_bits_size);

        if (scale < 2)
        {
            expand_char_bitmap (bbx_size->cx, bbx_size->cy, data, pitch, 1,
                    char_bmp.bmBits + char_bmp.bmPitch + 1,
                    0, 1, bold, italic);
        }
        else
        {
            expand_char_bitmap_scale (bbx_size->cx, bbx_size->cy, data, pitch, 1,
                    char_bmp.bmBits + char_bmp.bmPitch + 1,
                    0, 1, bold, italic, scale);
        }

        char_bmp.bmWidth += 2;
        char_bmp.bmHeight += 2;
        char_bmp.bmBits = do_inflate_filtering (pdc);

        /* special handle for light style */

        /*FIXME not need to set pdc->rc_output*/
        //InflateRect (&pdc->rc_output, 1, 1);

        fg_rect->x --; fg_rect->y --;
        fg_rect->w += 2; fg_rect->h += 2;
        return TRUE;
    }
    else
    {

        fgcolor = pdc->textcolor;
        bgcolor = pdc->bkcolor;
        char_bmp.bmType = BMP_TYPE_COLORKEY;

        if (bgcolor == fgcolor)
            bgcolor ^= 1;

        char_bmp.bmColorKey = bgcolor;
        bpp = GAL_BytesPerPixel (pdc->surface);

        if (scale < 2) {
            expand_char_bitmap (bbx_size->cx, bbx_size->cy, data, pitch, bpp,
                    char_bmp.bmBits, bgcolor, fgcolor, bold, italic);
        }
        else {
            expand_char_bitmap_scale (bbx_size->cx, bbx_size->cy, data, pitch, bpp,
                    char_bmp.bmBits, bgcolor, fgcolor,
                    bold, italic, scale);
        }

        return TRUE;
    }

}

static BOOL get_regular_bmp (PDC pdc, Glyph32 glyph_value, SIZE* bbx_size,
        int bold, int italic)
{
    LOGFONT* logfont;
    DEVFONT* devfont;
    unsigned short scale;
    int pitch;

    gal_pixel bgcolor = pdc->bkcolor;
    gal_pixel fgcolor = pdc->textcolor;
    int bpp = GAL_BytesPerPixel (pdc->surface);

    const BYTE* data;

    logfont = pdc->pLogFont;
    devfont = SELECT_DEVFONT (logfont, glyph_value);
    bgcolor = pdc->bkcolor;

    data = NULL;

    data = (*devfont->font_ops->get_glyph_monobitmap) (logfont, devfont,
            REAL_GLYPH(glyph_value), &pitch, &scale);

    if (!data)
        return FALSE;

    char_bmp.bmType = BMP_TYPE_COLORKEY;

    if (bgcolor == fgcolor)
    {
        bgcolor ^= 1;
    }
    char_bmp.bmColorKey = bgcolor;

    if (scale < 2) {
        expand_char_bitmap (bbx_size->cx, bbx_size->cy, data, pitch, bpp,
                char_bmp.bmBits, bgcolor, fgcolor, bold, italic);
    }
    else {
        expand_char_bitmap_scale (bbx_size->cx, bbx_size->cy, data, pitch, bpp,
                char_bmp.bmBits, bgcolor, fgcolor,
                bold, italic, scale);
    }
    return TRUE;
}

int _gdi_draw_one_glyph (PDC pdc, Glyph32 glyph_value, BOOL direction,
            int x, int y, int* adv_x, int* adv_y)
{
    LOGFONT* logfont;
    DEVFONT* devfont;

    BBOX bbox;
    int advance;
    GAL_Rect fg_gal_rc;
    GAL_Rect bg_gal_rc;

    RECT rc_output;
    RECT rc_front;
    RECT rc_tmp;
    RECT rc_back;
    BOOL need_rc_back = FALSE;

    int italic = 0;
    int bold = 0;
    SIZE bbx_size;

    POINT area[4];
    int flag = 0;
    int glyph_bmptype;

    y += pdc->alExtra;

    rc_tmp = pdc->rc_output;
    advance = _gdi_get_glyph_advance (pdc, glyph_value, direction,
            x, y, adv_x, adv_y, &bbox);

    logfont = pdc->pLogFont;
    devfont = SELECT_DEVFONT (logfont, glyph_value);

    glyph_bmptype = devfont->font_ops->get_char_type (logfont, devfont)
            & DEVFONTGLYPHTYPE_MASK_BMPTYPE;

    // VincentWei: only use auto bold when the weight of devfont does not
    // match the weight of logfont.
    if ((logfont->style & FS_WEIGHT_MASK) > FS_WEIGHT_MEDIUM
            && (devfont->style & FS_WEIGHT_MASK) < FS_WEIGHT_DEMIBOLD
            && (glyph_bmptype == DEVFONTGLYPHTYPE_MONOBMP)) {
        bold = GET_DEVFONT_SCALE (logfont, devfont);
    }

    if (logfont->style & FS_SLANT_ITALIC
            && !(devfont->style & FS_SLANT_ITALIC)) {
        italic = devfont->font_ops->get_font_height (logfont, devfont) >> 1;
    }

    fg_gal_rc.x = bbox.x;
    fg_gal_rc.y = bbox.y;
    fg_gal_rc.w = bbox.w + italic;
    fg_gal_rc.h = bbox.h;

#if 0 // VincentWei: use FS_DECORATE_XXXX instead (4.0.0)
    if ( pdc->bkmode != BM_TRANSPARENT
         || logfont->style & FS_UNDERLINE_LINE
         || logfont->style & FS_STRUCKOUT_LINE )
#else
    if (pdc->bkmode != BM_TRANSPARENT
         || logfont->style & FS_DECORATE_UNDERLINE
         || logfont->style & FS_DECORATE_STRUCKOUT)
#endif
        need_rc_back = TRUE;

#if 0 // VincentWei: use FS_RENDER_MASK instead (4.0.0)
    if (glyph_bmptype == DEVFONTGLYPHTYPE_MONOBMP) {
        if (logfont->style & FS_WEIGHT_BOOK_LIGHT) {
            fg_gal_rc.x--; fg_gal_rc.y--;
            fg_gal_rc.w += 2; fg_gal_rc.h += 2;
//            *adv_x  += 1;
//            advance += 1;
        }
    }
#else
    if (glyph_bmptype == DEVFONTGLYPHTYPE_MONOBMP
            && (logfont->style & FS_RENDER_MASK) == FS_RENDER_OUTLINE) {
        fg_gal_rc.x--; fg_gal_rc.y--;
        fg_gal_rc.w += 2; fg_gal_rc.h += 2;
    }
    else if (glyph_bmptype == DEVFONTGLYPHTYPE_SUBPIXEL) {
        fg_gal_rc.w += 2;
    }
#endif

    if (need_rc_back) {
        if (direction)
            make_back_area(pdc, x, y, x+*adv_x, y+*adv_y,
                    area, &bg_gal_rc, &flag);
        else
            make_back_area(pdc, x+*adv_x, y+*adv_y, x, y,
                    area, &bg_gal_rc, &flag);

#if 0 // VincentWei: use FS_RENDER_MASK instead (4.0.0)
        if (glyph_bmptype == DEVFONTGLYPHTYPE_MONOBMP) {
            if (logfont->style & FS_WEIGHT_BOOK_LIGHT) {
                bg_gal_rc.x--; bg_gal_rc.y--;
                bg_gal_rc.w += 2; bg_gal_rc.h += 2;
            }
        }
#else
        if (glyph_bmptype == DEVFONTGLYPHTYPE_MONOBMP
                && (logfont->style & FS_RENDER_MASK) == FS_RENDER_OUTLINE) {
            bg_gal_rc.x--; bg_gal_rc.y--;
            bg_gal_rc.w += 2; bg_gal_rc.h += 2;
        }
        else if (glyph_bmptype == DEVFONTGLYPHTYPE_SUBPIXEL) {
            bg_gal_rc.w += 2;
        }
#endif
        make_back_rect(&rc_back, area, &bg_gal_rc, flag);
    }

    rc_front.left = fg_gal_rc.x;
    rc_front.top = fg_gal_rc.y;
    rc_front.right = fg_gal_rc.x + fg_gal_rc.w;
    rc_front.bottom = fg_gal_rc.y + fg_gal_rc.h + 1; /* for under line */

    if (need_rc_back)
        GetBoundRect (&rc_output, &rc_back, &rc_front);
    else
        rc_output = rc_front;

    if (!(pdc = __mg_check_ecrgn ((HDC)pdc))) {
        return advance;
    }

    if (!IntersectRect(&pdc->rc_output, &rc_output, &pdc->rc_output)) {
        goto end;
    }


    if (WITHOUT_DRAWING (pdc)) goto end;

    ENTER_DRAWING (pdc);

    pdc->step = 1;
    pdc->cur_ban = NULL;

    /*draw back ground */
    if (pdc->bkmode != BM_TRANSPARENT) {
        pdc->cur_pixel = pdc->bkcolor;
        draw_back_area (pdc, area, &bg_gal_rc, flag);
    }

    if (devfont->font_ops->get_glyph_prbitmap != NULL)
    {
        if(!(devfont->font_ops->get_glyph_prbitmap) (logfont, devfont,
                    REAL_GLYPH(glyph_value), &char_bmp)){
            is_draw_glyph = FALSE;
        }
    }
    else {
        prepare_bitmap (pdc, fg_gal_rc.w, fg_gal_rc.h);

        bbx_size.cx = bbox.w;
        bbx_size.cy = bbox.h;

#if 0 // VincentWei: use FS_RENDER_MASK instead (4.0.0)
        if (logfont->style & FS_WEIGHT_BOOK) {
            if(! get_book_bmp(pdc, glyph_value, &bbx_size, bold, italic))
                is_draw_glyph = FALSE;
        }
        else if (logfont->style & FS_WEIGHT_LIGHT) {
            if(!get_light_bmp (pdc, glyph_value, &bbx_size,  &fg_gal_rc, bold, italic))
                is_draw_glyph = FALSE;
        }
        else if ((logfont->style & FS_WEIGHT_MASK) == FS_WEIGHT_SUBPIXEL) {
            if (! get_subpixel_bmp (pdc, glyph_value, &bbx_size,
                        &fg_gal_rc, bold, italic))
                is_draw_glyph = FALSE;
        }
        else {
            if(! get_regular_bmp (pdc, glyph_value, &bbx_size, bold, italic))
                is_draw_glyph = FALSE;
        }
#else
        switch (logfont->style & FS_RENDER_MASK) {
        case FS_RENDER_GREY:
            if (!get_book_bmp(pdc, glyph_value, &bbx_size, bold, italic))
                is_draw_glyph = FALSE;
            break;

        case FS_RENDER_OUTLINE:
            if (!get_light_bmp (pdc, glyph_value, &bbx_size,  &fg_gal_rc, bold, italic))
                is_draw_glyph = FALSE;
            break;

        case FS_RENDER_SUBPIXEL:
            if (!get_subpixel_bmp (pdc, glyph_value, &bbx_size,
                        &fg_gal_rc, bold, italic))
                is_draw_glyph = FALSE;
            break;

        default:
            if (!get_regular_bmp (pdc, glyph_value, &bbx_size, bold, italic))
                is_draw_glyph = FALSE;
            break;
        }
#endif
    }

    if (is_draw_glyph) {
        /*draw glyph bitmap*/
        pdc->cur_pixel = pdc->textcolor;
        pdc->skip_pixel = pdc->bkcolor;

        if (char_bmp.bmBits) {
            /*
             * To optimize:
             * the flip should do when expand bitmap/pixmap.
             */
            if (logfont->style & FS_FLIP_HORZ) {
                HFlipBitmap (&char_bmp, char_bmp.bmBits +
                        char_bmp.bmPitch * char_bmp.bmHeight);
            }
            if (logfont->style & FS_FLIP_VERT) {
                VFlipBitmap (&char_bmp, char_bmp.bmBits +
                        char_bmp.bmPitch * char_bmp.bmHeight);
            }

            _dc_fillbox_bmp_clip (pdc, &fg_gal_rc, &char_bmp);
        }
    }
    draw_glyph_lines (pdc, x, y, x + *adv_x, y + *adv_y);

    LEAVE_DRAWING (pdc);
end:
    pdc->rc_output = rc_tmp;
    UNLOCK_GCRINFO (pdc);
    return advance;
}

