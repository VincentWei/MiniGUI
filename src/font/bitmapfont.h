
/*
** $Id$
**
** bitmapfont.h: the head file of Bitmap Font operation set.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Liu Peng.
**
** Create date: 2007/09/22
*/

#ifndef GUI_FONT_BITMAP_H
    #define GUI_FONT_BITMAP_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct _GLYPHTREENODE {
    unsigned int min, max;  
    int nr_glyphs; 
    int glyph_width;
    BITMAP *data;
    
    struct _GLYPHTREENODE *left;
    struct _GLYPHTREENODE *right;
        
} GLYPHTREENODE;

extern FONTOPS __mg_bitmap_font_ops;

typedef struct _BMPFONTINFO
{
    char name[LEN_UNIDEVFONT_NAME]; /* font name */
    int height;                 /* character height. */
    int ave_width;              /* average width of character */
    int max_width;              /* max width of character */
    
    GLYPHTREENODE* tree;        /* The pointer to glyph tree */
    int node_cnt;               /* The node count of AVL tree */
          
} BMPFONTINFO;    

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* GUI_FONT_BITMAP_H */


