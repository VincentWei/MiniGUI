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
** fontcache.c: Font cache for TrueType fonts.
** 
** Create date: 2006/02/01
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include "common.h"

#ifdef _MGFONT_TTF_CACHE
#include "minigui.h"
#include "gdi.h"

#ifdef _MGFONT_TTF 
#include <freetype/freetype.h>
#include <freetype/ftxkern.h>
#include <freetype/ftnameid.h>
#include <freetype/ftxcmap.h>
#include <freetype/ftxwidth.h>
#include "freetype.h"
#else 
#include "freetype2.h"
#endif


typedef struct _MemBlk
{
    void *data;
    int len;            
    struct _MemBlk *hashPrev; 
    struct _MemBlk *hashNext;
    struct _MemBlk *lruPrev;  
    struct _MemBlk *lruNext;
}MemBlk;

typedef MemBlk LruQueueDummyHead; 
typedef MemBlk HashQueueDummyHead;

/* hash directory array in every cache */
typedef struct _HashDirectory
{
    HashQueueDummyHead hashHead;
}HashDirectory;

/* The structure is description of Cache
   blkSize : the size of each block in cache(in byte)
   nBlk : how many blocks in cache
   nDir : how many directory entries in cache
   *memPool : the memory pool(data use it) pointer. 
   *lruQueueNodes : point all nodes by malloc. just 
                    release function use it  
   *hashDir : the hash directory pointer
   lruQueueDummyHead : head node of lru queue 
   makeHashKeyFunc : the function which make hash key */
typedef struct cache
{   
    /* Information about font */
    char family[LEN_FONT_NAME + 1];
    char charset[LEN_FONT_NAME + 1];
    DWORD style;
    int fontsize;
    
    int refers;
    int blkSize;
    int nBlk;
    int nDir;
    void *memPool;
    MemBlk *lruQueueNodes;
    HashDirectory *hashDir;
    LruQueueDummyHead lruQueueDummyHead;
    MakeHashKeyFunc makeHashKeyFunc;
}cache_t;

/* Least-Recently-Used-Order
   global cache LRU queue */
typedef struct _CacheQueueNode
{
    /* hold global cache queue 
       max cache node maybe 10 */
    cache_t cache;
    struct _CacheQueueNode *prevCache;
    struct _CacheQueueNode *nextCache;
}CacheQueueNode;

/* cache system. global cache descriptor,
   but user don't touch it, we use it!
   queueDummyHead : the head node of cache queue.
   
   maxCache : the max count of cache in programm
   cacheSize : each cache's size(in BYTE and 
               everyone has the same size!) 
   nCache : how many cache Now!! */
struct _CacheSystem
{
    CacheQueueNode queueDummyHead;
    
    int maxCache;
    int cacheSize;
    int nCache;
};

struct _CacheSystem __mg_globalCache;



/* functions */

static void 
AppendCache(CacheQueueNode *new)
{
    new->nextCache = &(__mg_globalCache.queueDummyHead);
    new->prevCache = __mg_globalCache.queueDummyHead.prevCache;
    (__mg_globalCache.queueDummyHead.prevCache)->nextCache = new;
    __mg_globalCache.queueDummyHead.prevCache = new;
}

/* create a new LRU-Queue. */
static int 
CreateCacheLruQueue(cache_t *cache, int nblk)
{
    int i;
    MemBlk *blk;
    
    blk = malloc(nblk * sizeof(MemBlk));
    DP(("Allocate cache %p  LRU == %p\n", cache, blk));
    if (blk == NULL) {
        return -1;
    }
    
    memset(blk, 0, nblk * sizeof(MemBlk));
    cache->lruQueueNodes = blk;
    /* link the node as a double-linked-loop-list */
    blk->lruPrev = &cache->lruQueueDummyHead;
    blk->lruNext = (blk + 1);
    for (i = 1; i < nblk - 1; i++) {
        (blk + i)->lruPrev = (blk + i - 1);
        (blk + i)->lruNext = (blk + i + 1);
    }
    (blk + nblk - 1)->lruPrev = blk + nblk - 2;
    (blk + nblk - 1)->lruNext = &cache->lruQueueDummyHead;
    
    cache->lruQueueDummyHead.lruPrev = blk + nblk - 1;
    cache->lruQueueDummyHead.lruNext = blk;
    return 0;
}

static int 
InitCache(CacheQueueNode *new, int ndir, int nblock)
{
    int i;
    /* allocate Hash directory array and LRU queue nodes 
       follow code is bad, but no way to fix it. */
    if (new->cache.hashDir) {
        free(new->cache.hashDir);
        DP(("free a old hash-dir, %p\n", new->cache.hashDir));
    }
    new->cache.hashDir = malloc(ndir * sizeof(HashDirectory));
    DP(("Allocate %p hash dir %p\n", new, new->cache.hashDir));
    if (new->cache.hashDir == NULL) {
        return -1;
    }
    
    memset(new->cache.hashDir, 0, ndir * sizeof(HashDirectory));
    for (i = 0; i < ndir; i++) {
        new->cache.hashDir[i].hashHead.hashPrev = 
          &(new->cache.hashDir[i].hashHead);
        new->cache.hashDir[i].hashHead.hashNext = 
          &(new->cache.hashDir[i].hashHead);
    }

    if (new->cache.lruQueueDummyHead.lruNext != 
        &(new->cache.lruQueueDummyHead)) {
        /* It's a old node, not malloced, 
           so we must release the old LRU-queue */
        free(new->cache.lruQueueNodes);
        DP(("free a old lru. %p\n", new->cache.lruQueueNodes));
    }
    if (CreateCacheLruQueue(&new->cache, nblock) == -1) {
        if (new->cache.lruQueueNodes) {
            free(new->cache.lruQueueNodes);
            DP(("free a old lru. %p\n", new->cache.lruQueueNodes));
        }
        return -1; 
    }

    return 0;
}


static void
LruQueueBufferSplit(cache_t *cache, int bufsize)
{
    MemBlk *blk;
    unsigned char *buff;

    blk = cache->lruQueueDummyHead.lruNext;
    buff = cache->memPool;
    while (blk != &cache->lruQueueDummyHead) {
        blk->data = buff;
        blk = blk->lruNext;
        buff = buff + bufsize;
    } 
}

static void 
DestroyCache(cache_t *cache)
{
    if (cache->hashDir != NULL) {
        free(cache->hashDir);
        DP(("destory cache %p hash dir %p\n", cache, cache->hashDir));
    }
    if (cache->memPool != NULL) {
        DP(("destory cache %p memory pool %p\n", cache, cache->memPool));
        free(cache->memPool);
    }
    if (cache->lruQueueNodes) {
        DP(("destory cache %p LRU_QUEUE %p\n", cache, cache->lruQueueNodes));
        free(cache->lruQueueNodes);
    }
}

/* release a cache node from the global LRU cache queue.
   Free the memory and unlink the pointers. */
static void 
ReleaseCache(CacheQueueNode *cache)
{
    /* Re-Link the double link queue */
    if (cache->prevCache != NULL && cache->nextCache != NULL) {
        cache->nextCache->prevCache = cache->prevCache;
        cache->prevCache->nextCache = cache->nextCache;
    }
    DestroyCache(&cache->cache);
}



/* allocate a new cache and append at the end of the global cache queue.
 * If global cache queue is full, we should kill the first node of the queue,
 * and we call this : Least-Recently-Used-Order
 * If global cache is not full, just allocate a new one. 
 * the func return pointer to the cache if succeed, return NULL means failed. */
static CacheQueueNode *
GetCache(int nblock, int ndir)
{
    CacheQueueNode *temp;
    
    if (__mg_globalCache.nCache == __mg_globalCache.maxCache) {
        /* The Queue is Full!! in this case, we don't 
           release any node, just return NULL */
        return NULL;
    }
    /* there is empty space in the queue 
       we should allocate a new CacheQueueNode and 
       allocate Memory-Pool for it */
    if ((temp = malloc(sizeof(CacheQueueNode))) == NULL) {
        return NULL;
    }
    DP(("New a cache node %p\n", temp));
    /* Set zero the new CacheQueueNode 
       and link the head to itself */
    memset(temp, 0, sizeof(CacheQueueNode));
    temp->cache.lruQueueDummyHead.lruPrev = &(temp->cache.lruQueueDummyHead);
    temp->cache.lruQueueDummyHead.lruNext = &(temp->cache.lruQueueDummyHead);
    /* allocate Memory-Pool for new new CacheQueueNode */
    temp->cache.memPool = malloc(__mg_globalCache.cacheSize);
    DP(("allocate cache %p mem pool %p\n", temp, temp->cache.memPool));
    if (temp->cache.memPool == NULL) {
        free(temp);
        return NULL;
    }
    __mg_globalCache.nCache++;
    
    /* init hash directory, lru-queue 
       the function should release the old hash directory
       and lru-queue and allocate new one.  */
    if (InitCache(temp, ndir, nblock) == -1) {
        /* init node failed. we must FREE has directory
           Memory-Poll and Lru-queue in follow function.
           And unlink the global cache queue if required */
        ReleaseCache(temp);
        free(temp);
        __mg_globalCache.nCache--;
        return NULL;
    }
    /* link the node at the end of global cache queue */
    AppendCache(temp);
    return temp;
}


/* Pick-off a block from the head of cache's 
   lru-queue and also Hash queue(anywhere in hash q).
   Relink the lru queue and hash queue */
static MemBlk *
PickOffBlk(cache_t *cache)
{
    MemBlk *blk;
    
    if (cache->lruQueueDummyHead.lruNext ==  
        &cache->lruQueueDummyHead) {
        return NULL;
    }
    /* get the head of the lru-queue 
       it is a Least-Recently-Used-Order Node 
       and we should re-link the queue */
    blk = cache->lruQueueDummyHead.lruNext;
    
    cache->lruQueueDummyHead.lruNext= 
      (cache->lruQueueDummyHead.lruNext)->lruNext;
    (cache->lruQueueDummyHead.lruNext)->lruPrev = 
      &cache->lruQueueDummyHead;
    /* we pick the block, and if the block already 
       in a hash q, so we must re-link the hash q */
    if (blk->hashNext && blk->hashPrev) {
        blk->hashNext->hashPrev = blk->hashPrev;
        blk->hashPrev->hashNext = blk->hashNext; 
    }
    return blk;
}

static void 
AppendBlk(cache_t *cache, MemBlk *blk, int key)
{
    /* first, append the blcok at the end of the 
       lru-queue. */
    blk->lruNext = &(cache->lruQueueDummyHead);
    blk->lruPrev = cache->lruQueueDummyHead.lruPrev;
    (cache->lruQueueDummyHead.lruPrev)->lruNext = blk;
    cache->lruQueueDummyHead.lruPrev = blk;
    
    /* Then, insert the block at the head of 
       the hash queue which KEY==key */
    blk->hashNext = cache->hashDir[key].hashHead.hashNext;
    (cache->hashDir[key].hashHead.hashNext)->hashPrev = blk;
    cache->hashDir[key].hashHead.hashNext = blk;
    blk->hashPrev = &(cache->hashDir[key].hashHead);

}



/**********************************************************
 *                    extern functions                    *
 **********************************************************/
/* search a data in cache, 
   hCache : which cache
   *cmpdata : your data pointer point to data for compare
   *size : if found the data, how long is the data(return val)
   reutrn = pointer to the data */ 
TTFCACHEINFO *
__mg_ttc_search(HCACHE hCache, unsigned short unicode, int *size)
{
    int key;
    CacheQueueNode *ca;
    MemBlk *blk;

    if (hCache == 0 || size == NULL) {
        return NULL;
    }
    ca = (CacheQueueNode *)(hCache);
    key = ca->cache.makeHashKeyFunc(unicode);
    if (key > (ca->cache.nDir - 1)) {
        return NULL;
    }
    blk = ca->cache.hashDir[key].hashHead.hashNext;

    for ( ; blk != &(ca->cache.hashDir[key].hashHead); 
          blk = blk->hashNext) {
        if (*(unsigned short *)(blk->data) == unicode) {
            /* If found the blk, we should pick off it
               and link it at the end of LRU-q */
            *size = blk->len;
            blk->lruNext->lruPrev = blk->lruPrev;
            blk->lruPrev->lruNext = blk->lruNext;
            blk->lruNext = &(ca->cache.lruQueueDummyHead);
            blk->lruPrev = ca->cache.lruQueueDummyHead.lruPrev;
            (ca->cache.lruQueueDummyHead.lruPrev)->lruNext = blk;
            ca->cache.lruQueueDummyHead.lruPrev = blk;
            return blk->data;
        }
    }
    return NULL;
}

/* add data into a speicial cache 
   hCache : which cache , 
   *data : your data pointer
   size : your data's bytes
   key : hash key of your data */
int 
__mg_ttc_write(HCACHE hCache, TTFCACHEINFO *data, int size)
{
    CacheQueueNode *ca;
    MemBlk *blk;
    int key; 

    if (hCache == 0 || data == NULL) {
        return -1;
    }
    ca = (CacheQueueNode *)(hCache);
    if (ca->cache.blkSize < size) {
        return -1;
    }
    key = ca->cache.makeHashKeyFunc(*(unsigned short *)data);
    
    /* pick off a block from lru queue */
    if ((blk = PickOffBlk(&ca->cache)) == NULL) {
        return -1;
    }
    /* 1. copy the TTFCACHEINFO structure
       2. copy the BITMAP data by data->bitmap 
       3. change the bitmap pointer */
    memcpy(blk->data, data, sizeof(TTFCACHEINFO));
    if (data->bitmap)
        memcpy((blk->data + sizeof(TTFCACHEINFO)), 
                data->bitmap, 
                (size - sizeof(TTFCACHEINFO)));
    blk->len = size;
    ((TTFCACHEINFO *)(blk->data))->bitmap =  blk->data + sizeof(TTFCACHEINFO);
    /* append the block to the end of lru-queue */
    AppendBlk(&ca->cache, blk, key);
    return 0;
}

/* create a cache for a font instance. 
   nblk : how many block in the cache.
   blks  ize : each block's size
   ndir : how many hash entries in the cache
   makeHashKey : the function which make the hash key */
HCACHE  
__mg_ttc_create(char *family, char *charset, DWORD style, int size, 
    int nblk, int blksize, int ndir, MakeHashKeyFunc makeHashKey)
{
    CacheQueueNode *temp;
    if (family == NULL || charset == NULL || makeHashKey == NULL) {
        return (HCACHE)(0);
    }
    if (nblk <= 0 || blksize <= 0 || size <= 0 || ndir <= 0) {
        return (HCACHE)(0);
    }

    if ((nblk * blksize) > __mg_globalCache.cacheSize) {
        return (HCACHE)(0);
    }
    if ((temp = GetCache(nblk, ndir)) == NULL) {
        return (HCACHE)(0);
    }

    strncpy(temp->cache.family, family, LEN_FONT_NAME);
    strncpy(temp->cache.charset, charset, LEN_FONT_NAME);
    temp->cache.style = style;
    temp->cache.fontsize = size;
    temp->cache.blkSize = blksize;
    temp->cache.nBlk = nblk;
    temp->cache.nDir = ndir;
    temp->cache.makeHashKeyFunc = makeHashKey;
    /* Cut the Big memory pool into some small block */
    LruQueueBufferSplit(&temp->cache, blksize);
    temp->cache.refers = 1;
    return (HCACHE)(temp);
}

void 
__mg_ttc_refer(HCACHE hCache)
{
    CacheQueueNode *ca;
    if (hCache == 0) {
        return;
    }
    ca = (CacheQueueNode *)(hCache);
    ca->cache.refers++; 
}

void 
__mg_ttc_release(HCACHE hCache)
{
    CacheQueueNode *ca;
    if (hCache == 0) {
        return;
    }
    ca = (CacheQueueNode *)(hCache);
    ca->cache.refers--;
    DP(("cache %p -- %d\n", ca, ca->cache.refers));
    if (ca->cache.refers == 0) {
        /* It will re-link the global cache list in ReleaseCache() */
        ReleaseCache(ca);
        free(ca);
        DP(("Free cache==%p\n", ca));
        __mg_globalCache.nCache--;        
    }
}

/* init cache system, create cache queue, and clear cache counter,
   if need use cache, should call this function first */
int 
__mg_ttc_sys_init(int maxCache, int cacheSize)
{
    if (maxCache <= 0 || cacheSize <= 0) {
        return -1;
    }
    __mg_globalCache.queueDummyHead.prevCache = &__mg_globalCache.queueDummyHead;
    __mg_globalCache.queueDummyHead.nextCache = &__mg_globalCache.queueDummyHead;
    __mg_globalCache.maxCache = maxCache;
    __mg_globalCache.nCache = 0;
    __mg_globalCache.cacheSize = cacheSize;
    return 0;
}


void 
__mg_ttc_sys_deinit(void)
{
    CacheQueueNode *q, *pfree;
    
    if (__mg_globalCache.nCache == 0) {
        /* it means just no cache in our system, 
           no malloc at all, just set zero global val 
           and return safe :) */
        memset(&__mg_globalCache, 0, sizeof(__mg_globalCache));
        return;
    }
    /* In this case, it means more than ONE cache in 
       our system. we should release follow structures: 
       a. every CacheQueueNode(s) in global cache queue
       b. every Memory-Pool in every cache 
       c. every Hash-Directory in every cache
       d. every LRU queue nodes in every cache 
       that's all, may be~~ :(  */
    q = __mg_globalCache.queueDummyHead.nextCache;
    
    while (q != &__mg_globalCache.queueDummyHead) {
        pfree = q;
        q = q->nextCache;
        DestroyCache(&pfree->cache);
        free(pfree);
        DP(("Free cache==%p\n", pfree));
    }
}



HCACHE 
__mg_ttc_is_exist(char *family, char *charset, DWORD style, int size)
{
    CacheQueueNode *p;

    if (__mg_globalCache.nCache == 0) {
        return (HCACHE)0;
    }
    
    /* If can not match font name*/
    p = __mg_globalCache.queueDummyHead.nextCache;
    while (p != &__mg_globalCache.queueDummyHead) {
        if (strncmp(p->cache.family, family, LEN_FONT_NAME) == 0) {
            if (strncmp(p->cache.charset, charset, LEN_FONT_NAME) == 0) {
                if (p->cache.fontsize == size) {
                    int s = style & 0x000000FF;
                    if ((s == FS_WEIGHT_BOOK || s == FS_WEIGHT_DEMIBOLD || s == FS_WEIGHT_SUBPIXEL) && 
                        (p->cache.style == FS_WEIGHT_BOOK || p->cache.style == FS_WEIGHT_DEMIBOLD || 
                         p->cache.style == FS_WEIGHT_SUBPIXEL)) {
                        return (HCACHE) p;
                    }
                    if ((s != FS_WEIGHT_BOOK && s != FS_WEIGHT_DEMIBOLD && s != FS_WEIGHT_SUBPIXEL) && 
                        (p->cache.style != FS_WEIGHT_BOOK && p->cache.style != FS_WEIGHT_DEMIBOLD && 
                         p->cache.style != FS_WEIGHT_SUBPIXEL)) {
                        return (HCACHE) p;
                    }
                }
            }
        }
        p = p->nextCache;
    }
    return (HCACHE)0;
}

#if 0

static void 
queue_traversal(LruQueueDummyHead *pQ)
{
    MemBlk *ptr;

    ptr = pQ->lruNext;
    for (; ptr != pQ; ptr = ptr->lruNext) {
        DP(("MemBlk = %p, MemBlk->prev = %p, "
            "MemBlk->next = %p, MemBlk->data : %d\n", 
            ptr, ptr->lruPrev, ptr->lruNext, *(int *)(ptr->data)));
    } 
}

static void 
queue_traversal_reverse(LruQueueDummyHead *pQ)
{
    MemBlk *ptr;

    ptr = pQ->lruPrev;
    for ( ; ptr != pQ; ptr = ptr->lruPrev) {
        DP(("MemBlk = %p, MemBlk->prev = %p, "
            "MemBlk->next = %p, MemBlk->data : %d\n", 
            ptr, ptr->lruPrev, ptr->lruNext, *(int *)(ptr->data)));
    } 
}


void __mg_ttc_dump_all(void)
{
    CacheQueueNode* pc;
#if 0
    DP(("__mg_globalCache.maxCache = %d\n", __mg_globalCache.maxCache));
    DP(("__mg_globalCache.cacheSize = %d\n", __mg_globalCache.cacheSize));
    DP(("__mg_globalCache.nCache = %d\n", __mg_globalCache.nCache));
    
    DP(("__mg_globalCache.queueDummyHead.prevCache = %p\n", 
           __mg_globalCache.queueDummyHead.prevCache));
    DP(("__mg_globalCache.queueDummyHead.nextCache = %p\n",
           __mg_globalCache.queueDummyHead.nextCache));
#endif 
    pc = __mg_globalCache.queueDummyHead.nextCache;
    for ( ; pc != &__mg_globalCache.queueDummyHead; pc = pc->nextCache) {
        __mg_ttc_dump(&pc->cache);
        DP(("* * * * * * * * * * * * * * * * * * * * * * * * * * * *\n"));
    }
}

void __mg_ttc_dump(cache_t *cache)
{
    int i, j, k;
    MemBlk *ptr;
#if 0    
    DP(("&cache = %p\n", cache));
    DP(("cache.family = %s\n", cache->family));
    DP(("cache.charset = %s\n", cache->charset));
    DP(("cache.style = %d\n", cache->style));
    DP(("cache.fontsize = %d\n", cache->fontsize));
    DP(("cache.blkSize = %d\n", cache->blkSize));
    DP(("cache.refers = %d\n", cache->refers));
    DP(("cache.nBlk = %d\n", cache->nBlk));
    DP(("cache.nDir = %d\n", cache->nDir));
    DP(("cache.memPool = %d, (HEX) = %p\n", 
           cache->memPool, cache->memPool));
    DP(("cache.lruQueueNodes=%p\n", cache->lruQueueNodes));
    DP(("cache.hashDir = %p\n", cache->hashDir));
#endif 
    DP(("&cache.lruQueueDummyHead=%p\n",
           &cache->lruQueueDummyHead));
    DP(("cache.lruQueueDummyHead.lruPrev=%p\n",
           cache->lruQueueDummyHead.lruPrev));
    DP(("cache.lruQueueDummyHead.lruNext=%p\n",
           cache->lruQueueDummyHead.lruNext));
    //queue_traversal(&cache->lruQueueDummyHead);
    //DP(("----\n"));
    //queue_traversal_reverse(&cache->lruQueueDummyHead);
#if 0        
    for (i=0; i<cache->nDir; i++) {
        MemBlk *p;
        printf("hash[%d]:", i);
        p = (cache->hashDir)[i].hashHead.hashNext;
        while(p != &(cache->hashDir[i].hashHead)) {
            printf("-->%d(%d)", *(unsigned short *)(p->data), p->len);
            p = p->hashNext;
        }
        printf("\n");
    }
#endif
    //printf("LRU: ");  
    
    ptr = cache->lruQueueDummyHead.lruNext;
    for ( ; ptr != &cache->lruQueueDummyHead; ptr = ptr->lruNext) {
        DP(("-->%p(len:%d, data:%d)", 
            ptr, ptr->len, *(unsigned short *)(ptr->data)));
    }
    //printf("\n");  
}

void 
__mg_ttc_dump_cacheinfo(TTFCACHEINFO *ca)
{
    DP(("Dump TTFCACHEINFO %p...\n", ca));
    DP(("cacheInfo->unicode = %d\n", ca->unicode));
    DP(("cacheInfo->glyph_code = %d\n", ca->glyph_code));
    
    DP(("cacheInfo->cur_xmin = %d, cacheInfo->cur_ymin = %d\n",
        ca->cur_xmin, ca->cur_ymin));
    DP(("cacheInfo->width = %d, cacheInfo->height = %d\n",
        ca->width, ca->height));
    DP(("cacheInfo->advance = %d\n", ca->advance));
    DP(("cacheInfo->cur_bbox = %d, %d, %d, %d\n",
        ca->bbox.xMin, ca->bbox.yMin, 
        ca->bbox.xMax, ca->bbox.yMax));

    DP(("\n"));
}

void 
__mg_ttc_dump_instance(TTFINSTANCEINFO *in)
{
    DP(("Dump TTFINSTANCEINFO %p...\n", in));
    DP(("Instance->cur_unicode = %d\n", in->cur_unicode));
    DP(("Instance->cur_glyph_code = %d\n", in->cur_glyph_code));
    DP(("Instance->outline(addr) = %p\n", &in->cur_outline));
    DP(("Instance->cur_xmin = %d, Instance->cur_ymin = %d\n",
        in->cur_xmin, in->cur_ymin));
    DP(("Instance->cur_width = %d, Instance->cur_height = %d\n",
        in->cur_width, in->cur_height));
    DP(("Instance->advance = %d\n", in->cur_advance));
    DP(("Instance->cur_bbox = %d, %d, %d, %d\n",
        in->cur_bbox.xMin, in->cur_bbox.yMin, 
        in->cur_bbox.xMax, in->cur_bbox.yMax));
    
    DP(("\n"));
}

#endif /* 0 */

#endif /*  _MGFONT_TTF_CACHE */

