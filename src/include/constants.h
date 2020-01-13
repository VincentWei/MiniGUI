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

#define DEF_NR_TIMERS           NR_BITS_DWORD

#if defined(_MGRM_PROCESSES) && defined(_MGUSE_COMPOSITING)
#   define IS_COMPOSITING_SCHEMA    1
#   define NR_DIRTY_RECTS           8
#   define NR_COMPOSITING_SEMS      64
#else
#   define IS_COMPOSITING_SCHEMA    0
#   define NR_DIRTY_RECTS           0
#   define NR_COMPOSITING_SEMS      0
#endif

#if defined(_MGRM_PROCESSES) && defined(_MGUSE_SHAREDFB)
#   define IS_SHAREDFB_SCHEMA       1
#else
#   define IS_SHAREDFB_SCHEMA       0
#endif

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
  #define MAX_LEN_FIXSTR      64
  #define NR_HEAP             5
  #define LEN_BITMAP          (1+2+4+8+16)
 #endif
#endif

/* constants for engine name and video mode */
#define LEN_DEVICE_NAME     127
#define LEN_EXDRIVER_NAME   127

#define LEN_ENGINE_NAME     23
#define LEN_VIDEO_MODE      31
#define LEN_MTYPE_NAME      23
#define LEN_FOURCC_FORMAT   7

#define ROUND_TO_MULTIPLE(n, m) (((n) + (((m) - 1))) & ~((m) - 1))

// misc constants
// #define MAX_SRV_CLIP_RECTS      8

#endif // GUI_CONSTANTS_H

