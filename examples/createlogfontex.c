    static LOGFONT  *logfont, *logfontgb12, *logfontttf;

    logfont = CreateLogFontEx (NULL, "SansSerif", "ISO8859-1",
                FONT_WEIGHT_REGULAR, FONT_SLANT_ITALIC, FONT_FLIP_NONE,
                FONT_OTHER_NIL, FONT_DECORATE_NONE, FONT_RENDER_MONO,
                16, 0);
    logfontgb12 = CreateLogFontEx (NULL, "song", "GB2312",
                FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                FONT_OTHER_NIL, FONT_DECORATE_NONE, FONT_RENDER_MONO,
                12, 0);
    logfontttf = CreateLogFontEx ("ttf", "Courier", "UTF-8",
                FONT_WEIGHT_BLACK, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                FS_OTHER_LCDPORTRAIT, FONT_DECORATE_NONE, FONT_RENDER_SUBPIXEL,
                24, 0);
