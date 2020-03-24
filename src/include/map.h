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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/*
** map.h: a map implementation based on red-black tree.
*/

#ifndef _MG_MAP_H
#define _MG_MAP_H

#include "common.h"
#include "rbtree.h"

#ifdef _MGHAVE_VIRTUAL_WINDOW
#include <pthread.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void *(*copy_key_fn) (const void *key);
typedef void  (*free_key_fn) (void *key);
typedef void *(*copy_val_fn) (const void *val);
typedef int   (*comp_key_fn) (const void *key1, const void *key2);
typedef void  (*free_val_fn) (void *val);

/* common functions for string key */
static inline void* copy_key_string (const void *key)
{
    return strdup (key);
}

static inline void free_key_string (void *key)
{
    free (key);
}

static inline int comp_key_string (const void *key1, const void *key2)
{
    return strcmp (key1, key2);
}

typedef struct _map_t map_t;
typedef struct _map_entry_t {
    struct rb_node  node;
    void*           key;
    void*           val;
    free_val_fn     free_val_alt;   // alternative free function per entry
} map_entry_t;

map_t* __mg_map_create (copy_key_fn copy_key, free_key_fn free_key,
        copy_val_fn copy_val, free_val_fn free_val,
        comp_key_fn comp_key);
int __mg_map_destroy (map_t* map);
int __mg_map_clear (map_t* map);
int __mg_map_get_size (map_t* map);

map_entry_t* __mg_map_find (map_t* map, const void* key);

int __mg_map_insert_ex (map_t* map, const void* key,
        const void* val, free_val_fn free_val_alt);
static inline int  __mg_map_insert (map_t* map, const void* key,
        const void* val)
{
    return __mg_map_insert_ex (map, key, val, NULL);
}

int __mg_map_find_replace_or_insert (map_t* map, const void* key,
        const void* val, free_val_fn free_val_alt);

int __mg_map_replace (map_t* map, const void* key,
        const void* val, free_val_fn free_val_alt);

int __mg_map_erase (map_t* map, void* key);
int __mg_map_get_size (map_t* map);

#if 0   /* deprecated code */
void* __mg_map_find_or_insert_ex (map_t* map, const void* key,
        const void* val, free_val_fn free_val_alt);

static inline void* __mg_map_find_or_insert (map_t* map, const void* key,
        const void* val)
{
    return __mg_map_find_or_insert_ex (map, key, val, NULL);
}
#endif  /* deprecated code */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _MG_MAP_H */

