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
** map.c: the implementation of map based on red-black tree.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common.h"
#include "map.h"
#include "internals.h"

#ifdef _MGHAVE_VIRTUAL_WINDOW

#define WRLOCK_INIT(map)        pthread_rwlock_init (&(map)->rwlock, NULL)
#define WRLOCK_DSTR(map)        pthread_rwlock_destroy (&(map)->rwlock)

#define RDLOCK_MAP(map)         pthread_rwlock_rdlock (&(map)->rwlock)
#define WRLOCK_MAP(map)         pthread_rwlock_wrlock (&(map)->rwlock)
#define UNLOCK_MAP(map)         pthread_rwlock_unlock (&(map)->rwlock)

#else   /* defined _MGHAVE_VIRTUAL_WINDOW */

#define WRLOCK_INIT(map)
#define WRLOCK_DSTR(map)

#define RDLOCK_MAP(map)
#define WRLOCK_MAP(map)
#define UNLOCK_MAP(map)

#endif  /* not defined _MGHAVE_VIRTUAL_WINDOW */

struct _map_t {
    struct rb_root  root;

    copy_key_fn     copy_key;   // If NULL, use the iteral value.
    free_key_fn     free_key;   // Not NULL if copy_key is not NULL.
    copy_val_fn     copy_val;   // If NULL, use the iteral value.
    free_val_fn     free_val;   // Not NULL if copy_key is not NULL.
    comp_key_fn     comp_key;   // If NULL, use the iteral value of key as integer

    size_t          size;       // number of entries

#ifdef _MGHAVE_VIRTUAL_WINDOW
    pthread_rwlock_t rwlock;   // read-write lock
#endif
};

map_t* __mg_map_create (copy_key_fn copy_key, free_key_fn free_key,
        copy_val_fn copy_val, free_val_fn free_val,
        comp_key_fn comp_key)
{
    map_t* map;

    if (!(map = calloc (1, sizeof(map_t))))
        return NULL;

    map->copy_key = copy_key;
    map->free_key = free_key;
    map->copy_val = copy_val;
    map->free_val = free_val;
    map->comp_key = comp_key;

    WRLOCK_INIT (map);
    return map;
}

int __mg_map_get_size (map_t* map)
{
    return map->size;
}

int __mg_map_destroy (map_t* map)
{
    if (map == NULL)
        return -1;

    __mg_map_clear (map);

    WRLOCK_DSTR (map);

    free (map);
    return 0;
}

static map_entry_t* new_entry (map_t* map, const void* key,
        const void* val, free_val_fn free_val_alt)
{
    map_entry_t* entry;

    entry = mg_slice_new (map_entry_t);
    if (entry) {
        if (map->copy_key) {
            entry->key = map->copy_key (key);
        }
        else
            entry->key = (void*)key;

        if (map->copy_val) {
            entry->val = map->copy_val (val);
        }
        else
            entry->val = (void*)val;

        entry->free_val_alt = free_val_alt;
    }

    return entry;
}

static void clear_node (map_t* map, struct rb_node* node)
{
    if (node) {
        map_entry_t *entry = (map_entry_t*)node;

        if (map->free_key) {
            map->free_key (entry->key);
        }

        if (entry->free_val_alt) {
            entry->free_val_alt (entry->val);
        }
        else if (map->free_val) {
            map->free_val (entry->val);
        }

        clear_node (map, node->rb_left);
        clear_node (map, node->rb_right);

        mg_slice_delete (map_entry_t, entry);
    }
}

static void erase_entry (map_t* map, map_entry_t *entry)
{
    __mg_rb_erase (&entry->node, &map->root);
    clear_node (map, &entry->node);
    map->size--;
}

int __mg_map_clear (map_t* map)
{
    if (map == NULL)
        return -1;

    WRLOCK_MAP (map);

    clear_node (map, map->root.rb_node);
    map->root.rb_node = NULL;
    map->size = 0;

    UNLOCK_MAP (map);
    return 0;
}

static map_entry_t *find_entry (map_t* map, const void* key)
{
    map_entry_t* entry = NULL;

    if (map == NULL)
        return NULL;

    entry = (map_entry_t*)(map->root.rb_node);
    while (entry) {
        int ret;
       
        if (map->comp_key)
            ret = map->comp_key (key, entry->key);
        else
            ret = (int)((intptr_t)key - (intptr_t)entry->key);

        if (ret < 0) {
            entry = (map_entry_t*)(entry->node.rb_left);
        }
        else if (ret > 0) {
            entry = (map_entry_t*)(entry->node.rb_right);
        }
        else
            break;
    }

    return entry;
}

map_entry_t* __mg_map_find (map_t* map, const void* key)
{
    map_entry_t* entry = NULL;

    if (map == NULL)
        return NULL;

    RDLOCK_MAP (map);

    entry = find_entry (map, key);

    UNLOCK_MAP (map);
    return entry;
}

int __mg_map_erase (map_t* map, void* key)
{
    int retval = -1;
    map_entry_t* entry = NULL;

    WRLOCK_MAP (map);

    entry = find_entry (map, key);
    if (entry) {
        erase_entry (map, entry);
        retval = 0;
    }

    UNLOCK_MAP (map);
    return retval;
}

int __mg_map_replace (map_t* map, const void* key,
        const void* val, free_val_fn free_val_alt)
{
    int retval = -1;
    map_entry_t* entry = NULL;

    WRLOCK_MAP (map);

    entry = find_entry (map, key);
    if (entry == NULL) {
        goto ret;
    }

    retval = 0;
    /* XXX: is this reasonable? */
    if (val == entry->val) {
        goto ret;
    }

    if (entry->free_val_alt) {
        entry->free_val_alt (entry->val);
    }
    else if (map->free_val) {
        map->free_val (entry->val);
    }

    if (map->copy_val) {
        entry->val = map->copy_val (val);
    }
    else
        entry->val = (void*)val;

    entry->free_val_alt = free_val_alt;

ret:
    UNLOCK_MAP (map);
    return retval;
}

int __mg_map_insert_ex (map_t* map, const void* key,
        const void* val, free_val_fn free_val_alt)
{
    map_entry_t **pentry;
    map_entry_t *entry;
    map_entry_t *parent;

    if (map == NULL)
        return -1;

    WRLOCK_MAP (map);

    pentry = (map_entry_t**)&(map->root.rb_node);
    entry = NULL;
    parent = NULL;

    while (*pentry) {
        int ret;
       
        if (map->comp_key)
            ret = map->comp_key (key, (*pentry)->key);
        else
            ret = (int)((intptr_t)key - (intptr_t)(*pentry)->key);

        parent = *pentry;
        if (ret < 0)
            pentry = (map_entry_t**)&((*pentry)->node.rb_left);
        else if (ret > 0)
            pentry = (map_entry_t**)&((*pentry)->node.rb_right);
        else {
            entry = *pentry;
            break;
        }
    }

    if (!entry) {
        entry = new_entry (map, key, val, free_val_alt);
        rb_link_node (&entry->node,
                (struct rb_node*)parent, (struct rb_node**)pentry);
        __mg_rb_insert_color (&entry->node, &map->root);
        map->size++;
    }
    else {
        if (map->free_val) {
            map->free_val (entry->val);
        }

        if (map->copy_val) {
            entry->val = map->copy_val (val);
        }
        else
            entry->val = (void*)val;
    }

    UNLOCK_MAP (map);
    return 0;
}

int __mg_map_find_replace_or_insert (map_t* map, const void* key,
        const void* val, free_val_fn free_val_alt)
{
    map_entry_t **pentry;
    map_entry_t *entry;
    map_entry_t *parent;

    if (map == NULL)
        return -1;

    WRLOCK_MAP (map);

    pentry = (map_entry_t**)&(map->root.rb_node);
    entry = NULL;
    parent = NULL;
    while (*pentry) {
        int ret;
       
        if (map->comp_key) {
            ret = map->comp_key (key, (*pentry)->key);
        }
        else
            ret = (int)((intptr_t)key - (intptr_t)entry->key);

        parent = *pentry;
        if (ret < 0)
            pentry = (map_entry_t**)&((*pentry)->node.rb_left);
        else if (ret > 0)
            pentry = (map_entry_t**)&((*pentry)->node.rb_right);
        else {
            entry = *pentry;
            break;
        }
    }

    if (entry == NULL) {
        entry = new_entry (map, key, val, free_val_alt);


        rb_link_node (&entry->node,
                (struct rb_node*)parent, (struct rb_node**)pentry);
        __mg_rb_insert_color (&entry->node, &map->root);

        map->size++;
    }
    else {
        if (entry->free_val_alt) {
            entry->free_val_alt (entry->val);
        }
        else if (map->free_val) {
            map->free_val (entry->val);
        }

        if (map->copy_val) {
            entry->val = map->copy_val (val);
        }
        else
            entry->val = (void*)val;

        entry->free_val_alt = free_val_alt;
    }

    UNLOCK_MAP (map);
    return 0;
}

#if 0   /* deprecated code */
void* __mg_map_find_or_insert_ex (map_t* map, const void* key,
        const void* val, free_val_fn free_val_alt)
{
    map_entry_t **pentry;
    map_entry_t *entry;
    map_entry_t *parent;

    if (map == NULL)
        return NULL;

    RDLOCK_MAP (map);

    pentry = (map_entry_t**)&(map->root.rb_node);
    entry = NULL;
    parent = NULL;
    while (*pentry) {
        int ret;
       
        if (map->comp_key) {
            ret = map->comp_key (key, (*pentry)->key);
        }
        else
            ret = (int)((intptr_t)key - (intptr_t)entry->key);

        parent = *pentry;
        if (ret < 0)
            pentry = (map_entry_t**)&((*pentry)->node.rb_left);
        else if (ret > 0)
            pentry = (map_entry_t**)&((*pentry)->node.rb_right);
        else {
            entry = *pentry;
            break;
        }
    }

    UNLOCK_MAP (map);

    if (entry == NULL) {
        entry = new_entry (map, key, val, free_val_alt);

        WRLOCK_MAP (map);

        rb_link_node (&entry->node,
                (struct rb_node*)parent, (struct rb_node**)pentry);
        __mg_rb_insert_color (&entry->node, &map->root);

        map->size++;

        UNLOCK_MAP (map);
    }

    return entry->val;
}
#endif  /* deprecated code */

