/*
** $Id: qpf.h 13674 2010-12-06 06:45:01Z wanzheng $
**
** qpf.h: the head file of Qt Prerendered Font operation set.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Create date: 2003/01/28
*/

#ifndef GUI_FONT_QPF_H
    #define GUI_FONT_QPF_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define FLAG_MODE_SMOOTH   1

typedef struct _QPF_GLYPHMETRICS {
    Uint8 linestep;
    Uint8 width;
    Uint8 height;
    Uint8 padding;

    Sint8 bearingx;     /* Difference from pen position to glyph's left bbox */
    Uint8 advance;      /* Difference between pen positions */
    Sint8 bearingy;     /* Used for putting characters on baseline */

    Sint8 reserved;     /* Do not use */
} QPF_GLYPHMETRICS;

typedef struct _QPF_GLYPH
{
    const QPF_GLYPHMETRICS* metrics;
    const unsigned char* data;
} QPF_GLYPH;

typedef struct _QPF_GLYPHTREE
{
    unsigned int min, max;
    struct _QPF_GLYPHTREE* less;
    struct _QPF_GLYPHTREE* more;
    QPF_GLYPH* glyph;
#ifdef _MGINCORE_RES
    const QPF_GLYPHMETRICS* metrics;
    const unsigned char* data;
#endif
} QPF_GLYPHTREE;

#ifdef __GNUC__
typedef struct __attribute__ ((packed)) _QPFMETRICS
#elif WIN32
#pragma pack(8)
typedef struct _QPFMETRICS
#else
#   warning /* XXX: Please make sure sizeof(QPFMETRICS) == 10 */
typedef struct _QPFMETRICS
#endif
{
    Sint8 ascent, descent;
    Sint8 leftbearing, rightbearing;

    Uint8 maxwidth;
    Sint8 leading;
    Uint8 flags;
    Uint8 underlinepos;

    Uint8 underlinewidth;
    Uint8 reserved;
} QPFMETRICS;
#ifdef WIN32
#pragma pack()
#endif

typedef struct 
{
    unsigned int height;
    unsigned int width;

    unsigned int file_size;
    QPFMETRICS* fm;

    QPF_GLYPHTREE* tree;
} QPFINFO;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* GUI_FONT_QPF_H */


