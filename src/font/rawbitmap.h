/*
** $Id: rawbitmap.h 10974 2008-10-06 07:40:27Z houhuihua $
**
** rawbitmap.h: the head file of raw bitmap font operation set.
**
** Copyright (C) 2000 ~ 2002 Wei Yongming.
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
*/

#ifndef GUI_FONT_RAWBITMAP_H
    #define GUI_FONT_RAWBITMAP_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct
{
    int nr_glyphs;

    int width;
    int height;

    unsigned char* data;
    long data_size;
} RBFINFO;

typedef struct
{
    const char* name;
    RBFINFO* data;
} INCORE_RBFINFO;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_FONT_RAWBITMAP_H

