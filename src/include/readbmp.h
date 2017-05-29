/*
** $Id: readbmp.h 11102 2008-10-23 01:58:25Z tangjianbin $
**
** readbmp.h: Low Level bitmap file read/save routines.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** Create date: 2001/xx/xx
*/

#ifndef GUI_GDI_READBMP_H
    #define GUI_GDI_READBMP_H

#include <string.h>
#include "endianrw.h"

typedef struct _BITMAP_TYPE_INFO BITMAP_TYPE_INFO;

/*
 * structure for Image operations
 * The idea comes from readbmp.c in Allegro by Shawn Hargreaves.
 */
struct _BITMAP_TYPE_INFO
{  
    char ext[8];
    void* (*init)       (MG_RWops* fp, MYBITMAP *my_bmp, RGB *pal);
    int (*load)         (MG_RWops* fp, void* init_info, MYBITMAP *my_bmp,
                         CB_ONE_SCANLINE cb, void* context);
    void (*cleanup)     (void* init_info);
    int (*save)         (MG_RWops* fp, MYBITMAP *my_bmp, RGB *pal);
    BOOL (*check)       (MG_RWops* fp);
};

typedef struct _LOAD_MYBITMAP_INFO LOAD_MYBITMAP_INFO;

struct _LOAD_MYBITMAP_INFO
{
    BITMAP_TYPE_INFO* type_info;
    void* init_info;
};

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

void* __mg_init_bmp (MG_RWops* fp, MYBITMAP *bmp, RGB *pal);
int __mg_load_bmp (MG_RWops* fp, void* init_info, MYBITMAP *bmp, CB_ONE_SCANLINE cb, void* context);
void __mg_cleanup_bmp (void* init_info);
int __mg_save_bmp (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
BOOL __mg_check_bmp (MG_RWops* fp);

#ifdef _MGIMAGE_LBM
void* __mg_init_lbm (MG_RWops* fp, MYBITMAP *lbm, RGB *pal);
int __mg_load_lbm (MG_RWops* f, void * init_info, MYBITMAP* bmp, CB_ONE_SCANLINE cb, void * context);
void __mg_cleanup_lbm (void* init_info);
int __mg_save_lbm (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
BOOL __mg_check_lbm (MG_RWops* fp);
#endif

#ifdef _MGIMAGE_PCX
void* __mg_init_pcx (MG_RWops* fp, MYBITMAP *pcx, RGB *pal);
int __mg_load_pcx (MG_RWops* fp, void* init_info, MYBITMAP *pcx, CB_ONE_SCANLINE cb, void* context);
void __mg_cleanup_pcx (void* init_info);
int __mg_save_pcx (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
BOOL __mg_check_pcx (MG_RWops* fp);
#endif

#ifdef _MGIMAGE_TGA
void* __mg_init_tga (MG_RWops* fp, MYBITMAP *tga, RGB *pal);
int __mg_load_tga (MG_RWops* fp, void* init_info, MYBITMAP *tga, CB_ONE_SCANLINE cb, void* context);
void __mg_cleanup_tga (void* init_info);
int __mg_save_tga (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
BOOL __mg_check_tga (MG_RWops* fp);
#endif

#ifdef _MGIMAGE_GIF
void* __mg_init_gif (MG_RWops* fp, MYBITMAP *gif, RGB *pal);
int __mg_load_gif (MG_RWops* fp, void* init_info, MYBITMAP *gif, CB_ONE_SCANLINE cb, void* context);
void __mg_cleanup_gif (void* init_info);
int __mg_save_gif (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
BOOL __mg_check_gif (MG_RWops* fp);
#endif

#ifdef _MGIMAGE_JPG
void* __mg_init_jpg (MG_RWops* fp, MYBITMAP *jpg, RGB *pal);
int __mg_load_jpg (MG_RWops* fp, void* init_info, MYBITMAP *jpg, CB_ONE_SCANLINE cb, void* context);
void __mg_cleanup_jpg (void* init_info);
BOOL __mg_check_jpg (MG_RWops* fp);
#endif

#ifdef _MGIMAGE_PNG
void* __mg_init_png (MG_RWops* fp, MYBITMAP *png, RGB *pal);
int __mg_load_png (MG_RWops* fp, void* init_info, MYBITMAP *png, CB_ONE_SCANLINE cb, void* context);
void __mg_cleanup_png (void* init_info);
int __mg_save_png (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
BOOL __mg_check_png (MG_RWops* fp);
#endif

int bmp_ComputePitch (int bpp, Uint32 width, Uint32 *pitch, BOOL does_round);

static inline const char* 
get_extension (const char* filename)
{
    const char* ext;

    ext = strrchr (filename, '.');

    if (ext)
        return ext + 1;

    return NULL;
}


#define fp_getc(fp)     MGUI_RWgetc(fp)
#define fp_igetw(fp)    MGUI_ReadLE16(fp)
#define fp_igetl(fp)    MGUI_ReadLE32(fp)
#define fp_mgetw(fp)    MGUI_ReadBE16(fp)
#define fp_mgetl(fp)    MGUI_ReadBE32(fp)

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_GDI_READBMP_H

