/*
** $Id: devfont.h 13674 2010-12-06 06:45:01Z wanzheng $
**
** devfont.h: the head file of Device Font Manager.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Create date: 2000/xx/xx
*/

#ifndef GUI_DEVFONT_H
    #define GUI_DEVFONT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* defined in devfont.c */
BOOL font_InitIncoreFonts (void);
BOOL font_TerminateIncoreFonts (void);

/* defined in freetype.c or freetype2.c */
#if defined _MGFONT_TTF || defined _MGFONT_FT2
BOOL font_InitFreetypeLibrary (void);
void font_TermFreetypeLibrary (void);
#endif

#ifdef _MGFONT_SEF
BOOL initialize_scripteasy(void);
void uninitialize_scripteasy(void);
#endif

BOOL font_InitSpecificalFonts (char* etc_section);
void font_TermSpecificalFonts (char* etc_section);

/* Device font management functions */
void font_AddSBDevFont (DEVFONT* dev_font);
void font_AddMBDevFont (DEVFONT* dev_font);
void font_DelRelatedDevFont (DEVFONT* dev_font);
void font_ResetDevFont (void);

DEVFONT* font_GetMatchedSBDevFont (LOGFONT* log_font);
DEVFONT* font_GetMatchedMBDevFont (LOGFONT* log_font);

#define GET_DEVFONT_SCALE(logfont, devfont) \
        ((devfont->charset_ops->bytes_maxlen_char > 1)?logfont->mbc_scale:logfont->sbc_scale)

#define SET_DEVFONT_SCALE(logfont, devfont, scale) \
        ((devfont->charset_ops->bytes_maxlen_char > 1)?(logfont->mbc_scale = scale):(logfont->sbc_scale = scale))

unsigned short font_GetBestScaleFactor (int height, int expect);

/* Add for bitmap font interface */
void font_DelSBDevFont (DEVFONT* dev_font);    
void font_DelMBDevFont (DEVFONT* dev_font);

#ifdef _DEBUG
void dbg_dumpDevFonts (void);
#endif

/* A type definition for an glyph value */
#define INV_GLYPH_VALUE    -1

static inline long get_file_size (const char* filename)
{
    FILE* fp = fopen (filename, "rb");
    long size = 0;

    if (fp) {
        fseek (fp, 0, SEEK_END);
        size = ftell (fp);
        fclose (fp);
    }
    return size;
}

static inline long get_opened_file_size (FILE* fp)
{
    long size;
    long pos = ftell (fp);

    fseek (fp, 0, SEEK_END);
    size = ftell (fp);
    fseek (fp, pos, SEEK_SET);
    return size;
}

/** The character set operation structure. */
struct _CHARSETOPS
{
	/** The character number of the character set. */
    int nr_chars;

	/** The byte number of the max length character. */
    int bytes_maxlen_char;

	/** The name of the character set. */
    const char* name;

	/** Default character. */
    Glyph32 def_glyph_value;

	/** The method to get the length of the first character function. */
    int (*len_first_char) (const unsigned char* mstr, int mstrlen);

	/** The method to get character offset function. */
    Glyph32 (*char_glyph_value) (const unsigned char* pre_mchar, int pre_len,
                const unsigned char* cur_mchar, int cur_len);

    /** The method to get the require shape type glyph value. */
    Glyph32 (*glyph_shape) (const unsigned char* cur_mchar, int cur_len, int shape_type);

	/** The method to get the type of one glyph. */
    unsigned int (*bidi_glyph_type) (Glyph32 glyph_value);

	/** The method to get the type of one glyph. */
    unsigned int (*glyph_type) (Glyph32 glyph_value);

	/** The method to get character number in the string function. */
    int (*nr_chars_in_str) (const unsigned char* mstr, int mstrlen);

	/** The method to judge the \a charset is belong to the character set
	 *  function.
	 */
    int (*is_this_charset) (const unsigned char* charset);

	/** The method to get  the length of the first substring. */
    int (*len_first_substr) (const unsigned char* mstr, int mstrlen);

	/** The method to get next word in the specitied length string function. */
    const unsigned char* (*get_next_word) (const unsigned char* mstr, 
                int strlen, WORDINFO* word_info);

	/** The method to get the position of the first character in the
	 *  specified length string function.
	 */
    int (*pos_first_char) (const unsigned char* mstr, int mstrlen);

	 /** reorder bidi string specified length function.
	 */
    Glyph32* (*bidi_reorder) (Glyph32* glyphs, int len);

#ifdef _MGCHARSET_UNICODE
	/** The method to convert \a mchar to 32 bit UNICODE function. */
    UChar32 (*conv_to_uc32) (Glyph32 glyph_value);

	/** The method to convert \a wc to multily byte character function. */
    int (*conv_from_uc32) (UChar32 wc, unsigned char* mchar);
#endif /* _UNICODE_SUPPORT */
};

#define DEVFONTGLYPHTYPE_MONOBMP        GLYPHBMP_TYPE_MONO
#define DEVFONTGLYPHTYPE_GREYBMP        GLYPHBMP_TYPE_GREY
#define DEVFONTGLYPHTYPE_SUBPIXEL       GLYPHBMP_TYPE_SUBPIXEL
#define DEVFONTGLYPHTYPE_PRERDRBMP      GLYPHBMP_TYPE_PRERENDER

#define DEVFONTGLYPHTYPE_MASK_BMPTYPE   0x0F

#define DEVFONTGLYPHTYPE_OUTLINE        0x10

/** The font operation structure. */
struct _FONTOPS
{
    /** The method to get the glyph type . */
    DWORD (*get_glyph_type) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to get average width function. */
    int (*get_ave_width) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to get max width function. */
    int (*get_max_width) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to get font height function. */
    int (*get_font_height) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to get font size function. */
    int (*get_font_size) (LOGFONT* logfont, DEVFONT* devfont, int expect);

    /** The method to get font ascent function. */
    int (*get_font_ascent) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to get font descent function. */
    int (*get_font_descent) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to judge the glyph is exist function. */
    BOOL (*is_glyph_existed) (LOGFONT* logfont, DEVFONT* devfont, 
            Glyph32 glyph_value);

    /** The method to get character advance function. */
    int (*get_glyph_advance) (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 galph_value, int* px, int* py);

    /** The method to get character bound box function. */
    int (*get_glyph_bbox) (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 galph_value, int* px, int* py, int* pwidth, int* pheight);

    /** The method to get mono-bitmap function. */
    const void* (*get_glyph_monobitmap) (LOGFONT* logfont, DEVFONT* devfont, 
            const Glyph32 glyph_value, int* pitch, unsigned short* scale);

    /** The method to get grey bitmap, pitch and scale function */
    const void* (*get_glyph_greybitmap) (LOGFONT* logfont, DEVFONT* devfont, 
            const Glyph32 galph_value, int* pitch, unsigned short* scale);

    /** The method to get the pre-rendered bitmap of one glyph. */
    int (*get_glyph_prbitmap) (LOGFONT* logfont, DEVFONT* devfont, 
            Glyph32 glaph_value, BITMAP *bmp);

    /* The method to preproccess start string output fuction, call this 
     * function before output a string, can be NULL.
     */
    void (*start_str_output) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to instance new device font function, can be NULL. */
    DEVFONT* (*new_instance) (LOGFONT* logfont, DEVFONT* devfont, 
            BOOL need_sbc_font);

    /** The method to delete instance of device font function, can be NULL. */
    void (*delete_instance) (DEVFONT* devfont);

    int (*is_rotatable)  (LOGFONT* logfont, DEVFONT* devfont, int rot_desired);

    /** The method to load data of a devfont from font file
     * FIXME real_fontname: return real name in file by a char point 
     **/
    //void* (*load_font_data) (char* fontname, char* filename, char* real_fontname);
    void* (*load_font_data) (const char* fontname, const char* filename);

    /** The method to unload data of a devfont*/
    void (*unload_font_data) (void* data);

};

typedef struct {
        char* type;
        FONTOPS* fontops;
} FONTOPS_INFO;

extern FONTOPS_INFO __mg_fontops_infos[];

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_DEVFONT_H

