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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

#ifdef _MGGAL_XLIB

#ifdef __ECOS__           /* ECOS */
/* for eCos Linux Synthetic target */
#include <cyg/hal/hal_io.h>

typedef int key_t;

#define shmget cyg_hal_sys_shmget
#define shmat cyg_hal_sys_shmat
#define shmdt cyg_hal_sys_shmdt
#define ftok cyg_hal_sys_ftok

#else                    /* not ECOS */

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "minigui.h"
#include "misc.h"
#include "newgal.h"
#include "sysvideo.h"
#include "xlibvideo.h"

#endif                  /* end ECOS */  

#define WINDOW_CAPTION_LEN 255
#define EXECL_FILENAME_LEN 255
#define LEN_MODE           255

/* Initialization/Query functions */
static int XXVFB_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **XXVFB_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *XXVFB_SetVideoMode(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int XXVFB_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void XXVFB_VideoQuit(_THIS);

/* Hardware surface functions */
static int XXVFB_AllocHWSurface(_THIS, GAL_Surface *surface);
static void XXVFB_FreeHWSurface(_THIS, GAL_Surface *surface);

#ifndef WIN32
static int semid;

static int shm_init_lock(key_t key)
{
    union semun {
        int val;
        struct semid_ds* buf;
        unsigned short* array;
    } sem;
    
    semid = semget(key,1,IPC_CREAT|0666);
    if(semid==-1){
        fprintf(stderr,"create semaphoreerror\n");
        exit(-1);
    }
    sem.val=1;
    semctl(semid,0,SETVAL,sem);
    
    return 0;
}

static int shm_lock(int semid){
    struct sembuf sops={0, -1, SEM_UNDO};
    return(semop(semid,&sops,1));
}

static int shm_unlock(int semid){
    struct sembuf sops = {0, +1, SEM_UNDO};
    return(semop(semid,&sops,1));
}

static int execl_xxvfb(void)
{
    char execl_file[EXECL_FILENAME_LEN + 1];
    char mode [LEN_MODE + 1];
    char ch_pid[10];
    int i;
#if defined (WIN32) || !defined(__NOUNIX__)
	char* env_value;
#endif

    if (GetMgEtcValue ("xxvfb", "exec_file", 
                execl_file, EXECL_FILENAME_LEN) < 0)
        return ERR_CONFIG_FILE;
    execl_file[EXECL_FILENAME_LEN] = '\0';

#if defined (WIN32) || !defined(__NOUNIX__)
    if ((env_value = getenv ("MG_DEFAULTMODE"))) {
        strncpy (mode, env_value, LEN_MODE);
        mode [LEN_MODE] = '\0';
    }
    else
#endif
    if (GetMgEtcValue ("xxvfb", "defaultmode", mode, LEN_MODE) < 0)
        if (GetMgEtcValue ("system", "defaultmode", mode, LEN_MODE) < 0)
            return ERR_CONFIG_FILE;

    mode[LEN_MODE] = '\0';

    for(i=0;i<LEN_MODE;i++)
    {
        mode[i]=tolower(mode[i]);
    }

    sprintf(ch_pid, "%d", getppid());

	fprintf(stderr,"start-xxvfb: %s xxvfb %s %s\n", execl_file, ch_pid, mode);
    if (execl(execl_file, execl_file, "xxvfb", ch_pid, mode, NULL) < 0)
        fprintf(stderr, "execl error!!\n");

    return 0;
}
int __mg_xxvfb_server_sockfd;
int __mg_xxvfb_client_sockfd;
#endif

/* XXVFB driver bootstrap functions */
static int XXVFB_Available (void)
{
    return 1;
}

static void XXVFB_DeleteDevice (GAL_VideoDevice *device)
{
    free (device->hidden);
    free (device);
}

static void XXVFB_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    int i;
    RECT bound;

    shm_lock(semid);

    bound.left   = this->hidden->hdr->dirty_rc_l;
    bound.top    = this->hidden->hdr->dirty_rc_t;
    bound.right  = this->hidden->hdr->dirty_rc_r;
    bound.bottom = this->hidden->hdr->dirty_rc_b;

    if (bound.right == -1) bound.right = 0;
    if (bound.bottom == -1) bound.bottom = 0;

    for (i = 0; i < numrects; i++) {
        RECT rc;

        SetRect (&rc, rects[i].x, rects[i].y, 
                rects[i].x + rects[i].w, rects[i].y + rects[i].h);
        if (IsRectEmpty (&bound))
            bound = rc;
        else
            GetBoundRect (&bound, &bound, &rc);
    }

    this->hidden->hdr->dirty_rc_l = bound.left;
    this->hidden->hdr->dirty_rc_t = bound.top;
    this->hidden->hdr->dirty_rc_r = bound.right;
    this->hidden->hdr->dirty_rc_b = bound.bottom;

    this->hidden->hdr->dirty = TRUE;

#if 0
    fprintf(stderr,"%d,%d,%d,%d\n",this->hidden->hdr->dirty_rc_l,this->hidden->hdr->dirty_rc_t,
            this->hidden->hdr->dirty_rc_r,this->hidden->hdr->dirty_rc_b);
#endif

    shm_unlock(semid);
}

static GAL_VideoDevice *XXVFB_CreateDevice (int devindex)
{
    GAL_VideoDevice *this;

    /* Initialize all variables that we clean on shutdown */
    this = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
    if (this) {
        memset (this, 0, (sizeof *this));
        this->hidden 
            = (struct GAL_PrivateVideoData *) malloc ((sizeof *this->hidden));
    }

    if ((this == NULL) || (this->hidden == NULL)) {
        GAL_OutOfMemory ();
        if (this) free (this);
        return (0);
    }
    memset (this->hidden, 0, (sizeof *this->hidden));

    /* Set the function pointers */
    this->VideoInit = XXVFB_VideoInit;
    this->ListModes = XXVFB_ListModes;
    this->SetVideoMode = XXVFB_SetVideoMode;
    this->SetColors = XXVFB_SetColors;
    this->VideoQuit = XXVFB_VideoQuit;

    this->AllocHWSurface = XXVFB_AllocHWSurface;
    this->FreeHWSurface = XXVFB_FreeHWSurface;

    this->UpdateRects = XXVFB_UpdateRects;

    this->CheckHWBlit = NULL;
    this->FillHWRect = NULL;
    this->SetHWColorKey = NULL;
    this->SetHWAlpha = NULL;

    this->free = XXVFB_DeleteDevice;

    return this;
}

VideoBootStrap XXVFB_bootstrap = {
    "xxvfb", "Xlib Virtual FrameBuffer",
    XXVFB_Available, XXVFB_CreateDevice
};

static int XXVFB_VideoInit (_THIS, GAL_PixelFormat *vformat)
{
    struct GAL_PrivateVideoData* data = this->hidden;
    int    display;
    pid_t  pid;

    int    server_len;
    socklen_t client_len;
    struct sockaddr_un server_address;
    struct sockaddr_un client_address;
    char   socket_file [50];

    int    shmid;
#ifndef _MGRM_THREADS
    FILE  *fp;
#endif

#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE)
    if (mgIsServer) {
#endif
        __mg_xxvfb_server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
        server_address.sun_family = AF_UNIX;
        sprintf(socket_file, "/tmp/xxvfb_socket%d", getpid());
        socket_file[49] = '\0';
        unlink(socket_file);
        strcpy(server_address.sun_path, socket_file);

        //FIXME
        server_len = sizeof(server_address);
        bind(__mg_xxvfb_server_sockfd, 
                (struct sockaddr *)&server_address, server_len);
        listen(__mg_xxvfb_server_sockfd, 5);
        client_len = sizeof(client_address);
        
        shm_init_lock(getpid());

        if ((pid = fork()) < 0) {
            fprintf(stderr, "fork() error in xxvfb\n");
        }else if (pid > 0) {
            ;/* do nothing */
        }else {

            if (execl_xxvfb() == ERR_CONFIG_FILE)
                fprintf(stderr, "XXVFB GAL: Reading configuration failure!\n");

            perror ("execl");
            _exit (1);
        }

        if (GetMgEtcIntValue ("xxvfb", "display", &display) < 0)
            display = 0;

        __mg_xxvfb_client_sockfd = 
                accept (__mg_xxvfb_server_sockfd, 
                    (struct sockaddr *)&client_address, &client_len);

        read(__mg_xxvfb_client_sockfd, &shmid, sizeof(int));

        if (shmid != -1) {
            data->shmrgn = (unsigned char *)shmat (shmid, 0, 0);
#ifndef _MGRM_THREADS
            fp = fopen("/tmp/.xxvfb_tmp", "w+");
            if (fp == NULL) {
                fprintf(stderr, "mgServer can't open file /tmp/.xxvfb_tmp\n");
                return -1;
            }
            fwrite(&shmid, sizeof(int), 1, fp);
            fclose(fp);
#endif
        }

#if defined(_MGRM_PROCESSES) && !defined(_MGRM_STANDALONE)
    } else {
        fp = fopen("/tmp/.xxvfb_tmp", "r");
        if (fp == NULL) {
            fprintf(stderr, "minigui can't open file /tmp/.xxvfb_tmp\n");
            return -1;
        }
        fread(&shmid, sizeof(int), 1, fp);
        fclose(fp);

        if (shmid != -1) {
            data->shmrgn = (unsigned char *)shmat (shmid, 0, 0);
        }
    }
#endif


    if ((int)data->shmrgn == -1 || data->shmrgn == NULL) {
        GAL_SetError ("NEWGAL>XXVFB: Unable to attach to "
                "virtual FrameBuffer server.\n");
        return -1;
    }
    
    data->hdr = (XXVFBHeader *) data->shmrgn;
    vformat->BitsPerPixel = data->hdr->depth;
    switch (vformat->BitsPerPixel) {
#ifdef _MGGAL_SHADOW
        case 1:
            break;
        case 2:
            break;
        case 4:
            break;
#endif
        case 8:
            vformat->BytesPerPixel = 1;
            break;
        case 12:
            vformat->BitsPerPixel = 16;
            vformat->BytesPerPixel = 2;
            vformat->Rmask = 0x00000F00;
            vformat->Gmask = 0x000000F0;
            vformat->Bmask = 0x0000000F;
            break;
        case 16:
            vformat->BytesPerPixel = 2;
            vformat->Amask = data->hdr->Amask;
            vformat->Rmask = data->hdr->Rmask;
            vformat->Gmask = data->hdr->Gmask;
            vformat->Bmask = data->hdr->Bmask;
            break;
        case 24:
        case 32:
            vformat->BytesPerPixel = 4;
            vformat->Amask = data->hdr->Amask;
            vformat->Rmask = data->hdr->Rmask;
            vformat->Gmask = data->hdr->Gmask;
            vformat->Bmask = data->hdr->Bmask;
            break;
        default:
            GAL_SetError ("NEWGAL>XXQVFB: Not supported depth: %d, " 
                    "please try to use Shadow NEWGAL engine with targetname qvfb.\n",
                    vformat->BitsPerPixel);
            return -1;
    }

    return 0;
}

static GAL_Rect **XXVFB_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    return (GAL_Rect **) -1;
}

static GAL_Surface *XXVFB_SetVideoMode (_THIS, GAL_Surface *current,
        int width, int height, int bpp, Uint32 flags)
{
    /* Set up the mode framebuffer */
    current->flags  = GAL_HWSURFACE | GAL_FULLSCREEN;
    current->w      = this->hidden->hdr->width;
    current->h      = this->hidden->hdr->height;
    current->pitch  = this->hidden->hdr->pitch;
    current->pixels = this->hidden->shmrgn + this->hidden->hdr->fb_offset;
    current->format->MSBLeft = this->hidden->hdr->MSBLeft;

    /* We're done */
    return current;
}

static int XXVFB_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    return -1;
}

static void XXVFB_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

static int XXVFB_SetColors(_THIS, int firstcolor, 
        int ncolors, GAL_Color *colors)
{
#if 0
    int i;
    int pixel = firstcolor;
    XVFBPalEntry *palette;

    palette = (XVFBPalEntry *)((BYTE *)this->hidden->hdr 
            + this->hidden->hdr->palette_offset);

    for (i = 0; i < ncolors; i++) {
        palette[pixel].r = colors[i].r & 0xff;
        palette[pixel].g = colors[i].g & 0xff;
        palette[pixel].b = colors[i].b & 0xff;
        palette[pixel].a = colors[i].a & 0xff;

        pixel++;
    }

    this->hidden->hdr->palette_changed = 1;
#endif

    return 1;
}

static void XXVFB_VideoQuit (_THIS)
{
    shmdt (this->hidden->shmrgn);
    semctl(semid,0,IPC_RMID);
}

#endif /* _MGGAL_XXVFB*/
