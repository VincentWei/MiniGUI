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
** freetype2.c: TrueType font support based on FreeType 2.
** 
** Create date: 2002/01/18
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

#ifdef _MGFONT_FT2

#include "freetype2.h"

#define IS_SUBPIXEL(logfont) \
   (((logfont)->style & FS_WEIGHT_MASK) == FS_WEIGHT_SUBPIXEL) 

#ifdef TTF_DBG
static int bbox_nohit = 0;
static int bbox_hit = 0;
static int bitmap_nohit = 0;
static int bitmap_hit = 0;
#endif

typedef struct _FT2_DATA{
    FTFACEINFO*     ft_face_info;
    FTINSTANCEINFO* ft_inst_info;
}FT2_DATA;

#define FT_FACE_INFO_P(devfont) ((FTFACEINFO*)(((FT2_DATA*)(devfont->data))->ft_face_info));
#define FT_INST_INFO_P(devfont) ((FTINSTANCEINFO*)(((FT2_DATA*)(devfont->data))->ft_inst_info));

static FT_Library       ft_library = NULL;
#ifdef _MGFONT_TTF_CACHE
static FTC_Manager      ft_cache_manager = NULL;
static FTC_CMapCache    ft_cmap_cache = NULL;
static FTC_ImageCache   ft_image_cache = NULL;

#if 0
static FTC_SBitCache    ft_sbit_cache = NULL;
#endif

/*find cached face, by FTC_Manager_LookupSize.
 * The size setting is image_type,
 * when getting glyph by FTC_Manager_LookupSize*/
static FT_Error
get_cached_face (FTINSTANCEINFO* ft_inst_info,
               FT_Face* face)
{
    FTC_ScalerRec   scaler;
    FT_Size         size;
    FT_Error        error;

    scaler.face_id = (FTC_FaceID)(ft_inst_info->ft_face_info);
    scaler.width   = 1;
    scaler.height  = 1;
    scaler.pixel   = 1;

    error = FTC_Manager_LookupSize( ft_cache_manager, &scaler, &size );

    if ( !error )
        *face= size->face;

    return error;
}

static FT_Error
my_face_requester (FTC_FaceID  face_id,
                   FT_Library  lib,
                   FT_Pointer  request_data,
                   FT_Face*    aface)
{
    PFTFACEINFO font = (PFTFACEINFO) face_id;

    FT_Error    error = FT_New_Face (lib,
                         font->filepathname,
                         font->face_index,
                         aface);
    if (!error)
    {
        if ((*aface)->charmaps)
            (*aface)->charmap = (*aface)->charmaps[font->cmap_index];
    }

    return error;
}
#endif
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

/************************ Alloc/Free raster bitmap buffer ********************/
static BYTE* rb_buffer;
static size_t rb_buf_size;

static BYTE* get_raster_bitmap_buffer (size_t size)
{
    if (size <= rb_buf_size) return rb_buffer;
    rb_buf_size = ((size + 31) >> 5) << 5;
    rb_buffer = realloc (rb_buffer, rb_buf_size);
    memset(rb_buffer, 0, rb_buf_size);
    return rb_buffer;
}

static void free_raster_bitmap_buffer (void)
{
    if (rb_buffer)
        free (rb_buffer);
    rb_buffer = NULL;
    rb_buf_size = 0;
}

/*************** TrueType on FreeType font operations ************************/

#ifdef DEBUG_GLYPH_DATA
static void 
print_bitmap(char* bits, int width, int height, int pitch)
{
    int y = 0;
    int x = 0;
    char* p_line_head;
    char* p_cur_char;
    
    for (y = 0, p_line_head = bits; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            p_cur_char = (x >> 3) + p_line_head;
            if (*p_cur_char & (128 >> (x%8)))
                printf("@ ");
            else
                printf(". ");
        }
        printf("\n");
        
        p_line_head += pitch;
    }
}

static void 
print_bitmap_grey (void* buffer, int width, int rows, int pitch)
{
    int     i;
    int     j;
    BYTE*   p = (BYTE*)buffer;

    printf("*******************************************\n");
    for (i = 0; i < rows; i++) {
        for (j = 0; j < pitch; j++) {
            printf("%02x, ", *p++);
        }
        printf("\n");
    }
    printf("*******************************************\n");
}
#endif

static DWORD get_glyph_type (LOGFONT* logfont, DEVFONT* devfont)
{
    if (logfont->style & FS_WEIGHT_SUBPIXEL)
        return DEVFONTGLYPHTYPE_SUBPIXEL;
    else if (logfont->style & FS_WEIGHT_BOOK)
        return DEVFONTGLYPHTYPE_GREYBMP;

    return DEVFONTGLYPHTYPE_MONOBMP;
}

static int 
get_ave_width (LOGFONT* logfont, DEVFONT* devfont)
{
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);
    return ft_inst_info->ave_width;
}

static int 
get_max_width (LOGFONT* logfont, DEVFONT* devfont)
{
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);
    return ft_inst_info->max_width;
}

static int 
get_font_height (LOGFONT* logfont, DEVFONT* devfont)
{
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);
    return ft_inst_info->height;
}

static int 
get_font_size (LOGFONT* logfont, DEVFONT* devfont, int expect)
{
    unsigned short scale = 1;
    SET_DEVFONT_SCALE (logfont, devfont, scale);
    return expect;
}

static int 
get_font_ascent (LOGFONT* logfont, DEVFONT* devfont)
{
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);
    return ft_inst_info->ascent;
}

static int 
get_font_descent (LOGFONT* logfont, DEVFONT* devfont)
{
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);
    return ft_inst_info->descent;
}

static int 
load_or_search_glyph (FTINSTANCEINFO* ft_inst_info, FT_Face* face, 
        FT_ULong uni_char, int glyph_type)
{
#ifndef _MGFONT_TTF_CACHE
    FTFACEINFO*     ft_face_info = ft_inst_info->ft_face_info;
    int ft_load_flags = FT_LOAD_NO_BITMAP | FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH;

    //setting load_flags
    if (glyph_type == DEVFONTGLYPHTYPE_MONOBMP)
        ft_load_flags |= FT_LOAD_TARGET_MONO;
    else {
        if (glyph_type == DEVFONTGLYPHTYPE_SUBPIXEL
                && ft_inst_info->ft_lcdfilter != FT_LCD_FILTER_NONE)
            ft_load_flags |= FT_LOAD_TARGET_LCD;
        else
            ft_load_flags |= FT_LOAD_TARGET_NORMAL | FT_LOAD_FORCE_AUTOHINT;
    }


    FT_Activate_Size (ft_inst_info->size);

    *face = ft_face_info->face;
    ft_inst_info->cur_index = FT_Get_Char_Index (*face, uni_char);

    if (FT_Load_Glyph (*face, ft_inst_info->cur_index, ft_load_flags)) {
        _MG_PRINTF ("FONT>FT2: FT_Load_Glyph error\n");
        return -1;
    }

    if (FT_Get_Glyph ((*face)->glyph, &(ft_inst_info->glyph))) {
        _MG_PRINTF ("FONT>FT2: FT_Get_Glyph error\n");
        return -1;
    }

#else

    FT_Glyph ft_glyph_tmp;

    if (get_cached_face (ft_inst_info, face)) {
        _MG_PRINTF ("FONT>FT2: can't access font file %p\n", face);
        return 0;
    }

    ft_inst_info->cur_index = FTC_CMapCache_Lookup (ft_cmap_cache,
            (FTC_FaceID) (ft_inst_info->image_type.face_id),
            ft_inst_info->ft_face_info->cmap_index,
            uni_char);

    if (FTC_ImageCache_Lookup (ft_image_cache,
                &ft_inst_info->image_type,
                ft_inst_info->cur_index,
                &ft_glyph_tmp,
                NULL)) {
        _MG_PRINTF ("FONT>FT2: can't access image cache.\n");
        return -1;
    }

    if (FT_Glyph_Copy (ft_glyph_tmp, &ft_inst_info->glyph)) {
        _MG_PRINTF ("FONT>FT2: can't copy glyph from cache.\n");
        return -1;
    }
#endif

    if (ft_inst_info->rotation)
        FT_Glyph_Transform (ft_inst_info->glyph, &ft_inst_info->matrix, NULL);

    return 0;
}

/* call this function before getting the bitmap/pixmap of the char
 * to get the bbox of the char */
static int 
get_glyph_bbox (LOGFONT* logfont, DEVFONT* devfont, 
                const Glyph32 glyph_value, 
                int* px, int* py, int* pwidth, int* pheight)
{
   
    FT_BBox         bbox;
    FT_Face         face;
    FT_UInt         uni_char;
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);

    bbox.xMin = bbox.yMin = 32000;
    bbox.xMax = bbox.yMax = -32000;

    if(devfont->charset_ops->conv_to_uc32)
        uni_char = (*devfont->charset_ops->conv_to_uc32) (glyph_value);
    else
        uni_char = glyph_value;

    /* Search cache by unicode !*/
#ifdef _MGFONT_TTF_CACHE
    if (ft_inst_info->cache && (ft_inst_info->rotation == 0)) {
        TTFCACHEINFO *cache_info;
        int datasize;
        cache_info = __mg_ttc_search(ft_inst_info->cache,
                                        uni_char, &datasize);

        if (cache_info != NULL) {
            DP(("\nBBOX Hit!! %d\n", bbox_hit++));
            ft_inst_info->bbox    = cache_info->bbox;
            ft_inst_info->advance = cache_info->advance;

            if (pwidth)  *pwidth  = cache_info->bbox.xMax - cache_info->bbox.xMin;
            if (pheight) *pheight = cache_info->bbox.yMax - cache_info->bbox.yMin;
            if (px)      *px += cache_info->bbox.xMin; 
            if (py)      *py -= cache_info->bbox.yMax;

            if (px)
                *px += (cache_info->delta.x >> 6);

            return (int)(cache_info->bbox.xMax - cache_info->bbox.xMin);
        }
        DP(("\nBBOX Non - Hit! %d, %d\n", bbox_nohit++, uni_char));
    }

#endif   

    if (load_or_search_glyph (ft_inst_info, &face, uni_char, 
                get_glyph_type(logfont, devfont))) {
        _MG_PRINTF ("FONT>FT2: load_or_search_glyph error in freetype2\n");
        return 0;
    }

    if (ft_inst_info->use_kerning) {
        /* get kerning. */
        if (ft_inst_info->prev_index && ft_inst_info->cur_index) {
            int error = FT_Get_Kerning (face, 
                    ft_inst_info->prev_index, ft_inst_info->cur_index,
                    FT_KERNING_DEFAULT, &(ft_inst_info->delta));

            if (error == 0) {
                if (px)
                    *px += (ft_inst_info->delta.x >> 6);
            }
            else {
                ft_inst_info->delta.x = 0;
                ft_inst_info->delta.y = 0;
            }
        }
        ft_inst_info->prev_index = ft_inst_info->cur_index;
    }

    FT_Glyph_Get_CBox (ft_inst_info->glyph, ft_glyph_bbox_pixels, &bbox);

#if 1
    //Note: using subpixel filter, the bbox_w is 2 pixel wider than normal.
    if (IS_SUBPIXEL(logfont) && (ft_inst_info->ft_lcdfilter != FT_LCD_FILTER_NONE))
        bbox.xMax += 2;
#endif

    /* We just save the BBOX :). */
    memcpy (&ft_inst_info->bbox, &bbox, sizeof(FT_BBox));
    memcpy (&ft_inst_info->advance, &ft_inst_info->glyph->advance, 
            sizeof(FT_Vector));

    FT_Done_Glyph (ft_inst_info->glyph);

    if (bbox.xMin > bbox.xMax) {
        return 0;
    }

#ifdef _MGFONT_TTF_CACHE
    if (ft_inst_info->cache && (ft_inst_info->rotation == 0)) {
        TTFCACHEINFO * pcache;
        TTFCACHEINFO cache_info = {0};
        int datasize;
        int size = 0;

        pcache = __mg_ttc_search(ft_inst_info->cache,
               uni_char, &datasize);

        if (pcache == NULL) {
            cache_info.glyph_code = ft_inst_info->cur_index;
            cache_info.unicode = uni_char;
            cache_info.bbox    = ft_inst_info->bbox;
            cache_info.advance = ft_inst_info->advance;
            cache_info.delta   = ft_inst_info->delta;
            cache_info.bitmap  = NULL;
            cache_info.pitch   = 0;
            cache_info.flag    = FALSE;
            DP(("Should Write %d to cache length = %d, bitmap size = %d, cache = %p\n", 
                        cache_info.unicode, sizeof(TTFCACHEINFO) + size, size, 
                        ft_inst_info->cache));

            __mg_ttc_write(ft_inst_info->cache, 
                    &cache_info, sizeof(TTFCACHEINFO) + size);
        }
    }
#endif

    if (pwidth)  *pwidth = bbox.xMax - bbox.xMin;
    if (pheight) *pheight = bbox.yMax - bbox.yMin;
    if (px)      *px += bbox.xMin; 
    if (py)      *py -= bbox.yMax;

    return (int)(bbox.xMax - bbox.xMin);
}

/* press double-byte align to byte align.*/
static void 
press_bitmap (void* buffer, int width, int rows, int pitch)
{
    unsigned char*   src_pos;
    unsigned char*   dest_pos;
	int     i;
    int     dest_pitch = (width + 7) >> 3;

    if (dest_pitch == pitch)
        return;

    src_pos = (unsigned char*)buffer + pitch;
    dest_pos = (unsigned char*)buffer + dest_pitch;

    for (i = 1; i < rows; i++) {
        memmove (dest_pos, src_pos, dest_pitch);
        src_pos += pitch;
        dest_pos += dest_pitch;
    }
}


/* call this function to get the bitmap/pixmap of the char */ 
static const void* 
char_bitmap_pixmap (LOGFONT* logfont, DEVFONT* devfont, 
        const Glyph32 glyph_value, int* pitch, BOOL is_grey) 
{
    FT_BitmapGlyph  glyph_bitmap;
    FT_Bitmap*      source;
    FT_UInt         uni_char;
    FT_Face         face;
    BYTE*           buffer = NULL;
	FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);

    /*because: lock in draw_one_glyph*/
    if (IS_SUBPIXEL(logfont))
        FT_Library_SetLcdFilter(ft_library, ft_inst_info->ft_lcdfilter);
    
    if(devfont->charset_ops->conv_to_uc32)
        uni_char = (*devfont->charset_ops->conv_to_uc32) (glyph_value);
    else
        uni_char = glyph_value;

#ifdef _MGFONT_TTF_CACHE
    if (ft_inst_info->cache && (ft_inst_info->rotation == 0)) {
        TTFCACHEINFO *cacheinfo;
        int datasize;
    
        cacheinfo = __mg_ttc_search(ft_inst_info->cache,
                        uni_char, &datasize);
        
        if (cacheinfo != NULL && cacheinfo->flag == TRUE) {
            DP(("Bitmap Hit!! %d\n", bitmap_hit++));
            if (pitch)
                *pitch = cacheinfo->pitch;

            return cacheinfo->bitmap;
        }
        DP(("Bitmap Non hit %d, %d\n", bitmap_nohit++, uni_char));
    }
#endif 
    
    if (load_or_search_glyph (ft_inst_info, &face, uni_char, 
                get_glyph_type(logfont, devfont))) {
        _MG_PRINTF ("FONT>FT2: load_or_search_glyph failed in freetype2\n");
        return NULL;
    }

    /*convert to a bitmap (default render mode + destroy old or not)*/
    if (ft_inst_info->glyph->format != ft_glyph_format_bitmap) {
        if (ft_inst_info->ft_lcdfilter != FT_LCD_FILTER_NONE &&
            IS_SUBPIXEL(logfont) && is_grey) {
            if (FT_Glyph_To_Bitmap (&(ft_inst_info->glyph), 
                        FT_RENDER_MODE_LCD, NULL, 1)) {
                _MG_PRINTF ("FONT>FT2: FT_Glyph_To_Bitmap failed\n");
                return NULL;
            }
        }
        else {
            if (FT_Glyph_To_Bitmap (&(ft_inst_info->glyph), 
                        is_grey? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_MONO,
                        NULL, 1)) {
                _MG_PRINTF ("FONT>FT2: FT_Glyph_To_Bitmap failed\n");
                return NULL;
            }
        }
    }
    
    /* access bitmap content by typecasting */
    glyph_bitmap = (FT_BitmapGlyph) ft_inst_info->glyph;
    source = &glyph_bitmap->bitmap;
    
    if (pitch)
        *pitch = source->pitch;

    if (!is_grey) {
        press_bitmap(source->buffer, 
            source->width, source->rows, source->pitch);
        *pitch = (source->width + 7) >> 3;
    }

#ifdef _MGFONT_TTF_CACHE
    if (ft_inst_info->cache && (ft_inst_info->rotation == 0)) {
        TTFCACHEINFO * pcache;
        int datasize;
        int size = source->rows * (*pitch);

        pcache = __mg_ttc_search(ft_inst_info->cache,
               uni_char, &datasize);

        if (pcache == NULL) {
            TTFCACHEINFO cache_info;
            cache_info.glyph_code = ft_inst_info->cur_index;
            cache_info.unicode = uni_char;
            cache_info.bbox    = ft_inst_info->bbox;
            cache_info.advance = ft_inst_info->advance;
            cache_info.delta   = ft_inst_info->delta;
            cache_info.bitmap  = source->buffer;
            cache_info.pitch   = *pitch;
            cache_info.flag    = TRUE;
            DP(("Should Write %d to cache length = %d, bitmap size = %d, cache = %p\n", 
                        cache_info.unicode, sizeof(TTFCACHEINFO) + size, size, 
                        ft_inst_info->cache));

            __mg_ttc_write(ft_inst_info->cache, 
                    &cache_info, sizeof(TTFCACHEINFO) + size);

        }
        else {
            if (pcache->flag == FALSE) {
                pcache->pitch   = *pitch;
                memcpy(pcache->bitmap, source->buffer, size);
                pcache->flag = TRUE;
            }
            FT_Done_Glyph (ft_inst_info->glyph);
            return pcache->bitmap;
        }
    }
#endif

    buffer = get_raster_bitmap_buffer (source->rows * source->pitch);
    memcpy(buffer, source->buffer, source->rows * source->pitch);

    FT_Done_Glyph (ft_inst_info->glyph);
    
    return buffer;
}

static const void* 
get_glyph_monobitmap (LOGFONT* logfont, DEVFONT* devfont,
        const Glyph32 glyph_value, int* pitch, unsigned short* scale)
{
    if (scale) *scale = 1;
    return char_bitmap_pixmap (logfont, devfont, glyph_value, pitch, FALSE);
}
static const void* 
get_glyph_greybitmap (LOGFONT* logfont, DEVFONT* devfont, 
            const Glyph32 glyph_value, int* pitch, 
            unsigned short* scale)
{
    if (scale) *scale = 1;
    return char_bitmap_pixmap (logfont, devfont, glyph_value, pitch, TRUE); 
}

/* call this function before output a string */
static void 
start_str_output (LOGFONT* logfont, DEVFONT* devfont)
{
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);

    ft_inst_info->prev_index = 0;
    ft_inst_info->cur_index = 0;
    ft_inst_info->is_index_old = 0;

    ft_inst_info->delta.x = 0;
    ft_inst_info->delta.y = 0;
}

/* call this function after getting the bitmap/pixmap of the char
 * to get the advance of the char */
static int
get_glyph_advance (LOGFONT* logfont, DEVFONT* devfont, 
    const Glyph32 glyph_value, int* px, int* py)
{
	FT_Fixed advance;
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);


    if (ft_inst_info->use_kerning 
            && ft_inst_info->prev_index && ft_inst_info->cur_index) {
        if (ft_inst_info->is_index_old) {
            if (px)
                *px += ft_inst_info->delta.x >> 6;
        }
    }

    if (px)
        *px += ((ft_inst_info->advance.x + 0x8000)>> 16);
    if (py)
        *py -= ((ft_inst_info->advance.y + 0x8000)>> 16);

    ft_inst_info->is_index_old = 0;
    advance = FT_Vector_Length (&ft_inst_info->advance);

    return (advance >> 16) + ((advance & 0x8000) >> 15);
}

#ifdef _MGFONT_TTF_CACHE
static int make_hash_key(unsigned short data)
{
    return ((int)data % 37);
}
#endif

static DEVFONT* 
new_instance (LOGFONT* logfont, DEVFONT* devfont, BOOL need_sbc_font)
{
	float           angle;
    FT_Face         face;
    FT_Size         size;
    FTINSTANCEINFO* ft_inst_info = NULL;
    DEVFONT*        new_devfont = NULL;
    FT2_DATA*       ft_data = NULL;
	FTFACEINFO*     ft_face_info = FT_FACE_INFO_P (devfont);

    if ((new_devfont = calloc (1, sizeof (DEVFONT))) == NULL)
        goto out;

    if ((ft_inst_info = calloc (1, sizeof (FTINSTANCEINFO))) == NULL)
        goto out;

    if ((ft_data = (FT2_DATA*) calloc(1, sizeof(FTFACEINFO))) == NULL)
        goto out;

    ft_inst_info->glyph = NULL;

    memcpy (new_devfont, devfont, sizeof (DEVFONT));

    /* copy CharsetOps */
    if (need_sbc_font && devfont->charset_ops->bytes_maxlen_char > 1) {
        char charset [LEN_FONT_NAME + 1];

        fontGetCompatibleCharsetFromName (devfont->name, charset);
        new_devfont->charset_ops = GetCharsetOps (charset);
    }

    new_devfont->data = ft_data;
    ft_data->ft_inst_info = ft_inst_info;
    ft_data->ft_face_info = ft_face_info;
    ft_inst_info->ft_face_info = ft_face_info;

    if (IS_SUBPIXEL(logfont)) {
        ft_inst_info->ft_lcdfilter = FT_LCD_FILTER_DEFAULT;
    }
    else {
        ft_inst_info->ft_lcdfilter = FT_LCD_FILTER_NONE;
    }

#ifdef _MGFONT_TTF_CACHE
    if (FTC_Manager_LookupFace (ft_cache_manager,
                  (FTC_FaceID)ft_face_info, &face)) {
        /* can't access the font file. do not render anything */
        _MG_PRINTF ("FONT>FT2: can't access font file %p\n", ft_face_info);
        return 0;
    }

#else
    face = ft_face_info->face;
#endif

    /* Create instance */
    if (FT_New_Size (face, &size))
        goto out;


    if (FT_Activate_Size (size))
        goto out_size;

    if (FT_Set_Pixel_Sizes (face, logfont->size, 0))
        goto out_size;
    
    ft_inst_info->rotation = logfont->rotation; /* in tenthdegrees */
    angle = ft_inst_info->rotation * M_PI / 1800;

    if (angle) {
        ft_inst_info->matrix.yy = cos (angle) * (1 << 16);
        ft_inst_info->matrix.yx = sin (angle) * (1 << 16);
        ft_inst_info->matrix.xx = ft_inst_info->matrix.yy;
        ft_inst_info->matrix.xy = -ft_inst_info->matrix.yx;
    }
    else {
        ft_inst_info->matrix.yy = 1 << 16;
        ft_inst_info->matrix.yx = 0;
        ft_inst_info->matrix.xx = ft_inst_info->matrix.yy;
        ft_inst_info->matrix.xy = -ft_inst_info->matrix.yx;
    }
    
    ft_inst_info->use_kerning = 0;
    ft_inst_info->use_kerning = FT_HAS_KERNING(face); 
    
    ft_inst_info->max_width = size->metrics.x_ppem;
    ft_inst_info->ave_width = ft_inst_info->max_width;

    ft_inst_info->height = (size->metrics.height)>>6;
    ft_inst_info->ascent = (size->metrics.ascender)>>6;
    ft_inst_info->descent = (abs(size->metrics.descender))>>6;
    ft_inst_info->height = ft_inst_info->ascent + ft_inst_info->descent;

#ifdef _MGFONT_TTF_CACHE
    /*the width and height is the zoom scale, so they are same*/
    ft_inst_info->image_type.width = logfont->size;
    ft_inst_info->image_type.height = logfont->size;
    ft_inst_info->image_type.face_id = (FTC_FaceID)ft_inst_info->ft_face_info;

    /* houhh 20110304, AUTOHINT will be get more clear 
     * and thin glyph. */
    ft_inst_info->image_type.flags = 
        FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP | 
        FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH | FT_LOAD_TARGET_NORMAL |FT_LOAD_FORCE_AUTOHINT;
        //FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH | FT_LOAD_TARGET_NORMAL;

    /* if unmask non-cache and no rotation */
    if (!(logfont->style & FS_OTHER_TTFNOCACHE) && 
        (ft_inst_info->rotation == 0)) {
        
        HCACHE hCache = __mg_ttc_is_exist(logfont->family, logfont->charset, 
                                   logfont->style, logfont->size);
        DP(("__mg_ttc_is_exist() return %p\n", hCache));
        /* No this style's cache */
        if (hCache == 0) {
            int pitch = 0, nblk, col, blksize, rows = ft_inst_info->height;
                     
            if (((logfont->style & 0x0000000F) == FS_WEIGHT_BOOK) 
                    || ((logfont->style & 0x0000000F) == FS_WEIGHT_DEMIBOLD)
                    || ((logfont->style & 0x0000000FF) == FS_WEIGHT_SUBPIXEL)) {
                pitch = 1;
            }
            
            if (!pitch) {
                col = (ft_inst_info->max_width + 7) >> 3;
            } else {
                col = (ft_inst_info->max_width + 3) & -4;
            }
            
            blksize = col * rows;

            if ((logfont->style & 0x0000000FF) == FS_WEIGHT_SUBPIXEL)
                blksize *= 3;

            blksize += sizeof(TTFCACHEINFO);
            
            DP(("BITMAP Space = %d, Whole Space = %d\n", 
                blksize - sizeof(TTFCACHEINFO), blksize));
            
            blksize = (blksize + 3) & -4;
            nblk = ( _MGTTF_CACHE_SIZE * 1024 )/ blksize;
            DP(("[Before New a Cache], col = %d, row = %d, blksize = %d, "
                "blksize(bitmap) = %d, nblk = %d\n", 
                rows, col, blksize, blksize-sizeof(TTFCACHEINFO), nblk));
            
            ft_inst_info->cache =  __mg_ttc_create(logfont->family, logfont->charset, 
                                        logfont->style, logfont->size, nblk , blksize , 
                                        _TTF_HASH_NDIR, make_hash_key);
            DP(("__mg_ttc_create() return %p\n", ft_inst_info->cache));
        } else {
            ft_inst_info->cache = hCache;
            __mg_ttc_refer(hCache);
        }
    } else {
        ft_inst_info->cache = 0;
    }

    FT_Done_Size (size);
#else
    ft_inst_info->size = size;
#endif

    //According to face information, change the style information for logical font.
    if (face->style_flags & FT_STYLE_FLAG_ITALIC)
        new_devfont->style |= FS_SLANT_ITALIC;

    if (face->style_flags & FT_STYLE_FLAG_BOLD)
        new_devfont->style |= FS_WEIGHT_BOLD;

    return new_devfont;

out_size:
    FT_Done_Size (size);

out:
    free (ft_inst_info);
    free (new_devfont);
    return NULL;
}

static void 
delete_instance (DEVFONT* devfont)
{
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);

#ifdef _MGFONT_TTF_CACHE
    if (ft_inst_info->cache) {
        __mg_ttc_release(ft_inst_info->cache);
    }
#endif 

    free (ft_inst_info);
    free (devfont->data);
    free (devfont);
}

static BOOL is_glyph_existed (LOGFONT* logfont, DEVFONT* devfont, Glyph32 glyph_value)
{
    FT_Face         face;
    FT_UInt         uni_char;
	FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);
    
    if(devfont->charset_ops->conv_to_uc32)
        uni_char = (*devfont->charset_ops->conv_to_uc32) (glyph_value);
    else
        uni_char = glyph_value;
    
#ifndef _MGFONT_TTF_CACHE
    face = ft_inst_info->ft_face_info->face;
    if(0 == FT_Get_Char_Index (face, uni_char))
        return FALSE;
    else
        return TRUE;
#else
    if (get_cached_face (ft_inst_info, &face)) {
        _MG_PRINTF ("FONT>FT2: can't access cached face %p\n", ft_inst_info->ft_face_info);
        return FALSE;
    }

    if(0 == FTC_CMapCache_Lookup (ft_cmap_cache,
                            (FTC_FaceID) (ft_inst_info->image_type.face_id),
                            ft_inst_info->ft_face_info->cmap_index,
                            uni_char))
        return FALSE;
    else
        return TRUE;
#endif
}

static int is_rotatable (LOGFONT* logfont, DEVFONT* devfont, int rot_desired)
{
    return rot_desired;
}

/************************ Create/Destroy FreeType font ***********************/
static void* load_font_data (const char* font_name, const char *file_name)
{
    int         i;
    FT_Error    error;
    FT_Face     face;
    FT_CharMap  charmap;
    FTFACEINFO* ft_face_info = (FTFACEINFO*) calloc (1, sizeof(FTFACEINFO));
    FT2_DATA*   ft_data = (FT2_DATA*) calloc(1, sizeof(FTFACEINFO));

#ifdef _MGFONT_TTF_CACHE
    strcpy (ft_face_info->filepathname, file_name);
    ft_face_info->face_index = 0;

    error = FT_New_Face (ft_library, file_name, ft_face_info->face_index, &face);
#else
    error = FT_New_Face (ft_library, file_name, 0, &ft_face_info->face);
    face = ft_face_info->face;
#endif

    if (error == FT_Err_Unknown_File_Format) {
        _MG_PRINTF ("FONT>FT2: the font file could be opened and read, but it\n\
                ...appears that its font format is unsupported \n");
        goto error;
    }
    else if (error) {
        _MG_PRINTF ("FONT>FT2: another error code means that the font file \n\
                ... could not be opened or read. or simply that it is\
                broken ... \n ");
        goto error;
    }

    /* Look for a Unicode charmap: Windows flavor of Apple flavor only */
    for (i = 0; i < face->num_charmaps; i++) {
        charmap = face->charmaps [i];

        if (((charmap->platform_id == TT_PLATFORM_MICROSOFT) 
                    && (charmap->encoding_id == TT_MS_ID_UNICODE_CS))
                || ((charmap->platform_id == TT_PLATFORM_APPLE_UNICODE)
                    && (charmap->encoding_id == TT_APPLE_ID_DEFAULT))) {
            error = FT_Set_Charmap (face, charmap);
            if (error) {
                _MG_PRINTF ("FONT>FT2: can not set \
                        UNICODE CharMap, error is %d \n", error);
                goto error;
            }
#ifdef _MGFONT_TTF_CACHE
            ft_face_info->cmap_index = FT_Get_Charmap_Index(face->charmap);
#endif
            break;
        }
    }

    if (i == face->num_charmaps) {
        _MG_PRINTF ("FONT>FT2: No UNICODE CharMap\n");
        goto error;
    }

    ft_data->ft_face_info = ft_face_info;

    return ft_data;

error:
    free (ft_data);
    free (ft_face_info);
    return NULL;
}

static void unload_font_data (void* data)
{
    FT2_DATA* ft_data = (FT2_DATA*)data;
#ifndef _MGFONT_TTF_CACHE
    FT_Done_Face ((ft_data->ft_face_info)->face);
#endif
    free (ft_data->ft_face_info);
    free (ft_data->ft_inst_info);
    free (ft_data);
}

static void 
ShowErr (const char*  message , int error)
{
    _MG_PRINTF ("FONT>FT2: %s\n  error = 0x%04x\n", message, error );
}


/******************************* Global data ********************************/
BOOL font_InitFreetypeLibrary (void)
{
    FT_Error    error;

    /* Clear manually for VxWorks */
    ft_library = NULL;
#ifdef _MGFONT_TTF_CACHE
    ft_cache_manager = NULL;
    ft_cmap_cache = NULL;
    ft_image_cache = NULL;
#endif

    /* Init freetype library */
    error = FT_Init_FreeType (&ft_library);
    if (error) {
        ShowErr ("could not initialize FreeType 2 library", error);
        goto error_library;
    }

    /* Init freetype2 cache manager */
#ifdef _MGFONT_TTF_CACHE
    error = FTC_Manager_New (ft_library, 0, 0, 0,
                 my_face_requester, 0, &ft_cache_manager);
    if (error) {
        ShowErr ("could not initialize cache manager", error);
        goto error_ftc_manager;
    }

    error = FTC_ImageCache_New (ft_cache_manager, &ft_image_cache);
    if (error) {
        ShowErr ("could not initialize glyph image cache", error);
        goto error_ftc_manager;
    }

    error = FTC_CMapCache_New (ft_cache_manager, &ft_cmap_cache);
    if (error) {
        ShowErr ("could not initialize charmap cache", error);
        goto error_ftc_manager;
    }

#if 0
    error = FTC_SBitCache_New(ft_cache_manager, &ft_sbit_cache);
    if (error) {
        ShowErr ("could not initialize sbit cache", error);
        goto error_ftc_manager;
    }
#endif
    if (__mg_ttc_sys_init (_MGMAX_TTF_CACHE, _MGTTF_CACHE_SIZE * 1024)) {
        _MG_PRINTF ("FONT>FT2: init ttf cache sys failed\n");
        goto error_library;
    }

    return TRUE;

error_ftc_manager:
    FTC_Manager_Done (ft_cache_manager); 
#endif
    return TRUE;

error_library:
    _ERR_PRINTF ("FONT>FT2: Could not initialise FreeType 2 library\n");
    FT_Done_FreeType (ft_library);

    return FALSE;
}

void font_TermFreetypeLibrary (void)
{
#ifdef _MGFONT_TTF_CACHE
    if (ft_cache_manager)
        FTC_Manager_Done (ft_cache_manager);
    __mg_ttc_sys_deinit();
#endif

    free_raster_bitmap_buffer ();

    if (ft_library)
        FT_Done_FreeType (ft_library);
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

BOOL 
ft2SetLcdFilter (LOGFONT* logfont, mg_FT_LcdFilter filter)
{
    BOOL        rv = FALSE;
    DEVFONT*    devfont;

    if (IS_SUBPIXEL(logfont)) {
        if (filter >= MG_SMOOTH_MAX)
            filter = MG_SMOOTH_DEFAULT; 
        devfont = logfont->sbc_devfont;
        if (devfont && devfont->font_ops == &__mg_ttf_ops) {
            FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);
            ft_inst_info->ft_lcdfilter = filter;
            rv = TRUE;
        }

        devfont = logfont->mbc_devfont;
        if (devfont && devfont->font_ops == &__mg_ttf_ops) {
            FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);
            ft_inst_info->ft_lcdfilter = filter;
            rv = TRUE;
        }

        /* remove subpixel style if filter is none */
        if (filter == MG_SMOOTH_NONE) {
            logfont->style &= ~FS_WEIGHT_SUBPIXEL;
        }
    }

    return rv;
}

int 
ft2GetLcdFilter (DEVFONT* devfont)
{
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);
    return ft_inst_info->ft_lcdfilter;
}

int 
ft2IsFreeTypeDevfont (DEVFONT* devfont)
{
    if (devfont && devfont->font_ops == &__mg_ttf_ops)
        return TRUE;

    return FALSE;
}

#endif /* _MGFONT_FT2 */

