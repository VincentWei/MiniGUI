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
** endianrw.c: 
**  Functions for dynamically reading and writing endian-specific values
**
** The idea and some code come from SDL - Simple DirectMedia Layer
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/09/18
*/

#include <stdio.h>

#include "common.h"
#include "endianrw.h"

Uint16 MGUI_ReadLE16 (MG_RWops *src)
{
	Uint16 value;

	MGUI_RWread(src, &value, (sizeof value), 1);
	return(ArchSwapLE16(value));
}

Uint16 MGUI_ReadBE16 (MG_RWops *src)
{
	Uint16 value;

	MGUI_RWread(src, &value, (sizeof value), 1);
	return(ArchSwapBE16(value));
}

Uint32 MGUI_ReadLE32 (MG_RWops *src)
{
	Uint32 value;

	MGUI_RWread(src, &value, (sizeof value), 1);
	return(ArchSwapLE32(value));
}

Uint32 MGUI_ReadBE32 (MG_RWops *src)
{
	Uint32 value;

	MGUI_RWread(src, &value, (sizeof value), 1);
	return(ArchSwapBE32(value));
}

Uint64 MGUI_ReadLE64 (MG_RWops *src)
{
	Uint64 value;

	MGUI_RWread(src, &value, (sizeof value), 1);
	return(ArchSwapLE64(value));
}

Uint64 MGUI_ReadBE64 (MG_RWops *src)
{
	Uint64 value;

	MGUI_RWread(src, &value, (sizeof value), 1);
	return(ArchSwapBE64(value));
}

int MGUI_WriteLE16 (MG_RWops *dst, Uint16 value)
{
	value = ArchSwapLE16(value);
	return(MGUI_RWwrite(dst, &value, (sizeof value), 1));
}

int MGUI_WriteBE16 (MG_RWops *dst, Uint16 value)
{
	value = ArchSwapBE16(value);
	return(MGUI_RWwrite(dst, &value, (sizeof value), 1));
}

int MGUI_WriteLE32 (MG_RWops *dst, Uint32 value)
{
	value = ArchSwapLE32(value);
	return(MGUI_RWwrite(dst, &value, (sizeof value), 1));
}

int MGUI_WriteBE32 (MG_RWops *dst, Uint32 value)
{
	value = ArchSwapBE32(value);
	return(MGUI_RWwrite(dst, &value, (sizeof value), 1));
}

int MGUI_WriteLE64 (MG_RWops *dst, Uint64 value)
{
	value = ArchSwapLE64(value);
	return(MGUI_RWwrite(dst, &value, (sizeof value), 1));
}

int MGUI_WriteBE64 (MG_RWops *dst, Uint64 value)
{
	value = ArchSwapBE64(value);
	return (MGUI_RWwrite(dst, &value, (sizeof value), 1));
}

Uint16 MGUI_ReadLE16FP (FILE *src)
{
	Uint16 value;
    size_t size;

	size = fread (&value, (sizeof value), 1, src);
    if (size < 1)
        return -1;

	return (ArchSwapLE16(value));
}

Uint32 MGUI_ReadLE32FP (FILE *src)
{
	Uint32 value;
	size_t size;

    size = fread(&value, (sizeof value), 1, src);
    if (size < 1)
        return -1;

	return (ArchSwapLE32(value));
}

int MGUI_WriteLE16FP (FILE *dst, Uint16 value)
{
	value = ArchSwapLE16(value);
	return (fwrite (&value, (sizeof value), 1, dst));
}

int MGUI_WriteLE32FP (FILE *dst, Uint32 value)
{
	value = ArchSwapLE32(value);
	return (fwrite (&value, (sizeof value), 1, dst));
}

