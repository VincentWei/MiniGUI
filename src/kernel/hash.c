/*
** $Id: hash.c 10660 2008-08-14 09:30:39Z weiym $
**
** hash.c: the hash module for MiniGUI, from the idea of GLIB.
** 
** Copyright (C) 2005 ~ 2008 Feynman Software.
**
** All rights reserved by Feynman Software.
**
** Some ideas come from GLIB.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "hash.h"


#define MG_HASH_LOCK() \
    if (hash_table->dwFlags & HASH_THREAD_SAFE) \
        MG_MUTEX_LOCK (&hash_table->lock)

#define MG_HASH_UNLOCK() \
    if (hash_table->dwFlags & HASH_THREAD_SAFE) \
        MG_MUTEX_UNLOCK (&hash_table->lock)


static void hash_node_destroy (HASHNODE *node);
static HASHNODE* hash_node_new (GHANDLE key, GHANDLE value);
static void hash_node_destroy (HASHNODE *node);


GHANDLE FuncDirectHash (GHANDLE key)
{
    return key;
}


HASHTABLE* HashTableCreate (void* memStart, size_t hash_size, BOOL bThreadSafe,
                            HASHFUNC hash_func, EQUALFUNC equal_func)
{
    HASHTABLE *hash_table;

    if( !(hash_table = calloc (1, sizeof(HASHTABLE))) ) return NULL;

    if (hash_size <= 0) hash_size = HASHTABLE_MIN_SIZE;

    if (memStart == NULL) { /* allocate space for nodes */
        hash_table->pNodes = malloc (sizeof(HASHNODE*) * hash_size);
        hash_table->dwFlags |= HASH_RESIZABLE;
        hash_table->size = hash_size;
    }
    else { /* use static space for nodes */
        hash_table->pNodes = memStart;
        hash_table->size = hash_size;
    }

    memset (hash_table->pNodes, 0, sizeof(HASHNODE*) * hash_size);

    hash_table->pfnHash = hash_func ? hash_func : FuncDirectHash;
    hash_table->pfnEqual = equal_func;

    if (bThreadSafe)
        hash_table->dwFlags |= HASH_THREAD_SAFE;

    if (hash_table->dwFlags & HASH_THREAD_SAFE)
        MG_MUTEX_INIT (&hash_table->lock);

    return hash_table;
}

void HashTableDestroy (HASHTABLE *hash_table)
{
    int i;

    MG_CHECK (hash_table != NULL);

    MG_HASH_LOCK();

    for (i = 0; i < hash_table->size; i++) {
        hash_node_destroy (hash_table->pNodes[i]);
    }

    if (hash_table->dwFlags & HASH_RESIZABLE) {
        free (hash_table->pNodes);
    }

    MG_HASH_UNLOCK();
//FIXME, not so safe

    if (hash_table->dwFlags & HASH_THREAD_SAFE)
        MG_MUTEX_DESTROY (&hash_table->lock);

    free (hash_table);
}


static inline HASHNODE**
hash_table_lookup_node (HASHTABLE *hash_table, GHANDLE key)
{
    HASHNODE **node;

    node = &hash_table->pNodes
           [(* hash_table->pfnHash) (key) % hash_table->size];

    /* Hash table lookup needs to be fast.
     *  We therefore remove the extra conditional of testing
     *  whether to call the key_equal_func or not from
     *  the inner loop.
     */
    if (hash_table->pfnEqual)
        while (*node && !(*hash_table->pfnEqual) ((*node)->key, key))
            node = &(*node)->next;
    else
        while (*node && (*node)->key != key)
            node = &(*node)->next;

  return node;
}


GHANDLE HashTableLookup (HASHTABLE *hash_table, GHANDLE key)
{
    HASHNODE *node;

    MG_CHECK_RET (hash_table != NULL, 0);

    MG_HASH_LOCK();

    node = *hash_table_lookup_node (hash_table, key);

    MG_HASH_UNLOCK();

    return node ? node->value : 0;
}

int HashTableInsert (HASHTABLE *hash_table, GHANDLE key, GHANDLE value)
{
    HASHNODE **node;

    MG_CHECK_RET (hash_table != NULL, -1);

    MG_HASH_LOCK();

    node = hash_table_lookup_node (hash_table, key);

    if (*node) {
        (*node)->value = value;
    }
    else {
        *node = hash_node_new (key, value);
        hash_table->nrNodes++;
        //TODO, resize hash table
    }

    MG_HASH_UNLOCK();
    return 0;
}

BOOL HashTableRemove (HASHTABLE *hash_table, GHANDLE key)
{
    HASHNODE **node;

    MG_CHECK_RET (hash_table != NULL, FALSE);

    node = hash_table_lookup_node (hash_table, key);
    if (*node)
    {
        HASHNODE *del = *node;

        (*node) = del->next;
        hash_node_destroy (del);

        hash_table->nrNodes--;
        //TODO resize
        return TRUE;
    }
  
    return FALSE;
}


size_t HashTableSize (HASHTABLE *hash_table)
{
    MG_CHECK_RET (hash_table != NULL, 0);
    return hash_table->nrNodes;
}

#if 0
static void hash_table_resize (HASHTABLE *hash_table)
{
    //TODO
}
#endif


static HASHNODE*
hash_node_new (GHANDLE key, GHANDLE value)
{
    HASHNODE *node;
  
    node = malloc (sizeof(HASHNODE));
  
    node->key   = key;
    node->value = value;
    node->next  = NULL;
  
    return node;
}

static void hash_node_destroy (HASHNODE *node)
{
    free (node);
}

#if 0
static void hash_nodes_destroy (HASHNODE *node)
{
    while (node)
    {
        HASHNODE *next = node->next;
        
        free (node);
        node = next;
    }
}
#endif
