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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _DEBUG

#include "common.h"
#include "minigui.h"
#include "constants.h"

#ifdef _MGRM_PROCESSES

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>

#include "misc.h"
#include "newgal.h"

/*
 * Create a RGB surface in shared memory in the appropriate depth and pixel format
 */
GAL_Surface * GAL_CreateRGBSurfaceInShm (const char* shm_name, BOOL create,
        Uint32 rw_modes, Uint32 hdr_size, int width, int height, int depth,
        Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    int fd = -1, pitch;
    size_t map_size, offset;
    uint8_t* buff = NULL;
    GAL_Surface* surface = NULL;

    if (width <= 0 || height <= 0)
        return NULL;

    if (create) {
        fd = shm_open (shm_name, O_CREAT | O_RDWR | O_CLOEXEC, rw_modes);
    }
    else {
        fd = shm_open (shm_name, O_RDWR | O_CLOEXEC, rw_modes);
    }

    if (fd <= 0) {
        _DBG_PRINTF ("failed to call shm_open: %m\n");
        return NULL;
    }

    pitch = width * ((depth + 7) / 8);
    pitch = (pitch + 3) & ~3;

    if (hdr_size) {
        int nr_hdr_lines = hdr_size / pitch;
        if (hdr_size % pitch)
            nr_hdr_lines++;

        map_size = pitch * (height + nr_hdr_lines);
        offset = pitch * nr_hdr_lines;
    }
    else {
        map_size = pitch * height;
        offset = 0;
    }

    _DBG_PRINTF ("map_size for depth (%d), width (%d), height (%d): %lu\n",
            depth, width, height, map_size);

    if (create && ftruncate (fd, map_size)) {
        close (fd);
        _DBG_PRINTF ("failed to call ftruncate: %m\n");
        goto error;
    }
    else {
        off_t file_size = lseek (fd, 0, SEEK_END);
        if (file_size != map_size) {
            _DBG_PRINTF ("file size (%lu) does not match the map size (%lu), "
                    "width (%d), height (%d), depth (%d)\n",
                    file_size, map_size, width, height, depth);
            goto error;
        }
    }

    buff = mmap (NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buff == MAP_FAILED) {
        _DBG_PRINTF ("failed to call mmap: %m\n");
        buff = NULL;
        goto error;
    }

    /* create a null size software surface first */
    surface = GAL_CreateRGBSurface (GAL_SWSURFACE, 0, 0, depth,
            Rmask, Gmask, Bmask, Amask);
    if (surface == NULL) {
        _DBG_PRINTF ("failed to create null surface\n");
        goto error;
    }

    /* Get the pixels */
    surface->flags |= GAL_PREALLOC;
    surface->w = width;
    surface->h = height;
    surface->pitch = pitch;
    surface->pixels_off = offset;
    surface->pixels = buff + offset;
    surface->hwdata = (void*)map_size;  // use hwdata for map_size

    close (fd);
    return surface;

error:
    if (surface) {
        surface->pixels = NULL;
        GAL_FreeSurface (surface);
    }

    if (buff) {
        munmap (buff, map_size);
    }

    if (fd >= 0) {
        close (fd);
        shm_unlink (shm_name);
    }

    return NULL;
}

#endif /* defined _MGRM_PROCESSES */

