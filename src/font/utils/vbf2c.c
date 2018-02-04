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
** vi: tabstop=4:expandtab
**
** vbf2c.c: Dump VBF font to in-core C file.
**
** Create date: 2007/04/24
*/

/*
** TODO:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#define LEN_VERSION_INFO    10

#define VBF_VERSION         "vbf-1.0**"
#define VBF_VERSION2        "vbf-2.0**"

typedef int BOOL;
#define TRUE    1
#define FALSE   0

#define MGUI_LIL_ENDIAN  1234
#define MGUI_BIG_ENDIAN  4321

/* Assume that the host is little endian */
#define MGUI_BYTEORDER   MGUI_LIL_ENDIAN

#define LEN_DEVFONT_NAME            79

/********************** Load/Unload of var bitmap font ***********************/
static int LoadVarBitmapFont (const char* file, OLD_VBFINFO* info)
{
#ifdef HAVE_MMAP
    int fd;
#else
    FILE* fp;
#endif
    char* temp = NULL;
    int len_header, len_offsets, len_widths, len_bits, len_bboxs = 0;
    int font_size;
    char version[LEN_VERSION_INFO + 1];

#ifdef HAVE_MMAP
    if ((fd = open (file, O_RDONLY)) < 0)
        return FALSE;

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

    if (read (fd, &len_header, sizeof (int)) == -1)
        goto error;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    len_header = ArchSwap32 (len_header);
#endif

    if (read (fd, &info->max_width, sizeof (char) * 2) == -1) goto error;
    if (read (fd, &info->height, sizeof (int) * 2) == -1) goto error;

#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    info->height = ArchSwap32 (info->height);
    info->descent = ArchSwap32 (info->descent);
#endif

    if (read (fd, &info->first_char, sizeof (char) * 3) == -1) goto error;

    if (lseek (fd, len_header - (((info->version == 2)?5:4) * sizeof (int)), SEEK_SET) == -1)
        goto error;

    if (read (fd, &len_offsets, sizeof (int)) == -1
            || read (fd, &len_widths, sizeof (int)) == -1
            || (info->version == 2 && read (fd, &len_bboxs, sizeof (int)) == -1)
            || read (fd, &len_bits, sizeof (int)) == -1
            || read (fd, &font_size, sizeof (int)) == -1)
        goto error;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    len_offsets = ArchSwap32 (len_offsets);
    len_widths = ArchSwap32 (len_widths);
    if (info->version == 2)
        len_bboxs = ArchSwap32 (len_bboxs);
    len_bits = ArchSwap32 (len_bits);
    font_size = ArchSwap32 (font_size);
#endif

    if ((temp = mmap (NULL, font_size, PROT_READ, MAP_SHARED, 
            fd, 0)) == MAP_FAILED)
        goto error;

    temp += len_header;
    close (fd);
#else
    // Open font file and read information of font.
    if (!(fp = fopen (file, "rb")))
        return FALSE;

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

    if (fread (&len_header, sizeof (int), 1, fp) < 1)
        goto error;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    len_header = ArchSwap32 (len_header);
#endif

    if (fread (&info->max_width, sizeof (char), 2, fp) < 2) goto error;
    if (fread (&info->height, sizeof (int), 2, fp) < 2) goto error;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    info->height = ArchSwap32 (info->height);
    info->descent = ArchSwap32 (info->descent);
#endif
    if (fread (&info->first_char, sizeof (char), 3, fp) < 3) goto error;

    if (fseek (fp, len_header - (((info->version == 2)?5:4)*sizeof (int)), SEEK_SET) != 0)
            goto error;

    if (fread (&len_offsets, sizeof (int), 1, fp) < 1
            || fread (&len_widths, sizeof (int), 1, fp) < 1
            || (info->version == 2 && fread (&len_bboxs, sizeof (int), 1, fp) < 1)
            || fread (&len_bits, sizeof (int), 1, fp) < 1
            || fread (&font_size, sizeof (int), 1, fp) < 1)
        goto error;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    len_offsets = ArchSwap32 (len_offsets);
    len_widths = ArchSwap32 (len_widths);
    if (info->version == 2)
        len_bboxs = ArchSwap32 (len_bboxs);
    len_bits = ArchSwap32 (len_bits);
    font_size = ArchSwap32 (font_size);
#endif

    // Allocate memory for font data.
    font_size -= len_header;
    if ((temp = (char *)malloc (font_size)) == NULL)
        goto error;

    if (fseek (fp, len_header, SEEK_SET) != 0)
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

#if 0
    fprintf (stderr, "VBF: %s-%dx%d-%d (%d~%d:%d).\n", 
            info->name, info->max_width, info->height, info->descent,
            info->first_char, info->last_char, info->def_char);
#endif

    return len_bits;

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

static void dump_incore_offset (const OLD_VBFINFO* vbf_info)
{
    int ch;
    for (ch = vbf_info->first_char; ch <= vbf_info->last_char; ch++) {
        printf ("%d, \n", vbf_info->offset[ch]);
    }
}

static void dump_incore_width (const OLD_VBFINFO* vbf_info)
{
    int ch;
    for (ch = vbf_info->first_char; ch <= vbf_info->last_char; ch++) {
        printf ("%d, \n", vbf_info->width[ch]);
    }
}

static void dump_incore_bits (const OLD_VBFINFO* vbf_info, int len_bits)
{
    int i;

    for (i = 0; i < len_bits; i++) {
        if (i % 8 == 0)
            printf ("\n\t");
        printf ("0x%02x, ", vbf_info->bits [i]);
    }
}

static void dump_incore_bbox (const OLD_VBFINFO* vbf_info, int which)
{
    int ch;
    for (ch = vbf_info->first_char; ch <= vbf_info->last_char; ch++) {
        switch (which) {
            case 1:
            printf ("%d, \n", vbf_info->bbox_x[ch]);
            break;
            case 2:
            printf ("%d, \n", vbf_info->bbox_y[ch]);
            break;
            case 3:
            printf ("%d, \n", vbf_info->bbox_w[ch]);
            break;
            case 4:
            printf ("%d, \n", vbf_info->bbox_h[ch]);
            break;
        }
    }
}

static void dumpVBF2C (const char* name, const OLD_VBFINFO* vbf_info, int len_bits)
{
    printf ("/*\n");
    printf ("** In-core VBF file for %s.\n", name);
    printf ("**\n");
    printf ("** This file is created by 'vbf2c' by FMSoft (http://www.fmsoft.cn).\n");
    printf ("** Please do not modify it manually.\n");
    printf ("**\n");
    printf ("** Copyright (C) 2007 FMSoft\n");
    printf ("**\n");
    printf ("*/\n");

    printf ("#include <stdio.h>\n");
    printf ("#include <stdlib.h>\n");
    printf ("\n");
    printf ("#include \"common.h\"\n");
    printf ("#include \"minigui.h\"\n");
    printf ("#include \"gdi.h\"\n");
    printf ("\n");
    printf ("#ifdef _VBF_SUPPORT\n");
    printf ("\n");
    printf ("#include \"varbitmap.h\"\n");
    printf ("\n");

    if (vbf_info->offset) {
        printf ("static const unsigned short offset [] = {\n");
        dump_incore_offset (vbf_info);
        printf ("};\n\n");
    }

    if (vbf_info->width) {
        printf ("static const unsigned char width [] = {\n");
        dump_incore_width (vbf_info);
        printf ("};\n\n");
    }

    printf ("static const unsigned char bits [] = {");
    dump_incore_bits (vbf_info, len_bits);
    printf ("\n};\n\n");

    if (vbf_info->version == 2) {
        printf ("static const unsigned char bbox_x [] = {\n");
        dump_incore_bbox (vbf_info, 1);
        printf ("};\n\n");

        printf ("static const unsigned char bbox_y [] = {\n");
        dump_incore_bbox (vbf_info, 2);
        printf ("};\n\n");

        printf ("static const unsigned char bbox_w [] = {\n");
        dump_incore_bbox (vbf_info, 3);
        printf ("};\n\n");

        printf ("static const unsigned char bbox_h [] = {\n");
        dump_incore_bbox (vbf_info, 4);
        printf ("};\n\n");
    }

    printf ("OLD_VBFINFO %s = {\n", name);
    printf ("\t%d, \n", vbf_info->version);
    printf ("\t\"%s\", \n", vbf_info->name);
    printf ("\t%d, \n", vbf_info->max_width);
    printf ("\t%d, \n", vbf_info->ave_width);
    printf ("\t%d, \n", vbf_info->height);
    printf ("\t%d, \n", vbf_info->descent);
    printf ("\t%d, \n", vbf_info->first_char);
    printf ("\t%d, \n", vbf_info->last_char);
    printf ("\t%d, \n", vbf_info->def_char);

    if (vbf_info->offset)
        printf ("\toffset, \n");
    else
        printf ("\tNULL, \n");

    if (vbf_info->width)
        printf ("\twidth, \n");
    else
        printf ("\tNULL, \n");

    printf ("\tbits, \n");
    printf ("\t0, \n");

    if (vbf_info->version == 2) {
        printf ("\tbbox_x, \n");
        printf ("\tbbox_x, \n");
        printf ("\tbbox_w, \n");
        printf ("\tbbox_h, \n");
    }
    else {
        printf ("\tNULL, NULL, NULL, NULL, \n");
    }
    printf ("};\n\n");

    printf ("#endif /* _VBF_SUPPORT */\n");
}

int main (int argc, char **argv)
{
    int len_bits;
    char name [256];
    OLD_VBFINFO vbf_info;

    if (argc < 3) {
        fprintf (stderr, "Usage: vbf2c <vbffile> <name>\n");
        return 1;
    }

    if ((len_bits = LoadVarBitmapFont (argv[1], &vbf_info)) == 0) {
        fprintf (stderr, "vbf2c: can't load VBF file: %s\n", argv[1]);
        return 2;
    }
  
    strcpy (name, "__mgif_vbf_");
    strcat (name, argv[2]);

    dumpVBF2C (name, &vbf_info, len_bits);

    UnloadVarBitmapFont (&vbf_info);

    return 0;
}

