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
** pcx.c: Low-level PCX bitmap file read/save routines.
** 
** Some code comes from pcx.c of Allegro (a gift-ware) by Shawn Hargreaves.
** 
** Current maintainer:  Wei Yongming.
*/

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "gdi.h"
#include "readbmp.h"

#ifdef _MGIMAGE_PCX

/* __mg_load_pcx:
 *  Loads a 256 color PCX file, returning in a bitmap structure and storing
 *  the palette data in the specified palette (this should be an array of
 *  at least 256 RGB structures).
 */
typedef struct {
	int bytes_per_line;

} init_info_t;

void * __mg_init_pcx (MG_RWops* f, MYBITMAP * bmp, RGB *pal)
{
    int c;
    int width, height;
    int bpp;
    unsigned long size;
    unsigned char* bits;
    int pitch;
	init_info_t * bmp_info = NULL;

	bmp_info=(init_info_t *)malloc(sizeof(init_info_t));
	if (!bmp_info)
		return NULL;
 
    fp_getc(f);                    /* skip manufacturer ID */
    fp_getc(f);                    /* skip version flag */
    fp_getc(f);                    /* skip encoding flag */

    if (fp_getc(f) != 8) {         /* we like 8 bit color planes */
        return NULL;
    }

    width = -(fp_igetw(f));        /* xmin */
    height = -(fp_igetw(f));       /* ymin */
    width += fp_igetw(f) + 1;      /* xmax */
    height += fp_igetw(f) + 1;     /* ymax */

    fp_igetl(f);                   /* skip DPI values */

    for (c=0; c<16; c++) {           /* read the 16 color palette */
        pal[c].r = fp_getc(f);
        pal[c].g = fp_getc(f);
        pal[c].b = fp_getc(f);
    }

    fp_getc(f);

    bpp = fp_getc(f) * 8;          /* how many color planes? */
    if ((bpp != 8) && (bpp != 24)) {
        return NULL;
    }

    bmp_info->bytes_per_line = fp_igetw(f);

    for (c=0; c<60; c++)             /* skip some more junk */
        fp_getc(f);

    pitch = bmp_info->bytes_per_line * bpp / 8;
    size = pitch * height;
    bmp->bits = bits;
	bmp->depth = bpp;
    bmp->w     = width;
    bmp->h     = height;
    bmp->flags = MYBMP_FLOW_DOWN;
    bmp->pitch = pitch;
    bmp->frames = 1;
    bmp->size  = size;
	
//	bmp_info = & (bmp_info->bytes_per_line);
	
	return bmp_info;

}

void __mg_cleanup_pcx (void * init_info)
{
	if (init_info)
		free(init_info);
	init_info = NULL;

}
int __mg_load_pcx (MG_RWops* f, void* init_info, MYBITMAP *bmp, CB_ONE_SCANLINE cb, void* context)
{
	init_info_t *info = (init_info_t *)init_info;
	int c;
	int xx, po;
    int x, y;
	char ch;
	BYTE * bits;

	bits=bmp->bits;
    for (y=0; y<bmp->h; y++, bits +=bmp->pitch) {       /* read RLE encoded PCX data */
        x = xx = 0;
        po = 0;

        while (x < bmp->pitch) {
            ch = fp_getc(f);
            if ((ch & 0xC0) == 0xC0) {
                c = (ch & 0x3F);
                ch = fp_getc(f);
            }
            else
                c = 1;

            if (bmp->depth == 8) {
                while (c--) {
                    if (x < bmp->w)
                        bmp->bits [x] = ch;
                    x++;
                }
            }
            else {
                while (c--) {
                    if (xx < bmp->w)
                        bmp->bits [xx*3+po] = ch;
                    x++;
                    if (x == info->bytes_per_line) {
                        xx = 0;
                        po = 1;
                    }
                    else if (x == info->bytes_per_line*2) {
                        xx = 0;
                        po = 2;
                    }
                    else
                        xx++;
                }
            }
        }
        if (cb ) cb(context, bmp, y);
    }
#if 0
    if (bpp == 8) {                  /* look for a 256 color palette */
        while (!MGUI_RWeof(f)) { 
            if (fp_getc(f)==12) {
                for (c=0; c<256; c++) {
                    pal[c].r = fp_getc(f);
                    pal[c].g = fp_getc(f);
                    pal[c].b = fp_getc(f);
                }
                break;
            }
        }
    }
#endif
     return ERR_BMP_OK;
}

#if 0
/* save_pcx:
 *  Writes a bitmap into a PCX file, using the specified palette (this
 *  should be an array of at least 256 RGB structures).
 */
int save_pcx (FILE* f, MYBITMAP *bmp, RGB *pal)
{
   FILE *f;
   PALETTE tmppal;
   int c;
   int x, y;
   int runcount;
   int depth, planes;
   char runchar;
   char ch;

   if (!pal) {
      get_palette(tmppal);
      pal = tmppal;
   }

   f = fp_fopen(filename, F_WRITE);
   if (!f)
      return *allegro_errno;

   depth = bitmap_color_depth(bmp);
   if (depth == 8)
      planes = 1;
   else
      planes = 3;

   fp_putc(10, f);                      /* manufacturer */
   fp_putc(5, f);                       /* version */
   fp_putc(1, f);                       /* run length encoding  */
   fp_putc(8, f);                       /* 8 bits per pixel */
   fp_iputw(0, f);                      /* xmin */
   fp_iputw(0, f);                      /* ymin */
   fp_iputw(bmp->w-1, f);               /* xmax */
   fp_iputw(bmp->h-1, f);               /* ymax */
   fp_iputw(320, f);                    /* HDpi */
   fp_iputw(200, f);                    /* VDpi */

   for (c=0; c<16; c++) {
      fp_putc(_rgb_scale_6[pal[c].r], f);
      fp_putc(_rgb_scale_6[pal[c].g], f);
      fp_putc(_rgb_scale_6[pal[c].b], f);
   }

   fp_putc(0, f);                       /* reserved */
   fp_putc(planes, f);                  /* one or three color planes */
   fp_iputw(bmp->w, f);                 /* number of bytes per scanline */
   fp_iputw(1, f);                      /* color palette */
   fp_iputw(bmp->w, f);                 /* hscreen size */
   fp_iputw(bmp->h, f);                 /* vscreen size */
   for (c=0; c<54; c++)                   /* filler */
      fp_putc(0, f);

   for (y=0; y<bmp->h; y++) {             /* for each scanline... */
      runcount = 0;
      runchar = 0;
      for (x=0; x<bmp->w*planes; x++) {   /* for each pixel... */
     if (depth == 8) {
        ch = getpixel(bmp, x, y);
     }
     else {
        if (x<bmp->w) {
           c = getpixel(bmp, x, y);
           ch = getr_depth(depth, c);
        }
        else if (x<bmp->w*2) {
           c = getpixel(bmp, x-bmp->w, y);
           ch = getg_depth(depth, c);
        }
        else {
           c = getpixel(bmp, x-bmp->w*2, y);
           ch = getb_depth(depth, c);
        }
     }
     if (runcount==0) {
        runcount = 1;
        runchar = ch;
     }
     else {
        if ((ch != runchar) || (runcount >= 0x3f)) {
           if ((runcount > 1) || ((runchar & 0xC0) == 0xC0))
          fp_putc(0xC0 | runcount, f);
           fp_putc(runchar,f);
           runcount = 1;
           runchar = ch;
        }
        else
           runcount++;
     }
      }
      if ((runcount > 1) || ((runchar & 0xC0) == 0xC0))
     fp_putc(0xC0 | runcount, f);
      fp_putc(runchar,f);
   }

   if (depth == 8) {                      /* 256 color palette */
      fp_putc(12, f); 

      for (c=0; c<256; c++) {
     fp_putc(_rgb_scale_6[pal[c].r], f);
     fp_putc(_rgb_scale_6[pal[c].g], f);
     fp_putc(_rgb_scale_6[pal[c].b], f);
      }
   }

   fp_fclose(f);
   return *allegro_errno;
}

#endif

#endif /* _MGIMAGE_PCX */

