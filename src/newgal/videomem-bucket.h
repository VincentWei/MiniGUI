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

#ifndef _VIDEOMEM_BUCKET_H
#define _VIDEOMEM_BUCKET_H

#include "list.h"

typedef struct _memblock {
    struct list_head list;
    int offset;
    int height; /* XXX: There is no need to know width at all */
    int pitch;
#define VMBLOCK_FLAG_USED 0x01
    int flag;
} gal_vmblock_t;

typedef struct _membucket {
    struct list_head block_head;
    unsigned char *start;
    int size;
} gal_vmbucket_t;

int gal_vmbucket_init(gal_vmbucket_t *bucket, unsigned char *start, int size);
void gal_vmbucket_destroy(gal_vmbucket_t *bucket);
gal_vmblock_t *gal_vmbucket_alloc(gal_vmbucket_t *bucket, int pitch, int height);
void gal_vmbucket_free(gal_vmbucket_t *bucket, gal_vmblock_t *block);

#endif
