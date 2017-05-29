/*
** Copyright (C) 2002 ~ 2007, FMSoft.
** Copyright (C) 2000 ~ 2002, Wei Yongming.
*/

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
	
typedef struct
{
    const char*     name;           /* font name */
    char            max_width;      /* max width in pixels */
    char            ave_width;      /* average width in pixels */
    int             height;         /* height in pixels */
    int             descent;        /* pixels below the base line */
    unsigned char   first_char;     /* first character in this font */
    unsigned char   last_char;      /* last character in this font */
    unsigned char   def_char;       /* default character in this font */
    unsigned short* offset;         /* glyph offsets into bitmap data or NULL */
    unsigned char*  width;          /* character widths or NULL */
    unsigned char*  bits;           /* 8-bit right-padded bitmap data */
    int             len_bits;       /* lenght of bits */
} VBFINFO;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

