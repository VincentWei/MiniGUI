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

#ifndef _3DFX_REGS_H
#define _3DFX_REGS_H

/* This information comes from the public 3Dfx specs for the Voodoo 3000 */

/* mapped_io register offsets */
#define TDFX_STATUS	0x00

#define INTCTRL		(0x00100000 + 0x04)
#define CLIP0MIN	(0x00100000 + 0x08)
#define CLIP0MAX	(0x00100000 + 0x0c)
#define DSTBASE		(0x00100000 + 0x10)
#define DSTFORMAT	(0x00100000 + 0x14)
#define SRCCOLORKEYMIN	(0x00100000 + 0x18)
#define SRCCOLORKEYMAX	(0x00100000 + 0x1c)
#define DSTCOLORKEYMIN	(0x00100000 + 0x20)
#define DSTCOLORKEYMAX	(0x00100000 + 0x24)
#define BRESERROR0	(0x00100000 + 0x28)
#define BRESERROR1	(0x00100000 + 0x2c)
#define ROP_2D		(0x00100000 + 0x30)
#define SRCBASE		(0x00100000 + 0x34)
#define COMMANDEXTRA_2D	(0x00100000 + 0x38)
#define PATTERN0	(0x00100000 + 0x44)
#define PATTERN1	(0x00100000 + 0x48)
#define CLIP1MIN	(0x00100000 + 0x4c)
#define CLIP1MAX	(0x00100000 + 0x50)
#define SRCFORMAT	(0x00100000 + 0x54)
#define SRCSIZE		(0x00100000 + 0x58)
#define SRCXY		(0x00100000 + 0x5c)
#define COLORBACK	(0x00100000 + 0x60)
#define COLORFORE	(0x00100000 + 0x64)
#define DSTSIZE		(0x00100000 + 0x68)
#define DSTXY		(0x00100000 + 0x6c)
#define COMMAND_2D	(0x00100000 + 0x70)
#define LAUNCH_2D	(0x00100000 + 0x80)
#define PATTERNBASE	(0x00100000 + 0x100)

#define COMMAND_3D	(0x00200000 + 0x120)

/* register bitfields (not all, only as needed) */

#define BIT(x) (1UL << (x))

#define COMMAND_2D_BITBLT		0x01
#define COMMAND_2D_FILLRECT		0x05
#define COMMAND_2D_LINE			0x06
#define COMMAND_2D_POLYGON_FILL		0x08
#define COMMAND_2D_INITIATE		BIT(8)
#define COMMAND_2D_REVERSELINE		BIT(9)
#define COMMAND_2D_STIPPLELINE		BIT(12)
#define COMMAND_2D_MONOCHROME_PATT	BIT(13)
#define COMMAND_2D_MONOCHROME_TRANSP    BIT(16)

#define COMMAND_3D_NOP			0x00

#define STATUS_RETRACE			BIT(6)
#define STATUS_BUSY			BIT(9)

#endif /* _3DFX_REGS_H */

