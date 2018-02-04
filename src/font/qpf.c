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
** qpf.c: The Qt Prerendered Font operation set.
**
** Create date: 2003/01/28
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "misc.h"

#ifdef _MGFONT_QPF

#ifdef HAVE_MMAP
    #include <sys/mman.h>
#endif

#include "devfont.h"
#include "charset.h"
#include "qpf.h"
#include "fontname.h"

#define QPFONT_INFO_P(devfont) ((QPFINFO*)(devfont->data))
#define QPFONT_INFO(devfont) ((QPFINFO*)(devfont.data))

/*load qpf from file, only support for linux*/
typedef unsigned char uchar;

static void read_node (QPF_GLYPHTREE* tree, uchar** data)
{
    uchar rw, cl;
    int flags, n;

    rw = **data; (*data)++;
    cl = **data; (*data)++;
    tree->min = (rw << 8) | cl;

    rw = **data; (*data)++;
    cl = **data; (*data)++;
    tree->max = (rw << 8) | cl;

    flags = **data; (*data)++;
    if (flags & 1)
        tree->less = calloc (1, sizeof (QPF_GLYPHTREE));
    else
        tree->less = NULL;

    if (flags & 2)
        tree->more = calloc (1, sizeof (QPF_GLYPHTREE));
    else
        tree->more = NULL;

    n = tree->max - tree->min + 1;
    tree->glyph = calloc (n, sizeof (QPF_GLYPH));

    if (tree->less)
        read_node (tree->less, data);
    if (tree->more)
        read_node (tree->more, data);
}

static void read_metrics (QPF_GLYPHTREE* tree, uchar** data)
{
    int i;
    int n = tree->max - tree->min + 1;

    for (i = 0; i < n; i++) {
        tree->glyph[i].metrics = (QPF_GLYPHMETRICS*) *data;

        *data += sizeof (QPF_GLYPHMETRICS);
    }

    if (tree->less)
        read_metrics (tree->less, data);
    if (tree->more)
        read_metrics (tree->more, data);
}

static void read_data (QPF_GLYPHTREE* tree, uchar** data)
{
    int i;
    int n = tree->max - tree->min + 1;

    for (i = 0; i < n; i++) {
        int datasize;

        datasize = tree->glyph[i].metrics->linestep * 
                tree->glyph[i].metrics->height;
        tree->glyph[i].data = *data; *data += datasize;
    }

    if (tree->less)
        read_data (tree->less, data);
    if (tree->more)
        read_data (tree->more, data);
}


static void build_glyph_tree (QPF_GLYPHTREE* tree, uchar** data)
{
    read_node (tree, data);
    read_metrics (tree, data);
    read_data (tree, data);
}

static void clear_glyph_tree (QPF_GLYPHTREE* tree)
{
    if (tree->less) {
        clear_glyph_tree (tree->less);
    }
    if (tree->more) {
        clear_glyph_tree (tree->more);
    }

    free (tree->glyph);
    free (tree->less);
    free (tree->more);
}

static void* load_font_data (const char* font_name, const char* file_name)
{
    FILE* fp = NULL;
    uchar* data;
    long file_size;
    QPFINFO* qpf_info = NULL;

    qpf_info = (QPFINFO*) calloc (1, sizeof(QPFINFO));

    if (!(fp = fopen (file_name, "rb"))) {
        _MG_PRINTF ("FONT>QPF: open file error: %s.\n",
                file_name);
        goto error;
    }

    file_size = get_opened_file_size (fp);
    if (file_size == 0) {
        _MG_PRINTF ("FONT>QPF: empty font file: %s.\n",
                file_name);
        goto error;
    }

    qpf_info->file_size = file_size;

    if ((qpf_info->height 
                = fontGetHeightFromName (font_name)) == -1) {
        _MG_PRINTF ("FONT>QPF: Invalid font name (height): %s.\n",
                font_name);
        goto error;
    }

    if ((qpf_info->width 
                = fontGetWidthFromName (font_name)) == -1) {
        _MG_PRINTF ("FONT>QPF: Invalid font name (width): %s.\n",
                font_name);
        goto error;
    }

#ifdef HAVE_MMAP
    qpf_info->fm = (QPFMETRICS*) mmap (0, file_size,
            PROT_READ, MAP_SHARED, fileno(fp), 0);

    if (!qpf_info->fm || qpf_info->fm == (QPFMETRICS *)MAP_FAILED)
        goto error;
#else
    qpf_info->fm = calloc (1, file_size);
    if (qpf_info->fm == NULL)
        goto error;

    fread (qpf_info->fm, sizeof(char), file_size, fp);
#endif

    qpf_info->tree = calloc (1, sizeof (QPF_GLYPHTREE));

    data = (uchar*)qpf_info->fm;
    data += sizeof (QPFMETRICS);
    build_glyph_tree (qpf_info->tree, &data);
    fclose (fp);

    return qpf_info;

error:
    if (fp)
        fclose (fp);
    free (qpf_info);
    return NULL;
}

static void unload_font_data (void* data)
{
    QPFINFO* qpf_info = (QPFINFO*)data;

    if (qpf_info->file_size == 0)
        return;

    clear_glyph_tree (qpf_info->tree);
    free (qpf_info->tree);

#ifdef HAVE_MMAP
    munmap (qpf_info->fm, qpf_info->file_size);
#else
    free (qpf_info->fm);
#endif

    free (qpf_info);
}


/*************** QPF font operations *********************************/
static QPF_GLYPH* get_glyph (QPF_GLYPHTREE* tree, unsigned int ch)
{
    if (ch < tree->min) {

        if (!tree->less)
            return NULL;

        return get_glyph (tree->less, ch);
    } else if ( ch > tree->max ) {

        if (!tree->more) {
            return NULL;
        }
        return get_glyph (tree->more, ch);
    }

    return &tree->glyph [ch - tree->min];
}

static QPF_GLYPHMETRICS def_metrics = {1, 8, 2, 0, 0, 8, 0};
static unsigned char def_bitmap [] = {0xFE, 0x7F};

static QPF_GLYPHMETRICS def_smooth_metrics = {8, 8, 2, 0, 0, 8, 0};
static unsigned char def_smooth_bitmap [] = 
{
        0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00
};

static QPF_GLYPH def_glyph = {&def_metrics, def_bitmap};
static QPF_GLYPH def_smooth_glyph = {&def_smooth_metrics, def_smooth_bitmap};

static DWORD get_glyph_type (LOGFONT* logfont, DEVFONT* devfont)
{
    if (QPFONT_INFO_P (devfont)->fm->flags & FLAG_MODE_SMOOTH)
        return DEVFONTGLYPHTYPE_GREYBMP;
    else
        return DEVFONTGLYPHTYPE_MONOBMP;
}

static int get_ave_width (LOGFONT* logfont, DEVFONT* devfont)
{
    return QPFONT_INFO_P (devfont)->width 
            * GET_DEVFONT_SCALE (logfont, devfont);
}

static int get_max_width (LOGFONT* logfont, DEVFONT* devfont)
{
    return QPFONT_INFO_P (devfont)->fm->maxwidth
            * GET_DEVFONT_SCALE (logfont, devfont);
}

static int get_font_height (LOGFONT* logfont, DEVFONT* devfont)
{
    return (QPFONT_INFO_P (devfont)->fm->ascent 
                    + QPFONT_INFO_P (devfont)->fm->descent) 
            * GET_DEVFONT_SCALE (logfont, devfont);
}

static int get_font_size (LOGFONT* logfont, DEVFONT* devfont, int expect)
{
    int height = QPFONT_INFO_P (devfont)->fm->ascent + 
            QPFONT_INFO_P (devfont)->fm->descent;
    unsigned short scale = 1;

    if (logfont->style & FS_OTHER_AUTOSCALE)
        scale = font_GetBestScaleFactor (height, expect);

    SET_DEVFONT_SCALE (logfont, devfont, scale);

    return height * scale;
}

static int get_font_ascent (LOGFONT* logfont, DEVFONT* devfont)
{
    return QPFONT_INFO_P (devfont)->fm->ascent
            * GET_DEVFONT_SCALE (logfont, devfont);
}

static int get_font_descent (LOGFONT* logfont, DEVFONT* devfont)
{
    return QPFONT_INFO_P (devfont)->fm->descent
            * GET_DEVFONT_SCALE (logfont, devfont);
}

static const void* get_glyph_monobitmap (LOGFONT* logfont, DEVFONT* devfont,
            const Glyph32 glyph_value, int* pitch, unsigned short* scale)
{
    unsigned int uc16;
    QPF_GLYPH* glyph;

    if(devfont->charset_ops->conv_to_uc32)
        uc16 = (*devfont->charset_ops->conv_to_uc32) (glyph_value);
    else
        uc16 = glyph_value;
    
    glyph = get_glyph (QPFONT_INFO_P (devfont)->tree, uc16);

    if (glyph == NULL) {
        if (uc16 < 0x80 && logfont->sbc_devfont != devfont) {   /* ASCII */
            unsigned char ascii_ch = uc16;
            return logfont->sbc_devfont->font_ops->get_glyph_monobitmap (logfont,
                            logfont->sbc_devfont, ascii_ch, pitch, scale);
        }
        else
            glyph = &def_glyph;
    }

    if (QPFONT_INFO_P (devfont)->fm->flags & FLAG_MODE_SMOOTH) {
        glyph = &def_glyph;
    }

    if (pitch){
        *pitch = (glyph->metrics->width + 7) >> 3;
    }

    if (scale)
        *scale = GET_DEVFONT_SCALE (logfont, devfont);
    return glyph->data;
}

static const void* get_glyph_greybitmap (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 glyph_value, int* pitch, unsigned short* scale)
{
    unsigned int uc16;
    QPF_GLYPH* glyph;

    if (scale)
        *scale = GET_DEVFONT_SCALE (logfont, devfont);

    if (!(QPFONT_INFO_P (devfont)->fm->flags & FLAG_MODE_SMOOTH)) {
        return NULL;
    }

    if(devfont->charset_ops->conv_to_uc32)
        uc16 = (*devfont->charset_ops->conv_to_uc32) (glyph_value);
    else
        uc16 = glyph_value;
    
    glyph = get_glyph (QPFONT_INFO_P (devfont)->tree, uc16);

    if (glyph == NULL) {
        glyph = &def_smooth_glyph;
    }

    *pitch = glyph->metrics->linestep;
    return glyph->data;
}

static int get_glyph_bbox (LOGFONT* logfont, DEVFONT* devfont, 
                Glyph32 glyph_value, 
                int* px, int* py, int* pwidth, int* pheight)
{
    unsigned int uc16;
    QPF_GLYPH* glyph;
    unsigned short scale = GET_DEVFONT_SCALE (logfont, devfont);

    if(devfont->charset_ops->conv_to_uc32)
        uc16 = (*devfont->charset_ops->conv_to_uc32) (glyph_value);
    else
        uc16 = glyph_value;
    
    glyph = get_glyph (QPFONT_INFO_P (devfont)->tree, uc16);

    if (glyph == NULL) {
        if (uc16 < 0x80 && logfont->sbc_devfont != devfont) {   /* ASCII */
            unsigned char ascii_ch = uc16;
            if (logfont->sbc_devfont->font_ops->get_glyph_bbox) {
                return logfont->sbc_devfont->font_ops->get_glyph_bbox (logfont,
                            logfont->sbc_devfont, ascii_ch,
                            px, py, pwidth, pheight);
            }
            else {
                int width, height, ascent;
                logfont->sbc_devfont->font_ops->get_glyph_advance 
                        (logfont, logfont->sbc_devfont, glyph_value, &width, 0);
                height = logfont->sbc_devfont->font_ops->get_font_height 
                        (logfont, logfont->sbc_devfont);
                ascent = logfont->sbc_devfont->font_ops->get_font_ascent
                        (logfont, logfont->sbc_devfont);

                if (pwidth) *pwidth = width;
                if (pheight) *pheight = height;
                if (py) *py  -= ascent;

                return width;
            }
        }
        else {
            glyph = &def_glyph;
            def_metrics.bearingy = (QPFONT_INFO_P (devfont)->fm->ascent+2)/2;
        }
    }

    if (pwidth) *pwidth = glyph->metrics->width * scale;
    if (pheight) *pheight = glyph->metrics->height * scale;

    if (px) *px += glyph->metrics->bearingx * scale;
    if (py) *py -= glyph->metrics->bearingy * scale;
    return glyph->metrics->width * scale;
}

static int get_glyph_advance (LOGFONT* logfont, DEVFONT* devfont, 
                Glyph32 glyph_value, int* px, int* py)
{
    unsigned int uc16;
    QPF_GLYPH* glyph;
    int advance;

    if(devfont->charset_ops->conv_to_uc32)
        uc16 = (*devfont->charset_ops->conv_to_uc32) (glyph_value);
    else
        uc16 = glyph_value;

    glyph = get_glyph (QPFONT_INFO_P (devfont)->tree, uc16);

    if (glyph == NULL) {
        if (uc16 < 0x80 && logfont->sbc_devfont != devfont) {   /* ASCII */
            unsigned char ascii_ch = uc16;
            if (logfont->sbc_devfont->font_ops->get_glyph_advance) {
                return logfont->sbc_devfont->font_ops->
                        get_glyph_advance (logfont, logfont->sbc_devfont, 
                            ascii_ch, px, py);
            }
        }
        else
            glyph = &def_glyph;
    }

    advance = glyph->metrics->advance * GET_DEVFONT_SCALE (logfont, devfont);
    *px += advance;

    return advance;
}

static DEVFONT* new_instance (LOGFONT* logfont, DEVFONT* devfont,
               BOOL need_sbc_font)
{
    if (QPFONT_INFO_P (devfont)->fm->flags & FLAG_MODE_SMOOTH) {
        logfont->style &= ~FS_WEIGHT_SUBPIXEL;
        logfont->style |=  FS_WEIGHT_BOOK;
        logfont->style &= ~FS_WEIGHT_LIGHT;
    }

    return devfont;
}

static BOOL is_glyph_existed (LOGFONT* logfont, DEVFONT* devfont, Glyph32 glyph_value)
{
    unsigned int uc16;
    QPF_GLYPH* glyph;

    if(devfont->charset_ops->conv_to_uc32)
        uc16 = (*devfont->charset_ops->conv_to_uc32) (glyph_value);
    else
        uc16 = glyph_value;
    
    glyph = get_glyph (QPFONT_INFO_P (devfont)->tree, uc16);

    if (glyph == NULL) {
        if (uc16 < 0x80 && logfont->sbc_devfont != devfont)   /* ASCII */
            return logfont->sbc_devfont->font_ops->is_glyph_existed (logfont,
                                    logfont->sbc_devfont, glyph_value);
        else
            return FALSE;
    }
    return TRUE;
}

static int is_rotatable (LOGFONT* logfont, DEVFONT* devfont, int rot_desired)
{
    return 0;
}

/**************************** Global data ************************************/
FONTOPS __mg_qpf_ops = {
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

    NULL,
    new_instance,
    NULL,
    is_rotatable,
    load_font_data,
    unload_font_data
};

#endif  /* _MGFONT_QPF */

