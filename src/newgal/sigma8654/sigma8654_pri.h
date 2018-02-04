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
/*
**  $Id: em86gfx.h 7351 2007-08-16 04:55:58Z xgwang $
**  
**  em86gfx.c: NEWGAL driver for EM86xx GFX.
**
**  Copyright (C) 2007 Feynman Software.
*/

#ifndef _sigma8654_pri_h_ 
#define _sigma8654_pri_h_

#ifndef ALLOW_OS_CODE
#define ALLOW_OS_CODE
#endif 
#define EM86XX_CHIP EM86XX_CHIPID_TANGO3
#define EM86XX_REVISION 3
#define WITH_THREADS

#include "dcc/include/dcc.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct _Sigma8654Private_HWSurface_hwdata
{
    RMuint32 gfx_addr;
    RMint32 size;
    RMuint32 unmapped_addr;
    RMuint32 surface_addr;
    struct DCCVideoSource* osd_source;
}Sigma8654Private_HWSurface_hwdata;


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _GAL_GFXVIDEO_H */
