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
** $Id: dumpqpf.c 8944 2007-12-29 08:29:16Z xwyan $
** 
** vi: tabstop=4:expandtab
**
** dumpqpf.c: Dump in-core QPF font.
**
** Create date: 2005/07/19
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
** TODO:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define FM_SMOOTH   1

#define BOOL int
#define TRUE 1
#define FALSE 0

typedef unsigned char uchar;
typedef unsigned char Uint8;
typedef char Sint8;

typedef struct __attribute__ ((packed)) _GLYPHMETRICS {
    Uint8 linestep;
    Uint8 width;
    Uint8 height;
    Uint8 padding;

    Sint8 bearingx;     /* Difference from pen position to glyph's left bbox */
    Uint8 advance;      /* Difference between pen positions */
    Sint8 bearingy;     /* Used for putting characters on baseline */

    Sint8 reserved;     /* Do not use */
} GLYPHMETRICS;

typedef struct _GLYPH
{
    GLYPHMETRICS* metrics;
    unsigned char* data;
} GLYPH;

typedef struct _GLYPHTREE
{
    unsigned int min, max;
    struct _GLYPHTREE* less;
    struct _GLYPHTREE* more;
    GLYPH* glyph;

    unsigned int idx_node;
} GLYPHTREE;

typedef struct __attribute__ ((packed)) _QPFMETRICS
{
    Sint8 ascent, descent;
    Sint8 leftbearing, rightbearing;
    Uint8 maxwidth;
    Sint8 leading;
    Uint8 flags;
    Uint8 underlinepos;
    Uint8 underlinewidth;
    Uint8 reserved3;
} QPFMETRICS;

typedef struct 
{
    char* font_name;
    unsigned int height;
    unsigned int width;

    unsigned int file_size;
    QPFMETRICS* fm;

    GLYPHTREE* tree;

    int max_bmp_size;
    unsigned char* std_bmp;
} QPFINFO;

#define HAVE_MMAP 1

/********************** Load/Unload of QPF font ***********************/

static void readNode (GLYPHTREE* tree, uchar** data)
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
        tree->less = calloc (1, sizeof (GLYPHTREE));
    else
        tree->less = NULL;

    if (flags & 2)
        tree->more = calloc (1, sizeof (GLYPHTREE));
    else
        tree->more = NULL;

    n = tree->max - tree->min + 1;
    tree->glyph = calloc (n, sizeof (GLYPH));

    if (tree->less)
        readNode (tree->less, data);
    if (tree->more)
        readNode (tree->more, data);
}

static void readMetrics (GLYPHTREE* tree, uchar** data)
{
    int i;
    int n = tree->max - tree->min + 1;

    for (i = 0; i < n; i++) {
        tree->glyph[i].metrics = (GLYPHMETRICS*) *data;

        *data += sizeof (GLYPHMETRICS);
    }

    if (tree->less)
        readMetrics (tree->less, data);
    if (tree->more)
        readMetrics (tree->more, data);
}

static void readData (GLYPHTREE* tree, uchar** data)
{
    int i;
    int n = tree->max - tree->min + 1;

    for (i = 0; i < n; i++) {
        int datasize;

        datasize = tree->glyph[i].metrics->linestep * tree->glyph[i].metrics->height;
        tree->glyph[i].data = *data; *data += datasize;
    }

    if (tree->less)
        readData (tree->less, data);
    if (tree->more)
        readData (tree->more, data);
}

static void BuildGlyphTree (GLYPHTREE* tree, uchar** data)
{
    readNode (tree, data);
    readMetrics (tree, data);
    readData (tree, data);
}

BOOL LoadQPFont (const char* file, QPFINFO* QPFInfo)
{
    int fd;
    struct stat st;
    uchar* data;

    if ((fd = open (file, O_RDONLY)) < 0) return FALSE;
    if (fstat (fd, &st)) return FALSE;
    QPFInfo->file_size = st.st_size;

#ifdef HAVE_MMAP

    QPFInfo->fm = (QPFMETRICS*) mmap( 0, st.st_size,
                    PROT_READ, MAP_PRIVATE, fd, 0 );

    if (!QPFInfo->fm || QPFInfo->fm == (QPFMETRICS *)MAP_FAILED)
        goto error;

#else

    QPFInfo->fm = calloc (1, st.st_size);
    if (QPFInfo->fm == NULL)
        goto error;

    read (fd, QPFInfo->fm, st.st_size);

#endif

    QPFInfo->tree = calloc (1, sizeof (GLYPHTREE));

    data = (uchar*)QPFInfo->fm;
    data += sizeof (QPFMETRICS);
    BuildGlyphTree (QPFInfo->tree, &data);

    close (fd);
    return TRUE;

error:
    close (fd);
    return FALSE;
}

static void ClearGlyphTree (GLYPHTREE* tree)
{
    if (tree->less) {
        ClearGlyphTree (tree->less);
    }
    if (tree->more) {
        ClearGlyphTree (tree->more);
    }

    free (tree->glyph);
    free (tree->less);
    free (tree->more);
}

void UnloadQPFont (QPFINFO* QPFInfo)
{
    if (QPFInfo->file_size == 0)
        return;

    ClearGlyphTree (QPFInfo->tree);
    free (QPFInfo->tree);
#if 0
    free (QPFInfo->std_bmp);
#endif

#ifdef HAVE_MMAP
    munmap (QPFInfo->fm, QPFInfo->file_size);
#else
    free (QPFInfo->fm);
#endif
}

unsigned int idx_node = 0;

static void make_incore_glyph_tree (GLYPHTREE* tree)
{
    int i, j, n;
    int datasize, dataoffset;

    if (tree->less) {
        make_incore_glyph_tree (tree->less);
    }
    if (tree->more) {
        make_incore_glyph_tree (tree->more);
    }

    n = tree->max - tree->min + 1;

    printf ("static GLYPHMETRICS glyph_metrics_%d [] = {\n", idx_node);
    for (i = 0; i < n; i++) {
        //printf ("\t{0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x}, \n",
        printf ("\t{%d, %d, %d, %d, %d, %d, %d, %d}, \n",
                        tree->glyph[i].metrics->linestep,
                        tree->glyph[i].metrics->width,
                        tree->glyph[i].metrics->height,
                        tree->glyph[i].metrics->padding,
                        tree->glyph[i].metrics->bearingx,
                        tree->glyph[i].metrics->advance,
                        tree->glyph[i].metrics->bearingy,
                        tree->glyph[i].metrics->reserved);
    }
    printf ("};\n\n");

    printf ("static unsigned char glyph_data_%d [] = {", idx_node);
    for (i = 0; i < n; i++) {

        datasize = tree->glyph[i].metrics->linestep * tree->glyph[i].metrics->height;
        for (j = 0; j < datasize; j++) {
            if (j % 8 == 0)
                printf ("\n\t");
            printf ("0x%02x, ", tree->glyph[i].data [j]);
        }

    }
    printf ("\n};\n\n");

    dataoffset = 0;
    printf ("static GLYPH glyph_%d [] = {\n", idx_node);
    for (i = 0; i < n; i++) {
        printf ("\t{glyph_metrics_%d + %d, glyph_data_%d + %d}, \n", idx_node, i, idx_node, dataoffset);
        dataoffset += tree->glyph[i].metrics->linestep * tree->glyph[i].metrics->height;
    }
    printf ("};\n\n");

    printf ("static GLYPHTREE glyph_tree_%d = {\n", idx_node);
    printf ("\t0x%04x, 0x%04x, ", tree->min, tree->max);
    if (tree->less)
        printf ("&glyph_tree_%d, ", tree->less->idx_node);
    else
        printf ("NULL, ");
    if (tree->more)
        printf ("&glyph_tree_%d, ", tree->more->idx_node);
    else
        printf ("NULL, ");

    printf ("glyph_%d\n", idx_node);
    printf ("};\n\n");

    tree->idx_node = idx_node;
    idx_node ++;
}

static int dump_qpf (const char* name, const QPFINFO* QPFInfo, const char* font_name)
{
    printf ("/*\n");
    printf ("** This file is created by 'dumpqpf' by FMSoft (http://www.fmsoft.cn).\n");
    printf ("** Please do not modify it manually.\n");
    printf ("*/\n");

    printf ("#include <stdio.h>\n");
    printf ("#include <stdlib.h>\n");
    printf ("\n");
    printf ("#include \"common.h\"\n");
    printf ("#include \"minigui.h\"\n");
    printf ("#include \"gdi.h\"\n");
    printf ("\n");
    printf ("#ifdef _MGFONT_QPF\n");
    printf ("#ifdef _MGINCORE_RES\n");
    printf ("\n");
    printf ("#include \"qpf.h\"\n");
    printf ("\n");

    make_incore_glyph_tree (QPFInfo->tree);

    printf ("static QPFMETRICS fm = {\n");
    printf ("\t%d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
            QPFInfo->fm->ascent,
            QPFInfo->fm->descent,
            QPFInfo->fm->leftbearing,
            QPFInfo->fm->rightbearing,
            QPFInfo->fm->maxwidth,
            QPFInfo->fm->leading,
            QPFInfo->fm->flags,
            QPFInfo->fm->underlinepos,
            QPFInfo->fm->underlinewidth,
            QPFInfo->fm->reserved3);

    printf ("};\n\n");

    printf ("QPFINFO %s = {\n", name);
    printf ("\t\"%s\", \n", font_name);
    printf ("\t0, 0, 0, &fm, \n");
    printf ("\t&glyph_tree_%d, \n", idx_node - 1);
    printf ("};\n\n");

    printf ("#endif /* _MGINCORE_RES */\n");
    printf ("#endif /* _MGFONT_QPF */\n");
    return 0;
}

int main (int argc, char **argv)
{
    char name [256];
    QPFINFO QPFInfo;

    if (argc < 4) {
        fprintf (stderr, "Usage: dumpqpf <qpffile> <name> <dev-font-name>\n");
        return 1;
    }

    if (!LoadQPFont (argv[1], &QPFInfo)) {
        fprintf (stderr, "dumpqpf: can't load QPF file: %s\n", argv[2]);
        return 3;
    }
  
    strcpy (name, "__mgif_qpf_");
    strcat (name, argv[2]);

    dump_qpf (name, &QPFInfo, argv[3]);

    UnloadQPFont (&QPFInfo);

    return 0;
}

