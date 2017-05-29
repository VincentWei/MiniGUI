/*
** $Id: client.h 12918 2010-05-14 05:37:10Z wanzheng $
**
** client.h: routines for client.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Create date: 2000/12/xx
*/

#ifndef GUI_CLIENT_H
    #define GUI_CLIENT_H

#define CL_PATH "/var/tmp/"

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
#define REQID_LAYEROP           0x000C
#define REQID_ZORDEROP          0x000D
#define REQID_IAMLIVE           0x000E
#define REQID_OPENIMEWND        0x000F
#define REQID_SETIMESTAT        0x0010
#define REQID_GETIMESTAT        0x0011
#define REQID_REGISTERHOOK      0x0012

#define REQID_HWSURFACE         0x0013

#ifdef _MGHAVE_CLIPBOARD
#define REQID_CLIPBOARD         0x0014
#endif

#ifdef _MGGAL_MLSHADOW
#define REQID_MLSHADOW_CLIREQ   0x0015
#endif

#define REQID_ZORDERMASKRECTOP  0x0016

#ifdef _MGGAL_NEXUS
#define REQID_NEXUS_CLIENT_GET_SURFACE  0x0017
#endif
#ifdef _MGGAL_SIGMA8654
#define REQID_SIGMA8654_CLIENT_GET_SURFACE 0x0018
#endif

#define REQID_GETIMEPOS        0x0019
#define REQID_SETIMEPOS        0x001A

#define REQID_COPYCURSOR        0x001B

/*
 * XXX: To fellows who need to add a new REQID, please make sure your new ID _less_ than MAX_SYS_REQID (defined in /include/minigui.h).
 */


typedef struct JoinLayerInfo
{
    char layer_name [LEN_LAYER_NAME + 1];
    char client_name [LEN_CLIENT_NAME + 1];

    int max_nr_topmosts;
    int max_nr_normals;
} JOINLAYERINFO;

typedef struct JoinedClientInfo
{
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

#define ID_ZOOP_ALLOC       1
#define ID_ZOOP_FREE        2
#define ID_ZOOP_MOVE2TOP    3
#define ID_ZOOP_SHOW        4
#define ID_ZOOP_HIDE        5
#define ID_ZOOP_MOVEWIN     6
#define ID_ZOOP_SETACTIVE   7

#define ID_ZOOP_START_TRACKMENU     9
#define ID_ZOOP_END_TRACKMENU       10
#define ID_ZOOP_CLOSEMENU           11
#define ID_ZOOP_ENABLEWINDOW        12
#define ID_ZOOP_STARTDRAG           13
#define ID_ZOOP_CANCELDRAG          14
#define ID_ZOOP_CHANGECAPTION       15

#define ID_ZOOP_MASKRECT_SET        16 
#define ID_ZOOP_MASKRECT_FREE       17 

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
    DWORD   flags;
    HWND    hwnd;
    HWND    main_win;
    RECT    rc;
    RECT    rcA;
    char    caption[MAX_CAPTION_LEN + 1];
} ZORDEROPINFO;

#define ID_REG_KEY          1
#define ID_REG_MOUSE        2

typedef struct RegHookInfo
{
    int     id_op;
    HWND    hwnd;
    DWORD   flag;
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
}MLSHADOW_REPLY_SURFACE_CREATE;

typedef struct _MLSHADOW_REPLY_SLAVE_GETINFO {
    BOOL ret_value;
    int offset_x; 
    int offset_y;
    int enable; 
    DWORD blend_flags;
    gal_pixel color_key;
    int alpha;
    int z_order;
}MLSHADOW_REPLY_SLAVE_GETINFO;

#endif

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
int GUIAPI ClientRequestEx (const REQUEST* request, const void* ex_data, int ex_data_len,
                void* result, int len_rslt);
static inline int ClientRequest (const REQUEST* request,
                void* result, int len_rslt)
{
    return ClientRequestEx (request, NULL, 0, result, len_rslt);
}
#endif

void __mg_set_select_timeout (unsigned int usec);
void __mg_start_client_desktop (void);

void __mg_update_window (HWND hwnd, int left, int top, int right, int bottom);

BOOL __mg_client_check_hwnd (HWND hwnd, int cli);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_CLIENT_H


