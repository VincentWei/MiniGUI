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
 *   Copyright (C) 2023, Beijing FMSoft Technologies Co., Ltd.
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
** The graphics display interface module of MiniGUI.
**
** Current maintainer: Wei Yongming.
**
** Create date: 1999.01.03
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGSCHEMA_COMPOSITING

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "cursor.h"
#include "internals.h"
#include "inline.h"
#include "dc.h"
#include "debug.h"
#include "client.h"
#include "server.h"

HSURF GUIAPI CreateSharedSurface(GHANDLE video, const char *name, DWORD flags,
        int width, int height, int depth,
        Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    GAL_Surface *nssurf = NULL;

    if (name) {
        OPERATENSSURFINFO nssurf_req = {};

        if (strlen(name) + 1 > sizeof(nssurf_req.name)) {
            _ERR_PRINTF("TOOLONG: too long name for shared surface: %s\n",
                    name);
            goto failed;
        }

        strcpy(nssurf_req.name, name);
        nssurf_req.id_op = ID_NAMEDSSURFOP_REGISTER;

        int result;
        if (IsServer()) {
            result = __mg_nssurf_map_operate_srv(&nssurf_req, 0, -1);
        }
        else {
            REQUEST req;

            req.id = REQID_OPERATENAMEDSSURF;
            req.data = &nssurf_req;
            req.len_data = sizeof(OPERATENSSURFINFO);

            if (ClientRequest(&req, &result, sizeof(result))) {
                _ERR_PRINTF("BAD_REQUEST: when registering name for a new shared surface\n");
                goto failed;
            }
        }

        if (result) {
            _ERR_PRINTF("FAILED_REQUEST: when registering name of shared surface\n");
            goto failed;
        }
    }

    nssurf = GAL_CreateSharedRGBSurface(video, flags, 0666, width, height, depth,
            Rmask, Gmask, Bmask, Amask);
    if (nssurf == NULL) {
        goto failed;
    }

    if (name) {
        strcpy(nssurf->shared_header->name, name);

        OPERATENSSURFINFO nssurf_req = {};
        strcpy(nssurf_req.name, name);
        nssurf_req.id_op = ID_NAMEDSSURFOP_SET;
        nssurf_req.map_size = nssurf->shared_header->map_size;

        int result;
        if (IsServer()) {
            result = __mg_nssurf_map_operate_srv(&nssurf_req, 0,
                    nssurf->shared_header->fd);
        }
        else {
            REQUEST req;

            req.id = REQID_OPERATENAMEDSSURF;
            req.data = &nssurf_req;
            req.len_data = sizeof(OPERATENSSURFINFO);

            if (ClientRequestEx2(&req, NULL, 0, nssurf->shared_header->fd,
                        &result, sizeof(result), NULL)) {
                _ERR_PRINTF("BAD_REQUEST: when setting fd of the new shared surface.\n");
                goto failed;
            }
        }

        if (result) {
            _ERR_PRINTF("FAILED_REQUEST: when setting fd of the shared surface\n");
            goto failed;
        }
    }
    else {
        nssurf->shared_header->name[0] = 0;
    }

    return nssurf;

failed:
    if (nssurf)
        GAL_FreeSurface(nssurf);

    return NULL;
}

BOOL GUIAPI DestroySharedSurface(HSURF surf)
{
    if (surf->shared_header == NULL) {
        _ERR_PRINTF("INVALID_ARG: surface handle: %p\n", surf);
        goto failed;
    }

    if (surf->shared_header->creator == getpid() &&
            surf->shared_header->name[0]) {
        /* This is a named shared surface created by this client */

        OPERATENSSURFINFO nssurf_req = {};
        strcpy(nssurf_req.name, surf->shared_header->name);
        nssurf_req.id_op = ID_NAMEDSSURFOP_REVOKE;

        int result;
        if (IsServer()) {
            result = __mg_nssurf_map_operate_srv(&nssurf_req, 0, -1);
        }
        else {
            REQUEST req;

            req.id = REQID_OPERATENAMEDSSURF;
            req.data = &nssurf_req;
            req.len_data = sizeof(OPERATENSSURFINFO);

            if (ClientRequest(&req, &result, sizeof(result))) {
                _ERR_PRINTF("BAD_REQUEST: when revoking name of a shared surface.\n");
                goto failed;
            }
        }

        if (result) {
            _ERR_PRINTF("FAILED_REQUEST: when revoking name of a shared surface\n");
            goto failed;
        }
    }

    GAL_FreeSurface(surf);
    return TRUE;

failed:
    return FALSE;
}

static int get_shared_surface(const char *name, size_t *map_size, DWORD *flags)
{
    int fd = -1;
    SHAREDSURFINFO info;

    if (IsServer()) {
        fd = __mg_get_shared_surface_srv(name, &info);
    }
    else {
        REQUEST req;

        req.id = REQID_GETSHAREDSURFACE;
        req.data = name;
        req.len_data = strlen(name) + 1;

        if (ClientRequestEx2(&req, NULL, 0, -1,
                &info, sizeof(SHAREDSURFINFO), &fd) < 0) {
            _ERR_PRINTF("BAD_REQUEST: when getting shared surface: %s\n", name);
            goto failed;
        }

        if (fd < 0) {
            _ERR_PRINTF("FAILED_REQUEST: when getting shared surface: %s\n", name);
            goto failed;
        }

        _DBG_PRINTF("REQID_GETSHAREDSURFACE: size (%lu), flags (0x%x), fd: %d\n",
                    info.size, info.flags, fd);

    }

    if (fd >= 0) {
        if (map_size)
            *map_size = info.size;
        if (flags)
            *flags = info.flags;
    }
    return fd;

failed:
    return -1;
}

int GUIAPI GetSharedSurfaceFDByClientWindow(int client, HWND hwnd,
        size_t *map_size, DWORD *flags)
{
    char itn_name[sizeof(APPSF_NAME_PREFIX) + NAME_MAX + 1];

    int ret = snprintf(itn_name, sizeof(itn_name), APPSF_HWND_PATTER,
            client, hwnd);
    if (ret <= 0 || (size_t)ret >= sizeof(itn_name)) {
        return -1;
    }

    return get_shared_surface(itn_name, map_size, flags);
}

int GUIAPI GetSharedSurfaceFDByName(const char *name,
        size_t *map_size, DWORD *flags)
{
    char itn_name[sizeof(APPSF_NAME_PREFIX) + NAME_MAX + 1];

    int ret = snprintf(itn_name, sizeof(itn_name), APPSF_NAME_PATTER, name);
    if (ret <= 0 || (size_t)ret >= sizeof(itn_name)) {
        return -1;
    }

    return get_shared_surface(itn_name, map_size, flags);
}

#endif  /* defined _MGSCHEMA_COMPOSITING */

