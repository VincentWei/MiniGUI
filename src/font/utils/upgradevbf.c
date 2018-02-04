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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../varbitmap.h"

#define INFO_DEBUG
#define VAL_DEBUG
#include "my_debug.h"

typedef int BOOL;
typedef unsigned int Uint32;
typedef unsigned short Uint16;

#define TRUE    1
#define FALSE   0

#define MGUI_LIL_ENDIAN  1234
#define MGUI_BIG_ENDIAN  4321

/* Assume that the host is little endian */
#define MGUI_BYTEORDER   MGUI_LIL_ENDIAN

#define VBF_VERSION         "vbf-1.0**"
#define VBF_VERSION2        "vbf-2.0**"

#define LEN_VERSION_INFO    10
#define LEN_DEVFONT_NAME    79

#define LEN_VENDER_INFO     12  

typedef struct
{
    unsigned char   version;        /* font version */
    const char*     name;           /* font name */
    unsigned char   max_width;      /* max width in pixels */
    unsigned char   ave_width;      /* average width in pixels */
    int             height;         /* height in pixels */
    int             descent;        /* pixels below the base line */
    unsigned char   first_char;     /* first character in this font */
    unsigned char   last_char;      /* last character in this font */
    unsigned char   def_char;       /* default character in this font */
    const unsigned short* offset;   /* character glyph offsets into bitmap data or NULL */
    const unsigned char*  width;    /* character widths or NULL */
    const unsigned char*  bits;     /* 8-bit right-padded bitmap data */
    unsigned int    font_size;      /* used by mmap. It should be zero for in-core vbfs. */
    int*            bbox_x;
    int*            bbox_y;
    int*            bbox_w;
    int*            bbox_h;
} OLD_VBFINFO;

typedef struct _NEW_PROPT 
{
    char font_name [LEN_DEVFONT_NAME + 1];

    char max_width;      
    char ave_width;      
    char height;         
    char descent;        

    int  first_glyph;    
    int  last_glyph;     
    int  def_glyph;     
}NEW_PROPT;

typedef struct _PART_LAYOUT {
    Uint32 offset;
    Uint32 len;
    Uint32 padding;
}PART_LAYOUT;


/********************** Load/Unload of var bitmap font ***********************/
static int LoadVarBitmapFont (const char* file, OLD_VBFINFO* info)
{
#ifdef HAVE_MMAP
    int fd;
#else
    FILE* fp;
#endif
    char* temp = NULL;
    int header_len, len_offsets, len_widths, bits_len, len_bboxs = 0;
    int font_size;
    char version[LEN_VERSION_INFO + 1];

#ifdef HAVE_MMAP
    if ((fd = open (file, O_RDONLY)) < 0)
        return 0;

    if (read (fd, version, LEN_VERSION_INFO) == -1)
        goto error;
    version[LEN_VERSION_INFO] = '\0';

    if (!strcmp (version, VBF_VERSION2))
        info->version = 2;
    else
    {
        info->version = 1;
        if (strcmp (version, VBF_VERSION))
            fprintf (stderr, "Error on loading vbf: %s, version: %s, invalid version.\n", file, version);
    }

    if (read (fd, &header_len, sizeof (int)) == -1)
        goto error;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    header_len = ArchSwap32 (header_len);
#endif

    if (read (fd, &info->max_width, sizeof (char) * 2) == -1) goto error;
    if (read (fd, &info->height, sizeof (int) * 2) == -1) goto error;

#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    info->height = ArchSwap32 (info->height);
    info->descent = ArchSwap32 (info->descent);
#endif

    if (read (fd, &info->first_char, sizeof (char) * 3) == -1) goto error;

    if (lseek (fd, header_len - (((info->version == 2)?5:4) * sizeof (int)), SEEK_SET) == -1)
        goto error;

    if (read (fd, &len_offsets, sizeof (int)) == -1
            || read (fd, &len_widths, sizeof (int)) == -1
            || (info->version == 2 && read (fd, &len_bboxs, sizeof (int)) == -1)
            || read (fd, &bits_len, sizeof (int)) == -1
            || read (fd, &font_size, sizeof (int)) == -1)
        goto error;

#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    len_offsets = ArchSwap32 (len_offsets);
    len_widths = ArchSwap32 (len_widths);
    if (info->version == 2)
        len_bboxs = ArchSwap32 (len_bboxs);
    bits_len = ArchSwap32 (bits_len);
    font_size = ArchSwap32 (font_size);
#endif

    if ((temp = mmap (NULL, font_size, PROT_READ, MAP_SHARED, 
            fd, 0)) == MAP_FAILED)
        goto error;

    temp += header_len;
    close (fd);
#else
    // Open font file and read information of font.
    if (!(fp = fopen (file, "rb")))
        return 0;

    if (fread (version, sizeof (char), LEN_VERSION_INFO, fp) < LEN_VERSION_INFO)
        goto error;
    version [LEN_VERSION_INFO] = '\0'; 

    if (!strcmp (version, VBF_VERSION2))
        info->version = 2;
    else
    {
        info->version = 1;
        if (strcmp (version, VBF_VERSION))
            fprintf (stderr, "Error on loading vbf: %s, version: %s, invalid version.\n", file, version);
    }

    if (fread (&header_len, sizeof (int), 1, fp) < 1)
        goto error;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    header_len = ArchSwap32 (header_len);
#endif

    if (fread (&info->max_width, sizeof (char), 2, fp) < 2) goto error;
    if (fread (&info->height, sizeof (int), 2, fp) < 2) goto error;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    info->height = ArchSwap32 (info->height);
    info->descent = ArchSwap32 (info->descent);
#endif
    if (fread (&info->first_char, sizeof (char), 3, fp) < 3) goto error;

    if (fseek (fp, header_len - (((info->version == 2)?5:4)*sizeof (int)), SEEK_SET) != 0)
            goto error;

    if (fread (&len_offsets, sizeof (int), 1, fp) < 1
            || fread (&len_widths, sizeof (int), 1, fp) < 1
            || (info->version == 2 && fread (&len_bboxs, sizeof (int), 1, fp) < 1)
            || fread (&bits_len, sizeof (int), 1, fp) < 1
            || fread (&font_size, sizeof (int), 1, fp) < 1)
        goto error;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    len_offsets = ArchSwap32 (len_offsets);
    len_widths = ArchSwap32 (len_widths);
    if (info->version == 2)
        len_bboxs = ArchSwap32 (len_bboxs);
    bits_len = ArchSwap32 (bits_len);
    font_size = ArchSwap32 (font_size);
#endif

    // Allocate memory for font data.
    font_size -= header_len;
    if ((temp = (char *)malloc (font_size)) == NULL)
        goto error;

    if (fseek (fp, header_len, SEEK_SET) != 0)
        goto error;

    if (fread (temp, sizeof (char), font_size, fp) < font_size)
        goto error;

    fclose (fp);
#endif
    info->name = temp;
    info->offset = (unsigned short*) (temp + LEN_DEVFONT_NAME + 1);
    info->width = (unsigned char*) (temp + LEN_DEVFONT_NAME + 1 + len_offsets);
    if (info->version == 2)
    {
        info->bbox_x = (int*) (temp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths);
        info->bbox_y = (int*) (temp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths + len_bboxs);
        info->bbox_w = (int*) (temp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths + 2*len_bboxs);
        info->bbox_h = (int*) (temp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths + 3*len_bboxs);
        info->bits = (unsigned char*) (temp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths + 4*len_bboxs);
    }
    else
        info->bits = (unsigned char*) (temp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths);
    info->font_size = font_size;

    return bits_len;

error:
#ifdef HAVE_MMAP
    if (temp)
        munmap (temp, font_size);
    close (fd);
#else
    free (temp);
    fclose (fp);
#endif
    
    return 0;
}

static void UnloadVarBitmapFont (OLD_VBFINFO* info)
{
#ifdef HAVE_MMAP
    if (info->name)
        munmap ((void*)(info->name), info->font_size);
#else
    free ((void*)info->name);
#endif
}

#define LAYOUT_STEP(cur, cur_len, pre)\
    do{\
        (cur).offset = (pre).offset + (pre).len + (pre).padding;\
        (cur).len = (cur_len);\
        (cur).padding = (4 - ((cur).len & 3)) & 3;\
    }while (0)

#define TEST_LAYOUT(layout)\
    do {\
        TEST_VAL(layout.offset, %x);\
        TEST_VAL(layout.len, %x);\
        TEST_VAL(layout.padding, %x);\
    }while (0)

static void write_layout (FILE* fp, Uint32 offset, Uint32 len)
{
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    len = ArchSwap32 (len);
    offset = ArchSwap32 (offset);
#endif

    fwrite (&offset, sizeof(Uint32), 1, fp);
    fwrite (&len, sizeof(Uint32), 1, fp);
}

static void write_propt (FILE* fp, OLD_VBFINFO* old_info)
{
    NEW_PROPT new_propt;
    strncpy (new_propt.font_name, old_info->name, LEN_DEVFONT_NAME);
    new_propt.font_name [LEN_DEVFONT_NAME] = '\0';

    new_propt.max_width = old_info->max_width;
    new_propt.ave_width = old_info->ave_width;
    new_propt.height = old_info->height;
    new_propt.descent = old_info->descent;

    new_propt.first_glyph = old_info->first_char;
    new_propt.last_glyph = old_info->last_char;
    new_propt.def_glyph = old_info->def_char;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    new_propt.first_glyph = ArchSwap32 (new_propt.first_glyph);
    new_propt.last_glyph = ArchSwap32 (new_propt.last_glyph);
    new_propt.def_glyph = ArchSwap32 (new_propt.def_glyph);
#endif

    fwrite (&new_propt, sizeof(new_propt), 1, fp);
}

static void write_bbox (FILE* fp, int* x, int* y, int* w, int* h, int char_num)
{
    VBF_BBOX bbox;
    int i;

    for (i=0; i<char_num; i++)
    {
        bbox.x = x [i];
        bbox.y = y [i];
        bbox.w = w [i];
        bbox.h = h [i];

        fwrite (&bbox, sizeof(bbox), i, fp);
    }
}

static void 
write_bits_off (FILE* fp, const unsigned short* offsets, int glyph_num)
{
    unsigned int offset;
    while (glyph_num)
    {
        offset = *offsets++;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
        offset = ArchSwap32 (offset);
#endif
        fwrite (&offset, sizeof(offset), 1, fp);
        glyph_num--;
    }
}


static void 
print_bitmap(char* bits, int width, int height)
{
    int y = 0;
    int x = 0;
    char* p_line_head;
    char* p_cur_char;
    int pitch  = (width +7) >>3;
    
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

static int upgrade (char *old_file, char* new_file)
{
    PART_LAYOUT header_layout;
    PART_LAYOUT prop_layout;
    PART_LAYOUT bbox_layout;
    PART_LAYOUT advx_layout;
    PART_LAYOUT advy_layout;
    PART_LAYOUT bits_off_layout;
    PART_LAYOUT glyph_bits_layout;

    char vender_name [LEN_VENDER_INFO];
    char ver_info [LEN_VERSION_INFO];

    char padding_ch = '\0';

    Uint32 glyph_bits_len;
    Uint16 header_len;
    Uint32 font_size;

    FILE* fp;
    int tmp;

    OLD_VBFINFO old_info;
    Uint32 glyph_num;

    memset (&old_info, 0, sizeof(old_info));
    memset (vender_name, 0, sizeof(vender_name));
    memset (ver_info, 0, sizeof(ver_info));

    strcpy (vender_name, "FMSoft");
    strcpy (ver_info, "vbf-3.0**");


    if ((glyph_bits_len = LoadVarBitmapFont (old_file, &old_info)) == 0) {
        fprintf (stderr, "vbf2c: can't load VBF file: %s\n", old_file);
        return 2;
    }

    print_bitmap (old_info.bits + old_info.offset['A'-old_info.first_char], 
            old_info.width['A'-old_info.first_char],
            old_info.height);

    glyph_num = old_info.last_char - old_info.first_char + 1;

    header_layout.offset = 0;
    header_layout.len = 5*sizeof(PART_LAYOUT) - 5*sizeof(int) + 
                sizeof(int) + sizeof(short) + sizeof(vender_name) + sizeof(ver_info);
    TEST_VAL (header_layout.len, %d);

    header_layout.padding = (4 - (header_layout.len & 3)) & 3;
    TEST_LAYOUT (header_layout);

    LAYOUT_STEP (prop_layout, sizeof(NEW_PROPT), header_layout);
    TEST_LAYOUT (prop_layout);

    /*if old vbf have bbox*/
    tmp = (old_info.bbox_x) ? glyph_num*sizeof(VBF_BBOX) : 0;
    LAYOUT_STEP (bbox_layout, tmp, prop_layout);
    TEST_LAYOUT (bbox_layout);
    /*if old vbf have different width*/

    tmp = (old_info.offset) ? glyph_num * sizeof(char) : 0;
    LAYOUT_STEP (advx_layout, tmp, bbox_layout);
    TEST_LAYOUT (advx_layout);

    /*old vfb has not advance_y*/
    LAYOUT_STEP (advy_layout, 0, advx_layout);
    TEST_LAYOUT (advy_layout);

    tmp = (old_info.offset) ? glyph_num * sizeof(Uint32) : 0;
    LAYOUT_STEP (bits_off_layout, tmp, advy_layout);
    TEST_LAYOUT (bits_off_layout);

    LAYOUT_STEP (glyph_bits_layout, glyph_bits_len, bits_off_layout);
    TEST_LAYOUT (glyph_bits_layout);

    font_size = glyph_bits_layout.offset + glyph_bits_layout.len;

    fp = fopen (new_file, "wb");

    fwrite (ver_info, sizeof(ver_info), 1, fp);
    fwrite (vender_name, sizeof(vender_name), 1, fp);

    header_len = header_layout.len;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    header_len = ArchSwap16 (header_len);
    font_size = ArchSwap32 (font_size);
#endif
    fwrite (&(header_len), sizeof(Uint16), 1, fp);

    write_layout (fp, bbox_layout.offset, bbox_layout.len);
    write_layout (fp, advx_layout.offset, advx_layout.len);
    write_layout (fp, advy_layout.offset, advy_layout.len);
    write_layout (fp, bits_off_layout.offset, bits_off_layout.len);
    write_layout (fp, glyph_bits_layout.offset, glyph_bits_layout.len);

    fwrite (&font_size, sizeof(Uint32), 1, fp);
    fwrite (&padding_ch, 1, header_layout.padding, fp);
    
    write_propt (fp, &old_info);
    fwrite (&padding_ch, 1, prop_layout.padding, fp);

    if (old_info.bbox_x)
    {
        write_bbox (fp, old_info.bbox_x, old_info.bbox_y,
                old_info.bbox_w, old_info.bbox_h, glyph_num);
        fwrite (&padding_ch, 1, bbox_layout.padding, fp);
    }

    if (old_info.offset)
    {
        fwrite (old_info.width, 1, glyph_num, fp);
        fwrite (&padding_ch, 1, advx_layout.padding, fp);
    }
    
    if (old_info.offset)
    {
        write_bits_off (fp, old_info.offset, glyph_num);
        fwrite (&padding_ch, 1, bits_off_layout.padding, fp);
    }

    print_bitmap (old_info.bits + old_info.offset['A'-old_info.first_char], 
            old_info.width['A'-old_info.first_char],
            old_info.height);

    fwrite (old_info.bits, glyph_bits_len, 1, fp);
    fclose (fp);

    UnloadVarBitmapFont (&old_info);

    return 0;
}

int main (int argc, char **argv)
{

    int i;
    if (argc < 1) {
        fprintf (stderr, "Usage: upgradevbf <vbffile>\n");
        return 1;
    }

    for (i=1; i<argc; i++)
    {
        upgrade (argv[i], argv[i]);
    }
    return 0;
}
