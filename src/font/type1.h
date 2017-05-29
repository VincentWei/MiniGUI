/*
** $Id: type1.h 10418 2008-06-29 11:19:05Z hejian $
**
** type1.h: Type1 font support based on t1lib.
**
** Copyright (C) 2000 ~ 2002 Song Lixin, Wei Yongming.
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** Created by Song Lixin, 2000/8/29
*/

#ifndef GUI_TYPE1_H
    #define GUI_TYPE1_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct
{
  char *encfilename;
  char **encoding;
} encstruct;

typedef struct tagTYPE1GLYPHINFO {
    int font_id;
    //BBox 	font_bbox;
    //int 	ave_width;
    BOOL valid;
} TYPE1INFO, *PTYPE1INFO;

typedef struct tagTYPE1INSTANCEINFO {
    PTYPE1INFO  type1_info;
    int		rotation;/*in tenthdegrees*/
    T1_TMATRIX  * pmatrix;
    int 	size;
    int 	font_height;
    int 	font_ascent;
    int 	font_descent;

    int 	max_width;
    int 	ave_width;

    double 	csUnit2Pixel;
    /* 
     * last char or string's info
     * T1_SetChar, T1_SetString, T1_AASetSting, T1_AASetString all return a static
     * glyph pointer, we save the related infomation here for later use.
     * */
    char 	last_bitmap_char;
    char 	last_pixmap_char;
    char 	* last_bitmap_str;
    char 	* last_pixmap_str;
    int 	last_ascent;
    int 	last_descent;
    int 	last_leftSideBearing;
    int 	last_rightSideBearing;
    int 	last_advanceX;
    int 	last_advanceY;
    unsigned long 	last_bpp;
    char  *	last_bits;
    
} TYPE1INSTANCEINFO, *PTYPE1INSTANCEINFO;

#define TYPE1_INFO_P(devfont) ((TYPE1INFO*)(devfont->data))
#define TYPE1_INST_INFO_P(devfont) ((TYPE1INSTANCEINFO*)(devfont->data))

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_TYPE1_H

