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
**  mlshadow.c: Multi-Layer Shadow NEWGAL video driver.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#if defined( _MGGAL_MLSHADOW) && (_MGRM_THREADS)

#include "minigui.h"
#include "gdi.h"

#include "newgal.h"
#include "cliprect.h"
#include "sysvideo.h"
#include "pixels_c.h"

//#ifdef _MGGAL_MLSHADOW

#include <pthread.h>

#include "list.h"
#include "mlshadow-ths.h"

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "misc.h"
#include "bitmap.h"

#define MLSHADOW_NAME  "mlshadow"
#define LEN_MODE        20

#define MLSHADOW_CHECK(this) if(this){ \
                                if(strcmp(MLSHADOW_NAME, this->name)){ \
                                    fprintf(stderr, "NEWGAL: gal_enigne is %s, Init mlshadow failed.\n", this->name);\
                                    return FALSE; \
                                }\
                             }
/* Initialization/Query functions */
static int MLSHADOW_VideoInit (_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **MLSHADOW_ListModes (_THIS, GAL_PixelFormat *format, 
                Uint32 flags);
static GAL_Surface *MLSHADOW_SetVideoMode (_THIS, GAL_Surface *current, 
                int width, int height, int bpp, Uint32 flags);
static GAL_VideoDevice *MLSHADOW_CreateDevice(int devindex);
static int MLSHADOW_SetColors (_THIS, int firstcolor, int ncolors, 
                GAL_Color *colors);
static void MLSHADOW_VideoQuit (_THIS);
static void MLSHADOW_UpdateRects (_THIS, int numrects, GAL_Rect *rects);
static void MLSHADOW_UpdateSurfaceRects (_THIS, GAL_Surface* surface, int numrects, GAL_Rect *rects);
static void MLSHADOW_DeleteSurface (_THIS, GAL_Surface* surface);

static void MLSHADOW_DeleteDevice(GAL_VideoDevice *device);

/* Hardware surface functions */
static int MLSHADOW_AllocHWSurface (_THIS, GAL_Surface *surface);
static void MLSHADOW_FreeHWSurface (_THIS, GAL_Surface *surface);

static int MLSHADOW_SetSurfaceColors(GAL_Surface* surface, 
                int firstcolor, int ncolors, GAL_Color *colors);
static int MLSHADOW_Available(void);


extern BOOL GAL_ParseVideoMode (const char* mode, int* w, int* h, int* depth);
extern void Slave_FreeSurface (GAL_Surface *surface);
extern GAL_VideoDevice *GAL_GetVideo(const char* driver_name);

/* MLSHADOW driver bootstrap functions */
VideoBootStrap MLSHADOW_bootstrap = {
    MLSHADOW_NAME, "MLShadow video driver",
    MLSHADOW_Available, MLSHADOW_CreateDevice
};

/****************************/
typedef struct  __mlshadow_node 
{
    list_t node;
    int offset_x; 
    int offset_y; 
    DWORD flags;
    RECT updaterect;
    GAL_Surface* shadow_surface;
} mlshadow_node;

BOOL mlsEnableSlaveScreen (HDC dc_mls, BOOL enable)
{
    list_t * pos;
    list_t * back;
    BOOL flag = FALSE;
    mlshadow_node * node;
    GAL_Rect rect;
    GAL_VideoDevice * this;

    if (dc_mls == HDC_SCREEN)
        return FALSE;

    this = (GAL_VideoDevice *)((dc_HDC2PDC(dc_mls))->surface->video);
    /* houhh 20071116, check if  mlshadow. */
    MLSHADOW_CHECK(this);

    pthread_mutex_lock (&this->hidden->update_lock);

    list_for_each_safe(pos, back, &this->hidden->_mlshadow_list) {
        node = (mlshadow_node*)pos;
        if (node->shadow_surface == (dc_HDC2PDC(dc_mls))->surface) {
            flag = TRUE;
            break;
        }
    }

    if (!flag) {
        pthread_mutex_unlock (&this->hidden->update_lock);
        return FALSE;
    }

    node = (mlshadow_node*)pos;

//    node->flags = enable;
    if(enable == TRUE) //bugfix...
        node->flags |= MLSF_ENABLED;
    else if(enable == FALSE)
        node->flags &= ~MLSF_ENABLED; 

    pthread_mutex_unlock (&this->hidden->update_lock);

    rect.x = 0; rect.y = 0;
    rect.w = node->shadow_surface->w + rect.x;
    rect.h = node->shadow_surface->h + rect.y;
    MLSHADOW_UpdateSurfaceRects(this, node->shadow_surface, 1, &rect);

    return TRUE;
}

#if 0
void mlshadow_show_list (_THIS)
{
    list_t * pos;
    list_t * back;
    mlshadow_node * node;

    list_for_each_safe(pos, back, &this->hidden._mlshadow_list) {
        node = (mlshadow_node *)pos;
        fprintf(stderr, "\nstruct  __mlshadow_node \n "
                    "node = %p\n "
                    "offset_x = %d\n " 
                    "offset_y = %d\n "
                    "flags = %d\n "
                    "shadow_surface  = %p\n",
                    node, node->offset_x, node->offset_y, 
                    node->flags, node->shadow_surface);
    }

    return;
}
#endif


BOOL GUIAPI mlsGetSlaveScreenInfo (HDC dc_mls, DWORD mask,int* offset_x, int* offset_y,
                        DWORD blend_flags, gal_pixel* color_key, int* alpha, int* z_order)
{
    int index = 0;
    list_t * pos;
    list_t * back;
    BOOL flag;
    mlshadow_node * node;
    GAL_VideoDevice * this;

    this = (GAL_VideoDevice *)((dc_HDC2PDC(dc_mls))->surface->video);
    /* houhh 20071116, check if mlshadow. */
    MLSHADOW_CHECK(this);

    pthread_mutex_lock (&this->hidden->update_lock);
    flag = FALSE;

    if(!this->hidden->_mlshadow_list.next || !this->hidden->_mlshadow_list.prev)
        return FALSE;

    list_for_each_safe(pos, back, &this->hidden->_mlshadow_list) {
        node = (mlshadow_node*)pos;
        index++;
        if (node->shadow_surface == dc_HDC2PDC(dc_mls)->surface) {
            flag = TRUE;
            break;
        }    
    }
    if (!flag){
        pthread_mutex_unlock (&this->hidden->update_lock);
        return FALSE;
    }
    if (mask & MLS_INFOMASK_OFFSET) {
        if (offset_x < 0 || offset_y < 0)
            return FALSE;
        if(offset_x && offset_y){
            *offset_x = node->offset_x;
            *offset_y = node->offset_y;
            flag = TRUE;
       }
    }
    if (mask & MLS_INFOMASK_BLEND) {
        if ((blend_flags & MLS_BLENDMODE_COLORKEY) == MLS_BLENDMODE_COLORKEY) {
            if(color_key){
                *color_key = node->shadow_surface->format->colorkey;
                flag = TRUE;
            }
        }
        if ((blend_flags & MLS_BLENDMODE_ALPHA) == MLS_BLENDMODE_ALPHA) {
            if(alpha){
                *alpha = node->shadow_surface->format->alpha;
                flag = TRUE;
            }
        }
    }
    if (mask & MLS_INFOMASK_ZORDER){
        if(z_order)
            *z_order = index;
    }
    pthread_mutex_unlock (&this->hidden->update_lock);
    return flag;
}

BOOL GUIAPI mlsSetSlaveScreenInfo (HDC dc_mls, DWORD mask, 
                int offset_x, int offset_y, DWORD blend_flags, 
                gal_pixel color_key, int alpha, int z_order)
{
    list_t * pos;
    list_t * back;
    BOOL flag = FALSE;
    mlshadow_node* node;
    GAL_Rect rect;
    GAL_VideoDevice * this;

    this = (GAL_VideoDevice *)((dc_HDC2PDC(dc_mls))->surface->video);

    /* houhh 20071116, check if mlshadow. */
    MLSHADOW_CHECK(this);

    if(!this->hidden->_mlshadow_list.next || !this->hidden->_mlshadow_list.prev)
        return FALSE;

    pthread_mutex_lock (&this->hidden->update_lock);

    list_for_each_safe(pos, back, &this->hidden->_mlshadow_list) {
        node = (mlshadow_node*)pos;
        if (node->shadow_surface == dc_HDC2PDC(dc_mls)->surface) {
            flag = TRUE;
            break;
        }    
    }

    if (!flag){
        pthread_mutex_unlock (&this->hidden->update_lock);
        return flag;
    }

    if (mask & MLS_INFOMASK_OFFSET) {
        if (offset_x < 0 || offset_y < 0)
            return flag;
        if(node->offset_x != offset_x || node->offset_y != offset_y){
            rect.x = node->offset_x; rect.y = node->offset_y;
            rect.w = node->shadow_surface->w + rect.x;
            rect.h = node->shadow_surface->h + rect.y;
            /* bugfixed houhh 20071116, should update the surface's old rect 
             * by update the master surface. before update surface, should be
             * set current surface's offset.
             */
            node->offset_x = offset_x;
            node->offset_y = offset_y;
            pthread_mutex_unlock (&this->hidden->update_lock);
            MLSHADOW_UpdateSurfaceRects(this, dc_HDC2PDC(HDC_SCREEN)->surface, 1, &rect);
            pthread_mutex_lock (&this->hidden->update_lock);
        }
        else{
            pthread_mutex_unlock (&this->hidden->update_lock);
            return TRUE;
        }
    }

    if (mask & MLS_INFOMASK_BLEND) {
        if ((blend_flags & MLS_BLENDMODE_COLORKEY) == MLS_BLENDMODE_COLORKEY) {
            GAL_SetColorKey (node->shadow_surface, GAL_SRCCOLORKEY, color_key); 
            //    node->flags |= MLSF_ALPHA; 
            node->flags |= MLSF_COLORKEY;   //bugfix... 
        }

        if ((blend_flags & MLS_BLENDMODE_ALPHA) == MLS_BLENDMODE_ALPHA) {
            GAL_SetAlpha (node->shadow_surface, GAL_SRCALPHA, alpha);
            node->flags |= MLSF_ALPHA;
        }
    }

    if (mask & MLS_INFOMASK_ZORDER){
        list_del(pos);
        list_add_by_index(pos, &this->hidden->_mlshadow_list, z_order);
    }

    pthread_mutex_unlock (&this->hidden->update_lock);
    rect.x = 0; rect.y = 0;
    rect.w = node->shadow_surface->w + rect.x;
    rect.h = node->shadow_surface->h + rect.y;
    MLSHADOW_UpdateSurfaceRects(this, node->shadow_surface, 1, &rect);
    return TRUE;
}

static int MLSHADOW_Available (void)
{
    return 1;
}

static mlshadow_node* get_node(_THIS, GAL_Surface* surface)
{
    mlshadow_node* node = NULL;
    list_t* pos, *back;
    list_for_each_safe(pos, back, &this->hidden->_mlshadow_list){
        node = (mlshadow_node*)pos;
        if (node->shadow_surface == surface) {
            return node;
        }
    }
    return NULL;
}
                            
/* houhh 20071115
 * save the updaterect for each surface (master and slave surface).
 * GAL_Rect *rects's coordinate is relative slave surface.
 * the mlshadow_list's node is saved the updaterect that is relatived 
 * the master surface, not himself slave surface.
 */
static void MLSHADOW_UpdateSurfaceRects (_THIS, GAL_Surface* surface, int numrects, GAL_Rect *rects)
{
    int i;
    RECT bound;
    mlshadow_node* pnode = NULL;
    if(surface && this){ 
        pthread_mutex_lock (&this->hidden->update_lock);
    //  bound = this->hidden->update;
        pnode = get_node(surface->video, surface);
        if(pnode == NULL){
            pthread_mutex_unlock (&this->hidden->update_lock);
            return;
        }
        bound = pnode->updaterect;
        for(i = 0; i < numrects; i++) {
            RECT rc;
            rects[i].x += pnode->offset_x;
            rects[i].y += pnode->offset_y;
            SetRect (&rc, rects[i].x, rects[i].y, 
                    rects[i].x + rects[i].w, rects[i].y + rects[i].h);
            if (IsRectEmpty (&bound))
                bound = rc;
            else
                GetBoundRect (&bound, &bound, &rc);
        }
        if (!IsRectEmpty (&bound)) {
            if (IntersectRect (&bound, &bound, &g_rcScr)) {
                /* bugfixed houhh 20071116, the surface's dirty_rect can not beyond it's rect */
                RECT rect = {0};
                SetRect(&rect, pnode->offset_x, pnode->offset_y, 
                        surface->w + pnode->offset_x, surface->h + pnode->offset_y);
                if(IntersectRect (&bound, &bound, &rect)){
                    pnode->updaterect = bound;
                    this->hidden->dirty = TRUE;
                }
                //pnode->updaterect = bound;
                //this->hidden->dirty = TRUE;
            }
        }
        pthread_mutex_unlock (&this->hidden->update_lock);
    }
}

static void MLSHADOW_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    int i;
    RECT bound;
    pthread_mutex_lock (&this->hidden->update_lock);
    bound = this->hidden->update;
    for (i = 0; i < numrects; i++) {
        RECT rc;
        SetRect (&rc, rects[i].x, rects[i].y, 
                rects[i].x + rects[i].w, rects[i].y + rects[i].h);
        if (IsRectEmpty (&bound))
            bound = rc;
        else
            GetBoundRect (&bound, &bound, &rc);
    }
    if (!IsRectEmpty (&bound)) {
        if (IntersectRect (&bound, &bound, &g_rcScr)) {
            this->hidden->update = bound;
            this->hidden->dirty = TRUE;
        }
    }
    pthread_mutex_unlock (&this->hidden->update_lock);
}

static GAL_VideoDevice *MLSHADOW_CreateDevice (int devindex)
{
    static GAL_VideoDevice* device;

    /* Initialize all variables that we clean on shutdown */
    if (device == NULL) {
        device = (GAL_VideoDevice *)malloc (sizeof(GAL_VideoDevice));
        if (device) {
            memset (device, 0, (sizeof *device));
            device->hidden = (struct GAL_PrivateVideoData *)
            malloc((sizeof *device->hidden));
        }

        if ((device == NULL) || (device->hidden == NULL)) {
            GAL_OutOfMemory ();
            if (device) {
                free (device);
            }
            return (0);
        }

        memset (device->hidden, 0, sizeof (*device->hidden));
        /* Set the function pointers */
        device->VideoInit = MLSHADOW_VideoInit;
        device->ListModes = MLSHADOW_ListModes;
        device->SetVideoMode = MLSHADOW_SetVideoMode;
        device->SetSurfaceColors = MLSHADOW_SetSurfaceColors;
        device->UpdateSurfaceRects = MLSHADOW_UpdateSurfaceRects;
        device->SetColors = MLSHADOW_SetColors;
        device->VideoQuit = MLSHADOW_VideoQuit;
        device->AllocHWSurface = MLSHADOW_AllocHWSurface;
        device->CheckHWBlit = NULL;
        device->FillHWRect = NULL;
        device->SetHWColorKey = NULL;
        device->SetHWAlpha = NULL;
        device->FreeHWSurface = MLSHADOW_FreeHWSurface;
        device->UpdateRects = MLSHADOW_UpdateRects;
        device->DeleteSurface = MLSHADOW_DeleteSurface;
        device->free = MLSHADOW_DeleteDevice;

        /*Init _mlshadow_list */
        INIT_LIST_HEAD (&device->hidden->_mlshadow_list);

    }

    return device;
}

static int MLSHADOW_VideoInit (_THIS, GAL_PixelFormat *vformat)
{
    char real_engine [LEN_ENGINE_NAME + 1];
    char system_engine [LEN_ENGINE_NAME + 1];
    char double_buffer[20];
    GAL_PixelFormat real_vformat;
    
    /* houhh 20071116, if mlshadow is not the gal_engine, but call InitSlaveScreen. */
    if (GetMgEtcValue ("system", "gal_engine", system_engine, LEN_ENGINE_NAME) < 0) {
        return -1;
    }
    if(strcmp(system_engine, MLSHADOW_NAME)){
        return -1;
    }

    if (this->hidden->_real_surface != NULL) 
        return 0;

    this->hidden->dirty = FALSE;
    SetRect (&this->hidden->update, 0, 0, 0, 0);
    pthread_mutex_init (&this->hidden->update_lock, NULL);

    if (GetMgEtcValue (MLSHADOW_NAME, "real_engine", 
               real_engine, LEN_ENGINE_NAME) < 0) {
        return -1;
    }

    /* init real video engine here and get real_device. */
    this->hidden->_real_device = GAL_GetVideo (real_engine);
    if (this->hidden->_real_device == NULL) {
        return (-1);
    }

    memset (&real_vformat, 0, sizeof(real_vformat));
    if (this->hidden->_real_device->VideoInit (this->hidden->_real_device, 
                &real_vformat) < 0 ) {
        return (-1);
    }

    if (GetMgEtcValue (MLSHADOW_NAME, "def_bgcolor", 
                real_engine, LEN_ENGINE_NAME) < 0) {
        return -1;
    }

    this->hidden->_real_def_bgcolor = strtoul(real_engine, NULL, 0);
    if(this->hidden->_real_def_bgcolor > 0xffffff 
                || this->hidden->_real_def_bgcolor < 0) {
        this->hidden->_real_def_bgcolor = 0;
    }

    GetMgEtcValue (MLSHADOW_NAME, "double_buffer", double_buffer, LEN_ENGINE_NAME);  
    if(!strncmp(double_buffer, "enable", sizeof("enable"))) {
        this->hidden->swap_surface = malloc(sizeof(GAL_Surface));
    }

    this->hidden->_real_surface = GAL_CreateRGBSurface (GAL_SWSURFACE, 
                0, 0, real_vformat.BitsPerPixel, real_vformat.Rmask, 
                real_vformat.Gmask, real_vformat.Bmask, 0);

    if (this->hidden->_real_surface == NULL ) {
        this->hidden->_real_device->free (this->hidden->_real_device);
        return (-1);
    }      

    this->hidden->_real_surface->video = this->hidden->_real_device;
    this->hidden->_real_device->info.vfmt = this->hidden->_real_surface->format;

    /* We're done! */
    return 0;
}

/* FIXME: the following function is useless. */
static mlshadow_node* get_master_node(_THIS)
{
    mlshadow_node* pnode = NULL;
    list_t* pos, *back;
    PDC pdc = NULL;
    list_for_each_safe (pos, back, &this->hidden->_mlshadow_list) {
        pdc = dc_HDC2PDC (HDC_SCREEN);
        pnode = (mlshadow_node*)pos;
        if (pnode->shadow_surface == pdc->surface) {
            break;
        }
    }
    return pnode;
}

static void refresh_surface (_THIS, RECT* updaterect)
{
    list_t *me, *me2, *startnode;
    RECT intersect_rect;
    GAL_Rect cache_rect;
    mlshadow_node* pnode = NULL, *pnode2 = NULL, *master_node = NULL;
    GAL_Rect src = {0}, dst = {0}; 
    RECT* dirty_rect = NULL;
    RECT updatecache_rect = {0};

    master_node = get_master_node(this);
    updaterect = updaterect;

    list_for_each (me, &this->hidden->_mlshadow_list) {
        pnode = list_entry (me, mlshadow_node, node); 
        if (pnode && pnode->shadow_surface) {
            dirty_rect  = &pnode->updaterect;
            if(IsRectEmpty(dirty_rect))
                continue;
            cache_rect.x = dirty_rect->left;
            cache_rect.y = dirty_rect->top;
            cache_rect.w = RECTWP(dirty_rect);
            cache_rect.h = RECTHP(dirty_rect);

            if (master_node->shadow_surface && (master_node->flags & MLSF_ALPHA)) {    
                Uint32 color = this->hidden->_real_def_bgcolor; 
                GAL_FillRect (this->hidden->_real_surface, &cache_rect, 
                        RGB2Pixel(HDC_SCREEN, color & 0xff, (color & 0xff00) >> 8, (color & 0xff0000) >> 16));
            }
            startnode = me->prev;
            if((pnode->flags & MLSF_ALPHA) || (pnode->flags & MLSF_COLORKEY)){
                startnode = &this->hidden->_mlshadow_list;              // even surface is not enabled, it will affect other surface...
            }    
            //   list_for_each (me2, &this->hidden->_mlshadow_list)     // refresh the all list's surface... 
            list_for_each (me2, startnode){                             // only refresh the surface zorder hight...
                if(me2 == &this->hidden->_mlshadow_list){               // have reach end... 
                    break;
                }
                pnode2 = list_entry (me2, mlshadow_node, node); 
                if(pnode2 && pnode2->shadow_surface){
                    dst.x = pnode2->offset_x;
                    dst.y = pnode2->offset_y;
                    dst.w = pnode2->shadow_surface->w;       
                    dst.h = pnode2->shadow_surface->h;
                    src = dst;
                    if (pnode2->flags & MLSF_ENABLED) {
                        SetRect(&intersect_rect, dst.x, dst.y, 
                                dst.x + dst.w, dst.y + dst.h);

                        if (IntersectRect (&intersect_rect, &intersect_rect, dirty_rect)) {
                            dst.x = intersect_rect.left; dst.y = intersect_rect.top;          
                            dst.w = RECTW(intersect_rect);                                               
                            dst.h = RECTH(intersect_rect);                                               

                            src.x = dst.x - src.x;  src.y = dst.y - src.y;
                            src.w = dst.w;          src.h = dst.h;

                            if (this->hidden->swap_surface != NULL){
                                GAL_BlitSurface (pnode2->shadow_surface, &src,
                                        this->hidden->swap_surface, &dst);
                                if (IsRectEmpty (&updatecache_rect))
                                    updatecache_rect = intersect_rect;
                                else
                                    GetBoundRect (&updatecache_rect, &updatecache_rect, &intersect_rect);
#if 0
                                fprintf(stderr, "update_rect:(%d,%d)%d,%d,%d,%d\n", pnode2->offset_x, pnode2->offset_y, 
                                        dirty_rect->left, dirty_rect->top, RECTWP(dirty_rect), RECTHP(dirty_rect));
                                fprintf(stderr, "clip_rect:%d,%d,%d,%d\n", intersect_rect.left, intersect_rect.top, 
                                        RECTW(intersect_rect), RECTH(intersect_rect));
#endif
                            }
                            else{
                                //fprintf(stderr, "no swap_surface:clip_rect:%d,%d,%d,%d\n", intersect_rect.left, intersect_rect.top, 
                                //        RECTW(intersect_rect), RECTH(intersect_rect));
                                GAL_BlitSurface (pnode2->shadow_surface, &src,
                                        this->hidden->_real_surface, &dst);
                            }
                        }
                    }                       
                }
            }
            // if has enable the double_buffer, do real surface blit now...
            if(!this->hidden->swap_surface && this->hidden->_real_device->UpdateRects){
                this->hidden->_real_device->UpdateRects (this->hidden->_real_device,
                        1, &cache_rect);
            }
            dirty_rect->right  = dirty_rect->left;
            dirty_rect->bottom = dirty_rect->top;
        }
    }

    /* houhh 2007116, if has enable the double_buffer, do _real_surface blit here...
     * paint the cache surface to real surface...
     */ 
    if(this->hidden->swap_surface && !IsRectEmpty(&updatecache_rect)){
        //static int cc = 0;
        GAL_Rect rect = {0};
        rect.x = updatecache_rect.left;
        rect.y = updatecache_rect.top;
        rect.w = RECTW(updatecache_rect);
        rect.h = RECTH(updatecache_rect);
        GAL_BlitSurface (this->hidden->swap_surface, &rect,
                this->hidden->_real_surface, &rect);
        //fprintf(stderr, "%d:cacherect:%d,%d,%d,%d\n", cc++, rect.x, rect.y, rect.w, rect.h);
        if(this->hidden->_real_device->UpdateRects){
            this->hidden->_real_device->UpdateRects (this->hidden->_real_device, 
                    1, &rect);
        }
        updatecache_rect.right = updatecache_rect.left;
        updatecache_rect.top = updatecache_rect.bottom;
    }
}

static void* task_do_update (void* data)
{
    _THIS;
    this = data;
    while (1) {  
        __mg_os_time_delay (20);
        if (this->hidden->dirty) {
            pthread_mutex_lock (&this->hidden->update_lock);
            refresh_surface (this, &this->hidden->update);
            SetRect (&this->hidden->update, 0, 0, 0, 0);
            this->hidden->dirty = FALSE;
            pthread_mutex_unlock (&this->hidden->update_lock);
        }
    }
    return NULL;
}

static GAL_Surface *MLSHADOW_SetVideoMode (_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    int  ret = 0;
    mlshadow_node *shadow_node;
    char mode [LEN_MODE + 1], real_engine[LEN_ENGINE_NAME + 1];
    int w, h, depth;

    current->w = width;
    current->h = height;
    current->flags = flags;
    current->pitch = width * ((bpp + 7) / 8);
    current->pitch = (current->pitch + 3) & ~3;
    if (!GAL_ReallocFormat (current, bpp, 0, 0, 0, 0)) {
        return (NULL);
    }

    if (list_empty (&this->hidden->_mlshadow_list)) {

        /* create real surface. */  
        if (GetMgEtcValue (MLSHADOW_NAME, "real_engine", 
                real_engine, LEN_ENGINE_NAME) < 0) {
            fprintf (stderr, "NEWGAL>MLShadow: real_engine not defined.\n");
            return NULL;
        }
        
        if (GetMgEtcValue (real_engine, "defaultmode", mode, LEN_MODE) < 0) {
            if (GetMgEtcValue ("system", "defaultmode", mode, LEN_MODE) < 0) {
                fprintf (stderr, "NEWGAL>MLShadow: default mode not defined.\n");
                return NULL;
            }
        }

        if (!GAL_ParseVideoMode (mode, &w, &h, &depth)) {
            fprintf (stderr, "NEWGAL>MLShadow: bad real video mode parameter: %s.\n", mode);
            return NULL;
        }
        if (!(this->hidden->_real_surface 
                = this->hidden->_real_device->SetVideoMode (
                        this->hidden->_real_device, 
                        this->hidden->_real_surface, 
                        w, h, depth, GAL_HWPALETTE))) {
            fprintf (stderr, "NEWGAL>MLShadow: can not set video mode for real engine: %s.\n", mode);
            return NULL;
        }

        /* houhh 20071116 bugfix, reset the surface w/h/pitch.
         * if the input width/height/bpp is diffirent with the real_device.
         */ 
        
        current->w = w;
        current->h = h;
        current->pitch = w * ((depth + 7) / 8);
        current->pitch = (current->pitch + 3) & ~3;
        if (!GAL_ReallocFormat (current, depth, 0, 0, 0, 0)) {
            return (NULL);
        }

        /* houhh 20071116, set the slave surface must after the real surface init.*/
        if (this->hidden->swap_surface != NULL) {
            int swap_fb_size;
            int swap_fb_pitch;
            GAL_PixelFormat real_vformat = *this->hidden->_real_surface->format;
            bmp_ComputePitch(depth, (unsigned int)w, (unsigned int* )&swap_fb_pitch, 1);
            swap_fb_size = swap_fb_pitch * h;
            
            /* bugfixed 20071116, This will result GAL_FreeSurface() error. 
             * memcpy(this->hidden->swap_surface, this->hidden->_real_surface.
             * sizeof(GAL_Surface));
             */
            this->hidden->swap_surface = GAL_CreateRGBSurface (GAL_SWSURFACE, w, h, 
                    real_vformat.BitsPerPixel, real_vformat.Rmask, 
                    real_vformat.Gmask, real_vformat.Bmask, 0);
            if (this->hidden->swap_surface) {
                this->hidden->swap_surface->pixels = calloc(1, swap_fb_size);
                GAL_SetClipRect (this->hidden->swap_surface, NULL);
            }
        }

        GAL_SetClipRect (this->hidden->_real_surface, NULL);
    }

    // houhh 20071023...
    GAL_VideoSurface = current;
    if (this->hidden->_real_device->AllocHWSurface (this->hidden->_real_device,
                 current) <= 0)
       current->pixels = malloc (current->h * current->pitch);

    /*add shadow_node into list*/
    shadow_node = calloc (1, sizeof (mlshadow_node));
    shadow_node->offset_x = 0;
    shadow_node->offset_y = 0;
    shadow_node->shadow_surface = current;

    if (list_empty (&this->hidden->_mlshadow_list)) {
        shadow_node->flags = MLSF_ENABLED;

        pthread_attr_t new_attr;
        pthread_attr_init (&new_attr);
#ifndef __LINUX__
        pthread_attr_setstacksize (&new_attr, 512);
#endif
        pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
        ret = pthread_create (&this->hidden->update_th, 
                    &new_attr, task_do_update, this);
        pthread_attr_destroy (&new_attr);
    }
    else {
        shadow_node->flags = 0;
    //    /* Flag: when many surfaces = 1 */
    //    this->info.mlt_surfaces = 1;
    }

    /* Flag: when many surfaces = 1 */
    this->info.mlt_surfaces = 1;
    list_add_tail (&shadow_node->node, &this->hidden->_mlshadow_list);

    /* We're done */
    return (current);
}

static void MLSHADOW_DeleteDevice (GAL_VideoDevice *device)
{
    if(device->hidden->_real_device){ /* 20071008, check the real device if created success...*/
        device->hidden->_real_device->VideoQuit(device->hidden->_real_device);
        GAL_FreeSurface (device->hidden->_real_surface);
        device->hidden->_real_device->free(device->hidden->_real_device);

        if(device->hidden->swap_surface)
            GAL_FreeSurface (device->hidden->swap_surface);

        if (device->hidden->_real_device->screen != NULL) {
            device->hidden->_real_device->screen = NULL;
        }
    }
    free (device->hidden);
    free (device);
}

static void MLSHADOW_DeleteSurface (_THIS, GAL_Surface* surface)
{
    list_t * pos;
    list_t * back;
    BOOL flag;
    mlshadow_node* node = NULL;
    GAL_Rect rect;

    flag = FALSE;
    if (surface == NULL)
        return ;

    pthread_mutex_lock (&this->hidden->update_lock);
    list_for_each_safe(pos, back, &this->hidden->_mlshadow_list) {
        node = (mlshadow_node*)pos;
        if (node->shadow_surface == surface) {
            flag = TRUE;
            break;
        }
    }

    if (!flag) {
        pthread_mutex_unlock (&this->hidden->update_lock);
        return; 
    }
    list_del(pos);
    if (list_empty(&this->hidden->_mlshadow_list)) {
        this->info.mlt_surfaces = 0;
    }
    pthread_mutex_unlock (&this->hidden->update_lock);

    /* bugfixed houhh 20071116, should update the surface's old rect by update the master surface.*/
    rect.x = node->offset_x; rect.y = node->offset_y;
    rect.w = node->shadow_surface->w + rect.x;
    rect.h = node->shadow_surface->h + rect.y;
    MLSHADOW_UpdateSurfaceRects(this, (dc_HDC2PDC(HDC_SCREEN))->surface, 1, &rect);
    free(pos); // bugfixed, free slave surface node.
}

static void MLSHADOW_VideoQuit (_THIS)
{
    list_t * pos;
    list_t * back;
    mlshadow_node * node;

    GAL_VideoSurface = NULL;
    /* houhh 20071116, if user had delete slave surface, here will only
     * left master surface, else all the surface will free here.*/
    list_for_each_safe (pos, back, &this->hidden->_mlshadow_list) {
        node = (mlshadow_node*)pos;
        list_del (pos);
        Slave_FreeSurface (node->shadow_surface);
        if (list_empty(&this->hidden->_mlshadow_list)) {
            break;
        }
    }
    pthread_mutex_destroy (&this->hidden->update_lock);
}

static GAL_Rect **MLSHADOW_ListModes (_THIS, GAL_PixelFormat *format, 
                Uint32 flags)
{
    return (GAL_Rect **) -1;
}

/* We don't actually allow hardware surfaces other than the main one */
static int MLSHADOW_AllocHWSurface (_THIS, GAL_Surface *surface)
{
    return 0;
}

static void MLSHADOW_FreeHWSurface (_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}


/* houhh 20071115.
 * SetSurfaceColors() is used to set surface's palette when need.
 * it is distinguish with the SetColors().
 * it will call SetColors() when set the master surface.
 */ 
static int MLSHADOW_SetSurfaceColors (GAL_Surface* surface, 
                int firstcolor, int ncolors, GAL_Color *colors)
{
    list_t * pos, *back;
    mlshadow_node * pnode = NULL, *pmaster_node = NULL;
    GAL_Rect rect;
    GAL_Palette *pal = surface->format->palette;
    GAL_VideoDevice* this;
    BOOL flag = FALSE;

    if (surface == NULL) return -1;
    this = (GAL_VideoDevice *)(surface->video);

    pthread_mutex_lock (&this->hidden->update_lock);
    list_for_each_safe(pos, back, &this->hidden->_mlshadow_list) {
        pnode = (mlshadow_node*)pos;
        if (pnode->shadow_surface == surface) {
            flag = TRUE;
            break;
        }
    }
    if (!flag) {
        pthread_mutex_unlock (&this->hidden->update_lock);
        return -1;
    }
    pnode = (mlshadow_node*)pos;
    if (colors != (pal->colors + firstcolor) ) {
            memcpy (pal->colors + firstcolor, colors,
               ncolors * sizeof(*colors));
    }

    pmaster_node = get_master_node(this);
    // houhh 20071116, the __gal_screen is NULL now...
    //if((dc_HDC2PDC(HDC_SCREEN))->surface == surface){
    if(pmaster_node->shadow_surface == surface){
        this->SetColors(this, firstcolor, ncolors, colors);
    }

    GAL_FormatChanged (surface);
    pthread_mutex_unlock (&this->hidden->update_lock);

    rect.x = 0; rect.y = 0;
    rect.w = pnode->shadow_surface->w + rect.x;
    rect.h = pnode->shadow_surface->h + rect.y;
    MLSHADOW_UpdateSurfaceRects(this, pnode->shadow_surface, 1, &rect);
    return 0;
}

/* houhh 20071115.
 * if the surface is master surface, then set the master. 
 * surface's palette and the _real_device's palette. 
 * set the swap_surface and real_surface and _real_device palette.
 */ 
static int MLSHADOW_SetColors (_THIS, int firstcolor, int ncolors,
        GAL_Color *colors)
{
    GAL_Surface* surface = this->hidden->_real_surface;
    GAL_Palette* pal = surface->format->palette;

    if(!pal) return -1;

    if (colors != (pal->colors + firstcolor) ) {
        memcpy (pal->colors + firstcolor, colors,
                ncolors * sizeof(*colors));
    }
    if(this->hidden->swap_surface){
        pal = this->hidden->swap_surface->format->palette;
        if(pal){
            if (colors != (pal->colors + firstcolor) ) {
                memcpy (pal->colors + firstcolor, colors,
                        ncolors * sizeof(*colors));
            }
        }
    }
    this->hidden->_real_device->SetColors(this->hidden->_real_device, 
            firstcolor, ncolors, colors);
    return 0;
}

#endif /* _MGGAL_MLSHADOW && _MGRM_THREADS*/

