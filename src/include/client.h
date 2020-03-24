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
/*
** client.h: routines for client.
**
** Create date: 2000/12/xx
*/

#ifndef GUI_CLIENT_H
    #define GUI_CLIENT_H

#define CL_PATH "/var/tmp/"

#define REQMASK_FLAGS           0xFF00
#define REQMASK_JOINLAYERFIRST  0x1000

#define REQID_LOADCURSOR        0x0001
#define REQID_CREATECURSOR      0x0002
#define REQID_DESTROYCURSOR     0x0003
#define REQID_CLIPCURSOR        0x0004
#define REQID_GETCLIPCURSOR     0x0005
#define REQID_SETCURSOR         0x0006
#define REQID_GETCURRENTCURSOR  0x0007
#define REQID_SHOWCURSOR        0x0008
#define REQID_SETCURSORPOS      0x0009
#define REQID_LAYERINFO         0x000A
#define REQID_JOINLAYER         0x000B
#define REQID_LAYEROP           0x100C
#define REQID_ZORDEROP          0x100D
#define REQID_IAMLIVE           0x000E
#define REQID_OPENIMEWND        0x100F
#define REQID_SETIMESTAT        0x1010
#define REQID_GETIMESTAT        0x1011
#define REQID_REGISTERHOOK      0x1012

#define REQID_HWSURFACE         0x0013

#ifdef _MGHAVE_CLIPBOARD
#define REQID_CLIPBOARD         0x0014
#endif

#ifdef _MGGAL_MLSHADOW
#define REQID_MLSHADOW_CLIREQ   0x0015
#endif

#define REQID_ZORDERMASKRECTOP  0x1016

#ifdef _MGGAL_NEXUS
#define REQID_NEXUS_HWSURFACE   0x0017
#endif
#ifdef _MGGAL_SIGMA8654
#define REQID_SIGMA_HWSURFACE   0x0018
#endif

#define REQID_GETIMEPOS         0x1019
#define REQID_SETIMEPOS         0x101A

#define REQID_COPYCURSOR        0x001B

/* Since 5.0.0 */
// Get shared surface
#define REQID_GETSHAREDSURFACE      0x001C
    // for compositing schema
    #define SYSSF_WALLPAPER_PATTER      "syssf-wallpaper-pattern"
    // for sharedfb schema
    #define SYSSF_REAL_SCREEN           "syssf-real-screen"
    #define SYSSF_SHADOW_SCREEN         "syssf-shadow-screen"

// Load cursor from PNG file
#define REQID_LOADCURSOR_PNG        0x001D
// Load cursor from in-memory PNG
#define REQID_LOADCURSOR_PNG_MEM    0x001E
// allocate semaphore for shared surface
#define REQID_ALLOC_SURF_SEM        0x001F
// free semaphore of shared surface
#define REQID_FREE_SURF_SEM         0x0020

// Move the current client to another layer
#define REQID_MOVETOLAYER           0x0021
// Calculate the position a main window with WS_EX_AUTOPOSITION
#define REQID_CALCPOSITION          0x0022

// Authenticate client
#define REQID_AUTHCLIENT            0x0023

#define REQID_SYS_LAST              REQID_AUTHCLIENT

/*
 * XXX: To fellows who need to add a new REQID,
 * please make sure the your new ID is _LESS_ than MAX_SYS_REQID
 * (defined in /include/minigui.h).
 */

/* Make sure the MAX_SYS_REQID is large enough for system requests */
#define MGUI_COMPILE_TIME_ASSERT(name, x)               \
       typedef int MGUI_dummy_ ## name[((x)?1:0) * 2 - 1]

MGUI_COMPILE_TIME_ASSERT(sys_request_id, MAX_SYS_REQID >= REQID_SYS_LAST);

#undef MGUI_COMPILE_TIME_ASSERT

/* Since 5.0.0 */
typedef struct _SharedSurfInfo {
    uint32_t    flags;      // the flags of the surface
    uint32_t    width, height;
    uint32_t    pitch;
    uint32_t    name;       // when use flink name
    uint32_t    drm_format; // DRM pixel format
    size_t      size;       // whole size of the surface
    off_t       offset;     // offset of pixel data
} SHAREDSURFINFO;

typedef struct JoinLayerInfo {
    char layer_name [LEN_LAYER_NAME + 1];
    char client_name [LEN_CLIENT_NAME + 1];

    int max_nr_topmosts;
    int max_nr_normals;
} JOINLAYERINFO;

typedef struct JoinedClientInfo {
    GHANDLE layer;
    int cli_id;
    int zo_shmid;
} JOINEDCLIENTINFO;

typedef struct LayerInfo {
    GHANDLE handle;

    int nr_clients;
    int cli_active;
    BOOL is_topmost;
} LAYERINFO;

#define ID_LAYEROP_DELETE   1
#define ID_LAYEROP_SETTOP   2

typedef struct LayerOpInfo {
    int id_op;
    BOOL handle_name;   /* specify the layer by handle or name */
    union {
        GHANDLE handle;
        char name [LEN_LAYER_NAME + 1];
    } layer;
} LAYEROPINFO;

/* Since 5.0.0 */
typedef struct _MoveToLayerInfo {
    BOOL handle_name;   /* specify the layer by handle or name */
    union {
        GHANDLE handle;
        char name [LEN_LAYER_NAME + 1];
    } layer;
} MOVETOLAYERINFO;

typedef struct _MovedClientInfo {
    GHANDLE layer;
    int zo_shmid;
} MOVEDCLIENTINFO;

#define ID_ZOOP_ALLOC               1
#define ID_ZOOP_FREE                2
#define ID_ZOOP_MOVE2TOP            3
#define ID_ZOOP_SHOW                4
#define ID_ZOOP_HIDE                5
#define ID_ZOOP_MOVEWIN             6
#define ID_ZOOP_SETACTIVE           7
#define ID_ZOOP_SETALWAYSTOP        8   /* since 5.0.0 */
#define ID_ZOOP_SETCOMPOSITING      9   /* since 5.0.0 */

#define ID_ZOOP_START_TRACKMENU     10
#define ID_ZOOP_END_TRACKMENU       11
#define ID_ZOOP_CLOSEMENU           12
#define ID_ZOOP_ENABLEWINDOW        13
#define ID_ZOOP_STARTDRAG           14
#define ID_ZOOP_CANCELDRAG          15
#define ID_ZOOP_CHANGECAPTION       16

#define ID_ZOOP_MASKRECT_SET        17
#define ID_ZOOP_MASKRECT_FREE       18

#ifndef MAX_CAPTION_LEN
 #define MAX_CAPTION_LEN            39 //40
#endif

struct _RECT4MASK;

typedef struct ZorderMaskRectOpInfo
{
    int         id_op;
    int         idx_znode;
    DWORD       flags;
    unsigned int nr_maskrect;
    struct _RECT4MASK*  rc;
} ZORDERMASKRECTOPINFO;

typedef struct ZorderOpInfo
{
    int     id_op;

    int     idx_znode;

    /* reuse for the flag of ID_ZOOP_SETALWAYSTOP */
    DWORD   flags;

    HWND    hwnd;
    HWND    main_win;
    RECT    rc;
    RECT    rcA;

    int     location;
    /* Since 5.0.0: use extra data to pass the caption string
    char    caption[MAX_CAPTION_LEN + 1];
    */

#ifdef _MGSCHEMA_COMPOSITING
    Uint32  surf_flags;
    size_t  surf_size;
    /* the compositing type of ID_ZOOP_SETCOMPOSITING */
    int     ct;
    /* the compositing argument of ID_ZOOP_SETCOMPOSITING */
    DWORD   ct_arg;
#endif
} ZORDEROPINFO;

#define HOOKOP_ID_REGISTER      1
#define HOOKOP_ID_UNREGISTER    2

typedef struct RegHookInfo
{
    int     id_op;
    HWND    hwnd;
    DWORD   flags;
} REGHOOKINFO;

#ifdef _MGGAL_MLSHADOW

#define MLSOP_ID_GET_MASTERINFO   0x00
#define MLSOP_ID_CREATE_SURFACE   0x01
#define MLSOP_ID_GET_SLAVEINFO    0x02
#define MLSOP_ID_SET_SLAVEINFO    0x03
#define MLSOP_ID_DESTROY_SLAVE    0x04

#define DEV_NAME_LEN              23

typedef struct  _MLSHADOW_REPLY_MASTER_INFO {
    int type;
    int shm_id_header;
    char video_dev_name [DEV_NAME_LEN+1];
    unsigned int video_mem_len;
    unsigned int video_mem_offset;
    unsigned int surface_key;
    int ret_value;
    int pitch;
}MLSHADOW_REPLY_MASTER_INFO;


typedef struct _MLSHADOW_REPLY_SURFACE_CREATE {
    int ret_value;
    int shm_id_header;
    int surface_key;
} MLSHADOW_REPLY_SURFACE_CREATE;

typedef struct _MLSHADOW_REPLY_SLAVE_GETINFO {
    BOOL ret_value;
    int offset_x;
    int offset_y;
    int enable;
    DWORD blend_flags;
    gal_pixel color_key;
    int alpha;
    int z_order;
} MLSHADOW_REPLY_SLAVE_GETINFO;

#endif /* defined _MGGAL_MLSHADOW */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifndef _MGUI_MINIGUI_H

typedef struct tagREQUEST {
    int id;
    const void* data;
    size_t len_data;
} REQUEST;

typedef REQUEST* PREQUEST;

int cli_conn (const char* name, char project);

int ClientRequestEx2 (const REQUEST* request,
            const void* ex_data, size_t ex_data_len, int fd_to_send,
            void* result, size_t len_rslt, int* fd_received);

static inline int ClientRequestEx (const REQUEST* request,
            const void* ex_data, int ex_data_len,
            void* result, int len_rslt)
{
    return ClientRequestEx2 (request, NULL, 0, -1, result, len_rslt, NULL);
}

static inline int ClientRequest (const REQUEST* request,
            void* result, int len_rslt)
{
    return ClientRequestEx (request, NULL, 0, result, len_rslt);
}
#endif /* not defined _MGUI_MINIGUI_H */

//void __mg_set_select_timeout (unsigned int usec);
void __mg_start_client_desktop (void);

void __mg_update_window (HWND hwnd, int left, int top, int right, int bottom);

BOOL __mg_client_check_hwnd (HWND hwnd, int cli);
BOOL __mg_client_on_layer_changed (GHANDLE layer_handle, int zi_shmid);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_CLIENT_H


