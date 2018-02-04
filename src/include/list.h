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
 * list.h
 * list operations
 *
 * copied from Linux_Kernel_Dir/include/linux/list.h
 */

#ifndef _MG_LIST_H
  #define _MG_LIST_H

#include "mgconfig.h"
struct list_head {
    struct list_head *next, *prev;
};

typedef struct list_head list_t;

#define LIST_HEAD_INIT(name) { &(name), &(name) }

/* define and init a list head */
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

/* init a list head */
#define INIT_LIST_HEAD(ptr) do { \
    (ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

/*
 * Insert a new entry between two known consecutive entries. 
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(struct list_head * new,
    struct list_head * prev,
    struct list_head * next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void list_add(struct list_head *new, struct list_head *head)
{
    __list_add(new, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
    __list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_del(struct list_head * prev,
                  struct list_head * next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is in an undefined state.
 */
static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void list_del_init(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry); 
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int list_empty(struct list_head *head)
{
    return head->next == head;
}

/**
 * list_entry - get the struct for this entry
 * @ptr:    the &struct list_head pointer.
 * @type:    the type of the struct this is embedded in.
 * @member:    the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/**
 * list_for_each    -    iterate over a list
 * @pos:    the &struct list_head to use as a loop counter.
 * @head:    the head for your list.
 */
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)
            
/**
 * list_for_each_safe    -    iterate over a list safe against removal of list entry
 * @pos:    the &struct list_head to use as a loop counter.
 * @n:        another &struct list_head to use as temporary storage
 * @head:    the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

/**
 * list_for_each_ex    -    iterate over a list
 * @pos:    the &struct list_head to use as a loop counter.
 * @head:   the head for your list.
 * @begin:  the previous item of the begining item
 */
#define list_for_each_ex(pos, head, begin) \
    for (pos = (begin)->next; pos != (head); pos = (pos)->next)

/**
 * list_for_index    -    iterate over a list for index
 * @pos:    the &struct list_head to use as a loop counter.
 * @head:    the head for your list.
 */
#define list_for_index(pos, i, head, index) \
    for (pos = (head)->next, i=0; (pos != (head) && i < index); pos = pos->next,i++)


static inline int list_add_by_index(struct list_head *new, struct list_head *head, int index)
{
    list_t *pos = head;
    int i = -1;

    if (index >= 0) {
        for (pos = (head)->next, i=0; (pos != (head) && i < index); pos = pos->next,i++);
    }
    list_add_tail (new, pos);
    return i;
}


/* added for normal list operations */
#define GETBY_EX(func, head, entrytype, member, datatype, ret, equal_cond) \
        void* func (datatype data) \
        { \
            list_t *me; \
            entrytype *pdata; \
            int i = 0; \
            \
            list_for_each(me, head) { \
                pdata = list_entry(me, entrytype, member); \
                if (equal_cond) \
                    return ret; \
            } \
            return NULL; \
        }

#define GET_LIST_BY_DATA(func, head, entrytype, member, datamember, datatype) \
        GETBY_EX(func, head, entrytype, member, datatype, me, (pdata->datamember == data))

#define GET_ENTRY_BY_DATA(func, head, entrytype, member, datamember, datatype) \
        GETBY_EX(func, head, entrytype, member, datatype, pdata, (pdata->datamember == data))

/* for string */
#define GET_LIST_BY_STRING(func, head, entrytype, member, datamember, datatype) \
        GETBY_EX(func, head, entrytype, member, datatype, me, (strcmp(pdata->datamember,data)==0))

#define GET_ENTRY_BY_STRING(func, head, entrytype, member, datamember, datatype) \
        GETBY_EX(func, head, entrytype, member, datatype, pdata, (strcmp(pdata->datamember,data)==0))

/* for more compicated */
#if 0
#define GET_ENTRY_BY_STRUCT(func, head, entrytype, member, datamember, datatype, equal_cond) \
        GETBY_EX(func, head, entrytype, member, datatype, pdata, equal_cond)
#endif

/* for index */
#define GET_ENTRY_BY_INDEX(func, entrytype, member) \
        static inline entrytype* func (list_t *head, int index) \
        { \
            list_t *me; \
            int i = 0; \
            \
            list_for_each(me, head) { \
                if (i++ == index) \
                    return list_entry(me, entrytype, member); \
            } \
            return NULL; \
        }

#define GET_ENTRY_INDEX(func, entrytype, member) \
        static inline int func (list_t *head, entrytype* entry) \
        { \
            list_t *me; \
            int i = 0; \
            \
            list_for_each(me, head) { \
                if (entry == list_entry(me, entrytype, member)) \
                    return i; \
                i++; \
            } \
            return -1; \
        }


#endif /* _MG_LIST_H */
