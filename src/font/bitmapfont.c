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
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */
/*
** bitmapfont.c: The Bitmap Font operation set.
**
** Create date: 2007/09/22
*/

#include "common.h"

#ifdef _MGFONT_BMPF

#include "minigui.h"
#include "gdi.h"
#include "cliprect.h"
#include "gal.h"
#include "window.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "readbmp.h"

#include "devfont.h"
#include "charset.h"
#include "fontname.h"

#include "bitmapfont.h"

#if 0
/* default bitmap glyph */
static unsigned char def_bitmap [] = {0xFE, 0x7F};
static int def_glyph_height = 2;
static int def_glyph_width = 8;
#endif

/* default bitmap glyph */
static unsigned char def_glyph [] = {0};
#define DEFGLYPHWIDTH    8;

/*** tree ops ***/

/*
* rebalance the tree. 
*/
static int rebalance (GLYPHTREENODE **root, int *l, int *m)
{
    if ( *root ) {
        int ll, lm, ml, mm;
        *l = rebalance (&(*root)->left, &ll, &lm);
        *m = rebalance (&(*root)->right, &ml, &mm);
        
        if ( (*root)->right ) {
            if ( *l + ml + 1 < mm ) {
                /* Shift less-ward */
                GLYPHTREENODE* b = *root;
                GLYPHTREENODE* c = (*root)->right;
                *root = c;
                b->right = c->left;
                c->left = b;
            }
        }

        if ( (*root)->left ) {
            if ( *m + lm + 1 < ll ) {
                /* Shift more-ward */
                GLYPHTREENODE* c = *root;
                GLYPHTREENODE* b = (*root)->left;
                *root = b;
                c->left = b->right;
                b->right = c;
            }
        }
        
        return 1 + *l + *m;
    } 
    else {
        *l = *m = 0;
        return 0;
    }    
}

/*
* insert a node
*/
static void insert (GLYPHTREENODE *root, int min, int max,
                    int glyph_width, BITMAP *bmp)
{
    
    if (max < root->min) {
        if (!root->left) {
            root->left = (GLYPHTREENODE *) calloc (sizeof (GLYPHTREENODE), 1);
            root->left->min = min;  
            root->left->max = max;
        }
        
        insert (root->left, min, max, glyph_width, bmp);
        return;
    }
    else if (min > root->max) {
        if (!root->right) {
            root->right = (GLYPHTREENODE *) calloc (sizeof (GLYPHTREENODE), 
                                1);
            root->right->min = min; 
            root->right->max = max;
        }

        insert (root->right, min, max, glyph_width, bmp);
        return;
    }
    else if (min == root->min &&
             max == root->max &&
             root->nr_glyphs == 0) {
        root->data = bmp;
        root->nr_glyphs = max - min + 1; 
        root->glyph_width = glyph_width;
    }
    
    return;
}

/* Search in the glyph segment tree and return the pointer to tree node */
static GLYPHTREENODE *look_up (GLYPHTREENODE *root, int offset)
{
  GLYPHTREENODE *p;
  
  for (p = root; p; )
    {
      if (offset < p->min)
        p = p->left;
      else if (offset > p->max)
        p = p->right;
      else  /* found */
        return p;
    }
    
    return NULL;
}


/* Destroy segment glyph segment tree */
static void destroy (GLYPHTREENODE *root)
{
    GLYPHTREENODE *p, *q;

    for (p = root; p; p = q)
        if (p->left == NULL) {
            q = p->right;
            free (p);
        }
        else
        {
            q = p->left;
            p->left = q->right;
            q->right = p;
        }
  
    return;
}

/*** device font ops ***/

static DWORD get_glyph_type (LOGFONT* logfont, DEVFONT* devfont)
{
    return DEVFONTGLYPHTYPE_PRERDRBMP;
}

static int get_ave_width (LOGFONT* logfont, DEVFONT* devfont)
{
    return ((BMPFONTINFO *)devfont->data)->ave_width;
}

static int get_max_width (LOGFONT* logfont, DEVFONT* devfont)
{
    return ((BMPFONTINFO *)devfont->data)->max_width;
}

static int get_font_height (LOGFONT* logfont, DEVFONT* devfont)
{
    return ((BMPFONTINFO *)devfont->data)->height;
}

static int get_font_size (LOGFONT* logfont, DEVFONT* devfont, int expect)
{
    return ((BMPFONTINFO *)devfont->data)->height;
}

static int get_font_ascent (LOGFONT* logfont, DEVFONT* devfont)
{
    int height = ((BMPFONTINFO *)devfont->data)->height;

    if (height >= 40)
        return height - 6;
    else if (height >= 20)
        return height - 3;
    else if (height >= 15)
        return height - 2;
    else if (height >= 10)
        return height - 1;

    return height;    
}

static int get_font_descent (LOGFONT* logfont, DEVFONT* devfont)
{
    int height = ((BMPFONTINFO *)devfont->data)->height;    

    if (height >= 40)
        return 6;
    else if (height >= 20)
        return 3;
    else if (height >= 15)
        return 2;
    else if (height >= 10)
        return 1;

    return 0;    
}

static int
get_glyph_prbitmap (LOGFONT* logfont, DEVFONT* devfont, 
                    Glyph32 glyph_value, BITMAP *bmp)
{
    GLYPHTREENODE *node = NULL;
    BMPFONTINFO *bmpfont_info = (BMPFONTINFO *)devfont->data;
    GLYPHTREENODE *root = bmpfont_info->tree;
    CHARSETOPS* charset_ops = devfont->charset_ops;

    if (charset_ops == NULL)
        return 0;

    node = look_up (root, glyph_value);
    if (node == NULL) {
        bmp->bmType = BMP_TYPE_NORMAL;
        bmp->bmColorKey = 0;
        bmp->bmAlpha = 0;
        bmp->bmAlphaMask = NULL;
        bmp->bmAlphaPitch = 0;
        bmp->bmBits   = def_glyph;
        bmp->bmWidth  = 0;
        bmp->bmHeight = 0;
        bmp->bmPitch  = 0;

        _MG_PRINTF ("FONT>Bitmap: "
            "Glyph %d does not exist and is replaced with default glyph. \n",
            glyph_value);
        return 1;
    }

    bmp->bmType = node->data->bmType;
    bmp->bmBitsPerPixel = node->data->bmBitsPerPixel;
    bmp->bmBytesPerPixel = node->data->bmBytesPerPixel;
    bmp->bmColorKey = node->data->bmColorKey;
    bmp->bmWidth = node->glyph_width;
    bmp->bmHeight = bmpfont_info->height;
    bmp->bmPitch = node->data->bmPitch;   
    bmp->bmBits = node->data->bmBits;
    bmp->bmBits += node->glyph_width * (glyph_value - node->min) * 
                    node->data->bmBytesPerPixel;

    if(node->data->bmAlphaMask) {
        bmp->bmAlphaMask  = node->data->bmAlphaMask + node->glyph_width * (glyph_value - node->min);
        bmp->bmAlphaPitch = node->data->bmAlphaPitch;
    }

    return 1;
}

static BOOL is_glyph_existed (LOGFONT* logfont, DEVFONT* devfont, Glyph32 glyph_value)
{
    GLYPHTREENODE *node = NULL;
    BMPFONTINFO *bmpfont_info = (BMPFONTINFO *)devfont->data;
    GLYPHTREENODE *root = bmpfont_info->tree;
    CHARSETOPS* charset_ops = devfont->charset_ops;

    if (charset_ops == NULL)
        return FALSE;

    node = look_up (root, glyph_value);
    if (node == NULL) 
        return FALSE;
    else
        return TRUE;
    
}

static int get_glyph_advance (LOGFONT* logfont, DEVFONT* devfont, 
                Glyph32 glyph_value, int* px, int* py)
{
    GLYPHTREENODE *node = NULL;
    BMPFONTINFO *bmpfont_info = (BMPFONTINFO *)devfont->data;
    GLYPHTREENODE *root = bmpfont_info->tree;
    int advance;

    node = look_up (root, glyph_value);
    if (node == NULL) {
        advance = DEFGLYPHWIDTH;
        *px += advance;
    }
    else {
        advance = node->glyph_width;
        *px += advance;
    }

    return advance;
}

static int get_glyph_bbox (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 glyph_value, int* px, int* py, int* pwidth, int* pheight)
{
    GLYPHTREENODE *node = NULL;
    BMPFONTINFO *bmpfont_info = (BMPFONTINFO *)devfont->data;
    GLYPHTREENODE *root = bmpfont_info->tree;

    if (py)
        *py -= get_font_ascent (logfont, devfont);
 
    node = look_up (root, glyph_value);
    if (node == NULL) {
        if (pwidth)
            *pwidth  = DEFGLYPHWIDTH;
        if (pheight)
            *pheight = get_font_height (logfont, devfont);

        return DEFGLYPHWIDTH;
    }

    if (pwidth)
        *pwidth  = node->glyph_width;
    if (pheight)
        *pheight = bmpfont_info->height;

    return node->glyph_width;
}

/**************************** Global data ************************************/
FONTOPS __mg_bitmap_font_ops = {
    get_glyph_type,
    get_ave_width,
    get_max_width,
    get_font_height,
    get_font_size,
    get_font_ascent,
    get_font_descent,

    is_glyph_existed,
    get_glyph_advance,
    get_glyph_bbox,

    NULL,
    NULL,
    get_glyph_prbitmap,

    NULL,
    NULL,
    NULL,
};

static BMPFONTINFO *
build_bmpfont_info (const char *bmpfont_name, const BITMAP* glyph_bmp,
                  int offset, int nr_glyphs, int glyph_width)
{
    
    BMPFONTINFO *bmpfont_info = (BMPFONTINFO *)calloc (1, 
                                sizeof(BMPFONTINFO));
    if (!bmpfont_info)
        return NULL;

    /* set name */
    strncpy (bmpfont_info->name , bmpfont_name, LEN_UNIDEVFONT_NAME);
    bmpfont_info->name [LEN_UNIDEVFONT_NAME] = '\0';

    /* set height */
    if ((bmpfont_info->height 
         = fontGetHeightFromName (bmpfont_name)) == -1) {
        _ERR_PRINTF ("FONT>Bitmap: Invalid font name (height): %s.\n",
                 bmpfont_name);
        goto error_load;
    }


    bmpfont_info->ave_width = glyph_width;
    bmpfont_info->max_width = glyph_width;
    bmpfont_info->node_cnt = 1;

    /* set tree root */
    if (!bmpfont_info->tree) {
        bmpfont_info->tree = (GLYPHTREENODE *) calloc (1, 
                                sizeof (GLYPHTREENODE));
        if (!bmpfont_info->tree) {
            _ERR_PRINTF ("FONT>Bitmap: fail to calloc memory space for root.\n");
            goto error_load;
        }        
    }

    bmpfont_info->tree->min = offset;
    bmpfont_info->tree->max = offset + nr_glyphs - 1;
    bmpfont_info->tree->nr_glyphs = nr_glyphs;
    bmpfont_info->tree->glyph_width = glyph_width;
    bmpfont_info->tree->data = (void *)glyph_bmp;    
   
 
    return bmpfont_info;
    
error_load:
    _ERR_PRINTF ("FONT>Bitmap: Error in building Bitmap Font Info!\n");
    
    free (bmpfont_info);
    bmpfont_info = NULL;
    
    return NULL;
}

static int set_devfont_style (char* name, char style)
{
    int i;
    char* style_part = name;

    for (i = 0; i < NR_LOOP_FOR_STYLE; i++) {
        if ((style_part = strchr (style_part, '-')) == NULL)
            return 0;

        if (*(++style_part) == '\0')
            return 0;
    }

    if (style == FONT_WEIGHT_BOLD)
        *style_part = FONT_WEIGHT_BOLD;
    else if (style == FONT_SLANT_ITALIC)
        *(style_part + 1) = FONT_SLANT_ITALIC;

    return 1;
}


/************************/
DEVFONT *CreateBMPDevFont (const char *bmpfont_name, const BITMAP* glyph_bmp,
        const char* start_mchar, int nr_glyphs, int glyph_width)
{
    DEVFONT* bitmapfont_dev_font = NULL;
    CHARSETOPS* charset_ops;
    char charset[LEN_FONT_NAME + 1];
    BMPFONTINFO *bmpfont_info;
    int offset;

    if (!fontGetCharsetFromName (bmpfont_name, charset)) {
        _ERR_PRINTF ("FONT>Bitmap: Invalid font name (charset): %s.\n", 
                 bmpfont_name);
        goto error_create;
    }    

    if ((charset_ops = GetCharsetOpsEx (charset)) == NULL) {
        _ERR_PRINTF ("FONT>Bitmap: Not supported charset: %s.\n", charset);
        goto error_create;
    }

    offset = (charset_ops->char_glyph_value) (NULL, 0,
                (const unsigned char*) start_mchar, 0);      
    
    bmpfont_info = build_bmpfont_info (bmpfont_name, glyph_bmp,
                                       offset, nr_glyphs,
                                       glyph_width);
    if (bmpfont_info == NULL)
        return NULL;

    bitmapfont_dev_font = (DEVFONT *)calloc (1, sizeof (DEVFONT));    
    if (bitmapfont_dev_font == NULL) {
        free(bmpfont_info);
        free (bitmapfont_dev_font);
        return NULL;
    }

/*
 * Set device name and add BOLD & ITALIC style 
 */
    strncpy (bitmapfont_dev_font->name ,bmpfont_name, LEN_UNIDEVFONT_NAME);

    if (!set_devfont_style (bitmapfont_dev_font->name, FONT_WEIGHT_BOLD))
        return NULL; 
    if (!set_devfont_style (bitmapfont_dev_font->name, FONT_SLANT_ITALIC))
        return NULL; 

    bitmapfont_dev_font->name [LEN_UNIDEVFONT_NAME] = '\0';
    bitmapfont_dev_font->style = fontConvertStyle (bitmapfont_dev_font->name);
    bitmapfont_dev_font->font_ops = &__mg_bitmap_font_ops;
    bitmapfont_dev_font->charset_ops = charset_ops;
    bitmapfont_dev_font->data = bmpfont_info;


    if (bitmapfont_dev_font->charset_ops->bytes_maxlen_char > 1) 
        font_AddMBDevFont (bitmapfont_dev_font);
    else 
        font_AddSBDevFont (bitmapfont_dev_font);
             

    return bitmapfont_dev_font;
    
error_create:
    _ERR_PRINTF ("FONT>Bitmap: Error in creating bitmap font!\n");    
    free (bitmapfont_dev_font);
    bitmapfont_dev_font = NULL;
    return NULL;    
}

BOOL AddGlyphsToBMPFont (DEVFONT* dev_font, BITMAP* glyph_bmp, 
        const char* start_mchar, int nr_glyphs, int glyph_width)
{
    int offset;
    int l = 0;
    int m = 0;
    BMPFONTINFO *info = (BMPFONTINFO *)dev_font->data;
    GLYPHTREENODE *root = info->tree;
  
    if (!dev_font || !root)
        return FALSE;

    /* Insert a node in the avl tree */
    offset = (*dev_font->charset_ops->char_glyph_value) (NULL, 0,
                (const unsigned char*)start_mchar, 0);

    if (look_up (root, offset) == NULL) {
        insert (root, offset, (offset + nr_glyphs - 1 ),
                glyph_width, glyph_bmp);
        
        /* rebalance tree. 'root' can't be carried into rebalance */
        rebalance (&(((BMPFONTINFO *)dev_font->data)->tree), &l, &m);

        /* Update max_width, average_width */
        if (glyph_width > info->max_width)
            info->max_width = glyph_width;

        info->ave_width = (info->ave_width * info->node_cnt + glyph_width)/
                        (info->node_cnt + 1);
        info->node_cnt++;
    }    
    
    return TRUE;
}

void DestroyBMPFont (DEVFONT* dev_font)
{
    if (dev_font == NULL)
        return;

    /* destroy tree */
    if(((BMPFONTINFO *)dev_font->data)->tree != NULL) {
        destroy (((BMPFONTINFO *)dev_font->data)->tree);
        ((BMPFONTINFO *)dev_font->data)->tree = NULL;
    }
    
    /* free bitmap font info*/
    if (dev_font->data != NULL) {
        free(dev_font->data);
        dev_font->data = NULL;
    }

    /*delete dev_font from device font linked list and free dev_fonts*/
    if (dev_font->charset_ops->bytes_maxlen_char > 1) 
        font_DelMBDevFont (dev_font);
    else 
        font_DelSBDevFont (dev_font);
    
    dev_font = NULL;
    
    return;
}

#ifdef _DEBUG

static int
print_tree_struct ( GLYPHTREENODE *node)
{

    if (node == NULL)
        return 0;
    
    _MG_PRINTF ("%d", node->min);
    if (node->left != NULL || node->right != NULL)
    {
        _MG_PRINTF ("(");
        
        print_tree_struct (node->left);
        if (node->right != NULL)
        {
            _MG_PRINTF (",");
            print_tree_struct (node->right);
        }
        
        _MG_PRINTF (")");
    }
    return 0;
}


void dump_tree (DEVFONT *dev_font)
{
    if (dev_font == NULL ||
        dev_font->data == NULL ||
        ((BMPFONTINFO *)dev_font->data)->tree == NULL) {

        _MG_PRINTF ("the tree is empty.\n");
        return;
    }
    
    print_tree_struct (((BMPFONTINFO *)dev_font->data)->tree);
    _MG_PRINTF ("\n");
}

int avl_look_up (DEVFONT *dev_font, char *start_mchar, int n)
{
    GLYPHTREENODE *p;
    int offset;

    offset = (*dev_font->charset_ops->char_glyph_value) (NULL, 0,
                (const unsigned char*)start_mchar, n);
    offset += n;
    
    p = look_up (((BMPFONTINFO *)dev_font->data)->tree, offset);
    if (p == NULL) {
        _MG_PRINTF ("Find nothing. \n");
        return 0;
    }
    else {
        _MG_PRINTF ("Found %s+%d \n", start_mchar, n);
        return 1;
        
    }
    
    return 0;
}

void destroy_avl_tree (DEVFONT *dev_font)
{
    if (dev_font && ((BMPFONTINFO *)dev_font->data)->tree != NULL) {
        
        destroy (((BMPFONTINFO *)dev_font->data)->tree);
        ((BMPFONTINFO *)dev_font->data)->tree = NULL;
        
        _MG_PRINTF ("Succeed to destroy avl tree.\n");
    }
}

void draw_root_bmp (HDC hdc, DEVFONT *dev_font)
{
    GLYPHTREENODE *root = ((BMPFONTINFO *)dev_font->data)->tree;

    _MG_PRINTF ("draw_root_bmp is run. \n");
    if (root->data == NULL)
        _MG_PRINTF ("bmp data is NULL. \n");

    FillBoxWithBitmap (hdc, 0, 400, 0, 0, root->data);
}

#endif  /* End of _DEBUG */

#endif  /* End of _MGFONT_BMPF */

