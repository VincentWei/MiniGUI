/*
** $Id: hash.h 8944 2007-12-29 08:29:16Z xwyan $ 
**
** hash.h: header file of hash operations.
**
** Copyright (C) 2005 ~ 2007 Feynman Software.
*/

#ifndef _MG_HASH_H
#define _MG_HASH_H


#ifdef __cplusplus
extern "C"
{
#endif


#define HASHTABLE_MIN_SIZE      32          /* default hash table size */
#define HASHTABLE_MAX_SIZE      40960

#define HASH_THREAD_SAFE        0x00000001  /* thread safe flag */
#define HASH_RESIZABLE          0x00000002  /* resizable flag */


typedef GHANDLE (*HASHFUNC)  (GHANDLE key);
typedef BOOL    (*EQUALFUNC) (GHANDLE keya, GHANDLE keyb);


typedef struct _HASHNODE
{
    GHANDLE key;
    GHANDLE value;
    struct _HASHNODE *next;
} HASHNODE;

typedef struct _HASHTABLE
{
    DWORD      dwFlags;

    HASHNODE   **pNodes;
    size_t     size;
    int        nrNodes;

    HASHFUNC   pfnHash;
    EQUALFUNC  pfnEqual;

#ifdef _MGRM_THREADS
    pthread_mutex_t  lock;
#endif
} HASHTABLE;


HASHTABLE* HashTableCreate (void* memStart, size_t hash_size, BOOL bThreadSafe,
                            HASHFUNC hash_func, EQUALFUNC equal_func);
void       HashTableDestroy (HASHTABLE *hash_table);
GHANDLE    HashTableLookup (HASHTABLE *hash_table, GHANDLE key);
int        HashTableInsert (HASHTABLE *hash_table, GHANDLE key, GHANDLE value);
BOOL       HashTableRemove (HASHTABLE *hash_table, GHANDLE key);



#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif  /* _MG_HASH_H */

