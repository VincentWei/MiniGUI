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
** animation.c: the Animation control
**
** Create date: 2004/07/14
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGCTRL_ANIMATION
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"

#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "ctrl/animation.h"

static LRESULT AnimationCtrlProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL RegisterAnimationControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_ANIMATION;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = GetWindowElementPixel (HWND_NULL, WE_BGC_WINDOW);
    WndClass.WinProc     = AnimationCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

#define ANIM_STATUS_STOP        0
#define ANIM_STATUS_PLAY        1

typedef struct _ANIMATIONINFO
{
    ANIMATION*      anim;
    ANIMATIONFRAME* current;
    Uint32          status;
    Uint32          elapsed_10ms;
    HDC             mem_dc;
} ANIMATIONINFO;

#define ID_TIMER   100

#if 0

struct gal_pixel_table
{
    int size;
    int count;
    gal_pixel* colors;
};

static int pixel_compare(const void* a, const void* b)
{
    return (*(gal_pixel*)a)-(*(gal_pixel*)b);
}

static void add_pixel_to_gal_colors (struct gal_pixel_table* table, gal_pixel pixel)
{
    if(table->colors == NULL)
        table->colors = calloc(table->size, sizeof(gal_pixel));
    if(bsearch(&pixel, table->colors, table->count, 
                            sizeof(gal_pixel), pixel_compare))
        return;

    if (table->count == table->size) {
        table->colors = realloc(table->colors, table->size*2*sizeof(gal_pixel));
        memset(table->colors+table->size, 0, table->size*sizeof(gal_pixel));
        table->size *= 2;
    }
    table->colors[table->count] = pixel;
    table->count++;
    qsort(table->colors, table->count, sizeof(gal_pixel), pixel_compare);
}

static BOOL is_pixel_in_gal_colors(struct gal_pixel_table* table, 
                gal_pixel pixel)
{
    if (table->colors == NULL)
        return FALSE;
    if (bsearch(&pixel, table->colors, table->count, 
                            sizeof(gal_pixel), pixel_compare) == NULL)
        return FALSE;
    return TRUE;
}

static gal_pixel anim_get_color_key (ANIMATIONINFO* anim_info)
{
    int i;
    gal_pixel pixel;
    struct gal_pixel_table table;
    GAL_Color* colors;
    ANIMATIONFRAME* f;

    if (anim_info == NULL)
        return PIXEL_black;
    if (anim_info->anim == NULL)
        return PIXEL_black;

    table.size = 256;
    table.count = 0;
    table.colors = NULL;
    f = anim_info->anim->frames;
    colors = calloc(256, sizeof(GAL_Color));
    while(f)
    {
        int colornum = GetGDCapability(f->mem_dc, GDCAP_COLORNUM);
        if(colornum > 256)
            colornum = 256;
        GetPalette(f->mem_dc, 0, colornum, colors);
        for(i=0; i< colornum; i++)
        {
            pixel = RGB2Pixel(anim_info->mem_dc, 
                            colors[i].r, colors[i].g, colors[i].b);
            add_pixel_to_gal_colors(&table, pixel);
        }
        f = f->next;
    }
    free (colors);
    for(i=0; i<256; i++) // we try 256 times.
    {
        int r, g, b;
        r = rand()/(RAND_MAX/256);
        g = rand()/(RAND_MAX/256);
        b = rand()/(RAND_MAX/256);
        pixel = RGB2Pixel(anim_info->mem_dc, r, g, b);
        if(!is_pixel_in_gal_colors(&table, pixel)) {
            free (table.colors);
            return pixel;
        }
    }
    free (table.colors);
    return PIXEL_lightwhite;
}
#endif

static void draw_frame (HDC hdc, ANIMATIONFRAME* frame)
{
    if(frame == NULL)
        return;
    BitBlt (frame->mem_dc, 0, 0, 0, 0,
                hdc, frame->off_x, frame->off_y, 0);
}

static void restore_bk_color (ANIMATIONINFO* anim_info, ANIMATIONFRAME* frame)
{
    SetBrushColor (anim_info->mem_dc, GetBkColor(anim_info->mem_dc));
    FillBox (anim_info->mem_dc, frame->off_x, frame->off_y, 
                    frame->width, frame->height);
}

static void restore_prev_frame (ANIMATIONINFO* anim_info, ANIMATIONFRAME* frame)
{
    if (frame == NULL)
        return;

    switch (frame->disposal) {
        case 2:
            restore_bk_color (anim_info, frame);
            break;
        case 3:
            restore_prev_frame (anim_info, frame->prev);
            break;
        default:
            break;
    }
    draw_frame (anim_info->mem_dc, frame);
}

static void anim_treat_frame_disposal (ANIMATIONINFO* anim_info, 
                                       ANIMATIONFRAME* frame)
{
    int disposal = -1;
    
    if (frame)
        disposal = frame->disposal;
    else {
        SetBrushColor (anim_info->mem_dc, GetBkColor(anim_info->mem_dc));
        FillBox (anim_info->mem_dc, anim_info->anim->frames->off_x, anim_info->anim->frames->off_y, 
                 anim_info->anim->frames->width, anim_info->anim->frames->height);
    }

    switch (disposal) {
    case 2:
        restore_bk_color (anim_info, frame);
        break;
    case 3:
        restore_prev_frame (anim_info, frame->prev);
        break;
    default:
        break;
    }
}

static void next_frame (HWND hwnd, ANIMATIONINFO* anim_info)
{
    ANIMATIONFRAME* current;
    if (anim_info->anim == NULL)
        return;
    if (anim_info->current == NULL) {
        anim_info->current = anim_info->anim->frames;
    } else {
        if (anim_info->current->next == NULL 
                && (GetWindowStyle (hwnd) & ANS_AUTOLOOP)) {
            anim_info->current = anim_info->anim->frames;
        } else {
            anim_info->current = anim_info->current->next;
        }
    }
    current = anim_info->current;
    if (current) {
        anim_treat_frame_disposal (anim_info, current->prev);
        draw_frame (anim_info->mem_dc, current);
        InvalidateRect (hwnd, NULL, FALSE);
    }
    else {
        NotifyParent (hwnd, GetDlgCtrlID (hwnd), ANNC_NOFRAME);
    }
}

static void setup_anim_mem_dc (HWND hwnd, ANIMATIONINFO* anim_info)
{
    gal_pixel bk_pixel;
    HDC hdc;

    if (!anim_info->anim)
        return;

    if (anim_info->mem_dc)
        DeleteCompatibleDC (anim_info->mem_dc);
    
    hdc = GetClientDC (hwnd);
    
    anim_info->mem_dc = CreateCompatibleDCEx (hdc, 
                    anim_info->anim->width, anim_info->anim->height);

    if (GetWindowStyle (hwnd) & ANS_WINBGC) {
        if (hwnd != HWND_NULL) {
            bk_pixel = GetWindowElementPixel (hwnd, WE_BGC_WINDOW);
        }
        else {
            bk_pixel = GetWindowElementPixel (HWND_DESKTOP, WE_BGC_DESKTOP);
        }
    }
    else {
        RGB2Pixels (anim_info->mem_dc, &anim_info->anim->bk, &bk_pixel, 1);
    }

    if (GetWindowExStyle(hwnd) & WS_EX_TRANSPARENT) {
        SetMemDCColorKey (anim_info->mem_dc, MEMDC_FLAG_SRCCOLORKEY, bk_pixel);
    }

    SetBkColor (anim_info->mem_dc, bk_pixel);
    SetBrushColor (anim_info->mem_dc, bk_pixel);
    FillBox (anim_info->mem_dc, 0, 0,
            GetGDCapability (hdc, GDCAP_MAXX)+1, 
            GetGDCapability (hdc, GDCAP_MAXY)+1);
    
    ReleaseDC (hdc);
}

static LRESULT AnimationCtrlProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ANIMATIONINFO* anim_info = (ANIMATIONINFO*)GetWindowAdditionalData2 (hwnd);

    switch (message) {   
    case MSG_CREATE:
        if (!HaveFreeTimer ()) {
            _ERR_PRINTF ("EX_CTRL>Animation: no free timer is available!\n");
            return -1;
        }
        anim_info = (ANIMATIONINFO*) calloc (1, sizeof (ANIMATIONINFO));
        if (anim_info == NULL)
            return -1;
        
        SetWindowAdditionalData2 (hwnd, (DWORD)anim_info);
        if(lParam)
            SendMessage (hwnd, ANM_SETANIMATION, 0, lParam);
        break;
        
    case MSG_DESTROY:
        if (anim_info->mem_dc)
                DeleteCompatibleDC (anim_info->mem_dc);
        free (anim_info);
        /* timer will be freed automatically */
        //KillTimer (hwnd, ID_TIMER);
        break;
        
    case MSG_TIMER:
        if(anim_info->anim == NULL)
            return 0;
        if (anim_info->status == ANIM_STATUS_PLAY) {
            int delay_time = -1;
            if(anim_info->current) {
                delay_time = anim_info->current->delay_time;
            }
            anim_info->elapsed_10ms++;
            if (delay_time > 0 && anim_info->elapsed_10ms >= delay_time) {
                next_frame (hwnd, anim_info);
                anim_info->elapsed_10ms = 0;
            }
        }
        return 0;

    case MSG_PAINT: 
    {
        HDC hdc;
        RECT rc_anim;

        if (!anim_info->mem_dc)
            break;
        
        if (GetWindowStyle (hwnd) & ANS_SCALED) {
            GetClientRect (hwnd, &rc_anim);
        } else {
            SetRect (&rc_anim, 0, 0, anim_info->anim->width, 
                     anim_info->anim->height);
        }
       
        hdc = BeginPaint (hwnd);
        
        if (RECTW (rc_anim) == anim_info->anim->width
                && RECTH (rc_anim) == anim_info->anim->height) {
            BitBlt (anim_info->mem_dc, 0, 0, 0, 0, hdc, 0, 0, 0);
        } else {
            StretchBlt (anim_info->mem_dc, 0, 0, 
                        anim_info->anim->width, anim_info->anim->height,
                        hdc,
                        0, 0, 
                        RECTW (rc_anim), RECTH (rc_anim), 0);
        }
        
        EndPaint (hwnd, hdc);
        return 0;
    }
        
    case ANM_SETANIMATION: {
        
          ANIMATION* old = anim_info->anim;
          anim_info->anim = (ANIMATION*)lParam;
          anim_info->current = NULL;
          if (anim_info->anim) {
              KillTimer (hwnd, ID_TIMER);
              if (GetWindowStyle (hwnd) & ANS_FITTOANI) {
                  RECT rc;
                  GetWindowRect(hwnd, &rc);
                  MoveWindow(hwnd, rc.left, rc.top, 
                             anim_info->anim->width, 
                             anim_info->anim->height, FALSE);
              }
              anim_info->current = NULL;
              setup_anim_mem_dc(hwnd, anim_info);
              next_frame(hwnd, anim_info);
              anim_info->elapsed_10ms = 0;
              SetTimer (hwnd, ID_TIMER, anim_info->anim->time_unit);
          }
          return (LRESULT)old;
      }
      
    case ANM_GETANIMATION:
        return (LRESULT)anim_info->anim;
        
    case ANM_STARTPLAY: {
     
        if (anim_info->anim) {
            anim_info->current = NULL;
            anim_info->status = ANIM_STATUS_PLAY;
            next_frame(hwnd, anim_info);
            anim_info->elapsed_10ms = 0;
        }
        return 0;
    }
    case ANM_PAUSE_RESUME:
        if(anim_info->anim)
        {
            if (anim_info->status == ANIM_STATUS_PLAY)
                anim_info->status = ANIM_STATUS_STOP;
            else
                anim_info->status = ANIM_STATUS_PLAY;
        }
        return 0;
        
    case ANM_STOPPLAY:
        if(anim_info->anim)
        {
            anim_info->current = NULL;
            anim_info->status = ANIM_STATUS_STOP;
            next_frame (hwnd, anim_info); //back to the first frame
            anim_info->elapsed_10ms = 0;
            InvalidateRect (hwnd, NULL, FALSE);
        }
        return 0;
        
    case MSG_LBUTTONDBLCLK:
        NotifyParent (hwnd, GetDlgCtrlID (hwnd), ANNC_DBLCLK);
        break;
        
    case MSG_LBUTTONDOWN:
        NotifyParent (hwnd, GetDlgCtrlID (hwnd), ANNC_CLICKED);
        break;
        
    default:
        break;
    }
    
    return DefaultControlProc (hwnd, message, wParam, lParam);
}

#endif /* _MGCTRL_ANIMATION */

