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
** freetype1.c: TrueType font support based on FreeType 1.3.1.
** 
** Create date: 2000/08/21
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "devfont.h"
#include "charset.h"
#include "fontname.h"
#include "misc.h"

#if defined(_MGFONT_TTF) && !defined(_MGFONT_FT2)

#include "freetype1.h"

#if TT_FREETYPE_MAJOR != 1 | TT_FREETYPE_MINOR < 3
    #error "You must link with freetype lib version 1.3.x +, and not freetype 2. \
        You can download it at http://www.freetype.org"
#endif

/******************************* Global data ********************************/
static TT_Engine ttf_engine;        /* The ONLY freetype engine */
static BYTE virtual_palette [] = {0, 32, 64, 128, 255};

/************************ Create/Destroy FreeType font ***********************/
static void* load_font_data (const char* font_name, const char *file_name)
{
    unsigned short      i, n;
    unsigned short      platform, encoding;
    TT_Face_Properties  properties;

    TTFINSTANCEINFO* ttf_inst_info = (TTFINSTANCEINFO*) calloc (1, sizeof(TTFINSTANCEINFO));
    TTFGLYPHINFO* ttf_glyph_info = (TTFGLYPHINFO*) calloc (1, sizeof(TTFGLYPHINFO));

    /* Load face */
    if (TT_Open_Face (ttf_engine, file_name, &ttf_glyph_info->face) != TT_Err_Ok) {
        _MG_PRINTF ("FONT>FT1: TT_Open_Face error.\n");
        goto error_after_create_info;
    }

    /* Load first kerning table */
    ttf_glyph_info->can_kern = TRUE;
    if (TT_Load_Kerning_Table (ttf_glyph_info->face, 0) != TT_Err_Ok)
        ttf_glyph_info->can_kern = FALSE;
    else {
        if (TT_Get_Kerning_Directory (ttf_glyph_info->face, &ttf_glyph_info->directory)
            != TT_Err_Ok)
            ttf_glyph_info->can_kern = FALSE;
        else {
            /* Support only version 0 kerning table ... */
            if ((ttf_glyph_info->directory.version != 0) ||
                (ttf_glyph_info->directory.nTables <= 0) ||
                (ttf_glyph_info->directory.tables->loaded != 1) ||
                (ttf_glyph_info->directory.tables->version != 0) ||
                (ttf_glyph_info->directory.tables->t.kern0.nPairs <= 0))
                    ttf_glyph_info->can_kern = FALSE;
        }
    }

    /* Get face properties and allocate preload arrays */
    TT_Get_Face_Properties (ttf_glyph_info->face, &properties);


    /* Look for a Unicode charmap: Windows flavor of Apple flavor only */
    n = properties.num_CharMaps;

    for (i = 0; i < n; i++) {
        TT_Get_CharMap_ID (ttf_glyph_info->face, i, &platform, &encoding);
        if (((platform == TT_PLATFORM_MICROSOFT) &&
            (encoding == TT_MS_ID_UNICODE_CS)) ||
                ((platform == TT_PLATFORM_APPLE_UNICODE) &&
                     (encoding == TT_APPLE_ID_DEFAULT)))
        {
            TT_Get_CharMap (ttf_glyph_info->face, i, &ttf_glyph_info->char_map);
            i = n + 1;
        }
    }

    if (i == n) {
        _MG_PRINTF ("FONT>FT1: no unicode map table\n");
        goto error_after_create_face;
    }
    
    ttf_glyph_info->first_char = TT_CharMap_First (ttf_glyph_info->char_map, NULL);
    ttf_glyph_info->last_char = TT_CharMap_Last (ttf_glyph_info->char_map, NULL);
    ttf_glyph_info->last_glyph_index
            = (properties.num_Glyphs > 255) ? 255 : properties.num_Glyphs - 1;

    if (TT_New_Glyph (ttf_glyph_info->face, &ttf_glyph_info->glyph) != TT_Err_Ok) {
        _MG_PRINTF ("FONT>FT1: TT_New_Glyph error.\n");
        goto error_after_create_face;
    }
    ttf_inst_info->ttf_glyph_info = ttf_glyph_info;

    return ttf_inst_info;

error_after_create_face:
    TT_Close_Face (ttf_glyph_info->face);
error_after_create_info:
    free (ttf_glyph_info);
    free (ttf_inst_info);
    return NULL;
}

static void unload_font_data (void* data)
{
    TTFINSTANCEINFO* ttf_inst_info = (TTFINSTANCEINFO*)data;
    TTFGLYPHINFO* ttf_glyph_info = ttf_inst_info->ttf_glyph_info;

    TT_Done_Glyph ((ttf_glyph_info)->glyph);
    TT_Close_Face ((ttf_glyph_info)->face);

    free (data);
}

/************************ Alloc/Free raster bitmap buffer ********************/
static BYTE* rb_buffer;
static size_t rb_buf_size;

static BYTE* get_raster_bitmap_buffer (size_t size)
{
    if (size <= rb_buf_size) return rb_buffer;

    rb_buf_size = ((size + 31) >> 5) << 5;
#if 0
    fprintf (stderr, "buf_size: %d.\n", rb_buf_size);
#endif
    rb_buffer = realloc (rb_buffer, rb_buf_size);

    return rb_buffer;
}

static void free_raster_bitmap_buffer (void)
{
    free (rb_buffer);
    rb_buffer = NULL;
    rb_buf_size = 0;
}

/************************ Init/Term of FreeType fonts ************************/
/*************** TrueType on FreeType font operations ************************/
static TT_UShort 
Get_Glyph_Width (TTFINSTANCEINFO* ttf_inst_info, TT_UShort glyph_index)
{
    TT_F26Dot6  xmin, xmax;
    TT_Outline  outline;
    TT_BBox     bbox;
    TTFGLYPHINFO* ttf_glyph_info = ttf_inst_info->ttf_glyph_info;

    if (TT_Load_Glyph (ttf_inst_info->instance, 
            ttf_glyph_info->glyph, glyph_index, 
            TTLOAD_DEFAULT) != TT_Err_Ok) {
        /* Try to load default glyph: index 0 */
        if (TT_Load_Glyph (ttf_inst_info->instance, 
                ttf_glyph_info->glyph, 0, TTLOAD_DEFAULT) != TT_Err_Ok) {
            return 0;
        }
    }

    TT_Get_Glyph_Outline (ttf_glyph_info->glyph, &outline);
    TT_Get_Outline_BBox (&outline, &bbox);

    xmin = (bbox.xMin & -64) >> 6;
    xmax = ((bbox.xMax + 63) & -64) >> 6;

    return (xmax - xmin);
}

static int get_max_width (LOGFONT* logfont, DEVFONT* devfont)
{
    TTFINSTANCEINFO* ttf_inst_info = TTF_INST_INFO_P (devfont);
    return ttf_inst_info->max_width;
}
static int
compute_kernval (TTFINSTANCEINFO* ttf_inst_info)
{
    int i = 0;
    int kernval;
    TTFGLYPHINFO* ttf_glyph_info = ttf_inst_info->ttf_glyph_info;
    int nPairs = ttf_glyph_info->directory.tables->t.kern0.nPairs;
    TT_Kern_0_Pair *pair = ttf_glyph_info->directory.tables->t.kern0.pairs;
    
    if (ttf_inst_info->last_glyph_code != -1) {
        while ((pair->left != ttf_inst_info->last_glyph_code)
               && (pair->right != ttf_inst_info->cur_glyph_code)) {
            
            pair++;
            i++;
            if (i == nPairs)
                break;
        }

        if (i == nPairs)
            kernval = 0;
        else
            /* We round the value (hence the +32) */
            kernval = (pair->value + 32) & -64;
    } else
        kernval = 0;

    return kernval;
}

static DWORD get_glyph_type (LOGFONT* logfont, DEVFONT* devfont)
{
    if (logfont->style & FS_WEIGHT_BOOK)
        return DEVFONTGLYPHTYPE_GREYBMP;

    return DEVFONTGLYPHTYPE_MONOBMP;
}

static int get_ave_width (LOGFONT* logfont, DEVFONT* devfont)
{
    TTFINSTANCEINFO* ttf_inst_info = TTF_INST_INFO_P (devfont);
    return ttf_inst_info->ave_width;
}

static int get_font_height (LOGFONT* logfont, DEVFONT* devfont)
{
    TTFINSTANCEINFO* ttf_inst_info = TTF_INST_INFO_P (devfont);
    return ttf_inst_info->height;
}

static int get_font_size (LOGFONT* logfont, DEVFONT* devfont, int expect)
{
    unsigned short scale = 1;
    SET_DEVFONT_SCALE (logfont, devfont, scale);
    return expect;
}

static int get_font_ascent (LOGFONT* logfont, DEVFONT* devfont)
{
    TTFINSTANCEINFO* ttf_inst_info = TTF_INST_INFO_P (devfont);
    return ttf_inst_info->ascent;
}

static int get_font_descent (LOGFONT* logfont, DEVFONT* devfont)
{
    TTFINSTANCEINFO* ttf_inst_info = TTF_INST_INFO_P (devfont);
    return -ttf_inst_info->descent;
}

/* call this function before output a string */
static void start_str_output (LOGFONT* logfont, DEVFONT* devfont)
{
    TTFINSTANCEINFO* ttf_inst_info = TTF_INST_INFO_P (devfont);

    ttf_inst_info->last_glyph_code = -1;       /* reset kerning*/
    ttf_inst_info->last_pen_pos = -1;
}

/* call this function before getting the bitmap/pixmap of the char
 * to get the bbox of the char */
static int get_glyph_bbox (LOGFONT* logfont, DEVFONT* devfont, 
                const Glyph32 glyph_value, 
                int* px, int* py, int* pwidth, int* pheight)
{
    TT_UShort uni_char;
    TT_F26Dot6 x = 0, y = 0;
    TT_F26Dot6  xmin, ymin, xmax, ymax;
    TT_Pos vec_x, vec_y;
    TT_Glyph_Metrics metrics;
    TT_BBox     bbox;

    TTFINSTANCEINFO* ttf_inst_info = TTF_INST_INFO_P (devfont);
    TTFGLYPHINFO* ttf_glyph_info = ttf_inst_info->ttf_glyph_info;

    if (px) x = *px; 
    if (py) y = *py;
    
    if(devfont->charset_ops->conv_to_uc32)
        uni_char = (*devfont->charset_ops->conv_to_uc32) (glyph_value);
    else
        uni_char = glyph_value;

#ifdef _MGFONT_TTF_CACHE
    ttf_inst_info->cur_unicode = uni_char;
#endif 
    
    /* Search cache by unicode !*/
#ifdef _MGFONT_TTF_CACHE
   //if (px && py && pwidth && pheight && 
   if (ttf_inst_info->cache && (ttf_inst_info->rotation == 0)) {
       TTFCACHEINFO *cache_info;
       int datasize;
       cache_info = __mg_ttc_search(ttf_inst_info->cache,
                                    uni_char, &datasize);
       
       if (cache_info != NULL) {
           TT_F26Dot6 _x, _y, _xmin, _ymin, _xmax, _ymax;
           
           memcpy(&ttf_inst_info->cur_unicode,
                  cache_info, sizeof(TTFCACHEINFO)-sizeof(void*));
           
           DP(("\nBBOX Hit!!\n"));
           if (px) {
               _x = *px; 
               _x -= (ttf_inst_info->cur_vec_x & -64) >> 6;
               if ((logfont->style & FS_OTHER_TTFKERN) && 
                       ttf_glyph_info->can_kern) {
                   _x += compute_kernval (ttf_inst_info) / 64;
               }
           }
           if (py) {
               _y = *py;
               _y += ((ttf_inst_info->cur_vec_y + 63) & -64) >> 6;
           }

           _xmin = (ttf_inst_info->cur_bbox.xMin & -64) >> 6;
           _ymin = (ttf_inst_info->cur_bbox.yMin & -64) >> 6;
           _xmax = ((ttf_inst_info->cur_bbox.xMax + 63) & -64) >> 6;
           _ymax = ((ttf_inst_info->cur_bbox.yMax + 63) & -64) >> 6;
           
           ttf_inst_info->cur_xmin = (ttf_inst_info->cur_bbox.xMin & -64);
           ttf_inst_info->cur_ymin = (ttf_inst_info->cur_bbox.yMin & -64);
           ttf_inst_info->cur_width = _xmax - _xmin;
           ttf_inst_info->cur_height = _ymax - _ymin;
           
           if (pwidth)  *pwidth = (int)(_xmax - _xmin);
           if (pheight) *pheight = (int)(_ymax - _ymin);
           if (px)      *px = (int)(_x + _xmin);
           if (py)      *py = (int)(_y - _ymax);
     
           return (int)(_xmax - _xmin);
       }
       DP(("\nBBOX Non - Hit!\n"));
   }
#endif   
   
   ttf_inst_info->cur_glyph_code 
     = TT_Char_Index (ttf_glyph_info->char_map, uni_char);
   
   vec_x = 0;
#if 0
   vec_y = ttf_inst_info->ascent << 6;
#else
   vec_y = 0;
#endif
    TT_Transform_Vector (&vec_x, &vec_y, &ttf_inst_info->matrix);
    
    x -= (vec_x & -64) >> 6;
    y += ((vec_y + 63) & -64) >> 6;
    
    if (TT_Load_Glyph (ttf_inst_info->instance, ttf_glyph_info->glyph, 
            ttf_inst_info->cur_glyph_code, TTLOAD_DEFAULT) != TT_Err_Ok)
        return 0;

    TT_Get_Glyph_Metrics (ttf_glyph_info->glyph, &metrics);
    ttf_inst_info->cur_advance = metrics.advance;

    TT_Get_Glyph_Outline (ttf_glyph_info->glyph, &ttf_inst_info->cur_outline);
    if (ttf_inst_info->rotation) {
        TT_Transform_Outline (&ttf_inst_info->cur_outline, &ttf_inst_info->matrix);
    }
    
    if ((logfont->style & FS_OTHER_TTFKERN) && ttf_glyph_info->can_kern) {
        if (ttf_inst_info->rotation) {
            vec_x = compute_kernval (ttf_inst_info);
            vec_y = 0;
            TT_Transform_Vector (&vec_x, &vec_y, &ttf_inst_info->matrix);

            x += vec_x / 64;
            y -= vec_y / 64;
        } else
            x += compute_kernval (ttf_inst_info) / 64;
    }
    
    /* we begin by grid-fitting the bounding box */
    TT_Get_Outline_BBox (&ttf_inst_info->cur_outline, &bbox);
    
#ifdef _MGFONT_TTF_CACHE
    /* We just save the BBOX :) */
    memcpy(&ttf_inst_info->cur_bbox, &bbox, sizeof(TT_BBox));
    ttf_inst_info->cur_vec_x = vec_x;
    ttf_inst_info->cur_vec_y = vec_y;
#endif 

    xmin = (bbox.xMin & -64) >> 6;
    ymin = (bbox.yMin & -64) >> 6;
    xmax = ((bbox.xMax + 63) & -64) >> 6;
    ymax = ((bbox.yMax + 63) & -64) >> 6;

    ttf_inst_info->cur_xmin = (bbox.xMin & -64);
    ttf_inst_info->cur_ymin = (bbox.yMin & -64);
    ttf_inst_info->cur_width = xmax - xmin;
    ttf_inst_info->cur_height = ymax - ymin;

    if (pwidth) *pwidth = (int)(xmax - xmin);
    if (pheight) *pheight = (int)(ymax - ymin);

    if (px) *px = (int)(x + xmin);
    if (py) *py = (int)(y - ymax);

    return (int)(xmax - xmin);
}


/* call this function to get the bitmap/pixmap of the char */ 
static const void* 
char_bitmap_pixmap (LOGFONT* logfont, DEVFONT* devfont, 
                const Glyph32 glyph_value, int* pitch, BOOL is_grey) 
{
    TT_Raster_Map Raster;
    TT_Error error;
    TTFINSTANCEINFO* ttf_inst_info = TTF_INST_INFO_P (devfont);

    /* now allocate the raster bitmap */
    Raster.rows = ttf_inst_info->cur_height;
    Raster.width = ttf_inst_info->cur_width;
    DP(("Raster.rows = %d, Raster.width = %d\n", 
        Raster.rows, Raster.width));
    //if (!pitch) {
    if (!is_grey) {
        *pitch = Raster.cols = (Raster.width + 7) >> 3;  /* pad to 8-bits */
    } else {
        *pitch = Raster.cols = (Raster.width + 3) & -4;  /* pad to 32-bits */
    }
    Raster.flow = TT_Flow_Down;
   
    Raster.size = Raster.rows * Raster.cols;
    Raster.bitmap = get_raster_bitmap_buffer (Raster.size);
    memset (Raster.bitmap, 0, Raster.size);
    
    DP(("Raster.size = %d, Raster.rows = %d, Raster.width = %d, Raster.cols = %d\n", 
        Raster.size, Raster.rows, Raster.width, Raster.cols));
    
#ifdef _MGFONT_TTF_CACHE
    if (ttf_inst_info->cache && (ttf_inst_info->rotation == 0)) {
        
        TTFCACHEINFO *cacheinfo;
        int datasize;
    
        cacheinfo = __mg_ttc_search(ttf_inst_info->cache,
                        ttf_inst_info->cur_unicode, &datasize);
        
        if (cacheinfo != NULL) {
                                    
            memcpy(Raster.bitmap, cacheinfo->bitmap, Raster.size);
            
            DP(("Bitmap Hit!! Read Data %d, %d\n", Raster.size, datasize));
            
            ttf_inst_info->last_glyph_code = -1;
            ttf_inst_info->last_pen_pos = -1;
            
            return Raster.bitmap;
        }
        DP(("Bitmap Non hit\n"));
    }
#endif 

    /* now render the glyph in the small bitmap/pixmap */

    /* IMPORTANT NOTE: the offset parameters passed to the function     */
    /* TT_Get_Glyph_Bitmap() must be integer pixel values, i.e.,        */
    /* multiples of 64.  HINTING WILL BE RUINED IF THIS ISN'T THE CASE! */
    /* This is why we _did_ grid-fit the bounding box, especially xmin  */
    /* and ymin.                                                        */
    if (!is_grey) {
        TT_Translate_Outline (&ttf_inst_info->cur_outline, 
                -ttf_inst_info->cur_xmin, -ttf_inst_info->cur_ymin);
        if ((error = TT_Get_Outline_Bitmap (ttf_engine, &ttf_inst_info->cur_outline,
                &Raster)))
            return NULL;
    }
    else {
        TT_Translate_Outline (&ttf_inst_info->cur_outline, 
                -ttf_inst_info->cur_xmin, -ttf_inst_info->cur_ymin);

        if ((error = TT_Get_Outline_Pixmap (ttf_engine, &ttf_inst_info->cur_outline, 
                &Raster)))
            return NULL;
    }

    
#ifdef _MGFONT_TTF_CACHE
    if (ttf_inst_info->cache && (ttf_inst_info->rotation == 0)) {
        TTFCACHEINFO cache_info;
        int ret;
        
        memcpy(&cache_info, &ttf_inst_info->cur_unicode, 
               sizeof(TTFCACHEINFO) - sizeof(void*));
        
        cache_info.bitmap = Raster.bitmap;
        
        DP(("Should Write to cache length =  %d, cache = %p\n", 
            sizeof(TTFCACHEINFO) + Raster.size, 
            ttf_inst_info->cache));
        
        ret = __mg_ttc_write(ttf_inst_info->cache, 
                   &cache_info, sizeof(TTFCACHEINFO) + Raster.size);
        DP(("__mg_ttc_write() return %d\n", ret));
    }
#endif
    
    return Raster.bitmap;
}

static const void* get_glyph_monobitmap (LOGFONT* logfont, DEVFONT* devfont,
            const Glyph32 glyph_value, int* pitch, unsigned short* scale)
{
    if (scale) *scale = 1;
    return char_bitmap_pixmap (logfont, devfont, glyph_value, pitch, FALSE);
}

static const void* get_glyph_greybitmap (LOGFONT* logfont, DEVFONT* devfont,
            const Glyph32 glyph_value, int* pitch, unsigned short* scale)
{
    if (scale) *scale = 1;
    return char_bitmap_pixmap (logfont, devfont, glyph_value, pitch, TRUE);
}

/* call this function after getting the bitmap/pixmap of the char
 * to get the advance of the char */
static int get_glyph_advance (LOGFONT* logfont, DEVFONT* devfont, 
            const Glyph32 glyph_value, int* px, int* py)
{
    TT_Pos vec_x, vec_y;
    TTFINSTANCEINFO* ttf_inst_info = TTF_INST_INFO_P (devfont);
    TTFGLYPHINFO* ttf_glyph_info = TTF_GLYPH_INFO_P (devfont);

    if (ttf_inst_info->rotation) {
        vec_x = ttf_inst_info->cur_advance;
        vec_y = 0;
        TT_Transform_Vector (&vec_x, &vec_y, &ttf_inst_info->matrix);

        *px += vec_x / 64;
        *py -= vec_y / 64;
    } else {
        *px += ttf_inst_info->cur_advance / 64;
        if ((logfont->style & FS_OTHER_TTFKERN) && ttf_glyph_info->can_kern) {
                *px += compute_kernval (ttf_inst_info) / 64;
        }

        ttf_inst_info->last_pen_pos = *px;
    }

    ttf_inst_info->last_glyph_code = ttf_inst_info->cur_glyph_code;

    // bugfixed, houhh 20080222.
    return (int)ttf_inst_info->cur_advance / 64;
}

#ifdef _MGFONT_TTF_CACHE
static int make_hash_key(unsigned short data)
{
    return ((int)data % 37);
}
#endif

static DEVFONT* new_instance (LOGFONT* logfont, DEVFONT* devfont, 
                              BOOL need_sbc_font)
{
    TTFGLYPHINFO* ttf_glyph_info = TTF_GLYPH_INFO_P (devfont);
    TTFINSTANCEINFO* ttf_inst_info = NULL;
    DEVFONT* new_devfont = NULL;
    float angle;
    TT_Face_Properties  properties;
    TT_Instance_Metrics imetrics;
    unsigned short* widths = NULL;

#ifdef _MGFONT_TTF_CACHE
    HCACHE hCache = 0;
#endif
    
    DP(("New a log Font logfont->type = %s, logfont->family = %s, "
        "logfont->charset = %s logfont->style = %d, logfont->size = %d\n", 
        logfont->type, logfont->family, logfont->charset,
        logfont->style, logfont->size));
    
    if ((new_devfont = calloc (1, sizeof (DEVFONT))) == NULL)
        goto out;
    
    if ((ttf_inst_info = calloc (1, sizeof (TTFINSTANCEINFO))) == NULL)
        goto out;
    
    if ((widths = calloc (256, sizeof (unsigned short))) == NULL)
        goto out;

    memcpy (new_devfont, devfont, sizeof (DEVFONT));
    
    if (need_sbc_font && devfont->charset_ops->bytes_maxlen_char > 1) {
        char charset [LEN_FONT_NAME + 1];
        
        fontGetCompatibleCharsetFromName (devfont->name, charset);
        new_devfont->charset_ops = GetCharsetOpsEx (charset);
    }

    new_devfont->data = ttf_inst_info;
    ttf_inst_info->ttf_glyph_info = ttf_glyph_info;
    ttf_inst_info->widths = widths;

    /* Create instance */
    if (TT_New_Instance (ttf_glyph_info->face, &ttf_inst_info->instance) != TT_Err_Ok)
        goto out;

    /* Set the instance resolution */
    if (TT_Set_Instance_Resolutions (ttf_inst_info->instance, 96, 96) != TT_Err_Ok)
        goto out;
    
    /* We want real pixel sizes ... not points ...*/
    TT_Set_Instance_PixelSizes (ttf_inst_info->instance, logfont->size,
                                logfont->size, logfont->size * 64);
    
    /* reset kerning*/
    ttf_inst_info->last_glyph_code = -1;
    ttf_inst_info->last_pen_pos = -1;

    /* Font rotation */
    ttf_inst_info->rotation = logfont->rotation; /* in tenthdegrees */

    /* Build the rotation matrix with the given angle */
    TT_Set_Instance_Transform_Flags (ttf_inst_info->instance, TRUE, FALSE);

    angle = ttf_inst_info->rotation * M_PI / 1800;
    ttf_inst_info->matrix.yy = (TT_Fixed) (cos (angle) * (1 << 16));
    ttf_inst_info->matrix.yx = (TT_Fixed) (sin (angle) * (1 << 16));
    ttf_inst_info->matrix.xx = ttf_inst_info->matrix.yy;
    ttf_inst_info->matrix.xy = -ttf_inst_info->matrix.yx;
    
    /* Fill up the info fields */
    TT_Get_Face_Properties (ttf_glyph_info->face, &properties);
    TT_Get_Instance_Metrics(ttf_inst_info->instance, &imetrics);

    ttf_inst_info->max_width = ((properties.horizontal->xMax_Extent * \
                            imetrics.x_scale)/0x10000) >> 6;
    

    ttf_inst_info->ascent = (((properties.horizontal->Ascender * \
                            imetrics.y_scale)/0x10000) >> 6);
    ttf_inst_info->descent = (((properties.horizontal->Descender * \
                            imetrics.y_scale)/0x10000) >> 6);

    ttf_inst_info->height = ttf_inst_info->ascent - ttf_inst_info->descent;

    ttf_inst_info->ave_width = Get_Glyph_Width (ttf_inst_info, 0);

#ifdef _MGFONT_TTF_CACHE
    /* if unmask non-cache and no rotation */
    if (!(logfont->style & FS_OTHER_TTFNOCACHE) && 
        (ttf_inst_info->rotation == 0)) {
        
        hCache = __mg_ttc_is_exist(logfont->family, logfont->charset, 
                                   logfont->style, logfont->size);
        DP(("__mg_ttc_is_exist() return %p\n", hCache));
        /* No this style's cache */
        if (hCache == 0) {
            int pitch = 0, nblk, col, blksize, rows = ttf_inst_info->height;
                     
            if (((logfont->style & 0x0000000F) == FS_WEIGHT_BOOK) || 
                ((logfont->style & 0x0000000F) == FS_WEIGHT_DEMIBOLD)) {
                pitch = 1;
            }
            
            if (!pitch) {
                col = (ttf_inst_info->max_width + 7) >> 3;
            } else {
                col = (ttf_inst_info->max_width + 3) & -4;
            }
            
            blksize = col * rows;
            blksize += sizeof(TTFCACHEINFO);
            
            DP(("BITMAP Space = %d, Whole Space = %d\n", 
                blksize - sizeof(TTFCACHEINFO), blksize));
            
            blksize = (blksize + 3) & -4;
            nblk = ( _MGTTF_CACHE_SIZE * 1024 )/ blksize;
            DP(("[Before New a Cache], col = %d, row = %d, blksize = %d, "
                "blksize(bitmap) = %d, nblk = %d\n", 
                rows, col, blksize, blksize-sizeof(TTFCACHEINFO), nblk));
            
            ttf_inst_info->cache =  __mg_ttc_create(logfont->family, logfont->charset, 
                                        logfont->style, logfont->size, nblk , blksize , 
                                        _TTF_HASH_NDIR, make_hash_key);
            DP(("__mg_ttc_create() return %p\n", ttf_inst_info->cache));
        } else {
            ttf_inst_info->cache = hCache;
            __mg_ttc_refer(hCache);
        }
    } else {
        ttf_inst_info->cache = 0;
    }
#endif

    return new_devfont;

out:
    free (widths);
    free (ttf_inst_info);
    free (new_devfont);
    return NULL;
}

static void delete_instance (DEVFONT* devfont)
{
    TTFINSTANCEINFO* ttf_inst_info = TTF_INST_INFO_P (devfont);

#ifdef _MGFONT_TTF_CACHE
    if (ttf_inst_info->cache) {
        __mg_ttc_release(ttf_inst_info->cache);
    }
#endif 
    TT_Done_Instance (ttf_inst_info->instance);
    free (ttf_inst_info->widths);
    free (ttf_inst_info);
    free (devfont);
}

static BOOL is_glyph_existed (LOGFONT* logfont, DEVFONT* devfont, Glyph32 glyph_value)
{
    TT_UShort uni_char;
    TTFINSTANCEINFO* ttf_inst_info = TTF_INST_INFO_P (devfont);
    TTFGLYPHINFO* ttf_glyph_info = ttf_inst_info->ttf_glyph_info;
    
    if(devfont->charset_ops->conv_to_uc32)
        uni_char = (*devfont->charset_ops->conv_to_uc32) (glyph_value);
    else
        uni_char = glyph_value;
    
     if(0 == TT_Char_Index (ttf_glyph_info->char_map, uni_char))
         return FALSE;
     else
         return TRUE;
}

static int is_rotatable (LOGFONT* logfont, DEVFONT* devfont, int rot_desired)
{
    return rot_desired;
}
/**************************** Global data ************************************/
FONTOPS __mg_ttf_ops = {
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

    get_glyph_monobitmap,
    get_glyph_greybitmap,
    NULL,

    start_str_output,
    new_instance,
    delete_instance,
    is_rotatable,
    load_font_data,
    unload_font_data
};

BOOL font_InitFreetypeLibrary (void)
{
    /* Init freetype library */
    if (TT_Init_FreeType (&ttf_engine) != TT_Err_Ok) {
        return FALSE;
    }
 
    TT_Set_Raster_Gray_Palette (ttf_engine, virtual_palette);

    /* Init kerning extension */
    if (TT_Init_Kerning_Extension (ttf_engine) != TT_Err_Ok) {
        goto error_library;
    }

#ifdef _MGFONT_TTF_CACHE
    if (__mg_ttc_sys_init(_MGMAX_TTF_CACHE, _MGTTF_CACHE_SIZE * 1024)) {
        _MG_PRINTF ("FONT>FT1: init ttf cache sys failed\n");
        goto error_library;
    }
#endif
    
    return TRUE;

error_library:
    fprintf (stderr, "FONT>FT1: Could not initialise FreeType library\n");
    TT_Done_FreeType (ttf_engine);
    return FALSE;
}


void font_TermFreetypeLibrary (void)
{
    TT_Done_FreeType (ttf_engine);
    free_raster_bitmap_buffer ();
#ifdef _MGFONT_TTF_CACHE
    __mg_ttc_sys_deinit();
#endif
}

int 
ftIsFreeTypeDevfont (DEVFONT* devfont)
{
    if (devfont && devfont->font_ops == &__mg_ttf_ops)
        return TRUE;

    return FALSE;
}
#endif /* _MGFONT_TTF */

