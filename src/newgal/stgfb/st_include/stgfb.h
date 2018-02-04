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
 @File   stgfb.h
 @brief  STGFB API



*/

#ifndef _STGFB_H
#define _STGFB_H

#include <stblit.h>
#include "stgxobj.h"

#define STGFB_GetRevision()  "STGFB-REL_1.1.3"

#define STGFB_DRIVER_ID      417
#define STGFB_DRIVER_BASE    (STGFB_DRIVER_ID << 16)


enum
{
    STGFB_ERROR_I2C = STGFB_DRIVER_BASE,
    STGFB_ERROR_INVALID_PARAMETER,
    STGFB_ERROR_FEATURE_NOT_SUPPORTED
};


/* Type of acceleration available --> the same as STS frame buffer*/
#define STGFB_ACCEL             100

#define STGFB_MAX_DEVICE        8
#define STGFB_MAX_MIXER_NUMBER  8

/*
 * non-standard ioctls to control the FB plane and blitter, although
 * these can be used directly they are really provided for the DirectFB
 * driver
 */


#ifndef FBIO_WAITFORVSYNC
#define FBIO_WAITFORVSYNC       _IOW('F', 0x20, u_int32_t)
#endif


typedef enum
{
  STGFB_COLOR_FORMAT_ARGB8888,
  STGFB_COLOR_FORMAT_RGB888,
  STGFB_COLOR_FORMAT_RGB565,
  STGFB_COLOR_FORMAT_ARGB1555,
  STGFB_COLOR_FORMAT_ARGB4444
} STGFB_ColorFormat_t;




/*
 *  Structure for compositor alpha
 */
typedef struct STGFB_CompositorAlpha_s
{
    int Alpha0;
    int Alpha1;
} STGFB_CompositorAlpha_t;

/*
 * Structure for Color key
 */
typedef struct STGFB_CompositorColoKey_s
{
    U32 RMin;
    U32 GMin;
    U32 BMin;

    U32 RMax;
    U32 GMax;
    U32 BMax;
} STGFB_CompositorColoKey_t;


/*
 * Structure for Positionning FB
 */
typedef struct STGFB_Position_s
{
    U32 OffsetX;
    U32 OffsetY;
} STGFB_Position_t;





/* structures and functions used DirectFB acceleration modules */
typedef enum
{
    STGFB_BLIT_UNKNOWN = 0,
    STGFB_BLIT_FILLRECTANGLE,
    STGFB_BLIT_DRAWRECTANGLE,
    STGFB_BLIT_BLIT,
    STGFB_BLIT_COPYRECTANGLE
} STGFB_BLIT_Operation_t;

typedef struct
{
    STBLIT_Handle_t         Handle;
    STGXOBJ_Bitmap_t        DestBitmap;
    STGXOBJ_Rectangle_t     Rectangle;
    STGXOBJ_Color_t         Color;
    STBLIT_BlitContext_t    Context;
} STGFB_BLIT_FillRectangleParams_t;


typedef struct
{
    STBLIT_Handle_t         Handle;
    STGXOBJ_Bitmap_t        DestBitmap;
    STGXOBJ_Rectangle_t     Rectangle;
    STGXOBJ_Color_t         Color;
    STBLIT_BlitContext_t    Context;
} STGFB_BLIT_DrawRectangleParams_t;



typedef enum STGFB_ModulationColorMode_e
{
    STGFB_MODULATION_COLOR_NONE,
    STGFB_MODULATION_COLOR_COLORIZE,
    STGFB_MODULATION_COLOR_PREMULTICOLOR
} STGFB_ModulationColorMode_t;


typedef struct
{
    STGFB_ModulationColorMode_t ModulationColorMode;
    U32                         ModulationColorCoef;
} STGFB_BLIT_Effect_t;



typedef struct
{
    STBLIT_Handle_t         Handle;

    /* source 2 */
    STBLIT_SourceType_t     Src2Type;
    void*                   Src2Data_p;
    STGXOBJ_Rectangle_t     Src2Rectangle;


	/* source */
    STBLIT_SourceType_t     SrcType;
    void*                   SrcData_p;
    STGXOBJ_Rectangle_t     SrcRectangle;

    /* destination */
    STGXOBJ_Bitmap_t        DestBitmap;
    STGXOBJ_Rectangle_t     DestRectangle;

    STBLIT_BlitContext_t    Context;
    STGFB_BLIT_Effect_t     Effect;
    void                    *Palette_p;
} STGFB_BLIT_BlitParams_t;

typedef struct
{
    STGFB_BLIT_Operation_t Operation;
	union
	{
        STGFB_BLIT_FillRectangleParams_t    FillRectangle;
        STGFB_BLIT_DrawRectangleParams_t    DrawRectangle;
        STGFB_BLIT_BlitParams_t             Blit;
    } Params;
    ST_ErrorCode_t Result;

} STGFB_BLIT_Command_t;


typedef struct
{
    int                     x, y;
    int                     w, h;
} STGFB_Rectangle_t;



#define STGFB_IO_SET_OVERLAY_COLORKEY       _IOW('B', 0x1, int)
#define STGFB_IO_ENABLE_FLICKER_FILTER      _IOW('B', 0x2, int)
#define STGFB_IO_DISABLE_FLICKER_FILTER     _IOW('B', 0x3, int)
#define STGFB_IO_BLIT_COMMAND               _IOW('B', 0x4, STGFB_BLIT_Command_t)
#define STGFB_IO_SET_OVERLAY_ALPHA          _IOW('B', 0x5, int)
#define STGFB_IO_GET_LAYER_HANDLE           _IOW('B', 0x6, int)
#define STGFB_IO_GET_VIEWPORT_HANDLE        _IOW('B', 0x7, int)
#define STGFB_IO_SET_FB_POSITION            _IOW('B', 0x8, int)
#define STGFB_IO_SYNC_BLITTER               _IOW('B', 0x9, int)
#define STGFB_IO_SET_WINDOW                 _IOW('B', 0x10, int)
#define STGFB_IO_ENABLE_LAYER               _IOW('B', 0x11, int)
#define STGFB_IO_SET_PREMULTIPLIED          _IOW('B', 0x12, int)



#endif /* STGFB_H */
