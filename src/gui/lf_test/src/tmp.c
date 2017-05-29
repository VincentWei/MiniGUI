
 /* \param other Other rendering features, can be one of the following values:
 *      - FONT_OTHER_NIL\n
 *        Not specified.
 *      - FONT_OTHER_AUTOSCALE\n
 *        Auto scale the bitmap glyph to match the desired font size.
 *      - FONT_OTHER_TTFNOCACHE\n
 *        Do not use cache when using TrueType font.
 *      - FONT_OTHER_TTFKERN\n
 *        Kern the glyph when using TrueType font.
 *      - FONT_OTHER_TTFNOCACHEKERN\n
 *        When using TrueType font, kern the glyph and do not use cache.
 *      - FONT_OTHER_LCDPORTRAIT\n
 *        When using TrueType font and sub-pixels smoothing strategy, 
 *        set lcd portrait and do not use kern the glyph.
 *      - FONT_OTHER_LCDPORTRAITKERN\n
 *        When using TrueType font and sub-pixels smoothing strategy, 
 *        set lcd portrait and use kern the glyph.
 * \param underline The underline of the logical font, can be one of the values:
 *      - FONT_UNDERLINE_ALL\n
 *        Any one.
 *      - FONT_UNDERLINE_LINE\n
 *        With underline.
 *      - FONT_UNDERLINE_NONE\n
 *        Without underline.
 * \param struckout The struckout line of the logical font, can be one of 
 *        the values:
 *      - FONT_STRUCKOUT_ALL\n
 *        Any one.
 *      - FONT_STRUCKOUT_LINE\n
 *        With struckout line.
 *      - FONT_STRUCKOUT_NONE\n
 *        Without struckout line.
 * \param size The size, i.e. the height, of the logical font. Note that 
 *        the size of the created logical font may be different from the 
 *        size expected.
 * \param rotation The rotation of the logical font, it is in units of 
 *        tenth degrees. Note that you can specify rotation only for 
 *        TrueType and Adobe Type1 fonts.
 * \return The pointer to the logical font created, NULL on error.
 *
 * \sa CreateLogFontIndirect, CreateLogFontByName, SelectFont
 *
 * Example:
 *
 * \include createlogfont.c
 *
 */
MG_EXPORT PLOGFONT GUIAPI CreateLogFont (const char* type, const char* family, 
        const char* charset, char weight, char slant, char flip, 
        char other, char underline, char struckout, 
        int size, int rotation);
