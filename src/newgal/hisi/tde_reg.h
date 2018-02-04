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
** $Id: tde_reg.h 7268 2007-07-03 08:08:43Z xwyan $
**
** tde_reg.h: header file. 
**
** Copyright (C) 2007 Feynman Software.
**
** All rights reserved by Feynman Software.
*/

#ifndef __TDE_REG_H__
#define __TDE_REG_H__

/* 2D registers virtual base address */
extern U32 TDE_REG_BASE;

/* 2D registers physical base address */
#define TDE_REG_BASE_ADDR  0x90010300
#define TDE_REG_BASE_SIZE  (0x80+4)    /* 0x0~0x80 */

//#define TDE_REG_TDE_CTRL            (TDE_REG_BASE + 0x00)
typedef union
{
    struct 
    {
        U32 tde_start             :1;
        U32                       :1;
        U32 rop_code              :4;
        U32                       :1;

        //U32 tde_cmd             :6;
        U32 tde_cmd_OutAlphaFrom  :1;
        U32 tde_cmd_Data_Opt      :2;
        U32 tde_cmd_Colorspace    :1;
        U32 tde_cmd_Cmd           :2;

        U32                       :3;
        U32 alpha                 :8;
        U32 out_color_format      :4;
        U32 in_color_format       :4;
    }bits;

    U32 ul32;
}TDE_Reg_Control;

//#define TDE_REG_SIZE            (TDE_REG_BASE + 0x04)
typedef union
{
    struct 
    {
        U32 Blt_Width        :12;
        U32 Blt_Height       :12;
        U32 reserved1        :8;
    }bits;

    U32 ul32;
}TDE_Reg_Size;

//#define TDE_REG_SRC_ADDR        (TDE_REG_BASE + 0x08)
//#define TDE_REG_DST_ADDR        (TDE_REG_BASE + 0x0c)
//#define TDE_REG_OUT_ADDR        (TDE_REG_BASE + 0x10)
//#define TDE_REG_SRC_DST_STRIDE    (TDE_REG_BASE + 0x14)
typedef union
{
    struct 
    {
        U32 Dst_Stride        :16;
        U32 Src_Stride        :16;
    }bits;

    U32 ul32;
}TDE_Reg_SrcDstStride;

//#define TDE_REG_MISC    (TDE_REG_BASE + 0x18)
typedef union
{
    struct 
    {
        U32 Gout_Stride      :16;
        U32 Pattern_height   :5;
        U32 Pattern_width    :5;
        U32                  :4;
        U32 Endian_sel       :1;
        U32 Fifo_wrpretect   :1;
    }bits;

    U32 ul32;
}TDE_Reg_Misc;

//#define TDE_REG_COLOR_FILLED        (TDE_REG_BASE + 0x1C)
typedef union
{
    struct 
    {
        U32 Color_filled_B    :8;
        U32 Color_filled_G    :8;
        U32 Color_filled_R    :8;
        U32                   :8;
    }bits;

    U32 ul32;
}TDE_Reg_ColorFilled;

//#define TDE_REG_COLOR_SPACE_MAX    (TDE_REG_BASE + 0x20)
typedef union
{
    struct 
    {
        U32 Color_filled_B    :8;
        U32 Color_filled_G    :8;
        U32 Color_filled_R    :8;
        U32                   :8;
    }bits;

    U32 ul32;
}TDE_Reg_ColorSpaceMAX;

//#define TDE_REG_COLOR_SPACE_MIN    (TDE_REG_BASE + 0x24)
typedef union
{
    struct 
    {
        U32 Color_filled_B    :8;
        U32 Color_filled_G    :8;
        U32 Color_filled_R    :8;
        U32                   :8;
    }bits;

    U32 ul32;
}TDE_Reg_ColorSpaceMIN;

//#define TDE_REG_TDE_STATUS        (TDE_REG_BASE + 0x80)
typedef union
{
    struct 
    {
        U32 Fifo_depth            :5;
        U32                       :2;
        U32 History_fifooverfall  :1;
        U32                       :24;
    }bits;

    U32 ul32;
}TDE_Reg_Status;

/* VOU's registers definition. */
typedef struct
{
    /* 0x0 */
    volatile TDE_Reg_Control  control;

    /* 0x04 */
    volatile TDE_Reg_Size     size;

    /* 0x08 ~ 0x10 */
    volatile U32 src_addr;
    volatile U32 dst_addr;
    volatile U32 out_addr;

    /* 0x14 ~ 0x18 */
    volatile TDE_Reg_SrcDstStride src_dst_stride;
    volatile TDE_Reg_Misc         misc;

    /* 0x1c */
    volatile TDE_Reg_ColorFilled    color_filled;

    /* 0x20 ~ 0x24 */
    volatile TDE_Reg_ColorSpaceMAX  colo_max;
    volatile TDE_Reg_ColorSpaceMIN  colo_min;

    /* 0x28 ~ 0x7C */
    volatile U32 reserved0[22];

    /* 0x80 */
    volatile TDE_Reg_Status status;
}TDE_Reg_Type;

#endif /* __TDE_REG_H__  */

