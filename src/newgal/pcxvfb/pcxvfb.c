///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/**
 ** pcxvfb.c: PC X virtual FrameBuffer based video driver implementation.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

#ifdef _MGGAL_PCXVFB

#ifdef __ECOS__           /* ECOS */
/* for eCos Linux Synthetic target */
#include <cyg/hal/hal_io.h>

typedef int key_t;

#define shmget cyg_hal_sys_shmget
#define shmat cyg_hal_sys_shmat
#define shmdt cyg_hal_sys_shmdt
#define ftok cyg_hal_sys_ftok

#else                    /* not ECOS */

#ifndef WIN32            /* not WIN32 */
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>

#else                    /* win32 */

#include "pcxvfb-winfb.h"

#endif                   /* end win32 def*/

#include "minigui.h"
#include "misc.h"
#include "newgal.h"
#include "../sysvideo.h"
#include "../shadow-screen.h"
#include "pcxvfb.h"

#endif                  /* end ECOS */

#define WINDOW_CAPTION_LEN 255
#define EXECL_FILENAME_LEN 255
#define LEN_MODE           255

/* Initialization/Query functions */
static int PCXVFB_VideoInit(_THIS, GAL_PixelFormat *vformat);

static GAL_Rect **PCXVFB_ListModes(_THIS,
        GAL_PixelFormat *format, Uint32 flags);

static GAL_Surface *PCXVFB_SetVideoMode(_THIS, GAL_Surface *current,
        int width, int height, int bpp, Uint32 flags);

static int PCXVFB_SetColors(_THIS, int firstcolor, int ncolors,
        GAL_Color *colors);

static void PCXVFB_VideoQuit(_THIS);

/* Hardware surface functions */
static int PCXVFB_AllocHWSurface (_THIS, GAL_Surface *surface);
static void PCXVFB_FreeHWSurface (_THIS, GAL_Surface *surface);


#ifndef WIN32
static int semid;

static int shm_init_lock (key_t key)
{
    union semun {
        int val;
        struct semid_ds* buf;
        unsigned short* array;
    } sem;

    semid = semget(key, 1, IPC_CREAT|0666);
    if (semid == -1) {
        _ERR_PRINTF ("NEWGAL>PCXVFB: Failed to create semaphore.\n");
        exit(-1);
    }
    sem.val = 1;
    semctl (semid, 0, SETVAL, sem);

    return 0;
}

static void shm_lock (int semid)
{
    struct sembuf sops = {0, -1, SEM_UNDO};

try_again:
    if (semop (semid, &sops, 1) < 0) {
        if (errno == EINTR)
            goto try_again;
        else
            _WRN_PRINTF ("NEWGAL>PCXVFB: failed to lock sempahore id: %d; pid (%d): %s\n",
                    semid, getpid(), strerror (errno));
    }
}

static void shm_unlock (int semid)
{
    struct sembuf sops = {0, +1, SEM_UNDO};

try_again:
    if (semop (semid, &sops, 1)) {
        if (errno == EINTR)
            goto try_again;
        else
            _WRN_PRINTF ("NEWGAL>PCXVFB: failed to unlock sempahore id: %d; pid (%d): %s\n",
                    semid, getpid(), strerror (errno));
    }
}

static int execl_pcxvfb (void)
{
    char execl_file [EXECL_FILENAME_LEN + 1];
    char window_caption [WINDOW_CAPTION_LEN + 1];
    char mode [LEN_MODE + 1];
    char ch_pid [32];
    char skin [256];
    int i;
#if defined (WIN32) || !defined(__NOUNIX__)
    char* env_value;
#endif

    if (GetMgEtcValue ("pc_xvfb", "exec_file",
                execl_file, EXECL_FILENAME_LEN) < 0)
        return ERR_CONFIG_FILE;
    execl_file[EXECL_FILENAME_LEN] = '\0';

    if (GetMgEtcValue ("pc_xvfb", "window_caption",
                window_caption, WINDOW_CAPTION_LEN) < 0)
        return ERR_CONFIG_FILE;
    window_caption[WINDOW_CAPTION_LEN] = '\0';

#if defined (WIN32) || !defined(__NOUNIX__)
    if ((env_value = getenv ("MG_DEFAULTMODE"))) {
        strncpy (mode, env_value, LEN_MODE);
        mode [LEN_MODE] = '\0';
    }
    else
#endif
    if (GetMgEtcValue ("pc_xvfb", "defaultmode", mode, LEN_MODE) < 0)
        if (GetMgEtcValue ("system", "defaultmode", mode, LEN_MODE) < 0)
            return ERR_CONFIG_FILE;

    mode[LEN_MODE] = '\0';

    for (i = 0; i < LEN_MODE; i++) {
        mode[i] = tolower (mode[i]);
    }

    sprintf (ch_pid, "%d", getppid ());

    skin[0] = '\0';
    GetMgEtcValue ("pc_xvfb", "skin", skin, sizeof(skin)-1);

    _DBG_PRINTF ("NEWGAL>PCXVFB: %s %s %s %s %s\n",
            execl_file, ch_pid, window_caption, mode, skin);

    if (execlp (execl_file, "pcxvfb",
                ch_pid, window_caption, mode, skin, NULL) < 0) {
        _ERR_PRINTF ("NEWGAL>PCXVFB: failed to start the XVFB process!\n");
    }

    return 0;
}
#endif

/* PCXVFB driver bootstrap functions */
static int PCXVFB_Available (void)
{
#ifndef WIN32
    //return shmid != -1;
    return 1;
#else
    return win_PCXVFbAvailable ();
#endif
}

static void PCXVFB_DeleteDevice (GAL_VideoDevice *device)
{
    free (device->hidden);
    free (device);
}

#if 0   // deprecated code
static void PCXVFB_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
#ifdef WIN32
    win_PCXVFbLock ();
#else
    shm_lock(semid);
#endif

    shadowScreen_UpdateRects (this, numrects, rects);

#ifdef WIN32
    win_PCXVFbUnlock ();
#else
    shm_unlock(semid);
#endif
}
#endif  // deprecated code

static BOOL PCXVFB_SyncUpdate (_THIS)
{
    RECT dirty_rc;

    if (IsRectEmpty(&this->hidden->dirty_rc))
        return FALSE;

#ifdef WIN32
    win_PCXVFbLock();
#else
    shm_lock(semid);
#endif

    dirty_rc.left = this->hidden->hdr->dirty_rc_l;
    dirty_rc.top = this->hidden->hdr->dirty_rc_t;
    dirty_rc.right = this->hidden->hdr->dirty_rc_r;
    dirty_rc.bottom = this->hidden->hdr->dirty_rc_b;
    if (dirty_rc.right == -1) dirty_rc.right = 0;
    if (dirty_rc.bottom == -1) dirty_rc.bottom = 0;

    GetBoundRect(&this->hidden->dirty_rc, &dirty_rc,
            &this->hidden->dirty_rc);

    shadowScreen_BlitToReal(this, NULL);

    this->hidden->hdr->dirty_rc_l = this->hidden->dirty_rc.left;
    this->hidden->hdr->dirty_rc_t = this->hidden->dirty_rc.top;
    this->hidden->hdr->dirty_rc_r = this->hidden->dirty_rc.right;
    this->hidden->hdr->dirty_rc_b = this->hidden->dirty_rc.bottom;
    this->hidden->hdr->dirty = TRUE;

    SetRectEmpty(&this->hidden->dirty_rc);

#ifdef WIN32
    win_PCXVFbUnlock();
#else
    shm_unlock(semid);
#endif
    return TRUE;
}

static GAL_VideoDevice *PCXVFB_CreateDevice (int devindex)
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
    this->hidden->magic = MAGIC_SHADOW_SCREEN_HEADER;
    this->hidden->version = 0;

    /* Set the function pointers */
    this->VideoInit = PCXVFB_VideoInit;
    this->ListModes = PCXVFB_ListModes;
    this->SetVideoMode = PCXVFB_SetVideoMode;
    this->SetColors = PCXVFB_SetColors;
    this->VideoQuit = PCXVFB_VideoQuit;

    this->AllocHWSurface = PCXVFB_AllocHWSurface;
    this->FreeHWSurface = PCXVFB_FreeHWSurface;

#if IS_COMPOSITING_SCHEMA
    if (mgIsServer) {
        this->UpdateRects = shadowScreen_UpdateRects;
        this->SyncUpdate = PCXVFB_SyncUpdate;
    }
    else {
        this->UpdateRects = NULL;
        this->SyncUpdate = NULL;
    }
#else
    this->UpdateRects = shadowScreen_UpdateRects;
    this->SyncUpdate = PCXVFB_SyncUpdate;
#endif

    this->CheckHWBlit = NULL;
    this->FillHWRect = NULL;

    this->free = PCXVFB_DeleteDevice;

    return this;
}

VideoBootStrap PCXVFB_bootstrap = {
    "pc_xvfb", "PCX Virtual FrameBuffer",
    PCXVFB_Available, PCXVFB_CreateDevice
};

#ifndef WIN32
static int my_select(int nfds, fd_set *readfds, fd_set *writefds,
        fd_set *exceptfds, struct timeval *timeout) {
    struct timeval _tv, *tv;
    struct timeval start, now;
    int ret;

    if (timeout) {
        gettimeofday(&start, NULL);
        memcpy(&_tv, timeout, sizeof(_tv));
        tv = &_tv;
    }
    else {
        tv = NULL;
    }

    for (;;) {
        ret = select(nfds, readfds, writefds, exceptfds, tv);
        if (ret == -1 && errno == EINTR) {
            if (timeout) {
                gettimeofday(&now, NULL);
                ret = (timeout->tv_sec * 1000000 + timeout->tv_usec) -
                    ((now.tv_sec - start.tv_sec) * 1000000 +
                     (now.tv_usec - start.tv_usec));
                if (ret <= 0) {
                    return 0; /* Timeout */
                }
                else {
                    _tv.tv_sec = ret / 1000000;
                    _tv.tv_usec = ret % 1000000;
                    continue;
                }
            }
            else {
                continue;
            }
        }
        else {
            return ret;
        }
    }

    /* XXX: Never reach here */
    return -1;
}
#endif

static int PCXVFB_VideoInit (_THIS, GAL_PixelFormat *vformat)
{
    int i;
    char* env_value;
    char execl_file[EXECL_FILENAME_LEN + 1];
    char window_caption[WINDOW_CAPTION_LEN + 1];
    char mode [LEN_MODE + 1];
    struct GAL_PrivateVideoData* data = this->hidden;
#ifdef WIN32 //-----------win32----------------
    char etc_param[128];
    char skin[256];
#endif

    if (GetMgEtcValue ("pc_xvfb", "exec_file",
                execl_file, EXECL_FILENAME_LEN) < 0)
        return ERR_CONFIG_FILE;
    execl_file[EXECL_FILENAME_LEN] = '\0';

    if (GetMgEtcValue ("pc_xvfb", "window_caption",
                window_caption, WINDOW_CAPTION_LEN) < 0)
        return ERR_CONFIG_FILE;
    window_caption[WINDOW_CAPTION_LEN] = '\0';

#if defined (WIN32) || !defined(__NOUNIX__)
    if ((env_value = getenv ("MG_DEFAULTMODE"))) {
        strncpy (mode, env_value, LEN_MODE);
        mode [LEN_MODE] = '\0';
    }
    else
#endif
    if (GetMgEtcValue ("pc_xvfb", "defaultmode", mode, LEN_MODE) < 0)
        if (GetMgEtcValue ("system", "defaultmode", mode, LEN_MODE) < 0)
            return ERR_CONFIG_FILE;

    mode [LEN_MODE] = '\0';
    for(i = 0; i < LEN_MODE; i++) {
        mode[i] = tolower(mode[i]);
    }

#ifdef WIN32 //-----------win32----------------
    sprintf (etc_param, "%s %s", window_caption, mode);
    memset (skin, 0, sizeof (skin));
    GetMgEtcValue ("pc_xvfb", "skin", skin, sizeof(skin)-1);
    data->shmrgn = win_PCXVFbInit (execl_file, etc_param, skin);
#elif defined(__CYGWIN__) //----------cygwin--------------
    pid_t  pid;
    int    shmid;
    socklen_t client_len;
    char mapFile[128];
    int  w, h, d, pitch, dataSize, color_num;
    int fd;
    struct sockaddr_in srv_addr;
    struct sockaddr_in clt_addr;

    w = atoi (mode);
    h = atoi (strchr(mode, 'x')+1);
    d = atoi (strrchr(mode, '-')+1);

    if (d == 1) {
        pitch = (w * d + 7) / 8;
    }
    else {
        pitch = ((w * d + 31) / 32) * 4;
    }

    if (d <= 8)
        color_num = 1 << d;
    else
        color_num = 0;

    dataSize = pitch * h + sizeof(XVFBHeader) + color_num * sizeof(XVFBPalEntry);

    sprintf (mapFile,"%s-%d", preMapFile, getpid());

    __mg_pcxvfb_server_sockfd = socket (AF_INET, SOCK_STREAM, 0);

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(getpid()); //port : use the pid
    srv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bind (__mg_pcxvfb_server_sockfd,
            (struct sockaddr *)&srv_addr, sizeof(struct sockaddr));

    listen (__mg_pcxvfb_server_sockfd, 3);

    if ((pid = fork()) < 0) {
        _ERR_PRINTF ("NEWGAL>PCXVFB: fork() error.\n");
    }
    else if (pid == 0) {
        if (execl_pcxvfb() == ERR_CONFIG_FILE)
            _ERR_PRINTF ("NEWGAL>PCXVFB: failed to read configuration.\n");
        perror ("execl");
        _exit (1);
    }

    client_len = sizeof (clt_addr);
    __mg_pcxvfb_client_sockfd =
            accept (__mg_pcxvfb_server_sockfd,
                (struct sockaddr *)&clt_addr, &client_len);

    fd = open (mapFile, O_RDWR);
    if (fd < 0){
        _ERR_PRINTF ("NEWGAL>PCXVFB: open file %s error.\n", mapFile);
        return NULL;
    }

    data->shmrgn = (unsigned char *)mmap (NULL, dataSize,
            PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
#else //-----------------linux----------------------

    pid_t  pid;

    int    server_len;
    socklen_t client_len;
    struct sockaddr_un server_address;
    struct sockaddr_un client_address;
    char   socket_file [50];

    int    shmid;
#ifdef _MGRM_PROCESSES
    FILE  *fp;
#endif

#if defined(_MGRM_PROCESSES)
    if (mgIsServer) {
#endif
        __mg_pcxvfb_server_sockfd = socket (AF_UNIX, SOCK_STREAM, 0);
        server_address.sun_family = AF_UNIX;
        sprintf (socket_file, "/tmp/pcxvfb_socket%d", getpid());
        socket_file[49] = '\0';
        unlink (socket_file);
        strcpy (server_address.sun_path, socket_file);

        //FIXME
        server_len = sizeof(server_address);
        bind (__mg_pcxvfb_server_sockfd,
                (struct sockaddr *)&server_address, server_len);
        listen (__mg_pcxvfb_server_sockfd, 5);
        client_len = sizeof(client_address);

        shm_init_lock(getpid());

        if ((pid = fork()) < 0) {
            GAL_SetError ("NEWGAL>PCXVFB: Error occurred when calling fork().\n");
        }
        else if (pid > 0) {
            ;/* do nothing */
        }
        else {
            if (setpgid (getpid(), 0) < 0) {
                GAL_SetError ("NEWGAL>PCXVFB: Failed to change the gid.\n");
            }

            if (execl_pcxvfb() == ERR_CONFIG_FILE) {
                GAL_SetError ("NEWGAL>PCXVFB: Failed to read configuration!\n");
            }

            perror ("execl");
            _exit (1);
        }

        {
            fd_set rset;
            struct timeval tv;

            FD_ZERO (&rset);
            FD_SET (__mg_pcxvfb_server_sockfd, &rset);
            tv.tv_sec = 2;
            tv.tv_usec = 0;
            if (my_select (__mg_pcxvfb_server_sockfd + 1, &rset,
                        NULL, NULL, &tv) != 1) {
                GAL_SetError ("NEWGAL>PCXVFB: Wait too long for CLIENT.\n");
                close (__mg_pcxvfb_server_sockfd);
                return -1;
            }
        }
        __mg_pcxvfb_client_sockfd =
                accept (__mg_pcxvfb_server_sockfd,
                    (struct sockaddr *)&client_address, &client_len);

        {
            fd_set rset;
            struct timeval tv;

            FD_ZERO (&rset);
            FD_SET (__mg_pcxvfb_client_sockfd, &rset);
            tv.tv_sec = 2;
            tv.tv_usec = 0;
            if (my_select (__mg_pcxvfb_client_sockfd + 1, &rset,
                        NULL, NULL, &tv) != 1) {
                GAL_SetError ("NEWGAL>PCXVFB: Wait too long for SHMID.\n");
                close (__mg_pcxvfb_client_sockfd);
                close (__mg_pcxvfb_server_sockfd);
                return -1;
            }
        }

        if (read (__mg_pcxvfb_client_sockfd, &shmid, sizeof(int)) <
                sizeof (int)) {
            GAL_SetError ("NEWGAL>PCXVFB: read error from client socket.\n");
            close (__mg_pcxvfb_client_sockfd);
            close (__mg_pcxvfb_server_sockfd);
            return -1;
        }

        if (shmid != -1) {
            data->shmrgn = (unsigned char *)shmat (shmid, 0, 0);
#ifdef _MGRM_PROCESSES
            fp = fopen ("/var/tmp/.pcxvfb_tmp", "w+");
            if (fp == NULL) {
                GAL_SetError ("NEWGAL>PCXVFB: failed to open tmp file\n");
                return -1;
            }
            fwrite (&shmid, sizeof(int), 1, fp);
            fwrite (&semid, sizeof(int), 1, fp);
            fclose (fp);
#endif /* _MGRM_PROCESSES */
        }

#if defined(_MGRM_PROCESSES)
    }
    else {
        fp = fopen ("/var/tmp/.pcxvfb_tmp", "r");
        if (fp == NULL) {
            GAL_SetError ("NEWGAL>PCXVFB: can't open tmp file\n");
            return -1;
        }

        if (fread (&shmid, sizeof(int), 1, fp) < 1) {
            GAL_SetError ("NEWGAL>PCXVFB: can't read shmid from tmp file\n");
            fclose (fp);
            return -1;
        }

        if (fread (&semid, sizeof(int), 1, fp) < 1) {
            GAL_SetError ("NEWGAL>PCXVFB: can't read semid from tmp file\n");
            fclose (fp);
            return -1;
        }
        fclose(fp);

        if (shmid != -1) {
            data->shmrgn = (unsigned char *)shmat (shmid, 0, 0);
        }
    }
#endif /* _MGRM_PROCESSES */
#endif //end of os (windows, cygwin, linux)

    if ((INT_PTR)data->shmrgn == -1 || data->shmrgn == NULL) {
        GAL_SetError ("NEWGAL>PCXVFB: Unable to attach to VFB.\n");
        return -1;
    }

    data->hdr = (XVFBHeader *) data->shmrgn;

    vformat->BitsPerPixel = data->hdr->depth;
    switch (vformat->BitsPerPixel) {
#ifdef _MGGAL_SHADOW
        case 1:
            break;
        case 2:
            break;
        case 4:
            break;
#endif /* _MGGAL_SHADOW */
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
        case 32:
            vformat->BytesPerPixel = 4;
            vformat->Amask = data->hdr->Amask;
            vformat->Rmask = data->hdr->Rmask;
            vformat->Gmask = data->hdr->Gmask;
            vformat->Bmask = data->hdr->Bmask;
            break;
        default:
            GAL_SetError ("NEWGAL>PCXVFB: Not supported depth: %d, "
                    "please try to use Shadow NEWGAL engine with "
                    "targetname `pc_xvfb`.\n",
                    vformat->BitsPerPixel);
            return -1;
    }

    return 0;
}

static GAL_Rect **PCXVFB_ListModes (_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    return (GAL_Rect **) -1;
}

static GAL_Surface *PCXVFB_SetVideoMode (_THIS, GAL_Surface *current,
        int width, int height, int bpp, Uint32 flags)
{
    /* Set up the mode framebuffer */
    current->flags  = GAL_HWSURFACE | GAL_FULLSCREEN;
    current->w      = this->hidden->hdr->width;
    current->h      = this->hidden->hdr->height;
    current->pitch  = this->hidden->hdr->pitch;
    current->format->MSBLeft = this->hidden->hdr->MSBLeft;

    /* Since 5.0.0, check double buffers.
     * XXX: not work for platforms other than Linux
     */
    if (this->hidden->hdr->data_size >=
            this->hidden->hdr->fb_offset + (current->pitch * current->h * 2)) {
        unsigned char* real_pixels = this->hidden->shmrgn;
        real_pixels += this->hidden->hdr->fb_offset;

        this->hidden->real_screen = GAL_CreateRGBSurfaceFrom (real_pixels,
                        current->w, current->h, current->format->BitsPerPixel,
                        current->pitch,
                        current->format->Rmask, current->format->Gmask,
                        current->format->Bmask, current->format->Amask);
        if (this->hidden->real_screen) {
            current->pixels = real_pixels + (current->pitch * current->h);
            this->hidden->shadow_screen = current;

            GAL_SetClipRect (this->hidden->real_screen, NULL);
            GAL_SetColorKey (this->hidden->shadow_screen, 0, 0);
            GAL_SetAlpha (this->hidden->shadow_screen, 0, 0);

#ifdef _MGSCHEMA_COMPOSITING
            this->info.hw_cursor = 1;
            this->hidden->cursor = NULL;
            this->SetCursor = shadowScreen_SetCursor;
            this->MoveCursor = shadowScreen_MoveCursor;
#endif
        }
        else {
            current->pixels = real_pixels;
            this->hidden->shadow_screen = NULL;
        }
    }
    else {
        current->pixels = this->hidden->shmrgn + this->hidden->hdr->fb_offset;
        this->hidden->real_screen = NULL;
        this->hidden->shadow_screen = NULL;
    }

    /* We're done */
    return current;
}

static int PCXVFB_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    return -1;
}

static void PCXVFB_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

static int PCXVFB_SetColors(_THIS, int firstcolor,
        int ncolors, GAL_Color *colors)
{
    XVFBPalEntry *palette;
    int i;
    int bpp = this->screen->format->BitsPerPixel;
    int pixel = firstcolor;

    palette = (XVFBPalEntry *)((BYTE *)this->hidden->hdr
            + this->hidden->hdr->palette_offset);

    if (bpp < 8) {
        int num = 1 <<  bpp;
        pixel = firstcolor >> bpp;
        colors = __gal_screen->format->palette->colors;
        for (i = pixel; i < num; i++) {
            int index = i << (8 - bpp);
            if (index) index--;
            palette[pixel].r = colors[index].r & 0xff;
            palette[pixel].g = colors[index].g & 0xff;
            palette[pixel].b = colors[index].b & 0xff;
            palette[pixel].a = colors[index].a & 0xff;

            pixel++;
        }
        palette[(1<<bpp)-1].r = colors[255].r & 0xff;
        palette[(1<<bpp)-1].g = colors[255].g & 0xff;
        palette[(1<<bpp)-1].b = colors[255].b & 0xff;
        palette[(1<<bpp)-1].a = colors[255].a & 0xff;
    }
    else {
        for (i = 0; i < ncolors; i++) {
            palette[pixel].r = colors[i].r & 0xff;
            palette[pixel].g = colors[i].g & 0xff;
            palette[pixel].b = colors[i].b & 0xff;
            palette[pixel].a = colors[i].a & 0xff;
            pixel++;
        }
    }
    this->hidden->hdr->palette_changed = 1;

    return 1;
}

static void PCXVFB_VideoQuit (_THIS)
{
    /* Since 5.0.0 */
    if (this->hidden->real_screen) {
        GAL_FreeSurface (this->hidden->real_screen);
    }

#ifdef WIN32 // windows
    win_PCXVFbClose ();
#elif defined (__CYGWIN__) // cygwin
    int fd;
    char mapFile[128];
    struct stat sb;
    sprintf (mapFile,"%s-%d", preMapFile, getpid());
    fd = open (mapFile, O_RDONLY);
    fstat (fd, &sb);
    int size = sb.st_size;
    munmap (this->hidden->shmrgn, size);
    close (fd);
    remove (mapFile);
#else        // linux
    shmdt (this->hidden->shmrgn);
#endif

#if defined(_MGRM_PROCESSES)
    if (mgIsServer)
#endif
        semctl (semid, 0, IPC_RMID);
}

#endif /* _MGGAL_PCXVFB*/

