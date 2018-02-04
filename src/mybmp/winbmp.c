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
** Low-level Windows bitmap read/save function.
**
** Create date: 2000/08/26, derived from original bitmap.c
**
** Current maintainer: Wei Yongming.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "gdi.h"
#include "readbmp.h"

/************************* Bitmap-related structures  ************************/
typedef struct tagRGBTRIPLE
{
    BYTE    rgbtBlue;
    BYTE    rgbtGreen;
    BYTE    rgbtRed;
} RGBTRIPLE;
typedef RGBTRIPLE* PRGBTRIPLE;

typedef struct tagRGBQUAD
{
    BYTE    rgbBlue;
    BYTE    rgbGreen;
    BYTE    rgbRed;
    BYTE    rgbReserved;
} RGBQUAD;
typedef RGBQUAD* PRGBQUAD;

#define SIZEOF_RGBQUAD      4

#define BI_RGB          0
#define BI_RLE8         1
#define BI_RLE4         2
#define BI_BITFIELDS    3

#define SIZEOF_BMFH     14
#define SIZEOF_BMIH     40

#define OS2INFOHEADERSIZE  12
#define WININFOHEADERSIZE  40

typedef struct BITMAPFILEHEADER
{
   unsigned short bfType;
   unsigned long  bfSize;
   unsigned short bfReserved1;
   unsigned short bfReserved2;
   unsigned long  bfOffBits;
} BITMAPFILEHEADER;


/* Used for both OS/2 and Windows BMP. 
 * Contains only the parameters needed to load the image 
 */
typedef struct BITMAPINFOHEADER
{
   unsigned long  biWidth;
   unsigned long  biHeight;
   unsigned short biBitCount;
   unsigned long  biCompression;
} BITMAPINFOHEADER;


typedef struct WINBMPINFOHEADER  /* size: 40 */
{
   unsigned long  biSize;
   unsigned long  biWidth;
   unsigned long  biHeight;
   unsigned short biPlanes;
   unsigned short biBitCount;
   unsigned long  biCompression;
   unsigned long  biSizeImage;
   unsigned long  biXPelsPerMeter;
   unsigned long  biYPelsPerMeter;
   unsigned long  biClrUsed;
   unsigned long  biClrImportant;
} WINBMPINFOHEADER;


typedef struct OS2BMPINFOHEADER  /* size: 12 */
{
   unsigned long  biSize;
   unsigned short biWidth;
   unsigned short biHeight;
   unsigned short biPlanes;
   unsigned short biBitCount;
} OS2BMPINFOHEADER;


/* read_bmfileheader:
 *  Reads a BMP file header and check that it has the BMP magic number.
 */
static int read_bmfileheader(MG_RWops *f, BITMAPFILEHEADER *fileheader)
{
   fileheader->bfType = fp_igetw(f);
   fileheader->bfSize= fp_igetl(f);
   fileheader->bfReserved1= fp_igetw(f);
   fileheader->bfReserved2= fp_igetw(f);
   fileheader->bfOffBits= fp_igetl(f);

   if (fileheader->bfType != 19778)
      return -1;

   return 0;
}


/* read_win_bminfoheader:
 *  Reads information from a BMP file header.
 */
static int read_win_bminfoheader(MG_RWops *f, BITMAPINFOHEADER *infoheader)
{
   WINBMPINFOHEADER win_infoheader;

   win_infoheader.biWidth = fp_igetl(f);
   win_infoheader.biHeight = fp_igetl(f);
   win_infoheader.biPlanes = fp_igetw(f);
   win_infoheader.biBitCount = fp_igetw(f);
   win_infoheader.biCompression = fp_igetl(f);
   win_infoheader.biSizeImage = fp_igetl(f);
   win_infoheader.biXPelsPerMeter = fp_igetl(f);
   win_infoheader.biYPelsPerMeter = fp_igetl(f);
   win_infoheader.biClrUsed = fp_igetl(f);
   win_infoheader.biClrImportant = fp_igetl(f);


   infoheader->biWidth = win_infoheader.biWidth;
   infoheader->biHeight = win_infoheader.biHeight;
   infoheader->biBitCount = win_infoheader.biBitCount;
   infoheader->biCompression = win_infoheader.biCompression;

   return 0;
}


/* read_os2_bminfoheader:
 *  Reads information from an OS/2 format BMP file header.
 */
static int read_os2_bminfoheader(MG_RWops *f, BITMAPINFOHEADER *infoheader)
{
   OS2BMPINFOHEADER os2_infoheader;

   os2_infoheader.biWidth = fp_igetw(f);
   os2_infoheader.biHeight = fp_igetw(f);
   os2_infoheader.biPlanes = fp_igetw(f);
   os2_infoheader.biBitCount = fp_igetw(f);

   infoheader->biWidth = os2_infoheader.biWidth;
   infoheader->biHeight = os2_infoheader.biHeight;
   infoheader->biBitCount = os2_infoheader.biBitCount;
   infoheader->biCompression = 0;

   return 0;
}


/* read_bmicolors:
 *  Loads the color palette for 1,4,8 bit formats.
 */
static void read_bmicolors (int ncols, RGB *pal, MG_RWops *f, int win_flag)
{
   int i;

   for (i=0; i<ncols; i++) {
      pal[i].b = fp_getc(f);
      pal[i].g = fp_getc(f);
      pal[i].r = fp_getc(f);
      if (win_flag)
	    fp_getc(f);
   }
}

#define BMP_ERR     0
#define BMP_LINE    1
#define BMP_END     2
/* read_RLE8_compressed_image:
 *  For reading the 8 bit RLE compressed BMP image format.
 */
static int read_RLE8_compressed_image(MG_RWops * f, BYTE * bits, int pitch, int width)
{
    unsigned char count, val, val0;
    int j, pos = 0;
    int flag = BMP_ERR;

    while (pos <= width && flag == BMP_ERR) {
	    count = fp_getc(f);
	    val = fp_getc(f);

        if (count > 0) {
	        for (j = 0;j < count;j++) {
	            bits[pos] = val;
	            pos++;
	        }
	    }
	    else {
            switch (val) {

            case 0:                       /* end of line flag */
	            flag = BMP_LINE;
	        break;

	        case 1:                       /* end of picture flag */
	            flag = BMP_END;
	        break;

	        case 2:                       /* displace picture */
                count = fp_getc(f);
	            val = fp_getc(f);
	            pos += count;
	        break;

            default:                      /* read in absolute mode */
	            for (j = 0; j < val; j++) {
	                val0 = fp_getc(f);
                    bits[pos] = val0;
	                pos++;
	            }
	            if (j % 2 == 1)
	                val0 = fp_getc(f);    /* align on word boundary */
	        break;
	        }
	    }
    }

    return flag;
}

/* read_RLE4_compressed_image:
 *  For reading the 4 bit RLE compressed BMP image format.
 */

static int read_RLE4_compressed_image (MG_RWops *f, BYTE *bits, int pitch, int width)
{
    unsigned char b[8];
    unsigned char count;
    unsigned short val0, val;
    int j, k, pos = 0, flag = BMP_ERR;

    while (pos <= width && flag == BMP_ERR) {
        count = fp_getc(f);
        val = fp_getc(f);

        if (count > 0) {                    /* repeat pixels count times */
            b[1] = val & 15;
            b[0] = (val >> 4) & 15;
            for (j = 0; j < count; j++) {
                if (pos % 2 == 0)
                    bits[pos / 2] = b[0] << 4;
                else
                    bits[pos / 2] = bits[pos / 2] | b[1];
                if (pos >= width) return flag;
                pos++;
            }
        }
        else {
            switch (val) {
            case 0:                       /* end of line */
                flag = BMP_LINE;
            break;

            case 1:                       /* end of picture */
                flag = BMP_END;
            break;

            case 2:                       /* displace image */
                count = fp_getc(f);
                val = fp_getc(f);
                pos += count;
            break;

            default:                      /* read in absolute mode */
                for (j=0; j<val; j++) {
                    if ((j%4) == 0) {
                        val0 = fp_igetw(f);
                        for (k=0; k<2; k++) {
                            b[2*k+1] = val0 & 15;
                            val0 = val0 >> 4;
                            b[2*k] = val0 & 15;
                            val0 = val0 >> 4;
                        }
                    }

                    if (pos % 2 == 0)
                        bits [pos/2] = b[j%4] << 4;
                    else
                        bits [pos/2] = bits [pos/2] | b[j%4];
                    pos++;
                }
            break;

            }
        }
    }

    return flag;
}

/* read_16bit_image:
 *  For reading the 16-bit BMP image format.
 * This only support bit masks specific to Windows 95.
 */
static void read_16bit_image (MG_RWops *f, BYTE *bits, int pitch, int width, DWORD gmask)
{
    int i;
    WORD pixel;
    BYTE *line;

    line = bits;
    for (i = 0; i < width; i++) {
        pixel = fp_igetw (f);
        if (gmask == 0x03e0)    /* 5-5-5 */
        {
            line [2] = ((pixel >> 10) & 0x1f) << 3;
            line [1] = ((pixel >> 5) & 0x1f) << 3;
            line [0] = (pixel & 0x1f) << 3;
        }
        else                    /* 5-6-5 */
        {
            line [2] = ((pixel >> 11) & 0x1f) << 3;
            line [1] = ((pixel >> 5) & 0x3f) << 2;
            line [0] = (pixel & 0x1f) << 3;
        }

        line += 3;
    }

    if (width & 0x01)
        pixel = fp_igetw (f);   /* read the gap */

}

/* __mg_load_bmp:
 *  Loads a Windows BMP file, returning in the my_bitmap structure and storing
 *  the palette data in the specified palette (this should be an array of
 *  at least 256 RGB structures).
 *
 *  Thanks to Seymour Shlien for contributing this function.
 */

typedef struct {
    unsigned long  biWidth;
    unsigned long  biHeight;
    unsigned short biBitCount;
    unsigned long  biCompression;
    unsigned long  rmask, gmask, bmask;
} init_info_t;

void * __mg_init_bmp (MG_RWops* fp, MYBITMAP * bmp, RGB * pal)
{
    int effect_depth, biSize;
    int ncol;
    BITMAPFILEHEADER fileheader;
    BITMAPINFOHEADER infoheader;
    init_info_t * bmp_info = NULL;

    bmp_info = (init_info_t *)malloc(sizeof(init_info_t));
    if (!bmp_info)
        return NULL;

    bmp_info->rmask = 0x001f;
    bmp_info->gmask = 0x03e0;
    bmp_info->bmask = 0x7c00;
    if (read_bmfileheader (fp, &fileheader) != 0)
        goto err;

    biSize = fp_igetl(fp);
    if (biSize >= WININFOHEADERSIZE) {
        if (read_win_bminfoheader (fp, &infoheader) != 0)
            goto err;

        MGUI_RWseek (fp, biSize - WININFOHEADERSIZE, SEEK_CUR);
        ncol = (fileheader.bfOffBits - biSize - 14) / 4;
        
        /* there only 1,4,8 bit read color panel data */
        if (infoheader.biBitCount <= 8)
            read_bmicolors(ncol, pal, fp, 1);
    }
    else if (biSize == OS2INFOHEADERSIZE) {
        if (read_os2_bminfoheader (fp, &infoheader) != 0)
            goto err;
        ncol = (fileheader.bfOffBits - 26) / 3;

        if (infoheader.biBitCount <= 8)
            read_bmicolors (ncol, pal, fp, 0);
    }
    else
        goto err;

    if (infoheader.biBitCount == 16)
        effect_depth = 24;
    else
        effect_depth = infoheader.biBitCount;

    bmp_ComputePitch(effect_depth, infoheader.biWidth, (Uint32 *)(&bmp->pitch), TRUE);

    bmp->flags |= MYBMP_TYPE_BGR | MYBMP_FLOW_DOWN;
    bmp->depth = effect_depth;
    bmp->w     = infoheader.biWidth;
    bmp->h     = infoheader.biHeight;
    bmp->frames = 1;
    bmp->size  = biSize;

    *(BITMAPINFOHEADER *)bmp_info = infoheader;

    return bmp_info;

err:
    free(bmp_info);
    return NULL;
}

void __mg_cleanup_bmp (void* init_info)
{
    if (init_info)
        free(init_info);

    init_info = NULL;
}

int __mg_load_bmp(MG_RWops* fp, void* init_info, MYBITMAP *bmp, CB_ONE_SCANLINE cb, void* context)
{
    init_info_t * info = (init_info_t *)init_info;
    int i, flag;
    int pitch = 0;
    BYTE * bits;

    if (!(bmp->flags & MYBMP_LOAD_ALLOCATE_ONE))
        pitch = bmp->pitch;

    switch (info->biCompression) {
        case BI_BITFIELDS:
            info->rmask = fp_igetl(fp);
            info->gmask = fp_igetl(fp);
            info->bmask = fp_igetl(fp);
        break;

        case BI_RGB:
            if (info->biBitCount == 16)
                bmp->flags |= MYBMP_RGBSIZE_3;
            else if (info->biBitCount == 32)
                bmp->flags |= MYBMP_RGBSIZE_4;
            else
                bmp->flags |= MYBMP_RGBSIZE_3;
        break;

        case BI_RLE8:
        case BI_RLE4:
        break;

        default:
            goto err;
    }

    flag = BMP_LINE;
    bits = bmp->bits + (bmp->h - 1) * pitch;
    for (i = bmp->h - 1; i >= 0; i--, bits -= pitch) {
        switch (info->biCompression) {
        case BI_BITFIELDS:
        case BI_RGB:
            if (info->biBitCount == 16)
                read_16bit_image(fp, bits, bmp->pitch, bmp->w, info->gmask);
            else if (info->biBitCount == 32)
                MGUI_RWread(fp, bits, 1, bmp->pitch);
            else
                MGUI_RWread(fp, bits, 1, bmp->pitch);
        break;

        case BI_RLE8:
            flag = read_RLE8_compressed_image(fp, bits, bmp->pitch, bmp->w);
            if (flag == BMP_ERR)
                goto err;
        break;

        case BI_RLE4:
            flag = read_RLE4_compressed_image(fp, bits, bmp->pitch, bmp->w);
            if (flag == BMP_ERR)
                goto err;
        break;

        }

        if (cb && !pitch) cb(context, bmp, i);

        if (flag == BMP_END)
            goto ret;
    }

ret:
    return ERR_BMP_OK;

err:
    return ERR_BMP_OTHER;
}

BOOL __mg_check_bmp (MG_RWops* fp)
{
   WORD bfType = fp_igetw (fp);

   if (bfType != 19778)
      return FALSE;

   return TRUE;
}

#ifdef _MGMISC_SAVEBITMAP
static void bmpGet16CScanline(BYTE* bits, BYTE* scanline, 
                        int pixels)
{
    int i;

    for (i = 0; i < pixels; i++) {
        if (i % 2 == 0)
            *scanline = (bits [i] << 4) & 0xF0;
        else {
            *scanline |= bits [i] & 0x0F;
            scanline ++;
        }
    }
}

static inline void bmpGet256CScanline (BYTE* bits, BYTE* scanline, 
                        int pixels)
{
    memcpy (scanline, bits, pixels);
}

static inline void pixel2rgb (gal_pixel pixel, GAL_Color* color, int depth)
{
    switch (depth) {
    case 24:
    case 32:
        color->r = (gal_uint8) ((pixel >> 16) & 0xFF);
        color->g = (gal_uint8) ((pixel >> 8) & 0xFF);
        color->b = (gal_uint8) (pixel & 0xFF);
        break;

    case 15:
        color->r = (gal_uint8)((pixel & 0x7C00) >> 7) | 0x07;
        color->g = (gal_uint8)((pixel & 0x03E0) >> 2) | 0x07;
        color->b = (gal_uint8)((pixel & 0x001F) << 3) | 0x07;
        break;

    case 16:
        color->r = (gal_uint8)((pixel & 0xF800) >> 8) | 0x07;
        color->g = (gal_uint8)((pixel & 0x07E0) >> 3) | 0x03;
        color->b = (gal_uint8)((pixel & 0x001F) << 3) | 0x07;
        break;
    }
}

static void bmpGetHighCScanline (BYTE* bits, BYTE* scanline, 
                        int pixels, int bpp, int depth)
{
    int i;
    gal_pixel c;
    GAL_Color color;
    memset (&color, 0, sizeof(GAL_Color));

    for (i = 0; i < pixels; i++) {
        c = *((gal_pixel*)bits);

        pixel2rgb (c, &color, depth);
        *(scanline)     = color.b;
        *(scanline + 1) = color.g;
        *(scanline + 2) = color.r;

        bits += bpp;
        scanline += 3;
    }
}

inline static int depth2bpp (int depth)
{
    switch (depth) {
    case 4:
    case 8:
        return 1;
    case 15:
    case 16:
        return 2;
    case 24:
        return 3;
    case 32:
        return 4;
    }
    
    return 1;
}

int __mg_save_bmp (MG_RWops* fp, MYBITMAP* bmp, RGB* pal)
{
    BYTE* scanline = NULL;
    int i, bpp;
    int scanlinebytes;

    BITMAPFILEHEADER bmfh;
    WINBMPINFOHEADER bmih;

    memset (&bmfh, 0, sizeof (BITMAPFILEHEADER));
    bmfh.bfType         = MAKEWORD16 ('B', 'M');
    bmfh.bfReserved1    = 0;
    bmfh.bfReserved2    = 0;

    memset (&bmih, 0, sizeof (WINBMPINFOHEADER));
    bmih.biSize         = (unsigned long)(WININFOHEADERSIZE);
    bmih.biWidth        = (unsigned long)(bmp->w);
    bmih.biHeight       = (unsigned long)(bmp->h);
    bmih.biPlanes       = 1;
    bmih.biCompression  = BI_RGB;

    bpp = depth2bpp (bmp->depth);
    switch (bmp->depth) {
        case 4:
            scanlinebytes       = (bmih.biWidth + 1)>>1;
            scanlinebytes       = ((scanlinebytes + 3)>>2)<<2;

#ifdef HAVE_ALLOCA
            if (!(scanline = alloca (scanlinebytes))) return ERR_BMP_MEM;
#else
            if (!(scanline = malloc (scanlinebytes))) return ERR_BMP_MEM;
#endif
            memset (scanline, 0, scanlinebytes);

            bmih.biSizeImage    = (DWORD)(bmih.biHeight*scanlinebytes);
            bmfh.bfOffBits      = SIZEOF_BMFH + SIZEOF_BMIH
                                    + (SIZEOF_RGBQUAD<<4);
            bmfh.bfSize         = (DWORD)(bmfh.bfOffBits + bmih.biSizeImage);
            bmih.biBitCount     = 4;
            bmih.biClrUsed      = 16L;
            bmih.biClrImportant = 16L;

            MGUI_RWwrite (fp, &bmfh.bfType, sizeof (WORD16), 1);
            MGUI_RWwrite (fp, &bmfh.bfSize, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmfh.bfReserved1, sizeof (WORD16), 1);
            MGUI_RWwrite (fp, &bmfh.bfReserved2, sizeof (WORD16), 1);
            MGUI_RWwrite (fp, &bmfh.bfOffBits, sizeof (DWORD32), 1);
            
            MGUI_RWwrite (fp, &bmih.biSize, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biWidth, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biHeight, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biPlanes, sizeof (WORD16), 1);
            MGUI_RWwrite (fp, &bmih.biBitCount, sizeof (WORD16), 1);
            MGUI_RWwrite (fp, &bmih.biCompression, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biSizeImage, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biXPelsPerMeter, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biYPelsPerMeter, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biClrUsed, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biClrImportant, sizeof (DWORD32), 1);

            for (i = 0; i < 16; i++) {
                RGBQUAD rgbquad;
                rgbquad.rgbRed = pal [i].r;
                rgbquad.rgbBlue = pal [i].b;
                rgbquad.rgbGreen = pal [i].g;
                MGUI_RWwrite (fp, &rgbquad, sizeof (char), sizeof (RGBQUAD));
            }
            
            for (i = bmp->h  - 1; i >= 0; i--) {
                bmpGet16CScanline (bmp->bits + i * bmp->pitch, scanline, bmp->w);
                MGUI_RWwrite (fp, scanline, sizeof (char), scanlinebytes);
            }
        break;

        case 8:
            scanlinebytes       = bmih.biWidth;
            scanlinebytes       = ((scanlinebytes + 3)>>2)<<2;

#ifdef HAVE_ALLOCA
            if (!(scanline = alloca (scanlinebytes))) return ERR_BMP_MEM;
#else
            if (!(scanline = malloc (scanlinebytes))) return ERR_BMP_MEM;
#endif
            memset (scanline, 0, scanlinebytes);

            bmih.biSizeImage    = bmih.biHeight*scanlinebytes;
            bmfh.bfOffBits      = SIZEOF_BMFH + SIZEOF_BMIH
                                    + (SIZEOF_RGBQUAD<<8);
            bmfh.bfSize         = bmfh.bfOffBits + bmih.biSizeImage;
            bmih.biBitCount     = 8;
            bmih.biClrUsed      = 256;
            bmih.biClrImportant = 256;

            MGUI_RWwrite (fp, &bmfh.bfType, sizeof (WORD16), 1);
            MGUI_RWwrite (fp, &bmfh.bfSize, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmfh.bfReserved1, sizeof (WORD16), 1);
            MGUI_RWwrite (fp, &bmfh.bfReserved2, sizeof (WORD16), 1);
            MGUI_RWwrite (fp, &bmfh.bfOffBits, sizeof (DWORD32), 1);

            MGUI_RWwrite (fp, &bmih.biSize, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biWidth, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biHeight, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biPlanes, sizeof (WORD16), 1);
            MGUI_RWwrite (fp, &bmih.biBitCount, sizeof (WORD16), 1);
            MGUI_RWwrite (fp, &bmih.biCompression, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biSizeImage, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biXPelsPerMeter, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biYPelsPerMeter, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biClrUsed, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biClrImportant, sizeof (DWORD32), 1);
            
            for (i = 0; i < 256; i++) {
                RGBQUAD rgbquad;
                rgbquad.rgbRed = pal [i].r;
                rgbquad.rgbBlue = pal [i].b;
                rgbquad.rgbGreen = pal [i].g;
                MGUI_RWwrite (fp, &rgbquad, sizeof (char), sizeof (RGBQUAD));
            }
            
            for (i = bmp->h - 1; i >= 0; i--) {
                bmpGet256CScanline (bmp->bits + bmp->pitch * i, scanline, bmp->w);
                MGUI_RWwrite (fp, scanline, sizeof (char), scanlinebytes);
            }
        break;

        default:
            scanlinebytes       = bmih.biWidth*3;
            scanlinebytes       = ((scanlinebytes + 3)>>2)<<2;

#ifdef HAVE_ALLOCA
            if (!(scanline = alloca (scanlinebytes))) return ERR_BMP_MEM;
#else
            if (!(scanline = malloc (scanlinebytes))) return ERR_BMP_MEM;
#endif
            memset (scanline, 0, scanlinebytes);

            bmih.biSizeImage    = bmih.biHeight*scanlinebytes;
            bmfh.bfOffBits      = SIZEOF_BMFH + SIZEOF_BMIH;
            bmfh.bfSize         = bmfh.bfOffBits + bmih.biSizeImage;
            bmih.biBitCount     = 24;

            MGUI_RWwrite (fp, &bmfh.bfType, sizeof (WORD16), 1);
            MGUI_RWwrite (fp, &bmfh.bfSize, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmfh.bfReserved1, sizeof (WORD16), 1);
            MGUI_RWwrite (fp, &bmfh.bfReserved2, sizeof (WORD16), 1);
            MGUI_RWwrite (fp, &bmfh.bfOffBits, sizeof (DWORD32), 1);
            
            MGUI_RWwrite (fp, &bmih.biSize, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biWidth, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biHeight, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biPlanes, sizeof (WORD16), 1);
            MGUI_RWwrite (fp, &bmih.biBitCount, sizeof (WORD16), 1);
            MGUI_RWwrite (fp, &bmih.biCompression, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biSizeImage, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biXPelsPerMeter, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biYPelsPerMeter, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biClrUsed, sizeof (DWORD32), 1);
            MGUI_RWwrite (fp, &bmih.biClrImportant, sizeof (DWORD32), 1);

            for (i = bmp->h - 1; i >= 0; i--) {
                bmpGetHighCScanline (bmp->bits + i * bmp->pitch, scanline, 
                                bmp->w, bpp, bmp->depth);
                MGUI_RWwrite (fp, scanline, sizeof (char), scanlinebytes);
            }
        break;
    }

#ifndef HAVE_ALLOCA
    free (scanline);
#endif

    return ERR_BMP_OK;
}

#endif

