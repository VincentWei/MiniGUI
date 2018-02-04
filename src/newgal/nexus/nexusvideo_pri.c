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
#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "cursor.h"
#include "internals.h"
#include "inline.h"
#include "memops.h"
#include "ctrlclass.h"
#include "dc.h"

#include "nexusvideo_pri.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#define dbg() printf("%s %d\n", __FUNCTION__, __LINE__)

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_display.h"
#include "nexus_graphics2d.h"
#include "nexus_core_utils.h"

/*************************************/

/* Primary-surface flushing methods
 */

/* #define AUTO_FLUSH *//* Should not use this option, since it doesn't flush until there comes a 2D-accelerate operation */

/* #define SYNC_FLUSH */
#define ASYNC_FLUSH             /* Flush the _Primary_ surface asynchronously */

/*************************************/

/* Wait methods
 */

#if 0
#   define HAS_WAIT_SEM /* Slower than BLOCKED_SYNC */
#else
#   define BLOCKED_SYNC
#endif

/*************************************/

#define UNDO_FLAG SEM_UNDO

#if defined(ASYNC_FLUSH) || defined(HAS_WAIT_SEM)
#   ifdef _MGRM_THREADS
union semun
{
    int     val;                /* Value for SETVAL */
    struct semid_ds *buf;       /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array;      /* Array for GETALL, SETALL */
    struct seminfo *__buf;      /* Buffer for IPC_INFO (Linux specific) */
};
#   endif /* _MGRM_THREADS */
#endif
#endif

#ifdef HAS_WAIT_SEM
static int wait_semid = -1;
#endif

#ifdef ASYNC_FLUSH
static int async_running = 1;
static int flush_semid = -1;
pthread_t async_th = (pthread_t) -1;

#define SEM_NUM_MUTEX 0
#define SEM_NUM_DIRTY 1
#endif

typedef struct _NexusPrivateData NexusPrivateData;
struct _NexusPrivateData
{
    NEXUS_DisplayHandle display;
    NEXUS_SurfaceHandle primarySurface;
    NEXUS_Graphics2DHandle gfx;
};
static NEXUS_DisplayHandle s_display = NULL;
static NEXUS_SurfaceHandle s_primary_surface = NULL;
static NEXUS_Graphics2DHandle s_graphics2d = NULL;

#define IS_PRIMARY_SURFACE(hwdata) ((hwdata)->surfaceHandle == s_primary_surface)

struct exit_routing {
        struct exit_routing *next;
        void (*on_exit)(void *);
        void *arg;
};

static struct exit_routing *exit_routings_head = NULL;

#define GET_this NexusPrivateData *this = (NexusPrivateData *)_this
#define UNUSED(arg) arg = arg

static void
my_close_sems(void)
{
#ifdef HAS_WAIT_SEM
    if (wait_semid >= 0)
    {
        semctl(wait_semid, -1, IPC_RMID);
        wait_semid = -1;
    }
#endif
#ifdef ASYNC_FLUSH
    if (flush_semid >= 0)
    {
        semctl(flush_semid, -1, IPC_RMID);
        flush_semid = -1;
    }
#endif
}


#if defined(HAS_WAIT_SEM) || defined(ASYNC_FLUSH)
static void
my_sem_setvalue(int semid, int num, int val)
{
    union semun arg;

    for (;;)
    {
        arg.val = val;
        if (semctl(semid, num, SETVAL, arg) < 0)
        {
            if (errno == EINTR)
                continue;
            else
            {
                fprintf(stderr,
                        "semctl(op=setval, semid=%d, num=%d, val=%d): [%d]%s",
                        semid, num, val, errno, strerror(errno));
                exit(1);
            }
        }

        break;
    }
}

static int
my_sem_getvalue(int semid, int num)
{
    int     ret;

    for (;;)
    {
        if ((ret = semctl(semid, num, GETVAL)) < 0)
        {
            if (errno == EINTR)
                continue;
            else
            {
                fprintf(stderr, "semctl(op=get_value, id=%d, num=%d): [%d]%s",
                        semid, num, errno, strerror(errno));
                exit(1);
            }
        }
        return ret;
    }
}

static void
my_sem_do_op(int semid, int num, int val, int flag)
{
    struct sembuf op;

    for (;;)
    {
        op.sem_num = num;
        op.sem_op = val;
        op.sem_flg = flag;
        if (semop(semid, &op, 1) < 0)
        {
            if (errno == EINTR)
                continue;
            else
            {
                fprintf(stderr,
                        "semop(semid=%d, num=%d, op=%d, flag=%x): [%d]%s",
                        semid, num, val, flag, errno, strerror(errno));
                exit(1);
            }
        }

        break;
    }
}

static void
my_sem_lock(int semid, int num)
{
    my_sem_do_op(semid, num, -1, UNDO_FLAG);
}

static void
my_sem_unlock(int semid, int num)
{
    my_sem_do_op(semid, num, 1, UNDO_FLAG);
}
#endif

#ifdef ASYNC_FLUSH

/* Lock and flush */
static void
before_2d(NEXUS_SurfaceHandle primarySurface)
{
    if (!primarySurface)
    {
        return;
    }
    my_sem_lock(flush_semid, SEM_NUM_MUTEX);

    if (my_sem_getvalue(flush_semid, SEM_NUM_DIRTY) > 0)
    {
        NEXUS_Surface_Flush(primarySurface);
        my_sem_setvalue(flush_semid, SEM_NUM_DIRTY, 0);
    }
}

/* Unlock */
static void
end_2d(NEXUS_SurfaceHandle primarySurface)
{
    if (!primarySurface)
        return;
    my_sem_unlock(flush_semid, SEM_NUM_MUTEX);
}

/* Thread for flushing the _PRIMARY_ surface asynchronously */
static void *
async_flush_thread(void *_primarysurface)
{
    while (async_running)
    {
        if (my_sem_getvalue(flush_semid, SEM_NUM_DIRTY) > 0)
        {
            my_sem_lock(flush_semid, SEM_NUM_MUTEX);

            if (my_sem_getvalue(flush_semid, SEM_NUM_DIRTY) > 0)
            {
                NEXUS_Surface_Flush((NEXUS_SurfaceHandle) _primarysurface);
                my_sem_setvalue(flush_semid, SEM_NUM_DIRTY, 0);
            }

            my_sem_unlock(flush_semid, SEM_NUM_MUTEX);
        }

        usleep(200);
    }
    return NULL;
}
#else
#   define before_2d(x)         /* NULL */
#   define end_2d(x)            /* NULL */
#endif

/*for hdmi*/
static void hotplug_callback(void *pParam, int iParam)
{
    static bool connected = false;
    NEXUS_HdmiOutputStatus status;
    NEXUS_HdmiOutputHandle hdmi = (NEXUS_HdmiOutputHandle)pParam;
    NEXUS_DisplayHandle display = (NEXUS_DisplayHandle)iParam;
    NEXUS_DisplaySettings displaySettings;

    NEXUS_HdmiOutput_GetStatus(hdmi, &status);
    
    if ( connected != status.connected )
    {
        if ( connected )
        {
            fprintf(stderr, "\nHotplug - Disconnecting HDMI from display\n");
            /*
            disconnect_hdmi_audio(hdmi);
            */
            NEXUS_Display_RemoveOutput(display, NEXUS_HdmiOutput_GetVideoConnector(hdmi));           
        }
        else
        {
            NEXUS_Display_GetSettings(display, &displaySettings);
            if ( !status.videoFormatSupported[displaySettings.format] )
            {
                fprintf(stderr, "\nCurrent format not supported by attached monitor.  Switching to preferred format %d", status.preferredVideoFormat);
                displaySettings.format = status.preferredVideoFormat;
                NEXUS_Display_SetSettings(display, &displaySettings);
            }

            
            fprintf(stderr, "\nHotplug - connecting HDMI to display\n");
            NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(hdmi));
            /*
            connect_hdmi_audio(hdmi);
            */
        }
        connected = status.connected;
    }
}

static struct {
    GAL_Rect rect;
    int format;
    int aspectRatio; /* not needed */
} resolutions[] = {
    { { 0, 0, 1920, 1080}, NEXUS_VideoFormat_e1080i, -1 },
    { { 0, 0, 1280, 720}, NEXUS_VideoFormat_e720p, -1 },
    { { 0, 0, 720, 576}, NEXUS_VideoFormat_e576p, -1 },
    { { 0, 0, 720, 483}, NEXUS_VideoFormat_e480p, -1 },
    { { 0, 0, 720, 482}, NEXUS_VideoFormat_eNtsc, -1 },
    /* The following vesa mode resolutions may only be supported on a HDMI output device
     */
    { { 0, 0, 1024, 768}, NEXUS_VideoFormat_eVesa1024x768p60hz, -1 },
    { { 0, 0, 800, 600}, NEXUS_VideoFormat_eVesa800x600p60hz, -1 },

    { { 0, 0, 0, 0 }, 0, 0 }
};

GAL_Rect **NexusPrivate_get_modes_rects(GAL_Rect ** modes, int size)
{
    int i;
    for (i=0; i<size-1; i++)
    {
        modes[i] = &resolutions[i].rect;
    }
    modes[i] = NULL;
    return modes;
}

int NexusPrivate_get_mode(int bpp, int width, int height)
{
    int i;
    if (bpp != 32)
    {
        fprintf(stderr, "NexusGAL: support 32-bit only\n");
        return -1;
    }
    for (i=0; resolutions[i].rect.w; i++)
    {
        if (resolutions[i].rect.w == width && resolutions[i].rect.h == height)
        {
            return i;
        }
    }
    fprintf(stderr, "NexusGAL: unsupported resolution %dx%d-32\n", width, height);
    return -1;
}

static int
NexusPrivate_init_server(void **pThis, int width, int height)
{
    NexusPrivateData *this;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_HdmiOutputSettings hdmiSettings;
    int mode_index;

    mode_index = NexusPrivate_get_mode(32, width, height);
    if (mode_index < 0)
    {
        return -1;
    }

    this = (NexusPrivateData *) malloc(sizeof(*this));
    if (!this)
    {
        fprintf(stderr, "malloc() failed\n");
        return -1;
    }
    memset(this, 0, sizeof(*this));

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    if (NEXUS_Platform_Init(&platformSettings))
    {
        fprintf(stderr, "NEXUSGAL: NEXUS_Platform_Init() failed\n");
        goto fail;
    }
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if NEXUS_DTV_PLATFORM
#error
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_e720p;
    this->display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display,
                            NEXUS_PanelOutput_GetConnector(platformConfig.
                                                           outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;

    displaySettings.format = resolutions[mode_index].format;
#if 0
    displaySettings.aspectRatio = resolutions[mode_index].aspectRatio;
#endif

    this->display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(this->display,
                            NEXUS_ComponentOutput_GetConnector(platformConfig.
                                                               outputs.
                                                               component[0]));
#endif
#if NEXUS_NUM_SCART_INPUTS
    NEXUS_Display_AddOutput(this->display,
                            NEXUS_ScartInput_GetVideoOutputConnector
                            (platformConfig.inputs.scart[0]));
#if NEXUS_NUM_SCART_INPUTS>1
    NEXUS_Display_AddOutput(this->display,
                            NEXUS_ScartInput_GetVideoOutputConnector
                            (platformConfig.inputs.scart[1]));
#endif
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS && !NEXUS_DTV_PLATFORM
    if (platformConfig.outputs.composite[0])
    {
        NEXUS_Display_AddOutput(this->display,
                                NEXUS_CompositeOutput_GetConnector
                                (platformConfig.outputs.composite[0]));
    }
#endif
#endif

    /* for hdmi */
    NEXUS_HdmiOutput_GetSettings(platformConfig.outputs.hdmi[0], &hdmiSettings);
    hdmiSettings.hotplugCallback.callback = hotplug_callback;
    hdmiSettings.hotplugCallback.context = platformConfig.outputs.hdmi[0];
    hdmiSettings.hotplugCallback.param = (int) this->display;
    NEXUS_HdmiOutput_SetSettings(platformConfig.outputs.hdmi[0], &hdmiSettings);
    hotplug_callback(platformConfig.outputs.hdmi[0], (int) this->display);
    

    this->gfx = NEXUS_Graphics2D_Open(0, NULL);
    if (!this->gfx)
    {
        fprintf(stderr, "NEXUS_Graphics2D_Open() failed\n");
        goto fail;
    }
#ifdef BLOCKED_SYNC
    {
        NEXUS_Graphics2DSettings settings;

        NEXUS_Graphics2D_GetSettings(this->gfx, &settings);
        settings.blockedSync = true;
        if (NEXUS_Graphics2D_SetSettings(this->gfx, &settings))
        {
            fprintf(stderr,
                    "NEXUS_Graphics2D_SetSettings(autoFlush=true) failed\n");
            goto fail;
        }
    }
#endif

#ifdef HAS_WAIT_SEM
    wait_semid = semget(0x3eadbeef, 1, IPC_CREAT | 0600);
    if (wait_semid < 0)
    {
        fprintf(stderr, "create sem failed\n");
        goto fail;
    }
    my_sem_setvalue(wait_semid, 0, 0);
#endif

#ifdef ASYNC_FLUSH
    flush_semid = semget(0xeeadbeef, 2, IPC_CREAT | 0600);
    if (flush_semid < 0)
    {
        fprintf(stderr, "create sem failed: [%d]%s\n", errno, strerror(errno));
        goto fail;
    }
    my_sem_setvalue(flush_semid, SEM_NUM_MUTEX, 1);
    my_sem_setvalue(flush_semid, SEM_NUM_DIRTY, 0);
#endif

    *pThis = this;
    return 0;
  fail:
    my_close_sems();
    if (this)
    {
        if (this->gfx)
        {
            NEXUS_Graphics2D_Close(this->gfx);
        }
        if (this->display)
        {
            NEXUS_Display_RemoveAllOutputs(this->display);
            NEXUS_Display_Close(this->display);
        }
        free(this);
    }
    return -1;
}

int
NexusPrivate_init_client(void **pThis)
{
    NexusPrivateData *this;

    /* Client */
    if (NEXUS_Platform_Join())
    {
        return -1;
    }
    this = (NexusPrivateData *) malloc(sizeof(*this));
    if (!this)
    {
        fprintf(stderr, "malloc() failed\n");
        return -1;
    }
    memset(this, 0, sizeof(*this));
    this->display = NULL;       /* No use */

#ifdef HAS_WAIT_SEM
    wait_semid = semget(0x3eadbeef, 1, 0600);
    if (wait_semid < 0)
    {
        fprintf(stderr, "get sem failed: [%d]%s\n", errno, strerror(errno));
        free(this);
        return -1;
    }
#endif

#ifdef ASYNC_FLUSH
    flush_semid = semget(0xeeadbeef, 2, 0600);
    if (flush_semid < 0)
    {
        fprintf(stderr, "get sem failed: [%d]%s\n", errno, strerror(errno));
        free(this);
        return -1;
    }
#endif

    *pThis = this;
    return 0;
}

int
NexusPrivate_init(void **pThis, int isServer, int width, int height)
{
    if (isServer)
    {
        return NexusPrivate_init_server(pThis, width, height);
    }
    else
    {
        return NexusPrivate_init_client(pThis);
    }
}

void
NexusPrivate_close(void *_this, int isServer)
{
    GET_this;

    {
        struct exit_routing *i, *j;
        for (i=exit_routings_head; i; /* NULL */)
        {
                i->on_exit(i->arg);
                j = i->next;
                free(i);
                i = j;
        }
    }

    /* XXX: Need to clear screen? */
    /* NEXUS_StopCallbacks(this->display); */
    if (isServer)
    {
#ifdef ASYNC_FLUSH
        async_running = 0;
        if (async_th != ((pthread_t) -1))
        {
            pthread_join(async_th, NULL);
            async_th = (pthread_t) -1;
        }
#endif
        my_close_sems();
        NEXUS_Graphics2D_Close(this->gfx);
        NEXUS_Display_RemoveAllOutputs(this->display);
        NEXUS_Display_Close(this->display);
    }
    NEXUS_Platform_Uninit();
}

int
NexusPrivate_CreateSurface(void *_this, int w, int h, unsigned int *pPitch,
                           void **pPixels,
                           NexusPrivate_HWSurface_hwdata ** pHwdata)
{
    NEXUS_SurfaceCreateSettings surfaceCfg;
    NEXUS_SurfaceHandle surfaceHandle;
    NEXUS_SurfaceMemory surfaceMemory;
    NexusPrivate_HWSurface_hwdata *hwdata;

    GET_this;

    /* printf("CreateSurface(w=%d, h=%d)\n", w, h); */

    NEXUS_Surface_GetDefaultCreateSettings(&surfaceCfg);
    surfaceCfg.width = w;
    surfaceCfg.height = h;
    /* TODO */
    surfaceCfg.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    surfaceCfg.pitch = 0;       /* default, width * sizeof(pixel) */
    surfaceHandle = NEXUS_Surface_Create(&surfaceCfg);
    if (!surfaceHandle)
    {
        fprintf(stderr, "NEXUS_Surface_Create() failed. size=%dx%d\n", w, h);
        return -1;
    }
#ifdef AUTO_FLUSH
    if (1)
#else
    if (this->primarySurface)   /* add AutoFlush flag for all of the MemDC */
#endif
    {                           /* Auto flush */
        NEXUS_SurfaceSettings settings;

        NEXUS_Surface_GetSettings(surfaceHandle, &settings);
        settings.autoFlush = true;
        if (NEXUS_Surface_SetSettings(surfaceHandle, &settings))
        {
            fprintf(stderr,
                    "NEXUS_Surface_SetSettings(autoFlush=true) failed\n");
            NEXUS_Surface_Destroy(surfaceHandle);
            return -1;
        }
    }

    NEXUS_Surface_GetMemory(surfaceHandle, &surfaceMemory);
    *pPitch = surfaceMemory.pitch;
    *pPixels = surfaceMemory.buffer;

    hwdata = malloc(sizeof(*hwdata));
    memset(hwdata, 0, sizeof(*hwdata));
    hwdata->display = this->display;
    hwdata->surfaceHandle = surfaceHandle;
    hwdata->gfx = this->gfx;
    *pHwdata = hwdata;

    NexusPrivate_FillRect (this, hwdata, 0, 0, w, h, 0xff000000);

    return 0;
}

NexusPrivate_HWSurface_hwdata *
NexusPrivate_SurfaceDup(NexusPrivate_HWSurface_hwdata * src)
{
    NexusPrivate_HWSurface_hwdata *dst;

    dst = (NexusPrivate_HWSurface_hwdata *) malloc(sizeof(*dst));
    memset(dst, 0, sizeof(*dst));
    memcpy(dst, src, sizeof(*dst));
    dst->isCopied = 1;
    return dst;
}

void
NexusPrivate_FreeSurface(void *_this, NexusPrivate_HWSurface_hwdata * hwdata)
{
    UNUSED(_this);
    if (!hwdata)
    {
        fprintf(stderr,
                "Error: NexusPrivate_FreeSurface() Failed, bad param\n");
        return;
    }
    if (!hwdata->isCopied)
    {
        NEXUS_Surface_Destroy((NEXUS_SurfaceHandle) hwdata->surfaceHandle);
    }
    free(hwdata);
}

void   *
NexusPrivate_SurfaceGetMemory(void *_surfaceHandle)
{
    NEXUS_SurfaceMemory surfaceMemory;

    NEXUS_Surface_GetMemory((NEXUS_SurfaceHandle) _surfaceHandle,
                            &surfaceMemory);
    return surfaceMemory.buffer;
}

#ifdef HAS_WAIT_SEM
static void
complete(void *data, int unused)
{
    UNUSED(data);
    UNUSED(unused);
    my_sem_do_op(wait_semid, 0, 1, UNDO_FLAG);
}

static void
NexusPrivate_Wait(NEXUS_Graphics2DHandle gfx)
{
    NEXUS_CallbackDesc callback;

    callback.callback = complete;
    callback.context = NULL;
    NEXUS_Graphics2D_Checkpoint(gfx, &callback);

    my_sem_do_op(wait_semid, 0, -1, UNDO_FLAG);
}
#else
#   define NexusPrivate_Wait(x) /* NULL */
#endif

int
NexusPrivate_FillRect(void *_this, NexusPrivate_HWSurface_hwdata * hwdata,
                      int x, int y, int w, int h, unsigned int color)
{
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Error ret;

    GET_this;
    NEXUS_SurfaceHandle primarySurface = NULL;

    /*
    printf("Enter NexusPrivate_FillRect(surface=%p, (gfx=%p), x=%d, y=%d, w=%d, h=%d, color=%x)\n", hwdata->surfaceHandle, hwdata->gfx, x, y, w, h, color);
    */

    if (IS_PRIMARY_SURFACE(hwdata))
        primarySurface = hwdata->surfaceHandle;
    before_2d(primarySurface);

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = (NEXUS_SurfaceHandle) hwdata->surfaceHandle;
    fillSettings.rect.x = x;
    fillSettings.rect.y = y;
    fillSettings.rect.width = w;
    fillSettings.rect.height = h;
    fillSettings.color = color;
    ret = NEXUS_Graphics2D_Fill(this->gfx, &fillSettings);

    if (ret)
    {
        end_2d(primarySurface);
        fprintf(stderr, "NEXUS_Graphics2D_Fill() failed: %d\n", ret);

        return -1;
    }
    NexusPrivate_Wait(this->gfx);

    end_2d(primarySurface);

    return 0;
}

int
NexusPrivate_SelectSurface(void *_this, NexusPrivate_HWSurface_hwdata * hwdata,
                           int isServer)
{
    NEXUS_GraphicsSettings graphicsSettings;

    GET_this;

    if (isServer)
    {
        NEXUS_Display_GetGraphicsSettings(this->display, &graphicsSettings);
        graphicsSettings.enabled = true;
        graphicsSettings.zorder = 0; /* put video(zorder=1) on top of graphics */
        NEXUS_Display_SetGraphicsSettings(this->display, &graphicsSettings);
        NEXUS_Display_SetGraphicsFramebuffer(this->display,
                                             (NEXUS_SurfaceHandle) hwdata->
                                             surfaceHandle);
#ifdef ASYNC_FLUSH
        pthread_create(&async_th, NULL, async_flush_thread,
                       hwdata->surfaceHandle);
#endif
    }
    else
    {
        this->display = hwdata->display;
        this->gfx = hwdata->gfx;
    }
    s_display = this->display;
    printf("display = %p\n", (void *)s_display);
    s_primary_surface = this->primarySurface = (NEXUS_SurfaceHandle) hwdata->surfaceHandle;
    s_graphics2d = this->gfx;

    return 0;
}

int
NexusPrivate_BitBlitEx(void *is_null, NexusPrivate_HWSurface_hwdata * src,
                     int src_x, int src_y, int src_w, int src_h,
                     NexusPrivate_HWSurface_hwdata * dst, int dst_x, int dst_y,
                     int dst_w, int dst_h,
                     int flag, unsigned int colorkey_src, unsigned int colorkey_dst, unsigned int alpha_src)
{
    NEXUS_Rect src_rect;
    NEXUS_Rect dst_rect;
    NEXUS_Error ret;
    NEXUS_SurfaceHandle primarySurface = NULL;

    UNUSED(is_null);            /* use dst->gfx */
    UNUSED(colorkey_dst);
    /*
    printf("<%d> Enter NexusPrivate_BitBlit(%p(%d,%d,%d,%d) -> %p(%d,%d,%d,%d)) flag=%d colorkey(0x%x,0x%x) alpha(0x%x)\n", 
            times(NULL),
            src->surfaceHandle, src_x, src_y, src_w, src_h,
            dst->surfaceHandle, dst_x, dst_y, dst_w, dst_h,
            flag, colorkey_src, colorkey_dst, alpha_src);
    */

    if (IS_PRIMARY_SURFACE(src))
        primarySurface = src->surfaceHandle;
    else if (IS_PRIMARY_SURFACE(dst))
        primarySurface = dst->surfaceHandle;

    before_2d(primarySurface);

    src_rect.x = src_x;
    src_rect.y = src_y;
    src_rect.width = src_w;
    src_rect.height = src_h;
    dst_rect.x = dst_x;
    dst_rect.y = dst_y;
    dst_rect.width = dst_w;
    dst_rect.height = dst_h;
    /* if (flag){ */
    if (1){
        NEXUS_Graphics2DBlitSettings blit;
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blit);

        blit.source.surface = src->surfaceHandle;
        memcpy(&blit.source.rect, &src_rect, sizeof(src_rect));
        blit.dest.surface = dst->surfaceHandle;
        memcpy(&blit.dest.rect, &dst_rect, sizeof(dst_rect));
        blit.output.surface = dst->surfaceHandle;
        memcpy(&blit.output.rect, &dst_rect, sizeof(dst_rect));

        if ((flag & 4)) /* src alpha */
        {
            blit.colorOp = NEXUS_BlitColorOp_eUseConstantAlpha;
            blit.constantColor = alpha_src << 24;
        }
        else
        {
            blit.colorOp = NEXUS_BlitColorOp_eUseSourceAlpha;
        }
#if 1
        blit.alphaOp = NEXUS_BlitAlphaOp_eCopyDest;
#else /* source over */
        blit.alphaOp = NEXUS_BlitAlphaOp_eCombine;
#endif

        if ((flag & 1)){ /* src colorkey */
            blit.colorKey.source.enabled = true;
            blit.colorKey.source.upper = colorkey_src;
            blit.colorKey.source.mask = 0x00ffffff;
#if 0
        /* TODO: */
        }else if ((flag & 2)){  /* dst colorkey */
            blit.colorKey.dest.enabled = true;
            blit.colorKey.dest.mask = colorkey_dst;
#endif
        }

        ret = NEXUS_Graphics2D_Blit(dst->gfx, &blit);
    }else{
        ret = NEXUS_Graphics2D_FastBlit(dst->gfx,
                (NEXUS_SurfaceHandle) dst->surfaceHandle,
                &dst_rect,
                (NEXUS_SurfaceHandle) src->surfaceHandle,
                &src_rect,
                /* NEXUS_BlitColorOp_eCopySource, */
                NEXUS_BlitColorOp_eUseSourceAlpha,
                /* NEXUS_BlitAlphaOp_eCopySource, */
                NEXUS_BlitAlphaOp_eCopyDest,
                0);
    }

    if (ret)
    {
        end_2d(primarySurface);
        fprintf(stderr, "NEXUS_Graphics2D_FastBlit() failed: %d\n", ret);
        return -1;
    }
    else
    {
        NexusPrivate_Wait(dst->gfx);
        end_2d(primarySurface);
        return 0;
    }
}

void
NexusPrivate_Flush(void *_this, NexusPrivate_HWSurface_hwdata *surface)
{
#ifdef SYNC_FLUSH
    GET_this;
#   ifndef PLEASE_TEST_MGALLERY_ON_97405
    NEXUS_Surface_Flush(this->primarySurface);
#   endif
    NEXUS_Surface_Flush(surface);
#elif defined(ASYNC_FLUSH)
    my_sem_lock(flush_semid, SEM_NUM_MUTEX);
    my_sem_setvalue(flush_semid, SEM_NUM_DIRTY, 1);
    my_sem_unlock(flush_semid, SEM_NUM_MUTEX);
    UNUSED(surface);
    UNUSED(_this);
#else
    UNUSED(surface);
    UNUSED(_this);
#endif
}

void   *
NexusPrivate_GetDisplay(void)
{
    return s_display;
}

void   *
NexusPrivate_GetPrimarySurface(void)
{
    return s_primary_surface;
}

void    *
NexusPrivate_GetGraphics2D(void)
{
    return s_graphics2d;
}

void
NexusPrivate_add_on_exit(void (*on_exit)(void *), void *arg)
{
    struct exit_routing *r, *i;
    r = (struct exit_routing *)malloc(sizeof(struct exit_routing));
    r->on_exit = on_exit;
    r->arg = arg;
    r->next = NULL;
    if (! exit_routings_head)
    {
        exit_routings_head = r;
        return;
    }
    for (i=exit_routings_head; i->next; i=i->next)
        ;
    i->next = r;
}

/* XXX: use CreateMemDCEx() instead
HDC NexusPrivate_CreateMemDCFromPixels(void *pixels, int width, int height)
{
    HDC memdc;
    DC *pdc;

    memdc = CreateMemDC (width, height, 32, 0,
            0x00ff0000,
            0x0000ff00,
            0x000000ff,
            0xff000000);

    pdc = (DC *)memdc;
    pdc->surface->flags = GAL_PREALLOC;
    free(pdc->surface->pixels);
    pdc->surface->pixels = pixels;
    return memdc;
}
*/
