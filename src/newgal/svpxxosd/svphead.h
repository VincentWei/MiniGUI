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

#ifndef _SVPHEAD_H
#define _SVPHEAD_H

#define      Successful           0
#define      Fail                 1

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef struct
{
    uint32 vmiScreenWidth;
    uint32 vmiScreenHeight;
    uint32 vmiBpp;
    uint32 vmiStride;
} VIDEO_MODE_INFO;

typedef struct 
{
    int w;
    int h;
}XY_SIZE;
/******************************************************************************
 * Definition of W99661 application mode
 ******************************************************************************/
#define	     BPP_4			4
#define	     BPP_8			8
#define	     BPP_16			16

#define      AP_ENC                     0x01
#define      AP_DEC                     0x02
#define      AP_ENC_DEC                 0x03
#define      AP_GFX_ENC                 0x11
#define      AP_GFX_DEC                 0x12
#define      AP_GFX_ENC_DEC             0x13
#define      AP_EXT_CODEC_ENC           0x21
#define      AP_EXT_CODEC_DEC           0x22
#define      AP_EXT_CODEC_ENC_DEC       0x23
#define      AP_EXT_CODEC_GFX_ENC       0x31
#define      AP_EXT_CODEC_GFX_DEC       0x32
#define	     AP_EXT_CODEC_GFX_ENC_DEC	0x33

///////////////////////////////////////////////////////////////////////////////
// Definition of video formats
///////////////////////////////////////////////////////////////////////////////
#define      PIC_UNSUPPORTED      0
#define      SUB_QCIF             1
#define      QCIF                 2
#define      CIF                  3
#define      FOUR_CIF             4
#define      PIC_128_96           1
#define      PIC_176_144          2
#define      PIC_352_288          3
#define	     PIC_704_576	  4
#define	     PIC_640_480	  5
#define	     PIC_704_240	  6
#define	     PIC_704_288	  7
#define	     PIC_VGA		  5
#define	     PIC_NTSC		  6
#define	     PIC_PAL		  7

///////////////////////////////////////////////////////////////////////////////
// Definition of display mode parameters
///////////////////////////////////////////////////////////////////////////////
#define	     MODE_BPP_4		  0	// 16  colors
#define	     MODE_BPP_8		  1	// 256 colors
#define	     MODE_BPP_16	  2	// 64K colors

#define	     INDEX_COLOR	  0	// indexed color mode
#define	     DIRECT_COLOR	  1	// direct color mode

#define      TVSTD_UNDEFINED      0
#define      TVSTD_NTSC           1
#define      TVSTD_PAL            2
#define      TVSTD_PALM           3

#define	     TVCON_SVIDEO	  1	// S video output
#define	     TVCON_COMP		  2	// compositive video output


uint8
SetW9961Application (uint8 ap_id, uint8 max_gfx_bpp, uint8 max_cap_fmt,
		     uint8 max_enc_fmt, uint8 max_dec_fmt);

uint8 SetDisplayMode (uint8 mode_id);

uint8 GetVideoModeInfo (VIDEO_MODE_INFO * info);

uint32 GetVideoBaseAddr (void);
uint8 GetDesktopSize(XY_SIZE * size);

uint8 sETpALETTE (int first, GAL_Color *in_color, int count);

#endif /* _SVPHEAD_H*/
