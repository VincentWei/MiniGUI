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
** varbitmap.c: The Var Bitmap Font operation set.
**
** Create date: 2000/06/13
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"

#ifdef _MGFONT_VBF

#include "minigui.h"
#include "gdi.h"
#include "endianrw.h"
#include "misc.h"

#ifdef HAVE_MMAP
    #include <sys/mman.h>
#endif

#include "devfont.h"
#include "varbitmap.h"
#include "charset.h"
#include "fontname.h"

#define HEADER_LEN 68

#define VARFONT_INFO_P(devfont) ((VBFINFO*)(devfont->data))
#define VARFONT_INFO(devfont) ((VBFINFO*)(devfont.data))

typedef struct _FILE_LAYOUT {
    Uint32 off_bboxs; 
    Uint32 len_bboxs; 

    Uint32 off_advxs; 
    Uint32 len_advxs; 

    Uint32 off_advys; 
    Uint32 len_advys; 

    Uint32 off_bit_offs; 
    Uint32 len_bit_offs; 

    Uint32 off_bits; 
    Uint32 len_bits; 

    Uint32 font_size; 
} FILE_LAYOUT;

typedef struct _FONT_PROPT {
    char font_name [LEN_DEVFONT_NAME + 1];
    char max_width; 
    char ave_width; 
    char height; 
    char descent; 

    int first_glyph; 
    int last_glyph; 
    int def_glyph; 
} FONT_PROPT;

static DWORD get_glyph_type (LOGFONT* logfont, DEVFONT* devfont)
{
    return DEVFONTGLYPHTYPE_MONOBMP;
}

static int get_ch_width (LOGFONT* logfont, DEVFONT* devfont, 
                Glyph32 glyph_value)
{
    VBFINFO* vbf_info = VARFONT_INFO_P (devfont);
    unsigned short scale = GET_DEVFONT_SCALE (logfont, devfont);

    /*monospace font*/
    if (vbf_info->bits_offset == NULL)
        return vbf_info->max_width * scale;

    return vbf_info->advance_x [glyph_value - vbf_info->first_glyph] * scale;
}

static int get_max_width (LOGFONT* logfont, DEVFONT* devfont)
{
   return VARFONT_INFO_P (devfont)->max_width * GET_DEVFONT_SCALE (logfont, devfont);
}

static int get_ave_width (LOGFONT* logfont, DEVFONT* devfont)
{
    return VARFONT_INFO_P(devfont)->ave_width * GET_DEVFONT_SCALE (logfont, devfont);
}

static int get_font_height (LOGFONT* logfont, DEVFONT* devfont)
{
    return VARFONT_INFO_P (devfont)->height * GET_DEVFONT_SCALE (logfont, devfont);
}

static int get_font_size (LOGFONT* logfont, DEVFONT* devfont, int expect)
{
    int height = VARFONT_INFO_P (devfont)->height;
    unsigned short scale = 1;

    if (logfont->style & FS_OTHER_AUTOSCALE)
        scale = font_GetBestScaleFactor (height, expect);

    SET_DEVFONT_SCALE (logfont, devfont, scale);

    return height * scale;
}

static int get_font_ascent (LOGFONT* logfont, DEVFONT* devfont)
{
    int ascent = VARFONT_INFO_P (devfont)->height 
            - VARFONT_INFO_P (devfont)->descent;

    return ascent * GET_DEVFONT_SCALE (logfont, devfont);
}

static int get_font_descent (LOGFONT* logfont, DEVFONT* devfont)
{
    return VARFONT_INFO_P (devfont)->descent 
            * GET_DEVFONT_SCALE (logfont, devfont);
}


static int get_glyph_bbox (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 glyph_value,
            int* px, int* py, int* pwidth, int* pheight)
{
    int tempint;
    VBFINFO* vbf_info = VARFONT_INFO_P (devfont);
    unsigned short scale = GET_DEVFONT_SCALE (logfont, devfont);

    if (glyph_value < vbf_info->first_glyph || glyph_value > vbf_info->last_glyph)
        glyph_value = vbf_info->def_glyph;

    /*have bbox*/
    if (vbf_info->bbox) {
        int offset;

        offset = glyph_value - vbf_info->first_glyph;

        if (px) {
            tempint = vbf_info->bbox[offset].x;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
            tempint = ArchSwap32(tempint);
#endif
            *px += tempint * scale;
        }
        if (py) {
            tempint = vbf_info->bbox[offset].y;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
            tempint = ArchSwap32(tempint);
#endif
            *py -= tempint * scale;
        }
        if (pheight) {
            tempint = vbf_info->bbox[offset].h;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
            tempint = ArchSwap32(tempint);
#endif
            *pheight = tempint * scale;
        }

        tempint = vbf_info->bbox[offset].w;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
        tempint = ArchSwap32(tempint);
#endif
        tempint *= scale;

        if (pwidth) *pwidth = tempint;
    }
    else {
        tempint = get_ch_width (logfont, devfont, glyph_value);

        /* width * scale inner get_ch_width. */
        if (pwidth)
            *pwidth     = tempint;
        if (pheight)
            *pheight    = vbf_info->height * scale;
        if (py)
            *py        -= get_font_ascent(logfont, devfont);
    }

    return tempint;
}

static int get_glyph_advance (LOGFONT* logfont, DEVFONT* devfont,
                Glyph32 glyph_value, int* px, int* py)
{
    int advance;
    VBFINFO* vbf_info = VARFONT_INFO_P (devfont);

    if (glyph_value < vbf_info->first_glyph || glyph_value > vbf_info->last_glyph)
        glyph_value = vbf_info->def_glyph;

    advance = get_ch_width(logfont, devfont, glyph_value);
    *px += advance;
    return advance;
}

static const void* get_glyph_monobitmap (LOGFONT* logfont, DEVFONT* devfont,
            const Glyph32 glyph_value, int* pitch, unsigned short* scale)
{
    int offset;
    Glyph32 eff_value = glyph_value;
    Glyph32 first_value;
    Glyph32 last_value;
    VBFINFO* vbf_info = VARFONT_INFO_P (devfont);

    first_value = vbf_info->first_glyph;
    last_value = vbf_info->last_glyph;

    if (glyph_value < first_value || glyph_value > last_value)
        eff_value = vbf_info->def_glyph;

    if (vbf_info->bits_offset == NULL)
        offset = (((size_t)vbf_info->max_width + 7) >> 3) * vbf_info->height 
                    * (eff_value - first_value);
    else {
        offset = vbf_info->bits_offset [eff_value - vbf_info->first_glyph];
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
        if (vbf_info->font_size)
            offset = ArchSwap16 (offset);
#endif
    }
    
    if(pitch){
        int width = 0;
        if (vbf_info->bbox) {
            width = vbf_info->bbox[glyph_value - vbf_info->first_glyph].w;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
            width = ArchSwap32(width);
#endif
            //get_glyph_bbox (logfont, devfont,
            //    glyph_value, NULL, NULL, &width, NULL);
        }
        else{
            width = get_ch_width (logfont, devfont, glyph_value) / GET_DEVFONT_SCALE (logfont, devfont);
        }
        *pitch = ((size_t)width + 7) >> 3;
    }

    if (scale)
        *scale = GET_DEVFONT_SCALE (logfont, devfont);

#if 0
    {
        int bbox_w; 
        int bbox_h;
        get_glyph_bbox (logfont, devfont, glyph_value,
                NULL,  NULL, &bbox_w, &bbox_h);
        print_bitmap (vbf_info->all_glyph_bits + offset, 
                bbox_w, bbox_h, *pitch);
    }
#endif

    return vbf_info->all_glyph_bits + offset;
}

static BOOL is_glyph_existed (LOGFONT* logfont, DEVFONT* devfont, Glyph32 glyph_value)
{
    Glyph32 first_value;
    Glyph32 last_value;
    
    VBFINFO* vbf_info = VARFONT_INFO_P (devfont);

    first_value = vbf_info->first_glyph;
    last_value = vbf_info->last_glyph;

    if (glyph_value < first_value || glyph_value > last_value)
        return FALSE;
    else
        return TRUE;
}

static int is_rotatable (LOGFONT* logfont, DEVFONT* devfont, int rot_desired)
{
    return 0;
}


#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
static void swap_intdata (Uint32* data, int num)
{
    while (num)
    {
        *data = ArchSwap32 (*data);
        data++;
        num--;
    }

}
#endif

static void* load_font_data (const char* fontname, const char* filename)
{
    FILE* fp = NULL;

    char version[VBF_LEN_VERSION_INFO + 1];
    char vender[VBF_LEN_VENDER_INFO+1];

    FILE_LAYOUT layout;
    const FONT_PROPT* propt;
    char* temp = NULL;
    Uint16 len_header;

    VBFINFO* info = (VBFINFO*) calloc (1, sizeof(VBFINFO));

    // Open font file and read information of font.
    if (!(fp = fopen (filename, "rb")))
        goto error;

    if (fread (version, 1, VBF_LEN_VERSION_INFO, fp) < VBF_LEN_VERSION_INFO)
        goto error;
    version [VBF_LEN_VERSION_INFO] = '\0'; 

    if (strcmp (version, VBF_VERSION3) != 0) {
        _ERR_PRINTF ("FONT>VBF: Error on loading vbf: %s, version: %s,"
                         " invalid version.\n", filename, version);
        goto error;
    }

    if (fread(vender, 1, VBF_LEN_VENDER_INFO, fp) < VBF_LEN_VENDER_INFO)
        goto error;

    if (fread(&len_header, sizeof(short), 1, fp) < 1)
        goto error;

    if (fread (&layout, sizeof (FILE_LAYOUT), 1, fp) < 1)
        goto error;

#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    len_header = ArchSwap16 (len_header);
    swap_intdata ((Uint32*)&layout, sizeof(layout)/sizeof(int));
#endif

#ifdef HAVE_MMAP
    if ((temp = mmap (NULL, layout.font_size, PROT_READ, MAP_SHARED, 
            fileno(fp), 0)) == MAP_FAILED)
        goto error;
    temp += len_header;
#else
    layout.font_size -= len_header;
    if ((temp = (char *)malloc (layout.font_size)) == NULL)
        goto error;
    if (fread (temp, sizeof (char), layout.font_size, fp) < layout.font_size)
        goto error;
#endif

    fclose (fp);
    fp = NULL;
    propt = (const FONT_PROPT*) temp;

    strcpy (info->ver_info, "3.0");
    info->name = (char*)propt;
    info->max_width = propt->max_width;
    info->ave_width = propt->ave_width;
    info->height = propt->height;
    info->descent = propt->descent;

    info->font_size = layout.font_size;

#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    info->first_glyph = ArchSwap32 (propt->first_glyph);
    info->last_glyph = ArchSwap32 (propt->last_glyph);
    info->def_glyph = ArchSwap32 (propt->def_glyph);
#else
    info->first_glyph = propt->first_glyph;
    info->last_glyph = propt->last_glyph;
    info->def_glyph = propt->def_glyph;
#endif

    info->bbox = (const VBF_BBOX*)
           (layout.len_bboxs ? temp+layout.off_bboxs-HEADER_LEN : NULL);
    info->advance_x = (const char*)
           (layout.len_advxs ? temp+layout.off_advxs-HEADER_LEN : NULL);
    info->advance_y = (const char*)
           (layout.len_advys ? temp+layout.off_advys-HEADER_LEN : NULL);
    info->bits_offset = (const Uint32*)
            (layout.len_bit_offs ? temp+layout.off_bit_offs-HEADER_LEN : NULL);

    if (layout.len_bits <= 0)
        goto error;
    info->all_glyph_bits = (const unsigned char*)(temp + 
                            layout.off_bits-HEADER_LEN);
#if 0
    if (real_fontname) {
        strncpy (real_fontname, info->name, LEN_DEVFONT_NAME);
        real_fontname [LEN_DEVFONT_NAME] = '\0';
    }
#endif
    
    return info;

error:
#ifdef HAVE_MMAP
    if (temp)
        munmap (temp, layout.font_size);
#else
    free (temp);
#endif

    if (fp)
        fclose (fp);
    free (info);
    return NULL;
}

static void unload_font_data (void* data)
{
    VBFINFO* info = (VBFINFO*) data;
#ifdef HAVE_MMAP
    /*FIXME*/
    if (info->name)
        munmap ((void*)(info->name)-HEADER_LEN, info->font_size);
#else
    free ((void*)info->name);
#endif

    free (info);
}

FONTOPS __mg_vbf_ops = {
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
    NULL,
    NULL,

    NULL,
    NULL,
    NULL,
    is_rotatable,
    load_font_data,
    unload_font_data
};

#endif /* _MGFONT_VBF */

