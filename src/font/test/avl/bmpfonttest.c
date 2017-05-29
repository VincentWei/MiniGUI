/*
** $Id$
**
** bitmapfont.c: Test code of bitmap font AVL tree.
**
** Copyright (C) 2003 ~ 2007 Feynman Software
**
** All right reserved by Feynman Software.
**
** Current maintainer: Liu Peng.
**
** Create date: 2007/09/22
*/


#include "bmpfonttest.h"

#define INSERT_COUNT    10

static char *schar_insert []= {"2", "4", "6", "8", "a", 
                               "d", "f", "h", "j", "o"};

static char *mchar_insert []= {"啊", "薄", "病", "场", "础", 
                               "怠", "丁", "贰", "浮", "埂"};

static char *get_mchar (char *str[], int n, int flag)
{
    int i = 0;

    if (n > INSERT_COUNT)
        return NULL;

    if (flag == 0) //ascending
        return str[n];
    else if (flag == 1)         //descending
        return str[INSERT_COUNT-n-1];
    else if (flag == 2) {
        int j = (int)(((float)INSERT_COUNT)*rand()/(RAND_MAX+1.0));
#if 0
        fprintf (stderr, "rand = %d \n", j);
#endif
        return str [j];      
    }
    
}

void gen_insertions (enum insert_order insert_order, enum charset charset,
                     ELEMENT insert[], int n)
{
    int i = 0;

    switch (insert_order) {
        
        case INS_RANDOM:
            for (i = 0; i < n; i++) {
                if (charset == ISO8859)
                    insert[i].start_mchar = get_mchar(schar_insert, i, 2);
                else if (charset == GB2312)
                    insert[i].start_mchar = get_mchar(mchar_insert, i, 2);
                fprintf (stderr, "mchar = %s \n", insert[i].start_mchar);
                insert[i].nr_glyphs = 2;
                insert[i].glyph_width = 6;
                insert[i].data = NULL;
            }

            break;
            
        case INS_ASCENDING:
            for (i = 0; i < n; i++) {
                if (charset == ISO8859)
                    insert[i].start_mchar = get_mchar(schar_insert, i, 0);
                else if (charset == GB2312)
                    insert[i].start_mchar = get_mchar(mchar_insert, i, 0);
                fprintf (stderr, "mchar = %s \n", insert[i].start_mchar);
                insert[i].nr_glyphs = 2;
                insert[i].glyph_width = 6;
                insert[i].data = NULL;                      
            }
            break;

        case INS_DESCENDING:
            for (i = 0; i < n; i++) {
                if (charset == ISO8859)
                    insert[i].start_mchar = get_mchar(schar_insert, i, 1);
                else if (charset == GB2312)
                    insert[i].start_mchar = get_mchar(mchar_insert, i, 1);

                fprintf (stderr, "mchar = %s \n", insert[i].start_mchar);
                insert[i].nr_glyphs = 2;
                insert[i].glyph_width = 6;
                insert[i].data = NULL;                      
            }            
            break;

        case INS_CUSTOM:
            break;

        default:
            ;
            
    }
    
    return;
}

static int verify_tree (DEVFONT *dev_font, ELEMENT array[], int n)
{
    int i = 0;
    int j = 0;
    
    /* check count */
    
    /* check that all the values in array are in tree */
    for (i = 0; i<n; i++) {
        for (j = 0; j<array[i].nr_glyphs; j++)
            if ( avl_look_up (dev_font, array[i].start_mchar, j) == 0) {
                fprintf (stderr,
                         " Tree does not contain expected value %d.\n",
                         array[i]);
                return 0;
            }
    }
    return 1;
}

int test_correctness (const char *font_name, ELEMENT insert[], int n)
{
    int i = 0;
    DEVFONT *dev_font;
   
    dev_font = CreateBMPDevFont (font_name,
                                 NULL, insert[0].start_mchar,
                                 insert[0].nr_glyphs, insert[0].glyph_width);
    if (dev_font == NULL) {
        fprintf (stderr, "create device font failed. \n");
        return 0;
    }

    dbg_dumpDevFonts ();    
    dump_tree (dev_font);

    for (i = 1; i<n; i++) {
        
        AddGlyphsToBMPFont (dev_font, NULL, insert[i].start_mchar,
                            insert[i].nr_glyphs, insert[i].glyph_width);
        
        dump_tree (dev_font);
        
        if (verify_tree (dev_font, insert, i + 1) == 0) {
            fprintf (stderr, "error in verifying tree.\n ");
            break;
        }
    }

    destroy_avl_tree (dev_font);
    dump_tree (dev_font);

    DestroyBMPFont (dev_font);
    dbg_dumpDevFonts ();        
    
    return;
}


