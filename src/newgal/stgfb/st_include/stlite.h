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
/*****************************************************************************/
/* COPYRIGHT (C) 2009 STMicroelectronics - All Rights Reserved               */
/* ST makes no warranty express or implied including but not limited to,     */
/* any warranty of                                                           */
/*                                                                           */
/*   (i)  merchantability or fitness for a particular purpose and/or         */
/*   (ii) requirements, for a particular purpose in relation to the LICENSED */
/*        MATERIALS, which is provided “AS IS”, WITH ALL FAULTS. ST does not */
/*        represent or warrant that the LICENSED MATERIALS provided here     */
/*        under is free of infringement of any third party patents,          */
/*        copyrights,trade secrets or other intellectual property rights.    */
/*        ALL WARRANTIES, CONDITIONS OR OTHER TERMS IMPLIED BY LAW ARE       */
/*        EXCLUDED TO THE FULLEST EXTENT PERMITTED BY LAW                    */
/*                                                                           */
/*****************************************************************************/
/**                                                                            
 @File  stlite.h                                                            
 @brief Includes all stlite header files, so that drivers & applications
        need only include this file.

*/
#ifndef __STLITE_H
#define __STLITE_H

#ifdef ST_OSLINUX
#include <stddefs.h>
#endif


#ifndef ST_OSLINUX /*corresponding endif at end of file*/
#ifdef ST_OS21
#include <os21.h>
#include <time.h>
#ifdef ARCHITECTURE_ST40
#include <os21/st40.h>
#endif /*#ifdef ARCHITECTURE_ST40*/
#ifdef ARCHITECTURE_ST200
#include <os21/st200.h>
#endif /*#ifdef ARCHITECTURE_ST200*/
#endif

#ifdef ST_OS20
#include <interrup.h>
#include <kernel.h>
#include <message.h>
#include <mutex.h>
#include <partitio.h>
#include <semaphor.h>
#include <task.h>
#include <ostime.h>
#include <cache.h>
#endif

/*
The macros in os_20to21_map.h are included by default.
The following DVD_EXCLUDE_OS20TO21_MACROS needs to be passed as a Cflag
from that driver's makefile, which intends to disable the macros provided
by os_20to21_map.h

As an example, the following 3 lines can be added to the makefile of that
driver/testapp, which wishes to disable these macros:

ifdef DVD_EXCLUDE_OS20TO21_MACROS
 CFLAGS  +=-DDVD_EXCLUDE_OS20TO21_MACROS
endif

*/
#ifndef DVD_EXCLUDE_OS20TO21_MACROS
#include "os_20to21_map.h" /*OS20-OS21 wrapper*/
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ARCHITECTURE_ST20
/* fix for DCU 1.8.1 - different number of parameters */
#if OS20_VERSION_MAJOR >= 2 && OS20_VERSION_MINOR >= 7
  #define interrupt_status(a,b) interrupt_status(a,b,0)
#endif
#endif

/* The following is required for (legacy) PTI and TestTool compatibility,
  which require the identifiers boolean, true, and false */

#ifdef __cplusplus
/* true and false are already defined. Define the type the same size
  as under C compilation below (DDTS 18417) */
#ifndef ST_OSWINCE
typedef int boolean;
#endif

#else
#if !defined(true) && !defined(false)
typedef enum { false = 0, true = 1 } boolean;
#endif
#endif

/****************************************************************************
 Toolset compatibility issues 
****************************************************************************/

/* ST20/ST40 __inline compatibility */
#ifndef __inline
#if defined(ARCHITECTURE_ST40) && !defined(ST_OSWINCE)
#define __inline __inline__
#endif
#endif


#ifdef __cplusplus
}
#endif

#endif /*#ifndef ST_OSLINUX*/

#endif /* __STLITE_H */
