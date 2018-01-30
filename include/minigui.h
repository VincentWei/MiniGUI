/**
 * \file minigui.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2002/01/06
 * 
 * \brief This file includes global and miscellaneous interfaces of MiniGUI.
 *
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing 
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
    Copyright (C) 1998~2002, WEI Yongming

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.

 \endverbatim
 */

/*
 * $Id: minigui.h 13674 2010-12-06 06:45:01Z wanzheng $
 * 
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef _MGUI_MINIGUI_H
  #define _MGUI_MINIGUI_H

#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup global_vars Global variables
     * @{
     */

    /**
     * \defgroup rect_vars Global Rectangles
     * @{
     */

/**
 * \var RECT g_rcScr
 * \brief Contains the rectangle of the whole screen.
 */
extern MG_EXPORT RECT g_rcScr;

/**
 * \def g_rcDesktop
 * \brief Contains the rectangle of desktop of the application.
 *
 * \a g_rcDesktop is defined as an alias (macro) of \a g_rcScr.
 *
 * \sa g_rcScr
 */
#define g_rcDesktop     g_rcScr

    /** @} end of rect_vars */

    /**
     * \defgroup lite_vars MiniGUI-Processes specific variables
     * @{
     */

#ifdef _MGRM_PROCESSES

#include <sys/types.h>

/**
 * \var BOOL mgIsServer
 * \brief Indicates whether the process is the server or a client on 
 * MiniGUI-Processes.
 *
 * \note Only defined for MiniGUI-Processes.
 */
extern MG_EXPORT BOOL mgIsServer;

/**
 * \var void* mgSharedRes
 * \brief Contains the pointer to the shared resource of MiniGUI-Processes.
 *
 * \note Not defined for MiniGUI-Threads, and the shared resource is
 * read-only for all clients.
 *
 * \sa mgSizeRes
 */
extern MG_EXPORT void* mgSharedRes;

/**
 * \var void* mgSizeRes
 * \brief Contains the length of shared resource of MiniGUI-Processes.
 *
 * \note Only defined for MiniGUI-Processes. 
 *
 * \sa mgSharedRes
 */
extern MG_EXPORT size_t mgSizeRes;

/**
 * \def LEN_LAYER_NAME
 * \brief The maximum length of name of layer in MiniGUI-Processes.
**/
#define LEN_LAYER_NAME      14

/**
 * \def LEN_CLIENT_NAME
 * \brief The maximum length of name of client in MiniGUI-Processes.
**/
#define LEN_CLIENT_NAME     14

/**
 * \def INV_LAYER_HANDLE
 * \brief Invalid handle value of the layer.
**/
#define INV_LAYER_HANDLE    0

struct _MG_Layer;

/** Client information. */
typedef struct _MG_Client
{
    /** The name of the client. */
    char    name [LEN_CLIENT_NAME + 1];

    /** PID of the client process. */
    pid_t   pid;
    /** UID of the client process. */
    uid_t   uid;
    /** The file descriptor of the socket connected to the client. */
    int     fd;

    /** Flag indicate whether this client has dirty windows. */
    BOOL    has_dirty;
    /** The last active tick count of the client. */
    DWORD   last_live_time;
    /** The additional data of the client. */
    DWORD   dwAddData;

    /** The pointer to the next client in the same layer. */
    struct  _MG_Client* next;
    /** The pointer to the previous client in the same layer. */
    struct  _MG_Client* prev;

    /** The pointer to the layer on which the client lays. */
    struct  _MG_Layer* layer;

    /** The pointer to the global resoures of the client. */
    struct GlobalRes* global_res;
} MG_Client;

/** Layer information. */
typedef struct _MG_Layer
{
    /** The name of the layer. */
    char    name [LEN_LAYER_NAME + 1];

    /** The pointer to the active client on the layer. */
    DWORD   dwAddData;

    /** The pointer to the list of clients in the layer. */
    MG_Client* cli_head;
    /** The pointer to the active client on the layer. */
    MG_Client* cli_active;

    /** The pointer to the next layer. */
    struct _MG_Layer* next;
    /** The pointer to the previous layer. */
    struct _MG_Layer* prev;

    /** Internal field. */
    void* zorder_info;
    /** Internal field. */
    int   zorder_shmid;
} MG_Layer;

/*screen attr type*/
#define SCREEN_ATTR_ALPHA_CHANNEL    0x01      //alpha channel     
#define SCREEN_ATTR_COLORKEY         0x02      //colorkey
#define SCREEN_ATTR_COLORSPACE       0x03      //colorspace
#define SCREEN_ATTR_ALPHA            0x04
#define SCREEN_NO_EXIST              -99       //screen don't exist

/**
 * \var int mgClientSize
 * \brief The current size of the array \a mgClients.
 *
 * \sa mgClients
 */
extern MG_EXPORT int mgClientSize;

/**
 * \var MG_Client* mgClients
 * \brief The pointer to the array contains all clients' information.
 *
 * You can access the elements in \a mgClients as a normal array. If the 
 * field \a fd of one element is not less than zero, then the element
 * will be a vaild client.
 *
 * \sa MG_Client
 */
extern MG_EXPORT MG_Client* mgClients;

/**
 * \var MG_Layer* mgTopmostLayer
 * \brief The pointer to the topmost layer.
 *
 * \sa MG_Layer
 */
extern MG_EXPORT MG_Layer* mgTopmostLayer;

/**
 * \var MG_Layer* mgLayers
 * \brief The pointer to the array of layers.
 *
 * \sa MG_Layer
 */
extern MG_EXPORT MG_Layer* mgLayers;

#endif /* _MGRM_PROCESSES */

    /** @} end of lite_vars */

    /** @} end of global_vars */


/** 
* \fn int GUIAPI InitGUI (int, const char **)
* \brief Initialize MiniGUI.
*
* The meaning of two parameters is same with parameters of main function.
*
**/
MG_EXPORT int GUIAPI InitGUI (int, const char **);

/** 
* \fn void GUIAPI TerminateGUI (int not_used)
* \brief Terminate MiniGUI.
*
* \param not_used not used
*
**/
MG_EXPORT void GUIAPI TerminateGUI (int not_used);

/** 
* \fn void GUIAPI MiniGUIPanic (int exitcode)
* \brief The panic of MiniGUI application.
*
* The function forces to close GAL and IAL engine.
*
* \param exitcode The value of exitcode, now it can be any values.
*
**/
MG_EXPORT void GUIAPI MiniGUIPanic (int exitcode);

    /**
     * \addtogroup fns Functions
     * @{
     */

    /**
     * \addtogroup global_fns Global/general functions
     * @{
     */

    /**
     * \defgroup lite_fns MiniGUI-Processes specific functions
     * @{
     */

#ifndef _MGRM_THREADS

    /**
     * \defgroup lite_listenfd_fns Listening a file descriptor
     *
     * Register/Unregister a listen fd to MiniGUI.
     *
     * When you need to listen a file descriptor, you can use \a select(2)
     * system call. In MiniGUI, you can also register it to MiniGUI to 
     * be a listened fd, and when there is a read/write/except event on 
     * the registered fd , MiniGUI will sent a notification message to 
     * the registered window.
     *
     * Example:
     *
     * \include listenfd.c
     *
     * @{
     */

/**
* \def MAX_NR_LISTEN_FD
* \brief The max number of listen fd which user can use.
**/
#define MAX_NR_LISTEN_FD   5

#ifdef WIN32
#ifndef POLLIN
#define POLLIN  0x001
#endif

#ifndef POLLOUT
#define POLLOUT 0x004
#endif

#ifndef POLLERR
#define POLLERR 0x008
#endif

#endif

/** 
 * \fn BOOL GUIAPI RegisterListenFD (int fd, int type,\
                HWND hwnd, void* context)
 * \brief Registers a listening file descriptor to MiniGUI-Lite.
 *
 * This function registers the file desciptor \a fd to MiniGUI-Lite for 
 * listening.
 *
 * When there is a read/write/except event on this \a fd, MiniGUI
 * will post a MSG_FDEVENT message with wParam being equal to
 * MAKELONG (fd, type), and the lParam being set to \a context
 * to the target window \a hwnd.
 *
 * \param fd The file descriptor to be listened.
 * \param type The type of the event to be listened, can be POLLIN, POLLOUT, 
 *        or POLLERR.
 * \param hwnd The handle to the window will receive MSG_FDEVENT message.
 * \param context The value will be passed to the window as lParam of 
 *        MSG_FDEVENT message.
 *
 * \return TRUE if all OK, and FALSE on error.
 *
 * \note Only available on MiniGUI-Processes.
 *
 * \sa UnregisterListenFD, system_msgs
 */
MG_EXPORT BOOL GUIAPI RegisterListenFD (int fd, int type,
                HWND hwnd, void* context);

/** 
 * \fn BOOL GUIAPI UnregisterListenFD (int fd)
 * \brief Unregisters a being listened file descriptor.
 *
 * This function unregisters the being listened file descriptor \a fd.
 *
 * \param fd The file descriptor to be unregistered, should be a being 
 *        listened file descriptor.
 * \return TRUE if all OK, and FALSE on error.
 *
 * \note Only available on MiniGUI-Processes.
 *
 * \sa RegisterListenFD
 */
MG_EXPORT BOOL GUIAPI UnregisterListenFD (int fd);

    /** @} end of lite_listenfd_fns */

#ifdef _MGRM_PROCESSES

    /**
     * \defgroup lite_layer_fns Layer operations
     *
     * A client in MiniGUI-Processes can create a new layer or join 
     * an existed layer.
     *
     * Example:
     *
     * \include client_startup.c
     *
     * @{
     */

/**
* \def NAME_SELF_LAYER
* \brief The name of the self layer.
**/
#define NAME_SELF_LAYER         ""

/**
* \def NAME_TOPMOST_LAYER
* \brief The name of the topmost layer.
**/
#define NAME_TOPMOST_LAYER      ""

/**
* \def NAME_DEF_LAYER
* \brief The default name of the layer.
**/
#define NAME_DEF_LAYER          "mginit"

/** 
 * \fn GHANDLE GUIAPI JoinLayer (const char* layer_name,\
                const char* client_name,\
                int max_nr_topmosts, int max_nr_normals)
 * \brief Joins to a layer.
 *
 * This function should be called by clients before calling any other MiniGUI
 * functions. You can call \a GetLayerInfo to get the layer information.
 * If the layer to be joined does not exist, the server, i.e. \a mginit, will
 * try to create a new one. If you passed a NULL pointer or a null string for 
 * \a layer_name, the client will join to the default layer.
 *
 * If the client want to create a new layer, you should specify the maximal 
 * number of topmost frame objects (max_nr_topmosts) and the maximal number of 
 * normal frame objects (max_nr_normals) in the new layer. Passing zero to 
 * \a max_nr_topmosts and max_nr_normals will use the default values, and the default 
 * values are specified by ServerStartup.
 *
 * Note that the server will create a default layer named "mginit". 
 *
 * \param layer_name The name of the layer.You can use NAME_TOPMOST_LAYER to
 *        specify the current topmost layer.
 * \param client_name The name of the client.
 * \param max_nr_topmosts The maximal number of topmost z-order nodes in 
 *        the new layer.
 * \param max_nr_normals The maximal number of normal z-order nodes in
 *        the new layer.
 *
 * \return The handle to the layer on success, INV_LAYER_HANDLE on error.
 *
 * \note Only call this function in clients of MiniGUI-Processes.
 *
 * \sa GetLayerInfo, ServerStartup, ServerCreateLayer
 */
MG_EXPORT GHANDLE GUIAPI JoinLayer (const char* layer_name,
                const char* client_name,
                int max_nr_topmosts, int max_nr_normals);

/** 
 * \fn GHANDLE GUIAPI GetLayerInfo (const char* layer_name,\
                int* nr_clients, BOOL* is_topmost, int* cli_active)
 * \brief Gets information of a layer by a client.
 *
 * You can get the information of a layer through this function. 
 * The information will be returned through the pointer arguments 
 * if the specific pointer is not NULL.
 *
 * \param layer_name The name of the layer. You can use NAME_SELF_LAYER to
 *        specify the layer the calling client belongs to.
 * \param nr_clients The number of clients in the layer will be returned 
 *        through this pointer.
 * \param is_topmost A boolean which indicates whether the layer is the 
 *        topmost layer will be returned.
 * \param cli_active The identifier of the active client in the layer.
 *
 * \return Returns the handle to the layer on success, 
 *        INV_LAYER_HANDLE on error.
 *
 * \note Only call this function in clients of MiniGUI-Processes.
 *
 * \sa JoinLayer
 */
MG_EXPORT GHANDLE GUIAPI GetLayerInfo (const char* layer_name,
                int* nr_clients, BOOL* is_topmost, int* cli_active);

/** 
 * \fn BOOL GUIAPI SetTopmostLayer (BOOL handle_name,\
 *                 GHANDLE handle, const char* name)
 * \brief Brings a layer to be the topmost one.
 *
 * This function brings the specified layer \a handle to be the topmost layer.
 *
 * \param handle_name The way specifing the layer; TRUE for handle of 
 *        the layer, FALSE for name.
 * \param handle The handle to the layer.
 * \param name The name of the layer. You can use NAME_SELF_LAYER to
 *        specify the layer to which the calling client belongs.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Only call this function in clients of MiniGUI-Processes.
 */
MG_EXPORT BOOL GUIAPI SetTopmostLayer (BOOL handle_name,
                GHANDLE handle, const char* name);

/** 
 * \fn BOOL GUIAPI DeleteLayer (BOOL handle_name,\
                        GHANDLE handle, const char* layer_name)
 * \brief Deletes a specific layer.
 *
 * \param handle_name The way specifing the layer; TRUE for handle of 
 *        the layer, FALSE for name.
 * \param handle The handle to the layer.
 * \param layer_name The name of the layer. You can use NAME_SELF_LAYER to
 *        specify the layer to which the calling client belongs.
 *
 * \return TRUE for success, FALSE on error.
 *
 * \note Only call this function in clients of MiniGUI-Processes.
 *
 * \sa JoinLayer
 */
MG_EXPORT BOOL GUIAPI DeleteLayer (BOOL handle_name,
                        GHANDLE handle, const char* layer_name);

    /** @} end of lite_layer_fns */

    /**
     * \defgroup lite_server_fns Server-only operations
     *
     * MiniGUI provides some server-only functions for you to create a
     * customized server for MiniGUI-Processes, i.e. \a mginit.
     *
     * Example:
     *
     * \include server_startup.c
     *
     * @{
     */

/**
 * \var typedef void (* ON_LOCK_CLIENT_REQ) (void)
 * \brief Type of client request lock callback.
 *
 * \sa OnTrylockClientReq, OnLockClientReq, OnUnlockClientReq
 */
typedef int (* ON_LOCK_CLIENT_REQ) (void);


/**
 * \var typedef void (* ON_TRYLOCK_CLIENT_REQ) (void)
 * \brief Type of client request lock callback.
 *
 * \sa OnTrylockClientReq, OnLockClientReq, OnUnlockClientReq
 */
typedef int (* ON_TRYLOCK_CLIENT_REQ) (void);

/**
 * \var typedef void (* ON_UNLOCK_CLIENT_REQ) (void)
 * \brief Type of client request unlock callback.
 *
 * \sa OnTrylockClientReq, OnLockClientReq, OnUnlockClientReq
 */
typedef void (* ON_UNLOCK_CLIENT_REQ) (void);

/**
 * \var ON_LOCK_CLIENT_REQ OnLockClientReq
 * \brief Sets to a function to lock a client request.
 *
 * \note Only available for the client of MiniGUI-Processes.
 * 
 * \sa ON_LOCK_CLIENT_REQ
 */
extern MG_EXPORT ON_LOCK_CLIENT_REQ  OnLockClientReq;

/**
 * \var ON_TRYLOCK_CLIENT_REQ OnTrylockClientReq
 * \brief Sets to a function to lock a client request.
 *
 * \note Only available for the client of MiniGUI-Processes.
 * 
 * \sa ON_TRYLOCK_CLIENT_REQ
 */
extern MG_EXPORT ON_TRYLOCK_CLIENT_REQ  OnTrylockClientReq;
/**
 * \var ON_UNLOCK_CLIENT_REQ OnUnlockClientReq
 * \brief Sets to a function to unlock a client request.
 *
 * \note Only available for the client of MiniGUI-Processes.
 * 
 * \sa ON_UNLOCK_CLIENT_REQ
 */
extern MG_EXPORT ON_UNLOCK_CLIENT_REQ  OnUnlockClientReq;

#define LCO_NEW_CLIENT      1
#define LCO_DEL_CLIENT      2

/**
 * \var typedef void (* ON_NEW_DEL_CLIENT) (int op, int cli)
 * \brief Type of client event callback.
 *
 * \sa OnNewDelClient, OnChangeLayer
 */
typedef void (* ON_NEW_DEL_CLIENT) (int op, int cli);

#define LCO_NEW_LAYER       1
#define LCO_DEL_LAYER       2
#define LCO_JOIN_CLIENT     3
#define LCO_REMOVE_CLIENT   4
#define LCO_TOPMOST_CHANGED 5
#define LCO_ACTIVE_CHANGED  6

/**
 * \var typedef void (* ON_CHANGE_LAYER) (int op, 
                MG_Layer* layer, MG_Client* client)
 * \brief Type of layer change event callback.
 *
 * \sa OnNewDelClient, OnChangeLayer
 */
typedef void (* ON_CHANGE_LAYER) (int op, MG_Layer* layer, 
                MG_Client* client);

#define ZNOP_ALLOCATE   1
#define ZNOP_FREE       2
#define ZNOP_MOVE2TOP   3
#define ZNOP_SHOW       4
#define ZNOP_HIDE       5
#define ZNOP_MOVEWIN    6
#define ZNOP_SETACTIVE  7

#define ZNOP_ENABLEWINDOW        11
#define ZNOP_DISABLEWINDOW       12
#define ZNOP_STARTDRAG           13
#define ZNOP_CANCELDRAG          14
#define ZNOP_CHANGECAPTION       15

/**
 * \var typedef void (* ON_ZNODE_OPERATION) (int op, int cli, int idx_znode)
 * \brief Type of z-node operation callback.
 *
 * \sa OnNewDelClient, OnChangeLayer, OnZNodeOperation
 */
typedef void (* ON_ZNODE_OPERATION) (int op, int cli, int idx_znode);

/**
 * \var ON_NEW_DEL_CLIENT OnNewDelClient
 * \brief Sets to a function to handle a comming in (going away) 
 *        connection of client.
 *
 * When a client is connecting to or disconnecting from the server, MiniGUI
 * will call this function to tell you the event and the client identifier. 
 * The event could be one of the following:
 *
 *  - LCO_NEW_CLIENT\n
 *    A new client is connecting to the server.
 *  - LCO_DEL_CLIENT\n
 *    A new client is disconnecting from the server.
 *
 * The event will be passed through the argument of \a op, and the client
 * identifier will be passed through the argument of \a cli.
 * You can get the information of the client by accessing \a mgClients 
 * with \a cli.
 *
 * \note Only available for the server of MiniGUI-Processes.
 * 
 * \sa ON_NEW_DEL_CLIENT, mgClients
 */
extern MG_EXPORT ON_NEW_DEL_CLIENT OnNewDelClient;

/**
 * \var ON_CHANGE_LAYER OnChangeLayer
 * \brief Sets to a function to handle events of layers.
 *
 * When a layer is changing, MiniGUI will call this function to tell 
 * you the event and the layer or the client which leads to the event.
 * The event could be one of the following:
 *
 *  - LCO_NEW_LAYER\n
 *    A new layer is creating.
 *  - LCO_DEL_LAYER\n
 *    A new layer is deleting.
 *  - LCO_JOIN_CLIENT\n
 *    A client is joining to the layer.
 *  - LCO_REMOVE_CLIENT\n
 *    A client is removing from the layer.
 *  - LCO_TOPMOST_CHANGED\n
 *    The topmost layer changed, the layer will be the topmost one.
 *  - LCO_ACTIVE_CHANGED\n
 *    The active client changed, the client will be the active one.
 *
 * The event will be passed through the argument of \a op, and the 
 * pointers to the relevant layer and client will be passed through 
 * the argument of \a layer and \a client respectively.
 *
 * \note Only available for the server of MiniGUI-Processes.
 * 
 * \sa ON_NEW_DEL_CLIENT, mgClients
 */
extern MG_EXPORT ON_CHANGE_LAYER OnChangeLayer;

/**
 * \var ON_ZNODE_OPERATION OnZNodeOperation
 * \brief Sets to a function to handle events of z-node.
 *
 * After the server does an operation on a znode, MiniGUI will call 
 * this function to tell you the event and the layer, the client, and 
 * the z-node which leads to the event.
 *
 * The event could be one of the following:
 *
 *  - ZNOP_ALLOCATE\n
 *    The z-node has been created.
 *  - ZNOP_FREE\n
 *    The z-node has been destroyed.
 *  - ZNOP_MOVE2TOP\n
 *    The z-node has been moved to be the topmost one.
 *  - ZNOP_SHOW\n
 *    The z-node has been shown.
 *  - ZNOP_HIDE\n
 *    The z-node has been hidden.
 *  - ZNOP_MOVEWIN\n
 *    The z-node has been moved or its size has changed.
 *  - ZNOP_SETACTIVE\n
 *    The z-node has been set to be the active one.
 *  - ZNOP_ENABLEWINDOW\n
 *    The z-node is disabled or enabled.
 *  - ZNOP_STARTDRAG\n
 *    Start to drag the z-node.
 *  - ZNOP_CANCELDRAG\n
 *    Cancel to drag the z-node.
 *  - ZNOP_CHANGECAPTION\n
 *    The caption of the z-node has changed.
 *
 * The event will be passed through the argument of \a op; the 
 * pointers to the layer, the identifier of the client, and the index of 
 * the z-node will be passed through the argument of \a layer, \a cli, 
 * and \a idx_znode respectively.
 *
 * \note Only available for the server of MiniGUI-Processes.
 * 
 * \sa ON_ZNODE_OPERATION, ServerGetZNodeInfo, mgClients
 */
extern MG_EXPORT ON_ZNODE_OPERATION OnZNodeOperation;

/** 
 * \fn BOOL GUIAPI ServerStartup (int nr_globals,\
                int def_nr_topmosts, int def_nr_normals)
 * \brief Initializes the server of MiniGUI-Processes.
 *
 * This function initializes the server, i.e. \a mginit. It creates the 
 * shared resource, the listening socket, the default layer, and other 
 * internal objects. Your costomized \a mginit program should call 
 * this function before calling any other function.
 *
 * Note that the default layer created by the server called 
 * "mginit" (NAME_DEF_LAYER).
 *
 * \param nr_globals The number of the global z-order nodes. All z-order nodes
 *        created by mginit are global ones.
 * \param def_nr_topmosts The maximal number of the topmost z-order nodes in 
 *        the default layer. It is also the default number of topmost 
 *        z-order nodes of a new layer.
 * \param def_nr_normals The maximal number of normal global z-order nodes in
 *        the new layer. It is also the default number of normal
 *        z-order nodes of a new layer.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Server-only function, i.e. \em only can be called by \a mginit.
 */
MG_EXPORT BOOL GUIAPI ServerStartup (int nr_globals,
                int def_nr_topmosts, int def_nr_normals);

/** 
 * \fn MG_Layer* GUIAPI ServerCreateLayer (const char* layer_name,\
                int max_nr_topmosts, int max_nr_normals)
 * \brief Create a new layer from the server.
 *
 * This function creates a new layer named by \a layer_name.
 * You should specify the maximal number of topmost frame 
 * objects (max_nr_topmosts) and the maximal number of normal frame 
 * objects (max_nr_normals) in the new layer. Passing zero to 
 * max_nr_topmosts and max_nr_normals will use the default values, 
 * and the default values are specified by ServerStartup.
 *
 * Note that the server will create a default layer named "mginit". 
 *
 * \param layer_name The name of the layer. If there is already a layer 
 *        named \a layer_name, the function will return the pointer to the layer.
 * \param max_nr_topmosts The maximal number of topmost z-order nodes in 
 *        the new layer.
 * \param max_nr_normals The maximal number of normal z-order nodes in
 *        the new layer.
 *
 * \return The handle to the layer on success, NULL on error.
 *
 * \note Only call this function in the server of MiniGUI-Processes.
 *
 * \sa ServerDeleteLayer, ServerStartup
 */
MG_EXPORT MG_Layer* GUIAPI ServerCreateLayer (const char* layer_name, 
                int max_nr_topmosts, int max_nr_normals);

/** 
 * \fn BOOL GUIAPI ServerSetTopmostLayer (MG_Layer* layer)
 * \brief Sets topmost layer from the server.
 *
 * This functions sets the specified layer \a layer to be the topmost layer.
 *
 * \param layer The pointer to the layer.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Server-only function.
 *
 * \sa SetTopmostClient, SetTopmostLayer
 */
MG_EXPORT BOOL GUIAPI ServerSetTopmostLayer (MG_Layer* layer);

/** 
 * \fn BOOL GUIAPI ServerDeleteLayer (MG_Layer* layer)
 * \brief Delete a layer from the server.
 *
 * This functions deletes the specified layer \a layer.
 *
 * \param layer The pointer to the layer.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Server-only function.
 *
 * \sa ServerCreateLayer, JoinLayer, DeleteLayer
 */
MG_EXPORT BOOL GUIAPI ServerDeleteLayer (MG_Layer* layer);

/** 
 * \fn int GUIAPI ServerGetNextZNode (MG_Layer* layer, int idx_znode, \
 *              int* cli)
 * \brief Get the next z-node in the specified layer from the server.
 *
 * This functions gets the next z-node in the specified layer \a layer from 
 * the server.
 *
 * \param layer The pointer to the layer, NULL for the current topmost layer.
 * \param idx_znode The initial z-node. If the initial z-node index is 
 *        less than or equal to zero, the function will return 
 *        the index of the first z-node in the layer. 
 * \param cli The client identifier of the next znode will be returned 
 *        through this pointer. NULL is okay.
 *
 * \return The index of the next z-node. Zero when the next z-node is 
 *         the last one; < 0 when error;
 *
 * \note Server-only function. Note that this function will not return 
 *       the z-node of the desktop, and the desktop always has the index 
 *       of z-node zero.
 *
 * \sa ServerGetZNodeInfo
 */
MG_EXPORT int GUIAPI ServerGetNextZNode (MG_Layer* layer, int idx_znode, 
                int* cli);

#define ZNIT_DESKTOP         0x50000000

#define ZNIT_GLOBAL_MAINWIN  0x31000000
#define ZNIT_GLOBAL_TOOLWIN  0x32000000
#define ZNIT_GLOBAL_CONTROL  0x30000000

#define ZNIT_TOPMOST_MAINWIN 0x21000000
#define ZNIT_TOPMOST_TOOLWIN 0x22000000
#define ZNIT_TOPMOST_CONTROL 0x20000000

#define ZNIT_NORMAL_MAINWIN  0x11000000
#define ZNIT_NORMAL_TOOLWIN  0x12000000
#define ZNIT_NORMAL_CONTROL  0x10000000

#define ZNIT_NULL            0x00000000

#define ZNIF_VISIBLE         0x00000002
#define ZNIF_DISABLED        0x00000004

/** Z-node information structure */
typedef struct _ZNODEINFO
{
    /**
     * The type of the znode, can be one of the following values:
     *      - ZNIT_GLOBAL_MAINWIN\n 
     *        a global main window.
     *      - ZNIT_GLOBAL_TOOLWIN\n
     *        a global tool window.
     *      - ZNIT_GLOBAL_CONTROL\n 
     *        a global control with WS_EX_CTRLASMAINWIN style.
     *      - ZNIT_TOPMOST_MAINWIN\n
     *        a topmost main window.
     *      - ZNIT_TOPMOST_TOOLWIN\n
     *        a topmost tool window.
     *      - ZNIT_TOPMOST_CONTROL\n
     *        a topmost control with WS_EX_CTRLASMAINWIN style.
     *      - ZNIT_NORMAL_MAINWIN\n
     *        a normal main window.
     *      - ZNIT_NORMAL_TOOLWIN\n
     *        a normal tool window.
     *      - ZNIT_NORMAL_CONTROL\n
     *        a normal control with WS_EX_CTRLASMAINWIN style.
     *      - ZNIT_DESKTOP\n
     *        the desktop.
     *      - ZNIT_NULL\n
     *        a null and not-used z-node which does not refer to any window.
     */
    DWORD           type;

    /** 
     * The flags of the znode, can be OR'd with the following values:
     *      - ZNIF_VISIBLE\n
     *        a visible window.
     *      - ZNIF_DISABLED\n
     *        a disabled window.
     *  Note that the flags are only applied to window.
     */
    DWORD           flags;

    /** The pointer to the caption string of the znode if it is a window. */
    const char*     caption;
    /** The rectangle of the znode in the screen. */
    RECT            rc;

    /** Client id of the znode. */
    int             cli;
    /** The window handle of the znode if it is a window. */
    HWND            hwnd;
    /**
     * The window handle of the znode's main window if it is a control 
     * with WS_EX_CTRLASMAINWIN style.
     */
    HWND            main_win;
} ZNODEINFO;

/** 
 * \fn BOOL GUIAPI ServerGetZNodeInfo (MG_Layer* layer, int idx_znode, \
 *              ZNODEINFO* znode_info)
 * \brief Get the z-node information in the specified layer from the server.
 *
 * This functions gets the z-node information in the specified layer \a layer 
 * from the server.
 *
 * \param layer The pointer to the layer, NULL for the current topmost layer.
 * \param idx_znode The index of the znode.
 * \param znode_info The information of the requested znode will be returned 
 *        through this structure.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Server-only function.
 *
 * \sa ServerGetNextZNode, ZNODEINFO
 */
MG_EXPORT BOOL GUIAPI ServerGetZNodeInfo (MG_Layer* layer, int idx_znode, 
                ZNODEINFO* znode_info);

/** 
 * \fn BOOL GUIAPI ServerDoZNodeOperation (MG_Layer* layer, int idx_znode, \
 *              int op_code, void* op_data, BOOL notify)
 * \brief Does an operation on the z-node in the specified layer 
 *        from the server.
 *
 * This functions does an operation upon the z-node in the specified 
 * layer \a layer from the server.
 *
 * \param layer The pointer to the layer, NULL for the current topmost layer.
 * \param idx_znode The index of the znode.
 * \param op_code The code of the operation, can be one of the following 
 *        values:
 *          - ZNOP_MOVE2TOP\n
 *            Move the znode to be the topmost one.
 *          - ZNOP_SETACTIVE\n
 *            Set the znode to be the active one.
 *       Note that the operation can be applied only for a main window.
 * \param op_data The data of the operation, used to pass the data need by
 *        the operation. For example, if the operation is moving the z-node, 
 *        \a op_data will be a pointer to a RECT structure, which contains
 *        the new position and size of the znode. Not used currently, reserved
 *        for future use.
 * \param notify Whether to notify the client about the change of the znode.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Server-only function, and the operation can be applied only for 
 *       a main window.
 *
 * \sa ServerGetZNodeInfo
 */
MG_EXPORT BOOL GUIAPI ServerDoZNodeOperation (MG_Layer* layer, int idx_znode, 
                int op_code, void* op_data, BOOL notify);

/** 
 * \fn int GUIAPI GetClientByPID (int pid)
 * \brief Returns the client identifier from PID of a client.
 *
 * This function gets the identifier of the sepcified client from the PID of it.
 *
 * \param pid The process ID of the client.
 * \return The client identifier on success, less than 0 on error.
 *
 * \note Server-only function.
 */
MG_EXPORT int GUIAPI GetClientByPID (int pid);

/** 
 * \fn BOOL GUIAPI SetTopmostClient (int cli)
 * \brief Sets topmost layer by a client identifier.
 *
 * This function sets the topmost layer by the specified client 
 * identifier \a cli. It will bring the layer contains the client 
 * to be the topmost one.
 *
 * \param cli The identifier of the client.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Server-only function.
 *
 * \sa ServerSetTopmostLayer, SetTopmostLayer
 */
MG_EXPORT BOOL GUIAPI SetTopmostClient (int cli);

/**
 * \fn void GUIAPI DisableClientsOutput (void)
 * \brief Disable all clients output.
 */
MG_EXPORT void GUIAPI DisableClientsOutput (void);

/**
 * \fn void GUIAPI UpdateTopmostLayer (const RECT* dirty_rc)
 * \brief Update topmost layer.
 *
 * \param dirty_rc The refresh region.
 */
MG_EXPORT void GUIAPI UpdateTopmostLayer (const RECT* dirty_rc);

    /** @} end of lite_server_fns */

    /**
     * \defgroup lite_request_fns Simple request/reply interfaces
     * 
     * You can register a customized request handler to extend your server, 
     * i.e. mginit, of MiniGUI-Processes.
     *
     * A request consists of an identifier and the data associated with 
     * the request. The identifier is used by MiniGUI to determine which 
     * handler should be called when a request arrives. When MiniGUI 
     * finds one handler, it will call the handler and pass the socket 
     * fd connected to the client, the data associated with the request,
     * and the length of the data. Eventually, the handler will sent the 
     * reply to the client.
     *
     * After register a customized request handler in your server, you can call 
     * \a ClientRequest function in the client to send a request to the server 
     * and wait for the reply. On the other hand, the request handler in the 
     * server will receive the request and call \a ServerSendReply to send the 
     * reply to the client. In this way, you can create a simple IPC 
     * (inter-process conmmunication) mechanism between clients and the server.
     *
     * Example:
     *
     * \include request.c
     *
     * @{
     */

/**
 * \def MAX_SYS_REQID
 * \brief Maximal system reserved request identifier.
 *
 * \sa RegisterRequestHandler
 */
#define MAX_SYS_REQID           0x0020

/**
 * \def MAX_REQID
 * \brief Maximal request identifier.
 *
 * \sa RegisterRequestHandler
 */
#define MAX_REQID               0x0030

/** A request will be sent to the server of MiniGUI-Processes. */
typedef struct _REQUEST {
    /** The identifier of the type of the request. */
    int id;
    /** The data will be sent to the server. */
    const void* data;
    /** The length of the data. */
    size_t len_data;
} REQUEST;
/** Data type of pointer to a REQUEST */
typedef REQUEST* PREQUEST;

/** 
 * \fn int ClientRequestEx (const REQUEST* request, const void* ex_data, 
                int ex_data_len, void* result, int len_rslt)
 * \brief Sends a request to the server and wait reply.
 *
 * If \a result is NULL or \a len_rslt is zero, the function will return 
 * immediately after sent the data to the server.
 *
 * \param request The pointer to REQUEST, which contains the data of 
 *        the request.
 * \param ex_data The pointer to extra data to be sent to the server.
 * \param ex_data_len The length of the extra data in bytes.
 * \param result The buffer receives the reply.
 * \param len_rslt The lenght of the buffer.
 *
 * \return Zero on success, no-zero on error.
 *
 * \note Only used by clients to send a request to the server of 
 *       MiniGUI-Processes.
 *
 * \sa ServerSendReply
 */
MG_EXPORT int GUIAPI ClientRequestEx (const REQUEST* request, const void* ex_data, 
                int ex_data_len, void* result, int len_rslt);

/** 
 * \fn int ClientRequest (const REQUEST* request, void* result, int len_rslt)
 * \brief Sends a request to the server and wait reply.
 *
 * If \a result is NULL or \a len_rslt is zero, the function will return 
 * immediately after sent the data to the server.
 *
 * This function is a simplified version of ClientRequestEx, i.e.
 * there is no extra data to be sent.
 *
 * \param request The pointer to REQUEST, which contains the data of 
 *        the request.
 * \param result The buffer receives the reply.
 * \param len_rslt The lenght of the buffer.
 *
 * \return Zero on success, no-zero on error.
 *
 * \note Only used by clients to send a request to the server of 
 *       MiniGUI-Processes.
 *
 * \sa ClientRequestEx, ServerSendReply
 */
static inline int ClientRequest (const REQUEST* request, 
                void* result, int len_rslt)
{
    return ClientRequestEx (request, NULL, 0, result, len_rslt);
}

/** 
 * \fn int GUIAPI GetSockFD2Server (void)
 * \brief Gets the file descriptor of the socket connected to the server.
 *
 * This function returns the file descriptor of the socket connected 
 * to the server, i.e. mginit.
 *
 * \return The file descriptor of the socket connected to the server.
 *
 * \note Only used by clients, no meaning for the server.
 */
MG_EXPORT int GUIAPI GetSockFD2Server (void);

/** 
 * \fn int GUIAPI ServerSendReply (int clifd, const void* reply, int len)
 * \brief Sends the reply to the client.
 *
 * This function sends a replay pointed to by \a reply which is 
 * \a len bytes long to the client.
 *
 * \note Only used by the server to send the reply to the client.
 *       This function typically called in your customized request handler.
 *
 * \param clifd The fd connected to the client.
 * \param reply The buffer contains the reply data.
 * \param len The length of the reply data in bytes.
 * \return Zero on success, no-zero on error.
 *
 * \sa ClientRequest, RegisterRequestHandler
 */
MG_EXPORT int GUIAPI ServerSendReply (int clifd, const void* reply, int len);

/**
 * \var typedef int (* REQ_HANDLER)(int cli, int clifd, void* buff, size_t len)
 * \brief Request handler.
 *
 * \sa RegisterRequestHandler
 */
typedef int (* REQ_HANDLER) (int cli, int clifd, void* buff, size_t len);

/** 
 * \fn BOOL GUIAPI RegisterRequestHandler (int req_id, REQ_HANDLER your_handler)
 * \brief Registers a customize request handler.
 *
 * This function registers a request handler to the server, i.e. \a mginit.
 *
 * \param req_id The identifier of the customized request.
 * \param your_handler The handler of the request. 
 *        Being NULL to unregister the request handler.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \note Only used by the server to register a request handler.
 *       And the identifier should be larger than \a MAX_SYS_REQID and 
 *       less than or equal to \a MAX_REQID.
 *
 * \sa ClientRequest, ServerSendReply, MAX_SYS_REQID, MAX_REQID
 */
MG_EXPORT BOOL GUIAPI RegisterRequestHandler (int req_id, REQ_HANDLER your_handler);

/** 
 * \fn EQ_HANDLER GUIAPI GetRequestHandler (int req_id)
 * \brief Gets the request handler by request identifier.
 *
 * This function returns the request handler of the specified request 
 * identifier \a req_id.
 *
 * \param req_id The request identifier.
 *
 * \return The handler on success, NULL on error.
 *
 * \note Only can be used by the server.
 *
 * \sa RegisterRequestHandler
 */
MG_EXPORT REQ_HANDLER GUIAPI GetRequestHandler (int req_id);

    /** @} end of lite_request_fns */

    /**
     * \defgroup lite_socket_fns General socket operations
     *
     * MiniGUI-Processes uses UNIX domain socket to build the communication
     * between the server and the clients.
     *
     * You can also use the underlay interfaces which MiniGUI uses to create
     * your own UNIX domain socket.
     *
     * Example:
     *
     * \include socket.c
     *
     * @{
     */

/**
 * \fn int serv_listen (const char* name)
 * \brief Creates a listen socket.
 *
 * This function is used by the server to create a listening socket. 
 * Any MiniGUI-Processes application can call this function to create a 
 * listening socket. The server, i.e. \a mginit, of MiniGUI-Processes uses 
 * this function to create its listening socket, and named the socket 
 * to '/var/tmp/minigui'.
 *
 * \param name The path name of the listening socket.
 * \return The file discriptor of the listening socket created, -1 on error.
 *
 * \note As a convention, you should located the socket in '/var/tmp/' 
 * directory.
 */
MG_EXPORT int serv_listen (const char* name);

/**
 * \fn int serv_accept (int listenfd, pid_t *pidptr, uid_t *uidptr)
 * \brief Waits for a client connection to arrive, and accept it.
 *
 * This function is used by the server to wait a connection and accept it.
 *
 * After creating a listening socket by calling \a serv_listen, you can 
 * call this function to create a connection with a client. We also obtain 
 * the client's PID and UID from the pathname that it must bind before 
 * calling us.
 *
 * \param listenfd The fd of listen socket.
 * \param pidptr The client PID will be saved to this buffer when this 
 *        function returns.
 * \param uidptr The client UID will be saved to this buffer when this 
 *        function returns.
 *
 * \return The new connected fd if all OK, < 0 on error.
 *
 * \sa serv_listen, cli_conn
 */
MG_EXPORT int serv_accept (int listenfd, pid_t *pidptr, uid_t *uidptr);

/**
 * \fn int cli_conn (const char* name, char project)
 * \brief Used by clients to connect to a server.
 *
 * This function is used by clients to connect to a server.
 *
 * The created socket will be located at the directory '/var/tmp',
 * and with name of '/var/tmp/xxxxx-c', where 'xxxxx' is the pid of client.
 * and 'c' is a character to distinguish different projects.
 *
 * Note that MiniGUI itself uses 'a' as the project character to 
 * create socket between 'mginit' and clients.
 *
 * \param name The name of the well-known listen socket (created by server).
 * \param project A character to distinguish different projects 
 *        (Do \em NOT use 'a').
 * \return The new connected fd if all OK, < 0 on error.
 *
 * \sa serv_listen, serv_accept
 */
MG_EXPORT int cli_conn (const char* name, char project);

#define SOCKERR_IO          -1
#define SOCKERR_CLOSED      -2
#define SOCKERR_INVARG      -3
#define SOCKERR_TIMEOUT     -4
#define SOCKERR_OK          0

/**
 * \fn int sock_write_t (int fd, const void* buff,\
                int count, unsigned int timeout) 
 * \brief Writes data to socket.
 *
 * This function writes the data block pointed to by \a buff 
 * which is \a count bytes long to the socket \a fd.
 *
 * \param fd The file descriptor of the socket.
 * \param buff The buffer contains the data.
 * \param count The length in bytes of the buffer.
 * \param timeout An upper bound on the amount of time elapsed before 
 *        \a sock_write_t returns. When it is zero, \a sock_write_t can 
 *        block indefinitely. The timeout value is in tick count, and 
 *        tick count of MiniGUI is in unit of 10 milliseconds.
 * \return SOCKERR_OK if all OK, < 0 on error.
 *
 * \retval SOCKERR_OK       Read data successfully.
 * \retval SOCKERR_IO       There are some I/O errors occurred.
 * \retval SOCKERR_CLOSED   The socket has been closed by the peer.
 * \retval SOCKERR_INVARG   You passed invalid arguments.
 * \retval SOCKERR_TIMEOUT  Timeout.
 *
 * \note The \a timeout only goes into effect when this function called 
 *       by the server of MiniGUI-Processes, i.e. \a mginit. 
 *
 * \sa sock_read_t
 */
MG_EXPORT int sock_write_t (int fd, const void* buff, 
                int count, unsigned int timeout);

/**
 * \fn int sock_read_t (int fd, void* buff, int count, unsigned int timeout)
 * \brief Reads data from socket.
 *
 * This function reads data which is \a count bytes long to the buffer \a buff
 * from the socket \a fd.
 *
 * \param fd The file descriptor of the socket.
 * \param buff The buffer used to save the data.
 * \param count The length in bytes of the buffer.
 * \param timeout An upper bound on the amount of time elapsed before 
 *        \a sock_read_t returns. When it is zero, \a sock_read_t can 
 *        block indefinitely. The timeout value is in the tick count of MiniGUI,
 *        and tick count of MiniGUI is in unit of 10 milliseconds.
 * \return SOCKERR_OK if all OK, < 0 on error.
 *
 * \retval SOCKERR_OK       Read data successfully.
 * \retval SOCKERR_IO       There are some I/O errors occurred.
 * \retval SOCKERR_CLOSED   The socket has been closed by the peer.
 * \retval SOCKERR_INVARG   You passed invalid arguments.
 * \retval SOCKERR_TIMEOUT  Timeout.
 *
 * \note The \a timeout only goes into effect when this function called 
 *       by the server of MiniGUI-Processes, i.e. \a mginit. 
 *
 * \sa sock_write_t
 */
MG_EXPORT int sock_read_t (int fd, void* buff, int count, unsigned int timeout);

/**
 * \def sock_write(fd, buff, count)
 * \brief The blocking version of \a sock_write_t function.
 *
 * \sa sock_write_t
 */
#define sock_write(fd, buff, count) sock_write_t(fd, buff, count, 0)

/**
 * \def sock_read(fd, buff, count)
 * \brief The blocking version of \a sock_read_t function.
 *
 * \sa sock_read_t
 */
#define sock_read(fd, buff, count) sock_read_t(fd, buff, count, 0)

    /** @} end of lite_socket_fns */

    /** @} end of lite_fns */

#endif /* _MGRM_PROCESSES */

#endif /* LITE_VERSION */

    /**
     * \defgroup init_fns Initialization and termination functions
     *
     * Normally, the only entry of any MiniGUI application is \a MiniGUIMain.
     * The application will terminate when you call \a exit(3) or just 
     * return from \a MiniGUIMain.
     *
     * Example 1:
     *
     * \include miniguimain.c
     *
     * Example 2:
     *
     * \include helloworld.c
     *
     * @{
     */

/**
 * \fn BOOL GUIAPI ReinitDesktopEx (BOOL init_sys_text)
 * \brief Re-initializes the desktop.
 *
 * When you changed the charset or the background picture of the desktop,
 * you should call this function to re-initialize the local system text
 * (when \a init_sys_text is TRUE), the background picture, and the desktop
 * menu.
 *
 * \param init_sys_text Indicates whether to initialize the local system text.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa ReinitDesktop
 */
MG_EXPORT BOOL GUIAPI ReinitDesktopEx (BOOL init_sys_text);

/**
 * \def ReinitDesktop()
 * \brief Re-initializes the desktop including the local system text.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note This function defined as a macro calling \a ReinitDesktopEx with
 * \a init_sys_text set to TRUE.
 *
 * \sa ReinitDesktopEx
 */
#define ReinitDesktop()    ReinitDesktopEx (TRUE)

/**
 * \fn void GUIAPI ExitGUISafely (int exitcode)
 * \brief Exits your MiniGUI application safely.
 *
 * Calling this function will terminate your MiniGUI application. This
 * function will restore console attributes and call \a exit() function and 
 * pass \a exitcode to it.
 *
 * \param exitcode The exit status will be passed to exit(3) function.
 *
 * \return This function will not return.
 *
 * \sa exit(3)
 */
MG_EXPORT void GUIAPI ExitGUISafely (int exitcode);

#ifdef _USE_MINIGUIENTRY
  #define main_entry minigui_entry
  int minigui_entry (int args, const char* arg[]);
#else
  #define main_entry main
#endif

/**
 * \def MiniGUIMain
 * \brief The main entry of a MiniGUI application.
 *
 * This function should be defined by your application. Before Version 1.6.1, 
 * MiniGUI defines \a main() function in libminigui library for your 
 * application, and call \a MiniGUIMain() in this \a main() function. 
 * The \a main() defined by MiniGUI is responsible of initializing and 
 * terminating MiniGUI.
 *
 * After version 1.6.1, MiniGUI defines MiniGUIMain as a macro.
 *
 * \param args The number of arguments passed to \a main() by operating system.
 * \param argv The arguments passed to \a main() by operating system.
 *
 * \return The exit status will be retured to the parent process.
 */
#define MiniGUIMain \
MiniGUIAppMain (int args, const char* argv[]); \
int main_entry (int args, const char* argv[]) \
{ \
    int iRet = 0; \
    if (InitGUI (args, argv) != 0) { \
        return 1; \
    } \
    iRet = MiniGUIAppMain (args, argv); \
    TerminateGUI (iRet); \
    return iRet; \
} \
int MiniGUIAppMain

/**
 * \def IDM_DTI_FIRST
 * \brief The minimum interger value of command ID when user customize
 * desktop menu.
**/
#define IDM_DTI_FIRST   (300)

/** Desktop operation set */
typedef struct _DESKTOPOPS {
        
        /** called when starting a new session, and return a context */
        void* (*init) (void);
        
        /** called when terminating a seesion */
        void (*deinit) (void* context);
        
        /** called when the desktop should be repainted */
        void (*paint_desktop) (void* context, 
				HDC dc_desktop, const RECT* inv_rc);
        
        /** the keyboard event handler for the desktop */
        void (*keyboard_handler) (void* context, 
				int message, WPARAM wParam, LPARAM lParam);
        
        /** the mouse event handler for the desktop */
        void (*mouse_handler) (void* context, 
				int message, WPARAM wParam, LPARAM lParam);
        
        /** the desktop menu customizer */
        void (*customize_desktop_menu) (void* context, 
				                 HMENU hmenu, int start_pos);
        
        /** the desktop menu command handler */
        void (*desktop_menucmd_handler) (void* context, int id);
} DESKTOPOPS;

/**
 * \fn DESKTOPOPS* GUIAPI SetCustomDesktopOperationSet (DESKTOPOPS* usr_dsk_ops)
 *
 * \brief Set customer desktop operation set.
 *
 * \param usr_dsk_ops The pointer to user customer desktop operation set.
 *
 * \return Old desktop operation set.
 *
 * \code
 * static void* this_init(void)
 * {
 *     ......
 * }
 *
 * static void this_deinit(void* context)
 * {
 *     ......
 * }
 *
 * static void this_paint_desktop(void* context,\
 *                         HDC dc_desktop, const RECT* inv_rc)
 * {
 *     ......
 * }
 *
 * static void this_keyboard_handler(void* context, int message,\
 *	                               WPARAM wParam, LPARAM lParam)
 * {
 *     ......
 * }
 *
 * static void this_mouse_handler(void* context, int message,\
 *		                            WPARAM wParam, LPARAM lParam)
 * {
 *     ......
 * }
 *
 * static void this_customize_desktop_menu (void* context,\
 *                                           HMENU hmnu, int start_pos)
 * {
 *     ......
 * }
 *
 * static void this_desktop_menucmd_handler (void* context, int id)
 * {
 *     ......
 * }
 *
 * static DESKTOPOPS this_dsk_ops =
 * {
 *     this_init,
 *     this_deinit,
 *     this_paint_desktop,
 *     this_keyboard_handler,
 *     this_mouse_handler,
 *     this_customize_desktop_menu,
 *     this_desktop_menucmd_handler,
 * };
 *
 * SetCustomDesktopOperationSet(&this_dsk_ops);
 *
 * \endcode
 *
 * \sa DESKTOPOPS
 * 
 */
MG_EXPORT DESKTOPOPS* GUIAPI SetCustomDesktopOperationSet 
                                       (DESKTOPOPS* usr_dsk_ops);

/**
 * \fn void GUIAPI DesktopUpdateAllWindow (void)
 *
 * \brief Update all visible windows on the desktop.
 *
 * On MiniGUI-Processes update all the main windows of the client and desktop window,
 * and it only can be used by mginit on MiniGUI-Processes.
 * On MiniGUI-Threads and MiniGUI-Standalone update all visible windows and desktop window.
 *
 */
MG_EXPORT void GUIAPI DesktopUpdateAllWindow (void);

    /** @} end of init_fns */

    /**
     * \defgroup mouse_calibrate Mouse calibration.
     * @{
     */

#ifdef _MGHAVE_MOUSECALIBRATE

/**
 * \fn BOOL GUIAPI SetMouseCalibrationParameters (const POINT* src_pts,\
                const POINT* dst_pts)
 * \brief Sets the parameters for doing mouse calibration.
 *
 * This function set the parameters for doing mouse calibration.
 * You should pass five source points and five destination points.
 *
 * Normally, the points should be the upper-left, upper-right, lower-right, 
 * lower-left, and center points on the touch panel. The source point is 
 * the coordinates before calibrating, and the destination point is the 
 * desired coordinates after calibrating.
 *
 * This function will try to evaluate a matrix to calibrate. If the points 
 * are okay, MiniGUI will do the calibration after getting a point from the 
 * underlay IAL engine.
 *
 * \param src_pts The pointer to an array of five source points.
 * \param dst_pts The pointer to an array of five destination points.
 *
 * \return TRUE for success, FALSE for bad arguments.
 *
 * \note This function is available when _MGHAVE_MOUSECALIBRATE 
 *       (option: --enable-mousecalibrate) defined.
 * \note Only call this function in MiniGUI-Standalone, MiniGUI-Threads,
 *       and the server (mginit) of MiniGUI-Processes. The behavior of this
 *       function will be undefined if you call it in a client of 
 *       MiniGUI-Processes.
 */
MG_EXPORT BOOL GUIAPI SetMouseCalibrationParameters (const POINT* src_pts, 
                const POINT* dst_pts);

/**
 * \fn void GUIAPI GetOriginalMousePosition (int* x, int* y)
 * \brief Gets the original mouse position.
 *
 * \param x The pointer used to return the x coordinate of original mouse 
 *        position.
 * \param y The pointer used to return the y coordinate of original mouse 
 *        position.
 *
 * \return none.
 *
 * \note This function is available when _MGHAVE_MOUSECALIBRATE 
 *       (option: --enable-mousecalibrate) defined.
 *
 * \note Only call this function in MiniGUI-Standalone, MiniGUI-Threads,
 *       and the server (mginit) of MiniGUI-Processes. The behavior of this
 *       function will be undefined if you call it in a client of 
 *       MiniGUI-Processes.
 */
MG_EXPORT void GUIAPI GetOriginalMousePosition (int* x, int* y);

#endif /* _MGHAVE_MOUSECALIBRATE */

    /** @} end of mouse_calibrate */

    /**
     * \defgroup about_dlg About MiniGUI dialog
     * @{
     */
#ifdef _MGMISC_ABOUTDLG
#ifdef _MGRM_THREADS
/**
 * \fn HWND GUIAPI OpenAboutDialog (void)
 * \brief Opens or actives the 'About MiniGUI' dialog.
 *
 * Calling this function will create a main window displaying 
 * copyright and license information of MiniGUI. When the about dialog 
 * is displaying, calling this function again will bring the dialog to be 
 * the topmost main window, not create a new one.
 *
 * \return The handle to the about dialog box.
 *
 * \note This function is available for MiniGUI-Threads and when 
 *       _MGMISC_ABOUTDLG defined. For MiniGUI-Processes, you should 
 *       call 'void GUIAPI OpenAboutDialog (HWND hHosting)' function instead.
 */
MG_EXPORT  void GUIAPI OpenAboutDialog (void);
#else

/**
 * \fn HWND GUIAPI OpenAboutDialog (HWND hHosting)
 * \brief Opens or actives the 'About MiniGUI' dialog.
 *
 * Calling this function will create a main window displaying 
 * copyright and license information of MiniGUI. When the about dialog 
 * is displaying, calling this function again will bring the dialog to be 
 * the topmost main window, not create a new one.
 *
 * \param hHosting The hosting main window of the about dialog.
 *
 * \return The handle to the about dialog box.
 *
 * \note This function is available for MiniGUI-Processes and when 
 *       _MGMISC_ABOUTDLG defined. For MiniGUI-Threads, you should 
 *       call 'void GUIAPI OpenAboutDialog (void)' function instead.
 */
MG_EXPORT HWND GUIAPI OpenAboutDialog (HWND hHosting);
#endif /* _MGRM_THREADS */
#endif /* _MGMISC_ABOUTDLG */

    /** @} end of about_dlg */

    /**
     * \defgroup etc_fns Configuration file operations
     *
     * The configuration file used by MiniGUI have a similiar format as 
     * M$ Windows INI file, i.e. the file consists of sections, and 
     * the section consists of key-value pairs, like this:
     *
     * \code
     * [system]
     * # GAL engine
     * gal_engine=fbcon
     *
     * # IAL engine
     * ial_engine=console
     *
     * mdev=/dev/mouse
     * mtype=PS2
     * 
     * [fbcon]
     * defaultmode=1024x768-16bpp
     * 
     * [qvfb]
     * defaultmode=640x480-16bpp
     * display=0
     * \endcode
     *
     * Assume that the configuration file named \a my.cfg, if you want get 
     * the value of \a mdev in \a system section, you can call 
     * \a GetValueFromEtcFile in the following way:
     *
     * \code
     * char buffer [51];
     *
     * GetValueFromEtcFile ("my.cfg", "system", "mdev", buffer, 51);
     * \endcode
     *
     * Example:
     *
     * \include cfgfile.c
     *
     * @{
     */

/**
* \def ETC_MAXLINE
* \brief The max line number of etc file.
**/
#define ETC_MAXLINE             1024

/**
* \def ETC_FILENOTFOUND
* \brief No found etc file.
**/
#define ETC_FILENOTFOUND        -1
/**
* \def ETC_SECTIONNOTFOUND
* \brief No found section in etc file.
**/
#define ETC_SECTIONNOTFOUND     -2
/**
* \def ETC_KEYNOTFOUND
* \brief No found key in etc file.
**/
#define ETC_KEYNOTFOUND         -3
/**
* \def ETC_TMPFILEFAILED
* \brief Create tmpfile failed.
**/
#define ETC_TMPFILEFAILED       -4
/**
* \def ETC_FILEIOFAILED
* \brief IO operation failed to etc file.
**/
#define ETC_FILEIOFAILED        -5
/**
* \def ETC_INTCONV
* \brief  Convert the value string to an integer failed.
**/
#define ETC_INTCONV             -6
/**
* \def ETC_INVALIDOBJ
* \brief Invalid object to etc file.
**/
#define ETC_INVALIDOBJ          -7
/**
* \def ETC_READONLYOBJ
* \brief Read only to etc file.
**/
#define ETC_READONLYOBJ         -8
/**
* \def ETC_OK
* \brief Operate success to etc file.
**/
#define ETC_OK                  0

/** Etc The current config section information */
typedef struct _ETCSECTION
{
    /** Allocated number of keys */
    int key_nr_alloc;           
    /** Key number in the section */
    int key_nr;                 
    /** Name of the section */
    char *name;                 
    /** Array of keys */
    char** keys;                
    /** Array of values */
    char** values;              
} ETCSECTION;
/** Data type of pointer to a ETCSECTION */
typedef ETCSECTION* PETCSECTION;

/** ETC_S The current config file information*/
typedef struct _ETC_S
{
    /** Allocated number of sections */
    int sect_nr_alloc;          
    /** Number of sections */
    int section_nr;             
    /** Pointer to section arrays */
    PETCSECTION sections;       
} ETC_S;

#ifndef _MGINCORE_RES

/**
 * \var char* ETCFILEPATH
 * \brief The path name of MiniGUI configuration file.
 *
 * By default, the configuration file of MiniGUI must be installed in /etc,
 * /usr/local/etc or your home directory. When you install it in your
 * home directory, the name should be ".MiniGUI.cfg".
 *
 * MiniGUI will try to use MiniGUI.cfg in the current directory, 
 * \a ~/.MiniGUI.cfg, then \a /usr/local/etc/MiniGUI.cfg, and 
 * \a /etc/MiniGUI.cfg last.
 *
 * If MiniGUI can not find any \a MiniGUI.cfg file, or find a bad 
 * formated configure file, the initialzation of MiniGUI will be canceled.
 */
extern MG_EXPORT char ETCFILEPATH [];

#endif /* _MGINCORE_RES */

/**
 * \fn int GUIAPI GetValueFromEtcFile (const char* pEtcFile,\
                const char* pSection, const char* pKey, char* pValue, int iLen)
 * \brief Gets value from a configuration file.
 *
 * This function gets the value of the key \a pKey in the section \a pSection 
 * of the configuration file \a pEtcFile, and saves the value to the buffer
 * pointed to by \a pValue. 
 *
 * \param pEtcFile The path name of the configuration file.
 * \param pSection The section name in which the value located.
 * \param pKey The key name of the value.
 * \param pValue The value will be saved in this buffer.
 * \param iLen The length in bytes of the buffer.
 *
 * \return ETC_OK on success, < 0 on error.
 *
 * \retval ETC_OK               Gets value successfullly.
 * \retval ETC_FILENOTFOUND     Can not find the specified configuration file.
 * \retval ETC_SECTIONNOTFOUND  Can not find the specified section in the 
 *                              configuration file.
 * \retval ETC_KEYNOTFOUND      Can not find the specified key in the section.
 * \retval ETC_FILEIOFAILED     File I/O operation error occurred.
 *
 * \note MiniGUI use \a strncpy to copy actual value to \a pValue. Thus, 
 *       if the length of the actual value is larger than \a iLen, the 
 *       result copied to \a pValue will \em NOT be null-terminated.
 *
 * \sa GetIntValueFromEtcFile, SetValueToEtcFile, strncpy(3)
 */
MG_EXPORT int GUIAPI GetValueFromEtcFile (const char* pEtcFile, 
                const char* pSection, const char* pKey, char* pValue, int iLen);

/**
 * \fn int GUIAPI GetIntValueFromEtcFile (const char* pEtcFile,\
                const char* pSection, const char* pKey, int* value)
 * \brief Gets integer value from a configuration file.
 *
 * This function gets the integer value of the key \a pKey in the section 
 * \a pSection of the configuration file \a pEtcFile, and returns the 
 * integer value through the buffer pointed to by \a value. 
 *
 * \param pEtcFile The path name of the configuration file.
 * \param pSection The section name in which the value located.
 * \param pKey The key name of the value.
 * \param value The integer value will be saved in this buffer.
 *
 * \return ETC_OK on success, < 0 on error.
 *
 * \retval ETC_OK               Gets value successfullly.
 * \retval ETC_FILENOTFOUND     Can not find the specified configuration file.
 * \retval ETC_SECTIONNOTFOUND  Can not find the specified section in the 
 *                              configuration file.
 * \retval ETC_KEYNOTFOUND      Can not find the specified key in the section.
 * \retval ETC_FILEIOFAILED     File I/O operation error occurred.
 * \retval ETC_INTCONV          Can not convert the value string to an integer.
 *
 * \note MiniGUI uses \a strtol to convert the string value to an integer, 
 *       and pass the base as 0. Thus, the valid string value can be 
 *       converted to integer should be in the following forms:
 *
 *  - [+|-]0x[0-9|A-F]*\n
 *    Will be read in base 16.
 *  - [+|-]0[0-7]*\n
 *    Will be read in base 8.
 *  - [+|-][1-9][0-9]*\n
 *    Will be read in base 10.
 *
 * \sa GetValueFromEtcFile, SetValueToEtcFile, strtol(3)
 */
MG_EXPORT int GUIAPI GetIntValueFromEtcFile (const char* pEtcFile, 
                const char* pSection, const char* pKey, int* value);

/**
 * \fn int GUIAPI SetValueToEtcFile (const char* pEtcFile,\
                const char* pSection, const char* pKey, char* pValue)
 * \brief Sets a value in a configuration file.
 *
 * This function sets the value of the key \a pKey in the section \a pSection
 * of the configuration file \a pEtcFile to be the string pointed to by 
 * \a pValue.
 *
 * \param pEtcFile The path name of the configuration file.
 * \param pSection The section name in which the value located.
 * \param pKey The key name of the value.
 * \param pValue The null-terminated value string.
 *
 * \return ETC_OK on success, < 0 on error.
 *
 * \retval ETC_OK               Sets value successfullly.
 * \retval ETC_FILEIOFAILED     File I/O operation error occurred.
 * \retval ETC_TMPFILEFAILED    Can not create temporary file.
 *
 * \note If the specified configuration file does not exist, 
 *       MiniGUI will try to create this file.
 *
 * \sa GetValueFromEtcFile, GetIntValueFromEtcFile
 */
MG_EXPORT int GUIAPI SetValueToEtcFile (const char* pEtcFile, 
                const char* pSection, const char* pKey, char* pValue);

/**
 * \fn int GUIAPI RemoveSectionInEtcFile (const char* pEtcFile,\
                const char* pSection)
 * \brief Removes a section in an etc file.
 *
 * This function removes a section named \a pSection from the etc file
 * named \a pEtcFile.
 *
 * \param pEtcFile The name of the etc file.
 * \param pSection The name of the pSection;
 *
 * \return ETC_OK on success, < 0 on error.
 *
 * \retval ETC_OK               Gets value successfullly.
 * \retval ETC_FILEIOFAILED     File I/O operation error occurred.
 * \retval ETC_SECTIONNOTFOUND  Can not find the specified section in the 
 *                              etc object.
 *
 * \sa RemoveSectionInEtc
 */
MG_EXPORT int GUIAPI RemoveSectionInEtcFile (const char* pEtcFile, 
                const char* pSection);

/**
 * \fn int GUIAPI SaveSectionToEtcFile (const char* pEtcFile, \
                PETCSECTION psect);
 * \brief Saves a section to an etc file.
 *
 * This function saves a section named \a psect to the etc file
 * named \a pEtcFile.
 *
 * \param pEtcFile The name of the etc file.
 * \param psect The name of the psect;
 *
 * \return ETC_OK on success, < 0 on error.
 *
 * \retval ETC_OK               Gets value successfullly.
 * \retval ETC_FILEIOFAILED     File I/O operation error occurred.
 * \retval ETC_SECTIONNOTFOUND  Can not find the specified section in the 
 *                              etc object.
 */
MG_EXPORT int GUIAPI SaveSectionToEtcFile (const char* pEtcFile, 
										   PETCSECTION psect);
/**
 * \fn GHANDLE GUIAPI LoadEtcFile (const char * pEtcFile)
 * \brief Loads an etc file into memory.
 *
 * This function loads the content of an etc file into the memory, later, you
 * can visit the content using \a GetValueFromEtc function.
 *
 * \param pEtcFile The path name of the configuration file.
 *        If pEtcFile is NULL, the function will create an empty ETC object.
 *
 * \return Handle of the etc object on success, NULL on error.
 *
 * \sa UnloadEtcFile, GetValueFromEtc
 */
MG_EXPORT GHANDLE GUIAPI LoadEtcFile (const char * pEtcFile);

/**
 * \fn int GUIAPI SaveEtcToFile (GHANDLE hEtc, const char* file_name);
 * \brief Saves an ETC object into a file.
 *
 * This function saves the etc object into the file named \a file_name;
 *
 * \param hEtc Handle to the etc object.
 * \param file_name The name of the target file.
 *
 * \return ETC_OK on success, 0 < on error.
 *
 * \retval ETC_OK               Sets the etc object successfullly.
 * \retval ETC_INVALIDOBJ       Invalid etc object.
 * \retval ETC_FILEIOFAILED     File I/O operation error occurred.
 *
 * \sa LoadEtcFile
 */
MG_EXPORT int GUIAPI SaveEtcToFile (GHANDLE hEtc, const char* file_name);

/**
 * \fn GUIAPI UnloadEtcFile (GHANDLE hEtc)
 * \brief Unloads an etc file.
 *
 * This function unloads the etc object generated by using \sa LoadEtcFile 
 * function.
 *
 * \param hEtc Handle of the etc object.
 *
 * \return Returns 0 on success, -1 on error.
 *
 * \sa LoadEtcFile, GetValueFromEtc
 */
MG_EXPORT int GUIAPI UnloadEtcFile (GHANDLE hEtc);

/**
 * \fn int GUIAPI GetValueFromEtc (GHANDLE hEtc, const char* pSection, \
                const char* pKey, char* pValue, int iLen)
 * \brief Gets value from a configuration etc object.
 *
 * This function gets value from an etc object, similar to GetValueFromEtcFile.
 * This function gets the value of the key \a pKey in the section \a pSection 
 * of the etc object \a hEtc, and saves the value to the buffer pointed to 
 * by \a pValue. 
 *
 * \param hEtc Handle to the etc object.
 * \param pSection The section name in which the value located.
 * \param pKey The key name of the value.
 * \param pValue The value will be saved in this buffer.
 * \param iLen The length in bytes of the buffer. This function will set value
 *        if the iLen is less than 1.
 *
 * \return ETC_OK on success, < 0 on error.
 *
 * \retval ETC_OK               Gets value successfullly.
 * \retval ETC_INVALIDOBJ       Invalid etc object.
 * \retval ETC_SECTIONNOTFOUND  Can not find the specified section in the 
 *                              configuration file.
 * \retval ETC_KEYNOTFOUND      Can not find the specified key in the section.
 * \retval ETC_READONLYOBJ      The etc object is read-only.
 *
 * \sa GetValueFromEtcFile, LoadEtcFile, UnloadEtcFile
 */
MG_EXPORT int GUIAPI GetValueFromEtc (GHANDLE hEtc, const char* pSection, 
                const char* pKey, char* pValue, int iLen);

/**
 * \fn int GUIAPI GetIntValueFromEtc (GHANDLE hEtc, const char* pSection,\
                const char* pKey, int* pValue)
 * \brief Gets the integer value from a configuration etc object.
 *
 * \sa GetValueFromEtc, GetIntValueFromEtcFile
 */
MG_EXPORT int GUIAPI GetIntValueFromEtc (GHANDLE hEtc, const char* pSection,
                const char* pKey, int* pValue);

/**
 * \def SetValueToEtc(GHANDLE hEtc, const char* pSection,\
                const char* pKey, char* pValue)
 * \brief Sets the value in the etc object.
 *
 * This fuctions sets the value in the etc object, somewhat similiar 
 * to \sa SetValueToEtcFile.
 *
 * \sa SetValueToEtcFile, GetValueFromEtc
 */
#define SetValueToEtc(hEtc, pSection, pKey, pValue) \
        GetValueFromEtc(hEtc, pSection, pKey, pValue, -1)

/**
 * \fn GHANDLE GUIAPI FindSectionInEtc (GHANDLE hEtc,\
                const char* pSection, BOOL bCreateNew)
 * \brief Finds/Creates a section from an etc object.
 *
 * This function look for a section named \a pSection from the etc object
 * \a hEtc. If there is no such section in the etc object and \a bCreateNew
 * is TRUE, the function will create an empty section.
 *
 * \param hEtc Handle to the etc object.
 * \param pSection The name of the section.
 * \param bCreateNew Indicate whether to create a new section.
 *
 * \return The handle to the section, 0 if not found or creatation failed.
 *
 * \sa GetValueFromEtcSec, GetIntValueFromEtcSec, SetValueInEtcSec
 */
MG_EXPORT GHANDLE GUIAPI FindSectionInEtc (GHANDLE hEtc, 
                const char* pSection, BOOL bCreateNew);

/**
 * \fn int GUIAPI GetValueFromEtcSec (GHANDLE hSect,\
                const char* pKey, char* pValue, int iLen)
 * \brief Gets value from an etc section object.
 *
 * This function gets value from an etc section object, similar 
 * to GetValueFromEtc. It gets the value of the key \a pKey in the 
 * section \a hSect, and saves the value to the buffer pointed to 
 * by \a pValue. 
 *
 * \param hSect The handle to the section.
 * \param pKey The key name of the value.
 * \param pValue The value will be saved in this buffer.
 * \param iLen The length in bytes of the buffer. This function will set value
 *        if the iLen is less than 1.
 *
 * \return ETC_OK on success, < 0 on error.
 *
 * \retval ETC_OK               Gets value successfullly.
 * \retval ETC_INVALIDOBJ       Invalid etc object.
 * \retval ETC_KEYNOTFOUND      Can not find the specified key in the section.
 * \retval ETC_READONLYOBJ      The section object is read-only.
 *
 * \sa GetValueFromEtcFile, GetValueFromEtc, FindSectionInEtc
 */
MG_EXPORT int GUIAPI GetValueFromEtcSec (GHANDLE hSect, 
                const char* pKey, char* pValue, int iLen);

/**
 * \fn int GUIAPI GetIntValueFromEtcSec (GHANDLE hSect,\
                const char* pKey, int* pValue)
 * \brief Gets an integer value from an etc section object.
 *
 * This function gets an integer value from an etc section object, 
 * similar to GetIntValueFromEtc. It gets the value of the key \a pKey 
 * in the section \a hSect, and saves the value to the buffer pointed to 
 * by \a pValue. 
 *
 * \param hSect The handle to the section.
 * \param pKey The key name of the value.
 * \param pValue The value will be saved in this buffer.
 *
 * \return ETC_OK on success, < 0 on error.
 *
 * \retval ETC_OK               Gets value successfullly.
 * \retval ETC_INVALIDOBJ       Invalid etc object.
 * \retval ETC_KEYNOTFOUND      Can not find the specified key in the section.
 * \retval ETC_INTCONV          Can not convert the value string to an integer.
 *
 * \sa GetValueFromEtcFile, GetValueFromEtc, FindSectionInEtc
 */
MG_EXPORT int GUIAPI GetIntValueFromEtcSec (GHANDLE hSect, 
                const char* pKey, int* pValue);

/**
 * \fn int GUIAPI SetValueToEtcSec (GHANDLE hSect,\
                const char* pKey, char* pValue)
 * \brief Sets the value in the etc section object.
 *
 * This fuctions sets the value in the etc section object \a hSect, 
 * somewhat similiar to SetValueToEtc \sa SetValueToEtc.
 *
 * \sa GetValueFromEtc, FindSectionInEtc
 */
MG_EXPORT int GUIAPI SetValueToEtcSec (GHANDLE hSect, 
                const char* pKey, char* pValue);

/**
 * \fn int GUIAPI RemoveSectionInEtc (GHANDLE hEtc, const char* pSection)
 * \brief Removes a section in etc object.
 *
 * This function removes a section named \a pSection from the etc object
 * \a hEtc.
 *
 * \param hEtc The handle to the etc object.
 * \param pSection The name of the pSection;
 *
 * \return ETC_OK on success, < 0 on error.
 *
 * \retval ETC_OK               Gets value successfullly.
 * \retval ETC_INVALIDOBJ       Invalid etc object.
 * \retval ETC_READONLYOBJ      The etc object is read-only.
 * \retval ETC_SECTIONNOTFOUND  Can not find the specified section in the 
 *                              etc object.
 *
 * \sa RemoveSectionInEtcFile
 */
MG_EXPORT int GUIAPI RemoveSectionInEtc (GHANDLE hEtc, const char* pSection);

/* global MiniGUI etc file object */
extern MG_EXPORT GHANDLE hMgEtc;

/**
 *  \fn static inline int GetMgEtcValue(const char* pSection,\
                        const char *pKey, char *pValue, int iLen) 
 * \brief Gets value from MiniGUI configuration etc object 
 *
 * This fuctions gets the value from MiniGUi configuration etc object, 
 * somewhat similiar to GetValueFromEtcFile and GetValueFromEtc 
 * \sa GetValueFromEtcFile \sa GetValueFromEtc.
 */
static inline int GetMgEtcValue(const char* pSection, 
                const char *pKey, char *pValue, int iLen) 
{
#ifndef _MGINCORE_RES
    if (!hMgEtc)
        return GetValueFromEtcFile (ETCFILEPATH, pSection, pKey, pValue, iLen);
#endif

    return GetValueFromEtc (hMgEtc, pSection, pKey, pValue, iLen);
}

/**
 * \fn static inline int GetMgEtcIntValue (const char *pSection,\
                       const char* pKey, int *value)
 * \brief Gets integer value from MiniGUI configuration etc object
 *
 * This fuctions get integer value from MiniGUI configuration etc object
 * some what similiar to GetIntValueFromEtcFile and GetIntValueFromEtc
 * \sa GetIntValueFromEtcFile \sa GetIntValueFromEtc
 */
static inline int GetMgEtcIntValue (const char *pSection, 
                const char* pKey, int *value)
{
#ifndef _MGINCORE_RES
    if (!hMgEtc)
        return GetIntValueFromEtcFile (ETCFILEPATH, pSection, pKey, value);
#endif

    return GetIntValueFromEtc (hMgEtc, pSection, pKey, value);
}

    /** @} end of etc_fns */

#ifdef _MGHAVE_CLIPBOARD

    /**
     * \addtogroup clipboard_fns ClipBoard Operations
     * @{
     */

/**
* \def LEN_CLIPBOARD_NAME
* \brief The maximum length of the name of clipboard.
**/
#define LEN_CLIPBOARD_NAME      15
/**
* \def NR_CLIPBOARDS
* \brief The maximum number of clipboard.
**/
#define NR_CLIPBOARDS           4

/**
* \def CBNAME_TEXT
* \brief The default clipboard name of text control.
**/
#define CBNAME_TEXT             ("text")

/**
* \def CBERR_OK
* \brief Operate clipboard success. 
* \sa CreateClipBoard
**/
#define CBERR_OK        0
/**
* \def CBERR_BADNAME
* \brief Bad name to clipboard.
* \sa CreateClipBoard
**/
#define CBERR_BADNAME   1
/**
* \def CBERR_NOMEM
* \brief No enough memory to clipboard. 
* \sa CreateClipBoard
**/
#define CBERR_NOMEM     2

/**
* \def CBOP_NORMAL
* \brief Overwrite operation to clipboard. 
**/
#define CBOP_NORMAL     0
/**
* \def CBOP_APPEND
* \brief Append the new data to clipboarda after the old data. 
**/
#define CBOP_APPEND     1

/**
 * \fn int GUIAPI CreateClipBoard (const char* cb_name, size_t size)
 * \brief Creates a new clipboard.
 *
 * This function creates a new clipboard with the name \a cb_name.
 * MiniGUI itself creates a clipboard for text copying/pasting
 * called CBNAME_TEXT.
 *
 * \param cb_name The name of the new clipboard.
 * \param size The size of the clipboard.
 *
 * \retval CBERR_OK         The clipboard created.
 * \retval CBERR_BADNAME    Duplicated clipboard name.
 * \retval CBERR_NOMEM      No enough memory.
 *
 * \sa DestroyClipBoard
 */
MG_EXPORT int GUIAPI CreateClipBoard (const char* cb_name, size_t size);

/**
 * \fn int GUIAPI DestroyClipBoard (const char* cb_name)
 * \brief Destroys a new clipboard.
 *
 * This function destroys a clipboard with the name \a cb_name.
 *
 * \param cb_name The name of the clipboard.
 *
 * \retval CBERR_OK         The clipboard has been destroyed.
 * \retval CBERR_BADNAME    Can not find the clipboard with the name.
 *
 * \sa CreateClipBoard
 */
MG_EXPORT int GUIAPI DestroyClipBoard (const char* cb_name);

/**
 * \fn int GUIAPI SetClipBoardData (const char* cb_name,\
                void* data, size_t n, int cbop)
 * \brief Sets the data of a clipboard.
 *
 * This function sets the data into the clipboard named \a cb_name.
 *
 * \param cb_name The name of the clipboard.
 * \param data The pointer to the data.
 * \param n The length of the data.
 * \param cbop Type of clipboard operations, can be one of 
 *        the following values:
 *          - CBOP_NORMAL\n
 *            Overwrite the old data.
 *          - CBOP_APPEND\n
 *            Append the new data after the old data.
 *
 * \retval CBERR_OK         Success.
 * \retval CBERR_BADNAME    Bad clipboard name.
 * \retval CBERR_NOMEM      No enough memory.
 *
 * \sa GetClipBoardDataLen, GetClipBoardData
 */
MG_EXPORT int GUIAPI SetClipBoardData (const char* cb_name, 
                void* data, size_t n, int cbop);

/**
 * \fn size_t GUIAPI GetClipBoardDataLen (const char* cb_name)
 * \brief Gets the length of the data of a clipboard.
 *
 * This function gets the data length of the clipboard named \a cb_name.
 *
 * \param cb_name The name of the clipboard.
 *
 * \return The size of the data if success, otherwise zero.
 *
 * \sa GetClipBoardData
 */
MG_EXPORT size_t GUIAPI GetClipBoardDataLen (const char* cb_name);

/**
 * \fn size_t GUIAPI GetClipBoardData (const char* cb_name,\
                void* data, size_t n)
 * \brief Gets the data of a clipboard.
 *
 * This function gets the all data from the clipboard named \a cb_name.
 *
 * \param cb_name The name of the clipboard.
 * \param data The pointer to a buffer will save the data.
 * \param n The length of the buffer.
 *
 * \return The size of the data got if success, otherwise zero.
 *
 * \sa GetClipBoardByte
 */
MG_EXPORT size_t GUIAPI GetClipBoardData (const char* cb_name, 
                void* data, size_t n);

/**
 * \fn int GUIAPI GetClipBoardByte (const char* cb_name,\
                int index, unsigned char* byte);
 * \brief Gets a byte from a clipboard.
 *
 * This function gets a byte from the clipboard named \a cb_name.
 *
 * \param cb_name The name of the clipboard.
 * \param index The index of the byte.
 * \param byte The buffer saving the returned byte.
 *
 * \retval CBERR_OK         Success.
 * \retval CBERR_BADNAME    Bad clipboard name.
 * \retval CBERR_NOMEM      The index is beyond the data in the clipboard.
 *
 * \sa GetClipBoardData
 */
MG_EXPORT int GUIAPI GetClipBoardByte (const char* cb_name, 
                int index, unsigned char* byte);

    /** @} end of clipboard_fns */

#endif /* _MGHAVE_CLIPBOARD */

    /**
     * \addtogroup misc_fns Miscellaneous functions
     * @{
     */

/**
 * \fn void GUIAPI Ping (void)
 * \brief Makes a beep sound.
 * \sa Beep
 */
MG_EXPORT void GUIAPI Ping (void);

/**
 * \def Beep
 * \brief Alias of Ping.
 * \sa Ping
 */
#define Beep Ping

/**
 * \fn void GUIAPI Tone (int frequency_hz, int duration_ms)
 * \brief Makes a tone.
 *
 * This function will return after the tone. Thus, your program
 * will be blocked when the tone is being played.
 *
 * \param frequency_hz The frequency of the tone in hertz.
 * \param duration_ms The duration of the tone in millisecond.
 *
 * \bug When MiniGUI runs on X Window or RTOS, the tone can not be played 
 * correctly.
 *
 * \sa Ping
 */
MG_EXPORT void GUIAPI Tone (int frequency_hz, int duration_ms);

/**
 * \fn void* GUIAPI GetOriginalTermIO (void)
 * \brief Gets \a termios structure of the original terminal before 
 *        initializing MiniGUI.
 *
 * \return The pointer to the original \a termios structure.
 */
MG_EXPORT void* GUIAPI GetOriginalTermIO (void);

    /** @} end of misc_fns */

    /**
     * \defgroup fixed_str Length-Fixed string operations
     *
     * MiniGUI maintains a private heap for length-fixed strings, and allocates
     * length-fixed strings from this heap for window caption, menu item text, 
     * and so on. You can also use this private heap to allocate length-fixed 
     * strings.
     *
     * \include fixstr.c
     *
     * @{
     */

/**
 * \fn char* GUIAPI FixStrAlloc (int len)
 * \brief Allocates a buffer for a length-fixed string.
 *
 * This function allocates a buffer from the length-fixed string heap
 * for a string which is \a len bytes long (does not include the null 
 * character of the string). 
 *
 * \note You can change the content of the string, but do not change the 
 *       length of this string (shorter is valid) via \a strcat function or 
 *       other equivalent functions or operations.
 *
 * \param len The length of the string.
 *
 * \return The pointer to the buffer on success, otherwise NULL.
 *
 * \sa FreeFixStr
 */
MG_EXPORT char* GUIAPI FixStrAlloc (int len);

/**
 * \fn char* GUIAPI FixStrDup (const char* str)
 * \brief Duplicates a length-fixed string.
 *
 * This function allocates a buffer from the length-fixed string heap
 * and copy the string \a str to the buffer.
 *
 * \note You can change the content of the string, but do not change the 
 *       length of this string (shorter is valid) via \a strcat function or 
 *       other equivalent functions or operations.
 *
 * \param str The pointer to the string will be duplicated.
 *
 * \return The pointer to the buffer on success, otherwise NULL.
 *
 * \sa FreeFixStr
 */
MG_EXPORT char* GUIAPI FixStrDup (const char* str);

/**
 * \fn void GUIAPI FreeFixStr (char* str)
 * \brief Frees a length-fixed string.
 *
 * This function frees the buffer used by the length-fixed string \a str.
 *
 * \param str The length-fixed string.
 *
 * \note Do not use \a free to free the length-fixed string.
 *
 * \sa FixStrAlloc
 */
MG_EXPORT void GUIAPI FreeFixStr (char* str);

    /** @} end of fixed_str */

    /**
     * \defgroup cursor_fns Cursor operations
     * @{
     */

#ifndef _MGHAVE_CURSOR
static inline void do_nothing (void) { return; }
#endif

#ifdef _MGHAVE_CURSOR

/**
 * \fn HCURSOR GUIAPI LoadCursorFromFile (const char* filename)
 * \brief Loads a cursor from a M$ Windows cursor file.
 *
 * This function loads a cursor from M$ Windows *.cur file 
 * named \a filename and returns the handle to loaded cursor. 
 * The returned handle can be used by \a SetCursor to set new mouse cursor.
 *
 * \param filename The path name of the cursor file.
 *
 * \return Handle to the cursor, zero on error.
 *
 * \note MiniGUI does not support 256-color or animation cursor.
 *
 * \sa SetCursor
 */
MG_EXPORT HCURSOR GUIAPI LoadCursorFromFile (const char* filename);

/**
 * \fn HCURSOR GUIAPI LoadCursorFromMem (const void* area)
 * \brief Loads a cursor from a memory area.
 *
 * This function loads a cursor from a memory area pointed to by \a area. 
 * The memory has the same layout as a M$ Windows CURSOR file.
 * The returned handle can be used by \a SetCursor to set new mouse cursor.
 *
 * \param area The pointer to the cursor data.
 *
 * \return Handle to the cursor, zero on error.
 *
 * \note MiniGUI does not support 256-color or animation cursor.
 *
 * \sa SetCursor
 */
MG_EXPORT HCURSOR GUIAPI LoadCursorFromMem (const void* area);

/**
 * \fn HCURSOR GUIAPI CreateCursor (int xhotspot, int yhotspot, int w, int h,\
                const BYTE* pANDBits, const BYTE* pXORBits, int colornum)
 * \brief Creates a cursor from memory data.
 *
 * This function creates a cursor from memory data rather than cursor file. 
 * \a xhotspot and \a yhotspot specify the hotpot of the cursor, \a w 
 * and \a h are the width and the height of the cursor respectively. 
 * \a pANDBits and \a pXORBits are AND bitmask and XOR bitmask of the cursor. 
 * MiniGUI currently support mono-color cursor and 16-color cursor, \a colornum 
 * specifies the cursor's color depth. For mono-color, it should be 1, and for
 * 16-color cursor, it should be 4.
 *
 * \param xhotspot The x-coordinate of the hotspot.
 * \param yhotspot The y-coordinate of the hotspot.
 * \param w The width of the cursor.
 * \param h The height of the cursor.
 * \param pANDBits The pointer to AND bits of the cursor.
 * \param pXORBits The pointer to XOR bits of the cursor.
 * \param colornum The bit-per-pixel of XOR bits.
 *
 * \return Handle to the cursor, zero on error.
 *
 * \note MiniGUI only support 2-color or 16-color cursor.
 */
MG_EXPORT HCURSOR GUIAPI CreateCursor (int xhotspot, int yhotspot, int w, int h,
               const BYTE* pANDBits, const BYTE* pXORBits, int colornum);

/**
 * \fn HCURSOR GUIAPI CopyCursor (HCURSOR hcsr)
 * \brief Copies a cursor object.
 *
 * This function copies the specified cursor.
 *
 * \param hcsr Handle to the cursor to be copied. 
 *
 * \return If the function succeeds, the return value is the 
 *         handle to the duplicate cursor. If the function fails, 
 *         the return value is 0.
 */
MG_EXPORT HCURSOR GUIAPI CopyCursor (HCURSOR hcsr);

/**
 * \fn BOOL GUIAPI DestroyCursor (HCURSOR hcsr)
 * \brief Destroys a cursor object.
 *
 * This function destroys a cursor object specified by \a hcsr.
 *
 * \param hcsr Handle to the cursor.
 *
 * \return TRUE on success, otherwise FALSE.
 */
MG_EXPORT BOOL GUIAPI DestroyCursor (HCURSOR hcsr);

/**
 * \fn HCURSOR GUIAPI GetSystemCursor (int csrid)
 * \brief Gets the handle to a system cursor by its identifier.
 *
 * MiniGUI creates (MAX_SYSCURSORINDEX + 1) system cursors for application.
 * You can use \a GetSystemCursor to get the handle to these system cursors.
 * The identifier can be one of the following:
 * 
 *  - IDC_ARROW\n
 *    Normal arrow cursor.
 *  - IDC_IBEAM\n
 *    'I' shaped beam cursor, indicate an input field.
 *  - IDC_PENCIL\n
 *    Pencil-shape cursor.
 *  - IDC_CROSS\n
 *    Cross cursor.
 *  - IDC_MOVE\n
 *    Moving cursor.
 *  - IDC_SIZENWSE\n
 *    Sizing cursor, along north-west and south-east.
 *  - IDC_SIZENESW\n
 *    Sizing cursor, along north-east and south-west.
 *  - IDC_SIZEWE\n
 *    Sizing cursor, along west and east.
 *  - IDC_SIZENS\n
 *    Sizing cursor, along north and south.
 *  - IDC_UPARROW\n
 *    Up arrow cursor.
 *  - IDC_NONE\n
 *    None cursor.
 *  - IDC_HELP\n
 *    Arrow with question.
 *  - IDC_BUSY\n
 *    Busy cursor.
 *  - IDC_WAIT\n
 *    Wait cursor.
 *  - IDC_RARROW\n
 *    Right arrow cursor.
 *  - IDC_COLOMN\n
 *    Cursor indicates column.
 *  - IDC_ROW\n
 *    Cursor indicates row.
 *  - IDC_DRAG\n
 *    Draging cursor.
 *  - IDC_NODROP\n
 *    No droping cursor.
 *  - IDC_HAND_POINT\n
 *    Hand point cursor.
 *  - IDC_HAND_SELECT\n
 *    Hand selection cursor.
 *  - IDC_SPLIT_HORZ\n
 *    Horizontal splitting cursor.
 *  - IDC_SPLIT_VERT\n
 *    Vertical splitting cursor.
 *
 * \param csrid The identifier of the system cursor.
 * \return Handle to the system cursor, otherwise zero.
 */
MG_EXPORT HCURSOR GUIAPI GetSystemCursor (int csrid);

/**
 * \fn HCURSOR GUIAPI GetCurrentCursor (void)
 * \brief Gets the handle to the current cursor.
 *
 * This function retrives the current cursor and returns its handle.
 *
 * \return Handle to the current system cursor, zero means no current cursor.
 */
MG_EXPORT HCURSOR GUIAPI GetCurrentCursor (void);
#else
  #define LoadCursorFromFile(filename)    (do_nothing(), 0)
  #define CreateCursor(x, y, w, h, ANDbs, XORbs, cr) (do_nothing(), 0)
  #define DestroyCursor(hcsr)             (do_nothing(), 0)
  #define GetSystemCursor(csrid)          (do_nothing(), 0)
  #define GetCurrentCursor()              (do_nothing(), 0)
#endif /* _MGHAVE_CURSOR */

#define MAX_SYSCURSORINDEX    22

/* System cursor index. */
/** System arrow cursor index. */
#define IDC_ARROW       0
/** System beam cursor index. */
#define IDC_IBEAM       1
/** System pencil cursor index. */
#define IDC_PENCIL      2
/** System cross cursor index. */
#define IDC_CROSS       3
/** System move cursor index. */
#define IDC_MOVE        4
/** System  size northwest to southeast cursor index. */
#define IDC_SIZENWSE    5
/** System size northeast to southwest cursor index. */
#define IDC_SIZENESW    6
/** System west to east cursor index. */
#define IDC_SIZEWE      7
/** System north to south cursor index. */
#define IDC_SIZENS      8
/** System up arrow cursor index. */
#define IDC_UPARROW     9
/** System none cursor index. */
#define IDC_NONE        10
/** System help cursor index. */
#define IDC_HELP        11
/** System busy cursor index. */
#define IDC_BUSY        12
/** System wait cursor index. */
#define IDC_WAIT        13
/** System right arrow cursor index. */
#define IDC_RARROW      14
/** System colomn cursor index. */
#define IDC_COLOMN      15
/** System row cursor index. */
#define IDC_ROW         16
/** System drag cursor index. */
#define IDC_DRAG        17
/** System nodrop cursor index. */
#define IDC_NODROP      18
/** System hand point cursor index. */
#define IDC_HAND_POINT  19
/** System hand select cursor index. */
#define IDC_HAND_SELECT 20
/** System horizontal split cursor index. */
#define IDC_SPLIT_HORZ  21
/** System vertical cursor index. */
#define IDC_SPLIT_VERT  22

/**
 * \fn void GUIAPI ClipCursor (const RECT* prc)
 * \brief Clips the cursor range.
 *
 * This function sets cursor's clipping rectangle. \a prc 
 * is the new clipping rectangle in screen coordinates. If \a prc is NULL, 
 * \a ClipCursor will disable cursor clipping.
 *
 * \param prc The clipping rectangle.
 * \return None.
 */
MG_EXPORT void GUIAPI ClipCursor (const RECT* prc);

/**
 * \fn void GUIAPI GetClipCursor (RECT* prc)
 * \brief Gets the current cursor clipping rectangle.
 *
 * This function copies the current clipping rectangle to 
 * a RECT pointed to by \a prc.
 *
 * \param prc The clipping rectangle will be saved to this rectangle.
 * \return None.
 */
MG_EXPORT void GUIAPI GetClipCursor (RECT* prc);

/**
 * \fn void GUIAPI GetCursorPos (POINT* ppt)
 * \brief Gets position of the current cursor.
 *
 * This function copies the current mouse cursor position to 
 * a POINT structure pointed to by \a ppt.
 *
 * \param ppt The position will be saved in this buffer.
 * \return None.
 *
 * \sa SetCursorPos, POINT
 */
MG_EXPORT void GUIAPI GetCursorPos (POINT* ppt);

/**
 * \fn void GUIAPI SetCursorPos (int x, int y)
 * \brief Sets position of the current cursor.
 *
 * This function sets mouse cursor position with the given 
 * arguments: \a (\a x,\a y).
 *
 * \param x The x-corrdinate of the expected poistion.
 * \param y The y-corrdinate of the expected poistion.
 * \return None.
 *
 * \sa GetCursorPos
 */
MG_EXPORT void GUIAPI SetCursorPos (int x, int y);

#ifdef _MGHAVE_CURSOR
/**
 * \fn HCURSOR GUIAPI SetCursorEx (HCURSOR hcsr, BOOL set_def)
 * \brief Changes the current cursor.
 *
 * This function changes the current cursor to be \a hcsr,
 * and/or sets it to be the default cursor.
 *
 * If you pass \a set_def as TRUE, the expected cursor will be the default 
 * cursor. The default cursor will be used when you move the cursor to 
 * the desktop.
 *
 * \param hcsr The expected cursor handle.
 * \param set_def Indicates whether setting the cursor as the default cursor.
 *
 * \return The old cursor handle.
 *
 * \sa SetCursor, SetDefaultCursor, GetDefaultCursor
 */
 MG_EXPORT HCURSOR GUIAPI SetCursorEx (HCURSOR hcsr, BOOL set_def);

/**
 * \def SetCursor(hcsr)
 * \brief Changes the current cursor.
 *
 * This function changes the current cursor to be \a hcsr.
 *
 * \param hcsr The expected cursor handle.
 * \return The old cursor handle.
 *
 * \note This function defined as a macro calling \a SetCursorEx with 
 *       passing \a set_def as FALSE.
 *
 * \sa SetCursorEx, SetDefaultCursor
 */
  #define SetCursor(hcsr) SetCursorEx (hcsr, FALSE)

/**
 * \def SetDefaultCursor(hcsr)
 * \brief Changes the current cursor, and set it as the default cursor.
 *
 * This function changes the current cursor to be \a hcsr, and set it as 
 * the default cursor.
 *
 * \param hcsr The expected cursor handle.
 * \return The old cursor handle.
 *
 * \note This function defined as a macro calling \a SetCursorEx with
 * passing \a set_def as TRUE.
 *
 * \sa SetCursorEx, SetCursor
 */
  #define SetDefaultCursor(hcsr) SetCursorEx (hcsr, TRUE)

/**
 * \fn HCURSOR GUIAPI GetDefaultCursor (void)
 * \brief Gets the default cursor.
 *
 * This function gets the current default cursor.
 *
 * \return The current default cursor handle.
 *
 * \sa SetCursorEx, SetDefaultCursor
 */
MG_EXPORT  HCURSOR GUIAPI GetDefaultCursor (void);

#else
  #define SetCursorEx(hcsr, set_def)    (do_nothing(), 0)
  #define SetCursor(hcsr)               (do_nothing(), 0)
  #define SetDefaultCursor(hcsr)        (do_nothing(), 0)
  #define GetDefaultCursor()            (do_nothing(), 0)
#endif /* _MGHAVE_CURSOR */

#ifdef _MGHAVE_CURSOR

/**
 * \fn int GUIAPI ShowCursor (BOOL fShow)
 * \brief Shows or hides cursor.
 *
 * This function shows or hides cursor according to the argument \a fShow. 
 * Show cursor when \a fShow is TRUE, and hide cursor when \a fShow is FALSE.
 * MiniGUI maintains a showing count value. Calling \a ShowCursor once, the 
 * count will increase when \a fShow is TRUE, or decrease one when FALSE.
 * When the count is less than 0, the cursor will disapear actually.
 *
 * \param fShow Indicates show or hide the cursor.
 *
 * \return Cursor showing count value. 
 */
MG_EXPORT int GUIAPI ShowCursor (BOOL fShow);
#else
  #define ShowCursor(fShow)             (do_nothing(), 0)
#endif /* _MGHAVE_CURSOR */

    /** @} end of cursor_fns */

    /**
     * \defgroup key_status Asynchronous key status functions
     * @{
     */

/**
 * \fn BOOL GUIAPI GetKeyStatus (UINT uKey)
 * \brief Gets a key or a mouse button status.
 *
 * This function gets a key or a mouse button status, returns TRUE 
 * when pressed, or FALSE when released. \a uKey indicates 
 * the key or mouse button. For keys on keyboard, \a uKey should be 
 * the scancode of the key, for mouse button, \a uKey should be one 
 * value of the following:
 *
 *  - SCANCODE_LEFTBUTTON\n
 *    Left mouse button.
 *  - SCANCODE_MIDDLBUTTON\n
 *    Middle mouse button.
 *  - SCANCODE_RIGHTBUTTON\n
 *    Right mouse button.
 *
 * These constants and the scancodes of keys are defined in <minigui/common.h>.
 *
 * \param uKey Indicates the key or mouse button. 
 *
 * \return Returns TRUE when pressed, or FALSE when released.
 *
 * \sa GetShiftKeyStatus
 */
MG_EXPORT BOOL GUIAPI GetKeyStatus (UINT uKey);

/**
 * \fn DWORD GUIAPI GetShiftKeyStatus (void)
 * \brief Gets status of the shift keys.
 *
 * This function gets ths status of the shift keys, the returned value 
 * indicates the status of shift keys -- CapsLock, ScrollLock, NumLock, 
 * Left Shift, Right Shift, Left Ctrl, Right Ctrl, Left Alt, and Right Alt. 
 * You can use KS_* ORed with the status value to determine one shift key's 
 * status:
 *
 *  - KS_CAPSLOCK\n
 *    Indicates that CapsLock is locked.
 *  - KS_NUMLOCK\n
 *    Indicates that NumLock is locked.
 *  - KS_SCROLLLOCK\n
 *    Indicates that ScrollLock is locked.
 *  - KS_LEFTCTRL\n
 *    Indicates that left Ctrl key is pressed.
 *  - KS_RIGHTCTRL\n
 *    Indicates that right Ctrl key is pressed.
 *  - KS_CTRL\n
 *    Indicates that either left or right Ctrl key is pressed.
 *  - KS_LEFTALT\n
 *    Indicates that left Alt key is pressed.
 *  - KS_RIGHTALT\n
 *    Indicates that right Alt key is pressed.
 *  - KS_ALT\n
 *    Indicates that either left or right Alt key is pressed.
 *  - KS_LEFTSHIFT\n
 *    Indicates that left Shift key is pressed.
 *  - KS_RIGHTSHIFT\n
 *    Indicates that right Shift key is pressed.
 *  - KS_SHIFT\n
 *    Indicates that either left or right Shift key is pressed.
 *
 * These constants are defined in <minigui/common.h>.
 *
 * \return The status of the shift keys.
 * \sa key_defs
 */
MG_EXPORT DWORD GUIAPI GetShiftKeyStatus (void);

/**
 * \fn void GUIAPI GetKeyboardState (BYTE* kbd_state)
 * \brief Gets status of all keys on keyboard.
 *
 * This function gets the status of all keys on keyboard. 
 *
 * The scancodes of all keys are defined in <minigui/common.h>.
 *
 * \param kbd_state The buffer returns the current status of all keys.
 *        Note that the length of the buffer should be larger than 
 *        (MGUI_NR_KEYS + 1).
 *
 * \sa GetKeyStatus, MGUI_NR_KEYS
 */
MG_EXPORT void GUIAPI GetKeyboardState (BYTE* kbd_state);

    /** @} end of key_status */

    /**
     * \defgroup sys_text Internationlization of system text
     * @{
     */

/**
 * \def IDS_MGST_WINDOW
 * \brief The identifier of the window system text.
 */
#define IDS_MGST_WINDOW          0

/**
 * \def IDS_MGST_START
 * \brief The identifier of the start system text.
 */
#define IDS_MGST_START           1

/**
 * \def IDS_MGST_REFRESH
 * \brief The identifier of the refresh background system text.
 */
#define IDS_MGST_REFRESH         2

/**
 * \def IDS_MGST_CLOSEALLWIN
 * \brief The identifier of the close all windows system text.
 */
#define IDS_MGST_CLOSEALLWIN     3

/**
 * \def IDS_MGST_ENDSESSION
 * \brief The identifier of the end session system text.
 */
#define IDS_MGST_ENDSESSION      4

/**
 * \def IDS_MGST_OPERATIONS
 * \brief The identifier of the operations system text.
 */
#define IDS_MGST_OPERATIONS      5

/**
 * \def IDS_MGST_MINIMIZE
 * \brief The identifier of the minimize system text.
 */
#define IDS_MGST_MINIMIZE        6

/**
 * \def IDS_MGST_MAXIMIZE
 * \brief The identifier of the maximize system text.
 */
#define IDS_MGST_MAXIMIZE        7

/**
 * \def IDS_MGST_RESTORE
 * \brief The identifier of the restore system text.
 */
#define IDS_MGST_RESTORE         8

/**
 * \def IDS_MGST_CLOSE
 * \brief The identifier of the close system text.
 */
#define IDS_MGST_CLOSE           9

/**
 * \def IDS_MGST_OK
 * \brief The identifier of the ok system text.
 */
#define IDS_MGST_OK              10

/**
 * \def IDS_MGST_NEXT
 * \brief The identifier of the next system text.
 */
#define IDS_MGST_NEXT            11

/**
 * \def IDS_MGST_CANCEL
 * \brief The identifier of the cancel system text.
 */
#define IDS_MGST_CANCEL          12

/**
 * \def IDS_MGST_PREV
 * \brief The identifier of the previous system text.
 */
#define IDS_MGST_PREV            13

/**
 * \def IDS_MGST_YES
 * \brief The identifier of the yes system text.
 */
#define IDS_MGST_YES             14

/**
 * \def IDS_MGST_NO
 * \brief The identifier of the no system text.
 */
#define IDS_MGST_NO              15

/**
 * \def IDS_MGST_ABORT
 * \brief The identifier of the abort system text.
 */
#define IDS_MGST_ABORT           16

/**
 * \def IDS_MGST_RETRY
 * \brief The identifier of the retry system text.
 */
#define IDS_MGST_RETRY           17

/**
 * \def IDS_MGST_IGNORE
 * \brief The identifier of the ignore system text.
 */
#define IDS_MGST_IGNORE          18

/**
 * \def IDS_MGST_ABOUTMG
 * \brief The identifier of the about minigui system text.
 */
#define IDS_MGST_ABOUTMG         19

/**
 * \def IDS_MGST_OPENFILE
 * \brief The identifier of the open file system text.
 */
#define IDS_MGST_OPENFILE        20

/**
 * \def IDS_MGST_SAVEFILE
 * \brief The identifier of the save file system text.
 */
#define IDS_MGST_SAVEFILE        21

/**
 * \def IDS_MGST_COLORSEL
 * \brief The identifier of the color selection system text.
 */
#define IDS_MGST_COLORSEL        22

/**
 * \def IDS_MGST_SWITCHLAYER
 * \brief The identifier of the switch layer system text.
 */
#define IDS_MGST_SWITCHLAYER     23

/**
 * \def IDS_MGST_DELLAYER
 * \brief The identifier of the delete layer system text.
 */
#define IDS_MGST_DELLAYER        24

/**
 * \def IDS_MGST_ERROR
 * \brief The identifier of the error system text.
 */
#define IDS_MGST_ERROR           25

/**
 * \def IDS_MGST_LOGO
 * \brief The identifier of the logo system text.
 */
#define IDS_MGST_LOGO            26

/**
 * \def IDS_MGST_CURRPATH
 * \brief The identifier of the current path system text.
 */
#define IDS_MGST_CURRPATH        27

/**
 * \def IDS_MGST_FILE
 * \brief The identifier of the file system text.
 */
#define IDS_MGST_FILE            28

/**
 * \def IDS_MGST_LOCATION
 * \brief The identifier of the location system text.
 */
#define IDS_MGST_LOCATION        29

/**
 * \def IDS_MGST_UP
 * \brief The identifier of the up system text.
 */
#define IDS_MGST_UP              30

/**
 * \def IDS_MGST_NAME
 * \brief The identifier of the name system text.
 */
#define IDS_MGST_NAME            31

/**
 * \def IDS_MGST_SIZE
 * \brief The identifier of the size system text.
 */
#define IDS_MGST_SIZE            32

/**
 * \def IDS_MGST_ACCESSMODE
 * \brief The identifier of the access mode system text.
 */
#define IDS_MGST_ACCESSMODE      33

/**
 * \def IDS_MGST_LASTMODTIME
 * \brief The identifier of the last modify time system text.
 */
#define IDS_MGST_LASTMODTIME     34

/**
 * \def IDS_MGST_OPEN
 * \brief The identifier of the open system text.
 */
#define IDS_MGST_OPEN            35

/**
 * \def IDS_MGST_FILENAME
 * \brief The identifier of the file name system text.
 */
#define IDS_MGST_FILENAME        36

/**
 * \def IDS_MGST_FILETYPE
 * \brief The identifier of the file type system text.
 */
#define IDS_MGST_FILETYPE        37

/**
 * \def IDS_MGST_SHOWHIDEFILE
 * \brief The identifier of the show hide file system text.
 */
#define IDS_MGST_SHOWHIDEFILE    38

/**
 * \def IDS_MGST_NOTFOUND
 * \brief The identifier of the not found file system text.
 */
#define IDS_MGST_NOTFOUND        39

/**
 * \def IDS_MGST_NR
 * \brief The identifier of the can't read system text.
 */
#define IDS_MGST_NR              40

/**
 * \def IDS_MGST_NW
 * \brief The identifier of the can't write system text.
 */
#define IDS_MGST_NW              41

/**
 * \def IDS_MGST_INFO
 * \brief The identifier of the information system text.
 */
#define IDS_MGST_INFO            42

/**
 * \def IDS_MGST_R
 * \brief The identifier of the read system text.
 */
#define IDS_MGST_R               43

/**
 * \def IDS_MGST_W
 * \brief The identifier of the write system text.
 */
#define IDS_MGST_W               44

/**
 * \def IDS_MGST_WR
 * \brief The identifier of the read and write system text.
 */
#define IDS_MGST_WR              45

/**
 * \def IDS_MGST_SAVE
 * \brief The identifier of the save system text.
 */
#define IDS_MGST_SAVE            46

/**
 * \def IDS_MGST_FILEEXIST
 * \brief The identifier of the file exist system text.
 */
#define IDS_MGST_FILEEXIST       47

#define IDS_MGST_MAXNUM          47

/**
 * \fn const char* GUIAPI GetSysText (unsigned int id);
 * \brief Translates system text to localized text.
 *
 * When MiniGUI display some system messages, it will call \a GetSysText 
 * function to translate system text from English to other language. 
 * Global variable \a SysText contains all text used by MiniGUI in English.
 *
 * \a GetSysText function returns localized text from \a local_SysText. 
 * MiniGUI have already defined localized sytem text for en_US, zh_CN.GB2312 
 * and zh_TW.Big5 locales. MiniGUI initializes \a local_SysText to 
 * point one of above localized system text when startup.  You can also 
 * let \a local_SysText point to your customized string array.
 * 
 * \param id The system text id.
 * \return The localized text.
 *
 * \sa SysText, local_SysText
 */
MG_EXPORT const char* GUIAPI GetSysText (unsigned int id);

/**
 * \var const char* SysText []
 * \brief Contains all text used by MiniGUI in English.
 *
 * System text defined as follows in MiniGUI:
 *
 * \code
 * const char* SysText [] =
 * {
 *    "Windows...",
 *    "Start...",
 *    "Refresh Background",
 *    "Close All Windows",
 *    "End Session",
 *    "Operations...",
 *    "Minimize",
 *    "Maximize",
 *    "Restore",
 *    "Close",
 *    "OK",
 *    "Next",
 *    "Cancel",
 *    "Previous",
 *    "Yes",
 *    "No",
 *    "Abort",
 *    "Retry",
 *    "Ignore",
 *    "About MiniGUI...",
 *    "Open File",
 *    "Save File",
 *    "Color Selection",
 *    "Switch Layer",
 *    "Delete Layer",
 *    "Error",
 *    "LOGO",
 *    "Current Path",
 *    "File",
 *  #if (!defined (__NOUNIX__) || defined (WIN32)) && defined (_MGCTRL_LISTVIEW)

 *    "Location",
 *    "Up",
 *    "Name",
 *    "Size",
 *    "Access Mode",
 *    "Last Modify Time",
 *    "Open",
 *    "File Name",
 *    "File Type",
 *    "Show Hide File",
 *    "Sorry! not find %s ",
 *    "Can't Read %s !",
 *    "Can't Write  %s !",
 *    "Information",
 *    "R",
 *    "W",
 *    "WR",
 *    "Save",
 *    "File %s exists, Replace or not?",
 *  #endif
 *    NULL
 * };
 * \endcode
 *
 * \sa GetSysText, local_SysText
 */
extern MG_EXPORT const char* SysText [];

/**
 * \var const char** local_SysText
 * \brief The pointer to the current localized system text array.
 *
 * Changing \a local_SysText will lead to \a GetSysText returns a different 
 * localized system text. Please set it when MiniGUI starts 
 * and send desktop a MSG_REINITSESSION message (call \a ReinitDesktop 
 * function) after assigned a different value to this variable.
 *
 * \sa GetSysText, SysText, ReinitDesktopEx
 */
extern MG_EXPORT const char** local_SysText;

#ifdef _MGCHARSET_UNICODE

/**
 * \fn const char** GUIAPI GetSysTextInUTF8 (const char* language)
 * \brief Gets the localized system text array in UTF-8 for 
 *        a specified language.
 *
 * This function returns the localized system text array in UTF-8 encode
 * for the specified language. You can use the returned localized system
 * text array to set \a local_SysText if your system logical fonts are using
 * UNICODE/UTF-8 charset.
 * 
 * \param language The language name. So far, you can specify the language 
 *        as 'zh_CN' (for simlified Chinese) 
 *        or 'zh_TW' (for tranditional Chinese).
 *
 * \return The pointer to the localized system text array.
 *
 * \sa SysText, local_SysText
 */
extern MG_EXPORT const char** GUIAPI GetSysTextInUTF8 (const char* language);

#endif /* _MGCHARSET_UNICODE */

    /** @} end of sys_text */

    /**
     * \defgroup str_helpers String operation helpers
     * @{
     */

/**
 * \fn char* strnchr (const char* s, size_t n, int c);
 * \brief Locates character in the first \a n characters of string \a s.
 *
 * \param s The pointer to the string.
 * \param n The number of first characters will be searched.
 * \param c The expected character.
 *
 * \return Returns a pointer to the first occurrence of the character \a c 
 *         in the string \a s.
 *
 * \sa strchr(3)
 */
MG_EXPORT char* strnchr (const char* s, size_t n, int c);

/**
 * \fn int substrlen (const char* text, int len, int delimiter,\
                int* nr_delim)
 * \brief Locates a substring delimited by one or more delimiters in the 
 * first \a len characters of string \a text.
 *
 * \param text The pointer to the string.
 * \param len The number of first characters will be searched.
 * \param delimiter The delimiter which delimites the substring from other.
 * \param nr_delim  The number of continuous delimiters will be returned 
 *        through this pointer.
 *
 * \return The length of the substring.
 *
 * \sa strstr(3)
 */
MG_EXPORT int substrlen (const char* text, int len, int delimiter,
                int* nr_delim);

/**
 * \fn char* strtrimall (char* src);
 * \brief Deletes all space characters.
 *
 * This function deletes the blank space, form-feed('\\f'), newline('\\n'), 
 * carriage return('\\r'), horizontal tab('\\t'),and vertical tab('\\v') 
 * in the head and the tail of the string.
 *
 * \param src The pointer to the string.
 *
 * \return Returns a pointer to the string.
 */
MG_EXPORT char * strtrimall (char* src);

    /** @} end of str_helpers */

    /** @} end of global_fns */

    /** @} end of fns */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_MINIGUI_H */

