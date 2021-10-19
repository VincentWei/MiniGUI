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
 *   Copyright (C) 2002~2021, Beijing FMSoft Technologies Co., Ltd.
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
/*
** webp.c: Low-level WebP file read routines (based on LibWebP 0.6)
**
** Current maintainer: Wei Yongming
**
** Create date: 2021/10/19
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGIMAGE_WEBP
#include "gdi.h"
#include "constants.h"

#include <webp/decode.h>

#define RIFF_HEADER_SIZE    12

#define HEADER_CHUNK_SIZE   64
#define READ_BUFF_SIZE      1024

struct webp_decode_info {
    WebPDecoderConfig   config;
    size_t              sz_data;
    char                buf[READ_BUFF_SIZE];
};

void* __mg_init_webp(MG_RWops *fp, MYBITMAP *mybmp, RGB *pal)
{
    int n;
    size_t pitch;
    struct webp_decode_info *info = NULL;

    if ((info = malloc(sizeof(struct webp_decode_info))) == NULL) {
        _ERR_PRINTF ("__mg_init_webp: failed to allocate memory\n");
        return NULL;
    }

    if (!WebPInitDecoderConfig(&info->config)) {
        _ERR_PRINTF ("__mg_init_webp: WebPInitDecoderConfig failed\n");
        goto error;
    }

    info->sz_data = 0;
    do {
        VP8StatusCode sc;

        if (info->sz_data + HEADER_CHUNK_SIZE > READ_BUFF_SIZE) {
            _ERR_PRINTF ("__mg_init_webp: two small buffer\n");
            goto error;
        }

        n = MGUI_RWread(fp, info->buf + info->sz_data, 1, HEADER_CHUNK_SIZE);
        if (n > 0) {
            info->sz_data += n;
        }
        else
            goto error;

        sc = WebPGetFeatures(info->buf, info->sz_data, &info->config.input);
        if (sc == VP8_STATUS_OK) {
            break;
        }
        else if (sc == VP8_STATUS_NOT_ENOUGH_DATA) {
            continue;
        }
        else {
            _ERR_PRINTF ("__mg_init_webp: WebPGetFeatures failed: %d\n", sc);
            goto error;
        }
    } while (1);

    if (info->config.input.has_animation) {
        _ERR_PRINTF ("__mg_init_webp: animation not supported\n");
        goto error;
    }

    mybmp->w = info->config.input.width;
    mybmp->h = info->config.input.height;
    mybmp->frames = 1;
    if (info->config.input.has_alpha) {
        info->config.output.colorspace = MODE_RGBA;

        mybmp->depth = 32;
        mybmp->flags |= MYBMP_FLOW_DOWN | MYBMP_ALPHA | MYBMP_TYPE_RGBA;
        mybmp->flags |= MYBMP_RGBSIZE_4;

        pitch = (size_t)info->config.input.width * 4;
    }
    else {
        info->config.output.colorspace = MODE_RGB;

        mybmp->depth = 24;
        mybmp->flags |= MYBMP_FLOW_DOWN | MYBMP_ALPHA | MYBMP_TYPE_RGBA;
        mybmp->flags |= MYBMP_RGBSIZE_4;

        pitch = (size_t)info->config.input.width * 3;
    }

    pitch = ROUND_TO_MULTIPLE(pitch, 8);

    // Have config.output point to an external buffer:
    info->config.output.u.RGBA.rgba = (uint8_t*)malloc(pitch);
    info->config.output.u.RGBA.stride = pitch;
    info->config.output.u.RGBA.size = pitch;
    info->config.output.is_external_memory = 1;

    if (info->config.output.u.RGBA.rgba == NULL) {
        _ERR_PRINTF ("__mg_init_webp: failed to allocate memory\n");
        goto error;
    }

    return info;

error:
    if (info)
        free(info);
    return NULL;
}

void __mg_cleanup_webp(void *init_info)
{
    struct webp_decode_info *info = init_info;

    if (info) {
        free(info->config.output.u.RGBA.rgba);
        free(info);
    }
}

int __mg_load_webp(MG_RWops *fp, void *init_info, MYBITMAP *my_bmp,
                CB_ONE_SCANLINE cb, void *context)
{
    return ERR_BMP_OK;
}

BOOL __mg_check_webp(MG_RWops* fp)
{
    unsigned char header[RIFF_HEADER_SIZE];

    MGUI_RWread(fp, header, RIFF_HEADER_SIZE, 1);
    if (!WebPGetInfo(header, RIFF_HEADER_SIZE, NULL, NULL)) {
    }

    return WebPGetInfo(header, RIFF_HEADER_SIZE, NULL, NULL);
}

#endif /* _MGIMAGE_WEBP */

