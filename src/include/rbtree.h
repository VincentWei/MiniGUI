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
 *   Copyright (C) 2010~2020, Beijing FMSoft Technologies Co., Ltd.
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
 * rbtree.h: implementation of a red-black tree.
** Copied from FMSoft's MiniGUI-based solution for feature phone.
 */

#ifndef _MG_RBTREE_H
  #define _MG_RBTREE_H

#ifdef __cplusplus
extern "C" {
#endif

#define    RB_RED      0
#define    RB_BLACK    1

struct rb_node
{
    uintptr_t       rb_color;
    struct rb_node *rb_parent;
    struct rb_node *rb_right;
    struct rb_node *rb_left;
};

struct rb_root
{
    struct rb_node *rb_node;
};

#define rb_parent(r)    ((r)->rb_parent)
#define rb_color(r)     ((r)->rb_color)
#define rb_is_red(r)    (!rb_color(r))
#define rb_is_black(r)  rb_color(r)
#define rb_set_red(r)   do { (r)->rb_color = 0; } while (0)
#define rb_set_black(r) do { (r)->rb_color = 1; } while (0)

static inline
void rb_set_parent (struct rb_node *rb, struct rb_node *p)
{
    rb->rb_parent = p;
}

static inline
void rb_set_color(struct rb_node *rb, int color)
{
    rb->rb_color = color;
}

#define RB_ROOT                     (struct rb_root) { NULL, }
#define rb_entry(ptr, type, member) container_of(ptr, type, member)

#define RB_EMPTY_ROOT(root)         ((root)->rb_node == NULL)
#define RB_EMPTY_NODE(node)         (rb_parent(node) == node)
#define RB_CLEAR_NODE(node)         (rb_set_parent(node, node))

static inline
void rb_init_node (struct rb_node *rb)
{
    rb->rb_color = 0;
    rb->rb_parent = NULL;
    rb->rb_right = NULL;
    rb->rb_left = NULL;
    RB_CLEAR_NODE(rb);
}

static inline
void rb_link_node (struct rb_node * node,
        struct rb_node * parent, struct rb_node ** rb_link)
{
    node->rb_color = 0;
    node->rb_parent = parent;
    node->rb_left = node->rb_right = NULL;

    *rb_link = node;
}

void __mg_rb_insert_color (struct rb_node *, struct rb_root *);
void __mg_rb_erase (struct rb_node *, struct rb_root *);

typedef void (*rb_augment_f)(struct rb_node *node, void *data);

void __mg_rb_augment_insert (struct rb_node *node,
        rb_augment_f func, void *data);
struct rb_node *__mg_rb_augment_erase_begin (struct rb_node *node);
void __mg_rb_augment_erase_end (struct rb_node *node,
        rb_augment_f func, void *data);

/* Find logical next and previous nodes in a tree */
struct rb_node *__mg_rb_next (const struct rb_node *);
struct rb_node *__mg_rb_prev (const struct rb_node *);
struct rb_node *__mg_rb_first (const struct rb_root *);
struct rb_node *__mg_rb_last (const struct rb_root *);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
void __mg_rb_replace_node (struct rb_node *victim,
        struct rb_node *newnode, struct rb_root *root);

#ifdef __cplusplus
}
#endif

#endif /* _MG_RBTREE_H */
