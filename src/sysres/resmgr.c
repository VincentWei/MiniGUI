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
** resmgr.c: This file include some functions for resource manager.
**
** Create date: 2008/08/05
**
** Current maintainer: Dong Junjie.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#if !defined(__NOUNIX__) && !defined(WIN32)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cursor.h"
#include "icon.h"
#include "sysres.h"
#include "misc.h"
#include "list.h"
#include "map.h"

#include "resmgr.h"

//////////////////////////////////////////
//global variables
static char* res_paths[4]; /*The Last element of res_paths must bu NULL*/
#define pwd_res_path res_paths[0]
#define cfg_res_path res_paths[1]
#define usr_res_path res_paths[2]
static RES_TYPE_INFO* user_types = NULL;
static int user_type_count = 0;

static HASH_TABLE hash_table;

#ifdef _MGHAVE_VIRTUAL_WINDOW
static pthread_mutex_t lock;
#define INIT_LOCKER() pthread_mutex_init(&lock, NULL)
#define UNINIT_LOCKER() pthread_mutex_destroy(&lock)
#define RES_LOCK() pthread_mutex_lock(&lock)
#define RES_UNLOCK() pthread_mutex_unlock(&lock)
#else   /* defined _MGHAVE_VIRTUAL_WINDOW */
#define INIT_LOCKER()
#define UNINIT_LOCKER()
#define UNINT_LOCKER()
#define RES_LOCK()
#define RES_UNLOCK()
#endif  /* not defined _MGHAVE_VIRTUAL_WINDOW */

#ifdef _DEBUG
static void res_error(int type, const char* funcname,
        const char* strinfo, ... );
#else
static inline void res_error(int type, const char* funcname,
        const char* strinfo, ... )
{
    /* do nothing */
}
#endif

#ifndef WIN32

#define ERR_RETV(ret, type, ...)                            \
    do {                                                    \
        res_error((int)type, __FUNCTION__, __VA_ARGS__);    \
        return (ret);                                       \
    } while(0)

#define ERR_RET(type, ...)                                  \
    do {                                                    \
        res_error((int)type, __FUNCTION__, __VA_ARGS__);    \
        return;                                             \
    } while(0)

#endif  /* not define WIN32 */

//type ops
static void* img_get_res_data (RESOURCE* res, int src_type, DWORD usr_param);
static void img_unload (RESOURCE* res, int src_type);
static RES_TYPE_OPS img_ops = {
    img_get_res_data,
    img_unload
};

static RES_TYPE_OPS nothing_ops = { NULL, NULL };

static void* mybmp_get_res_data (RESOURCE* res, int src_type, DWORD usr_param);
static void mybmp_unload (RESOURCE* res, int src_type);
static RES_TYPE_OPS mybmp_ops = {
    mybmp_get_res_data,
    mybmp_unload
};

static void* icon_get_res_data (RESOURCE* res, int src_type, DWORD usr_param);
static void icon_unload (RESOURCE* res, int src_type);
static RES_TYPE_OPS icon_ops = {
    icon_get_res_data,
    icon_unload
};

#ifdef _MGHAVE_CURSOR
static void* cursor_get_res_data (RESOURCE* res,int src_type, DWORD usr_param);
static void cursor_unload (RESOURCE* res, int src_type);
static RES_TYPE_OPS cursor_ops = {
    cursor_get_res_data,
    cursor_unload
};
#endif

static void* etc_get_res_data (RESOURCE* res, int src_type, DWORD usr_param);
static void etc_unload (RESOURCE* res, int src_type);
static RES_TYPE_OPS etc_ops = {
    etc_get_res_data,
    etc_unload
};

#define text_ops nothing_ops
#define menu_ops nothing_ops
#define window_ops nothing_ops

static void* mem_get_res_data(RESOURCE* res, int src_type, DWORD usr_param);
static void mem_unload(RESOURCE* res, int src_type);
static RES_TYPE_OPS mem_ops={
    mem_get_res_data,
    mem_unload
};

static void* font_get_res_data(RESOURCE* res, int src_type, DWORD usr_param);
static void font_unload(RESOURCE* res, int src_type);
static RES_TYPE_OPS font_ops={
    font_get_res_data,
    font_unload
};

static RES_TYPE_INFO pre_def_types[RES_TYPE_USER] = {
    {(short)RES_TYPE_IMAGE, 0, 0, REF_SRC_FILE, &img_ops},
    {(short)RES_TYPE_MYBITMAP, 0, 0,REF_SRC_FILE, &mybmp_ops},
    {(short)RES_TYPE_ICON, 0, 0, REF_SRC_FILE, &icon_ops},
#ifdef _MGHAVE_CURSOR
    {(short)RES_TYPE_CURSOR, 0, 0, REF_SRC_FILE, &cursor_ops},
#endif
    {(short)RES_TYPE_ETC, 0, 0, REF_SRC_FILE, &etc_ops},
    {(short)RES_TYPE_TEXT, 0, 0, REF_SRC_FILE, &text_ops},
    {(short)RES_TYPE_MENU, 0, 0, REF_SRC_FILE, &menu_ops},
    {(short)RES_TYPE_WINDOW, 0, 0, REF_SRC_FILE, &window_ops},
    {(short)RES_TYPE_MEM_RES, 0, 0, REF_SRC_FILE, &mem_ops},
    {(short)RES_TYPE_FONT, 0, 0, REF_SRC_LOGIC, &font_ops},
};

///////////////////////////////////////
// the hash table operations
//
#define Key2Idx(t,key)   ((key)%((t)->size))
static void init_hash_table (HASH_TABLE* table, int size)
{
    if (table == NULL)
        return;
    if (size <= 0)
        size = DEF_HASH_SIZE;

    table->size = size;
    table->count = 0;
    table->entries = NEWEX(RES_ENTRY*, size);
}

static RES_ENTRY* get_entry(HASH_TABLE* table, RES_KEY key, BOOL create)
{
    RES_ENTRY* entry;
    int idx;

    if(table == NULL || table->size<=0 || table->entries == NULL)
        return NULL;

    idx = Key2Idx(table,key);
    if(idx <0 || idx >= table->size)
        return NULL;

    //get the entry begin list
    entry = table->entries[idx];
    // get entry
    while(entry && entry->key != key)
        entry = entry->next;

    if(entry)
        return entry;

    if(!create)
        return NULL;

    //create the entry
    entry = NEW(RES_ENTRY);
    entry->key = key;
    entry->next = table->entries[idx];
    table->entries[idx] = entry;

    return entry;
}

static void remove_entry(HASH_TABLE* table, RES_KEY key)
{
    RES_ENTRY* entry, *prev;
    int idx;

    if (table == NULL)
        return;

    idx = Key2Idx(table, key);
    if (idx < 0 || idx >= table->size)
        return;

    //get the entry begin list
    entry = table->entries[idx];
    if (entry->key == key) {
        table->entries[idx] = entry->next;
        DELETE(entry);
        return;
    }

    prev = entry;
    entry = entry->next;
    while (entry && entry->key != key){
        prev = entry;
        entry = entry->next;
    }

    if (entry) {
        prev->next = entry->next;
        DELETE(entry);
    }
}

static void release_hash_table(HASH_TABLE* table)
{
    if (table) {
        if (table->entries)
            DELETE(table->entries);
        table->entries = NULL;
        table->count = 0;
        table->size = 0;
    }
}

//////////////////////////////////////////
// res type operator
static RES_TYPE_INFO* get_res_type_info(int type)
{
    int i;
    if(type >= 0 && type < RES_TYPE_USER)
        return &pre_def_types[type];

    for(i=0; i<user_type_count; i++)
    {
        if(user_types[i].type == type)
            return &user_types[i];
    }

    return NULL;
}

static void append_user_type(int type, RES_TYPE_OPS* ops)
{
    user_types = SNEW(RES_TYPE_INFO, user_types, user_type_count+1);
    user_types[user_type_count].type = type;
    user_types[user_type_count].ops = ops;
    user_type_count ++;
}

static void delete_user_type(int type)
{
    int i;
    for(i=0; i<user_type_count; i++)
    {
        if(user_types[i].type == type)
        {
            if(user_types[i].ops_ref > 0)
            {
                //set auto delete flag
                user_types[i].flag |= RETIF_AUTO_DELETE;
                return;
            }
            break;
        }
    }

    if(i < user_type_count -1) //is not the last one
    {
        //move the last one into i
        memcpy(&user_types[i], &user_types[user_type_count-1],sizeof(RES_TYPE_INFO));
    }
    else if(i >= user_type_count) //not found
        return;

    //reduce the mem
    user_types = SNEW(RES_TYPE_INFO, user_types, user_type_count -1);
    user_type_count --;
}

static void delete_entry_data(RES_ENTRY* entry)
{
    RES_TYPE_INFO *info;
    int src_type = GetSourceType(entry);
    info = get_res_type_info(entry->type);
    if(info && info->ops && info->ops->unload)
        (*info->ops->unload)((RESOURCE*)entry, src_type);
    switch(src_type)
    {
    case REF_SRC_FILE:
        DELETE(entry->source);
        break;
    case REF_SRC_INNER:
        if(IsInnerResCopyed(entry)){
            DELETE(entry->source);
        }
        break;
    }
    entry->data = NULL;
    if(info)
    {
        info->ops_ref --;
        if(info->ops_ref == 0 && info->flag&RETIF_AUTO_DELETE)
            delete_user_type(info->type);
    }
}

/* The hash table size should be a prime. The following table gives all primes
 * less than 1000:
 2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 59 61 67 71 73 79 83 89 97 101 103 107
 109 113 127 131 137 139 149 151 157 163 167 173 179 181 191 193 197 199 211 223
 227 229 233 239 241 251 257 263 269 271 277 281 283 293 307 311 313 317 331 337
 347 349 353 359 367 373 379 383 389 397 401 409 419 421 431 433 439 443 449 457
 461 463 467 479 487 491 499 503 509 521 523 541 547 557 563 569 571 577 587 593
 599 601 607 613 617 619 631 641 643 647 653 659 661 673 677 683 691 701 709 719
 727 733 739 743 751 757 761 769 773 787 797 809 811 821 823 827 829 839 853 857
 859 863 877 881 883 887 907 911 919 929 937 941 947 953 967 971 977 983 991 997
*/

/* Since 5.0.0: functions for system bitmap map */
static void free_val_bitmap (void *val)
{
    UnloadBitmap (val);
    free (val);
}

map_t* __mg_sys_bmp_map;
BOOL mg_InitResManager(int hash_table_size)
{
    // initialize paths
    if (pwd_res_path == NULL)
        pwd_res_path = strdup ("./");
    __sysres_get_system_res_path();

    //initialize hash table
    init_hash_table (&hash_table, hash_table_size);

    //initialize predefined type
    //
    INIT_LOCKER();

    /* Since 5.0.0, initialize the map for system bitmaps */
    if ((__mg_sys_bmp_map = __mg_map_create (copy_key_string, free_key_string,
            NULL, free_val_bitmap, comp_key_string)) == NULL)
        return FALSE;

    return TRUE;
}

void TerminateResManager()
{
    int i;

    //delete all entries
    RES_LOCK();
    for(i = 0; i < hash_table.size; i++){
        RES_ENTRY * entry = hash_table.entries[i];
        while (entry) {
            RES_ENTRY* etmp = entry;
            entry = entry->next;
            if (etmp->data)
                delete_entry_data (etmp);
            DELETE (etmp);
        }
        hash_table.entries[i] = NULL;
    }

    //delete entrys
    release_hash_table (&hash_table);

    RES_UNLOCK ();
    //delete the respaths
    for (i = 0; res_paths[i]; i++){
        DELETE (res_paths[i]);
        res_paths[i] = NULL;
    }
    //delete types
    DELETE (user_types);
    user_types = NULL;
    user_type_count = 0;
    UNINIT_LOCKER ();

    /* Since 5.0.0, destroy the map for system bitmaps */
    __mg_map_destroy (__mg_sys_bmp_map);
}

const char* __sysres_get_system_res_path()
{
    char szpath [MAX_PATH+1];
    char* p = NULL;

    if (cfg_res_path)
        return cfg_res_path;

#if !defined(__NOUNIX__) || defined(WIN32)
    if ((p = getenv ("MG_RES_PATH"))) {
        int len = strlen (p);
        if (p[len-1] == '/')
            sprintf (szpath, "%s", p);
        else
            sprintf (szpath, "%s/", p);
        cfg_res_path = strdup (szpath);
    }
    else
#endif
    {
        // get from MiniGUI.cfg
        if (GetMgEtcValue ("resinfo", "respath",
                    szpath, sizeof (szpath)-1) == ETC_OK) {
            cfg_res_path = strdup (szpath);
        }
    }

    return cfg_res_path;
}

const char* GetResPath (void)
{
    return usr_res_path;
}

int SetResPath(const char* path)
{
    if(path == NULL) {
#ifdef _DEBUG
        ERR_RETV (RES_RET_INVALID_PARAM, RES_RET_INVALID_PARAM, "param path is null");
#else
        return RES_RET_INVALID_PARAM;
#endif
    }

    //test path is valid directory
#if  !defined(__NOUNIX__) && !defined(WIN32)
    struct stat buf;
    if (stat(path, &buf) != 0 || !S_ISDIR(buf.st_mode)) {
#   ifdef _DEBUG
        ERR_RETV (RES_RET_INVALID_PARAM, RES_RET_INVALID_PARAM, "param path (%s) is not a valid directory", path);
#   else
        return RES_RET_INVALID_PARAM;
#endif
    }
#endif

    if (usr_res_path != NULL)
        DELETE(usr_res_path);

    usr_res_path = STR_DUP(path);

    return RES_RET_OK;
}

int AddInnerRes(INNER_RES* inner_res, int count, BOOL bcopy)
{
    int i;
    if (inner_res == NULL || count <= 0) {
#ifdef _DEBUG
        ERR_RETV (RES_RET_INVALID_PARAM, RES_RET_INVALID_PARAM, "param inner_res is null or count is less zero");
#else
        return RES_RET_INVALID_PARAM;
#endif
    }

    RES_LOCK();

    for(i=0; i< count; i++)
    {
        RES_ENTRY *entry = get_entry(&hash_table, inner_res[i].key, TRUE);
        if(entry && !IsUsed(entry))
        {
            entry->type = RES_TYPE_INVALID;
            if(bcopy){
                INNER_RES *inner =  NEW(INNER_RES);
                memcpy(inner, &inner_res[i], sizeof(INNER_RES));
                entry->source = inner;
                SetInnerResCopyed(entry);
            }
            else
            {
                //if the resource is not copied,
                //we did not free it until the system exits.
                entry->source = &inner_res[i];
            }
            SetUsed(entry);
            SetSourceType(entry, REF_SRC_INNER);
        }
    }

    RES_UNLOCK();
    return RES_RET_OK;
}

int AddSharedRes(const char* shared_name)
{
    return RES_RET_NOT_IMPLEMENTED;
}

int RegisterResType(int type, RES_TYPE_OPS* ops)
{
    RES_TYPE_INFO *info;
    if ((type >= 0 && type < RES_TYPE_USER)) {
#ifdef _DEBUG
        ERR_RETV(RES_RET_INVALID_PARAM, RES_RET_INVALID_PARAM, "cannot register a predefined type(%d)", type);
#else
        return RES_RET_INVALID_PARAM;
#endif
    }

    if (type >= RES_TYPE_USER_MAX || ops == NULL) {
#ifdef _DEBUG
        ERR_RETV(RES_RET_INVALID_PARAM, RES_RET_INVALID_PARAM,
                "cannot register a null ops(%p) or type(%d) is invalid(type must be in %d-%d)",
                ops, type, RES_TYPE_USER, RES_TYPE_USER_MAX-1);
#else
        return RES_RET_INVALID_PARAM;
#endif
    }

    RES_LOCK();
    if((info = get_res_type_info(type)))
    {
        //clear the auto delete flag
        info->flag &= (~RETIF_AUTO_DELETE);
        return RES_RET_OK;
    }

    append_user_type(type, ops);

    RES_UNLOCK();

    return RES_RET_OK;
}

int UnregisterResType(int type)
{
    if(type >= 0 && type < RES_TYPE_USER) {
#ifdef _DEBUG
        ERR_RETV (RES_RET_INVALID_PARAM, RES_RET_INVALID_PARAM, "cannot unregister predefined type(%d)", type);
#else
        return RES_RET_INVALID_PARAM;
#endif
    }

    RES_LOCK();
    delete_user_type(type);
    RES_UNLOCK();

    return RES_RET_OK;
}

#ifdef HAVE_ACCESS
static char* get_res_file(const char* res_name, char* filename)
{
    int i;

    if (res_name == NULL || filename == NULL)
        return NULL;

    //test it is a full path or not
    for (i = 0; res_name[i] && res_name[i] == '/'; i++);

    if (i > 0) { // full path
#ifndef __NOUNIX__
        struct stat buf;
        if(stat (res_name, &buf) == 0
                && !S_ISDIR(buf.st_mode)
                && (buf.st_mode&S_IRUSR))
#endif
        {
            return (char*)res_name;
        }
    }

    for (i = 0; res_paths[i]; i++) {
#ifndef __NOUNIX__
        struct stat buf;

        sprintf (filename,"%s/%s", res_paths[i], res_name);
        if (stat (filename, &buf) == 0 &&
                !S_ISDIR (buf.st_mode) && (buf.st_mode & S_IRUSR))
#else
        sprintf (filename,"%s/%s", res_paths[i], res_name);
        if (access (filename, 04) != -1)
#endif
        {
            return filename;
        }
    }

    return NULL;
}

#else

static char* get_res_file(const char* res_name, char* filename)
{
    return NULL;
}

#endif  /* !HAVE_ACCESS */

static void* get_res_data (RES_ENTRY *entry, RES_TYPE_OPS *ops, DWORD usr_param)
{
    if (entry == NULL && ops == NULL)
        return NULL;

    if (entry->data == NULL) {
        if ((ops == NULL || ops->get_res_data == NULL)) {
            entry->data = entry->source;
            return entry->data;
        }

        if (ops && ops->get_res_data) {
            entry->data = (*ops->get_res_data)((RESOURCE*)entry,
                GetSourceType(entry), usr_param);
        }
    }

    return entry->data;
}


void* LoadResource(const char* res_name, int type, DWORD usr_param)
{
    RES_ENTRY * entry;
    RES_KEY key;
    RES_TYPE_INFO *ti = NULL;
    int     src_type = 0;
    void *data = NULL;
    char szfilename[MAX_PATH+1]={0};

    if (res_name == NULL) {
#ifdef _DEBUG
        ERR_RETV (NULL, type, "param res_name is NULL");
#else
        return NULL;
#endif
    }

    RES_LOCK();
    key = Str2Key(res_name);

    entry = get_entry(&hash_table, key, TRUE);
    if(entry == NULL){
        RES_UNLOCK();
#ifdef _DEBUG
        ERR_RETV(NULL, type, "%s does not exist", res_name);
#else
        return NULL;
#endif
    }

    ti = get_res_type_info(type);

    if(ti == NULL){
        RES_UNLOCK();
#ifdef _DEBUG
        ERR_RETV (NULL, type, "type(%d) is invalid", type);
#else
        return NULL;
#endif
    }

    if(!IsUsed(entry) || entry->type == RES_TYPE_INVALID)
        entry->type = type;
    else if(entry->type != type){
        RES_UNLOCK();
#ifdef _DEBUG
        ERR_RETV (NULL, type, "type is not match(resource\' type:%d != type:%d)", entry->type, type);
#else
        return NULL;
#endif
    }

    if(!IsUsed(entry)) //try load entry from file
    {
        //add type info's
        ti->ops_ref ++;
        src_type = GetSourceType(entry);
        if(src_type == REF_SRC_NOTYPE){
            src_type = ti->def_source;
            SetSourceType(entry, src_type);
        }
        switch(src_type)
        {
        case REF_SRC_FILE:
            entry->source = (void*)get_res_file(res_name, szfilename);
            break;
        case REF_SRC_LOGIC:
            entry->source = (void*)res_name;
        }
        SetUsed(entry);
    }

    entry->refcnt++;
    _DBG_PRINTF("%s: reference count for %p: %d\n",
        __FUNCTION__, entry, entry->refcnt);

    data = get_res_data (entry, ti->ops, usr_param);
    if (GetSourceType (entry) == REF_SRC_FILE)
        entry->source = NULL;
    RES_UNLOCK();
    return data;
}

void* GetResource(RES_KEY key)
{
    RES_ENTRY * entry;

    if(key == RES_KEY_INVALID) {
#ifdef _DEBUG
        ERR_RETV (NULL, -1, "key(%x) is invalid", key);
#else
        return NULL;
#endif
    }

    RES_LOCK();
    entry = get_entry(&hash_table, key, FALSE);
    RES_UNLOCK();

    if (entry == NULL || entry->data == NULL) {
#ifdef _DEBUG
        ERR_RETV (NULL, -1, "data does not exist");
#else
        return NULL;
#endif
    }

    return entry->data;
}


int AddResRef(RES_KEY key)
{
    RES_ENTRY * entry ;
    RES_LOCK();
    entry = get_entry(&hash_table, key,FALSE);
    RES_UNLOCK();
    if (entry == NULL) {
#ifdef _DEBUG
        ERR_RETV (-1, -1, "resouce does not exist(key=%x)", key);
#else
        return -1;
#endif
    }
    return ++entry->refcnt;
}

static void delete_entry(HASH_TABLE *table, RES_ENTRY* entry)
{
    if(entry == NULL)
        return;

    if(IsUsed(entry) && entry->data != NULL) {
        delete_entry_data(entry);
    }

    if(!(GetSourceType(entry) == REF_SRC_INNER
        && !IsInnerResCopyed(entry)))
        remove_entry(table, entry->key);
}

int ReleaseRes(RES_KEY key)
{
    int ref = 0;
    RES_ENTRY *entry = NULL;
    RES_LOCK();
    entry = get_entry(&hash_table, key, FALSE);
    if (entry == NULL){
        RES_UNLOCK();
        return -1;
    }

    ref = --entry->refcnt;

    _DBG_PRINTF("%s: reference count for %p: %d\n",
        __FUNCTION__, entry, entry->refcnt);

    if (ref <= 0)
        delete_entry(&hash_table, entry);
    RES_UNLOCK();
    return ref;
}

// We use FNV-1a algrithm for Str2Key:
// http://isthe.com/chongo/tech/comp/fnv/

#if SIZEOF_PTR == 8
// 2^40 + 2^8 + 0xb3 = 1099511628211
#   define FNV_PRIME        ((RES_KEY)0x100000001b3ULL)
#   define FNV_INIT         ((RES_KEY)0xcbf29ce484222325ULL)
#else
// 2^24 + 2^8 + 0x93 = 16777619
#   define FNV_PRIME        ((RES_KEY)0x01000193)
#   define FNV_INIT         ((RES_KEY)0x811c9dc5)
#endif

RES_KEY Str2Key (const char* str)
{
    const unsigned char* s = (const unsigned char*)str;
    RES_KEY hval = FNV_INIT;

    if (str == NULL)
        return RES_KEY_INVALID;

    /*
     * FNV-1a hash each octet in the buffer
     */
    while (*s) {

        /* xor the bottom with the current octet */
        hval ^= (RES_KEY)*s++;

        /* multiply by the FNV magic prime */
#ifdef __GNUC__
#   if SIZEOF_PTR == 8
        hval += (hval << 1) + (hval << 4) + (hval << 5) +
            (hval << 7) + (hval << 8) + (hval << 40);
#   else
        hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
#   endif
#else
        hval *= FNV_PRIME;
#endif
    }

    /* return our new hash value */
    return hval;
}

#ifdef _DEBUG
static void res_error (int type, const char* funcname, const char* strinfo, ... )
{
    char strbuff [256];
    static const char* errinfo[]={
        "invalid param",
        "key does not exist",
        "failed to load file",
        "failed to load from memory",
        "unknown type",
        "not implement",
        "res is in use"
    };
    va_list va;

    va_start (va, strinfo);
    vsnprintf (strbuff, 255, strinfo, va);

    _ERR_PRINTF ("SYSRES: (@%s)%s(ret %d): %s\n",
            funcname?funcname:"<>",
            type>=1&&type<=RES_RET_INUSED?errinfo[type-1]:"",
            type,
            strbuff);
}
#endif  /* defined _DEBUG */

/////////////////////////////////////////////////
//ops
static void* img_get_res_data(RESOURCE* res, int src_type, DWORD usr_param)
{
    if (res == NULL)
        return NULL;

    if (res->data == NULL) {
        //source is null
        if (res->source.src == NULL)
            return NULL;

        switch(src_type) {
        case REF_SRC_FILE: {
            BITMAP *pbmp;
            pbmp = NEW(BITMAP);
            if (LoadBitmapFromFile((HDC)usr_param, pbmp,
                    res->source.file) != 0) {
                DELETE(pbmp);
                return NULL;
            }
            res->data = pbmp;
            break;
        }

        case REF_SRC_INNER: {
            INNER_RES* inner = res->source.inner;
            if(inner->additional == NULL) {
                // raw bitmap
                res->data = (void*)inner->data;
            }
            else {
                BITMAP *pbmp = NEW(BITMAP);
                if (LoadBitmapFromMem((HDC)usr_param, pbmp,
                    inner->data, inner->data_len,
                    (const char*)inner->additional) != 0) {
                    DELETE(pbmp);
                    return NULL;
                }
                res->data = pbmp;
            }
            break;
        }

        default:
            return NULL;
        }
    }

    return res->data;
}

static void img_unload (RESOURCE* res, int src_type)
{
    if (res && res->data) {
        if ((src_type == REF_SRC_FILE) ||
                (src_type == REF_SRC_INNER
                    && (res->source.inner &&
                        res->source.inner->additional != NULL))) {
            UnloadBitmap(res->data);
            DELETE(res->data);
        }

        res->data = NULL;
    }
}

typedef struct _MYBMP_WITH_PAL {
    MYBITMAP mybmp;
    RGB* pal;
} MYBMP_WITH_PAL;

static inline MYBMP_WITH_PAL* new_mybmp_with_pal (void)
{
    MYBMP_WITH_PAL* mybmp_pal;

    mybmp_pal = NEW(MYBMP_WITH_PAL);
    mybmp_pal->pal = malloc (sizeof (RGB) * 256);
    return mybmp_pal;
}

static inline void delete_pal_in_mybmp_with_pal (MYBMP_WITH_PAL* mybmp_pal)
{
    DWORD type = mybmp_pal->mybmp.flags & MYBMP_TYPE_MASK;

    if (type == MYBMP_TYPE_RGB || type == MYBMP_TYPE_BGR ||
            type == MYBMP_TYPE_RGBA) {
        if (mybmp_pal->pal) {
            free (mybmp_pal->pal);
            mybmp_pal->pal = NULL;
        }
    }
}

static inline void delete_mybmp_with_pal (MYBMP_WITH_PAL* mybmp_pal)
{
    if (mybmp_pal->pal)
        free (mybmp_pal->pal);
    free (mybmp_pal);
}

static void* mybmp_get_res_data (RESOURCE* res, int src_type, DWORD usr_param)
{
    if (res == NULL)
        return NULL;

    if (res->data == NULL) {
        //source is null
        if (res->source.src == NULL)
            return NULL;

        switch (src_type) {
        case REF_SRC_FILE: {
            MYBMP_WITH_PAL *mybmp_pal;
            mybmp_pal = new_mybmp_with_pal ();
            if (LoadMyBitmapFromFile (&mybmp_pal->mybmp, mybmp_pal->pal,
                    res->source.file) != 0) {
                delete_mybmp_with_pal (mybmp_pal);
                return NULL;
            }
            delete_pal_in_mybmp_with_pal (mybmp_pal);
            res->data = mybmp_pal;
            if (usr_param) {
                *(RGB**)usr_param = mybmp_pal->pal;
            }
            break;
        }

        case REF_SRC_INNER: {
            INNER_RES* inner = res->source.inner;
            if(inner->additional == NULL) {
                // XXX: raw mybitmap?
                res->data = (void*)inner->data;
            }
            else {
                MYBMP_WITH_PAL *mybmp_pal;
                mybmp_pal = new_mybmp_with_pal ();
                if (LoadMyBitmapFromMem(&mybmp_pal->mybmp, mybmp_pal->pal,
                            inner->data, inner->data_len,
                            (const char*)inner->additional) != 0) {
                    delete_mybmp_with_pal (mybmp_pal);
                    return NULL;
                }
                delete_pal_in_mybmp_with_pal (mybmp_pal);
                res->data = mybmp_pal;
                if (usr_param) {
                    *(RGB**)usr_param = mybmp_pal->pal;
                }
            }
            break;
        }

        default:
            return NULL;
        }
    }
    else {
        MYBMP_WITH_PAL *mybmp_pal = res->data;
        if (usr_param) {
            *(RGB**)usr_param = mybmp_pal->pal;
        }
    }

    return res->data;
}

static void mybmp_unload (RESOURCE* res, int src_type)
{
    if (res && res->data) {
        if ((src_type == REF_SRC_FILE) ||
                (src_type == REF_SRC_INNER && (res->source.inner &&
                        res->source.inner->additional != NULL))) {
            MYBMP_WITH_PAL* mybmp_pal = res->data;
            UnloadMyBitmap (&mybmp_pal->mybmp);
            delete_mybmp_with_pal (mybmp_pal);
        }

        res->data = NULL;
    }
}

static void* icon_get_res_data (RESOURCE* res, int src_type, DWORD usr_param)
{
    if (res == NULL)
        return NULL;

    if (res->data == NULL) {
        HICON hIcon = 0;
        if (res->source.src == NULL)
            return NULL;

        switch(src_type) {
        case REF_SRC_FILE: {
            hIcon = LoadIconFromFile((HDC)usr_param, res->source.file, 0);
            break;
        }
        case REF_SRC_INNER: {
            hIcon = LoadIconFromMem((HDC)usr_param, res->source.inner->data, 0);
            break;
        }
        default:
            return NULL;
        }
        res->data = (void*)hIcon;
    }

    return res->data;
}

static void icon_unload(RESOURCE* res, int src_type)
{
    if (res && res->data)
        DestroyIcon((HICON)res->data);
}

#ifdef _MGHAVE_CURSOR
static void* cursor_get_res_data(RESOURCE* res, int src_type, DWORD usr_param)
{
    if (res == NULL)
        return NULL;

    if (res->data == NULL) {
        if(res->source.src == NULL)
            return NULL;

        switch(src_type) {
        case REF_SRC_FILE: {
            res->data =(void*) LoadCursorFromFile(res->source.file);
            break;
        }

        case REF_SRC_INNER: {
            res->data = (void*)LoadCursorFromMem(res->source.inner->data);
            break;
        }

        default:
            return NULL;
        }
    }

    return res->data;
}

static void cursor_unload(RESOURCE* res, int src_type)
{
    if (res && res->data) {
        DestroyCursor((HCURSOR)res->data);
    }
}

#endif

static void* etc_get_res_data(RESOURCE* res, int src_type, DWORD usr_param)
{
    if (res == NULL)
        return NULL;

    if (res->data == NULL) {
        if (res->source.src == NULL)
            return NULL;

        switch(src_type) {
        case REF_SRC_FILE: {
            res->data = (void*)LoadEtcFile(res->source.file);
            break;
        }
        case REF_SRC_INNER:
            res->data = (void*)res->source.inner->data;
            break;
        default:
            return NULL;
        }
    }

    return res->data;
}

static void etc_unload(RESOURCE* res, int src_type)
{
    if (res->data && src_type == REF_SRC_FILE)
        UnloadEtcFile((GHANDLE)res->data);
}

//////////////////////
//binery
static void* mem_get_res_data(RESOURCE* res, int src_type, DWORD usr_param)
{
    if (res == NULL)
        return NULL;

    if (res->data == NULL) {
        if (res->source.src == NULL)
            return NULL;

        switch (src_type) {
        case REF_SRC_INNER: {
            res->data = (MEM_RES*)&(res->source.inner->data);
            /* Since 5.0.0, return data size if user_param is not null */
            if (usr_param) {
                *(size_t*)usr_param =
                    ((MEM_RES*)&(res->source.inner->data))->data_len;
            }
            break;
        }
        default:
            return NULL;
        }
    }
    else {
        if (usr_param) {
            *(size_t*)usr_param =
                ((MEM_RES*)&(res->source.inner->data))->data_len;
        }
    }

    return res->data;
}

static void mem_unload(RESOURCE* res, int src_type)
{
    if (res)
        res->data = NULL;
}

static void* font_get_res_data(RESOURCE* res, int src_type, DWORD usr_param)
{
    if (res == NULL)
        return NULL;

    if (res->data == NULL) {
        if (res->source.src == NULL)
            return NULL;

        if (src_type == REF_SRC_LOGIC) {
            char *font_name = (char *)res->source.file;
            PLOGFONT p = CreateLogFontByName(font_name);
            if (p) {
                FONT_RES* data = (FONT_RES*)p;
                data->key = ((RES_ENTRY *)res)->key;
                res->data = data;
            }
            else {
                res->data = NULL;
            }
        }
    }

    return res->data;
}

static void font_unload(RESOURCE* res, int src_type)
{
    if (res && res->data) {
        FONT_RES *data = (FONT_RES *)res->data;
        _DBG_PRINTF("%s: LOGFONT %p will be destroyed\n",
            __FUNCTION__, &data->logfont);
        DestroyLogFont(&data->logfont);
        res->data = NULL;
    }
}

#if 0   /* deprecated code */
RES_KEY Str2Key (const char* str)
{
    int i,l;
    RES_KEY ret = 0;

    if (str == NULL)
        return RES_KEY_INVALID;

    l = (strlen(str)+1) / 2;

    for (i=0; i<l; i++) {
        Uint16 w = MAKEWORD(str[i<<1], str[(i<<1)+1]);
        ret ^= (w<<(i&0x0f));
    }

    return ret;
}
#endif  /* deprecated code */

