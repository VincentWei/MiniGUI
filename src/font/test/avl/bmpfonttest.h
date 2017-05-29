/*
** $Id$
**
** bitmapfont.h: Head file of bitmap font AVL tree unit test .
**
** Copyright (C) 2003 ~ 2007 Feynman Software
**
** All right reserved by Feynman Software.
**
** Current maintainer: Liu Peng.
**
** Create date: 2007/09/22
*/



#ifndef _BMPFONT_TEST_H
#define _BMPFONT_TEST_H

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>

typedef struct  _ELEMENT {
    char* start_mchar;
    int nr_glyphs;
    int glyph_width;
    BITMAP *data;  
} ELEMENT;

/* Insertion order. */
enum insert_order
{
        INS_RANDOM,                        /* Random order. */
        INS_ASCENDING,                   /* Ascending order. */
        INS_DESCENDING,                /* Descending order. */
        INS_CUSTOM,                        /* Custom order. */
        INS_CNT                               /* Number of insertion orders. */
};

/* Charset */
enum charset
{
        ISO8859,                        
        GB2312                   
};

/* Program options. */
struct test_options
{
    enum insert_order insert_order;     /* Insertion order. */
    enum charset charset;
    int node_cnt;
    int seed;
};


#endif
