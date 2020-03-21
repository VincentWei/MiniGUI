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
 *   Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
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
/*
** sharedres.h: structure of shared resource.
**
** Create date: 2000/12/xx
*/

#ifndef GUI_SHAREDRES_H
    #define GUI_SHAREDRES_H

#include "common.h"

#ifdef _MGRM_PROCESSES

#include <sys/time.h>
#include <sys/termios.h>

#include "constants.h"

enum {
    _IDX_SEM_DRAW = 0,
    _IDX_SEM_SCR,
#ifdef _MGHAVE_CURSOR
    _IDX_SEM_CURSOR,
    _IDX_SEM_HIDECOUNT,
#endif
#ifdef _MGRM_PROCESSES
    _IDX_SEM_MOUSEMOVE,
#endif
    _NR_SYS_SEM,
};

inline static key_t get_shm_key_for_system (void)
{
    return (key_t)(IPC_KEY_BASE + 0x01);
}

inline static key_t get_sem_key_for_system (void)
{
    return (key_t)(IPC_KEY_BASE + 0x02);
}

inline static key_t get_sem_key_for_layers (void)
{
    return (key_t)(IPC_KEY_BASE + 0x03);
}

inline static key_t get_sem_key_for_shared_surf (void)
{
    return (key_t)(IPC_KEY_BASE + 0x04);
}

typedef struct tagG_RES {
    int semid;
    int shmid;

    /* information of NEWGAL engine; since 5.0.0 */
    char video_engine [LEN_ENGINE_NAME + 1];
    char video_mode [LEN_VIDEO_MODE + 1];
    char video_device [LEN_DEVICE_NAME + 1];
    int  video_dpi, video_hres, video_vres, video_depth;
    Uint32 video_rmask, video_gmask, video_bmask, video_amask;
#ifdef _MGGAL_DRM
    char video_exdriver [LEN_EXDRIVER_NAME + 1];
    Uint32 video_drm_format;
    Uint32 video_dbl_buff:1;
#endif

    int nr_layers;
    int semid_layer;
#ifdef _MGSCHEMA_COMPOSITING
    int semid_shared_surf;  /* the id of semaphore set for shared surfaces */
#endif

    int nr_globals;
    int def_nr_topmosts;
    int dev_nr_normals;

    GHANDLE topmost_layer;

    DWORD timer_counter;
    DWORD tick_on_locksem;
    struct timeval timeout;
    struct termios savedtermio;
#ifdef _MGRM_PROCESSES
    int mouse_move_client; /* The client which can receive mouse move messages */
    unsigned int mouse_move_serial; /* serial number to maintain mouse move messages */
#endif
    int mousex, mousey;
    int mousebutton;
    int shiftstatus;
    Uint8 kbd_state [MGUI_NR_KEYS + 1];

#ifdef _MGHAVE_CURSOR
    int cursorx, cursory;
    int oldboxleft, oldboxtop;
    HCURSOR csr_current;
    int xhotspot, yhotspot;
    int csr_show_count;

    int csrnum;
#ifdef _MGSCHEMA_COMPOSITING
    DWORD   __align;
    HCURSOR sys_cursors[0];
#else
    unsigned long svdbitsoffset;
    unsigned long csroffset;
#endif
#endif

/*
    int sysfontnum;
    int rbffontnum;
    int varfontnum;
    int iconnum;
    int bmpnum;

    unsigned long sfontoffset;
    unsigned long rfontoffset;
    unsigned long vfontoffset;
    unsigned long iconoffset;
    unsigned long bmpoffset;
*/
} G_RES;
typedef G_RES* PG_RES;

#define SHAREDRES_VIDEO_ENGINE      (((PG_RES)mgSharedRes)->video_engine)
#define SHAREDRES_VIDEO_MODE        (((PG_RES)mgSharedRes)->video_mode)
#define SHAREDRES_VIDEO_DEVICE      (((PG_RES)mgSharedRes)->video_device)
#define SHAREDRES_VIDEO_DPI         (((PG_RES)mgSharedRes)->video_dpi)
#define SHAREDRES_VIDEO_HRES        (((PG_RES)mgSharedRes)->video_hres)
#define SHAREDRES_VIDEO_VRES        (((PG_RES)mgSharedRes)->video_vres)
#define SHAREDRES_VIDEO_DEPTH       (((PG_RES)mgSharedRes)->video_depth)
#define SHAREDRES_VIDEO_RMASK       (((PG_RES)mgSharedRes)->video_rmask)
#define SHAREDRES_VIDEO_GMASK       (((PG_RES)mgSharedRes)->video_gmask)
#define SHAREDRES_VIDEO_BMASK       (((PG_RES)mgSharedRes)->video_bmask)
#define SHAREDRES_VIDEO_AMASK       (((PG_RES)mgSharedRes)->video_amask)

#ifdef _MGGAL_DRM
#define SHAREDRES_VIDEO_EXDRIVER    (((PG_RES)mgSharedRes)->video_exdriver)
#define SHAREDRES_VIDEO_DRM_FORMAT  (((PG_RES)mgSharedRes)->video_drm_format)
#define SHAREDRES_VIDEO_DBL_BUFF    (((PG_RES)mgSharedRes)->video_dbl_buff)
#endif

#define SHAREDRES_TIMER_COUNTER (((PG_RES)mgSharedRes)->timer_counter)
#define SHAREDRES_TICK_ON_LOCKSEM  (((PG_RES)mgSharedRes)->tick_on_locksem)
#define SHAREDRES_TIMEOUT       (((PG_RES)mgSharedRes)->timeout)
#define SHAREDRES_TERMIOS       (((PG_RES)mgSharedRes)->savedtermio)

#ifdef _MGRM_PROCESSES
#   define SHAREDRES_MOUSEMOVECLIENT (((PG_RES)mgSharedRes)->mouse_move_client)
#   define SHAREDRES_MOUSEMOVESERIAL (((PG_RES)mgSharedRes)->mouse_move_serial)
#endif

#define SHAREDRES_MOUSEX        (((PG_RES)mgSharedRes)->mousex)
#define SHAREDRES_MOUSEY        (((PG_RES)mgSharedRes)->mousey)
#define SHAREDRES_BUTTON        (((PG_RES)mgSharedRes)->mousebutton)
#define SHAREDRES_SHIFTSTATUS   (((PG_RES)mgSharedRes)->shiftstatus)
#define SHAREDRES_KBDSTATE      (((PG_RES)mgSharedRes)->kbd_state)
#define SHAREDRES_SEMID         (((PG_RES)mgSharedRes)->semid)
#define SHAREDRES_SHMID         (((PG_RES)mgSharedRes)->shmid)
#define SHAREDRES_TOPMOST_LAYER (((PG_RES)mgSharedRes)->topmost_layer)

#define SHAREDRES_NR_LAYSERS    (((PG_RES)mgSharedRes)->nr_layers)
#define SHAREDRES_SEMID_LAYER   (((PG_RES)mgSharedRes)->semid_layer)

#ifdef _MGSCHEMA_COMPOSITING
# define SHAREDRES_SEMID_SHARED_SURF (((PG_RES)mgSharedRes)->semid_shared_surf)
#endif

#define SHAREDRES_NR_GLOBALS        (((PG_RES)mgSharedRes)->nr_globals)
#define SHAREDRES_DEF_NR_TOPMOSTS   (((PG_RES)mgSharedRes)->def_nr_topmosts)
#define SHAREDRES_DEF_NR_NORMALS    (((PG_RES)mgSharedRes)->dev_nr_normals)

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // _MGRM_PROCESSES

#endif // GUI_SHAREDRES_H

