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
#   define NR_DIRTY_RECTS           8
#else
#   define IS_COMPOSITING_SCHEMA    0
#   define NR_DIRTY_RECTS           0
#endif

#if defined(_MGRM_PROCESSES) && defined(_MGSCHEMA_SHAREDFB)
#   define IS_SHAREDFB_SCHEMA       1
#else
#   define IS_SHAREDFB_SCHEMA       0
#endif

/* constants for layers and z-order nodes */
#define MAX_NR_LAYERS               8

#ifdef _MGRM_PROCESSES
#   define DEF_NR_GLOBALS           16
#else
#   define DEF_NR_GLOBALS           0
#endif

/* number of znodes */
#define DEF_NR_MASKRECT             1024
#define DEF_NR_POPUPMENUS           16
#define DEF_NR_TOPMOSTS             16
#define DEF_NR_NORMALS              128
#define DEF_NR_FIXEDZNODES          4

/* Since 4.2.0; the fixed znode index for special main window */
#define ZNIDX_DESKTOP           0
#define ZNIDX_SCREENLOCK        1
#define ZNIDX_DOCKER            2
#define ZNIDX_LAUNCHER          3

/* constants for MiniGUI-Processes runtime mode */
#ifdef _MGRM_PROCESSES

// the lock file
#define LOCKFILE                    "/var/tmp/mginit"
// the well-known Unix domain socket name
#define CS_PATH                     "/var/tmp/minigui"

// key base for System V IPC
#define IPC_KEY_BASE                0x464D4700

// Live threshold: 5 seconds
#define THRES_LIVE                  500

// timeout value for socket I/O: 1 second
#define TO_SOCKIO                   100

#endif /* _MGRM_PROCESSES */

/* constants for clip rects heap and message heap */
#if defined (__NOUNIX__) || defined (__uClinux__)
  #define DEF_MSGQUEUE_LEN    32
  #define SIZE_CLIPRECTHEAP   16
  #define SIZE_INVRECTHEAP    32
  #define SIZE_QMSG_HEAP      32
#else
 #ifndef _MGRM_THREADS
  #define DEF_MSGQUEUE_LEN    32
  #define SIZE_CLIPRECTHEAP   16
  #define SIZE_INVRECTHEAP    32
  #define SIZE_QMSG_HEAP      32
 #else
  #define DEF_MSGQUEUE_LEN    32
  #define SIZE_CLIPRECTHEAP   32
  #define SIZE_INVRECTHEAP    64
  #define SIZE_QMSG_HEAP      32
 #endif
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
#define LEN_DEVICE_NAME     127
#define LEN_EXDRIVER_NAME   127
#define LEN_ENGINE_NAME     23
#define LEN_VIDEO_MODE      31
#define LEN_MTYPE_NAME      23
#define LEN_FOURCC_FORMAT   7

/* misc constants and utilities */

// Number of timers.
#define DEF_NR_TIMERS           NR_BITS_DWORD

// maximal lenght for z-order node caption
#define MAX_CAPTION_LEN         39

// round n to multiple of m
#define ROUND_TO_MULTIPLE(n, m) (((n) + (((m) - 1))) & ~((m) - 1))

#endif // GUI_CONSTANTS_H

