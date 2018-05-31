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

#ifdef _MGRM_THREADS
#include <pthread.h>
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

#ifdef _MGRM_THREADS
static pthread_mutex_t lock;
#define INIT_LOCKER() pthread_mutex_init(&lock, NULL)
#define UNINIT_LOCKER() pthread_mutex_destroy(&lock)
#define RES_LOCK() pthread_mutex_lock(&lock)
#define RES_UNLOCK() pthread_mutex_unlock(&lock)
#else
#define INIT_LOCKER()
#define UNINIT_LOCKER()
#define UNINT_LOCKER()
#define RES_LOCK()
#define RES_UNLOCK()
#endif

#ifdef _DEBUG
static void res_error(int type, const char* funcname, const char* strinfo, ... );
#else
static inline void res_error(int type, const char* funcname, const char* strinfo, ... )
{
    /* do nothing */
}
#endif

#ifdef WIN32

#else

#define ERR_RETV(ret, type, ...)  do{ res_error((int)type, __FUNCTION__, __VA_ARGS__); return (ret); } while(0)

#define ERR_RET(type, ...)  do{ res_error((int)type, __FUNCTION__, __VA_ARGS__); return ; } while(0)

#endif

//type ops
static void* img_get_res_data(RESOURCE* res, int src_type, DWORD usr_param);
static void img_unload(RESOURCE* res, int src_type);
static RES_TYPE_OPS img_ops={
    img_get_res_data,
    img_unload
};

static RES_TYPE_OPS nothing_ops = { NULL, NULL };

#define mybmp_ops nothing_ops

static void* icon_get_res_data(RESOURCE* res, int src_type, DWORD usr_param);
static void icon_unload(RESOURCE* res, int src_type);
static RES_TYPE_OPS icon_ops={
    icon_get_res_data,
    icon_unload
};

#ifdef _MGHAVE_CURSOR
static void* cursor_get_res_data(RESOURCE* res,int src_type, DWORD usr_param);
static void cursor_unload(RESOURCE* res, int src_type);
static RES_TYPE_OPS cursor_ops={
    cursor_get_res_data,
    cursor_unload
};
#endif

static void* etc_get_res_data(RESOURCE* res, int src_type, DWORD usr_param);
static void etc_unload(RESOURCE* res, int src_type);
static RES_TYPE_OPS etc_ops={
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
    {(short)RES_TYPE_IMAGE, 0, 0, REF_SRC_FILE,&img_ops},
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
#define IsInvalidKey(key)  ((key)>0 && (key)<0xFFFFFFFF)
static void init_hash_table(HASH_TABLE* table, int size)
{
    if(table == NULL)
        return;
    if(size <= 0)
        size = DEF_HASH_SIZE;
    table->size = size;
    table->count = 0;
    table->entries = NEWEX(RES_ENTRY*, size);
}

static RES_ENTRY* get_entry(HASH_TABLE* table, RES_KEY key, BOOL create)
{
    RES_ENTRY* entry;
    int idx;

    if(table == NULL || table->size<=0 || table->entries == NULL  || !IsInvalidKey(key))
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

    if(table == NULL && IsInvalidKey(key))
        return ;

    idx = Key2Idx(table,key);
    if(idx <0 || idx >= table->size)
        return ;

    //get the entry begin list
    entry = table->entries[idx];

    if(entry->key == key){
        table->entries[idx] = entry->next;
        DELETE(entry);
        return;
    }

    prev = entry;
    entry = entry->next;
    while(entry && entry->key != key){
        prev = entry;
        entry = entry->next;
    }

    if(entry )
    {
        prev->next = entry->next;
        DELETE(entry);
    }

}

static void release_hash_table(HASH_TABLE* table)
{
    if(table){
        if(table->entries)
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



////////////////////////////////////////////
//inside functions
BOOL InitializeResManager(int hash_table_size)
{
    char szpath[MAX_PATH+1];
    char* p = NULL;
	pwd_res_path = strdup("./");

    //initialize paths
#if !defined(__NOUNIX__) || defined(WIN32)
    if ((p = getenv ("MG_RES_PATH"))) {
        int len = strlen(p);
        if (p[len-1] == '/') 
            sprintf(szpath, "%s", p);
        else 
            sprintf(szpath, "%s/", p);
        cfg_res_path = strdup(szpath);
    }
    else
#endif
    {
        //step 2: get from MiniGUI.cfg
        if(GetMgEtcValue("resinfo", "respath", szpath, sizeof(szpath)-1) == ETC_OK) {
            cfg_res_path = strdup(szpath);
        }
    }

    //initialize hash table
    init_hash_table(&hash_table, hash_table_size);

    //initialize predefined type
    //
    INIT_LOCKER();
    return TRUE;
}

void TerminateResManager()
{
    int i;

    //delete all entries
    RES_LOCK();
    for(i=0; i<hash_table.size; i++){
        RES_ENTRY * entry = hash_table.entries[i];
        while(entry)
        {
            RES_ENTRY* etmp = entry;
            entry = entry->next;
            if(etmp->data)
                delete_entry_data(etmp);
            DELETE(etmp);
        }
        hash_table.entries[i] = NULL;
    }

    //delete entrys
    release_hash_table(&hash_table);

    RES_UNLOCK();
    //delete the respaths
    for(i=0; res_paths[i]; i++){
        DELETE(res_paths[i]);
        res_paths[i] = NULL;
    }
    //delete types
    DELETE(user_types);
    user_types = NULL;
    user_type_count = 0;
    UNINIT_LOCKER();
}

const char* sysres_get_system_res_path()
{
    return cfg_res_path;
}

//////////////////////////////////////////
//outside functions
#if 0
static int get_respath_count()
{
    int cnt;
    if(res_paths == NULL)
        return 0;

    for(cnt=0; res_paths[cnt]; cnt++);

    return cnt;
}

int strstrcnt(const char* strs, int s)
{
    int i;
    if(strs == NULL)
        return 0;
    i = 1;
    while((strs = strchr(strs,s))){
        i ++;
        strs ++;
    }
    return i;
}

int AddResPath(const char* paths)
{
    int cnt;
    int p_ctn = 0;
    const char* str;
    if(path == NULL)
        return RES_RET_INVALID_PARAM;

    cnt = get_respath_count();

    if(cnt <= 0) cnt = 1;

    //the path is "[path1]:[path2]:..."
    //get the count of paths
    p_cnt = strstrcnt(paths,':');
    if(p_cnt <= 0)
        return RES_RES_INVALID_PARAM;

    res_pahts = SNEW(char*, res_paths, cnt+p_cnt);

    str = paths;
    cnt --;
    while((str = strchr(paths,':')))
    {
        res_paths[cnt++] = strndup(paths, str-paths);
        paths = str + 1;
    }

    res_paths[cnt++] = strdup(paths);
    res_paths[cnt] = NULL;

    return RES_RET_OK;
}

#endif

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
    if(stat(path, &buf) != 0 || !S_ISDIR(buf.st_mode)) {
#   ifdef _DEBUG
        ERR_RETV (RES_RET_INVALID_PARAM, RES_RET_INVALID_PARAM, "param path (%s) is not a valid directory", path);
#   else
        return RES_RET_INVALID_PARAM;
#endif
    }
#endif

    if(usr_res_path != NULL)
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
                //if the resource is not copyed,
                //we didnot free it until the system
                //is exist
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

static char* get_res_file(const char* res_name, char* filename)
{
    int i;
    if (res_name == NULL || filename == NULL)
        return NULL;

    //test it is a full path or not
    for(i=0; res_name[i] && res_name[i] == '/'; i++);
    if(i>0) //is full path
    {
#ifndef __NOUNIX__
        struct stat buf;
        if(stat(res_name, &buf) == 0
            && !S_ISDIR(buf.st_mode)
            && (buf.st_mode&S_IRUSR))
#endif
        {
            return (char*)res_name;
        }
    }

    for(i=0; res_paths[i]; i++)
    {
#ifndef __NOUNIX__
        struct stat buf;
        sprintf(filename,"%s/%s", res_paths[i], res_name);
        if(stat(filename, &buf) == 0 && !S_ISDIR(buf.st_mode) && (buf.st_mode&S_IRUSR))
#else
        sprintf(filename,"%s/%s", res_paths[i], res_name);
#ifndef __VXWORKS__
        //in vxworks, don't support _access method.
        if (_access(filename, 04) != -1)
#endif
#endif
        {
            return filename;
        }
    }

    return NULL;
}

static void* get_res_data(RES_ENTRY *entry, RES_TYPE_OPS *ops, DWORD usr_param)
{
    if(entry == NULL && ops == NULL)
        return NULL;

    if(entry->data == NULL){
        if( ( ops == NULL || ops->get_res_data == NULL)){
            entry->data = entry->source;
            return entry->data;
        }
        if(ops && ops->get_res_data)
        {
            entry->data = (*ops->get_res_data)((RESOURCE*)entry,
                GetSourceType(entry),
                usr_param);
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
        ERR_RETV(NULL, type, "%s is not exists", res_name);
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

    entry->refcnt ++ ;

    data = get_res_data(entry, ti->ops, usr_param);
    if(GetSourceType(entry) == REF_SRC_FILE)
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
        ERR_RETV (NULL, -1, "data is not exist");
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
        ERR_RETV (-1, -1, "resouce is not exist(key=%x)", key);
#else
        return -1;
#endif
    }
    return ++entry->refcnt ;
}

static void delete_entry(HASH_TABLE *table, RES_ENTRY* entry)
{
    if(entry == NULL)
        return;

    if(IsUsed(entry) && entry->data != NULL)
    {
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
    if(entry == NULL){
        RES_UNLOCK();
        return -1;
    }
	ref = --entry->refcnt;
    if(ref <= 0)
        delete_entry(&hash_table, entry);
    RES_UNLOCK();
    return ref;
}

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

#ifdef _DEBUG
static void res_error(int type, const char* funcname, const char* strinfo, ... )
{
    char strbuff [256];
    static const char* errinfo[]={
        "invalid param",
        "key is not exits",
        "load file failed",
        "load mem failed",
        "unknown type",
        "not implement",
        "res is in used"
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
#endif

/////////////////////////////////////////////////
//ops
static void* img_get_res_data(RESOURCE* res, int src_type, DWORD usr_param)
{
    if(res == NULL)
        return NULL;

    if(res->data == NULL)
    {
        //source is null
        if(res->source.src == NULL)
            return NULL;

        switch(src_type)
        {
        case REF_SRC_FILE:
            {
                BITMAP *pbmp;
                pbmp = NEW(BITMAP);
                if(LoadBitmapFromFile((HDC)usr_param, pbmp, res->source.file) != 0)
                {
                    DELETE(pbmp);
                    return NULL;
                }
                res->data = pbmp;
                break;
            }
        case REF_SRC_INNER:
            {
                INNER_RES* inner = res->source.inner;
                if(inner->additional == NULL) //raw bitmap
                {
                    res->data = inner->data;
                }
                else {
                    BITMAP *pbmp = NEW(BITMAP);
                    if(LoadBitmapFromMem((HDC)usr_param, pbmp, inner->data, inner->data_len, (const char*)inner->additional) != 0)
                    {
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

static void img_unload(RESOURCE* res, int src_type)
{
    if(res && res->data){
        if((src_type == REF_SRC_FILE)
            || (src_type == REF_SRC_INNER
                && (res->source.inner && res->source.inner->additional != NULL))){
            UnloadBitmap(res->data);
            DELETE(res->data);
        }
        res->data = NULL;
    }
}

////////////////////////////////////////////////
//icons
#if 0
typedef struct _icon_res_t{
    DWORD dwMask;
    HICON *icons;
}icon_res_t;

static inline int icon_res_which_to_idx(DWORD dwMask, int which)
{
    int idx = 0;
    while(which>=0)
    {
        if(dwMask&1) idx ++;
        dwMask >>= 1;
    }
    return idx;
}
static inline int icon_res_get_cnt(icon_res_t* res)
{
    DWORD dw = res->dwMask;
    int cnt = 0;
    while(dw!=0)
    {
        cnt ++;
        dw >>= 1;
    }
    return cnt;
}

static BOOL icon_get_res_data(RESOURCE* res, int src_type, void* usr_param)
{
    int which;
    icon_res_t* res_icon;
    if(res == NULL)
        return FALSE;

    which = (int)usr_param;

    if(which < 0 || which>=32)
        which = 0;

    if(res->data == NULL)
    {
        res_icon = NEW(icon_res_t);
        res->data = res_icon;
    }

    if(res_icon->dwMask & (1<<which)){
        return res_icon->icons[icon_res_which_to_idx(res_icon->dwMask, which)];
    }
    else
    {
        //load icon
        HICON hi;
        if(res->source.src == NULL)
            return NULL;

        switch(src_type)
        {
        case REF_SRC_FILE:
        {
            char szpath[512];
            RES_FILE_SRC* file = res->source.file;
            sprintf(szpath, "%s/%s", file->path, file->filename);
            hi = LoadIconFromFile(HDC_SCREEN, szpath, which);
            break;
        }
        case REF_SRC_INNER:
        {
            INNER_RES* inner = res->source.inner;
            hi = LoadIconFromMem(HDC_SCREEN, inner->data, inner->data_len, which);
            break;
        }
        default:
            return NULL;
        }

        if(hi != 0)
        {
            //insrt at which
            int idx;
            int cnt;
            int i;
            res_icon->dwMask |= (1<<which);
            idx = icon_res_which_to_idx(res_icon->dwMask, which);
            cnt = icon_res_get_cnt(res_icon);
            res_icon->icons = SNEW(HICON, res_icon->icons,cnt+1);
            //move elements
            for(i=cnt; i>idx; i--)
            {
                res_icon->icons[i] = res_icon->icons[i-1];
            }
            res_icon->icons[idx] = hi;
            return hi;
        }
    }

    return NULL;
}

static void icon_unload(RESOURCE* res, int src_type)
{
    if(res && res->data)
    {
        icon_res_t* ir = (icon_res_t*)res->data;
        if(ir->icons)
        {
            int cnt = icon_res_get_cnt(ir);
            while(cnt>0)
                DestroyIcon(ir->icons[--cnt]);
            DELETE(ir->icons);
        }
        DELETE(ir);
    }
}
#endif

static void* icon_get_res_data(RESOURCE* res, int src_type, DWORD usr_param)
{
    if(res == NULL)
        return NULL;

    if(res->data==NULL)
    {
        HICON hIcon = 0;
        if(res->source.src == NULL)
            return NULL;

        switch(src_type)
        {
        case REF_SRC_FILE:
        {
            hIcon = LoadIconFromFile((HDC)usr_param, res->source.file, 0);
            break;
        }
        case REF_SRC_INNER:
        {
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
    if(res && res->data)
        DestroyIcon((HICON)res->data);
}


#ifdef _MGHAVE_CURSOR
static void* cursor_get_res_data(RESOURCE* res, int src_type, DWORD usr_param)
{
    if(res == NULL)
        return NULL;

    if(res->data == NULL)
    {
        if(res->source.src == NULL)
            return NULL;

        switch(src_type)
        {
        case REF_SRC_FILE:
        {
            res->data =(void*) LoadCursorFromFile(res->source.file);
            break;
        }
        case REF_SRC_INNER:
        {
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
    if(res && res->data){
        DestroyCursor((HCURSOR)res->data);
    }
}

#endif

static void* etc_get_res_data(RESOURCE* res, int src_type, DWORD usr_param)
{
    if(res == NULL)
        return NULL;

    if(res->data == NULL)
    {
        if(res->source.src == NULL)
            return NULL;

        switch(src_type)
        {
        case REF_SRC_FILE:
        {
            res->data = (void*)LoadEtcFile(res->source.file);
            break;
        }
        case REF_SRC_INNER:
            res->data = res->source.inner->data;
            break;
        default:
            return NULL;
        }
    }

    return res->data;
}

static void etc_unload(RESOURCE* res, int src_type)
{
    if(res->data && src_type == REF_SRC_FILE)
        UnloadEtcFile((GHANDLE)res->data);
}

//////////////////////
//binery
static void* mem_get_res_data(RESOURCE* res, int src_type, DWORD usr_param)
{
    if(res == NULL)
        return NULL;

    if(res->data == NULL)
    {
        if(res->source.src == NULL)
            return NULL;

        switch(src_type)
        {
        case REF_SRC_INNER:
            {
                res->data = (MEM_RES*)&(res->source.inner->data);
                break;
            }
        default:
            return NULL;
        }
    }
    return res->data;
}

static void mem_unload(RESOURCE* res, int src_type)
{
    if(res)
        res->data = NULL;
}

static void* font_get_res_data(RESOURCE* res, int src_type, DWORD usr_param)
{
    if(res == NULL)
        return NULL;

    if(res->data == NULL)
    {
        if(res->source.src == NULL)
            return NULL;

        if (src_type == REF_SRC_LOGIC)
        {
            char *font_name = (char *)res->source.file;
            PLOGFONT p = CreateLogFontByName(font_name);
            FONT_RES *data = (FONT_RES *)malloc(sizeof(FONT_RES));
            memcpy(&data->logfont, p, sizeof(LOGFONT));
            data->key = ((RES_ENTRY *)res)->key;
            free(p);
            res->data = data;
        }
    }

    return res->data;
}

static void font_unload(RESOURCE* res, int src_type)
{
    FONT_RES *data = (FONT_RES *)res->data;
    //FIXME, .... how to destroy the log font ???
    DestroyLogFont (&data->logfont);
    //free(data);
}

