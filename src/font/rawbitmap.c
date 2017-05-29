/*
** $Id: rawbitmap.c 12350 2009-12-16 05:01:08Z weiym $
** 
** rawbitmap.c: The Raw Bitmap Font operation set.
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** All right reserved by Feynman Software.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/06/13
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "misc.h"

#undef HAVE_MMAP
#ifdef HAVE_MMAP
#error
    #include <sys/mman.h>
#endif

#ifdef _MGFONT_RBF

#include "devfont.h"
#include "charset.h"
#include "rawbitmap.h"
#include "fontname.h"

#define RBFONT_INFO_P(devfont) ((RBFINFO*)(devfont->data))
#define RBFONT_INFO(devfont) ((RBFINFO*)(devfont.data))

/********************** Load/Unload of raw bitmap font ***********************/
#if 0
static void* load_font_data (char* font_name, char* file_name, 
        char* real_font_name)
#endif
static void* load_font_data (const char* font_name, const char* file_name)
{
    FILE* fp = NULL;
    RBFINFO* rbf_info = calloc (1, sizeof(RBFINFO));
    long data_size;

    if (!rbf_info)
        return NULL;

    if ((rbf_info->width = fontGetWidthFromName (font_name)) == -1)
        goto error_load;

    if ((rbf_info->height = fontGetHeightFromName (font_name)) == -1)
        goto error_load;

    if (!(fp = fopen (file_name, "rb")))
        goto error_load;

    data_size = get_opened_file_size (fp);
    rbf_info->data_size = data_size;

    if (rbf_info->data_size <= 0)
        goto error_load;

    rbf_info->nr_glyphs = rbf_info->data_size /
        (((rbf_info->width+7)>>3) * (rbf_info->height));

#ifdef HAVE_MMAP
    if ((rbf_info->data = mmap (NULL, rbf_info->data_size, PROT_READ, MAP_SHARED, fileno(fp), 0)) == MAP_FAILED) {
        goto error_load;
    }
#else
    // Allocate memory for font data.
    if (!(rbf_info->data = (unsigned char*) malloc (data_size)))
        goto error_load;

    if (fread (rbf_info->data, sizeof(char), data_size, fp) < data_size) {
        free (rbf_info->data);
        goto error_load;
    }
#endif  /* HAVE_MMAP */
    fclose (fp);

#if 0
    if (real_font_name)
        strcpy (real_font_name, font_name);
#endif

    return rbf_info;

error_load:

    if (fp)
        fclose (fp);
    free (rbf_info);
    return NULL;
}

/********************** Init/Term of raw bitmap font ***********************/

static void unload_font_data (void* data)
{
    RBFINFO* rbfinfo = (RBFINFO*)data;
#ifdef HAVE_MMAP
    if (rbfinfo->data)
        munmap (rbfinfo->data, rbfinfo->data_size);
#else
    free (rbfinfo->data);
#endif
    free (rbfinfo);
}

/*************** Raw bitmap font operations *********************************/

static DWORD get_glyph_type (LOGFONT* logfont, DEVFONT* devfont)
{
    return DEVFONTGLYPHTYPE_MONOBMP;
}

static int get_ave_width (LOGFONT* logfont, DEVFONT* devfont)
{
    return RBFONT_INFO_P (devfont)->width * GET_DEVFONT_SCALE (logfont, devfont);
}

static int get_font_height (LOGFONT* logfont, DEVFONT* devfont)
{
    return RBFONT_INFO_P (devfont)->height * GET_DEVFONT_SCALE (logfont, devfont);
}

static int get_font_size (LOGFONT* logfont, DEVFONT* devfont, int expect)
{
    int height = RBFONT_INFO_P (devfont)->height;
    unsigned short scale = 1;

    if (logfont->style & FS_OTHER_AUTOSCALE)
        scale = font_GetBestScaleFactor (height, expect);

    SET_DEVFONT_SCALE (logfont, devfont, scale);

    return height * scale;
}

static int get_font_ascent (LOGFONT* logfont, DEVFONT* devfont)
{
    int height = RBFONT_INFO_P (devfont)->height;

    height *= GET_DEVFONT_SCALE (logfont, devfont);

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
    int height = RBFONT_INFO_P (devfont)->height;
    
    height *= GET_DEVFONT_SCALE (logfont, devfont);

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

static const void* get_glyph_monobitmap (LOGFONT* logfont, DEVFONT* devfont,
            const Glyph32 glyph_value, int* pitch, unsigned short* scale)
{
    int bitmap_size;
    Glyph32 glyph_tmp = glyph_value;

    if (glyph_tmp >= RBFONT_INFO_P (devfont)->nr_glyphs)
        glyph_tmp = devfont->charset_ops->def_glyph_value;

    bitmap_size = ((RBFONT_INFO_P (devfont)->width + 7) >> 3) 
                * RBFONT_INFO_P (devfont)->height; 
    if(pitch)
        *pitch = (RBFONT_INFO_P (devfont)->width + 7) >> 3;

    if (scale)
        *scale = GET_DEVFONT_SCALE (logfont, devfont);

    return RBFONT_INFO_P (devfont)->data + bitmap_size * glyph_tmp;
}

static BOOL is_glyph_existed (LOGFONT* logfont, DEVFONT* devfont, 
        Glyph32 glyph_value)
{
    if (glyph_value > RBFONT_INFO_P(devfont)->nr_glyphs)
        return FALSE;
    else
        return TRUE;
}

static int get_glyph_advance (LOGFONT* logfont, DEVFONT* devfont, 
                Glyph32 glyph_value, int* px, int* py)
{
    int advance = RBFONT_INFO_P (devfont)->width * 
                    GET_DEVFONT_SCALE (logfont, devfont);
    *px += advance;
    return advance;
}

static int get_glyph_bbox (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 glyph_value, int* px, int* py, int* pwidth, int* pheight)
{
    int scale = GET_DEVFONT_SCALE (logfont, devfont);
    int width = RBFONT_INFO_P (devfont)->width * scale;

    if (py)
        *py -= get_font_ascent (logfont, devfont);
    if (pwidth)
        *pwidth  = width;
    if (pheight)
        *pheight = RBFONT_INFO_P (devfont)->height * scale;

    return width;
}

static int is_rotatable (LOGFONT* logfont, DEVFONT* devfont, int rot_desired)
{
    return 0;
}

/**************************** Global data ************************************/
FONTOPS __mg_rbf_ops = {
    get_glyph_type,
    get_ave_width,
    get_ave_width,  // max_width same as ave_width
    get_font_height,
    get_font_size,
    get_font_ascent,
    get_font_descent,

    is_glyph_existed,
    get_glyph_advance,
    get_glyph_bbox,

    get_glyph_monobitmap,
    NULL,
    NULL,

    NULL,
    NULL,
    NULL,
    is_rotatable,
    load_font_data,
    unload_font_data
};

#endif  /* _MGFONT_RBF */

