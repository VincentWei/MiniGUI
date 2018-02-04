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
** upf.c: The Unicode Prerendered Font operation set.
**
** Create date: 2008/01/26 mgwang
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "misc.h"

#ifdef _MGFONT_UPF

#ifdef HAVE_MMAP
    #include <sys/mman.h>
#endif

#include "devfont.h"
#include "charset.h"
#include "upf.h"
#include "fontname.h"

#define UPFONT_INFO_P(devfont) ((UPFINFO*)(devfont->data))
#if 0
static void* load_font_data (char* font_name, char* file_name
        char* real_font_name)
#endif
static void* load_font_data (const char* font_name, const char* file_name)
{
    FILE* fp = NULL;
    UPFV1_FILE_HEADER * filehead;
    long file_size;
    UPFINFO* upf_info = (UPFINFO*) calloc (1, sizeof(UPFINFO));
    int tmp;

    if ((tmp = fontGetWidthFromName (font_name)) == -1) {
        _MG_PRINTF ("FONT>UPF: Invalid font name (width): %s.\n", font_name);
        goto error;
    }
    upf_info->width = (Uint8)tmp;

    if ((tmp = fontGetHeightFromName (font_name)) == -1) {
        _MG_PRINTF ("FONT>UPF: Invalid font name (height): %s.\n", font_name);
        goto error;
    }
    upf_info->height = (Uint8)tmp;

    if ((fp = fopen (file_name, "rb")) == NULL) {
        _MG_PRINTF ("FONT>UPF: can't open font file: %s\n", file_name);
        goto error;
    }

    if ((file_size = get_opened_file_size (fp)) <= 0) {
        _MG_PRINTF ("FONT>UPF: empty font file: %s\n", file_name);
        goto error;
    }

    upf_info->file_size = file_size;

#ifdef HAVE_MMAP
    upf_info->root_dir =  mmap( 0, file_size, PROT_READ, 
            MAP_SHARED, fileno(fp), 0 );

    if (!upf_info->root_dir || upf_info->root_dir == MAP_FAILED)
        goto error;
#else
    upf_info->root_dir = malloc (file_size);
    if (upf_info->root_dir == NULL)
        goto error;

    fread (upf_info->root_dir, 1, file_size, fp);
#endif

    filehead = (UPFV1_FILE_HEADER *)upf_info->root_dir;
    if (filehead->endian !=0x1234)
    {
#ifdef HAVE_MMAP
        munmap (upf_info->root_dir, upf_info->file_size);
#else
        free (upf_info->root_dir);
#endif
        goto error;
    }
    
#if 0
    if (real_font_name) {
        strncpy (real_font_name, filehead->font_name, LEN_DEVFONT_NAME_MAX);
        real_font_name [LEN_DEVFONT_NAME_MAX] = '\0';
    }
#endif

    fclose (fp);
    return upf_info;

error:
    if (fp)
        fclose (fp);
    free (upf_info);
    return NULL;
}

static void unload_font_data (void* data)
{
#ifdef HAVE_MMAP
    munmap (((UPFINFO*) data)->root_dir, ((UPFINFO*) data)->file_size);
#else
    free (((UPFINFO*) data)->root_dir);
#endif

    free (((UPFINFO*) data));
}

static DWORD get_glyph_type (LOGFONT* logfont, DEVFONT* devfont)
{
    Uint8* p_upf = (Uint8 *)UPFONT_INFO_P (devfont)->root_dir;

    if (((UPFV1_FILE_HEADER *)(p_upf))->mono_bitmap)
        return DEVFONTGLYPHTYPE_MONOBMP;
    else
        return DEVFONTGLYPHTYPE_GREYBMP;
}

static int get_ave_width (LOGFONT* logfont, DEVFONT* devfont)
{
    UPFV1_FILE_HEADER * p_upf;
    
    p_upf = (UPFV1_FILE_HEADER *)UPFONT_INFO_P (devfont)->root_dir;
    return p_upf->width * GET_DEVFONT_SCALE (logfont, devfont);
    
}
static int get_max_width (LOGFONT* logfont, DEVFONT* devfont)
{
    UPFV1_FILE_HEADER * p_upf;
    
    p_upf = (UPFV1_FILE_HEADER *)UPFONT_INFO_P (devfont)->root_dir;
    return p_upf->max_width * GET_DEVFONT_SCALE (logfont, devfont);
}

static int get_font_height (LOGFONT* logfont, DEVFONT* devfont)
{
    UPFV1_FILE_HEADER * p_upf;
    
    p_upf = (UPFV1_FILE_HEADER *)UPFONT_INFO_P (devfont)->root_dir;
    return (p_upf->ascent + p_upf->descent) * GET_DEVFONT_SCALE (logfont, devfont);
}

static int get_font_size (LOGFONT* logfont, DEVFONT* devfont, int expect)
{
    UPFV1_FILE_HEADER * p_upf;
    int       height;
    unsigned short scale = 1;
    
    p_upf = (UPFV1_FILE_HEADER *)UPFONT_INFO_P (devfont)->root_dir;
#if 0
    height = p_upf->ascent + p_upf->descent;
#else
    /* use height to do the matching */
    height = p_upf->height;
#endif

    if (logfont->style & FS_OTHER_AUTOSCALE)
        scale = font_GetBestScaleFactor (height, expect);
    
    SET_DEVFONT_SCALE (logfont, devfont, scale);

    return height * scale;
}

static int get_font_ascent (LOGFONT* logfont, DEVFONT* devfont)
{
    UPFV1_FILE_HEADER * p_upf;
    
    p_upf = (UPFV1_FILE_HEADER *)UPFONT_INFO_P (devfont)->root_dir;
    return p_upf->ascent * GET_DEVFONT_SCALE (logfont, devfont);
}

static int get_font_descent (LOGFONT* logfont, DEVFONT* devfont)
{
    UPFV1_FILE_HEADER * p_upf;
    
    p_upf = (UPFV1_FILE_HEADER *)UPFONT_INFO_P (devfont)->root_dir;
    return p_upf->descent * GET_DEVFONT_SCALE (logfont, devfont);
}

static UPFGLYPH* get_glyph (Uint8 *upf_root, UPFNODE * tree, unsigned int ch)
{
    if (ch < tree->min) {
        if (!tree->less_offset)
            return NULL;

        return get_glyph (upf_root, (UPFNODE*)(upf_root + tree->less_offset), ch);
    }
    else if ( ch > tree->max ) {

        if (!tree->more_offset) {
            return NULL;
        }
        return get_glyph (upf_root, (UPFNODE*)(upf_root + tree->more_offset), ch);
    }

    return (UPFGLYPH*)(upf_root + tree->glyph_offset + sizeof (UPFGLYPH) * (ch - tree->min));
}

static UPFGLYPH def_glyph = {0, 0, 8, 2, 1, 0, 8, 0, 0};
static unsigned char def_bitmap [] = {0xFE, 0x7F};

static UPFGLYPH def_smooth_glyph = {0, 0, 8, 2, 8, 0, 8, 0, 0};
static unsigned char def_smooth_bitmap [] =
{
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00
};

#if 0
static void 
print_bitmap(char* bits, int width, int height, int pitch)
{
    int y = 0;
    int x = 0;
    char* p_line_head;
    char* p_cur_char;
    
    printf ("width = %d\n", width);
    printf ("height = %d\n", height);
    printf ("pitch = %d\n", pitch);

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
#endif

static const void* get_glyph_monobitmap (LOGFONT* logfont, DEVFONT* devfont,
        const Glyph32 glyph_value, int* pitch, unsigned short* scale)
{
    unsigned int uc16;
    UPFGLYPH* glyph;
    Uint8   * p_upf;
    
    p_upf = (Uint8 *)UPFONT_INFO_P (devfont)->root_dir;

#ifdef _MGCHARSET_UNICODE
    if(devfont->charset_ops->conv_to_uc32)
        uc16 = (*devfont->charset_ops->conv_to_uc32) (glyph_value);
    else
#endif
        uc16 = glyph_value;

    glyph = get_glyph (p_upf, (UPFNODE *)(((UPFV1_FILE_HEADER *)p_upf)->off_nodes + p_upf), uc16);

    if (glyph == NULL) {
        if (uc16 < 0x80 && logfont->sbc_devfont != devfont) {   /* ASCII */
            unsigned char ascii_ch = uc16;
            return logfont->sbc_devfont->font_ops->get_glyph_monobitmap (logfont,
                            logfont->sbc_devfont, ascii_ch, pitch, scale);
        }
        else
            glyph = &def_glyph;
    }

    /*this font is not monobitmap font, set glyph to def_glyph*/
    if (! ((UPFV1_FILE_HEADER *)p_upf)->mono_bitmap) {
        glyph = &def_glyph;
    }

    if (pitch) {
        *pitch = (glyph->width + 7) >> 3;
    }

    if (scale)
        *scale = GET_DEVFONT_SCALE (logfont, devfont);

    if(glyph == &def_glyph)
        return def_bitmap;

    return (void*)(p_upf+glyph->bitmap_offset);

}

static const void* get_glyph_greybitmap (LOGFONT* logfont, DEVFONT* devfont,
            Glyph32 glyph_value, int* pitch, unsigned short* scale)
{
    unsigned int uc16;
    UPFGLYPH* glyph;
    Uint8   * p_upf;
    
    p_upf = (Uint8 *)UPFONT_INFO_P (devfont)->root_dir;

    if (scale)
        *scale = GET_DEVFONT_SCALE (logfont, devfont);

    /*if it is a monobitmap font, return NULL*/
    if (((UPFV1_FILE_HEADER *)(p_upf))->mono_bitmap) {
        return NULL;
    }

#ifdef _MGCHARSET_UNICODE
    if(devfont->charset_ops->conv_to_uc32)
        uc16 = (*devfont->charset_ops->conv_to_uc32) (glyph_value);
    else
#endif
        uc16 = glyph_value;

    glyph = get_glyph (p_upf, (UPFNODE *)(((UPFV1_FILE_HEADER *)p_upf)->off_nodes + p_upf), uc16);

    if (glyph == NULL) {
        glyph = &def_smooth_glyph;
    }

    *pitch = glyph->pitch;

    if(glyph == &def_smooth_glyph)
        return def_smooth_bitmap;

    return glyph->bitmap_offset + p_upf;

}

static int get_glyph_bbox (LOGFONT* logfont, DEVFONT* devfont, 
                Glyph32 glyph_value, 
                int* px, int* py, int* pwidth, int* pheight)
{
    unsigned int uc16;
    UPFGLYPH* glyph;
    unsigned short scale = GET_DEVFONT_SCALE (logfont, devfont);
    Uint8   * p_upf;
    
    p_upf = (Uint8 *)UPFONT_INFO_P (devfont)->root_dir;

#ifdef _MGCHARSET_UNICODE
    if(devfont->charset_ops->conv_to_uc32)
        uc16 = (*devfont->charset_ops->conv_to_uc32) (glyph_value);
    else
#endif
        uc16 = glyph_value;

    glyph = get_glyph (p_upf, (UPFNODE *)(((UPFV1_FILE_HEADER *)p_upf)->off_nodes + p_upf), uc16);

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
            def_glyph.bearingy = ( ((UPFV1_FILE_HEADER *)p_upf)->ascent + 2)/2;
        }
    }

    if (pwidth) *pwidth = glyph->width * scale;
    if (pheight) *pheight = glyph->height * scale;

    if (px) *px += glyph->bearingx * scale;
    if (py) *py -= glyph->bearingy * scale;
    return glyph->width * scale;
}

static int get_glyph_advance (LOGFONT* logfont, DEVFONT* devfont, 
                Glyph32 glyph_value, int* px, int* py)
{
    unsigned int uc16;
    UPFGLYPH* glyph;
    Uint8   * p_upf;
    int advance;
    
    p_upf = (Uint8 *)UPFONT_INFO_P (devfont)->root_dir;

#ifdef _MGCHARSET_UNICODE
    if(devfont->charset_ops->conv_to_uc32)
        uc16 = (*devfont->charset_ops->conv_to_uc32) (glyph_value);
    else
#endif
        uc16 = glyph_value;

    glyph = get_glyph (p_upf, (UPFNODE *)(((UPFV1_FILE_HEADER *)p_upf)->off_nodes + p_upf), uc16);

    if (glyph == NULL) {
        if (uc16 < 0x80 && logfont->sbc_devfont != devfont) {   /* ASCII */
            unsigned char ascii_ch = uc16;
            if (logfont->sbc_devfont->font_ops->get_glyph_advance) {
                return logfont->sbc_devfont->font_ops->get_glyph_advance (
                        logfont, logfont->sbc_devfont, ascii_ch, px, py);
            }
        }
        else
            glyph = &def_glyph;
    }

    advance = glyph->advance
            * GET_DEVFONT_SCALE (logfont, devfont);
    *px += advance;

    return advance;
}

static DEVFONT* new_instance (LOGFONT* logfont, DEVFONT* devfont,
               BOOL need_sbc_font)
{
    Uint8   * p_upf;
    
    p_upf = (Uint8 *)UPFONT_INFO_P (devfont)->root_dir;

    /*if the devfont is a gray bitmap font,
     *set logfont to weight book;
     **/
    if (!((UPFV1_FILE_HEADER*)p_upf)->mono_bitmap) {
        logfont->style &= ~FS_WEIGHT_SUBPIXEL;
        logfont->style |=  FS_WEIGHT_BOOK;
        logfont->style &= ~FS_WEIGHT_LIGHT;
    }

    return devfont;

}

static BOOL is_glyph_existed (LOGFONT* logfont, DEVFONT* devfont, Glyph32 glyph_value)
{
    unsigned int uc16;
    UPFGLYPH* glyph;
    Uint8   * p_upf;
    
    p_upf = (Uint8 *)UPFONT_INFO_P (devfont)->root_dir;

#ifdef _MGCHARSET_UNICODE
    if(devfont->charset_ops->conv_to_uc32)
        uc16 = (*devfont->charset_ops->conv_to_uc32) (glyph_value);
    else
#endif
        uc16 = glyph_value;

    glyph = get_glyph (p_upf, (UPFNODE *)(((UPFV1_FILE_HEADER *)p_upf)->off_nodes + p_upf), uc16);

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

FONTOPS __mg_upf_ops = {
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

#endif  /*_MGFONT_UPF*/

