    static LOGFONT  *logfont, *logfontgb12, *logfontbig24;

            logfont = CreateLogFont (NULL, "SansSerif", "ISO8859-1", 
                        FONT_WEIGHT_REGULAR, FONT_SLANT_ITALIC, FONT_SETWIDTH_NORMAL,
                        FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_LINE, 
                        16, 0);
            logfontgb12 = CreateLogFont (NULL, "song", "GB2312", 
                        FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
                        FONT_SPACING_CHARCELL, FONT_UNDERLINE_LINE, FONT_STRUCKOUT_LINE, 
                        12, 0);
            logfontbig24 = CreateLogFont (NULL, "ming", "BIG5", 
                        FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
                        FONT_SPACING_CHARCELL, FONT_UNDERLINE_LINE, FONT_STRUCKOUT_NONE, 
                        24, 0);
