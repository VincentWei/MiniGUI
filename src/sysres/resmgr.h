/*
** resmgr.h: This head file include some functions for resource manager. 
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
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

