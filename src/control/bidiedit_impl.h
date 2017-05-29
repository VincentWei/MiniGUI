/*
** $Id: bidiedit_impl.h 9892 2008-03-20 02:50:02Z xwyan $
**
** bidiedit_impl.h: the head file of BidiEdit Control module.
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 2008/03/10
*/

#ifndef BIDI_EDIT_IMPL_H_
#define BIDI_EDIT_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

typedef struct _GLYPHBUF
{   
    GLYPHMAPINFO* glyph_map;     /* glyph info array */ 
    int*  glyphs;                /* glyph array  */
    int   glyphs_len;            /* glyph array len or buffer used size */
        
    int   blocksize;             /* block size, a block is an allocate unit */
    int   glyphs_buffsize;       /* glyph string buffer size */        
    int   glyphmap_buffsize;     /* glyph map buffer size */        
} GLYPHBUF;

typedef struct tagBIDISLEDITDATA
{
    StrBuffer str_content;  // string text buffer
    GLYPHBUF glyph_content; // glyph string buffer

    int     editPos;        // current edit position
    int     selStart;       // selection start position
    int     selEnd;         // selection end position

    DWORD   status;         // status of box

    int     nContX;         // content x offset position
    int     nContW;         // content width
    RECT    rcVis;          // content rect
    int     nVisW;          // visible area width
    int     startx;         // text x display position
    int     starty;         // text y display position

    int     leftMargin;     // left margin
    int     topMargin;      // top margin
    int     rightMargin;    // right margin
    int     bottomMargin;   // bottom margin

    int     nBlockSize;     /* string buffer block size */
    char    *tiptext;       // tip text

    int     hardLimit;      // hard limit

    int     passwdChar;     // password character

    int     changed;        // change flag

}BIDISLEDITDATA;

typedef BIDISLEDITDATA* PBIDISLEDITDATA;

static inline void* 
glyphbuf_alloc (GLYPHBUF* glyph_buff, int len, int block_size)
{
    glyph_buff->glyphs_buffsize = (len + (block_size - len % block_size))
                                     * sizeof (int);
    glyph_buff->glyphs = malloc (glyph_buff->glyphs_buffsize);

    glyph_buff->glyphmap_buffsize = (len + (block_size - len % block_size)) 
                                    * sizeof (GLYPHMAPINFO);
    glyph_buff->glyph_map = malloc (glyph_buff->glyphmap_buffsize);

    glyph_buff->glyphs_len = len;
    glyph_buff->blocksize = block_size;

    return glyph_buff->glyphs;
}

static inline void glyphbuf_free (GLYPHBUF *glyph_buff)
{
    if (glyph_buff) {
        free (glyph_buff->glyphs);
        free (glyph_buff->glyph_map);
    }
}

static inline void* glyphbuf_realloc (GLYPHBUF *glyph_buff, int len)
{
    if (len * sizeof(int) > glyph_buff->glyphs_buffsize || 
        len * sizeof(int) < glyph_buff->glyphs_buffsize - glyph_buff->blocksize)    {
        /* realloc glyph string mem */
        glyph_buff->glyphs_buffsize = (len + (glyph_buff->blocksize - 
                            len % glyph_buff->blocksize))* sizeof(int);
        glyph_buff->glyphs = realloc (glyph_buff->glyphs, 
                                glyph_buff->glyphs_buffsize);

        /* realloc glyph map mem */
        glyph_buff->glyphmap_buffsize = (len + (glyph_buff->blocksize - 
                        len % glyph_buff->blocksize))* sizeof(GLYPHMAPINFO);
        glyph_buff->glyph_map = realloc (glyph_buff->glyph_map, 
                        glyph_buff->glyphmap_buffsize);
    }

    return glyph_buff->glyphs;
}

BOOL RegisterBIDISLEditControl (void);

#ifdef __cplusplus
}
#endif

#endif /* BIDI_EDIT_IMPL_H_ */

