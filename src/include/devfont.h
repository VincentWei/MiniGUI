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
/*
 *   This file is part of MiniGUI, a mature cross-platform windowing
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 *
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Or,
 *
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 *
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 *
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** devfont.h: the head file of Device Font Manager.
**
** Create date: 2000/xx/xx
*/

#ifndef GUI_DEVFONT_H
    #define GUI_DEVFONT_H

/* define _DEBUG_DEVFONF to dump the devfonts */
/* #undef _DEBUG_DEVFONT */

#define ACHAR_MBC_FLAG          0x80000000

#define IS_MBCHV(chv)   ((chv) & ACHAR_MBC_FLAG)
#define SET_MBCHV(chv)  ((chv) | ACHAR_MBC_FLAG)
#define REAL_ACHAR(chv) ((chv) & ~ACHAR_MBC_FLAG)

#define SELECT_DEVFONT_BY_ACHAR(logfont, chv) \
    IS_MBCHV(chv)?(logfont->devfonts[1]):(logfont->devfonts[0])

#define GLYPH_DEVFONT_INDEX_MASK    0xF0000000
#define GLYPH_DEVFONT_INDEX_SHIFT   28

/**
 * \def REAL_GLYPH(glyph)
 * \brief Get the real (DEVFONT) glyph value from a LOGFONT glyph.
 *
 * \param glyph The LOGFONT glyph value
 */
#define REAL_GLYPH(glyph)    ((glyph) & ~GLYPH_DEVFONT_INDEX_MASK)

/**
 * \def SELECT_DEVFONT_BY_GLYPH(logfont, glyph)
 * \brief Select a DEVFONT according to the LOGFONT glyph value.
 *
 * \param logfont The pointer to a logical font.
 * \param glyph   The logical glyph value.
 *
 */
#define SELECT_DEVFONT_BY_GLYPH(logfont, glyph) \
    (logfont)->devfonts[((glyph) >> GLYPH_DEVFONT_INDEX_SHIFT) & 0x07]

#define DFI_IN_GLYPH(glyph) \
    (((glyph) >> GLYPH_DEVFONT_INDEX_SHIFT) & 0x07)

/**
 * \def IS_MBC_GLYPH(glyph)
 * \brief Determine wether the glyph is multibyte glyph
 *
 * \param glyph The logfont glyph value
 */
//#define IS_MBC_GLYPH(glyph)  ((glyph) & GLYPH_DEVFONT_INDEX_MASK)

#define SET_GLYPH_DFI(glyph, dfi) \
    (((glyph) & ~GLYPH_DEVFONT_INDEX_MASK) | \
        (((Glyph32)((dfi) & 0x07)) << GLYPH_DEVFONT_INDEX_SHIFT))

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
BOOL font_InitializeScripteasy(void);
void font_UninitializeScripteasy(void);
#endif

BOOL font_InitSpecificalFonts (char* etc_section);
void font_TermSpecificalFonts (char* etc_section);

/* Device font management functions */
void font_AddSBDevFont (DEVFONT* dev_font);
void font_AddMBDevFont (DEVFONT* dev_font);
void font_DelRelatedDevFont (DEVFONT* dev_font);
void font_ResetDevFont (void);

DEVFONT* font_GetMatchedSBDevFont (LOGFONT* lf, const char* family);
DEVFONT* font_GetMatchedMBDevFont (LOGFONT* lf, const char* family, int slot);

static inline unsigned short get_devfont_scale(LOGFONT* lf,
        const DEVFONT* df)
{
    int i;
    for (i = 0; i < MAXNR_DEVFONTS; i++) {
        if (lf->devfonts[i] == df)
            return lf->scales[i];
    }

    return 1;
}

#define GET_DEVFONT_SCALE(logfont, devfont) \
        get_devfont_scale(logfont, devfont)

#define SET_DEVFONT_SCALE(logfont, df_slot, scale) \
        (logfont)->scales[(df_slot)] = (scale)

unsigned short font_GetBestScaleFactor (int height, int expect);

/* Add for bitmap font interface */
void font_DelSBDevFont (DEVFONT* dev_font);
void font_DelMBDevFont (DEVFONT* dev_font);

#ifdef _DEBUG_DEVFONT
void dbg_dumpDevFonts (void);
#endif

static inline BOOL check_zero_width(unsigned int t)
{
    return (t & ACHARTYPE_BASIC_MASK) == ACHAR_BASIC_ZEROWIDTH;
}

static inline BOOL check_vowel(unsigned int t)
{
    return ((t & ACHARTYPE_BASIC_MASK) == ACHAR_BASIC_VOWEL);
}

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
    Achar32 def_char_value;

    /** The method to get the length of the first character. */
    int (*len_first_char) (const unsigned char* mstr, int mstrlen);

    /** The method to get the character index. */
    Achar32 (*get_char_value) (const unsigned char* pre_mchar, int pre_len,
            const unsigned char* cur_mchar, int cur_len);

    /** The method to get the require shape type of a mchar. */
    Achar32 (*get_shaped_char_value) (const unsigned char* cur_mchar, int cur_len,
            int shape_type);

    /** The method to get the type of one character. */
    unsigned int (*char_type) (Achar32 chv);

    /** The method to get character number in the string function. */
    int (*nr_chars_in_str) (const unsigned char* mstr, int mstrlen);

    /** The method to judge a given charset name is belong to the character set */
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

    /** The method to get the BIDI type of one character. */
    BidiType (*bidi_char_type) (Achar32 chv);

    /** Get mirrored character */
    BOOL (*bidi_mirror_char) (Achar32 chv, Achar32* mirrored);

#ifdef _MGCHARSET_UNICODE
    /** The method to convert a mchar32 value to 32 bit UNICODE function. */
    Uchar32 (*conv_to_uc32) (Achar32 chv);

    /** The method to convert \a wc to multily byte character function. */
    int (*conv_from_uc32) (Uchar32 wc, unsigned char* mchar);
#endif /* _UNICODE_SUPPORT */
};

#define DEVFONTGLYPHTYPE_MONOBMP        GLYPHBMP_TYPE_MONO
#define DEVFONTGLYPHTYPE_GREYBMP        GLYPHBMP_TYPE_GREY
#define DEVFONTGLYPHTYPE_SUBPIXEL       GLYPHBMP_TYPE_SUBPIXEL
#define DEVFONTGLYPHTYPE_PRERDRBMP      GLYPHBMP_TYPE_PRERENDER

#define DEVFONTGLYPHTYPE_MASK_BMPTYPE   0x0F

/** The font operation structure. */
struct _FONTOPS
{
    /** The method to get the glyph bitmap type . */
    DWORD (*get_glyph_bmptype) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to get average width function. */
    int (*get_ave_width) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to get max width function. */
    int (*get_max_width) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to get font height function. */
    int (*get_font_height) (LOGFONT* logfont, DEVFONT* devfont);

    /** The method to get font size function. */
    int (*get_font_size) (LOGFONT* logfont, DEVFONT* devfont, int expect, int slot);

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
            Glyph32 glyph_value, SIZE* sz, int* pitch, unsigned short* scale);

    /** The method to get grey bitmap, pitch and scale function */
    const void* (*get_glyph_greybitmap) (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 galph_value, SIZE* sz, int* pitch, unsigned short* scale);

    /** The method to get the pre-rendered bitmap of one glyph. */
    int (*get_glyph_prbitmap) (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 glyph_value, BITMAP *bmp);

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
     */
    void* (*load_font_data) (DEVFONT* devfont, const char* fontname, const char* filename);

    /** The method to unload data of a devfont*/
    void (*unload_font_data) (DEVFONT* devfont, void* data);

    /**
     * The method to get the glyph value according to the character value.
     * If it is NULL, the glyph value will be equal to the character value.
     *
     * Since 4.0.0.
     */
    Glyph32 (*get_glyph_value) (LOGFONT* logfont, DEVFONT* devfont, Achar32 chv);

    /**
     * The method to get the kerning delta values
     *
     * Since 4.0.0.
     */
    void (*get_kerning) (LOGFONT* logfont, DEVFONT* devfont,
        Glyph32 prev, Glyph32 curr, int* delta_x, int* delta_y);

    /**
     * The method to get the FreeType2 FT_Face objece; only valid for FreeType2
     * font engine
     *
     * Since 4.0.0.
     */
    void* (*get_ft_face) (LOGFONT* logfont, DEVFONT* devfont);
};

typedef struct {
    char* type;
    FONTOPS* fontops;
} FONTOPS_INFO;

extern FONTOPS_INFO __mg_fontops_infos[];

typedef struct tagFT2FILEANDFACE {
    char*   filepathname;
    void*   face;
} FT2INFO;

const FT2INFO* __mg_ft2_get_face(LOGFONT* lf, Uchar32 uc, int* dfi);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_DEVFONT_H

