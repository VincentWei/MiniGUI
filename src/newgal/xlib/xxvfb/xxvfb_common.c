#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

#include "define.h"
#include "xxvfbhdr.h"
#include "xxvfb_common.h"

extern XXVFBInfo xInfo;

void DrawImage(int x, int y, int w, int h, char* data)
{
    XImage *pImage = XCreateImage(xInfo.display,xInfo.visual,xInfo.dev_depth,ZPixmap,
            0,data,w,h,8,0);
#if 0
    fprintf(stderr,"width=%d,height=%d\n",pImage->width,pImage->height);
    fprintf(stderr,"xoffset=%d\n",pImage->xoffset);
    fprintf(stderr,"format=%d\n",pImage->format);
    fprintf(stderr,"bitmap_bit_order=%d\n",pImage->bitmap_bit_order);
    fprintf(stderr,"bitmap_unit=%d\n",pImage->bitmap_unit);
    fprintf(stderr,"bitmap_pad=%d\n",pImage->bitmap_pad);
    fprintf(stderr,"bytes_per_line=%d\n",pImage->bytes_per_line);
    fprintf(stderr,"r=0x%x,g=0x%x,b=0x%x\n",pImage->red_mask, pImage->green_mask, pImage->blue_mask);
    fprintf(stderr,"x=%d,y=%d,w=%d,h=%d\n",x,y,w,h);
#endif

    if(pImage == NULL)
    {
        fprintf(stderr,"Create Image Error\n");
        return;
    }

    XPutImage(xInfo.display, xInfo.win, xInfo.gc, pImage, 0, 0, x, y, w, h);

    XFlush(xInfo.display);
    XDestroyImage(pImage);
}
