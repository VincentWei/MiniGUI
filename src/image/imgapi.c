//
// imgapi.c: The GIF and JPG Image Support API module.
//
// Copyright (C) 1999, Li Zhuo.
// Copyright (C) 1999, 2000, EESG of ICT.
//
// Current maintainer: Wei Yongming.

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

// Create date: 1999.04.19
//
// Modify records:
//
//  Who             When        Where       For What                Status
//-----------------------------------------------------------------------------
//
// TODO:
//

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <malloc.h>
#include <string.h>

#include "common.h"
#include "gdi.h"
#include "gif.h"
#include "jpg.h"

#ifndef lint
static char fileid[] = "$Id: imgapi.c 4781 2005-03-21 07:15:41Z snig $";
#endif

int GUIAPI CreateBitmapFromGIFFile (HDC hdc, PBITMAP pBitmap, const char* filename)
{
    int buffer_length;
    FILE* file;
    char* buffer;
    int retval;
    if ((file=fopen(filename,"rb"))==NULL) {
        return -1;
    }
    fseek(file,0,SEEK_END);
    buffer_length = ftell(file)+1;
    buffer=(char*)malloc(buffer_length);
    fseek(file,0,SEEK_SET);
    fread(buffer, sizeof(char), buffer_length, file);
    fclose(file);
    retval=CreateBitmapFromGIFMemory (hdc, pBitmap,buffer,buffer_length);
    free(buffer);
    return retval;
}

int GUIAPI CreateBitmapFromGIFMemory (HDC hdc, PBITMAP pBitmap, void* buffer, int buffer_length)
{
    unsigned char* pBmpSurface;
    unsigned char* pGifSurface;
    int i,j;
    int bpp = GetGDCapability (hdc, GDCAP_BPP);

    gif_info_t *ginfo;
    ginfo = (gif_info_t*)calloc(1, sizeof(gif_info_t));
    ginfo->gif_buffer_length=buffer_length;
	ginfo->gif_buffer=buffer;

    unpack_gif(ginfo);
    if (pBitmap!=NULL) {
        pBitmap->bmWidth=ginfo->gif_global.width;
        pBitmap->bmHeight=ginfo->gif_global.height;
        pBitmap->bmBits=malloc(
                pBitmap->bmWidth*pBitmap->bmHeight*bpp);
    }

    pGifSurface=ginfo->gif_surface;
    pBmpSurface=pBitmap->bmBits;

    for(i=0;i<ginfo->gif_global.height;i++) {
        for(j=0;j<ginfo->gif_global.width;j++) {
            int c;
            int r,g,b;
            r=*pGifSurface++;
            g=*pGifSurface++;
            b=*pGifSurface++;
            
            c = RGB2Pixel (hdc, r, g, b);
            switch (bpp) {
                case 1:
                    *pBmpSurface = c;
                    pBmpSurface++;
                    break;
                case 2:
                    *(ushort *) pBmpSurface = c;
                    pBmpSurface += 2;
                    break;
                case 3:
                    *(ushort *) pBmpSurface = c;
                    *(pBmpSurface + 2) = c >> 16;
                    pBmpSurface += 3;
                    break;
                case 4:
                    *(uint *) pBmpSurface = c;
                    pBmpSurface += 4;
                    break;
            }
        }
    }
    free(ginfo);
    //destroy_gif(ginfo);
	return 0;
}

int GUIAPI CreateBitmapFromJPGFile (HDC hdc, PBITMAP pBitmap, const char* filename)
{
    int buffer_length;
    FILE* file;
    char* buffer;
    int retval;
    if ((file=fopen(filename,"rb"))==NULL) {
        return -1;
    }
    fseek(file,0,SEEK_END);
    buffer_length = ftell(file)+1;
    buffer=(char*)malloc(buffer_length);
    fseek(file,0,SEEK_SET);
    fread(buffer, sizeof(char), buffer_length, file);
    fclose(file);
    retval = CreateBitmapFromJPGMemory (hdc, pBitmap,buffer,buffer_length);
    free(buffer);
    return retval;
}

int GUIAPI CreateBitmapFromJPGMemory (HDC hdc, PBITMAP pBitmap, void* buffer, int buffer_length)
{
    unsigned char* pBmpSurface;
    unsigned char* pJPGSurface;
    int i,j;
    int bpp = GetGDCapability (hdc, GDCAP_BPP);

    jpg_info_t *jinfo;
     jinfo = (jpg_info_t*)calloc(1, sizeof(jpg_info_t));

    jinfo->jpg_buffer_length=buffer_length;
	jinfo->jpg_buffer=buffer;

    unpack_jpg(jinfo);
    if (pBitmap!=NULL) {
        pBitmap->bmWidth=jinfo->width;
        pBitmap->bmHeight=jinfo->height;
        pBitmap->bmBits=
            malloc(pBitmap->bmWidth*pBitmap->bmHeight*bpp);
    }

    pJPGSurface=jinfo->jpg_surface;
    pBmpSurface=pBitmap->bmBits;

    for(i=0;i<jinfo->height;i++) {
        for(j=0;j<jinfo->width;j++) {
            int c;
            int r,g,b;
            r=*pJPGSurface++;
            g=*pJPGSurface++;
            b=*pJPGSurface++;

            c = RGB2Pixel (hdc, r, g, b);
            switch (bpp) {
                case 1:
                    *pBmpSurface = c;
                    pBmpSurface++;
                    break;
                case 2:
                    *(ushort *) pBmpSurface = c;
                    pBmpSurface += 2;
                    break;
                case 3:
                    *(ushort *) pBmpSurface = c;
                    *(pBmpSurface + 2) = c >> 16;
                    pBmpSurface += 3;
                    break;
                case 4:
                    *(uint *) pBmpSurface = c;
                    pBmpSurface += 4;
                    break;
            }
        }
    }
    free(jinfo);
    //destroy_jpg(jinfo);
	return 0;
}

