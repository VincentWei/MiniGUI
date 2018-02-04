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
**  mlshadow.c: Multi-Layer MLShadow NEWGAL video driver.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "common.h"

#if defined( _MGGAL_MLSHADOW) && defined ( _MGRM_PROCESSES)

#include "minigui.h"
#include "gdi.h"
#include <sys/mman.h>
#include "newgal.h"
#include "cliprect.h"
#include "sysvideo.h"
#include "pixels_c.h"

#include <pthread.h>
#include "list.h"
#include "mlshadow-proc.h"

#include "window.h"
#include "control.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "misc.h"
#include "client.h"

/* FIXME: Please reduce use of local variables. */

#define MLSHADOW_NAME  "mlshadow"
#define LEN_MODE 20

#define REQID_MLSHADOW (MAX_SYS_REQID + 1)
#define PALETTE_SIZE 1024

#define MLS_FB_TYPE_UNKNOWMEM       0
#define MLS_FB_TYPE_SYSTEMMEM       1
#define MLS_FB_TYPE_VIDEOMEM        2

#define LEN_ENGINE_NAME 10

#define MLSHADOW_CHECK(this) if(this){ \
                                if(strcmp(MLSHADOW_NAME, this->name)){ \
                                    fprintf(stderr, "NEWGAL: gal_enigne is %s, Init mlshadow failed.\n", this->name);\
                                    return FALSE; \
                                }\
                             }

typedef struct _MLShadowFBHeader {
    int type;
    int depth;
    unsigned int Rmask, Gmask, Bmask, Amask;
    /* houhh 20071116, surface's offset info.*/
    int offset_x, offset_y;
    int width, height;
    int pitch;
    int dirty;
    RECT dirty_rect;
    int video_offset;
    int palette_offset;
    BOOL palette_changed;
    unsigned int surface_key;
    int semid;
} MLShadowFBHeader;

static void unlock_fbh (const MLShadowFBHeader* fbh) 
{
    struct sembuf sb;
    
again:
    sb.sem_num = 1;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;

    if (semop (fbh->semid, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }
}

static void lock_fbh (const MLShadowFBHeader* fbh)
{
    struct sembuf sb;
    
again:
    sb.sem_num = 1;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;

    if (semop (fbh->semid, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }

}
static list_t mlshadow_list;
/*
 * * request_id = REQ_MLSHADOW_OPERATE
 * * request data type: int
 * * op_id = MLSOP_ID_GET_MASTERINFO 
 * * request_data_len = sizeof (int)
 * * reply data type: MLSHADOW_REPLY_MASTER_INFO
 * * reply_data_len = sizeof (MLSHADOW_REPLY_MASTER_INFO)
 * */



/*
 * * request_id = REQ_MLSHADOW_OPERATE
 * * request data type: MLSHADOW_REQ_SLAVE_CREATE
 * * op_id = MLSOP_ID_CREATE_SLAVE 
 * * request_data_len = sizeof (MLSHADOW_REQ_SLAVE_CREATE)
 * * reply data type: MLSHADOW_REPLY_SLAVE_CREATE
 * * reply_data_len = sizeof (MLSHADOW_REPLY_SLAVE_CREATE)
 * */
typedef struct  _MLSHADOW_REQ_SURFACE_CREATE {
    int op_id;
    int width;
    int height;
    int flags;
    int depth;
}MLSHADOW_REQ_SURFACE_CREATE;

/*
 * * request_id = REQ_MLSHADOW_OPERATE
 * * request data type: MLSHADOW_REQ_SLAVE_GETINFO
 * * op_id = MLSOP_ID_GET_SLAVEINFO
 * * request_data_len = sizeof (MLSHADOW_REQ_SLAVE_GETINFO)
 * * reply data type: MLSHADOW_REPLY_SLAVE_GETINFO
 * * reply_data_len = sizeof (MLSHADOW_REPLY_SLAVE_GETINFO)
 * */
typedef struct _MLSHADOW_REQ_SLAVE_GETINFO {
    int op_id;
    unsigned int surface_key;
}MLSHADOW_REQ_SLAVE_GETINFO;

/*
 * * request_id = REQ_MLSHADOW_OPERATE
 * * request data type: MLSHADOW_REQ_SLAVE_SETINFO
 * * op_id = MLSOP_ID_SET_SLAVEINFO 
 * * request_data_len = sizeof (MLSHADOW_REQ_SLAVE_SETINFO)
 * * reply data type: BOOL
 * * reply_data_len = sizeof (BOOL)
 * */
typedef struct _MLSHADOW_REQ_SLAVE_SETINFO {
    int op_id;
    unsigned int surface_key;

    DWORD mask;
    int offset_x; 
    int offset_y;
    int enable; 
    DWORD blend_flags;
    gal_pixel color_key;
    int alpha;
    int z_order;
}MLSHADOW_REQ_SLAVE_SETINFO;

/*
 * * request_id = REQ_MLSHADOW_OPERATE
 * * request data type: (MLSHADOW_REQ_SLAVE_DESTROY
 * * op_id = MLSOP_ID_DESTROY_SLAVE 
 * * request_data_len = sizeof (MLSHADOW_REQ_SLAVE_DESTROY)
 * * reply data type: BOOL
 * * reply_data_len = sizeof (BOOL)
 * */
typedef struct _MLSHADOW_REQ_SLAVE_DESTROY {
    int op_id;
    unsigned int surface_key;
}MLSHADOW_REQ_SLAVE_DESTROY;

typedef struct  _mlshadow_node 
{
    list_t node;
    BOOL enabled;
    DWORD blend_flags;
    gal_pixel color_key;
    int alpha;
    RECT rect;
    int shmid;
    GAL_Surface* surface;
} mlshadow_node;

BOOL GAL_ParseVideoMode (const char* mode, int* w, int* h, int* depth);
void Slave_FreeSurface (GAL_Surface *surface);
static mlshadow_node* get_master_node(void);
static int MLSHADOW_GetSurface(MLSHADOW_REQ_SLAVE_GETINFO* request, 
                MLSHADOW_REPLY_SLAVE_GETINFO* reply); 

/* Initialization/Query functions */
int MLSHADOW_Server(void* request, void* reply);
static int MLSHADOW_SetSurface(MLSHADOW_REQ_SLAVE_SETINFO *request, BOOL *reply);
void MLSHADOW_Del_List(void);
static int MLSHADOW_CreateFrameBuffer(MLSHADOW_REQ_SURFACE_CREATE* request,
                MLSHADOW_REPLY_SURFACE_CREATE* reply);
static int MLSHADOW_SetSurfaceColors (GAL_Surface* surface, 
                int firstcolor, int ncolors, GAL_Color *colors);

static void MLSHADOW_DeleteMLSurface (MLSHADOW_REQ_SLAVE_DESTROY *request, 
                BOOL *reply);
static mlshadow_node* get_node(GAL_Surface* surface);
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
static void MLSHADOW_UpdateSurfaceRects (_THIS, GAL_Surface* surface, int numrects, 
                GAL_Rect *rects);
static void MLSHADOW_DeleteSurface (_THIS, GAL_Surface* surface);
static void MLSHADOW_DeleteDevice(GAL_VideoDevice *device);

/* Hardware surface functions */
static int MLSHADOW_AllocHWSurface (_THIS, GAL_Surface *surface);
static void MLSHADOW_FreeHWSurface (_THIS, GAL_Surface *surface);

static int MLSHADOW_Available(void);
static void* task_do_update (_THIS);
static char* _mmaped_mem;
static VIDEO_MEM_INFO video_mem_info;

extern int bmp_ComputePitch(int bpp, Uint32 width, Uint32 *pitch, BOOL does_round);
extern GAL_VideoDevice* GAL_GetVideo (const char* );

/* MLSHADOW driver bootstrap functions */

VideoBootStrap MLSHADOW_bootstrap = {
    MLSHADOW_NAME, "MLShadow video driver",
    MLSHADOW_Available, MLSHADOW_CreateDevice
};

static int MLSHADOW_Available (void)
{
    return 1;
}

static GAL_VideoDevice *MLSHADOW_CreateDevice (int devindex)
{
    static GAL_VideoDevice* device;

    /* Initialize all variables that we clean on shutdown */
    if (device == NULL) {
        device = (GAL_VideoDevice *)malloc (sizeof(GAL_VideoDevice));
        if (device) {
            memset (device, 0, (sizeof *device));
            if (mgIsServer) {
                device->hidden = (struct GAL_PrivateVideoData *)
                    malloc((sizeof *device->hidden));
                if (device->hidden == NULL) {
                    GAL_OutOfMemory ();
                    if (device) {
                        free (device);
                    }
                    return (0);
                }
                memset (device->hidden, 0, sizeof (*device->hidden));
            }
        }

        if (device == NULL) {
            GAL_OutOfMemory ();
            if (device) {
                free (device);
            }
            return (0);
        }

        /* Set the function pointers */
        device->VideoInit = MLSHADOW_VideoInit;
        device->ListModes = MLSHADOW_ListModes;
        device->SetVideoMode = MLSHADOW_SetVideoMode;
        device->UpdateSurfaceRects = MLSHADOW_UpdateSurfaceRects; 
        device->SetColors = MLSHADOW_SetColors;
        device->SetSurfaceColors = MLSHADOW_SetSurfaceColors;
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
        /* Palette set function use it in video.c */
        device->info.mlt_surfaces = 1;
    }

    return device;
}

static int MLSHADOW_VideoInit (_THIS, GAL_PixelFormat *vformat)
{
    char real_engine [LEN_ENGINE_NAME + 1];
    char double_buffer[20];
    GAL_PixelFormat real_vformat;
    if (mgIsServer && (this->hidden->_real_device == NULL)) {
        /*Init _mlshadow_list */
        INIT_LIST_HEAD (&mlshadow_list);

        if (GetMgEtcValue (MLSHADOW_NAME, "real_engine", real_engine, LEN_ENGINE_NAME) < 0) {
            return -1;
        }
        /* init real video engine here and get real_device. */
        this->hidden->_real_device = GAL_GetVideo ((const char*) real_engine);
        if (this->hidden->_real_device == NULL) {
            return (-1);
        }
        memset (&real_vformat, 0, sizeof(real_vformat));
        if (this->hidden->_real_device->VideoInit (this->hidden->_real_device, 
                    &real_vformat) < 0 ) 
        {
            return (-1);
        }
        if (GetMgEtcValue (MLSHADOW_NAME, "def_bgcolor", real_engine, 
                    LEN_ENGINE_NAME) < 0) 
        {
            return -1;
        }
        GetMgEtcValue (MLSHADOW_NAME, "double_buffer", double_buffer, LEN_ENGINE_NAME); 
        this->hidden->_real_def_bgcolor = PIXEL_blue;
        if(this->hidden->_real_def_bgcolor > 0xffffff || 
                this->hidden->_real_def_bgcolor < 0)
        {
            this->hidden->_real_def_bgcolor = 0;
        }
        this->hidden->_real_surface = GAL_CreateRGBSurface (GAL_SWSURFACE, 0, 0, 
                real_vformat.BitsPerPixel, real_vformat.Rmask, 
                real_vformat.Gmask, real_vformat.Bmask, 0);
        if (this->hidden->_real_surface == NULL ) {
            this->hidden->_real_device->free (this->hidden->_real_device);
            return (-1);
        }      
        this->hidden->_real_surface->video = this->hidden->_real_device;
        this->hidden->_real_device->info.vfmt = this->hidden->_real_surface->format;
        if(!strncmp(double_buffer, "enable", sizeof("enable"))) {
            this->hidden->swap_surface = malloc(sizeof(GAL_Surface));
        }
        if(this->hidden->_real_device->GetFBInfo){
            this->hidden->_real_device->GetFBInfo(&video_mem_info);
            if (video_mem_info.video_mem_len > 0) {
                /* bugfixed, houhh 20071116, 1.mmap para error! 2.do mmap here, not in SetVideoMode.*/
                if(video_mem_info.type == MLS_FB_TYPE_VIDEOMEM) { 
                    int fd = open(video_mem_info.video_dev_name, O_RDWR/*O_RDONLY*/);
                    //_mmaped_mem = mmap(NULL, video_mem_info.video_mem_len, fd, 
                    //        PROT_READ|PROT_WRITE, MAP_SHARED, video_mem_info.video_mem_offset);
                    _mmaped_mem = mmap(NULL, video_mem_info.video_mem_len,
                            PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
                }
                video_mem_info.type = MLS_FB_TYPE_VIDEOMEM;
                fprintf(stderr, "video_mem_info = %d\n", video_mem_info.video_mem_len);
            }
            else 
                video_mem_info.type =  MLS_FB_TYPE_SYSTEMMEM; 
        }
        else
            video_mem_info.type =  MLS_FB_TYPE_SYSTEMMEM; 
    }
    else if(!mgIsServer){
        int request;
        MLSHADOW_REPLY_MASTER_INFO reply;
        REQUEST req;
        static BOOL _mmaped_flag = FALSE;
        //MLShadowFBHeader *header = NULL;

        /* Clent mmap the video memory if need.*/
        /* bugfixed, houhh 20071116, 1.mmap para error! 2.do mmap here, not in SetVideoMode.*/
        if(!_mmaped_flag){
            request = MLSOP_ID_GET_MASTERINFO;
            req.len_data = sizeof(request);
            req.data = &request;
            req.id = REQID_MLSHADOW_CLIREQ;

            if(ClientRequest(&req, &reply, sizeof(reply)) < 0){
                fprintf(stderr, "NEWGAL>MLSHADOW: Request server create FB error\n");
                return NULL;
            }
            _mmaped_flag = TRUE;
            video_mem_info.type = MLS_FB_TYPE_VIDEOMEM; 
            video_mem_info.video_mem_len = reply.video_mem_len;
            memcpy(video_mem_info.video_dev_name, reply.video_dev_name, 
                    sizeof(reply.video_dev_name));
            video_mem_info.video_mem_offset = reply.video_mem_offset;
            video_mem_info.video_pitch = reply.pitch;
            if(!reply.ret_value) {
                //header = shmat(reply.shm_id_header, 0, 0);
                //if(header->type == MLS_FB_TYPE_VIDEOMEM){
                if(video_mem_info.type == MLS_FB_TYPE_VIDEOMEM){
                    int fd;
                    fd = open(video_mem_info.video_dev_name, O_RDWR/*O_RDONLY*/);
                    _mmaped_mem = mmap(NULL, video_mem_info.video_mem_len,
                            PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
                    fprintf(stderr, "_mmaped_mem=%0x, video_mem_info = %d\n", _mmaped_mem, video_mem_info.video_mem_len);
                }
            }
        }
    }
    return 0;
}


static GAL_Surface *MLSHADOW_SetVideoMode (_THIS, GAL_Surface *current,
                int width, int height, int bpp, Uint32 flags)
{
    int  ret = 0;
    char mode [LEN_MODE + 1], real_engine[LEN_ENGINE_NAME + 1];
    int w, h, depth;
    static BOOL client_first = TRUE;
    MLShadowFBHeader *header;
    int fd;

    current->w = width;
    current->h = height;
    current->flags = flags;
    bmp_ComputePitch(bpp, (unsigned int)width, (unsigned int* )
                    &current->pitch, 1);

    if (!GAL_ReallocFormat (current, bpp, 0, 0, 0, 0)) {
        return (NULL);
    }

    if (mgIsServer && list_empty (&mlshadow_list)) {
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
            fprintf (stderr, "NEWGAL>MLShadow: bad real video mode parameter: %s.\n"
                            , mode);
            return NULL;
        }
        if (!(this->hidden->_real_surface 
                    = this->hidden->_real_device->SetVideoMode (
                        this->hidden->_real_device, 
                        this->hidden->_real_surface, 
                        w, h, depth, GAL_HWPALETTE))) {
            fprintf (stderr, "NEWGAL>MLShadow: can not set video mode for real "
                            "engine: %s.\n", mode);
            return NULL;
        }

        // houhh 20071116 bugfix, reset the surface w/h/pitch...
        // use the real_device's mode.
        current->w = w;
        current->h = h;
        bpp = depth;
        if (!GAL_ReallocFormat (current, depth, 0, 0, 0, 0)) {
            return (NULL);
        }

        // houhh 20071116, set the slave surface must after the real surface init...
        if (this->hidden->swap_surface != NULL) {
            int swap_fb_size;
            int swap_fb_pitch;
            GAL_PixelFormat real_vformat = *this->hidden->_real_surface->format;
            bmp_ComputePitch(depth, (unsigned int)w, (unsigned int* )&swap_fb_pitch, 1);
            swap_fb_size = swap_fb_pitch * h; 
            // bugfixed 20071116, should GAL_ReallocFormat the swap surface's palette. 
            // memcpy(this->hidden->swap_surface, this->hidden->_real_surface, 
            //                sizeof(GAL_Surface));
            this->hidden->swap_surface = GAL_CreateRGBSurface (GAL_SWSURFACE, w, h, 
                    real_vformat.BitsPerPixel, real_vformat.Rmask, 
                    real_vformat.Gmask, real_vformat.Bmask, 0);
            if (this->hidden->swap_surface) {
                this->hidden->swap_surface->pixels = calloc(1, swap_fb_size);
                GAL_SetClipRect (this->hidden->swap_surface, NULL);
            }
        }

        GAL_SetClipRect (this->hidden->_real_surface, NULL);

        /* houhh 20071116. add the real_surface's fb_size. */
        if(video_mem_info.type == MLS_FB_TYPE_VIDEOMEM){
            int fb_size, fb_pitch;
            bmp_ComputePitch(depth, (unsigned int)w, (unsigned int* )&fb_pitch, 1);
            fb_size = fb_pitch * h; 
            video_mem_info.video_mem_offset += fb_size;
        }
                        
        pthread_attr_t new_attr;
        pthread_attr_init (&new_attr);
#ifndef __LINUX__
        pthread_attr_setstacksize (&new_attr, 512);
#endif
        pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
        ret = pthread_create (&this->hidden->update_th, 
                &new_attr, (void*)task_do_update, this);
        pthread_attr_destroy (&new_attr);
    }

    this->info.mlt_surfaces = 1;
    if (mgIsServer) {
        MLSHADOW_REQ_SURFACE_CREATE request;
        MLSHADOW_REPLY_SURFACE_CREATE reply;
        MLSHADOW_REQ_SLAVE_SETINFO request_enable;
        BOOL reply_enable;
        
        request.op_id = MLSOP_ID_CREATE_SURFACE;
        request.width = width;
        request.height = height;
        request.flags = flags;
        request.depth = bpp;

        MLSHADOW_CreateFrameBuffer (&request, &reply);

        if(!reply.ret_value) {
            header = shmat(reply.shm_id_header, 0, 0);
            switch(header->type) {
                case MLS_FB_TYPE_VIDEOMEM : 
                    if(video_mem_info.type == MLS_FB_TYPE_VIDEOMEM) { 
                        /* houhh 20071116, move these code to VideoInit().*/
                        //fd = open(video_mem_info.video_dev_name, O_RDONLY);
                        //_mmaped_mem = mmap(NULL, video_mem_info.video_mem_len, fd, 
                        //        PROT_WRITE, MAP_SHARED, video_mem_info.video_mem_offset);
                        current->pixels = _mmaped_mem + header->video_offset; 
                        current->pitch = video_mem_info.video_pitch;
                    }
                    break;
                case MLS_FB_TYPE_SYSTEMMEM :
                    current->pixels = (char*)header + sizeof(*header);
                    break;
            }

            current->pitch = header->pitch;
            current->hwdata = (struct private_hwdata*)header;
            
            request_enable.op_id = MLSOP_ID_SET_SLAVEINFO;
            request_enable.surface_key = header->surface_key;
            request_enable.mask = MLS_INFOMASK_ENABLE;
            request_enable.enable = 1;
              
            MLSHADOW_SetSurface(&request_enable, &reply_enable);
        }
    }
    else{
        if(client_first){
            int request;
            int fb_size, fb_pitch;
            MLSHADOW_REPLY_MASTER_INFO reply;
            REQUEST req;

            request = MLSOP_ID_GET_MASTERINFO;
            req.len_data = sizeof(request);
            req.data = &request;
            req.id = REQID_MLSHADOW_CLIREQ;

            /* houhh 20071116, add the real_surface's fb_size.*/
            if(video_mem_info.type == MLS_FB_TYPE_VIDEOMEM)
            {
                bmp_ComputePitch(depth, (unsigned int)w, (unsigned int* )&fb_pitch, 1);
                fb_size = fb_pitch * h; 
                video_mem_info.video_mem_offset += fb_size;
            }
            if(ClientRequest(&req, &reply, sizeof(reply)) < 0){
                fprintf(stderr, "NEWGAL>MLSHADOW: Request server create FB error\n");
                return NULL;
            }
            if(!reply.ret_value) {
                header = shmat(reply.shm_id_header, 0, 0);
                switch(header->type) {
                    case MLS_FB_TYPE_VIDEOMEM :
                        if(reply.type == MLS_FB_TYPE_VIDEOMEM) { 
                            /*
                            video_mem_info.type = MLS_FB_TYPE_VIDEOMEM; 
                            video_mem_info.video_mem_len = reply.video_mem_len;
                            memcpy(video_mem_info.video_dev_name, reply.video_dev_name, 
                                    sizeof(reply.video_dev_name));

                            video_mem_info.video_mem_offset = reply.video_mem_offset;
                            video_mem_info.video_pitch = reply.pitch;
                            fd = open(reply.video_dev_name, O_RDONLY);
                            _mmaped_mem = mmap(NULL, reply.video_mem_len, fd, PROT_WRITE, 
                                    MAP_SHARED, reply.video_mem_offset);
                            */

                            /* houhh 20071116. add the real_surface's fb_size. */
                            current->pitch = reply.pitch;
                            current->pixels = _mmaped_mem + header->video_offset; 
                        }
                        break;
                    case MLS_FB_TYPE_SYSTEMMEM :
                        current->pixels = (char*)header + sizeof(*header);
                        break;
                }
                current->pitch = header->pitch;
                current->hwdata = (struct private_hwdata*)header;
                client_first = FALSE;
            }
            else{
                return NULL;
            }
        }
        else{
            MLSHADOW_REQ_SURFACE_CREATE request;
            MLSHADOW_REPLY_SURFACE_CREATE reply;
            REQUEST req;

            memset(&request, 0, sizeof(request));
            request.op_id  = MLSOP_ID_CREATE_SURFACE;
            request.depth  = bpp;
            request.height = height;
            request.width  = width;
            request.flags  = flags;

            req.id       = REQID_MLSHADOW_CLIREQ;
            req.len_data = sizeof(request);
            req.data     = &request;

            if(ClientRequest(&req, &reply, sizeof(reply)) < 0){
                fprintf(stderr, "NEWGAL>MLSHADOW: Request server create FB error\n");
                return NULL;
            }

            if(!reply.ret_value) {
                header = shmat(reply.shm_id_header, 0, 0);
                switch(header->type) {
                    case MLS_FB_TYPE_VIDEOMEM :
                        current->pitch = video_mem_info.video_pitch;
                        current->pixels = _mmaped_mem + header->video_offset; 
                        break;
                    case MLS_FB_TYPE_SYSTEMMEM :
                        current->pixels = (char*)header + sizeof(*header);
                        break;
                }
                current->pitch = header->pitch;
                current->hwdata = (struct private_hwdata*)header;
            }
            else{
                return NULL;
            }
        }
    }

    /* save surface address */
    this->info.mlt_surfaces = 1;
    
    return (current);
}
 
static void MLSHADOW_UpdateSurfaceRects (_THIS, GAL_Surface* surface, int numrects, 
                GAL_Rect *rects)
{
    int i;
    RECT bound;
    MLShadowFBHeader *header;
    header = (MLShadowFBHeader*)surface->hwdata;
    lock_fbh(header);
    if(surface){ 
        bound = header->dirty_rect;
        for(i = 0; i < numrects; i++) {
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
                /* bugfixed houhh 20071116, the surface's dirty_rect can not beyond it's rect */
                RECT rect = {0};
                SetRect(&rect, header->offset_x, header->offset_y, 
                        header->width + header->offset_x, header->height + header->offset_y);
                if(IntersectRect (&bound, &bound, &rect)){
                    header->dirty_rect = bound;
                    header->dirty = TRUE;
                }
                //header->dirty_rect = bound;
                //header->dirty = TRUE;
            }
        }
    }
    unlock_fbh(header); 
}


static int MLSHADOW_SetSurfaceColors (GAL_Surface* surface, 
                int firstcolor, int ncolors, GAL_Color *colors)
{
    GAL_Palette *pal;
    GAL_Color *dst_colors;
    MLShadowFBHeader *header;
    GAL_VideoDevice* this;

    if (surface == 0)
        return -1;

    this = (GAL_VideoDevice *)(surface->video);
    pal = surface->format->palette;
    if(!pal) return -1;
    header = (MLShadowFBHeader*)surface->hwdata;
    if (colors != (pal->colors + firstcolor) ) {
        memcpy (pal->colors + firstcolor, colors,
                ncolors * sizeof(*colors));
    }
    dst_colors = (GAL_Color*)((char*)(header) + header->palette_offset);
    if (colors != (dst_colors+ firstcolor) ) {
        memcpy (dst_colors + firstcolor, colors,
                ncolors * sizeof(*colors));
    }
    /* houhh 20071115...
    *  set the palette changed flag...
    *  then update the palette when the server do the task_do_update...
    */
    header->palette_changed = TRUE;
    GAL_FormatChanged (surface);
    return 0;
}


/* houhh 20071115.
 * it will called by the palette_update_check(), when task_do_update().
 * called in mlshadow server. 
 */
static int MLSHADOW_SetColors (_THIS, int firstcolor, int ncolors,
        GAL_Color *colors)
{
    if(mgIsServer && this){
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
        this->hidden->_real_device->SetColors(this->hidden->_real_device, firstcolor, ncolors, colors);
    }
    // client call return here...
    return 0;
}

static void MLSHADOW_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    return; 
}


static int MLSHADOW_CreateFrameBuffer(MLSHADOW_REQ_SURFACE_CREATE* request, 
                MLSHADOW_REPLY_SURFACE_CREATE* reply)
{
    int width, height;
    int depth, pitch;
    int flags;
    GAL_Surface *surface;
    mlshadow_node *node;
    int fb_size, shm_size;
    GAL_PixelFormat real_vformat;
    MLShadowFBHeader *header = NULL;
    BOOL videomem_enough  = TRUE;

    width  = request->width;
    height = request->height;
    depth  = request->depth;
    flags  = request->flags;

    reply->ret_value = -1;        
    
    memset (&real_vformat, 0, sizeof(real_vformat));
    surface = GAL_CreateRGBSurface(flags, width, height, depth, 0, 0, 0, 0); 
    
    if (!GAL_ReallocFormat (surface, depth, real_vformat.Rmask, real_vformat.Gmask, 
                        real_vformat.Bmask, 0)) 
        return -1;


    if(video_mem_info.type == MLS_FB_TYPE_VIDEOMEM) {
        pitch = video_mem_info.video_pitch;
        fb_size = pitch * height;
        shm_size = sizeof(MLShadowFBHeader);

        if(depth <= 8){
            shm_size += PALETTE_SIZE;
        }

        /* houhh 20071116, if video_mem is not enougth, alloc slave surface from video memory. */
        if (fb_size < (video_mem_info.video_mem_len - video_mem_info.video_mem_offset)) {
            reply->shm_id_header = shmget(0, shm_size, IPC_CREAT | 0666 | IPC_EXCL);
            if (reply->shm_id_header == -1){
                perror("shm");
                return -1;
            }
            header = shmat(reply->shm_id_header, 0, 0);
            header->video_offset = video_mem_info.video_mem_offset;
            video_mem_info.video_mem_offset += fb_size;

            if(depth <= 8)
                header->palette_offset = sizeof(*header);
            header->pitch    = pitch;
            surface->pixels  = _mmaped_mem + header->video_offset;
            reply->ret_value = 0;
            header->type     = MLS_FB_TYPE_VIDEOMEM;
        }
        else{
            videomem_enough = FALSE;
        }
    }

    /* houhh 20071116, alloc slave surface from system memory. */
    if(video_mem_info.type == MLS_FB_TYPE_SYSTEMMEM || !videomem_enough) { 
        bmp_ComputePitch(depth, (unsigned int)width, (unsigned int* )&pitch, 1);
        fb_size = pitch * height;
        shm_size = fb_size + sizeof(MLShadowFBHeader);

        if(depth <= 8)
            shm_size += PALETTE_SIZE;

        reply->shm_id_header = shmget(0, shm_size, IPC_CREAT | 0666 | IPC_EXCL);
        if (reply->shm_id_header == -1){
            fprintf(stderr, "create fb shm size = %d\n", fb_size);
            perror("shm");
            return -1;
        }

        header = shmat (reply->shm_id_header, 0 ,0);
        header->pitch = pitch;

        if(depth <= 8)
            header->palette_offset = sizeof(*header) + fb_size;

        header->type = MLS_FB_TYPE_SYSTEMMEM;
        reply->ret_value = 0;
    }

    header->depth       = depth;
    header->width       = width;
    header->height      = height;
    header->surface_key = (unsigned int)surface;
    surface->pixels     = (char*)header + sizeof(*header);
    surface->hwdata     = (struct private_hwdata*)header;                

    node = malloc(sizeof(*node));
    memset(node, 0 ,sizeof(*node));
    header->semid      = semget(0, 1, IPC_CREAT | 0666 | IPC_EXCL);
    reply->surface_key = (unsigned int)surface; 
    node->surface      = surface;
    node->enabled      = FALSE; 
    node->shmid        = reply->shm_id_header;
    node->rect.left    = 0;
    node->rect.top     = 0;
    node->rect.right   = width;
    node->rect.bottom  = height;
    
    SetRect (&header->dirty_rect, 0, 0, 0, 0);

    list_add_tail(&node->node, &mlshadow_list);
    return 0;
}

BOOL GUIAPI mlsGetSlaveScreenInfo (HDC dc_mls, DWORD mask, int* offset_x, int* offset_y, 
                DWORD blend_flags, gal_pixel* color_key, int* alpha, int* z_order)
{
    BOOL flag = FALSE;
    MLSHADOW_REPLY_SLAVE_GETINFO reply;
    MLSHADOW_REQ_SLAVE_GETINFO request;
    GAL_Surface *surface;
    MLShadowFBHeader *header;
    GAL_VideoDevice * this;

    this = (GAL_VideoDevice *)((dc_HDC2PDC(dc_mls))->surface->video);
    /* houhh 20071116, check if mlshadow. */
    MLSHADOW_CHECK(this);

    memset(&request, 0, sizeof(MLSHADOW_REQ_SLAVE_GETINFO));
    memset(&reply, 0, sizeof(MLSHADOW_REPLY_SLAVE_GETINFO));
    surface = (GAL_Surface *)((dc_HDC2PDC(dc_mls))->surface);
    header = (MLShadowFBHeader*)surface->hwdata;

    if(!header) return FALSE;

    request.op_id = MLSOP_ID_GET_SLAVEINFO;
    request.surface_key = header->surface_key;

    if(mgIsServer){
        MLSHADOW_GetSurface(&request, &reply);
    }
    else{
        REQUEST req;
        memset(&req, 0, sizeof(REQUEST));
        req.len_data = sizeof(request);
        req.data = &request;
        req.id = REQID_MLSHADOW_CLIREQ;
        if(ClientRequest((void*)&req, &reply, sizeof(reply)) < 0) {
            fprintf(stderr, "NEWGAL>MLSHADOW: Request server create FB error\n");
            return flag;
        }

        if (mask & MLS_INFOMASK_OFFSET) {
            if(offset_x && offset_y){
                *offset_x = reply.offset_x;
                *offset_y = reply.offset_y;
                flag = TRUE;
            }
        }
        if (mask & MLS_INFOMASK_BLEND) {
            if ((blend_flags & MLS_BLENDMODE_COLORKEY) == MLS_BLENDMODE_COLORKEY) {
                if(color_key){
                    *color_key = reply.color_key;
                    flag = TRUE;
                }
            }
            if ((blend_flags & MLS_BLENDMODE_ALPHA) == MLS_BLENDMODE_ALPHA) {
                if(alpha){
                    *alpha = reply.alpha;
                    flag = TRUE;
                }
            }
        }
        if (mask & MLS_INFOMASK_ZORDER){
            if(z_order){
                *z_order = reply.z_order;
                flag = TRUE;
            }
        }
    }
    return TRUE;
}

static int MLSHADOW_GetSurface(MLSHADOW_REQ_SLAVE_GETINFO* request, 
                MLSHADOW_REPLY_SLAVE_GETINFO* reply) 
{
    mlshadow_node* node = NULL;
    list_t* pos, *back;
    int index = 0;
    BOOL flag = FALSE;

    reply->ret_value = -1;
    
    list_for_each_safe(pos, back, &mlshadow_list){
        node = (mlshadow_node*)pos;
        index++;
        if (node->surface == (GAL_Surface*)(request->surface_key)) {
            flag = TRUE;
            break;
        }
    }

    //if((node = get_node((GAL_Surface*)(request->surface_key)))==NULL)
    //    return -1;
    if(!flag)
        return -1;

    reply->offset_x = node->rect.left;
    reply->offset_y = node->rect.top;
    reply->enable = node->enabled;
    reply->color_key = node->color_key;
    reply->alpha = node->alpha;
    reply->z_order = index;
    reply->ret_value = 0;

    return 0;
}

static int MLSHADOW_SetSurface(MLSHADOW_REQ_SLAVE_SETINFO *request, BOOL *reply) 
{
    GAL_Surface* surface;
    mlshadow_node * node;
    MLShadowFBHeader *header;
    GAL_Rect rect = {0};
    GAL_VideoDevice * this;

    if((node = get_node((GAL_Surface*)(request->surface_key)))==NULL)
        return -1;

    surface = node->surface;
    header = (MLShadowFBHeader*)surface->hwdata;
    this = (GAL_VideoDevice *)(surface->video);

    if (request->mask & MLS_INFOMASK_OFFSET) {
        if(node->rect.left != request->offset_x || node->rect.top != request->offset_y){
            header->offset_x  = request->offset_x;
            header->offset_y  = request->offset_y;
            rect.x = node->rect.left; rect.y = node->rect.top;
            rect.w = RECTW(node->rect);
            rect.h = RECTH(node->rect);
            /* bugfixed houhh 20071116, should update the surface's old rect 
             * by update the master surface. before update surface, should be
             * set current surface's offset.
             */
            node->rect.left   = request->offset_x;
            node->rect.top    = request->offset_y;
            node->rect.right  = request->offset_x + surface->w;
            node->rect.bottom = request->offset_y + surface->h;  
            MLSHADOW_UpdateSurfaceRects(this, dc_HDC2PDC(HDC_SCREEN)->surface, 1, &rect);
        }
        *reply = TRUE;
    }
    if (request->mask & MLS_INFOMASK_BLEND) {
        if (request->blend_flags == MLS_BLENDMODE_COLORKEY) {
            GAL_SetColorKey (surface, GAL_SRCCOLORKEY, request->color_key); 
            node->blend_flags |=  MLS_BLENDMODE_COLORKEY;
            *reply = TRUE;
        }

        if (request->blend_flags == MLS_BLENDMODE_ALPHA) {
            GAL_SetAlpha (surface, GAL_SRCALPHA, request->alpha);
            node->blend_flags |= MLS_BLENDMODE_ALPHA;
            *reply = TRUE;
        }
    }

    if (request->mask & MLS_INFOMASK_ZORDER){
        list_del((list_t*)node);
        list_add_by_index((list_t*)node, &mlshadow_list, request->z_order);
        *reply = TRUE;
    }

    if (request->mask & MLS_INFOMASK_ENABLE){
        node->enabled = request->enable;
        *reply = TRUE;
    }
    if(*reply) {
        header->dirty_rect = node->rect;
        header->dirty = TRUE;
    }
    return 0;
}

/************************** SERVER FUNCTION *************************/
int MLSHADOW_GetMasterSurface(MLSHADOW_REPLY_MASTER_INFO *reply)
{
    mlshadow_node *master_node;
  
    if ((master_node = get_master_node()) == NULL) {
        reply->ret_value = -1;
        return -1;
    }
    reply->type = video_mem_info.type;
    memcpy(reply->video_dev_name, video_mem_info.video_dev_name, DEV_NAME_LEN+1);
    reply->video_mem_len = video_mem_info.video_mem_len;
    reply->video_mem_offset = video_mem_info.video_mem_offset;
    reply->shm_id_header = master_node->shmid;
    reply->ret_value = 0;
    return 0;
}

int MLSHADOW_Server(void* request, void* reply)
{
    int *op_id;
    // bugfixed houhh 20071116, if mlshadow client's MiniGUI.cfg gal_engine=mlshadow.
    // but Server is not used this engine, return immediate.
    if (!mgIsServer || mlshadow_list.next == NULL) {
        return -1;
    }
    op_id = request;
    switch(*op_id){
        case MLSOP_ID_GET_MASTERINFO :
            MLSHADOW_GetMasterSurface(reply);
            break;
        case MLSOP_ID_CREATE_SURFACE : 
            MLSHADOW_CreateFrameBuffer(request, reply);
            break;
        case MLSOP_ID_SET_SLAVEINFO :
            MLSHADOW_SetSurface(request, reply);
            break;
        case MLSOP_ID_GET_SLAVEINFO :
            MLSHADOW_GetSurface(request, reply);
            break;
        case MLSOP_ID_DESTROY_SLAVE : 
            MLSHADOW_DeleteMLSurface(request, reply);
            break;
        default : return -1;
    }

    return 0; 
}

static mlshadow_node* get_node(GAL_Surface* surface)
{
    mlshadow_node* node = NULL;
    list_t* pos, *back;
    /* Fixed, houhh 20071116, check here is need for safe.*/
    if(!mgIsServer)
        return NULL;
    list_for_each_safe(pos, back, &mlshadow_list){
        node = (mlshadow_node*)pos;
        if (node->surface == surface) {
            return node;
        }
    }
    return NULL;
}

static mlshadow_node* get_master_node(void)
{
    mlshadow_node* pnode = NULL;
    list_t* pos, *back;
    PDC pdc = NULL;
    MLShadowFBHeader *header;

    list_for_each_safe (pos, back, &mlshadow_list) {
        pdc = dc_HDC2PDC (HDC_SCREEN);
        
        if (pdc == NULL || pdc->surface == NULL || pdc->surface->hwdata == NULL){
            fprintf(stderr, "Can't get master node\n");
            return NULL;
        }

        header = (MLShadowFBHeader*) pdc->surface->hwdata;
        
        pnode = (mlshadow_node*)pos;
        if (pnode->surface == (GAL_Surface*)header->surface_key) {
            if(pnode->surface->video == NULL)
                pnode->surface->video = pdc->surface->video;
            return pnode;
        }
    }
    return NULL;
}

void refresh_fb(RECT *dirty_rect, _THIS)
{
    mlshadow_node* node        = NULL;
    mlshadow_node* master_node = NULL;
    mlshadow_node* slave_node  = NULL;
    MLShadowFBHeader *header   = NULL;
    
    list_t* me; 

    GAL_Rect src = {0};
    GAL_Rect dst = {0};
    GAL_Rect GAL_dirty_rect;

    GAL_dirty_rect.x = dirty_rect->left;
    GAL_dirty_rect.y = dirty_rect->top;
    GAL_dirty_rect.w = RECTWP(dirty_rect);
    GAL_dirty_rect.h = RECTHP(dirty_rect);

    RECT intersect_rect;
    node = node; 
    master_node = get_master_node();
    if(master_node == NULL)
        return ;

    if (master_node && (master_node->blend_flags & MLS_BLENDMODE_ALPHA)) {    
        GAL_FillRect (this->hidden->_real_surface, 
                &GAL_dirty_rect, this->hidden->_real_def_bgcolor);
    }

    list_for_each (me, &mlshadow_list){
        slave_node = list_entry (me, mlshadow_node, node); 
        if(slave_node->enabled ){
            header = (MLShadowFBHeader*)(slave_node->surface->hwdata);

            if(IntersectRect (&intersect_rect, &slave_node->rect, dirty_rect)){

                dst.x = intersect_rect.left;
                dst.y = intersect_rect.top;          
                dst.w = intersect_rect.right - intersect_rect.left;
                dst.h = intersect_rect.bottom - intersect_rect.top;                                               

                src.x = dst.x - slave_node->rect.left;
                src.y = dst.y - slave_node->rect.top;
                src.w = dst.w;
                src.h = dst.h;
                //fprintf(stderr, "(%d,%d):l=%d,t=%d,w=%d,h=%d\n", slave_node->rect.left, slave_node->rect.top,
                //        dst.x, dst.y, dst.w, dst.h);
                if (this->hidden->swap_surface != NULL){
                    GAL_BlitSurface (slave_node->surface, &src, 
                                    this->hidden->swap_surface, &dst);
                }
                else{
                    GAL_BlitSurface (slave_node->surface, &src, 
                                    this->hidden->_real_surface, &dst);
                }
            }
        }
    }

    if (this->hidden->swap_surface != NULL){
        GAL_BlitSurface (this->hidden->swap_surface, &GAL_dirty_rect, 
                        this->hidden->_real_surface, &GAL_dirty_rect);
    }

    if(this->hidden->_real_device->UpdateRects){
        this->hidden->_real_device->UpdateRects
                        (this->hidden->_real_device, 1, &GAL_dirty_rect);
    }
}

/* houhh 20071115.
 * when mlshadow client change surface's palette, the relative
 * surface's palette_offset data is changed synchronous. 
 * but mlshadow server check and update surface's palette in the
 * task_do_update, it get palette data from MLShadowFBHeader's palette_offset 
 * note: 1.if you have enable the double_surface, update palette when have.
 *       2.if anyone mlshadow client have changed the master surface's palette
 *         the real_surface's palette is changed for all client.
 */
static void palette_update_check(MLShadowFBHeader *header, mlshadow_node* pnode)
{
    if (header && header->palette_changed) {
        char *dst_colors;
        char *src_colors;
        GAL_VideoDevice* this;
        mlshadow_node* pmaster_node = NULL;

        // update the server's surface palette.
        // get palette data from the share palette...
        dst_colors = (char*)pnode->surface->format->palette->colors;
        src_colors = (char*)header + header->palette_offset;
        memcpy(dst_colors, src_colors, PALETTE_SIZE);

        //  if the surface is master surface, set master surface's palette 
        //  and _real_device's palette... 
        //  include swap_surface/real_surface palette...
        pmaster_node = get_master_node();
        if(pmaster_node && pmaster_node->surface == pnode->surface){
           this = pmaster_node->surface->video; 
           if(this->SetColors)
               this->SetColors(this, 0, 256, (GAL_Color*)dst_colors); 
        }

        header->palette_changed = FALSE;
        header->dirty           = TRUE;
        header->dirty_rect      = pnode->rect;
    }
}

static void* task_do_update (_THIS)
{
    mlshadow_node* slave_node = NULL;
    list_t* me, *back; 
    MLShadowFBHeader *header = NULL;

    while (1) {
        __mg_os_time_delay (20);
        if(!list_empty (&mlshadow_list)) {
            list_for_each_safe(me, back, &mlshadow_list){
                slave_node = (mlshadow_node*)me;
                /* bugfixed houhh 20071116, if mlsEnableScreenInfo disable surface, surface can not update.
                 * it should let other surface do update in current dirty_rect.
                 */ 
                //if(slave_node->enabled)
                {
                    header = (MLShadowFBHeader*)(slave_node->surface->hwdata);
                    palette_update_check(header, slave_node);
                    if (header->dirty == TRUE) {
                        RECT dirty_rect = header->dirty_rect;
                        lock_fbh(header);
                        SetRect (&header->dirty_rect, 0, 0, 0, 0);
                        header->dirty = FALSE;
                        unlock_fbh(header);
                        refresh_fb(&dirty_rect, this);
                    }
                }
            }
        }
    }
    return NULL;
}

void MLSHADOW_Del_List(void)
{
    mlshadow_node* node = NULL;
    list_t* pos, *back;
    GAL_Surface *surface;
    MLShadowFBHeader *header;

    list_for_each_safe(pos, back, &mlshadow_list){
        node = (mlshadow_node*)pos;
        surface = node->surface;
        header = (MLShadowFBHeader*)(surface->hwdata);
        semctl(header->semid, 1, IPC_RMID);
        if (header->type == MLS_FB_TYPE_SYSTEMMEM) {
            shmdt(header);
            if(shmctl(node->shmid, IPC_RMID, NULL) != 0){
                perror("shmctl");
            }
        }
        node->surface->pixels = NULL;
        Slave_FreeSurface (node->surface);   
        list_del(pos);
    }
}

static void MLSHADOW_DeleteDevice (GAL_VideoDevice *device)
{
    if(mgIsServer){
        if(device && device->hidden){
            /* check the real device if created success...*/
            if(device->hidden->_real_device){ 
                device->hidden->_real_device->VideoQuit(device->hidden->_real_device);

                GAL_FreeSurface (device->hidden->_real_surface);
                if(device->hidden->swap_surface)
                    GAL_FreeSurface (device->hidden->swap_surface);
                device->hidden->_real_device->free(device->hidden->_real_device);

                if (device->hidden->_real_device->screen != NULL) {
                    device->hidden->_real_device->screen = NULL;
                }
            }

            if(device->hidden){
                free (device->hidden);
            }
        }
    }
    /* mlshadow client only need to free device only */
    if(device)
        free (device);
}

void srvMLSHADOW_DelSurface(void *res)
{
        MLSHADOW_REQ_SLAVE_DESTROY request;
        BOOL reply;

        request.surface_key = (int)res;
        request.op_id = MLSOP_ID_DESTROY_SLAVE;

        MLSHADOW_DeleteMLSurface(&request, &reply);
}

static void MLSHADOW_DeleteSurface(_THIS, GAL_Surface* surface)
{
    MLSHADOW_REQ_SLAVE_DESTROY request;
    BOOL reply;
    MLShadowFBHeader *header;

    header = (MLShadowFBHeader*)surface->hwdata;
    request.surface_key = header->surface_key;
    request.op_id = MLSOP_ID_DESTROY_SLAVE;

    if(mgIsServer) {
        MLSHADOW_DeleteMLSurface(&request, &reply);
    }else{
        REQUEST req;
        memset(&req, 0, sizeof(REQUEST));
        req.len_data = sizeof(request);
        req.data = &request;
        req.id = REQID_MLSHADOW_CLIREQ;
        surface->pixels = NULL;
        
        if (shmdt (surface->hwdata) == -1){
            fprintf(stderr, "shmdt failed.\n");
        }

        ClientRequest(&req, &reply, sizeof(reply));
    }
}

static void MLSHADOW_DeleteMLSurface (MLSHADOW_REQ_SLAVE_DESTROY *request, BOOL *reply)
{
    GAL_Surface* surface_key;
    mlshadow_node* node = NULL;
    list_t* pos, *back;
    MLShadowFBHeader *header;
    RECT deleted_rect;
    GAL_Rect rect;
    GAL_VideoDevice* this;

    surface_key = (GAL_Surface*)request->surface_key;
    *reply = FALSE;    

    if (surface_key == NULL) {
        return;
    }
    this = (GAL_VideoDevice *)(surface_key->video);
    list_for_each_safe(pos, back, &mlshadow_list){
        node = (mlshadow_node*)pos;
        if(surface_key == node->surface){
            header = (MLShadowFBHeader*)(surface_key->hwdata);
            deleted_rect = node->rect;
            semctl(header->semid, 1, IPC_RMID);
            
            /* bugfixed houhh 20071116, before detach share memory:
             * note: 1.delete surface from mlsahdow_list first, else
             *         task_do_update use the surface's share head will result error.
             * note: 2.must set surface->pixels = NULL.
             *         because the later GAL_FreeSurface() will free it again, this 
             *         result error!  
             */
            list_del(pos);
            semctl(header->semid, 1, IPC_RMID);
            shmdt(header);
            if(shmctl(node->shmid, IPC_RMID, NULL) != 0)
                perror("shmctl");

            /* houhh 20061116, free server's slave surface, client's slave 
             * surface is free in Slave_VideoQuit(). */
            node->surface->pixels = NULL;
            Slave_FreeSurface (node->surface);   
            *reply  = TRUE;

            /* bugfixed houhh 20071116, should update the surface's old rect 
             * by update the master surface.*/
            /* node    = get_master_node();
               refresh_fb(&deleted_rect, node->surface->video); 
             */
            rect.x = node->rect.left; rect.y = node->rect.top;
            rect.w = RECTW(node->rect);
            rect.h = RECTH(node->rect); 
            MLSHADOW_UpdateSurfaceRects(this, (dc_HDC2PDC(HDC_SCREEN))->surface, 1, &rect);
            /* bugfixed houhh 20071116, free slave surface node.*/
            free(pos);
        }
    }
    return ;
}

static void MLSHADOW_VideoQuit (_THIS)
{
    /* houhh 20071116, if user had delete slave surface, here will only
     * left master surface, else all the surface will free here.
     * note: master surface->pixels = NULL first, else GAL_FreeSurface() 
     *       free(surface->pixels) will error.
     * */
    if(GAL_VideoSurface)
        GAL_VideoSurface->pixels = NULL;
    if (mgIsServer) {
        MLSHADOW_Del_List();
    }
    /* houhh 20071116, munmap the video memory.*/
    if(video_mem_info.type == MLS_FB_TYPE_VIDEOMEM){
        munmap(_mmaped_mem, video_mem_info.video_mem_len);
    }
}

BOOL mlsEnableSlaveScreen (HDC dc_mls, BOOL enable)
{
    MLSHADOW_REQ_SLAVE_SETINFO request;
    BOOL reply;
    MLShadowFBHeader *header;
    GAL_VideoDevice * this;

    this = (GAL_VideoDevice *)((dc_HDC2PDC(dc_mls))->surface->video);
    /* houhh 20071116, check if mlshadow. */
    MLSHADOW_CHECK(this);

    memset(&request, 0, sizeof(MLSHADOW_REQ_SLAVE_SETINFO));
    reply = 0;

    header = (MLShadowFBHeader*)(((GAL_Surface *)((dc_HDC2PDC(dc_mls))->surface)->hwdata));
    if(!header) return FALSE;

    request.op_id = MLSOP_ID_SET_SLAVEINFO;
    request.surface_key = (unsigned int)header->surface_key;
    request.mask = MLS_INFOMASK_ENABLE;
    request.enable = enable;

    if(mgIsServer){
        MLSHADOW_SetSurface(&request, &reply);
    }
    else{
        REQUEST req;
        memset(&req, 0, sizeof(REQUEST));
        req.len_data = sizeof(request);
        req.data = &request;
        req.id = REQID_MLSHADOW_CLIREQ;

        if(ClientRequest(&req, &reply, sizeof(reply)) < 0){
            return FALSE;
        }
    }
    return TRUE;
}

BOOL GUIAPI mlsSetSlaveScreenInfo (HDC dc_mls, DWORD mask, 
        int offset_x, int offset_y, DWORD blend_flags, 
        gal_pixel color_key, int alpha, int z_order)
{
    MLSHADOW_REQ_SLAVE_SETINFO request;
    BOOL reply;
    GAL_Surface *surface;
    MLShadowFBHeader *header;
    GAL_VideoDevice * this;

    this = (GAL_VideoDevice *)((dc_HDC2PDC(dc_mls))->surface->video);
    /* houhh 20071116, check if mlshadow. */
    MLSHADOW_CHECK(this);

    memset(&request, 0, sizeof(MLSHADOW_REQ_SLAVE_SETINFO));
    reply = 0;
    surface = (GAL_Surface *)((dc_HDC2PDC(dc_mls))->surface);
    header = (MLShadowFBHeader*)surface->hwdata;

    if(!header) return FALSE;

    request.op_id = MLSOP_ID_SET_SLAVEINFO;
    request.surface_key = header->surface_key;
    request.mask = mask;
    request.offset_x = offset_x;
    request.offset_y = offset_y;
    request.blend_flags = blend_flags;
    request.z_order = z_order; 
    request.color_key = color_key;
    request.alpha = alpha;

    if(mgIsServer){
        MLSHADOW_SetSurface(&request, &reply);
    }
    else{
        REQUEST req;
        memset(&req, 0, sizeof(REQUEST));
        req.len_data = sizeof(request);
        req.data = &request;
        req.id = REQID_MLSHADOW_CLIREQ;
        if(ClientRequest((void*)&req, &reply, sizeof(reply)) < 0) {
            return FALSE;
        }
    }
    return TRUE;
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

#if 0 
void mlshadow_show_list (void)
{
    list_t * pos;
    list_t * back;
    mlshadow_node * node;

    list_for_each_safe(pos, back, &_mlshadowshmheader->_mlshadow_list) {
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

#endif /* _MGGAL_MLSHADOW _MGRM_PROCESSES */

