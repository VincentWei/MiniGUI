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
** resmgr.h: This head file include some functions for resource manager. 
**
** Create date: 2008/08/05
**
** Current maintainer: Dong Junjie.
*/

#ifndef RES_MANAGER_H
#define RES_MANAGER_H

typedef struct _RES_TYPE_INFO{
    short type;
    unsigned short flag;
    int ops_ref;
	int def_source;
    RES_TYPE_OPS *ops;
}RES_TYPE_INFO;
#define RETIF_AUTO_DELETE 0x01

typedef struct _RES_ENTRY{
    //same as RESOURCE
    void* data;
    void *source;
    Uint8 flag;
    Uint8 type;
    unsigned short refcnt;
    RES_KEY key;
    struct _RES_ENTRY* next;
}RES_ENTRY;

#define REF_IN_USE      0x80  //the entry is or not in used
#define REF_INNER_SRC_COPYED  0x40 //inner res is copyed from usr
#define GetSourceType(entry)  (((entry)->flag)&0xF)
#define SetSourceType(entry, type) (((entry)->flag) = (((entry)->flag)&0xF0)|(type))
#define IsUsed(entry)  (((entry)->flag)&REF_IN_USE)
#define SetUsed(entry) (((entry)->flag)|=REF_IN_USE)
#define ClrUsed(entry) (((entry)->flag)&=(~REF_IN_USE))

#define IsInnerResCopyed(entry) (((entry)->flag)&REF_INNER_SRC_COPYED)
#define SetInnerResCopyed(entry) (((entry)->flag)|=REF_INNER_SRC_COPYED)
#define ClrInnerResCopyed(entry) (((entry)->flag)&=(~REF_INNER_SRC_COPYED))

#define DEF_HASH_SIZE  97

typedef struct _HASH_TABLE{
    int size;  //size of array of entries
    int count; //count of current RES_ENTRY in table
    RES_ENTRY** entries;
}HASH_TABLE;

BOOL InitializeResManager(int hash_table_size);

void TerminateResManager(void);

#define NEWEX(type, count)   (type*)calloc(count,sizeof(type))
#define NEW(type)  NEWEX(type,1)

#define RENEW(type, p, new_count) (type*)realloc((p),(new_count)*sizeof(type))

#define SNEW(type, p, cnt)  (p)?NEWEX(type,cnt):RENEW(type,p, cnt)

#define NEW_SIZE(type, ex_size)  (type*)malloc(sizeof(type)+ex_size)

#define MEM_DUP(p,size) memcpy(NEW(char, size), p, size)

#define STR_DUP(str)  strdup(str)

#define DELETE(p)  if(p)free(p)

#define MG_RES_PATH  "MG_RES_PATH"

#endif

