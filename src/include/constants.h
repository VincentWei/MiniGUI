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
** constants.h: this head file declares common internal constants.
**
** Create date: 2020/01/06
*/

#ifndef GUI_CONSTANTS_H
    #define GUI_CONSTANTS_H

#include "common.h"

/* conditional macros for compositing/sharedfb schema */
#if defined(_MGRM_PROCESSES) && defined(_MGSCHEMA_COMPOSITING)
#   define IS_COMPOSITING_SCHEMA    1
#else
#   define IS_COMPOSITING_SCHEMA    0
#endif

#if defined(_MGRM_PROCESSES) && defined(_MGSCHEMA_SHAREDFB)
#   define IS_SHAREDFB_SCHEMA_PROCS 1
#else
#   define IS_SHAREDFB_SCHEMA_PROCS 0
#endif

/* the maximal number of the dirty rects */
#define NR_DIRTY_RECTS              16

/* constants for layers and z-order nodes */
#define MAX_NR_LAYERS               8

/* Since 5.0.0: the number of the fixed znodes  - DONOT adjust */
#define NR_FIXED_ZNODES             1

/* the numbers of znodes in different levels - should be mulitples of 8 */
#define DEF_NR_POPUPMENUS           16
#define DEF_NR_TOOLTIPS             8
#define DEF_NR_GLOBALS              15      // reserve slots for fixed znodes
#define DEF_NR_SCREENLOCKS          8
#define DEF_NR_DOCKERS              8
#define DEF_NR_TOPMOSTS             16
#define DEF_NR_NORMALS              128
#define DEF_NR_LAUNCHERS            8

/* Since 5.0.0: the number of the zorder levels - DONOT adjust */
#define NR_ZORDER_LEVELS            7
enum {
    ZLIDX_TOOLTIP = 0,
    ZLIDX_GLOBAL,
    ZLIDX_SCREENLOCK,
    ZLIDX_DOCKER,
    ZLIDX_TOPMOST,
    ZLIDX_NORMAL,
    ZLIDX_LAUNCHER,
};

/* Since 5.0.0; the znode index for fixed znodes - DONOT adjust */
#define ZNIDX_DESKTOP               0

/* The heap size for mask rectangles */
#define DEF_NR_MASKRECT             1024

#if IS_COMPOSITING_SCHEMA
/* max number of shared surfaces */
#   define MAX_NR_SHARED_SURF      (DEF_NR_NORMALS * (MAX_NR_LAYERS >> 1))
#endif

/* Since 5.0.0; radius for round corners */
#define RADIUS_WINDOW_CORNERS       7
#define RADIUS_POPUPMENU_CORNERS    7

/* Since 5.0.0; default offsets for overlapped windows */
#define DEF_OVERLAPPED_OFFSET_X     30
#define DEF_OVERLAPPED_OFFSET_Y     30

/* constants for MiniGUI-Processes runtime mode */
#ifdef _MGRM_PROCESSES

/* the lock file */
#define LOCKFILE                    "/var/tmp/mginit"
/* the well-known Unix domain socket name */
#define CS_PATH                     "/var/tmp/minigui"

/* key base for System V IPC objects */
#define IPC_KEY_BASE                0x464D4700

/* Live threshold: 5 seconds */
#define THRES_LIVE                  500

/* timeout value for socket I/O: 1 second */
#define TO_SOCKIO                   100

#endif /* _MGRM_PROCESSES */

/* constants for clip rects heap and message heap */
#if defined (__NOUNIX__) || defined (__uClinux__)
  #define DEF_MSGQUEUE_LEN    32
  #define SIZE_CLIPRECTHEAP   16
  #define SIZE_INVRECTHEAP    32
  #define SIZE_QMSG_HEAP      32
#else
  #define DEF_MSGQUEUE_LEN    NR_BITS_DWORD
  #define SIZE_CLIPRECTHEAP   NR_BITS_DWORD
  #define SIZE_INVRECTHEAP    NR_BITS_DWORD
  #define SIZE_QMSG_HEAP      NR_BITS_DWORD
#endif

/* constants for fix string module */
#if defined (__NOUNIX__) || defined (__uClinux__)
  #define MAX_LEN_FIXSTR      64
  #define NR_HEAP             5
  #define LEN_BITMAP          (1+2+4+8+16)
#else
 #ifdef _MGRM_THREADS
  #define MAX_LEN_FIXSTR      2048
  #define NR_HEAP             10
  #define LEN_BITMAP          (1+2+4+8+16+32+64+128+256+512)
 #else
  #define MAX_LEN_FIXSTR      1024
  #define NR_HEAP             9
  #define LEN_BITMAP          (1+2+4+8+16+32+64+128+256)
 #endif
#endif

/* constants for engine name and video mode */
#define LEN_SO_NAME         127 // name length of a shared object (library)
#define LEN_DEVICE_NAME     127
#define LEN_EXDRIVER_NAME   LEN_SO_NAME
#define LEN_ENGINE_NAME     23
#define LEN_VIDEO_MODE      31
#define LEN_MTYPE_NAME      23
#define LEN_FOURCC_FORMAT   7

/* misc constants and utilities */

/* Number of timers. */
#define DEF_NR_TIMERS           NR_BITS_DWORD
#define USEC_1S                 1000000UL
#define USEC_10MS               10000UL
#define USEC_TIMEOUT            300000UL    // 300ms
#define MAX_IDLE_COUNTER        (USEC_TIMEOUT/USEC_10MS)
#define DESKTOP_TIMER_INERTVAL  10UL        // every 10 ticks, 100ms

/* round n to multiple of m */
#define ROUND_TO_MULTIPLE(n, m) (((n) + (((m) - 1))) & ~((m) - 1))

#endif /* GUI_CONSTANTS_H */

