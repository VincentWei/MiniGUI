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
** hash.h: header file of hash operations.
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

