///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
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

#define IS_SUBPIXEL(logfont) (((logfont)->style & FS_RENDER_MASK) == FS_RENDER_SUBPIXEL)

#ifdef TTF_DBG
static int bbox_nohit = 0;
static int advance_hit = 0;
static int bbox_hit = 0;
static int bitmap_nohit = 0;
static int bitmap_hit = 0;
#endif

typedef struct _FT2_DATA {
    FTFACEINFO*     ft_face_info;
    FTINSTANCEINFO* ft_inst_info;
} FT2_DATA;

#define FT_FACE_INFO_P(devfont) \
    ((FTFACEINFO*)(((FT2_DATA*)(devfont->data))->ft_face_info));
#define FT_INST_INFO_P(devfont) \
    ((FTINSTANCEINFO*)(((FT2_DATA*)(devfont->data))->ft_inst_info));

static FT_Library       ft_library = NULL;

#ifdef _MGRM_THREADS
    static pthread_mutex_t ft_lock;
#   define FT_INIT_LOCK(lock, attr)   pthread_mutex_init(lock, attr)
#   define FT_LOCK(lock)              pthread_mutex_lock(lock)
#   define FT_UNLOCK(lock)            pthread_mutex_unlock(lock)
#   define FT_DESTROY_LOCK(lock)      pthread_mutex_destroy(lock)
#else
#   define FT_INIT_LOCK(lock, attr)
#   define FT_LOCK(lock)
#   define FT_UNLOCK(lock)
#   define FT_DESTROY_LOCK(lock)
#endif

#ifdef _MGFONT_TTF_CACHE
static FTC_Manager      ft_cache_manager = NULL;
static FTC_CMapCache    ft_cmap_cache = NULL;
static FTC_ImageCache   ft_image_cache = NULL;

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
    if (size <= rb_buf_size) {
        return rb_buffer;
    }
    rb_buf_size = ((size + 31) >> 5) << 5;
    rb_buffer = realloc (rb_buffer, rb_buf_size);
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

static inline void
print_bitmap_mono(const BYTE* bits, int width, int height, int pitch)
{
    int y = 0;
    int x = 0;
    const BYTE* p_line_head;
    const BYTE* p_cur_char;

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

static inline void
print_bitmap_grey (const BYTE* buffer, int width, int rows, int pitch)
{
    int i, j;
    const BYTE* p = buffer;

    printf("*******************************************\n");
    for (i = 0; i < rows; i++) {
        for (j = 0; j < pitch; j++) {
            printf("%02x, ", *p++);
        }
        printf("\n");
    }
    printf("*******************************************\n");
}

static DWORD get_glyph_bmptype (LOGFONT* logfont, DEVFONT* devfont)
{
    switch (logfont->style & FS_RENDER_MASK) {
    case FS_RENDER_SUBPIXEL:
        return DEVFONTGLYPHTYPE_SUBPIXEL;
    case FS_RENDER_GREY:
        return DEVFONTGLYPHTYPE_GREYBMP;
    default:
        break;
    }

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
get_font_size (LOGFONT* logfont, DEVFONT* devfont, int expect, int df_slot)
{
    unsigned short scale = 1;

    if (df_slot >= 0 && df_slot < MAXNR_DEVFONTS)
        SET_DEVFONT_SCALE (logfont, df_slot, scale);

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
        Glyph32 gv, int char_type)
{
    /* no need to lock/unlock in this function */
#ifndef _MGFONT_TTF_CACHE
    FTFACEINFO*     ft_face_info = ft_inst_info->ft_face_info;
    int ft_load_flags = FT_LOAD_NO_BITMAP | FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH;

    //setting load_flags
    if (char_type == DEVFONTGLYPHTYPE_MONOBMP)
        ft_load_flags |= FT_LOAD_TARGET_MONO;
    else {
        if (char_type == DEVFONTGLYPHTYPE_SUBPIXEL
                && ft_inst_info->ft_lcdfilter != FT_LCD_FILTER_NONE)
            ft_load_flags |= FT_LOAD_TARGET_LCD | FT_LOAD_FORCE_AUTOHINT;
        else
            ft_load_flags |= FT_LOAD_TARGET_NORMAL | FT_LOAD_FORCE_AUTOHINT;
    }

    FT_Activate_Size (ft_inst_info->size);

    *face = ft_face_info->face;
    ft_inst_info->cur_index = gv;
    if (FT_Load_Glyph (*face, ft_inst_info->cur_index, ft_load_flags)) {
        _ERR_PRINTF ("FONT>FT2: FT_Load_Glyph failed\n");
        return -1;
    }

    if (FT_Get_Glyph ((*face)->glyph, &(ft_inst_info->glyph))) {
        _ERR_PRINTF ("FONT>FT2: FT_Get_Glyph failed\n");
        return -1;
    }

#else /* !_MGFONT_TTF_CACHE */

    FT_Error error;
    FT_Glyph ft_glyph_tmp;
    FTFACEINFO* ft_face_info = ft_inst_info->ft_face_info;

    *face = ft_face_info->face;

    ft_inst_info->cur_index = gv;
    if ((error = FTC_ImageCache_Lookup (ft_image_cache, &ft_inst_info->image_type,
                ft_inst_info->cur_index, &ft_glyph_tmp, NULL))) {
        _ERR_PRINTF ("FONT>FT2: can't access image cache for index: 0x%X: %X\n",
            ft_inst_info->cur_index, error);
        return -1;
    }

    if (FT_Glyph_Copy (ft_glyph_tmp, &ft_inst_info->glyph)) {
        _ERR_PRINTF ("FONT>FT2: can't copy glyph from cache.\n");
        return -1;
    }

#endif /* _MGFONT_TTF_CACHE */

    if (ft_inst_info->rotation)
        FT_Glyph_Transform (ft_inst_info->glyph, &ft_inst_info->matrix, NULL);

    return 0;
}

/* call this function before getting the bitmap/pixmap of the char
 * to get the bbox of the char */
static int
get_glyph_bbox (LOGFONT* logfont, DEVFONT* devfont, Glyph32 gv,
                int* px, int* py, int* pwidth, int* pheight)
{
    FT_BBox         bbox;
    FT_Face         face;
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);

    bbox.xMin = bbox.yMin = 32000;
    bbox.xMax = bbox.yMax = -32000;

    gv = REAL_GLYPH(gv);

    FT_LOCK(&ft_lock);

#ifdef _MGFONT_TTF_CACHE
    /* Search cache */
    if (ft_inst_info->cache) {
        TTFCACHEINFO *cache_info;
        int datasize;
        cache_info = __mg_ttc_search(ft_inst_info->cache, gv, &datasize);

        if (cache_info != NULL && cache_info->valid_bbox) {
            DP(("%s: BBOX Hit!! %d\n", __FUNCTION__, bbox_hit++));
            ft_inst_info->bbox    = cache_info->bbox;
            //ft_inst_info->advance = cache_info->advance;

            if (pwidth)
                *pwidth = cache_info->bbox.xMax - cache_info->bbox.xMin;
            if (pheight)
                *pheight = cache_info->bbox.yMax - cache_info->bbox.yMin;
            if (px) *px += cache_info->bbox.xMin;
            if (py) *py -= cache_info->bbox.yMax;

            FT_UNLOCK(&ft_lock);
            return (int)(cache_info->bbox.xMax - cache_info->bbox.xMin);
        }
        DP(("%s: BBOX Non - Hit! %d, %d\n", __FUNCTION__, bbox_nohit++, gv));
    }
#endif /* _MGFONT_TTF_CACHE */

    if (load_or_search_glyph (ft_inst_info, &face, gv,
                get_glyph_bmptype(logfont, devfont))) {
        _ERR_PRINTF ("%s: load_or_search_glyph error in freetype2\n",
            __FUNCTION__);
        goto error;
    }

#if 0 /* VincentWei: do not handle kerning here; font ops are stateless */
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
#endif

    if (get_glyph_bmptype(logfont, devfont) == DEVFONTGLYPHTYPE_MONOBMP) {
        FT_Glyph_Get_CBox (ft_inst_info->glyph, FT_GLYPH_BBOX_TRUNCATE, &bbox);
    }
    else {
        FT_Glyph_Get_CBox (ft_inst_info->glyph, FT_GLYPH_BBOX_PIXELS, &bbox);

        if (IS_SUBPIXEL(logfont) &&
                (ft_inst_info->ft_lcdfilter != FT_LCD_FILTER_NONE)) {
            // Note: using subpixel filter, the bbox_w is 2 pixel wider than normal.
            bbox.xMin -= 1;
            bbox.xMax += 1;
        }
    }

    /* We just save the BBOX :). */
    memcpy (&ft_inst_info->bbox, &bbox, sizeof(FT_BBox));
    memcpy (&ft_inst_info->advance, &ft_inst_info->glyph->advance,
        sizeof(FT_Vector));

    FT_Done_Glyph (ft_inst_info->glyph);

    if (bbox.xMin > bbox.xMax) {
        goto error;
    }

#ifdef _MGFONT_TTF_CACHE
    if (ft_inst_info->cache) {
        TTFCACHEINFO * pcache;
        TTFCACHEINFO cache_info = {0};
        int datasize;

        pcache = __mg_ttc_search(ft_inst_info->cache, gv, &datasize);

        if (pcache == NULL) {
            cache_info.glyph_code = ft_inst_info->cur_index;
            cache_info.bbox    = ft_inst_info->bbox;
            cache_info.advance = ft_inst_info->advance;
            cache_info.bitmap  = NULL;
            cache_info.pitch   = 0;
            cache_info.width   = 0;
            cache_info.height  = 0;
            cache_info.valid_bbox = 1;
            cache_info.valid_advance = 1;
            cache_info.valid_bitmap = 0;
            DP(("%s: Write %d to cache length = %lu, cache = %p\n",
                    __FUNCTION__,
                    cache_info.glyph_code, sizeof(TTFCACHEINFO),
                    ft_inst_info->cache));

            __mg_ttc_write(ft_inst_info->cache,
                    &cache_info, sizeof(TTFCACHEINFO));
        }
        else {
            cache_info.bbox    = ft_inst_info->bbox;
            cache_info.advance = ft_inst_info->advance;
            cache_info.valid_bbox = 1;
            cache_info.valid_advance = 1;
        }
    }
#endif

    if (pwidth)  *pwidth = bbox.xMax - bbox.xMin;
    if (pheight) *pheight = bbox.yMax - bbox.yMin;
    if (px)      *px += bbox.xMin;
    if (py)      *py -= bbox.yMax;

    FT_UNLOCK(&ft_lock);
    return (int)(bbox.xMax - bbox.xMin);

error:
    FT_UNLOCK(&ft_lock);
    return 0;
}

/* call this function to get the bitmap/pixmap of the char */
static const void*
char_bitmap_pixmap (LOGFONT* logfont, DEVFONT* devfont,
        Glyph32 gv, SIZE* sz, int* pitch, BOOL is_grey)
{
    FT_BitmapGlyph  glyph_bitmap;
    FT_Bitmap*      source;
    FT_Face         face;
    BYTE*           buffer = NULL;
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);

    gv = REAL_GLYPH(gv);

    FT_LOCK(&ft_lock);

#ifdef _MGFONT_TTF_CACHE
    if (ft_inst_info->cache) {
        TTFCACHEINFO *cacheinfo;
        int datasize;

        cacheinfo = __mg_ttc_search(ft_inst_info->cache, gv, &datasize);

        if (cacheinfo != NULL && cacheinfo->valid_bitmap) {
            DP(("%s: Bitmap Hit!! %d\n", __FUNCTION__, bitmap_hit++));
            if (pitch)
                *pitch = cacheinfo->pitch;

            /* VincentWei: override the bbox.w and bbox.h with bitmap information */
            if (sz) {
                sz->cx = cacheinfo->width;
                sz->cy = cacheinfo->height;
            }
            FT_UNLOCK(&ft_lock);
            return cacheinfo->bitmap;
        }

        DP(("%s: Bitmap Non hit %d, %d\n", __FUNCTION__, bitmap_nohit++, gv));
    }
#endif /* _MGFONT_TTF_CACHE */

    if (IS_SUBPIXEL(logfont))
        FT_Library_SetLcdFilter(ft_library, ft_inst_info->ft_lcdfilter);

    if (load_or_search_glyph (ft_inst_info, &face, gv,
                get_glyph_bmptype(logfont, devfont))) {
        _ERR_PRINTF ("%s: load_or_search_glyph failed in freetype2\n",
            __FUNCTION__);
        goto error;
    }

    /*convert to a bitmap (default render mode + destroy old or not)*/
    if (ft_inst_info->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
        if (ft_inst_info->ft_lcdfilter != FT_LCD_FILTER_NONE &&
                IS_SUBPIXEL(logfont) && is_grey) {
            if (FT_Glyph_To_Bitmap (&(ft_inst_info->glyph),
                        FT_RENDER_MODE_LCD, NULL, 1)) {
                _ERR_PRINTF ("%s: FT_Glyph_To_Bitmap failed\n",
                    __FUNCTION__);
                goto error;
            }
        }
        else {
            if (FT_Glyph_To_Bitmap (&(ft_inst_info->glyph),
                        is_grey ? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_MONO,
                        NULL, 1)) {
                _ERR_PRINTF ("%s: FT_Glyph_To_Bitmap failed\n",
                    __FUNCTION__);
                goto error;
            }
        }
    }

    /* access bitmap content by typecasting */
    glyph_bitmap = (FT_BitmapGlyph) ft_inst_info->glyph;
    source = &glyph_bitmap->bitmap;

    if (pitch)
        *pitch = source->pitch;

#ifdef _MGFONT_TTF_CACHE
    if (ft_inst_info->cache) {
        TTFCACHEINFO * pcache;
        int datasize;
        int size = source->rows * source->pitch;

        pcache = __mg_ttc_search(ft_inst_info->cache, gv, &datasize);

        if (pcache == NULL) {
            TTFCACHEINFO cache_info;
            cache_info.glyph_code = ft_inst_info->cur_index;
            cache_info.bbox    = ft_inst_info->bbox;
            cache_info.advance = ft_inst_info->advance;
            cache_info.bitmap  = source->buffer;
            cache_info.pitch   = source->pitch;
            cache_info.width   = source->width;
            cache_info.height  = source->rows;
            cache_info.valid_advance = 1;
            cache_info.valid_bbox = 1;
            cache_info.valid_bitmap = 1;
            DP(("%s: Write 0x%x to cache length = %d, bitmap size = %d, cache = %p\n",
                    __FUNCTION__,
                    cache_info.glyph_code,
                    (int)(sizeof(TTFCACHEINFO) + size), size,
                    ft_inst_info->cache));

            __mg_ttc_write(ft_inst_info->cache,
                    &cache_info, sizeof(TTFCACHEINFO) + size);
        }
        else {
            if (pcache->valid_bitmap == 0) {
                pcache->pitch  = source->pitch;
                pcache->width  = source->width;
                pcache->height = source->rows;
                memcpy(pcache->bitmap, source->buffer, size);
                pcache->valid_bitmap = 1;

                DP(("%s: Write bitmap data for 0x%x to cache, bitmap (%d X %d, %d), cache = %p\n",
                        __FUNCTION__, gv, source->width, source->rows,
                        source->pitch, ft_inst_info->cache));
            }

            /* VincentWei: override the bbox.w and bbox.h with bitmap information */
            if (sz) {
                sz->cx = pcache->width;
                sz->cy = pcache->height;
            }

#ifdef TTF_DBG
            if (is_grey)
                print_bitmap_grey(source->buffer, source->width, source->rows, source->pitch);
            else
                print_bitmap_mono(source->buffer, source->width, source->rows, source->pitch);
#endif

            FT_Done_Glyph (ft_inst_info->glyph);
            FT_UNLOCK(&ft_lock);
            return pcache->bitmap;
        }
    }
#endif /* _MGFONT_TTF_CACHE */

    buffer = get_raster_bitmap_buffer(source->rows * source->pitch);
    memcpy(buffer, source->buffer, source->rows * source->pitch);
    /* VincentWei: override the bbox.w and bbox.h with bitmap */
    if (sz) {
        sz->cx = source->width;
        sz->cy = source->rows;
    }

#ifdef TTF_DBG
    if (is_grey)
        print_bitmap_grey(source->buffer, source->width, source->rows, source->pitch);
    else
        print_bitmap_mono(source->buffer, source->width, source->rows, source->pitch);
#endif

    FT_Done_Glyph (ft_inst_info->glyph);
    FT_UNLOCK(&ft_lock);
    return buffer;

error:
    FT_UNLOCK(&ft_lock);
    return NULL;
}

static const void*
get_glyph_monobitmap (LOGFONT* logfont, DEVFONT* devfont,
        Glyph32 gv, SIZE* sz, int* pitch, unsigned short* scale)
{
    gv = REAL_GLYPH(gv);
    if (scale) *scale = 1;
    return char_bitmap_pixmap (logfont, devfont, gv, sz, pitch, FALSE);
}

static const void*
get_glyph_greybitmap (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 gv, SIZE* sz, int* pitch,
            unsigned short* scale)
{
    gv = REAL_GLYPH(gv);
    if (scale) *scale = 1;
    return char_bitmap_pixmap (logfont, devfont, gv, sz, pitch, TRUE);
}

/* call this function before output a string */
static void
start_str_output (LOGFONT* logfont, DEVFONT* devfont)
{
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);

#if 0
    ft_inst_info->prev_index = 0;
    ft_inst_info->cur_index = 0;
    ft_inst_info->is_index_old = 0;

    ft_inst_info->delta.x = 0;
    ft_inst_info->delta.y = 0;
#else
    ft_inst_info->cur_index = 0;
#endif
}

/* call this function after getting the bitmap/pixmap of the char
 * to get the advance of the char */
static int
get_glyph_advance (LOGFONT* logfont, DEVFONT* devfont,
        Glyph32 gv, int* px, int* py)
{
    FT_Face  face;
    FT_Fixed advance;
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);

    gv = REAL_GLYPH(gv);

    FT_LOCK(&ft_lock);

#ifdef _MGFONT_TTF_CACHE
    /* Search cache */
    if (ft_inst_info->cache) {
        int datasize;
        TTFCACHEINFO *cache_info;
        cache_info = __mg_ttc_search(ft_inst_info->cache, gv, &datasize);

        if (cache_info != NULL && cache_info->valid_advance) {
            DP(("%s: ADVANCE Hit!! %d\n", __FUNCTION__, advance_hit++));
            ft_inst_info->advance = cache_info->advance;
            goto done;
        }

        DP(("%s: ADVANCE Not Hit! %d, %d\n", __FUNCTION__, advance_hit++, gv));
    }
#endif /* _MGFONT_TTF_CACHE */

    if (load_or_search_glyph (ft_inst_info, &face, gv,
                get_glyph_bmptype(logfont, devfont))) {
        _ERR_PRINTF ("%s: load_or_search_glyph error in freetype2\n",
            __FUNCTION__);
        goto error;
    }

    memcpy (&ft_inst_info->advance, &ft_inst_info->glyph->advance,
        sizeof(FT_Vector));

    FT_Done_Glyph (ft_inst_info->glyph);

#ifdef _MGFONT_TTF_CACHE
    if (ft_inst_info->cache) {
        TTFCACHEINFO *pcache;
        TTFCACHEINFO cache_info = {0};
        int datasize;

        pcache = __mg_ttc_search(ft_inst_info->cache, gv, &datasize);

        if (pcache == NULL) {
            cache_info.glyph_code = ft_inst_info->cur_index;
            cache_info.advance = ft_inst_info->advance;
            cache_info.bitmap  = NULL;
            cache_info.pitch   = 0;
            cache_info.valid_bbox = 0;
            cache_info.valid_advance = 1;
            cache_info.valid_bitmap = 0;

            __mg_ttc_write(ft_inst_info->cache, &cache_info,
                    sizeof(TTFCACHEINFO));
        }
        else {
            pcache->advance = ft_inst_info->advance;
            pcache->valid_advance = 1;
        }
    }
#endif

done:
    FT_UNLOCK(&ft_lock);

    if (px) {
        //int old_x = *px;
        *px += ((ft_inst_info->advance.x + 0x8000) >> 16);
    }
    if (py) {
        //int old_y = *py;
        *py -= ((ft_inst_info->advance.y + 0x8000) >> 16);
    }

    advance = FT_Vector_Length (&ft_inst_info->advance);
    return (advance >> 16) + ((advance & 0x8000) >> 15);

error:
    FT_UNLOCK(&ft_lock);
    return -1;
}

static void get_kerning (LOGFONT* logfont, DEVFONT* devfont,
        Glyph32 prev, Glyph32 curr, int* delta_x, int* delta_y)
{
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);
    FT_Vector       delta;

    delta.x = 0;
    delta.y = 0;

    prev = REAL_GLYPH(prev);
    curr = REAL_GLYPH(curr);
    if (ft_inst_info->use_kerning && curr && prev) {

        FTFACEINFO* ft_face_info = FT_FACE_INFO_P (devfont);
        /* get kerning. */
        FT_Get_Kerning(ft_face_info->face, prev, curr, FT_KERNING_DEFAULT, &delta);
    }

    if (delta_x)
        *delta_x = delta.x >> 6;

    if (delta_y)
        *delta_y = delta.y >> 6;
}

static void* get_ft_face (LOGFONT* logfont, DEVFONT* devfont)
{
    FTFACEINFO* ft_face_info = FT_FACE_INFO_P (devfont);
    return ft_face_info;
}

#ifdef _MGFONT_TTF_CACHE
static int make_hash_key(Glyph32 data)
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
        char charset [LEN_LOGFONT_NAME_FIELD + 1];

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

    FT_LOCK(&ft_lock);

    face = ft_face_info->face;

    /* Create instance */
    if (FT_New_Size (face, &size))
        goto out_lock;

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
    ft_inst_info->image_type.flags = FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP |
            FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH;

    if ((logfont->style & FS_RENDER_MASK) == FS_RENDER_MONO)
        ft_inst_info->image_type.flags |=
            FT_LOAD_TARGET_MONO | FT_LOAD_FORCE_AUTOHINT;
    else {
        if ((logfont->style & FS_RENDER_MASK) == FS_RENDER_SUBPIXEL
                && ft_inst_info->ft_lcdfilter != FT_LCD_FILTER_NONE) {
            ft_inst_info->image_type.flags |=
                FT_LOAD_TARGET_LCD | FT_LOAD_FORCE_AUTOHINT;
        }
        else
            ft_inst_info->image_type.flags |=
                FT_LOAD_TARGET_NORMAL | FT_LOAD_FORCE_AUTOHINT;
    }

    /* if unmask non-cache and no rotation */
    if (!(logfont->style & FS_OTHER_TTFNOCACHE)) {

        HCACHE hCache = __mg_ttc_is_exist(devfont->name,
                        logfont->style, logfont->size, logfont->rotation);
        DP(("__mg_ttc_is_exist() return %p\n", hCache));
        /* No this style's cache */
        if (hCache == 0) {
            int nblk, col, blksize, rows = ft_inst_info->height;

            if ((logfont->style & FS_RENDER_MASK) == FS_RENDER_MONO) {
                col = (ft_inst_info->max_width + 7) >> 3;
            }
            else {
                col = (ft_inst_info->max_width + 3) & -4;
            }

            blksize = col * rows;

            if ((logfont->style & FS_RENDER_MASK) == FS_RENDER_SUBPIXEL)
                blksize *= 3;

            blksize += sizeof(TTFCACHEINFO);

            DP(("BITMAP Space = %d, Whole Space = %d\n",
                (int)(blksize - sizeof(TTFCACHEINFO)), blksize));

            blksize = (blksize + 3) & -4;
            nblk = ( _MGTTF_CACHE_SIZE * 1024 )/ blksize;
            DP(("[Before New a Cache], col = %d, row = %d, blksize = %d, "
                "blksize(bitmap) = %d, nblk = %d\n",
                rows, col, blksize, (int)(blksize-sizeof(TTFCACHEINFO)), nblk));

            ft_inst_info->cache = __mg_ttc_create(devfont->name,
                logfont->style, logfont->size, logfont->rotation,
                nblk, blksize, _TTF_HASH_NDIR, make_hash_key);

            DP(("__mg_ttc_create() return %p\n", ft_inst_info->cache));
        } else {
            ft_inst_info->cache = hCache;
            __mg_ttc_refer(hCache);
        }
    } else {
        ft_inst_info->cache = 0;
    }

    FT_Done_Size (size);
#else /* _MGFONT_TTF_CACHE */
    ft_inst_info->size = size;
#endif

    FT_UNLOCK(&ft_lock);
    return new_devfont;

out_size:
    FT_Done_Size (size);

out_lock:
    FT_UNLOCK(&ft_lock);

out:
    if (ft_data) free (ft_data);
    if (ft_inst_info) free (ft_inst_info);
    if (new_devfont) free (new_devfont);
    return NULL;
}

static void
delete_instance (DEVFONT* devfont)
{
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);

#ifdef _MGFONT_TTF_CACHE
    FT_LOCK(&ft_lock);

    if (ft_inst_info->cache) {
        __mg_ttc_release(ft_inst_info->cache);
    }

    FT_UNLOCK(&ft_lock);
#else
    FT_Done_Size (ft_inst_info->size);
#endif /* _MGFONT_TTF_CACHE */

    free (ft_inst_info);
    free (devfont->data);
    free (devfont);
}

static Glyph32 get_glyph_value (LOGFONT* logfont, DEVFONT* devfont, Achar32 ac)
{
    FT_UInt uc;
    Glyph32 gv = INV_GLYPH_VALUE;
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);

    if (devfont->charset_ops->conv_to_uc32)
        uc = (*devfont->charset_ops->conv_to_uc32) (ac);
    else
        uc = REAL_ACHAR(ac);

    FT_LOCK(&ft_lock);

#ifndef _MGFONT_TTF_CACHE
    gv = FT_Get_Char_Index (ft_inst_info->ft_face_info->face, uc);
#else /* !_MGFONT_TTF_CACHE */
    gv = FTC_CMapCache_Lookup (ft_cmap_cache,
        (FTC_FaceID) (ft_inst_info->image_type.face_id),
        ft_inst_info->ft_face_info->cmap_index, uc);
#endif /* _MGFONT_TTF_CACHE */

    FT_UNLOCK(&ft_lock);
    return gv;
}

static BOOL is_glyph_existed (LOGFONT* logfont, DEVFONT* devfont, Glyph32 gv)
{
    if (REAL_GLYPH(gv))
        return TRUE;

    return FALSE;
}

static int is_rotatable (LOGFONT* logfont, DEVFONT* devfont, int rot_desired)
{
    return rot_desired;
}

/************************ Create/Destroy FreeType font ***********************/
static void* load_font_data (DEVFONT* devfont,
        const char* font_name, const char *file_name)
{
    int         i;
    FT_Error    error;
    FT_Face     face;
    FT_CharMap  charmap;
    FTFACEINFO* ft_face_info = (FTFACEINFO*) calloc (1, sizeof(FTFACEINFO));
    FT2_DATA*   ft_data = (FT2_DATA*) calloc(1, sizeof(FTFACEINFO));

    if (ft_face_info == NULL || ft_data == NULL) {
        goto error_free;
    }

    FT_LOCK(&ft_lock);

    ft_face_info->filepathname = strdup (file_name);
    if (ft_face_info->filepathname == NULL)
        goto error;

    error = FT_New_Face(ft_library, file_name, 0, &ft_face_info->face);
    face = ft_face_info->face;

    if (error == FT_Err_Unknown_File_Format) {
        _ERR_PRINTF ("%s: bad file format: %s\n", __FUNCTION__, file_name);
        goto error;
    }
    else if (error) {
        _ERR_PRINTF ("%s: failed to open the font file: %s (%d)\n",
            __FUNCTION__, file_name, error);
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
                _ERR_PRINTF ("%s: can not set UNICODE CharMap (%d)\n",
                    __FUNCTION__, error);
                goto error;
            }
#ifdef _MGFONT_TTF_CACHE
            ft_face_info->cmap_index = FT_Get_Charmap_Index(face->charmap);
#endif
            break;
        }
    }

    if (i == face->num_charmaps) {
        _ERR_PRINTF ("%s: No UNICODE CharMap: %s\n", __FUNCTION__, file_name);
        goto error;
    }

    ft_data->ft_face_info = ft_face_info;

    if (devfont) {
        // According to OS/2 table information,
        // change the style information for dev font.

        TT_OS2* tt_os2 = (TT_OS2*)FT_Get_Sfnt_Table (face, FT_SFNT_OS2);
        unsigned int weight_style = FS_WEIGHT_REGULAR;
        if (tt_os2->usWeightClass >= 100 && tt_os2->usWeightClass <= 900) {
            weight_style = (tt_os2->usWeightClass/100 * 10);
        }
        else {
            _DBG_PRINTF("FONT>FT2: Unknown tt_os2->usWeightClass: %u\n",
            tt_os2->usWeightClass);
        }

        devfont->style &= ~FS_WEIGHT_MASK;
        devfont->style |= weight_style;

        devfont->style &= ~FS_SLANT_MASK;
        if (tt_os2->fsSelection & (0x01 << 9)) // OBLIQUE
            devfont->style |= FS_SLANT_OBLIQUE;
        else if (tt_os2->fsSelection & 0x01) // ITALIC
            devfont->style |= FS_SLANT_ITALIC;
        else
            devfont->style |= FS_SLANT_ROMAN;

        devfont->style &= ~FS_DECORATE_MASK;
        if (tt_os2->fsSelection & (0x01 << 1)) // UNDERSCORE
            devfont->style |= FS_DECORATE_UNDERLINE;
        if (tt_os2->fsSelection & (0x01 << 2)) // NEGATIVE
            devfont->style |= FS_DECORATE_REVERSE;
        if (tt_os2->fsSelection & (0x01 << 3)) // OUTLINED (HOLLOW)
            devfont->style |= FS_DECORATE_OUTLINE;
        if (tt_os2->fsSelection & (0x01 << 4)) // STRIKEOUT
            devfont->style |= FS_DECORATE_STRUCKOUT;
    }

    FT_UNLOCK(&ft_lock);
    return ft_data;

error:
    FT_UNLOCK(&ft_lock);

error_free:
    if (ft_data)
        free (ft_data);

    if (ft_face_info) {
        if (ft_face_info->filepathname)
            free(ft_face_info->filepathname);
        free(ft_face_info);
    }

    return NULL;
}

static void unload_font_data (DEVFONT* devfont, void* data)
{
    FT2_DATA* ft_data = (FT2_DATA*)data;
    free(ft_data->ft_face_info->filepathname);
    FT_Done_Face((ft_data->ft_face_info)->face);
    free(ft_data->ft_face_info);
    free(ft_data->ft_inst_info);
    free(ft_data);
}

static void ShowErr (const char* func, const char* message, int error)
{
    _ERR_PRINTF ("%s: %s (error = 0x%04x)\n", func, message, error );
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
        ShowErr (__FUNCTION__, "could not initialize FreeType 2 library", error);
        goto error_library;
    }

    FT_INIT_LOCK(&ft_lock, NULL);

#ifdef _MGFONT_TTF_CACHE
    /* Init freetype2 cache manager */
    error = FTC_Manager_New (ft_library, 0, 0, 0,
                 my_face_requester, 0, &ft_cache_manager);
    if (error) {
        ShowErr (__FUNCTION__, "could not initialize cache manager", error);
        goto error_ftc_manager;
    }

    error = FTC_ImageCache_New (ft_cache_manager, &ft_image_cache);
    if (error) {
        ShowErr (__FUNCTION__, "could not initialize glyph image cache", error);
        goto error_ftc_manager;
    }

    error = FTC_CMapCache_New (ft_cache_manager, &ft_cmap_cache);
    if (error) {
        ShowErr (__FUNCTION__, "could not initialize charmap cache", error);
        goto error_ftc_manager;
    }

#if 0
    error = FTC_SBitCache_New(ft_cache_manager, &ft_sbit_cache);
    if (error) {
        ShowErr (__FUNCTION__, "could not initialize sbit cache", error);
        goto error_ftc_manager;
    }
#endif

    if (__mg_ttc_sys_init (_MGMAX_TTF_CACHE, _MGTTF_CACHE_SIZE * 1024)) {
        _ERR_PRINTF ("FONT>FT2: failed to initialize TTF cache system\n");
        goto error_library;
    }

#ifdef _MGCOMPLEX_SCRIPTS
    __mg_init_harzbuff_funcs();
#endif

    return TRUE;

error_ftc_manager:
    FTC_Manager_Done (ft_cache_manager);
    return FALSE;

#endif /* _MGFONT_TTF_CACHE */

#ifdef _MGCOMPLEX_SCRIPTS
    __mg_init_harzbuff_funcs();
#endif

    return TRUE;

error_library:
    _ERR_PRINTF ("FONT>FT2: Could not initialise FreeType 2 library\n");

    FT_DESTROY_LOCK(&ft_lock);
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

    FT_DESTROY_LOCK(&ft_lock);

#ifdef _MGCOMPLEX_SCRIPTS
    __mg_term_harzbuff_funcs();
#endif
}

/**************************** Global data ************************************/
FONTOPS __mg_ttf_ops = {
    get_glyph_bmptype,
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
    unload_font_data,
    get_glyph_value,
    get_kerning,
    get_ft_face
};

/*  Get the FreeType2 face object for HarfBuzz shaping engine */
const FT2INFO* __mg_ft2_get_face(LOGFONT* lf, Uchar32 uc, int* dfi)
{
    const FT2INFO* face = NULL;
    DEVFONT* df;
    int i;

    /* find the FreeType2 devfont which contains the glyph for
     * the Uchar32 character */
    for (i = 1; i < MAXNR_DEVFONTS; i++) {
        if ((df = lf->devfonts[i]) && df->font_ops->get_ft_face) {
            face = df->font_ops->get_ft_face(lf, df);
            if (df->font_ops->get_glyph_value(lf, df, uc)) {
                *dfi = i;
                return face;
            }
        }
    }

    /* return the default face object */
    if ((df = lf->devfonts[1]) && df->font_ops->get_ft_face) {
        *dfi = 1;
        face = df->font_ops->get_ft_face(lf, df);
        return face;
    }

    return NULL;
}

BOOL ft2SetLcdFilter (LOGFONT* logfont, FT2LCDFilter filter)
{
    int         i;
    BOOL        rv = FALSE;
    DEVFONT*    devfont;

    if (IS_SUBPIXEL(logfont)) {
        if (filter >= MG_SMOOTH_MAX)
            filter = MG_SMOOTH_DEFAULT;
        for (i = 0; i < MAXNR_DEVFONTS; i++) {
            if ((devfont = logfont->devfonts[i]) == NULL)
                break;

            if (devfont->font_ops == &__mg_ttf_ops) {
                FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);
                ft_inst_info->ft_lcdfilter = filter;
                rv = TRUE;
            }
        }

        /* remove subpixel style if filter is none */
        if (filter == MG_SMOOTH_NONE) {
            logfont->style &= ~FS_RENDER_MASK;
            logfont->style |= FS_RENDER_MONO;
        }
    }

    return rv;
}

int ft2GetLcdFilter (DEVFONT* devfont)
{
    FTINSTANCEINFO* ft_inst_info = FT_INST_INFO_P (devfont);
    return ft_inst_info->ft_lcdfilter;
}

int ft2IsFreeTypeDevfont (DEVFONT* devfont)
{
    if (devfont && devfont->font_ops == &__mg_ttf_ops)
        return TRUE;

    return FALSE;
}

#endif /* _MGFONT_FT2 */

