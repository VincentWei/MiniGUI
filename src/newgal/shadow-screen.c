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
/*
** shadow-screen.c:
**  This file implements the common helpers for GAL engines which
**  use shadow (double buffering) screen.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2020/03/11
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "common.h"
#include "minigui.h"
#include "constants.h"
#include "newgal.h"
#include "sysvideo.h"

#include "shadow-screen.h"

#define CURRENT_VERSION     0

#define CHECK_VERSION_RETVAL(this, retval)                      \
    if (this->hidden->magic != MAGIC_SHADOW_SCREEN_HEADER ||    \
            this->hidden->version != CURRENT_VERSION)           \
        return retval 

#define CHECK_VERSION_NORETVAL(this)                            \
    if (this->hidden->magic != MAGIC_SHADOW_SCREEN_HEADER ||    \
            this->hidden->version != CURRENT_VERSION)           \
        return

/* To use the helpers in this file, please:
  - put the fields at the header of your GAL_PrivateVideoData structure.
  - make sure to use the correct magic and version numbers.
  - create real_screen and shadow screen correctly.
  - Use shadowScreen_SetCursor, shadowScreen_MoveCursor,
    and shadowScreen_UpdateRects as your SetCursor, MoveCursor,
    and UpdateRects methods of your engine.
  - call shadowScreen_BlitToReal in your SyncUpdate method.
 */
struct GAL_PrivateVideoData {
    /* the magic number and version number */
    int magic, version;

    /* When double buffering supported, the real surface represents the ultimate
     * frame buffer, and the shadow screen represents the rendering surface.
     * When double buffering disabled, both are NULL.
     */
    GAL_Surface *real_screen, *shadow_screen;

    RECT dirty_rc;

#ifdef _MGSCHEMA_COMPOSITING
    /* Used to simulate the hardware cursor. */
    GAL_Surface *cursor;
    int csr_x, csr_y;
    int hot_x, hot_y;
#endif
};

#ifdef _MGSCHEMA_COMPOSITING
static inline int boxleft (_THIS)
{
    if (this->hidden->cursor == NULL)
        return -100;
    return this->hidden->csr_x - this->hidden->hot_x;
}

static inline int boxtop (_THIS)
{
    if (this->hidden->cursor == NULL)
        return -100;
    return this->hidden->csr_y - this->hidden->hot_y;
}

#include "cursor.h"

int shadowScreen_SetCursor (_THIS, GAL_Surface *surface, int hot_x, int hot_y)
{
    GAL_Rect rect;

    CHECK_VERSION_RETVAL (this, -1);

    if (this->hidden->cursor == surface &&
            this->hidden->hot_x == hot_x &&
            this->hidden->hot_y == hot_y) {
        return 0;
    }

    /* update screen to hide old cursor */
    if (this->hidden->cursor) {
        rect.x = boxleft (this);
        rect.y = boxtop (this);
        rect.w = CURSORWIDTH;
        rect.h = CURSORHEIGHT;

        this->hidden->cursor = NULL;
        this->UpdateRects (this, 1, &rect);
    }

    this->hidden->cursor = surface;
    this->hidden->hot_x = hot_x;
    this->hidden->hot_y = hot_y;

    /* update screen to show new cursor */
    if (this->hidden->cursor) {
        rect.x = boxleft (this);
        rect.y = boxtop (this);
        rect.w = CURSORWIDTH;
        rect.h = CURSORHEIGHT;
        this->UpdateRects (this, 1, &rect);
    }

    this->SyncUpdate (this);
    return 0;
}

int shadowScreen_MoveCursor (_THIS, int x, int y)
{
    CHECK_VERSION_RETVAL (this, -1);

    if (this->hidden->csr_x == x &&
             this->hidden->csr_y == y) {
        return 0;
    }

    if (this->hidden->cursor) {
        GAL_Surface* tmp;
        GAL_Rect rect;
        rect.x = boxleft (this);
        rect.y = boxtop (this);
        rect.w = CURSORWIDTH;
        rect.h = CURSORHEIGHT;

        /* update screen to hide cursor */
        tmp = this->hidden->cursor;
        this->hidden->cursor = NULL;
        this->UpdateRects (this, 1, &rect);

        /* update screen to show cursor */
        this->hidden->cursor = tmp;
        this->hidden->csr_x = x;
        this->hidden->csr_y = y;

        rect.x = boxleft (this);
        rect.y = boxtop (this);
        rect.w = CURSORWIDTH;
        rect.h = CURSORHEIGHT;
        this->UpdateRects (this, 1, &rect);
        this->SyncUpdate (this);
    }
    else {
        this->hidden->csr_x = x;
        this->hidden->csr_y = y;
    }

    return 0;
}

#endif /* _MGSCHEMA_COMPOSITING */

void shadowScreen_UpdateRects (_THIS, int numrects, GAL_Rect *rects)
{
    int i;
    RECT bound;

    CHECK_VERSION_NORETVAL (this);

    bound = this->hidden->dirty_rc;

    for (i = 0; i < numrects; i++) {
        RECT rc;

        SetRect (&rc, rects[i].x, rects[i].y,
                rects[i].x + rects[i].w, rects[i].y + rects[i].h);
        if (IsRectEmpty (&bound))
            bound = rc;
        else
            GetBoundRect (&bound, &bound, &rc);
    }

    this->hidden->dirty_rc = bound;
}

/* Blit dirty content from shadow surface to ultimate surface */
int shadowScreen_BlitToReal (_THIS)
{
    RECT bound;

    CHECK_VERSION_RETVAL (this, -1);

    bound = this->hidden->dirty_rc;

    if (this->hidden->real_screen) {
        GAL_Rect src_rect, dst_rect;
        src_rect.x = bound.left;
        src_rect.y = bound.top;
        src_rect.w = RECTW (bound);
        src_rect.h = RECTH (bound);
        dst_rect = src_rect;

        GAL_BlitSurface (this->hidden->shadow_screen, &src_rect,
                this->hidden->real_screen, &dst_rect);
#ifdef _MGSCHEMA_COMPOSITING
        if (this->hidden->cursor) {
            RECT csr_rc, eff_rc;
            csr_rc.left = boxleft (this);
            csr_rc.top = boxtop (this);
            csr_rc.right = csr_rc.left + CURSORWIDTH;
            csr_rc.bottom = csr_rc.top + CURSORHEIGHT;

            if (IntersectRect (&eff_rc, &csr_rc, &bound)) {
                src_rect.x = eff_rc.left - csr_rc.left;
                src_rect.y = eff_rc.top - csr_rc.top;
                src_rect.w = RECTW (eff_rc);
                src_rect.h = RECTH (eff_rc);

                dst_rect.x = eff_rc.left;
                dst_rect.y = eff_rc.top;
                dst_rect.w = src_rect.w;
                dst_rect.h = src_rect.h;
                GAL_BlitSurface (this->hidden->cursor, &src_rect,
                        this->hidden->real_screen, &dst_rect);
            }
        }
#endif  /* _MGSCHEMA_COMPOSITING */
    }

    return 0;
}

