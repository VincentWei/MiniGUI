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
** pcx.c: Low-level LBM/PBM bitmap file read/save routines.
** 
** Some code comes from lbm.c of Allegro by Shawn Hargreaves.
** 
** Current maintainer:  Wei Yongming.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "gdi.h"
#include "readbmp.h"

#ifdef _MGIMAGE_LBM

static void bmpSetBitsInLine (int bpp, BYTE* bits, int pos, BYTE* value)
{
    int offset, shift;

    switch (bpp) {
    case 1:
        offset = pos / 8;
        shift = 8 - (pos % 8);
        bits [offset] |= (*value & 0x01) << shift;
        break;
    case 4:
        offset = pos / 2;
        shift = 4 - ((pos % 2) << 2);
        bits [offset] |= (*value & 0x0F) << shift;
        break;
    case 8:
        offset = pos;
        bits [offset] |= *value;
        break;
    case 15:
    case 16:
        offset = pos * 2;
        bits [offset] |= value [0];
        bits [offset + 1] |= value [1];
        break;
    case 24:
        offset = pos * 3;
        bits [offset] |= value [0];
        bits [offset + 1] |= value [1];
        bits [offset + 2] |= value [2];
        break;
    case 32:
        offset = pos * 4;
        bits [offset] |= value [0];
        bits [offset + 1] |= value [1];
        bits [offset + 2] |= value [2];
        bits [offset + 3] |= value [3];
        break;
    }
}
typedef struct {
	int pbm_mode;
	char cmp_type;
	unsigned char check_flags;
	MG_RWops *ff;

} lbm_init_info_t;

void * __mg_init_lbm (MG_RWops *f, MYBITMAP * bmp, RGB *pal)
{
   	#define IFF_FORM     0x4D524F46     /* 'FORM' - IFF FORM structure  */
   	#define IFF_ILBM     0x4D424C49     /* 'ILBM' - interleaved bitmap  */
   	#define IFF_PBM      0x204D4250     /* 'PBM ' - new DP2e format     */
   	#define IFF_BMHD     0x44484D42     /* 'BMHD' - bitmap header       */
   	#define IFF_CMAP     0x50414D43     /* 'CMAP' - color map (palette) */
   	#define IFF_BODY     0x59444F42     /* 'BODY' - bitmap data         */

   	BYTE* bits;
   	int w, h, i, bpl, pbm_mode;
   	char cmp_type, color_depth;
   	unsigned char check_flags;
   	long id, len, l;
   	unsigned long size;
   	lbm_init_info_t * lbm_info=NULL;
   	lbm_info=(lbm_init_info_t *)malloc(sizeof(lbm_init_info_t));
   	if(!lbm_info)
			return NULL; 

   	id = fp_igetl(f);              /* read file header    */
   	if (id != IFF_FORM) {            /* check for 'FORM' id */
      	goto err;
   	}

   	fp_igetl(f);                   /* skip FORM length    */

   	id = fp_igetl(f);              /* read id             */

   	/* check image type ('ILBM' or 'PBM ') */
   	if ((id != IFF_ILBM) && (id != IFF_PBM)) {
      goto err;
	}

   	pbm_mode = id == IFF_PBM;

   	id = fp_igetl(f);              /* read id               */
   	if (id != IFF_BMHD) {            /* check for header      */
      	goto err;
   	}

   	len = fp_mgetl(f);             /* read header length    */
   	if (len != 20) {                 /* check, if it is right */
    	goto err;
   	}

   	w = fp_mgetw(f);               /* read screen width  */

   	h = fp_mgetw(f);               /* read screen height */

   	fp_igetw(f);                   /* skip initial x position  */
   	fp_igetw(f);                   /* skip initial y position  */

   	color_depth = fp_getc(f);      /* get image depth   */
   	if (color_depth > 8) {
      	goto err;
   	}

   	fp_getc(f);                    /* skip masking type */

   	cmp_type = fp_getc(f);         /* get compression type */
   	if ((cmp_type != 0) && (cmp_type != 1)) {
      goto err;
   	}

   	fp_getc(f);                    /* skip unused field        */
   	fp_igetw(f);                   /* skip transparent color   */
   	fp_getc(f);                    /* skip x aspect ratio      */
   	fp_getc(f);                    /* skip y aspect ratio      */
   	fp_igetw(f);                   /* skip default page width  */
   	fp_igetw(f);                   /* skip default page height */

	lbm_info->ff=f;
   	check_flags = 0;
   	do {  /* We'll use cycle to skip possible junk      */
         /*  chunks: ANNO, CAMG, GRAB, DEST, TEXT etc. */
      	id = fp_igetl(f);
		switch(id) {

         	case IFF_CMAP:
            	memset(pal, 0, 256 * 3);
            	len = fp_mgetl(f) / 3;
            	for (i=0; i<len; i++) {
               		pal[i].r = fp_getc(f);
               		pal[i].g = fp_getc(f);
               		pal[i].b = fp_getc(f);
            	}
            	check_flags |= 1;       /* flag "palette read" */
            	break;
         	default:                   /* skip useless chunks  */
            	len = fp_mgetl (f);
            	if (len & 1)
               		len++;
            	for (l=0; l < (len >> 1); l++)
               		fp_igetw (f);
      	}

      /* Exit from loop if we are at the end of file, */
      /* or if we loaded both bitmap and palette      */

	} while ((check_flags != 1) && (!MGUI_RWeof (f)));

   	if (check_flags != 1) {
      if (check_flags & 2)
         goto err;
   	}
   	if (pbm_mode)
		bpl = w;
   	else {
		bpl = w >> 3;        /* calc bytes per line  */
        if (w & 7)           /* for finish bits      */
        	bpl++;
   	}
   	if (bpl & 1)            /* alignment            */
   		bpl++;
   	size = bpl * w * h;

   	if (!(bits = malloc (size))) {
   		goto err;
   	}
   	memset (bits, 0, size);

   	bmp->bits = bits;
   	bmp->pitch = bpl;
   	bmp->size  = size;
   	bmp->flags = MYBMP_FLOW_DOWN;
   	bmp->depth = color_depth;
   	bmp->w     = w;
   	bmp->h     = h;
   	bmp->frames = 1;
	lbm_info->pbm_mode=pbm_mode;
	lbm_info->cmp_type=cmp_type;
	lbm_info->check_flags=check_flags;
	return lbm_info;
err:
	free(lbm_info);
	return NULL;
}

void __mg_cleanup_lbm(void * init_info)
{
	if (init_info)
		free(init_info);
	init_info=NULL;
}
/* __mg_load_lbm:
 *  Loads IFF ILBM/PBM files with up to 8 bits per pixel, returning
 *  in a bitmap structure and storing the palette data in the specified
 *  palette (this should be an array of at least 256 RGB structures).
 */
int __mg_load_lbm (MG_RWops* f, void * init_info, MYBITMAP* bmp, CB_ONE_SCANLINE cb, void * context)
{
	BYTE* bits;
   	int i, x, y, ppl, bpl;
   	char ch, bit_plane;
   	unsigned char uc, check_flags, bit_mask, *line_buf;
   	long id, len, l;
   	unsigned long size;
	lbm_init_info_t * info = (lbm_init_info_t *)init_info;
	check_flags=info->check_flags;
	bits=bmp->bits;
	bpl = bmp->pitch;
	f=info->ff;
   	do {  /* We'll use cycle to skip possible junk      */
         /*  chunks: ANNO, CAMG, GRAB, DEST, TEXT etc. */
      	id = fp_igetl(f);
     
      	switch(id) {
         	case IFF_BODY:
            	fp_igetl(f);          /* skip BODY size */
            	line_buf = malloc (bmp->pitch);
            	if (!line_buf) {
                	return ERR_BMP_MEM;
            	}

            	if (info->pbm_mode) {
               		for (y = 0; y < bmp->h; y++, bits += bpl) {
                  		if (info->cmp_type) {
                     		i = 0;
                     		while (i < bmp->pitch) {
                        		uc = fp_getc(f);
                        		if (uc < 128) {
                           			uc++;
                           			MGUI_RWread (f, &line_buf[i], 1, uc);
                           			i += uc;
                        		}
                        		else if (uc > 128) {
                           			uc = 257 - uc;
                           			ch = fp_getc(f);
                           			memset (&line_buf[i], ch, uc);
                           			i += uc;
                        		}
                        		/* 128 (0x80) means NOP - no operation  */
                     		}
                  		}
                  		else  /* pure theoretical situation */
                     		MGUI_RWread (f, line_buf, 1, bmp->pitch);
                  
                  		for (i = 0; i < bmp->pitch; i++)
                    		bmpSetBitsInLine (bmp->depth, bmp->bits, i, line_buf + i);
				if(cb) cb(context,bmp,y);
               		}
            	}
            	else {
               		for (y = 0; y < bmp->h; y++, bits += bpl) {
                  		for (bit_plane = 0; bit_plane < bmp->depth; bit_plane++) {
                     		if (info->cmp_type) {
                        		i = 0;
                        		while (i < bmp->pitch) {
                           			uc = fp_getc(f);
                           			if (uc < 128) {
                              			uc++;
                              			MGUI_RWread (f, &line_buf[i], 1, uc);
                              			i += uc;
                           			}
                           			else if (uc > 128) {
                              			uc = 257 - uc;
                              			ch = fp_getc(f);
                              			memset (&line_buf[i], ch, uc);
                              			i += uc;
                           			}
                           			/* 128 (0x80) means NOP - no operation  */
                        		}
                     		}
                     		else
                        		MGUI_RWread (f, line_buf, 1, bmp->pitch);
                     		bit_mask = 1 << bit_plane;
                     		ppl = bmp->pitch;     /* for all pixel blocks */
                     		if (bmp->w & 7)     /*  may be, except the  */
                        		ppl--;      /*  the last            */

                     		for (x = 0; x < ppl; x++) {
                        		if (line_buf[x] & 128)
                           			bmpSetBitsInLine (bmp->depth, bmp->bits, x * 8, &bit_mask);
                        		if (line_buf[x] & 64)
                           			bmpSetBitsInLine (bmp->depth, bmp->bits, x * 8 + 1, &bit_mask);
                        		if (line_buf[x] & 32)
                           			bmpSetBitsInLine (bmp->depth, bmp->bits, x * 8 + 2, &bit_mask);
                        		if (line_buf[x] & 16)
                           			bmpSetBitsInLine (bmp->depth, bmp->bits, x * 8 + 3, &bit_mask);
                        		if (line_buf[x] & 8)
                           			bmpSetBitsInLine (bmp->depth, bmp->bits, x * 8 + 4, &bit_mask);
                        		if (line_buf[x] & 4)
                           			bmpSetBitsInLine (bmp->depth, bmp->bits, x * 8 + 5, &bit_mask);
                        		if (line_buf[x] & 2)
                           			bmpSetBitsInLine (bmp->depth, bmp->bits, x * 8 + 6, &bit_mask);
                        		if (line_buf[x] & 1)
                           			bmpSetBitsInLine (bmp->depth, bmp->bits, x * 8 + 7, &bit_mask);
                     		}

                     			/* last pixel block */
                     		if (bmp->w & 7) {
                        		x = bmp->pitch - 1;
                        		/* no necessary to check if (w & 7) > 0 in */
		                        /* first condition, because (w & 7) != 0   */
                        		if (line_buf[x] & 128)
                           			bmpSetBitsInLine (bmp->depth, bmp->bits, x * 8, &bit_mask);
                        		if ((line_buf[x] & 64) && ((bmp->w & 7) > 1))
                           			bmpSetBitsInLine (bmp->depth, bmp->bits, x * 8 + 1, &bit_mask);
                        		if ((line_buf[x] & 32) && ((bmp->w & 7) > 2))
                           			bmpSetBitsInLine (bmp->depth, bmp->bits, x * 8 + 2, &bit_mask);
                        		if ((line_buf[x] & 16) && ((bmp->w & 7) > 3))
                           			bmpSetBitsInLine (bmp->depth, bmp->bits, x * 8 + 3, &bit_mask);
                        		if ((line_buf[x] & 8)  && ((bmp->w & 7) > 4))
                           			bmpSetBitsInLine (bmp->depth, bmp->bits, x * 8 + 4, &bit_mask);
                        		if ((line_buf[x] & 4)  && ((bmp->w & 7) > 5))
                           			bmpSetBitsInLine (bmp->depth, bmp->bits, x * 8 + 5, &bit_mask);
                        		if ((line_buf[x] & 2)  && ((bmp->w & 7) > 6))
                           			bmpSetBitsInLine (bmp->depth, bmp->bits, x * 8 + 6, &bit_mask);
                        		if ((line_buf[x] & 1)  && ((bmp->w & 7) > 7))
                           			bmpSetBitsInLine (bmp->depth, bmp->bits, x * 8 + 7, &bit_mask);
                     		}
                  		}

        				if(cb) cb(context, bmp, y);
       				}
            	}
            	free (line_buf);
            	check_flags |= 2;       /* flag "bitmap read" */
            	break;

         	default:                   /* skip useless chunks  */
            	len = fp_mgetl (f);
            	if (len & 1)
               		len++;
            	for (l=0; l < (len >> 1); l++)
               		fp_igetw (f);
      	}

      /* Exit from loop if we are at the end of file, */
      /* or if we loaded both bitmap and palette      */

   } while ((check_flags != 3) && (!MGUI_RWeof (f)));

   if (check_flags != 3) {
      if (check_flags & 2)
         return ERR_BMP_LOAD;
   }
   
   return ERR_BMP_OK;
}

BOOL __mg_check_lbm (MG_RWops* fp)
{
   long id, len;

   id = fp_igetl(fp);              /* read file header    */
   if (id != IFF_FORM) {           /* check for 'FORM' id */
      return FALSE;
   }

   fp_igetl(fp);                   /* skip FORM length    */

   id = fp_igetl(fp);              /* read id             */

   /* check image type ('ILBM' or 'PBM ') */
   if ((id != IFF_ILBM) && (id != IFF_PBM)) {
      return FALSE;
   }

   id = fp_igetl(fp);              /* read id               */
   if (id != IFF_BMHD) {            /* check for header      */
      return FALSE;
   }

   len = fp_mgetl(fp);             /* read header length    */
   if (len != 20) {                 /* check, if it is right */
      return FALSE;
   }

    return TRUE;
}

#endif /* _MGIMAGE_LBM */


