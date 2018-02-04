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
** Top-level bitmap file rotate/scale function.
**
** Current maintainer: Yan XiaoWei. 
**
** Allegro is a gift-ware.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGHAVE_FIXED_MATH
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "pixel_ops.h"
#include "cursor.h"
#include "fixedmath.h"
#include "bitmap.h"

#define M_PI          3.14159265358979323846

/* _get_bmpdata_on_scanline:
 * Get bitmap data on a dc scanline.
 * param buffer: a bitmap which temporary saves bitmap data on a scanline.
 */
static void _get_bmpdata_on_scanline (BITMAP *dstbmp, const BITMAP *srcbmp, 
                   fixed dc_lx, fixed dc_rx,  
                   fixed bmp_lx, fixed bmp_ly,  
                   fixed bmp_dx, fixed bmp_dy)    
{
    int i = 0, loop;
    int alpha_pitch = srcbmp->bmAlphaPitch;
    
    dstbmp->bmWidth = (dc_rx) - (dc_lx) + 1;
    loop = dstbmp->bmWidth;
    dstbmp->bmPitch = (dstbmp->bmBitsPerPixel * dstbmp->bmWidth + 7)/ 8;

    switch (srcbmp->bmBytesPerPixel) {
	case 1:
	{
	    for (i = 0; i < loop; i++ ) {         
	        *(dstbmp->bmBits + i) = *(srcbmp->bmBits + srcbmp->bmPitch * (bmp_ly>>16) + 
		        (bmp_lx>>16));

            if (srcbmp->bmType & BMP_TYPE_ALPHA_MASK) {
                *(dstbmp->bmAlphaMask + i) = *(srcbmp->bmAlphaMask + 
                        alpha_pitch * (bmp_ly>>16) + (bmp_lx>>16));
            }
            bmp_lx += bmp_dx;                                           
            bmp_ly += bmp_dy;                                           
	    }                                                               
	}
	break;

	case 2:
	{
	    for (i = 0; i < loop; i++ ) {         
            *(unsigned short*)(dstbmp->bmBits + 2*i) = *(unsigned short*)
		        (srcbmp->bmBits + srcbmp->bmPitch * (bmp_ly>>16) + 2 * (bmp_lx>>16));
            if (srcbmp->bmType & BMP_TYPE_ALPHA_MASK) {
                *(dstbmp->bmAlphaMask + i) = *(srcbmp->bmAlphaMask + 
                        alpha_pitch * (bmp_ly>>16) + (bmp_lx>>16));
            }
            bmp_lx += bmp_dx;                                           
            bmp_ly += bmp_dy;                                           
	    }                                                               
	}
	break;
	case 3:
	{
	    for (i = 0; i < loop; i++ ) {         
	        *(unsigned short *)(dstbmp->bmBits + 3*i) = *((unsigned short *)
	            (srcbmp->bmBits + srcbmp->bmPitch * (bmp_ly>>16) + 3 * (bmp_lx>>16)));
	        *(unsigned char*)(dstbmp->bmBits + 3*i + 2) = *((unsigned char*)
	            (srcbmp->bmBits + srcbmp->bmPitch * (bmp_ly>>16) + 3 * (bmp_lx>>16) + 2));
            if (srcbmp->bmType & BMP_TYPE_ALPHA_MASK) {
                *(dstbmp->bmAlphaMask + i) = *(srcbmp->bmAlphaMask + 
                        alpha_pitch * (bmp_ly>>16) + (bmp_lx>>16));
            }
            bmp_lx += bmp_dx;                                           
            bmp_ly += bmp_dy;                                           
	    }                                                               
				     
	}
	break;
	case 4:
	{
	    for (i = 0; i < loop; i++ ) {         
            *(unsigned int*)(dstbmp->bmBits + 4*i) = *((unsigned int*)
		        (srcbmp->bmBits + srcbmp->bmPitch * (bmp_ly>>16) + 4 * (bmp_lx>>16)));
            if (srcbmp->bmType & BMP_TYPE_ALPHA_MASK) {
                *(dstbmp->bmAlphaMask + i) = *(srcbmp->bmAlphaMask + 
                        alpha_pitch * (bmp_ly>>16) + (bmp_lx>>16));
            }
            bmp_lx += bmp_dx;                                           
            bmp_ly += bmp_dy;                                           
	    }                                                               
	}
	break;
    }
}

static void  _init_bitmap_buffer(BITMAP *dst, const BITMAP *src, int size)
{
    dst->bmType = src->bmType;
    dst->bmBitsPerPixel = src->bmBitsPerPixel;
    dst->bmBytesPerPixel = src->bmBytesPerPixel;
    dst->bmAlpha = src->bmAlpha;
    dst->bmColorKey = src->bmColorKey;
#ifdef _FOR_MONOBITMAP
    dst->bmColorRep = src->bmColorRep;
#endif
    if (src->bmType & BMP_TYPE_ALPHA_MASK) {
        dst->bmAlphaMask = calloc(1, size * sizeof(char));
        dst->bmAlphaPitch = (size+3) & (~3);
    }
    else
    {
        dst->bmAlphaMask = NULL;
        dst->bmAlphaPitch = 0;
    }
    dst->bmHeight = 1;
    dst->bmBits = malloc (size * src->bmBytesPerPixel); 
    dst->bmWidth = size;
}

static void _parallelogram_map(HDC hdc, const BITMAP *bmp, fixed sx[4], 
            fixed sy[4], int sub_pixel_accuracy)
{
    /* Index in sx[] and sy[] to topmost point. */
    int top_index;

    /* Rightmost point has index (top_index+right_index) int sx[] and sy[]. */
    int right_index;

    /* Loop variables. */
    int index, i;

    /* Coordinates in bmp ordered as top-right-bottom-left. */
    fixed corner_dc_x[4], corner_dc_y[4];

    /* Coordinates in spr ordered as top-right-bottom-left. */
    fixed corner_bmp_x[4], corner_bmp_y[4];

    /* y coordinate of left point and right point. */
    int dc_ly_bottom_i, dc_ry_bottom_i;

    /* Temporary variable. */
    fixed extra_scanline_fraction;

    /*
    * Variables used in the loop
    */
    /* Coordinates of bitmap and dc points in beginning of scanline. */
    fixed bmp_lx, bmp_ly, dc_lx, dc_ldx;

    /* Increment of left bitmap point as we move a scanline down. */
    fixed bmp_ldx, bmp_ldy;

    /* Coordinates of bitmap and bmp points in end of scanline. */
    fixed dc_rx, dc_rdx;

    /* Increment of bitmap point as we move right inside a scanline. */
    fixed bmp_dx, bmp_dy;
    /* Positions of beginning of scanline after rounding to integer coordinate
      in bmp. */
    fixed bmp_lx_rounded, bmp_ly_rounded, dc_lx_rounded, dc_rx_rounded;
    /* Current scanline. */
    int scanline, tmpsl;
    /* Right edge of scanline. */
    int right_edge_test;
 
    int rect_lx, rect_rx;

    int bmp_w, bmp_h, leftx, rightx, topy, bottomy, w, h;

    FILLINFO fill_info;

    PDC pdc;

    BITMAP buffer;
   
    int dc_maxw = GetGDCapability(hdc, GDCAP_MAXX);
    int dc_maxh = GetGDCapability(hdc, GDCAP_MAXY);

    bmp_w = bmp->bmWidth;
    bmp_h = bmp->bmHeight;

    /* Get index of topmost point. */
    top_index = 0;
    if (sy[1] < sy[0])
        top_index = 1;
    if (sy[2] < sy[top_index])
        top_index = 2;
    if (sy[3] < sy[top_index])
        top_index = 3;

    /* Get direction of points: clockwise or anti-clockwise. */
    right_index = (double)(sx[(top_index+1) & 3] - sx[top_index]) *
        (double)(sy[(top_index-1) & 3] - sy[top_index]) >
        (double)(sx[(top_index-1) & 3] - sx[top_index]) *
        (double)(sy[(top_index+1) & 3] - sy[top_index]) ? 1 : -1;
    /*
    * Get coordinates of the corners.
    */

    /* corner_*[0] is top, [1] is right, [2] is bottom, [3] is left. */
    index = top_index;
    for (i = 0; i < 4; i++) {
        corner_dc_x[i] = sx[index];
        corner_dc_y[i] = sy[index];

       if (index < 2)
           corner_bmp_y[i] = 0;
       else
	   /* Need `- 1' since otherwise it would be outside bitmap. */
	       corner_bmp_y[i] = (bmp_h << 16) - 1;

       if ((index == 0) || (index == 3))
	       corner_bmp_x[i] = 0;
       else
	       corner_bmp_x[i] = (bmp_w << 16) - 1;
           index = (index + right_index) & 3;
    }

    /*
    * Get scanline starts, ends and deltas, and clipping coordinates.
    */
    #define top_dc_y    corner_dc_y[0]
    #define right_dc_y  corner_dc_y[1]
    #define bottom_dc_y corner_dc_y[2]
    #define left_dc_y   corner_dc_y[3]
    #define top_dc_x    corner_dc_x[0]
    #define right_dc_x  corner_dc_x[1]
    #define bottom_dc_x corner_dc_x[2]
    #define left_dc_x   corner_dc_x[3]

    #define top_bmp_y    corner_bmp_y[0]
    #define right_bmp_y  corner_bmp_y[1]
    #define bottom_bmp_y corner_bmp_y[2]
    #define left_bmp_y   corner_bmp_y[3]
    #define top_bmp_x    corner_bmp_x[0]
    #define right_bmp_x  corner_bmp_x[1]
    #define bottom_bmp_x corner_bmp_x[2]
    #define left_bmp_x   corner_bmp_x[3]

    /* Calculate y coordinate of first scanline. */
    if (sub_pixel_accuracy)
        scanline = top_dc_y >> 16;
    else
        scanline = (top_dc_y + 0x8000) >> 16;

    /* Vertical gap between top corner and centre of topmost scanline. */
    extra_scanline_fraction = (scanline << 16) + 0x8000 - top_dc_y;

    /* Calculate x coordinate of beginning of scanline in bmp. */
    dc_ldx = fixdiv(left_dc_x - top_dc_x,
		   left_dc_y - top_dc_y);
    dc_lx = top_dc_x + fixmul(extra_scanline_fraction, dc_ldx);

    /* Calculate x coordinate of beginning of scanline in spr. */
    /* note: all these are rounded down which is probably a Good Thing (tm) */
    bmp_ldx = fixdiv(left_bmp_x - top_bmp_x,
		   left_dc_y - top_dc_y);
    bmp_lx = top_bmp_x + fixmul(extra_scanline_fraction, bmp_ldx);

    /* Calculate y coordinate of beginning of scanline in spr. */
    bmp_ldy = fixdiv(left_bmp_y - top_bmp_y,
		   left_dc_y - top_dc_y);
    bmp_ly = top_bmp_y + fixmul(extra_scanline_fraction, bmp_ldy);

    /* Calculate left loop bound. */
    dc_ly_bottom_i = (left_dc_y + 0x8000) >> 16;

    /* Calculate x coordinate of end of scanline in bmp. */
    dc_rdx = fixdiv(right_dc_x - top_dc_x,
		   right_dc_y - top_dc_y);
    dc_rx = top_dc_x + fixmul(extra_scanline_fraction, dc_rdx);

    /* Calculate right loop bound. */
    dc_ry_bottom_i = (right_dc_y + 0x8000) >> 16;

    /* Get dx and dy, the offsets to add to the source coordinates as we move
      one pixel rightwards along a scanline. This formula can be derived by
      considering the 2x2 matrix that transforms the bitmap to the
      parallelogram.
      We'd better use double to get this as exact as possible, since any
      errors will be accumulated along the scanline.
    */
    bmp_dx = (fixed)((sy[3] - sy[0]) * 65536.0 * (65536.0 * bmp_w) /
		    ((sx[1] - sx[0]) * (double)(sy[3] - sy[0]) -
		     (sx[3] - sx[0]) * (double)(sy[1] - sy[0])));
    bmp_dy = (fixed)((sy[1] - sy[0]) * 65536.0 * (65536.0 * bmp_h) /
		    ((sx[3] - sx[0]) * (double)(sy[1] - sy[0]) -
		     (sx[1] - sx[0]) * (double)(sy[3] - sy[0])));

    /*fill bitmap origin data*/
    leftx = left_dc_x >> 16;
    rightx = (right_dc_x >> 16) + 1;
    topy = top_dc_y >> 16;
    bottomy = (bottom_dc_y >> 16) + 1;

    //w = (rightx - leftx + 1);
    w = rightx - leftx;
    h = bottomy - topy;

    _init_bitmap_buffer(&buffer, bmp, w);
 
    pdc = _begin_fill_bitmap (hdc, leftx, topy, w, h, bmp, &fill_info);
    if (pdc == NULL) {
        free (buffer.bmBits);
        if (buffer.bmAlphaMask) free (buffer.bmAlphaMask);
        return;
    }

    fill_info.dst_rect.h = 1;
#if 0
    int y = 0;
#endif
    while (1) {
        if (scanline > MIN(dc_maxh, fixtoi(bottom_dc_y)))
            break;
        /* Has beginning of scanline passed a corner? */
        if (scanline >= dc_ly_bottom_i) {

	    /* Vertical gap between left corner and centre of scanline. */
	    extra_scanline_fraction = (scanline << 16) + 0x8000 - left_dc_y;
        
	    /* Update x coordinate of beginning of scanline in bmp. */
	    dc_ldx = fixdiv(bottom_dc_x - left_dc_x,
			 bottom_dc_y - left_dc_y);
  	    dc_lx = left_dc_x + fixmul(extra_scanline_fraction, dc_ldx);

	    /* Update x coordinate of beginning of scanline in spr. */
	    bmp_ldx = fixdiv(bottom_bmp_x - left_bmp_x,
	 		 bottom_dc_y - left_dc_y);
  	    bmp_lx = left_bmp_x + fixmul(extra_scanline_fraction, bmp_ldx);

	    /* Update y coordinate of beginning of scanline in spr. */
	    bmp_ldy = fixdiv(bottom_bmp_y - left_bmp_y,
	 		 bottom_dc_y - left_dc_y);
	    bmp_ly = left_bmp_y + fixmul(extra_scanline_fraction, bmp_ldy);

	    /* Update loop bound. */
	    if (sub_pixel_accuracy)
	        dc_ly_bottom_i = (bottom_dc_y + 0xffff) >> 16;
	    else
	        dc_ly_bottom_i = (bottom_dc_y + 0x8000) >> 16;
        }

        /* Has end of scanline passed a corner? */
        if (scanline >= dc_ry_bottom_i) {

	    /* Vertical gap between right corner and centre of scanline. */
	    extra_scanline_fraction = (scanline << 16) + 0x8000 - right_dc_y;

	    /* Update x coordinate of end of scanline in bmp. */
	    dc_rdx = fixdiv(bottom_dc_x - right_dc_x,
			 bottom_dc_y - right_dc_y);
	    dc_rx = right_dc_x + fixmul(extra_scanline_fraction, dc_rdx);

	    /* Update loop bound: We aren't supposed to use this any more, so
	    just set it to some big enough value. */
        }

        /* Make left bmp coordinate be an integer.*/
        if (sub_pixel_accuracy)
	        dc_lx_rounded = dc_lx;
        else
	       dc_lx_rounded = (dc_lx + 0x8000) & ~0xffff;

        /* ... and move starting point in bitmap accordingly. */
        if (sub_pixel_accuracy) {
	        bmp_lx_rounded = bmp_lx +
			   fixmul((dc_lx_rounded - dc_lx), bmp_dx);
	        bmp_ly_rounded = bmp_ly +
			   fixmul((dc_lx_rounded - dc_lx), bmp_dy);
        }
        else {
	        bmp_lx_rounded = bmp_lx +
			   fixmul(dc_lx_rounded + 0x7fff - dc_lx, bmp_dx);
	        bmp_ly_rounded = bmp_ly +
			   fixmul(dc_lx_rounded + 0x7fff - dc_lx, bmp_dy);
        }

        /* Make right bmp coordinate be an integer and clip it. */
        if (sub_pixel_accuracy)
	        dc_rx_rounded = dc_rx;
        else
	        dc_rx_rounded = (dc_rx - 0x8000) & ~0xffff;

        /* Draw! */
        if (dc_lx_rounded <= dc_rx_rounded) {
	        if (!sub_pixel_accuracy) {
	            if ((unsigned)(bmp_lx_rounded >> 16) >= (unsigned)bmp_w) {
	                if (((bmp_lx_rounded < 0) && (bmp_dx <= 0)) ||
	    	              ((bmp_lx_rounded > 0) && (bmp_dx >= 0))) {
	    	            goto skip_draw;
	                }
	                else {
		                do {
		                    bmp_lx_rounded += bmp_dx; dc_lx_rounded += 65536;
		                    if (dc_lx_rounded > dc_rx_rounded) {
		                	    goto skip_draw;
                            }
		                } while ((unsigned)(bmp_lx_rounded >> 16) >=
			               (unsigned)bmp_w);

	                }
	            }
                right_edge_test = bmp_lx_rounded +
			      ((dc_rx_rounded - dc_lx_rounded) >> 16) * bmp_dx;
	            if ((unsigned)(right_edge_test >> 16) >= (unsigned)bmp_w) {
	                if (((right_edge_test < 0) && (bmp_dx <= 0)) ||
		                ((right_edge_test > 0) && (bmp_dx >= 0))) {
		                do {
		                    dc_rx_rounded -= 65536;
		                    right_edge_test -= bmp_dx;
		                    if (dc_lx_rounded > dc_rx_rounded) {
			                    goto skip_draw;
                            }
		                } while ((unsigned)(right_edge_test >> 16) 
                                >= (unsigned)bmp_w);
	                }
	                else {
                        goto skip_draw;
	                }
	            }
	            if ((unsigned)(bmp_ly_rounded >> 16) >= (unsigned)bmp_h) {
	                if (((bmp_ly_rounded < 0) && (bmp_dy <= 0)) ||
		                    ((bmp_ly_rounded > 0) && (bmp_dy >= 0))) {
                        goto skip_draw;
                    }
                    else {
                        do {
                            bmp_ly_rounded += bmp_dy;
                            dc_lx_rounded += 65536;
                            if (dc_lx_rounded > dc_rx_rounded) {
                                goto skip_draw;
                            }
                            } while (((unsigned)bmp_ly_rounded >> 16) 
                                    >= (unsigned)bmp_h);
                    }
                }

                right_edge_test = bmp_ly_rounded +
                          ((dc_rx_rounded - dc_lx_rounded) >> 16) * bmp_dy;

                if ((unsigned)(right_edge_test >> 16) >= (unsigned)bmp_h) {
                    if (((right_edge_test < 0) && (bmp_dy <= 0)) ||
                           ((right_edge_test > 0) && (bmp_dy >= 0))) {
                        do { 
                            dc_rx_rounded -= 65536;
                            right_edge_test -= bmp_dy;
                            if (dc_lx_rounded > dc_rx_rounded) {
                                goto skip_draw;
                            }
                        } while ((unsigned)(right_edge_test >> 16) >=
                               (unsigned)bmp_h);
                    }
                    else {
                        goto skip_draw;
                    }
                }
             }

            dc_lx_rounded >>= 16;
            dc_rx_rounded >>= 16;

            tmpsl = scanline;
            rect_lx = (int)dc_lx_rounded;
            rect_rx = (int)dc_rx_rounded;

            coor_LP2SP(pdc, &rect_lx, &tmpsl);
            coor_LP2SP(pdc, &rect_rx, &tmpsl);

            if (rect_lx > dc_maxw + 1) {
                rect_rx = dc_maxw;
            }
            if (rect_rx > dc_maxw + 1) {
                rect_rx = dc_maxw;
            }

            /*get current bmp data on a scanline*/
             _get_bmpdata_on_scanline (&buffer, bmp,
                       dc_lx_rounded, dc_rx_rounded, 
                       bmp_lx_rounded, bmp_ly_rounded,
                       bmp_dx, bmp_dy);

             fill_info.dst_rect.x = (unsigned int) rect_lx;
             fill_info.dst_rect.w = buffer.bmWidth;

             _fill_bitmap_scanline(pdc, &buffer, &fill_info, scanline - topy);
        }

skip_draw:
        /* Jump to next scanline. */
        scanline++;

        /* Update beginning of scanline. */
        dc_lx += dc_ldx;
        bmp_lx += bmp_ldx;
        bmp_ly += bmp_ldy;
        /* Update end of scanline. */

        dc_rx += dc_rdx;
    }

    _end_fill_bitmap (pdc, &buffer, &fill_info); 

    if (buffer.bmAlphaMask) free (buffer.bmAlphaMask);
    free (buffer.bmBits);
}

/* _rotate_scale_flip_coordinates:
 *  Calculates the coordinates for the rotated, scaled and flipped bitmap,
 *  and passes them on to the given function.
 */
static BOOL _rotate_scale_flip_coordinates(fixed w, fixed h,
				    fixed x, fixed y, fixed cx, fixed cy,
				    fixed angle,
				    fixed scale_x, fixed scale_y,
				    int h_flip, int v_flip,
				    fixed sx[4], fixed sy[4])
{
    fixed fix_cos, fix_sin;
    int tl = 0, tr = 1, bl = 3, br = 2;
    int tmp;
    fixed xofs, yofs;

    /*angle = angle & 0xffffff;
    if (angle >= 0x800000)
        angle -= 0x1000000;*/

    fix_sin = fixsin(angle);
    fix_cos = fixcos(angle);
            
    /* Decide what order to take corners in. */
    if (v_flip) {
        tl = 3;
        tr = 2;
        bl = 0;
        br = 1;
    }
    else {
        tl = 0;
        tr = 1;
        bl = 3;
        br = 2;
    }
    if (h_flip) {
        tmp = tl;
        tl = tr;
        tr = tmp;
        tmp = bl;
        bl = br;
        br = tmp;
    }

    /* Calculate new coordinates of all corners. */
    w = fixmul(w, scale_x);
    h = fixmul(h, scale_y);
    cx = fixmul(cx, scale_x);
    cy = fixmul(cy, scale_y);
    
    /*error happen*/
    if (w == 0x7FFFFFFF || w == -0x7FFFFFFF
     || h == 0x7FFFFFFF || h == -0x7FFFFFFF
     || cx == 0x7FFFFFFF || cx == -0x7FFFFFFF
     || cy == 0x7FFFFFFF || cy == -0x7FFFFFFF)
    {
        return FALSE;
    }

    xofs = fixadd (fixsub (x, fixmul (cx, fix_cos)), fixmul (cy, fix_sin));

    yofs = fixsub (fixsub (y, fixmul (cx, fix_sin)), fixmul (cy, fix_cos));
    
    sx[tl] = xofs;
    sy[tl] = yofs;
    sx[tr] = fixadd (xofs, fixmul(w, fix_cos));
    sy[tr] = fixadd (yofs, fixmul(w, fix_sin));
    sx[bl] = fixsub (xofs, fixmul(h, fix_sin));
    sy[bl] = fixadd (yofs, fixmul(h, fix_cos));
    sx[br] = fixsub (fixadd (sx[tr], sx[bl]), sx[tl]);
    sy[br] = fixsub (fixadd (sy[tr], sy[bl]), sy[tl]);

    return TRUE;
}

/* PivotScaledBitmapFlip:
 *  The most generic routine to which you specify the position with angles,
 *  scales, etc.
 */
void GUIAPI PivotScaledBitmapFlip(HDC hdc, const BITMAP *bmp,
			       fixed x, fixed y, fixed cx, fixed cy,
			       int angle, fixed scale_x, fixed scale_y, 
                   BOOL h_flip, BOOL v_flip)
{
    fixed sx[4], sy[4], fixangle;
    BOOL ret;

    fixangle = ftofix((((angle/64) % 360) +360)%360 * 256/360);

    ret = _rotate_scale_flip_coordinates(bmp->bmWidth << 16, bmp->bmHeight << 16,
				    x, y, cx, cy, fixangle, scale_x, scale_y,
				    h_flip, v_flip, sx, sy);

    if (ret == TRUE)
    {
        _parallelogram_map(hdc, bmp, sx, sy, FALSE);
    }
}

/*  RotateBitmap: 
 *  Draws a bitmap onto destination context at the specified position, 
 *  rotating it by the specified angle. The angle is a fixed point 16.16 
 *  number which is with 256 equal to a full circle, 64 a right angle, etc. 
 */
void GUIAPI RotateBitmap (HDC hdc, const BITMAP *bmp, int lx, int ty, int angle)
{
   if (!bmp)
        return;

    PivotScaledBitmapFlip (hdc, bmp, (lx<<16) + (bmp->bmWidth * 0x10000) / 2,
			     			      (ty<<16) + (bmp->bmHeight * 0x10000) / 2,
			     			      bmp->bmWidth << 15, bmp->bmHeight << 15,
			     			      angle, 0x10000, 0x10000, FALSE, FALSE);
    
}

void GUIAPI RotateScaledBitmap (HDC hdc, const BITMAP *bmp, int lx,
                  int ty, int angle, int w, int h)
{
    fixed scale_x, scale_y;
    int max;

    if (!bmp || w < 0 || h < 0 || w > 32767 || h > 32767)
    {
        return;
    }

    max = sqrt(w*w + h*h);
    if (max > 32767)
    {
        return;
    }


    scale_x = ftofix ((float)w/bmp->bmWidth);
    scale_y = ftofix ((float)h/bmp->bmHeight);

    PivotScaledBitmapFlip (hdc, bmp, (lx<<16) + (bmp->bmWidth * 0x10000) / 2,
			     			      (ty<<16) + (bmp->bmHeight * 0x10000) / 2,
			     			      bmp->bmWidth << 15, bmp->bmHeight << 15,
			     			      angle, scale_x, scale_y, FALSE, FALSE);
    
}

void GUIAPI RotateScaledBitmapVFlip (HDC hdc, const BITMAP *bmp, int lx, 
                  int ty, int angle, int w, int h)
{
    fixed scale_x, scale_y;
    int max;

    if (!bmp || w < 0 || h < 0 || w > 32767 || h > 32767)
    {
        return;
    }

    max = sqrt(w*w + h*h);
    if (max > 32767)
    {
        return;
    }

    scale_x = ftofix ((float)w/bmp->bmWidth);
    scale_y = ftofix ((float)h/bmp->bmHeight);

    PivotScaledBitmapFlip (hdc, bmp, (lx<<16) + (bmp->bmWidth * 0x10000) / 2,
			     			      (ty<<16) + (bmp->bmHeight * 0x10000) / 2,
			     			      bmp->bmWidth << 15, bmp->bmHeight << 15,
			     			      angle, scale_x, scale_y, FALSE, TRUE);
    
}

void GUIAPI RotateScaledBitmapHFlip (HDC hdc, const BITMAP *bmp, int lx, int ty,
                        int angle, int w, int h)
{
    fixed scale_x, scale_y;
    int max;

    if (!bmp || w < 0 || h < 0 || w > 32767 || h > 32767)
    {
        return;
    }

    max = sqrt(w*w + h*h);
    if (max > 32767)
    {
        return;
    }

    scale_x = ftofix ((float)w/bmp->bmWidth);
    scale_y = ftofix ((float)h/bmp->bmHeight);

    PivotScaledBitmapFlip (hdc, bmp, (lx<<16) + (bmp->bmWidth * 0x10000) / 2,
			     			      (ty<<16) + (bmp->bmHeight * 0x10000) / 2,
			     			      bmp->bmWidth << 15, bmp->bmHeight << 15,
			     			      angle, scale_x, scale_y, TRUE, FALSE);
    
}

/*  PivotBitmap:
 *  Rotates a bitmap around the specified pivot centre point.
 */
void GUIAPI PivotBitmap(HDC hdc, const BITMAP *bmp, int x, int y, int cx, 
                int cy, int angle)
{
    if (!bmp)
        return;

    PivotScaledBitmapFlip (hdc, bmp, x<<16, y<<16, cx<<16, cy<<16, angle, 
                               0x10000,0x10000, FALSE, FALSE);
}

void GUIAPI RotateBitmapVFlip (HDC hdc, const BITMAP *bmp, int lx, 
                    int ty, int angle)
{
    if (!bmp)
        return;

    PivotScaledBitmapFlip (hdc, bmp, (lx<<16) + (bmp->bmWidth * 0x10000) / 2,
			     			      (ty<<16) + (bmp->bmHeight * 0x10000) / 2,
			     			      bmp->bmWidth << 15, bmp->bmHeight << 15,
			     			      angle, 0x10000,0x10000, FALSE, TRUE);
    
}

void GUIAPI RotateBitmapHFlip (HDC hdc, const BITMAP *bmp, int lx, 
                   int ty, int angle)
{
    if (!bmp)
        return;

    PivotScaledBitmapFlip (hdc, bmp, (lx<<16) + (bmp->bmWidth * 0x10000) / 2,
			     			      (ty<<16) + (bmp->bmHeight * 0x10000) / 2,
			     			      bmp->bmWidth << 15, bmp->bmHeight << 15,
			     			      angle, 0x10000, 0x10000, TRUE, FALSE);
    
}
#endif /*_MGHAVE_FIXED_MATH*/
