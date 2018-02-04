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
** gif.c: Low-level GIF file read/save routines.
** 
** Current maintainer: Wei Yongming
**
** Create date: 2000/08/29
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "gdi.h"
#include "readbmp.h"

#ifdef _MGIMAGE_GIF

/*
 * GIF decoding routine
 */
#define MAXCOLORMAPSIZE         256
#define MAX_LWZ_BITS            12
#define INTERLACE               0x40
#define LOCALCOLORMAP           0x80

#define CM_RED                  0
#define CM_GREEN                1
#define CM_BLUE                 2

#define BitSet(byte, bit)              (((byte) & (bit)) == (bit))
#define ReadOK(file,buffer,len)        MGUI_RWread(file, buffer, len, 1)
#define LM_to_uint(a,b)                (((b)<<8)|(a))

typedef struct _GifScreen {
    unsigned int Width;
    unsigned int Height;
    unsigned char ColorMap[3][MAXCOLORMAPSIZE];
    unsigned int BitPixel;
    unsigned int ColorResolution;
    unsigned int Background;
    unsigned int AspectRatio;
    int GrayScale;
} GifScreen;

typedef struct _Gif89 {
    int transparent;
    int delayTime;
    int inputFlag;
    int disposal;
} Gif89;

typedef struct _GifInfo {
    GifScreen   screen;
    Gif89       gif89;
    int useGColormap;
    int bitPixel;
    int width;
    int height;
    int interlace;
    int ZeroDataBlock;
}GifInfo;

static int ReadColorMap(MG_RWops* src, int number, unsigned char buffer[3][MAXCOLORMAPSIZE]);
static int GetDataBlock(MG_RWops* src, unsigned char *buf ,void * info_data);
static int GetCode(MG_RWops* src, int code_size, int flag , void * info);
static int LWZReadByte(MG_RWops* src, int flag, int input_code_size, void * info_data);
static int ReadImage(MG_RWops* src, MYBITMAP* bmp, int len, int height, 
                int interlace , CB_ONE_SCANLINE cb, void* context , void * info);
/*
 *
 *
 */

void* __mg_init_gif (MG_RWops* fp, MYBITMAP *gif, RGB *pal)
{
    unsigned char buf[256];
    unsigned char c;
    char version[4];
    GifInfo * info = NULL;
    int i;

    if (!ReadOK (fp, buf, 6))
        return NULL;                /* not gif image*/
    if (strncmp((char *) buf, "GIF", 3) != 0)
        return NULL;
    strncpy (version, (char *) buf + 3, 3);
    version [3] = '\0';

    if (strcmp(version, "87a") != 0 && strcmp(version, "89a") != 0) {
        _MG_PRINTF ("MYBMP>Gif: GIF version number is not 87a or 89a\n");
        return NULL;                /* image loading error*/
    }

    info = (GifInfo *) malloc (sizeof(GifInfo));
    info->ZeroDataBlock = FALSE;
    info->useGColormap = 0;
    info->bitPixel = 0;
    info->width = 0;
    info->height = 0;
    info->interlace = 0;
    
    info->gif89.transparent = -1;
    info->gif89.delayTime = -1;
    info->gif89.inputFlag = -1;
    info->gif89.disposal = 0;

    if (!ReadOK (fp, buf, 7)) {
        _MG_PRINTF ("MYBMP>Gif: bad screen descriptor\n");
        goto fini;                /* image loading error*/
    }

    info->screen.Width = LM_to_uint (buf[0], buf[1]);
    info->screen.Height = LM_to_uint (buf[2], buf[3]);
    info->screen.BitPixel = 2 << (buf[4] & 0x07);
    info->screen.ColorResolution = (((buf[4] & 0x70) >> 3) + 1);
    info->screen.Background = buf[5];
    info->screen.AspectRatio = buf[6];

    if (BitSet(buf[4], LOCALCOLORMAP)) {        /* Global Colormap */
        if (ReadColorMap(fp, info->screen.BitPixel, info->screen.ColorMap)) {
            _MG_PRINTF ("MYBMP>Gif: bad global colormap\n");
            goto fini;                /* image loading error*/
        }
    }

    do
    {
        if (!ReadOK (fp, &c, 1)) {
            _MG_PRINTF ("MYBMP>Gif: EOF on image data\n");
            goto fini;
         }

        if (';' == c) {/* GIF terminator */
             _MG_PRINTF ("MYBMP>Gif: no image \n" );
            goto fini;
         }
        
        if ('!' == c) /* Start of the Extension*/
        {
            if (!ReadOK (fp, &c, 1)) 
                goto fini;
            if (0xfe == c) {
                while (GetDataBlock(fp, (unsigned char *) buf , info) != 0);
            }
            if (0xff == c) {
                while (GetDataBlock(fp, (unsigned char *) buf , info) != 0);
            }
            
            if (0xf9 == c) {
                GetDataBlock (fp, (unsigned char *) buf , info);
                info->gif89.disposal = (buf[0] >> 2) & 0x7;//000 000 0 0 the middle 2 bit is disposal
                info->gif89.inputFlag = (buf[0] >> 1) & 0x1;//000 000 0 0 the secand last bit 
                            //is user input flag
                info->gif89.delayTime = LM_to_uint(buf[1], buf[2]);
                if ((buf[0] & 0x1) != 0)// 000 000 0 0 the last bit is transparent flag
                    info->gif89.transparent = buf[3];
            }
        }
        

        if (',' == c) /* Start of the Image Descriptor*/
            break;

    } while(TRUE);


    if (!ReadOK (fp, buf, 9)) { /* Read the Image Descriptor*/
         _MG_PRINTF ("MYBMP>Gif: bad image size\n");
        goto fini;
        }

    info->useGColormap = !BitSet (buf[8], LOCALCOLORMAP);
    info->bitPixel = 1 << ((buf[8] & 0x07) + 1);

    if (!info->useGColormap) {
        memset (info->screen.ColorMap , 0 , 3*256);
        if (0 != ReadColorMap(fp, info->bitPixel, info->screen.ColorMap)) {
             _MG_PRINTF ("MYBMP>Gif: bad local colormap\n");
            goto fini;
        }
    }
    else
        info->bitPixel = info->screen.BitPixel;

    for (i = 0; i < info->screen.BitPixel; i++) {
        /*ImageSetCmap(image, i, cmap[CM_RED][i],
                     cmap[CM_GREEN][i], cmap[CM_BLUE][i]);*/
        pal[i].r = info->screen.ColorMap[CM_RED][i];
        pal[i].g = info->screen.ColorMap[CM_GREEN][i];
        pal[i].b = info->screen.ColorMap[CM_BLUE][i];
    }

    info->width = LM_to_uint(buf[4], buf[5]);
    info->height = LM_to_uint(buf[6], buf[7]);
    info->interlace = BitSet(buf[8], INTERLACE);
        
    gif->w = info->width;
    gif->h = info->height;
    gif->frames = 1;
    gif->depth = 8;
    bmp_ComputePitch (8, info->width, &gif->pitch, TRUE);
  
    gif->flags |= MYBMP_FLOW_DOWN;
    if ( info->gif89.transparent > 0 ) {
        gif->flags |= MYBMP_TRANSPARENT;
        gif->transparent = info->gif89.transparent;
    }

    return info;

fini:
    free (info);
    return NULL;
}


int __mg_load_gif (MG_RWops* fp, void* init_info, MYBITMAP *gif, CB_ONE_SCANLINE cb, void* context)
{
    int ok = 0;
    GifInfo * info = (GifInfo *)init_info;

    if (!info ->useGColormap) {
        ok = ReadImage (fp, gif, info->width, 
                        info->height,
                        info->interlace,
                        cb, context, info);
    } else {
        ok = ReadImage (fp, gif, info->width,
                        info->height, info->interlace,
                        cb, context, info);
    }


    if(ok)
      return 0;                /* image load ok*/

    return ERR_BMP_LOAD;                        /* image load error*/
}

void __mg_cleanup_gif (void* init_info)
{
    free (init_info);   /* free the GifInfo struct */    
}

static int
ReadColorMap (MG_RWops *src, int number, unsigned char buffer[3][MAXCOLORMAPSIZE])
{
    int i;
    unsigned char rgb[3];
    int flag;

    flag = 1;

    for (i = 0; i < number; ++i) {
        if (!ReadOK(src, rgb, sizeof(rgb)))
            return 1;
        buffer[CM_RED][i] = rgb[0];
        buffer[CM_GREEN][i] = rgb[1];
        buffer[CM_BLUE][i] = rgb[2];
        flag &= (rgb[0] == rgb[1] && rgb[1] == rgb[2]);
    }

#if 0
    if (flag)
        *gray = (number == 2) ? PBM_TYPE : PGM_TYPE;
    else
        *gray = PPM_TYPE;
    *gray = 0;
#endif

    return 0;
}


static int
GetDataBlock(MG_RWops *src, unsigned char *buf , void * info_data)
{
    unsigned char count;
    GifInfo * info = info_data;
    if (!ReadOK(src, &count, 1))
        return -1;
    info->ZeroDataBlock = (count == 0);

    if ((count != 0) && (!ReadOK(src, buf, count)))
        return -1;
    return count;
}

static int
GetCode(MG_RWops *src, int code_size, int flag , void * info)
{
    static unsigned char buf[280];
    static int curbit, lastbit, done, last_byte;
    int i, j, ret;
    unsigned char count;

    if (flag) {
        curbit = 0;
        lastbit = 0;
        done = FALSE;
        return 0;
    }
    if ((curbit + code_size) >= lastbit) {
        if (done) {
            if (curbit >= lastbit)
                 _MG_PRINTF ("MYBMP>Gif: bad decode\n");
            return -1;
        }
        buf[0] = buf[last_byte - 2];
        buf[1] = buf[last_byte - 1];

        if ((count = GetDataBlock(src, &buf[2] , info)) == 0)
            done = TRUE;

        last_byte = 2 + count;
        curbit = (curbit - lastbit) + 16;
        lastbit = (2 + count) * 8;
    }
    ret = 0;
    for (i = curbit, j = 0; j < code_size; ++i, ++j)
        ret |= ((buf[i / 8] & (1 << (i % 8))) != 0) << j;

    curbit += code_size;

    return ret;
}

static int
LWZReadByte(MG_RWops *src, int flag, int input_code_size , void * info_data)
{
    int code, incode;
    register int i;
    static int fresh = FALSE;
    static int code_size, set_code_size;
    static int max_code, max_code_size;
    static int firstcode, oldcode;
    static int clear_code, end_code;
    static int table[2][(1 << MAX_LWZ_BITS)];
    static int stack[(1 << (MAX_LWZ_BITS)) * 2], *sp;
    
    GifInfo * info = info_data;

    if (flag) {
        set_code_size = input_code_size;
        code_size = set_code_size + 1;
        clear_code = 1 << set_code_size;
        end_code = clear_code + 1;
        max_code_size = 2 * clear_code;
        max_code = clear_code + 2;

        GetCode(src, 0, TRUE , info);

        fresh = TRUE;

        for (i = 0; i < clear_code; ++i) {
            table[0][i] = 0;
            table[1][i] = i;
        }
        for (; i < (1 << MAX_LWZ_BITS); ++i)
            table[0][i] = table[1][0] = 0;

        sp = stack;

        return 0;
    } else if (fresh) {
        fresh = FALSE;
        do {
            firstcode = oldcode = GetCode(src, code_size, FALSE , info);
        } while (firstcode == clear_code);
        return firstcode;
    }
    if (sp > stack)
        return *--sp;

    while ((code = GetCode(src, code_size, FALSE , info)) >= 0) {
        if (code == clear_code) {
            for (i = 0; i < clear_code; ++i) {
                table[0][i] = 0;
                table[1][i] = i;
            }
            for (; i < (1 << MAX_LWZ_BITS); ++i)
                table[0][i] = table[1][i] = 0;
            code_size = set_code_size + 1;
            max_code_size = 2 * clear_code;
            max_code = clear_code + 2;
            sp = stack;
            firstcode = oldcode = GetCode(src, code_size, FALSE , info);
            return firstcode;
        } else if (code == end_code) {
            int count;
            unsigned char buf[260];

            if (info->ZeroDataBlock)
                return -2;

            while ((count = GetDataBlock(src, buf , info)) > 0);

            if (count != 0) {
                _MG_PRINTF ("MYBMP>Gif: missing EOD in data stream (common occurence)");
            }
            return -2;
        }
        incode = code;

        if (code >= max_code) {
            *sp++ = firstcode;
            code = oldcode;
        }
        while (code >= clear_code) {
            *sp++ = table[1][code];
            if (code == table[0][code]) {
                _MG_PRINTF ("MYBMP>Gif: circular table entry\n");
                return -1;
            }
            code = table[0][code];
        }

        *sp++ = firstcode = table[1][code];

        if ((code = max_code) < (1 << MAX_LWZ_BITS)) {
            table[0][code] = oldcode;
            table[1][code] = firstcode;
            ++max_code;
            if ((max_code >= max_code_size) &&
                (max_code_size < (1 << MAX_LWZ_BITS))) {
                max_code_size *= 2;
                ++code_size;
            }
        }
        oldcode = incode;

        if (sp > stack)
            return *--sp;
    }
    return code;
}

static int
ReadImage(MG_RWops* src, MYBITMAP* bmp, int len, int height, 
        int interlace , CB_ONE_SCANLINE cb, void* context , void * info)
{
    unsigned char c;
    int v;
    int xpos = 0, ypos = 0, pass = 0;

    /*
     *        Initialize the compression routines
     */
    if (!ReadOK(src, &c, 1)) {
        _MG_PRINTF ("MYBMP>Gif: EOF on image data\n");
        return 0;
    }
    if (LWZReadByte(src, TRUE, c , info) < 0) {
        _MG_PRINTF ("MYBMP>Gif: error reading image\n");
        return 0;
    }

    /*image = ImageNewCmap(len, height, cmapSize);*/

    while ((v = LWZReadByte(src, FALSE, c , info)) >= 0) {
        if (bmp->flags &= MYBMP_LOAD_ALLOCATE_ONE )
            bmp->bits[xpos] = v;
        else
            bmp->bits[ypos * bmp->pitch + xpos] = v;

        ++xpos;
        if (xpos == len) {
            xpos = 0;
            if (interlace) {

                /* emit line at here */
                if (bmp->flags &= MYBMP_LOAD_ALLOCATE_ONE)
                    (*cb)(context, bmp, ypos);

                switch (pass) {
                case 0:
                case 1:
                    ypos += 8;
                    break;
                case 2:
                    ypos += 4;
                    break;
                case 3:
                    ypos += 2;
                    break;
                }

                if (ypos >= height) {
                    ++pass;
                    switch (pass) {
                    case 1:
                        ypos = 4;
                        break;
                    case 2:
                        ypos = 2;
                        break;
                    case 3:
                        ypos = 1;
                        break;
                    default:
                        goto fini;
                    }
                }
            }
            else {
            /* emit line at here */
            if (bmp->flags &= MYBMP_LOAD_ALLOCATE_ONE)
                (*cb)(context, bmp, ypos);
                ++ypos;
            }
        }
        if (ypos >= height)
            break;
    }

fini:
    return 1;
}

int __mg_save_gif (MG_RWops* fp, MYBITMAP* bmp, RGB* pal)
{
    return -1;
}

BOOL __mg_check_gif (MG_RWops* fp)
{
    char buf[16];
    char version[4];

    if (!ReadOK (fp, buf, 6))
        return FALSE;               /* not gif image*/
    if (strncmp(buf, "GIF", 3) != 0)
        return FALSE;
    strncpy (version, buf + 3, 3);
    version [3] = '\0';

    if (strcmp(version, "87a") != 0 && strcmp(version, "89a") != 0) {
        return FALSE;               /* image loading error*/
    }

    return TRUE;
}

#endif /* _MGIMAGE_GIF */

