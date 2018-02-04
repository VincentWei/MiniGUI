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
** $Id: qpf2upf.c 8944 2008-1-29 08:29:16Z hejian $
** 
** vi: tabstop=4:expandtab
**
** qpf2c.c: Dump QPF font to in-core C file.
**
** Create date: 2008/01/22
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>


#define TRUE   1
#define FALSE  0
#define LEN_UNIDEVFONT_NAME         127
#define LEN_VERSION_INFO 16
#define LEN_VENDER_NAME  16
#define LEN_DEVFONT_NAME 127

#define QPF_FLAG_MODE_SMOOTH 1


typedef  int            BOOL   ;
typedef  unsigned char  Uint8  ;
typedef  signed   char  Sint8  ;
typedef  unsigned short Uint16 ;
typedef  unsigned int   Uint32 ;


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**********************************************/

#define FM_SMOOTH   1

#ifdef __NOUNIX__
typedef struct _GLYPHMETRICS {
#else
typedef struct __attribute__ ((packed)) _GLYPHMETRICS {
#endif
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
    const GLYPHMETRICS* metrics;
    const unsigned char* data;
} GLYPH;

typedef struct _GLYPHTREE
{
    unsigned int min, max;
    struct _GLYPHTREE* less;
    struct _GLYPHTREE* more;
    GLYPH* glyph;
} GLYPHTREE;


#ifdef __NOUNIX__
typedef struct _QPFMETRICS
#else
typedef struct __attribute__ ((packed)) _QPFMETRICS
{
#endif
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
    char font_name [LEN_UNIDEVFONT_NAME + 1];
    unsigned int height;
    unsigned int width;

    unsigned int file_size;
    QPFMETRICS* fm;

    GLYPHTREE* tree;
} QPFINFO;


BOOL LoadQPFont (const char* file, int fd,  QPFINFO* QPFInfo);
void UnloadQPFont (QPFINFO* QPFInfo, int fd);

/****************upf.h*****************/

#ifdef __NOUNIX__
typedef struct _UPFGLYPH {
#else
typedef struct __attribute__ ((packed)) _UPFGLYPH
{
#endif
    /* BBOX information of the glyph */
    Sint8 bearingx;
    Sint8 bearingy;
    Uint8 width;
    Uint8 height;
    /* advance value of the glyph */
    Uint8 pitch;
    /* Paddings for alignment */
    Uint8 padding1;
    Uint8 advance;
    /* The pitch of the bitmap (bytes of one scan line) */
    Uint8 padding2;
     /* the index of the glyph bitmap in the bitmap section of this font file. */
    Uint32 bitmap_offset;
}UPFGLYPH;


typedef struct _UPFNODE
{
    Uint32 min;
    Uint32 max;
    Uint32 less_offset;
    Uint32 more_offset;
    Uint32 glyph_offset;
}UPFNODE;


#ifdef __NOUNIX__
typedef struct _UPFINFO {
#else
typedef struct __attribute__ ((packed)) _UPFINFO
{
#endif
    char       font_name [LEN_UNIDEVFONT_NAME + 1];

    Uint8      width;
    Uint8      height;
    Sint8      ascent;
    Sint8      descent;
    Uint8      max_width;
    Uint8      underline_pos;
    Uint8      underline_width;
    Sint8      leading;
    Uint8      mono_bitmap;
    Uint8      reserved[3];
    Uint32     root_dir;
    Uint32     file_size;
}UPFINFO;

#ifdef __NOUNIX__
typedef struct _UPFV1_FILE_HEADER{
#else
typedef struct  __attribute__ ((packed)) _UPFV1_FILE_HEADER
{
#endif
    char     ver_info [LEN_VERSION_INFO];
    char     vender_name [LEN_VENDER_NAME];
    char     font_name [LEN_DEVFONT_NAME + 1];
    Uint8    width;
    Uint8    height;
    Sint8    ascent;
    Sint8    descent;
    Uint8    max_width;
    Uint8    min_width;
    Sint8    left_bearing;
    Sint8    right_bearing;
    Uint8    underline_pos;
    Uint8    underline_width;
    Sint8    leading;
    Uint8    mono_bitmap;
    Uint16   endian;
    Uint32   off_nodes;
    Uint32   off_glyphs;
    Uint32   off_bitmaps;
    Uint32   len_nodes;
    Uint32   len_glyphs;
    Uint32   len_bitmaps;
    Uint32   nr_glyph;
    Uint32   nr_zones;
    Uint32   font_size;
} UPFV1_FILE_HEADER;

#ifdef __cplusplus
}
#endif  /* __cplusplus */


/***********************************************************************/


static unsigned off_nodes   =0;
static unsigned off_glyphs  =0;
static unsigned off_bitmaps =0;


//static unsigned len_nodes   =0;
//static unsigned len_glyphs  =0;
static unsigned len_bitmaps =0;


static unsigned nr_nodes   =0;
static unsigned nr_glyphs  =0;


/** =====================================================================
 ** Usage of the tool.
 ** ===================================================================== 
 */
void help (void)
{
    printf ("################USAGE######################\n");
    printf ("COMMANDLINE :qpf2upf filename fontname  \n");
    printf ("EXAMPLE:\n");
    printf ("\t qpf2upf unifont_160_50.qpf upf-unifont-rrncnn-16-16-ISO8859-1,ISO8859-15,GB2312-0,GBK,BIG5,UTF-8\n");
}


/** =====================================================================
 ** Read glyph node of qpf.
 ** ===================================================================== 
 */
static void readNode (GLYPHTREE* tree, Uint8** data)
{
    Uint8 rw, cl;
    int flags, n;

    rw = **data; (*data)++;
    cl = **data; (*data)++;
    tree->min = (rw << 8) | cl;

    rw = **data; (*data)++;
    cl = **data; (*data)++;
    tree->max = (rw << 8) | cl;

    flags = **data; (*data)++;
    if (flags & 1)
        tree->less = (GLYPHTREE*)calloc (1, sizeof (GLYPHTREE));
    else
        tree->less = NULL;

    if (flags & 2)
        tree->more = (GLYPHTREE*)calloc (1, sizeof (GLYPHTREE));
    else
        tree->more = NULL;

    n = tree->max - tree->min + 1;
    tree->glyph = calloc (n, sizeof (GLYPH));

    if (tree->less)
        readNode (tree->less, data);
    if (tree->more)
        readNode (tree->more, data);
}


/** =====================================================================
 ** Read glyph metrics of qpf.
 ** ===================================================================== 
 */

static void readMetrics (GLYPHTREE* tree, Uint8** data)
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


/** =====================================================================
 ** Read glyph bitmap data of qpf
 ** ===================================================================== 
 */

static void readData (GLYPHTREE* tree, Uint8** data)
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
        readData (tree->less, data);
    if (tree->more)
        readData (tree->more, data);
}


/** =====================================================================
 ** To build a glyph tree of qpf
 ** ===================================================================== 
 */

static void BuildGlyphTree (GLYPHTREE* tree, Uint8** data)
{
    readNode (tree, data);
    readMetrics (tree, data);
    readData (tree, data);
}


/** =====================================================================
 ** Load qpf font from the file name
 ** ===================================================================== 
 */

BOOL LoadQPFont (const char* file, int fd,  QPFINFO* QPFInfo)
{
    struct stat st;
    Uint8* data;

    if ((fd = open (file, O_RDONLY)) < 0) return FALSE;
    if (fstat (fd, &st)) return FALSE;
    QPFInfo->file_size = st.st_size;


    QPFInfo->fm = (QPFMETRICS*) mmap( 0, st.st_size,
            PROT_READ, MAP_PRIVATE, fd, 0 );

    if (!QPFInfo->fm || QPFInfo->fm == (QPFMETRICS *)MAP_FAILED)
        goto error;


    QPFInfo->tree = (GLYPHTREE*)calloc (1, sizeof (GLYPHTREE));

    data = (Uint8*)QPFInfo->fm;
    data += sizeof (QPFMETRICS);
    BuildGlyphTree (QPFInfo->tree, &data);

    return TRUE;

error:
    munmap (QPFInfo->fm, QPFInfo->file_size);
    close (fd);
    return FALSE;
}



/** =====================================================================
 ** Clear glyph tree of qpf
 ** ===================================================================== 
 */

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


/** =====================================================================
 ** Unload qpf font
 ** ===================================================================== 
 */

void UnloadQPFont (QPFINFO* QPFInfo, int fd)
{
    if (QPFInfo->file_size == 0)
        return;

    ClearGlyphTree (QPFInfo->tree);
    free (QPFInfo->tree);
    munmap (QPFInfo->fm, QPFInfo->file_size);
    close (fd);
}



/** =====================================================================
 ** Add glyph bitmap data to upf font file
 ** ===================================================================== 
 */

static void AddDate (GLYPHTREE* tree, FILE *upfile)
{
    UPFNODE  upfnode;
    UPFGLYPH upfglyph;
    int n, i;

    fread (&upfnode, sizeof (UPFNODE), 1, upfile); 

    n = upfnode.max - upfnode.min + 1;

    for (i =0; i<n; i++)
    {
        fseek (upfile, (upfnode.glyph_offset + i * sizeof (UPFGLYPH)), SEEK_SET );
        fread (&upfglyph, sizeof (UPFGLYPH), 1, upfile); 
        fseek (upfile, 0L, SEEK_END );
        upfglyph.bitmap_offset =ftell (upfile); 

        fseek (upfile, (upfnode.glyph_offset + i * sizeof (UPFGLYPH)), SEEK_SET );
        fwrite ((const void*)&upfglyph, sizeof (UPFGLYPH), 1, upfile );
        fseek (upfile, 0L, SEEK_END );

        if (off_bitmaps== 0)
            off_bitmaps = ftell (upfile);

        len_bitmaps +=(upfglyph.height * upfglyph.pitch);

        fwrite ((const void*)(tree->glyph[i].data), (upfglyph.height * upfglyph.pitch), 1, upfile );
    }

    if (upfnode.less_offset)
    {
        fseek (upfile, upfnode.less_offset, SEEK_SET); 
        AddDate (tree->less, upfile);
    }
    if (upfnode.more_offset)
    {
        fseek (upfile, upfnode.more_offset, SEEK_SET); 
        AddDate (tree->more, upfile);
    }


}


/** =====================================================================
 ** Add glyph information to upf font file
 ** ===================================================================== 
 */

static void CreatGlyph (GLYPHTREE* tree, FILE *upfile)
{
    UPFNODE  upfnode;
    static   UPFGLYPH upfglyph;
    unsigned int curr_pos;
    int n, i;

    curr_pos = ftell (upfile);
    fread (&upfnode, sizeof (UPFNODE), 1, upfile); 
    n = upfnode.max - upfnode.min + 1;


    fseek (upfile, 0L, SEEK_END );
    upfnode.glyph_offset = ftell (upfile);

    if (off_glyphs== 0)
        off_glyphs = ftell (upfile);

    for (i = 0; i<n; i++)
    {
        nr_glyphs ++;

        upfglyph.bearingx      = tree->glyph[i].metrics->bearingx;
        upfglyph.bearingy      = tree->glyph[i].metrics->bearingy;
        upfglyph.width         = tree->glyph[i].metrics->width;
        upfglyph.height        = tree->glyph[i].metrics->height;
        upfglyph.pitch         = tree->glyph[i].metrics->linestep;
        upfglyph.padding1      = tree->glyph[i].metrics->padding;
        upfglyph.advance       = tree->glyph[i].metrics->advance;
        upfglyph.padding2      = tree->glyph[i].metrics->reserved;
        upfglyph.bitmap_offset = 0;

        fwrite ((const void*)&upfglyph, sizeof (UPFGLYPH), 1, upfile );
    }   

    fseek (upfile, curr_pos, SEEK_SET); 
    fwrite ((const void*)&upfnode, sizeof (UPFNODE), 1, upfile);

    if (upfnode.less_offset)
    {
        fseek (upfile, upfnode.less_offset, SEEK_SET); 
        CreatGlyph (tree->less, upfile);
    }
    if (upfnode.more_offset)
    {
        fseek (upfile, upfnode.more_offset, SEEK_SET); 
        CreatGlyph (tree->more, upfile);
    }

}



/** =====================================================================
 ** Add glyph UNICODE value area node to upf font file
 ** ===================================================================== 
 */

static void CreateNodes (GLYPHTREE* tree, FILE *upfile )
{
    UPFNODE upfnode; 
    unsigned int curr_pos;

    curr_pos = ftell (upfile);
    upfnode.min = tree->min;
    upfnode.max = tree->max;
    upfnode.less_offset = 0;
    upfnode.more_offset = 0;

    nr_nodes ++;

    if (off_nodes==0)
        off_nodes = curr_pos;

    fwrite ((const void*)&upfnode,  sizeof (UPFNODE),  1, upfile);

    if (tree->less)
    {
        upfnode.less_offset = ftell (upfile) ;
        fseek (upfile, curr_pos, SEEK_SET );
        fwrite ((const void*)&upfnode,  sizeof (UPFNODE),  1, upfile);
        fseek (upfile, 0L, SEEK_END);
        CreateNodes (tree->less, upfile);
    }

    if (tree->more)
    {
        fseek (upfile, 0L, SEEK_END);
        upfnode.more_offset = ftell (upfile);
        fseek (upfile, curr_pos, SEEK_SET );
        fwrite ((const void*)&upfnode,  sizeof (UPFNODE),  1, upfile);
        fseek (upfile, 0L, SEEK_END);
        CreateNodes (tree->more, upfile);
    }

}



/** =====================================================================
 ** To Create a upf font file from a qpf font file, use the parameter font_name 
 ** as the device font name
 ** ===================================================================== 
 */

static void CreatUpfFile (QPFINFO *qpfinfo, FILE *upfile ,const char *font_name)
{
    UPFV1_FILE_HEADER  upfheade={0};
    Uint32 curr_pos, start_pos;

    sprintf (upfheade.ver_info, "upf-1.0");
    sprintf (upfheade.vender_name, "FMSoft");
    if(font_name)
        snprintf (upfheade.font_name, LEN_DEVFONT_NAME, "%s", font_name);

    upfheade.endian            = 0x1234 ;

    upfheade.width             = qpfinfo->fm->maxwidth;
    upfheade.height            = qpfinfo->fm->ascent +qpfinfo->fm->descent + qpfinfo->fm->leading;
    upfheade.ascent            = qpfinfo->fm->ascent;
    upfheade.descent           = qpfinfo->fm->descent;  
    upfheade.max_width         = qpfinfo->fm->maxwidth ;
    upfheade.underline_pos     = qpfinfo->fm->underlinepos;
    upfheade.underline_width   = qpfinfo->fm->underlinewidth;
    upfheade.leading           = qpfinfo->fm->leading;
    upfheade.mono_bitmap       = !(qpfinfo->fm->flags & QPF_FLAG_MODE_SMOOTH);
    upfheade.off_nodes         = sizeof (UPFV1_FILE_HEADER);

    start_pos = ftell (upfile);
    fwrite((const void*)&upfheade,  sizeof (UPFV1_FILE_HEADER),  1, upfile);
    curr_pos = ftell (upfile);

    CreateNodes (qpfinfo->tree, upfile);
    fseek (upfile, curr_pos, SEEK_SET);
    CreatGlyph  (qpfinfo->tree, upfile);
    fseek (upfile, curr_pos, SEEK_SET);
    AddDate (qpfinfo->tree, upfile);

    fseek (upfile, 0, SEEK_END);
    upfheade.font_size = ftell (upfile);

    fseek (upfile, start_pos, SEEK_SET);

    upfheade.off_nodes   = off_nodes;
    upfheade.off_glyphs  = off_glyphs;
    upfheade.off_bitmaps = off_bitmaps;
    upfheade.len_bitmaps = len_bitmaps;
    upfheade.len_nodes   = nr_nodes  * sizeof (UPFNODE);
    upfheade.len_glyphs  = nr_glyphs * sizeof (UPFGLYPH);

    fwrite((const void*)&upfheade,  sizeof (UPFV1_FILE_HEADER),  1, upfile);


}


/******************************main*************************************/

/** =====================================================================
 ** Main function
 ** ===================================================================== 
 */

int main (int args, char *arg[])
{
    QPFINFO qpfinfo;
    char upf_name [128];
    FILE *upf_file;
    int  fd =0;
    char *s,*filename;

    if (args <3)
    {
        help ();
        printf ("Please  specify  the 'qpf' file and the fontname\n");
        return 1;
    }

    filename =strrchr((const char *)arg[1], '/');
    sprintf (upf_name, "%s", filename?(filename+1):arg[1]);
    s = strrchr ((const char *)upf_name, '.');

    if (s==NULL)
    {
        help ();
        printf ("Please  specify  the 'qpf' file\n");
        return 1;
    }

    *s ='\0';
    sprintf (s, "%s", ".upf");

    if (!LoadQPFont (arg[1], fd, &qpfinfo))
    {
        help ();
        printf ("Load %s faile !\n", arg[1]);
        return 2;
    }

    /*****************create upf file********************************/
    upf_file = fopen ((const char *)upf_name, "w+"); 
    CreatUpfFile (&qpfinfo, upf_file, arg[2]);
    fclose (upf_file);
    /****************************************************************/    
    UnloadQPFont (&qpfinfo, fd);


    return 0;
}
