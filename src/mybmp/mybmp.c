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
** Top-level bitmap file read/save function.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/12/17, derived from original ../newgdi/readbmp.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "readbmp.h"
#include "bitmap.h"


#define MAX_BITMAP_TYPES   9

static BITMAP_TYPE_INFO bitmap_types [MAX_BITMAP_TYPES] =
{
   { "bmp", __mg_init_bmp, __mg_load_bmp, __mg_cleanup_bmp,
#ifdef _MGMISC_SAVEBITMAP
    __mg_save_bmp, __mg_check_bmp },
#else
    NULL, __mg_check_bmp },
#endif

#ifdef _MGIMAGE_LBM
   { "lbm", __mg_init_lbm, __mg_load_lbm, __mg_cleanup_lbm, NULL, __mg_check_lbm },
#endif
#ifdef _MGIMAGE_PCX
   { "pcx", __mg_init_pcx, __mg_load_pcx, __mg_cleanup_pcx, NULL, NULL },
#endif
#ifdef _MGIMAGE_TGA
   { "tga", __mg_init_tga, __mg_load_tga, __mg_cleanup_tga, NULL, NULL },
#endif
#ifdef _MGIMAGE_GIF
   { "gif", __mg_init_gif, __mg_load_gif, __mg_cleanup_gif, NULL, __mg_check_gif },
#endif
#ifdef _MGIMAGE_JPG
   { "jpg", __mg_init_jpg, __mg_load_jpg, __mg_cleanup_jpg, NULL, __mg_check_jpg },
   { "jpeg", __mg_init_jpg, __mg_load_jpg, __mg_cleanup_jpg, NULL, __mg_check_jpg },
#endif
#ifdef _MGIMAGE_PNG
   { "png", __mg_init_png, __mg_load_png, __mg_cleanup_png, NULL, __mg_check_png },
#endif
};

/*
 * gets image type from ext name
 */
static int get_image_type (const char *ext)
{
    int type;

    for (type = 0; type < MAX_BITMAP_TYPES; type++) {
        if ((bitmap_types[type].ext[0]) && (strcasecmp (bitmap_types[type].ext, ext) == 0))
            break;
    }

    if (type == MAX_BITMAP_TYPES) {
        fprintf(stderr, "get_image_type: unknown type %s.\n", ext);
        return ERR_BMP_UNKNOWN_TYPE;
    }

    return type;
}

/*
 *  Registers a new image file type, telling it how to load and
 *  save files of this format (either function may be NULL).
 */
BOOL GUIAPI RegisterBitmapFileType (const char *ext,
            void* (*init) (MG_RWops* fp, MYBITMAP *my_bmp, RGB *pal),
            int (*load) (MG_RWops* fp, void* init_info, MYBITMAP *my_bmp, CB_ONE_SCANLINE cb, void* context),
            void (*cleanup) (void* init_info),
            int (*save) (MG_RWops* fp, MYBITMAP *my_bmp, RGB *pal),
            BOOL (*check) (MG_RWops* fp))
{
    int i;

    for (i = 0; i < MAX_BITMAP_TYPES; i++) {
        if ((!bitmap_types[i].ext [0]) || (strcasecmp (bitmap_types[i].ext, ext) == 0)) {
	        strncpy (bitmap_types[i].ext, ext, sizeof (bitmap_types[i].ext) - 1);
	        bitmap_types[i].ext [sizeof (bitmap_types[i].ext) - 1] = 0;
	        bitmap_types[i].init = init;
	        bitmap_types[i].load = load;
	        bitmap_types[i].cleanup = cleanup;
	        bitmap_types[i].save = save;
	        bitmap_types[i].check = check;
	        return TRUE;
      }
   }

    return FALSE;
}

#define PIX2BYTES(n)    (((n)+7)/8)
/*
 * compute image line size and bytes per pixel
 * from bits per pixel and width
 */
int bmp_ComputePitch(int bpp, Uint32 width, Uint32 *pitch, BOOL does_round)
{
    int linesize;
    int bytespp = 1;

    if(bpp == 1)
        linesize = PIX2BYTES (width);
    else if(bpp <= 4)
        linesize = PIX2BYTES (width << 2);
    else if (bpp <= 8)
        linesize = width;
    else if(bpp <= 16) {
        linesize = width * 2;
        bytespp = 2;
    } else if(bpp <= 24) {
        linesize = width * 3;
        bytespp = 3;
    } else {
        linesize = width * 4;
        bytespp = 4;
    }

    /* rows are DWORD32 right aligned */
    if (does_round)
        *pitch = (linesize + 3) & -4;
    else
        *pitch = linesize;
    return bytespp;
}

void* GUIAPI InitMyBitmapSL (MG_RWops* area, const char* ext, MYBITMAP* my_bmp, RGB* pal)
{
    int type;
    LOAD_MYBITMAP_INFO* load_info;

    load_info = calloc (1, sizeof (LOAD_MYBITMAP_INFO));
    if (load_info == NULL)
        return NULL;

    if ((type = get_image_type(ext)) < 0)
    {
        free (load_info);
        return NULL;
    }

    load_info->type_info = bitmap_types + type;

    my_bmp->flags = MYBMP_LOAD_ALLOCATE_ONE;
    my_bmp->bits = NULL;
    my_bmp->frames = 1;
    my_bmp->depth = GetGDCapability (HDC_SCREEN, GDCAP_BPP) << 3;

    if (my_bmp->depth <= 8)
        GetPalette (HDC_SCREEN, 0, 256, (GAL_Color*)pal);

    /* This is just for gray screen. If your screen is gray, 
     * please define this macro _GRAY_SCREEN.
     */ 
#ifdef _GRAY_SCREEN
    my_bmp->flags |= MYBMP_LOAD_GRAYSCALE;
#endif

    load_info->init_info = load_info->type_info->init (area, my_bmp, pal);
    if (load_info->init_info == NULL)
        goto fail;

    my_bmp->bits = malloc (my_bmp->pitch);
    if (my_bmp->bits == NULL)
        goto fail;

    return load_info;

fail:
    if (my_bmp->bits != NULL)
        free (my_bmp->bits);
    free (load_info);
    return NULL;
}

int GUIAPI LoadMyBitmapSL (MG_RWops* area, void* load_info, MYBITMAP* my_bmp, 
                CB_ONE_SCANLINE cb, void* context)
{
    LOAD_MYBITMAP_INFO* info = (LOAD_MYBITMAP_INFO*) load_info;
    if (info == NULL || info->type_info == NULL)
        return ERR_BMP_IMAGE_TYPE;

    return info->type_info->load (area, info->init_info, my_bmp, cb, context);
}

int GUIAPI CleanupMyBitmapSL (MYBITMAP* my_bmp, void* load_info)
{
    LOAD_MYBITMAP_INFO* info = (LOAD_MYBITMAP_INFO*) load_info;

    if (info == NULL || info->type_info == NULL)
        return -1;

    free (my_bmp->bits);
    my_bmp->bits = NULL;

    info->type_info->cleanup (info->init_info);
    free (info);

    return 0;
}

int GUIAPI LoadMyBitmapEx (PMYBITMAP my_bmp, RGB* pal, MG_RWops* area, const char* ext)
{
    int type, ret;
    void* init_info;

    if ((type = get_image_type(ext)) < 0)
        return ERR_BMP_UNKNOWN_TYPE;

    my_bmp->flags = MYBMP_LOAD_NONE;
    my_bmp->bits = NULL;
    my_bmp->frames = 1;
    my_bmp->depth = GetGDCapability (HDC_SCREEN, GDCAP_BPP) << 3;

    if (my_bmp->depth <= 8)
        GetPalette (HDC_SCREEN, 0, 256, (GAL_Color*)pal);

#ifdef _GRAY_SCREEN
    my_bmp->flags |= MYBMP_LOAD_GRAYSCALE;
#endif

    init_info = bitmap_types [type].init (area, my_bmp, pal);
    if (init_info == NULL)
        return ERR_BMP_CANT_READ;

    my_bmp->bits = malloc (my_bmp->pitch * my_bmp->h);

    if (my_bmp->bits)
        ret = bitmap_types [type].load (area, init_info, my_bmp, NULL, NULL);
    else
        ret = ERR_BMP_MEM;

    bitmap_types [type].cleanup (init_info);

    return ret;
}

int GUIAPI LoadMyBitmapFromFile (PMYBITMAP my_bmp, RGB* pal, const char* file_name)
{
    MG_RWops* area;
    const char* ext;
    int ret_val;

    if ((ext = get_extension (file_name)) == NULL)
        return ERR_BMP_UNKNOWN_TYPE;

    if (!(area = MGUI_RWFromFile (file_name, "rb"))) {
        return ERR_BMP_FILEIO;
    }

    ret_val = LoadMyBitmapEx (my_bmp, pal, area, ext);

    MGUI_RWclose (area);

    return ret_val;
}

int GUIAPI LoadMyBitmapFromMem (PMYBITMAP my_bmp, RGB* pal, const void* mem, 
                                int size, const char* ext)
{
    MG_RWops* area;
    int ret_val;

    if (!(area = MGUI_RWFromMem ((void*)mem, size))) {
        return ERR_BMP_MEM;
    }

    ret_val = LoadMyBitmapEx (my_bmp, pal, area, ext);

    MGUI_RWclose (area);

    return ret_val;
}

void GUIAPI UnloadMyBitmap (PMYBITMAP my_bmp)
{
    free (my_bmp->bits);
}

const char* GUIAPI CheckBitmapType (MG_RWops* fp)
{
    int type;
    int start_pos = MGUI_RWseek (fp, 0, SEEK_CUR);

    for (type = 0; type < MAX_BITMAP_TYPES; type++) {
        if (bitmap_types[type].check) {
            if (bitmap_types[type].check (fp)) {
                MGUI_RWseek (fp, start_pos, SEEK_SET);
                return bitmap_types[type].ext;
            }
            else {
                MGUI_RWseek (fp, start_pos, SEEK_SET);
            }
        }
    }

    return NULL;
}

#ifdef _MGMISC_SAVEBITMAP
int GUIAPI SaveMyBitmapToFile (PMYBITMAP my_bmp, RGB* pal, const char* spFileName)
{
    MG_RWops* fp;
    const char* ext;
    int type;
    int save_ret;

    if ((ext = get_extension (spFileName)) == NULL)
        return ERR_BMP_UNKNOWN_TYPE;

    if ( (type = get_image_type(ext)) < 0 )
        return ERR_BMP_UNKNOWN_TYPE;

    if (bitmap_types[type].save == NULL)
        return ERR_BMP_CANT_SAVE;

    if (!(fp = MGUI_RWFromFile (spFileName, "wb+"))) {
        _MG_PRINTF ("MYBMP>WinBMP: Saving BMP file: fopen error.\n");
        return ERR_BMP_FILEIO;
    }

    save_ret = bitmap_types[type].save (fp, my_bmp, pal);

    MGUI_RWclose (fp);

    return save_ret;
}
#endif

