
/*
**  $Id$
**  
**  Copyright (C) 2003 ~ 2009 Feynman Software.
**  Copyright (C) 2001 ~ 2002 Wei Yongming.
*/

#ifndef _nexusvideo_pri_h
#define _nexusvideo_pri_h

typedef struct _NexusPrivate_HWSurface_hwdata
{
    void   *display;
    void   *surfaceHandle;
    void   *gfx;
    int     isCopied;
} NexusPrivate_HWSurface_hwdata;

GAL_Rect **NexusPrivate_get_modes_rects(GAL_Rect ** modes, int size);
int NexusPrivate_get_mode(int bpp, int width, int height);

int     NexusPrivate_init(void **pThis, int isServer, int width, int height);
void    NexusPrivate_close(void *_this, int isServer);
int     NexusPrivate_CreateSurface(void *_this, int w, int h,
                                   unsigned int *pPitch, void **pPixels,
                                   NexusPrivate_HWSurface_hwdata ** pHwdata);
NexusPrivate_HWSurface_hwdata *
NexusPrivate_SurfaceDup(NexusPrivate_HWSurface_hwdata * src);
void   *NexusPrivate_SurfaceGetMemory(void *_surfaceHandle);
void    NexusPrivate_FreeSurface(void *_this,
                                 NexusPrivate_HWSurface_hwdata * hwdata);
int     NexusPrivate_FillRect(void *_this,
                              NexusPrivate_HWSurface_hwdata * hwdata, int x,
                              int y, int w, int h, unsigned int color);
int     NexusPrivate_BitBlitEx(void *_this, NexusPrivate_HWSurface_hwdata * src,
                     int src_x, int src_y, int src_w, int src_h,
                     NexusPrivate_HWSurface_hwdata * dst, int dst_x, int dst_y,
                     int dst_w, int dst_h,
                     int flag, unsigned int colorkey_src, unsigned int colorkey_dst, 
                     unsigned int alpha_src);
int     NexusPrivate_SelectSurface(void *_this,
                                   NexusPrivate_HWSurface_hwdata * hwdata,
                                   int isServer);
void    NexusPrivate_Flush(void *_this, NexusPrivate_HWSurface_hwdata *hwdata);

void   *NexusPrivate_GetDisplay(void);
#endif /* _nexusvideo_pri_h */
