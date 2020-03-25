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
** compsor-manager.c: the compositor manager module for MiniGUI.
**
** Create date: 2020-01-19
**
** Current maintainer: Wei Yongming.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define _DEBUG
#include "common.h"

#if defined(_MGRM_PROCESSES) && defined(_MGSCHEMA_COMPOSITING)

#include <dlfcn.h>

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "constants.h"
#include "zorder.h"
#include "dc.h"

#define MAX_NR_COMPOSITORS      8
#define LEN_COMPOSITOR_NAME     15

extern CompositorOps __mg_fallback_compositor;

static struct _compositors {
    char name [LEN_COMPOSITOR_NAME + 1];
    const CompositorOps* ops;
} compositors [MAX_NR_COMPOSITORS] = {
    { "fallback", &__mg_fallback_compositor },
};

static void* dl_handle;

static const CompositorOps* load_default_compositor (void)
{
    const char* filename = NULL;
    char buff [LEN_SO_NAME + 1];
    const CompositorOps* (*ex_compsor_get) (const char*,
            const CompositorOps*, int*);
    char* error;
    const CompositorOps* ops;
    int version = 0;

    filename = getenv ("MG_DEF_COMPOSITOR_SO");
    if (filename == NULL) {
        memset (buff, 0, sizeof (buff));
        if (GetMgEtcValue ("compositing_schema", "def_compositor_so",
                buff, LEN_SO_NAME) < 0)
            return NULL;

        filename = buff;
    }

    dl_handle = dlopen (filename, RTLD_LAZY);
    if (!dl_handle) {
        _WRN_PRINTF ("Failed to open specified shared library for the default compositor: %s (%s)\n",
                filename, dlerror());
        return NULL;
    }

    dlerror();    /* Clear any existing error */
    ex_compsor_get = dlsym (dl_handle, "__ex_compositor_get");
    error = dlerror ();
    if (error) {
        _WRN_PRINTF ("Failed to get symbol: %s\n", error);
        dlclose (dl_handle);
        return NULL;
    }

    ops = ex_compsor_get (COMPSOR_NAME_DEFAULT,
        &__mg_fallback_compositor, &version);

    if (ops == NULL || version != COMPSOR_OPS_VERSION)
        return NULL;

    return ops;
}

static void lock_znode_surface (PDC pdc, ZORDERNODE* node)
{
    if (node->lock_count == 0) {
        if (pdc->surface->shared_header) {
            LOCK_SURFACE_SEM (pdc->surface->shared_header->sem_num);
        }

        node->dirty_age = pdc->surface->dirty_info->dirty_age;
        node->nr_dirty_rcs = pdc->surface->dirty_info->nr_dirty_rcs;
        node->dirty_rcs = pdc->surface->dirty_info->dirty_rcs;

    }

    node->lock_count++;
}

static void unlock_znode_surface (PDC pdc, ZORDERNODE* node)
{
    if (node->lock_count > 0) {
        node->lock_count--;
        if (node->lock_count == 0) {
            if (pdc->surface->shared_header)
                UNLOCK_SURFACE_SEM (pdc->surface->shared_header->sem_num);

            node->dirty_age = 0;
            node->nr_dirty_rcs = 0;
            node->dirty_rcs = NULL;
        }
    }
}

#if 0   /* not optimized */
void __mg_composite_dirty_znodes (void)
{
    MG_Layer* layer;
    ZORDERINFO* zi;
    ZORDERNODE* nodes;
    CompositorCtxt* ctxt;
    const CompositorOps* ops = ServerSelectCompositor (NULL, &ctxt);
    int i, next;
    unsigned int changes_in_dc;
    PDC pdc;

    assert (ops);

    zi = (ZORDERINFO*)mgTopmostLayer->zorder_info;

    /* travel menu znodes on the topmost layer */
    if (zi->nr_popupmenus > 0) {
        nodes = GET_MENUNODE(zi);
        for (i = 0; i < zi->nr_popupmenus; i++) {
            pdc = dc_HDC2PDC (nodes[i].mem_dc);
            assert (pdc->surface->dirty_info);

            lock_znode_surface (pdc, nodes + i);
            changes_in_dc = pdc->surface->dirty_info->dirty_age;
            if (changes_in_dc != nodes[i].changes) {
                ops->on_dirty_ppp (ctxt, i);
                nodes[i].changes = changes_in_dc;
                pdc->surface->dirty_info->nr_dirty_rcs = 0;
            }
            unlock_znode_surface (pdc, nodes + i);
        }
    }

    /* travel win znodes on the topmost layer first */
    nodes = GET_ZORDERNODE(zi);
    next = 0;
    while ((next = __kernel_get_next_znode (zi, next)) > 0) {
        if (nodes [next].flags & ZOF_VISIBLE) {
            pdc = dc_HDC2PDC (nodes[next].mem_dc);
            assert (pdc->surface->dirty_info);

            lock_znode_surface (pdc, nodes + next);
            changes_in_dc = pdc->surface->dirty_info->dirty_age;
            if (changes_in_dc != nodes[next].changes) {
                ops->on_dirty_win (ctxt, mgTopmostLayer, next);
                nodes[next].changes = changes_in_dc;
                pdc->surface->dirty_info->nr_dirty_rcs = 0;
            }
            unlock_znode_surface (pdc, nodes + next);
        }
    }

    /* then travel win znodes on other layers */
    layer = mgLayers;
    while (layer) {
        if (layer != mgTopmostLayer) {
            zi = (ZORDERINFO*)layer->zorder_info;
            nodes = GET_ZORDERNODE(zi);
            next = 0;
            while ((next = __kernel_get_next_znode (zi, next)) > 0) {
                if (nodes [next].flags & ZOF_VISIBLE) {
                    pdc = dc_HDC2PDC (nodes[next].mem_dc);
                    assert (pdc->surface->dirty_info);

                    lock_znode_surface (pdc, nodes + next);
                    changes_in_dc = pdc->surface->dirty_info->dirty_age;
                    if (changes_in_dc != nodes[next].changes) {
                        ops->on_dirty_win (ctxt, layer, next);
                        nodes[next].changes = changes_in_dc;
                        pdc->surface->dirty_info->nr_dirty_rcs = 0;
                    }
                    unlock_znode_surface (pdc, nodes + next);
                }
            }
        }

        layer = layer->next;
    }

    /* check wallpaper pattern */
    pdc = dc_HDC2PDC (HDC_SCREEN);
    if (pdc->surface->w > 0 && pdc->surface->h > 0) {
        assert (pdc->surface->dirty_info);
        lock_znode_surface (pdc, nodes);
        changes_in_dc = pdc->surface->dirty_info->dirty_age;
        if (changes_in_dc != nodes[0].changes) {
            ops->on_dirty_wpp (ctxt);
            nodes[0].changes = changes_in_dc;
            pdc->surface->dirty_info->nr_dirty_rcs = 0;
        }
        unlock_znode_surface (pdc, nodes);
    }
}

#else   /* not optimized */

static void composite_layer (MG_Layer* layer, CompositorCtxt* ctxt,
        const CompositorOps* ops)
{
    ZORDERINFO* zi;
    ZORDERNODE* nodes;
    int i, next;
    unsigned int changes_in_dc;
    PDC pdc;
    zi = (ZORDERINFO*)layer->zorder_info;

    if (!ops->reset_dirty_region (ctxt, layer))
        return;

    /* travel menu znodes on the layer;
       this will only work for topmost layer */
    if (layer == mgTopmostLayer && zi->nr_popupmenus > 0) {
        nodes = GET_MENUNODE(zi);
        for (i = 0; i < zi->nr_popupmenus; i++) {
            pdc = dc_HDC2PDC (nodes[i].mem_dc);
            assert (pdc->surface->dirty_info);

            lock_znode_surface (pdc, nodes + i);
            changes_in_dc = pdc->surface->dirty_info->dirty_age;
            if (changes_in_dc != nodes[i].changes) {
                ops->merge_dirty_ppp (ctxt, layer, i);
            }
        }
    }

    /* travel win znodes on the layer */
    nodes = GET_ZORDERNODE(zi);
    next = 0;
    while ((next = __kernel_get_next_znode (zi, next)) > 0) {
        if (nodes [next].flags & ZOF_VISIBLE) {
            pdc = dc_HDC2PDC (nodes[next].mem_dc);
            assert (pdc->surface->dirty_info);

            lock_znode_surface (pdc, nodes + next);
            changes_in_dc = pdc->surface->dirty_info->dirty_age;
            if (changes_in_dc != nodes[next].changes) {
                ops->merge_dirty_win (ctxt, layer, next);
            }
        }
    }

    /* check wallpaper pattern; only call for topmost layer */
    if (layer == mgTopmostLayer) {
        pdc = dc_HDC2PDC (HDC_SCREEN);
        if (pdc->surface->w > 0 && pdc->surface->h > 0) {
            assert (pdc->surface->dirty_info);
            lock_znode_surface (pdc, nodes);
            changes_in_dc = pdc->surface->dirty_info->dirty_age;
            if (changes_in_dc != nodes[0].changes) {
                ops->merge_dirty_wpp (ctxt, layer);
            }
        }
    }

    ops->refresh_dirty_region (ctxt, layer);

    /* unlock the znode surfaces for popup menus */
    if (layer == mgTopmostLayer && zi->nr_popupmenus > 0) {
        nodes = GET_MENUNODE(zi);
        for (i = 0; i < zi->nr_popupmenus; i++) {
            pdc = dc_HDC2PDC (nodes[i].mem_dc);

            nodes[i].changes = pdc->surface->dirty_info->dirty_age;
            pdc->surface->dirty_info->nr_dirty_rcs = 0;
            unlock_znode_surface (pdc, nodes + i);
        }
    }

    /* unlock the znode surfaces for windows */
    nodes = GET_ZORDERNODE(zi);
    next = 0;
    while ((next = __kernel_get_next_znode (zi, next)) > 0) {
        if (nodes [next].flags & ZOF_VISIBLE) {
            pdc = dc_HDC2PDC (nodes[next].mem_dc);
            nodes[next].changes = pdc->surface->dirty_info->dirty_age;
            pdc->surface->dirty_info->nr_dirty_rcs = 0;
            unlock_znode_surface (pdc, nodes + next);
        }
    }

    /* unlock the znode surfaces for wallpaper pattern;
       only for topmost layer */
    if (layer == mgTopmostLayer) {
        pdc = dc_HDC2PDC (HDC_SCREEN);
        if (pdc->surface->w > 0 && pdc->surface->h > 0) {
            nodes[0].changes = pdc->surface->dirty_info->dirty_age;
            pdc->surface->dirty_info->nr_dirty_rcs = 0;
            unlock_znode_surface (pdc, nodes);
        }
    }
}

void __mg_composite_dirty_znodes (void)
{
    MG_Layer* layer;
    CompositorCtxt* ctxt;
    const CompositorOps* ops = ServerSelectCompositor (NULL, &ctxt);

    assert (ops);

    /* first handle the topmost layer */
    composite_layer (mgTopmostLayer, ctxt, ops);

    /* then handle other layers */
    layer = mgLayers;
    while (layer) {
        if (layer != mgTopmostLayer) {
            composite_layer (layer, ctxt, ops);
        }

        layer = layer->next;
    }
}

#endif  /* optimized */

static void purge_znodes_private_data_in_layer (const CompositorOps* ops,
        CompositorCtxt* ctxt, MG_Layer* layer)
{
    ZORDERINFO* zi;
    ZORDERNODE* nodes;
    int i, next;

    zi = (ZORDERINFO*)layer->zorder_info;

    // travel popup menu znodes
    if (zi->nr_popupmenus > 0) {
        nodes = GET_MENUNODE(zi);
        for (i = 0; i < zi->nr_popupmenus; i++) {
            if (nodes[i].priv_data && ops->purge_ppp_data) {
                ops->purge_ppp_data (ctxt, i, nodes[i].priv_data);
                nodes[i].priv_data = NULL;
            }
        }
    }

    // travel window znodes
    nodes = GET_ZORDERNODE(zi);
    next = 0;
    while ((next = __kernel_get_next_znode (zi, next)) > 0) {
        if (nodes[next].priv_data && ops->purge_win_data) {
            ops->purge_win_data (ctxt, layer, next, nodes[next].priv_data);
            nodes[next].priv_data = NULL;
        }
    }
}

static inline void purge_all_znodes_private_data (const CompositorOps* ops,
        CompositorCtxt* ctxt)
{
    MG_Layer* layer;

    layer = mgLayers;
    while (layer) {
        purge_znodes_private_data_in_layer (ops, ctxt, layer);
        layer = layer->next;
    }
}

BOOL mg_InitCompositor (void)
{
    const char* name = COMPSOR_NAME_FALLBACK;
    const CompositorOps* ops;
    CompositorCtxt* ctxt = NULL;

    ops = load_default_compositor ();
    if (ops && ServerRegisterCompositor (COMPSOR_NAME_DEFAULT, ops)) {
        name = COMPSOR_NAME_DEFAULT;
    }

    ServerSelectCompositor (name, &ctxt);
    return (ctxt != NULL);
}

void mg_TerminateCompositor (void)
{
    CompositorCtxt* ctxt = NULL;

    // Select fallback compositor and terminate the fallback compositor.
    ServerSelectCompositor (COMPSOR_NAME_FALLBACK, &ctxt);
    if (ctxt) {
        purge_all_znodes_private_data (&__mg_fallback_compositor, ctxt);
        __mg_fallback_compositor.terminate (ctxt);
    }

    if (dl_handle)
        dlclose (dl_handle);
}

const CompositorOps* GUIAPI ServerGetCompositorOps (const char* name)
{
    int i;

    if (name == NULL || name[0] == 0)
        return NULL;

    for (i = 0; i < MAX_NR_COMPOSITORS; i++) {
        if (strcmp (compositors [i].name, name) == 0) {
            return compositors [i].ops;
        }
    }

    return NULL;
}

BOOL GUIAPI ServerRegisterCompositor (
            const char* name, const CompositorOps* ops)
{
    int i;

    if (name == NULL || name[0] == 0 || ops == NULL)
        return FALSE;

    for (i = 0; i < MAX_NR_COMPOSITORS; i++) {
        if (compositors [i].name[0] == 0) {
            strncpy (compositors [i].name, name, LEN_COMPOSITOR_NAME);
            compositors [i].ops = ops;
            return TRUE;
        }
    }

    return FALSE;
}

BOOL GUIAPI ServerUnregisterCompositor (const char* name)
{
    int i;
    const CompositorOps* curr_ops;

    if (name == NULL || name[0] == 0 ||
            strcmp (name, COMPSOR_NAME_FALLBACK) == 0)
        return FALSE;

    curr_ops = ServerSelectCompositor (NULL, NULL);

    for (i = 0; i < MAX_NR_COMPOSITORS; i++) {
        if (strcmp (compositors [i].name, name) == 0) {
            if (curr_ops == compositors [i].ops)
                return FALSE;

            compositors [i].name[0] = 0;
            compositors [i].ops = NULL;
            return TRUE;
        }
    }

    return FALSE;
}

const CompositorOps* GUIAPI ServerSelectCompositor (const char* name,
        CompositorCtxt** the_ctxt)
{
    static const CompositorOps* curr_ops;
    static CompositorCtxt* curr_ctxt;
    const CompositorOps* ops;

    if (name == NULL || name [0] == 0) {
        if (the_ctxt) *the_ctxt = curr_ctxt;
        return curr_ops;
    }

    ops = ServerGetCompositorOps (name);
    if (ops) {
        CompositorCtxt* ctxt;

        if (ops == curr_ops) {
            if (the_ctxt) *the_ctxt = curr_ctxt;
            return ops;
        }

        ctxt = ops->initialize (name);
        if (ctxt) {
            if (curr_ops) {
                purge_all_znodes_private_data (curr_ops, curr_ctxt);
                curr_ops->terminate (curr_ctxt);
            }
            curr_ops = ops;
            curr_ctxt = ctxt;
            curr_ops->refresh (ctxt);
        }

        if (the_ctxt) *the_ctxt = curr_ctxt;
        return curr_ops;
    }

    return NULL;
}

#endif /* defined(_MGRM_PROCESSES) && defined(_MGSCHEMA_COMPOSITING) */

