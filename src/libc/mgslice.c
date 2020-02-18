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
 *   Copyright (C) 2019, Beijing FMSoft Technologies Co., Ltd.
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
** sliced memory - a fast concurrent memory chunk allocator
**
** Create by WEI Yongming at 2019/04/04
**
** This implementation is derived from LGPL'd GLib:
**
** Copyright (C) 2005 Tim Janik
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, see <http://www.gnu.org/licenses/>.
*/

#include "common.h"
#include "minigui.h"

#ifdef _MGSLICE_FALLBACK

#include <stdlib.h>

void *mg_slice_alloc(size_t block_size)
{
    return malloc (block_size);
}

void *mg_slice_alloc0(size_t block_size)
{
    return calloc (1, block_size);
}

void *mg_slice_copy(size_t block_size, const void *mem_block)
{
    void* new_slice = malloc (block_size);
    memcpy (new_slice, mem_block, block_size);
    return new_slice;
}

void mg_slice_free(size_t block_size, void *mem_block)
{
    free (mem_block);
}

void mg_slice_free_chain_with_offset(size_t block_size,
        void *mem_chain, size_t next_offset)
{
    void* slice = mem_chain;

    while (slice) {
        Uint8 *current = slice;
        slice = *(void **) (current + next_offset);
        free (current);
    }
}

void mg_slice_debug_tree_statistics(void)
{
    _MG_PRINTF("MGSlice: we are using the fallback implementation\n");
}

#else /* define _MGSLICE_FALLBACK */

#if defined(HAVE_POSIX_MEMALIGN) && !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE 600       /* posix_memalign() */
#endif
#include <stdlib.h>             /* posix_memalign() */
#include <string.h>
#include <errno.h>

#ifndef __NOUNIX__
#include <unistd.h>             /* sysconf() */
#include <sys/time.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <process.h>
#endif

#include <stdio.h>              /* fputs */
#include <pthread.h>            /* pthread_xxx */

/* the slice allocator is split up into 4 layers, roughly modelled after the slab
 * allocator and magazine extensions as outlined in:
 * + [Bonwick94] Jeff Bonwick, The slab allocator: An object-caching kernel
 *   memory allocator. USENIX 1994, http://citeseer.ist.psu.edu/bonwick94slab.html
 * + [Bonwick01] Bonwick and Jonathan Adams, Magazines and vmem: Extending the
 *   slab allocator to many cpu's and arbitrary resources.
 *   USENIX 2001, http://citeseer.ist.psu.edu/bonwick01magazines.html
 * the layers are:
 * - the thread magazines. for each (aligned) chunk size, a magazine (a list)
 *   of recently freed and soon to be allocated chunks is maintained per thread.
 *   this way, most alloc/free requests can be quickly satisfied from per-thread
 *   free lists which only require one pthread_getspecific() call to retrive the
 *   thread handle.
 * - the magazine cache. allocating and freeing chunks to/from threads only
 *   occours at magazine sizes from a global depot of magazines. the depot
 *   maintaines a 15 second working set of allocated magazines, so full
 *   magazines are not allocated and released too often.
 *   the chunk size dependent magazine sizes automatically adapt (within limits,
 *   see [3]) to lock contention to properly scale performance across a variety
 *   of SMP systems.
 * - the slab allocator. this allocator allocates slabs (blocks of memory) close
 *   to the system page size or multiples thereof which have to be page aligned.
 *   the blocks are divided into smaller chunks which are used to satisfy
 *   allocations from the upper layers. the space provided by the reminder of
 *   the chunk size division is used for cache colorization (random distribution
 *   of chunk addresses) to improve processor cache utilization. multiple slabs
 *   with the same chunk size are kept in a partially sorted ring to allow O(1)
 *   freeing and allocation of chunks (as long as the allocation of an entirely
 *   new slab can be avoided).
 * - the page allocator. on most modern systems, posix_memalign(3) or
 *   memalign(3) should be available, so this is used to allocate blocks with
 *   system page size based alignments and sizes or multiples thereof.
 *   if no memalign variant is provided, valloc() is used instead and
 *   block sizes are limited to the system page size (no multiples thereof).
 *   as a fallback, on system without even valloc(), a malloc(3)-based page
 *   allocator with alloc-only behaviour is used.
 *
 * NOTES:
 * [1] some systems memalign(3) implementations may rely on boundary tagging for
 *     the handed out memory chunks. to avoid excessive page-wise fragmentation,
 *     we reserve 2 * sizeof (void*) per block size for the systems memalign(3),
 *     specified in NATIVE_MALLOC_PADDING.
 * [2] using the slab allocator alone already provides for a fast and efficient
 *     allocator, it doesn't properly scale beyond single-threaded uses though.
 *     also, the slab allocator implements eager free(3)-ing, i.e. does not
 *     provide any form of caching or working set maintenance. so if used alone,
 *     it's vulnerable to trashing for sequences of balanced (alloc, free) pairs
 *     at certain thresholds.
 * [3] magazine sizes are bound by an implementation specific minimum size and
 *     a chunk size specific maximum to limit magazine storage sizes to roughly
 *     16KB.
 * [4] allocating ca. 8 chunks per block/page keeps a good balance between
 *     external and internal fragmentation (<= 12.5%). [Bonwick94]
 */

/* --- macros and constants --- */
#define LARGEALIGNMENT          (256)
#define P2ALIGNMENT             (2 * sizeof (size_t))                            /* fits 2 pointers (assumed to be 2 * GLIB_SIZEOF_SIZE_T below) */
#define ALIGN(size, base)       ((base) * (size_t) (((size) + (base) - 1) / (base)))
#define NATIVE_MALLOC_PADDING   P2ALIGNMENT                                     /* per-page padding left for native malloc(3) see [1] */
#define SLAB_INFO_SIZE          P2ALIGN (sizeof (SlabInfo) + NATIVE_MALLOC_PADDING)
#define MAX_MAGAZINE_SIZE       (256)                                           /* see [3] and allocator_get_magazine_threshold() for this */
#define MIN_MAGAZINE_SIZE       (4)
#define MAX_STAMP_COUNTER       (7)                                             /* distributes the load of gettimeofday() */
#define MAX_SLAB_CHUNK_SIZE(al) (((al)->max_page_size - SLAB_INFO_SIZE) / 8)    /* we want at last 8 chunks per page, see [4] */
#define MAX_SLAB_INDEX(al)      (SLAB_INDEX (al, MAX_SLAB_CHUNK_SIZE (al)) + 1)
#define SLAB_INDEX(al, asize)   ((asize) / P2ALIGNMENT - 1)                     /* asize must be P2ALIGNMENT aligned */
#define SLAB_CHUNK_SIZE(al, ix) (((ix) + 1) * P2ALIGNMENT)
#define SLAB_BPAGE_SIZE(al,csz) (8 * (csz) + SLAB_INFO_SIZE)

/* optimized version of ALIGN (size, P2ALIGNMENT) */
#if     GLIB_SIZEOF_SIZE_T * 2 == 8  /* P2ALIGNMENT */
#define P2ALIGN(size)   (((size) + 0x7) & ~(size_t) 0x7)
#elif   GLIB_SIZEOF_SIZE_T * 2 == 16 /* P2ALIGNMENT */
#define P2ALIGN(size)   (((size) + 0xf) & ~(size_t) 0xf)
#else
#define P2ALIGN(size)   ALIGN (size, P2ALIGNMENT)
#endif

/* special helpers to avoid gmessage.c dependency */
static void mem_error (const char *format, ...);
#define mem_assert(cond)  \
    do { \
        if (MG_LIKELY (cond)) \
            ; \
        else \
            mem_error ("assertion failed: %s", #cond); \
    } while (0)

/* --- structures --- */
typedef struct _ChunkLink      ChunkLink;
typedef struct _SlabInfo       SlabInfo;
typedef struct _CachedMagazine CachedMagazine;
struct _ChunkLink {
    ChunkLink *next;
    ChunkLink *data;
};
struct _SlabInfo {
    ChunkLink *chunks;
    unsigned int n_allocated;
    SlabInfo *next, *prev;
};
typedef struct {
    ChunkLink *chunks;
    size_t      count;                     /* approximative chunks list length */
} Magazine;

typedef struct {
    Magazine   *magazine1;                /* array of MAX_SLAB_INDEX (allocator) */
    Magazine   *magazine2;                /* array of MAX_SLAB_INDEX (allocator) */
} ThreadMemory;

typedef struct {
    BOOL always_malloc;
    BOOL bypass_magazines;
    BOOL debug_blocks;
    size_t    working_set_msecs;
    unsigned int    color_increment;
} SliceConfig;

typedef struct {
    /* const after initialization */
    size_t          min_page_size, max_page_size;
    SliceConfig     config;
    size_t          max_slab_chunk_size_for_magazine_cache;
    /* magazine cache */
    pthread_mutex_t magazine_mutex;
    ChunkLink     **magazines;                /* array of MAX_SLAB_INDEX (allocator) */
    unsigned int   *contention_counters;      /* array of MAX_SLAB_INDEX (allocator) */
    int             mutex_counter;
    uintptr_t       stamp_counter;
    uintptr_t       last_stamp;
    /* slab allocator */
    pthread_mutex_t slab_mutex;
    SlabInfo      **slab_stack;                /* array of MAX_SLAB_INDEX (allocator) */
    unsigned int    color_accu;
} Allocator;

/* --- mg-slice prototypes --- */
static void *slab_allocator_alloc_chunk(size_t chunk_size);
static void  slab_allocator_free_chunk(size_t chunk_size, void *mem);
static void  private_thread_memory_cleanup(void *data);
static void *allocator_memalign(size_t alignment, size_t memsize);
static void  allocator_memfree(size_t memsize, void *mem);
static inline void magazine_cache_update_stamp (void);
static inline size_t allocator_get_magazine_threshold(Allocator *allocator, unsigned int ix);

/* --- mg-slice memory checker --- */
#ifdef _MGDEVEL_MODE
static void smc_notify_alloc(void *pointer, size_t size);
static int  smc_notify_free(void *pointer, size_t size);
#else
static inline void smc_notify_alloc (void *pointer, size_t size)
{
    return;
}

static inline int smc_notify_free (void *pointer, size_t size)
{
    return 1;
}
#endif

static BOOL _mem_gc_friendly;
static pthread_key_t private_thread_memory;

BOOL mg_InitSliceAllocator(void)
{
    const char *val;

    val = getenv ("MG_DEBUG");
    if (val != NULL && strstr(val, "gc-friendly"))
        _mem_gc_friendly = TRUE;

    return pthread_key_create(&private_thread_memory,
            private_thread_memory_cleanup) == 0;
}

void mg_TerminateSliceAllocator(void)
{
#ifdef _MGDEVEL_MODE
    mg_slice_debug_tree_statistics();
#endif
    pthread_key_delete(private_thread_memory);
}

static size_t      sys_page_size = 0;
static Allocator   allocator[1] = { { 0, }, };
static SliceConfig slice_config = {
    FALSE,        /* always_malloc */
    FALSE,        /* bypass_magazines */
    FALSE,        /* debug_blocks */
    15 * 1000,    /* working_set_msecs */
    1,            /* color increment, alt: 0x7fffffff */
};

static void slice_config_init (SliceConfig *config)
{
    const char *val;

    *config = slice_config;

    val = getenv ("MG_SLICE");
    if (val != NULL) {
        if (strstr(val, "always-malloc"))
            config->always_malloc = TRUE;
#ifdef _MGDEVEL_MODE
        if (strstr(val, "debug-blocks"))
            config->debug_blocks = TRUE;
#endif
    }
}

#define mg_new0(type, n) calloc(n, sizeof(type))

static void mg_slice_init_nomessage (void)
{
    /* we may not use g_error() or friends here */
    mem_assert (sys_page_size == 0);
    mem_assert (MIN_MAGAZINE_SIZE >= 4);

#ifdef WIN32
    {
        SYSTEM_INFO system_info;
        GetSystemInfo (&system_info);
        sys_page_size = system_info.dwPageSize;
    }
#else
    sys_page_size = sysconf (_SC_PAGESIZE); /* = sysconf (_SC_PAGE_SIZE); = getpagesize(); */
#endif
    mem_assert (sys_page_size >= 2 * LARGEALIGNMENT);
    mem_assert ((sys_page_size & (sys_page_size - 1)) == 0);
    slice_config_init (&allocator->config);
    allocator->min_page_size = sys_page_size;
#if HAVE_POSIX_MEMALIGN || HAVE_MEMALIGN
    /* allow allocation of pages up to 8KB (with 8KB alignment).
     * this is useful because many medium to large sized structures
     * fit less than 8 times (see [4]) into 4KB pages.
     * we allow very small page sizes here, to reduce wastage in
     * threads if only small allocations are required (this does
     * bear the risk of increasing allocation times and fragmentation
     * though).
     */
    allocator->min_page_size = MAX (allocator->min_page_size, 4096);
    allocator->max_page_size = MAX (allocator->min_page_size, 8192);
    allocator->min_page_size = MIN (allocator->min_page_size, 128);
#else
    /* we can only align to system page size */
    allocator->max_page_size = sys_page_size;
#endif
    if (allocator->config.always_malloc)
    {
        allocator->contention_counters = NULL;
        allocator->magazines = NULL;
        allocator->slab_stack = NULL;
    }
    else
    {
        allocator->contention_counters = mg_new0 (unsigned int, MAX_SLAB_INDEX (allocator));
        allocator->magazines = mg_new0 (ChunkLink*, MAX_SLAB_INDEX (allocator));
        allocator->slab_stack = mg_new0 (SlabInfo*, MAX_SLAB_INDEX (allocator));
    }

    pthread_mutex_init(&allocator->magazine_mutex, NULL);
    pthread_mutex_init(&allocator->slab_mutex, NULL);

    allocator->mutex_counter = 0;
    allocator->stamp_counter = MAX_STAMP_COUNTER; /* force initial update */
    allocator->last_stamp = 0;
    allocator->color_accu = 0;
    magazine_cache_update_stamp();
    /* values cached for performance reasons */
    allocator->max_slab_chunk_size_for_magazine_cache = MAX_SLAB_CHUNK_SIZE (allocator);
    if (allocator->config.always_malloc || allocator->config.bypass_magazines)
        allocator->max_slab_chunk_size_for_magazine_cache = 0;      /* non-optimized cases */
}

static inline unsigned int allocator_categorize (size_t aligned_chunk_size)
{
    /* speed up the likely path */
    if (MG_LIKELY (aligned_chunk_size && aligned_chunk_size <= allocator->max_slab_chunk_size_for_magazine_cache))
        return 1;           /* use magazine cache */

    if (!allocator->config.always_malloc &&
            aligned_chunk_size &&
            aligned_chunk_size <= MAX_SLAB_CHUNK_SIZE (allocator))
    {
        if (allocator->config.bypass_magazines)
            return 2;       /* use slab allocator, see [2] */
        return 1;         /* use magazine cache */
    }
    return 0;             /* use malloc() */
}

static inline void g_mutex_lock_a (pthread_mutex_t *mutex,
        unsigned int  *contention_counter)
{
    BOOL contention = FALSE;
    if (pthread_mutex_trylock(mutex) != 0) {
        pthread_mutex_lock (mutex);
        contention = TRUE;
    }

    if (contention) {
        allocator->mutex_counter++;
        if (allocator->mutex_counter >= 1)        /* quickly adapt to contention */
        {
            allocator->mutex_counter = 0;
            *contention_counter = MIN (*contention_counter + 1, MAX_MAGAZINE_SIZE);
        }
    }
    else /* !contention */
    {
        allocator->mutex_counter--;
        if (allocator->mutex_counter < -11)       /* moderately recover magazine sizes */
        {
            allocator->mutex_counter = 0;
            *contention_counter = MAX (*contention_counter, 1) - 1;
        }
    }
}

static inline void *_steal_pointer (void *pp)
{
    void **ptr = (void **) pp;
    void * ref;

    ref = *ptr;
    *ptr = NULL;

    return ref;
}

static void* _private_set_alloc0 (pthread_key_t key, size_t size)
{
    void* allocated = calloc (1, size);

    pthread_setspecific (key, allocated);

    return _steal_pointer (&allocated);
}

static inline ThreadMemory* thread_memory_from_self (void)
{
    ThreadMemory *tmem = pthread_getspecific (private_thread_memory);
    if (MG_UNLIKELY (!tmem))
    {
        static pthread_mutex_t init_mutex;
        unsigned int n_magazines;

        pthread_mutex_lock (&init_mutex);
        if MG_UNLIKELY (sys_page_size == 0)
            mg_slice_init_nomessage ();
        pthread_mutex_unlock (&init_mutex);

        n_magazines = MAX_SLAB_INDEX (allocator);
        tmem = _private_set_alloc0 (private_thread_memory,
                sizeof (ThreadMemory) + sizeof (Magazine) * 2 * n_magazines);
        tmem->magazine1 = (Magazine*) (tmem + 1);
        tmem->magazine2 = &tmem->magazine1[n_magazines];
    }
    return tmem;
}

static inline ChunkLink* magazine_chain_pop_head(ChunkLink **magazine_chunks)
{
    /* magazine chains are linked via ChunkLink->next.
     * each ChunkLink->data of the toplevel chain may point to a subchain,
     * linked via ChunkLink->next. ChunkLink->data of the subchains just
     * contains uninitialized junk.
     */
    ChunkLink *chunk = (*magazine_chunks)->data;
    if (MG_UNLIKELY (chunk))
    {
        /* allocating from freed list */
        (*magazine_chunks)->data = chunk->next;
    }
    else
    {
        chunk = *magazine_chunks;
        *magazine_chunks = chunk->next;
    }
    return chunk;
}

static inline size_t allocator_get_magazine_threshold (Allocator *allocator,
        unsigned int ix)
{
    /* the magazine size calculated here has a lower bound of MIN_MAGAZINE_SIZE,
     * which is required by the implementation. also, for moderately sized chunks
     * (say >= 64 bytes), magazine sizes shouldn't be much smaller then the number
     * of chunks available per page/2 to avoid excessive traffic in the magazine
     * cache for small to medium sized structures.
     * the upper bound of the magazine size is effectively provided by
     * MAX_MAGAZINE_SIZE. for larger chunks, this number is scaled down so that
     * the content of a single magazine doesn't exceed ca. 16KB.
     */
    size_t chunk_size = SLAB_CHUNK_SIZE (allocator, ix);
    unsigned int threshold = MAX (MIN_MAGAZINE_SIZE, allocator->max_page_size / MAX (5 * chunk_size, 5 * 32));
    unsigned int contention_counter = allocator->contention_counters[ix];
    if (MG_UNLIKELY (contention_counter))  /* single CPU bias */
    {
        /* adapt contention counter thresholds to chunk sizes */
        contention_counter = contention_counter * 64 / chunk_size;
        threshold = MAX (threshold, contention_counter);
    }
    return threshold;
}

/* --- magazine cache --- */
static inline void magazine_cache_update_stamp (void)
{
    if (allocator->stamp_counter >= MAX_STAMP_COUNTER) {
        struct timeval tv;
        gettimeofday (&tv, NULL);
        /* milli seconds */
        allocator->last_stamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        allocator->stamp_counter = 0;
    }
    else
        allocator->stamp_counter++;
}

static inline ChunkLink* magazine_chain_prepare_fields (
        ChunkLink *magazine_chunks)
{
    ChunkLink *chunk1;
    ChunkLink *chunk2;
    ChunkLink *chunk3;
    ChunkLink *chunk4;
    /* checked upon initialization: mem_assert (MIN_MAGAZINE_SIZE >= 4); */
    /* ensure a magazine with at least 4 unused data pointers */
    chunk1 = magazine_chain_pop_head (&magazine_chunks);
    chunk2 = magazine_chain_pop_head (&magazine_chunks);
    chunk3 = magazine_chain_pop_head (&magazine_chunks);
    chunk4 = magazine_chain_pop_head (&magazine_chunks);
    chunk4->next = magazine_chunks;
    chunk3->next = chunk4;
    chunk2->next = chunk3;
    chunk1->next = chunk2;
    return chunk1;
}

/* access the first 3 fields of a specially prepared magazine chain */
#define magazine_chain_prev(mc)         ((mc)->data)
#define magazine_chain_stamp(mc)        ((mc)->next->data)
#define magazine_chain_uint_stamp(mc)   (uintptr_t)((mc)->next->data)
#define magazine_chain_next(mc)         ((mc)->next->next->data)
#define magazine_chain_count(mc)        ((mc)->next->next->next->data)

static void magazine_cache_trim (Allocator *allocator,
        unsigned int ix, unsigned int stamp)
{
    /* pthread_mutex_lock (allocator->mutex); done by caller */
    /* trim magazine cache from tail */
    ChunkLink *current = magazine_chain_prev (allocator->magazines[ix]);
    ChunkLink *trash = NULL;
    while (ABS (stamp - magazine_chain_uint_stamp (current)) >=
            allocator->config.working_set_msecs)
    {
        /* unlink */
        ChunkLink *prev = magazine_chain_prev (current);
        ChunkLink *next = magazine_chain_next (current);
        magazine_chain_next (prev) = next;
        magazine_chain_prev (next) = prev;
        /* clear special fields, put on trash stack */
        magazine_chain_next (current) = NULL;
        magazine_chain_count (current) = NULL;
        magazine_chain_stamp (current) = NULL;
        magazine_chain_prev (current) = trash;
        trash = current;
        /* fixup list head if required */
        if (current == allocator->magazines[ix])
        {
            allocator->magazines[ix] = NULL;
            break;
        }
        current = prev;
    }
    pthread_mutex_unlock (&allocator->magazine_mutex);
    /* free trash */
    if (trash)
    {
        const size_t chunk_size = SLAB_CHUNK_SIZE (allocator, ix);
        pthread_mutex_lock (&allocator->slab_mutex);
        while (trash)
        {
            current = trash;
            trash = magazine_chain_prev (current);
            magazine_chain_prev (current) = NULL; /* clear special field */
            while (current)
            {
                ChunkLink *chunk = magazine_chain_pop_head (&current);
                slab_allocator_free_chunk (chunk_size, chunk);
            }
        }
        pthread_mutex_unlock (&allocator->slab_mutex);
    }
}

/* count must be >= MIN_MAGAZINE_SIZE */
static void magazine_cache_push_magazine (unsigned int ix,
        ChunkLink *magazine_chunks, size_t count)
{
    ChunkLink *current = magazine_chain_prepare_fields (magazine_chunks);
    ChunkLink *next, *prev;
    pthread_mutex_lock (&allocator->magazine_mutex);
    /* add magazine at head */
    next = allocator->magazines[ix];
    if (next)
        prev = magazine_chain_prev (next);
    else
        next = prev = current;
    magazine_chain_next (prev) = current;
    magazine_chain_prev (next) = current;
    magazine_chain_prev (current) = prev;
    magazine_chain_next (current) = next;
    magazine_chain_count (current) = (void *)count;
    /* stamp magazine */
    magazine_cache_update_stamp();
    magazine_chain_stamp (current) = (void *)(allocator->last_stamp);
    allocator->magazines[ix] = current;
    /* free old magazines beyond a certain threshold */
    magazine_cache_trim (allocator, ix, allocator->last_stamp);
    /* pthread_mutex_unlock (allocator->mutex); was done by magazine_cache_trim() */
}

static ChunkLink* magazine_cache_pop_magazine (unsigned int ix,
        size_t *countp)
{
    g_mutex_lock_a (&allocator->magazine_mutex,
        &allocator->contention_counters[ix]);

    if (!allocator->magazines[ix]) {
        unsigned int magazine_threshold = allocator_get_magazine_threshold (allocator, ix);
        size_t i, chunk_size = SLAB_CHUNK_SIZE (allocator, ix);
        ChunkLink *chunk, *head;
        pthread_mutex_unlock (&allocator->magazine_mutex);
        pthread_mutex_lock (&allocator->slab_mutex);
        head = slab_allocator_alloc_chunk (chunk_size);
        head->data = NULL;
        chunk = head;
        for (i = 1; i < magazine_threshold; i++)
        {
            chunk->next = slab_allocator_alloc_chunk (chunk_size);
            chunk = chunk->next;
            chunk->data = NULL;
        }
        chunk->next = NULL;
        pthread_mutex_unlock (&allocator->slab_mutex);
        *countp = i;
        return head;
    }
    else {
        ChunkLink *current = allocator->magazines[ix];
        ChunkLink *prev = magazine_chain_prev (current);
        ChunkLink *next = magazine_chain_next (current);
        /* unlink */
        magazine_chain_next (prev) = next;
        magazine_chain_prev (next) = prev;
        allocator->magazines[ix] = next == current ? NULL : next;
        pthread_mutex_unlock (&allocator->magazine_mutex);
        /* clear special fields and hand out */
        *countp = (size_t) magazine_chain_count (current);
        magazine_chain_prev (current) = NULL;
        magazine_chain_next (current) = NULL;
        magazine_chain_count (current) = NULL;
        magazine_chain_stamp (current) = NULL;
        return current;
    }
}

/* --- thread magazines --- */
static void private_thread_memory_cleanup (void * data)
{
    ThreadMemory *tmem = data;
    const unsigned int n_magazines = MAX_SLAB_INDEX (allocator);
    unsigned int ix;
    for (ix = 0; ix < n_magazines; ix++)
    {
        Magazine *mags[2];
        unsigned int j;
        mags[0] = &tmem->magazine1[ix];
        mags[1] = &tmem->magazine2[ix];
        for (j = 0; j < 2; j++)
        {
            Magazine *mag = mags[j];
            if (mag->count >= MIN_MAGAZINE_SIZE)
                magazine_cache_push_magazine (ix, mag->chunks, mag->count);
            else
            {
                const size_t chunk_size = SLAB_CHUNK_SIZE (allocator, ix);
                pthread_mutex_lock (&allocator->slab_mutex);
                while (mag->chunks)
                {
                    ChunkLink *chunk = magazine_chain_pop_head (&mag->chunks);
                    slab_allocator_free_chunk (chunk_size, chunk);
                }
                pthread_mutex_unlock (&allocator->slab_mutex);
            }
        }
    }
    free (tmem);
}

static void thread_memory_magazine1_reload (ThreadMemory *tmem,
        unsigned int ix)
{
    Magazine *mag = &tmem->magazine1[ix];
    mem_assert (mag->chunks == NULL); /* ensure that we may reset mag->count */
    mag->count = 0;
    mag->chunks = magazine_cache_pop_magazine (ix, &mag->count);
}

static void thread_memory_magazine2_unload (ThreadMemory *tmem,
        unsigned int ix)
{
    Magazine *mag = &tmem->magazine2[ix];
    magazine_cache_push_magazine (ix, mag->chunks, mag->count);
    mag->chunks = NULL;
    mag->count = 0;
}

static inline void thread_memory_swap_magazines (ThreadMemory *tmem,
        unsigned int ix)
{
    Magazine xmag = tmem->magazine1[ix];
    tmem->magazine1[ix] = tmem->magazine2[ix];
    tmem->magazine2[ix] = xmag;
}

static inline BOOL thread_memory_magazine1_is_empty (ThreadMemory *tmem,
        unsigned int ix)
{
    return tmem->magazine1[ix].chunks == NULL;
}

static inline BOOL thread_memory_magazine2_is_full (ThreadMemory *tmem,
        unsigned int ix)
{
    return tmem->magazine2[ix].count >=
            allocator_get_magazine_threshold (allocator, ix);
}

static inline void * thread_memory_magazine1_alloc (ThreadMemory *tmem,
        unsigned int ix)
{
    Magazine *mag = &tmem->magazine1[ix];
    ChunkLink *chunk = magazine_chain_pop_head (&mag->chunks);
    if (MG_LIKELY (mag->count > 0))
        mag->count--;
    return chunk;
}

static inline void
thread_memory_magazine2_free (ThreadMemory *tmem,
        unsigned int ix, void *mem)
{
    Magazine *mag = &tmem->magazine2[ix];
    ChunkLink *chunk = mem;
    chunk->data = NULL;
    chunk->next = mag->chunks;
    mag->chunks = chunk;
    mag->count++;
}

/* --- API functions --- */
void *mg_slice_alloc (size_t mem_size)
{
    ThreadMemory *tmem;
    size_t chunk_size;
    void * mem;
    unsigned int acat;

    /* This gets the private structure for this thread.  If the private
     * structure does not yet exist, it is created.
     *
     * This has a side effect of causing MGSlice to be initialised, so it
     * must come first.
     */
    tmem = thread_memory_from_self ();

    chunk_size = P2ALIGN (mem_size);
    acat = allocator_categorize (chunk_size);
    if (MG_LIKELY (acat == 1))     /* allocate through magazine layer */
    {
        unsigned int ix = SLAB_INDEX (allocator, chunk_size);
        if (MG_UNLIKELY (thread_memory_magazine1_is_empty (tmem, ix)))
        {
            thread_memory_swap_magazines (tmem, ix);
            if (MG_UNLIKELY (thread_memory_magazine1_is_empty (tmem, ix)))
                thread_memory_magazine1_reload (tmem, ix);
        }
        mem = thread_memory_magazine1_alloc (tmem, ix);
    }
    else if (acat == 2)           /* allocate through slab allocator */
    {
        pthread_mutex_lock (&allocator->slab_mutex);
        mem = slab_allocator_alloc_chunk (chunk_size);
        pthread_mutex_unlock (&allocator->slab_mutex);
    }
    else                          /* delegate to system malloc */
        mem = malloc (mem_size);
    if (MG_UNLIKELY (allocator->config.debug_blocks))
        smc_notify_alloc (mem, mem_size);

    _DBG_PRINTF("%s: slice allocated: %p (%zu)\n",
            __FUNCTION__, (void*)mem, mem_size);

    return mem;
}

void *mg_slice_alloc0 (size_t mem_size)
{
    void *mem = mg_slice_alloc (mem_size);
    if (mem)
        memset (mem, 0, mem_size);
    return mem;
}

void *mg_slice_copy (size_t  mem_size, const void *mem_block)
{
    void *mem = mg_slice_alloc (mem_size);
    if (mem)
        memcpy (mem, mem_block, mem_size);
    return mem;
}

void mg_slice_free (size_t mem_size, void *mem_block)
{
    size_t chunk_size = P2ALIGN (mem_size);
    unsigned int acat = allocator_categorize (chunk_size);
    if (MG_UNLIKELY (!mem_block))
        return;
    if (MG_UNLIKELY (allocator->config.debug_blocks) &&
            !smc_notify_free (mem_block, mem_size))
        abort();
    if (MG_LIKELY (acat == 1))             /* allocate through magazine layer */
    {
        ThreadMemory *tmem = thread_memory_from_self();
        unsigned int ix = SLAB_INDEX (allocator, chunk_size);
        if (MG_UNLIKELY (thread_memory_magazine2_is_full (tmem, ix)))
        {
            thread_memory_swap_magazines (tmem, ix);
            if (MG_UNLIKELY (thread_memory_magazine2_is_full (tmem, ix)))
                thread_memory_magazine2_unload (tmem, ix);
        }
        if (MG_UNLIKELY (_mem_gc_friendly))
            memset (mem_block, 0, chunk_size);
        thread_memory_magazine2_free (tmem, ix, mem_block);
    }
    else if (acat == 2)                   /* allocate through slab allocator */
    {
        if (MG_UNLIKELY (_mem_gc_friendly))
            memset (mem_block, 0, chunk_size);
        pthread_mutex_lock (&allocator->slab_mutex);
        slab_allocator_free_chunk (chunk_size, mem_block);
        pthread_mutex_unlock (&allocator->slab_mutex);
    }
    else                                  /* delegate to system malloc */
    {
        if (MG_UNLIKELY (_mem_gc_friendly))
            memset (mem_block, 0, mem_size);
        free (mem_block);
    }

    _DBG_PRINTF("%s: slice freed: %p (%zu)\n",
            __FUNCTION__, (void*)mem_block, mem_size);
}

void mg_slice_free_chain_with_offset (size_t mem_size,
        void * mem_chain, size_t next_offset)
{
    void * slice = mem_chain;
    /* while the thread magazines and the magazine cache are implemented so that
     * they can easily be extended to allow for free lists containing more free
     * lists for the first level nodes, which would allow O(1) freeing in this
     * function, the benefit of such an extension is questionable, because:
     * - the magazine size counts will become mere lower bounds which confuses
     *   the code adapting to lock contention;
     * - freeing a single node to the thread magazines is very fast, so this
     *   O(list_length) operation is multiplied by a fairly small factor;
     * - memory usage histograms on larger applications seem to indicate that
     *   the amount of released multi node lists is negligible in comparison
     *   to single node releases.
     * - the major performance bottle neck, namely pthread_getspecific() or
     *   pthread_mutex_lock()/pthread_mutex_unlock() has already been moved out of the
     *   inner loop for freeing chained slices.
     */
    size_t chunk_size = P2ALIGN (mem_size);
    unsigned int acat = allocator_categorize (chunk_size);
    if (MG_LIKELY (acat == 1))             /* allocate through magazine layer */
    {
        ThreadMemory *tmem = thread_memory_from_self();
        unsigned int ix = SLAB_INDEX (allocator, chunk_size);
        while (slice)
        {
            Uint8 *current = slice;
            slice = *(void **) (current + next_offset);
            if (MG_UNLIKELY (allocator->config.debug_blocks) &&
                    !smc_notify_free (current, mem_size))
                abort();
            if (MG_UNLIKELY (thread_memory_magazine2_is_full (tmem, ix)))
            {
                thread_memory_swap_magazines (tmem, ix);
                if (MG_UNLIKELY (thread_memory_magazine2_is_full (tmem, ix)))
                    thread_memory_magazine2_unload (tmem, ix);
            }
            if (MG_UNLIKELY (_mem_gc_friendly))
                memset (current, 0, chunk_size);
            thread_memory_magazine2_free (tmem, ix, current);
        }
    }
    else if (acat == 2)                   /* allocate through slab allocator */
    {
        pthread_mutex_lock (&allocator->slab_mutex);
        while (slice)
        {
            Uint8 *current = slice;
            slice = *(void **) (current + next_offset);
            if (MG_UNLIKELY (allocator->config.debug_blocks) &&
                    !smc_notify_free (current, mem_size))
                abort();
            if (MG_UNLIKELY (_mem_gc_friendly))
                memset (current, 0, chunk_size);
            slab_allocator_free_chunk (chunk_size, current);
        }
        pthread_mutex_unlock (&allocator->slab_mutex);
    }
    else                                  /* delegate to system malloc */
        while (slice)
        {
            Uint8 *current = slice;
            slice = *(void **) (current + next_offset);
            if (MG_UNLIKELY (allocator->config.debug_blocks) &&
                    !smc_notify_free (current, mem_size))
                abort();
            if (MG_UNLIKELY (_mem_gc_friendly))
                memset (current, 0, mem_size);
            free (current);
        }
}

/* --- single page allocator --- */
static void allocator_slab_stack_push(Allocator *allocator,
        unsigned int ix, SlabInfo  *sinfo)
{
    /* insert slab at slab ring head */
    if (!allocator->slab_stack[ix])
    {
        sinfo->next = sinfo;
        sinfo->prev = sinfo;
    }
    else
    {
        SlabInfo *next = allocator->slab_stack[ix], *prev = next->prev;
        next->prev = sinfo;
        prev->next = sinfo;
        sinfo->next = next;
        sinfo->prev = prev;
    }
    allocator->slab_stack[ix] = sinfo;
}

static inline unsigned int g_bit_storage (unsigned long number)
{
#if defined(__GNUC__) && (__GNUC__ >= 4) && defined(__OPTIMIZE__)
    return MG_LIKELY (number) ?
        ((sizeof(unsigned long) * 8U - 1) ^ (unsigned int) __builtin_clzl(number)) + 1 : 1;
#else
    unsigned int n_bits = 0;

    do {
        n_bits++;
        number >>= 1;
    } while (number);

    return n_bits;
#endif
}

static size_t allocator_aligned_page_size (Allocator *allocator,
        size_t n_bytes)
{
    size_t val = 1 << g_bit_storage (n_bytes - 1);
    val = MAX (val, allocator->min_page_size);
    return val;
}

static void allocator_add_slab (Allocator *allocator,
        unsigned int ix, size_t chunk_size)
{
    ChunkLink *chunk;
    SlabInfo *sinfo;
    size_t addr, padding, n_chunks, color = 0;
    size_t page_size;
    int errsv;
    void * aligned_memory;
    Uint8 *mem;
    unsigned int i;

    page_size = allocator_aligned_page_size (allocator, SLAB_BPAGE_SIZE (allocator, chunk_size));
    /* allocate 1 page for the chunks and the slab */
    aligned_memory = allocator_memalign (page_size, page_size - NATIVE_MALLOC_PADDING);
    errsv = errno;
    mem = aligned_memory;

    if (!mem)
    {
        const char *syserr = strerror (errsv);
        mem_error ("failed to allocate %u bytes (alignment: %u): %s\n",
                (unsigned int) (page_size - NATIVE_MALLOC_PADDING), (unsigned int) page_size, syserr);
    }
    /* mask page address */
    addr = ((size_t) mem / page_size) * page_size;
    /* assert alignment */
    mem_assert (aligned_memory == (void *) addr);
    /* basic slab info setup */
    sinfo = (SlabInfo*) (mem + page_size - SLAB_INFO_SIZE);
    sinfo->n_allocated = 0;
    sinfo->chunks = NULL;
    /* figure cache colorization */
    n_chunks = ((Uint8*) sinfo - mem) / chunk_size;
    padding = ((Uint8*) sinfo - mem) - n_chunks * chunk_size;
    if (padding)
    {
        color = (allocator->color_accu * P2ALIGNMENT) % padding;
        allocator->color_accu += allocator->config.color_increment;
    }
    /* add chunks to free list */
    chunk = (ChunkLink*) (mem + color);
    sinfo->chunks = chunk;
    for (i = 0; i < n_chunks - 1; i++)
    {
        chunk->next = (ChunkLink*) ((Uint8*) chunk + chunk_size);
        chunk = chunk->next;
    }
    chunk->next = NULL;   /* last chunk */
    /* add slab to slab ring */
    allocator_slab_stack_push (allocator, ix, sinfo);
}

static void *slab_allocator_alloc_chunk(size_t chunk_size)
{
    ChunkLink *chunk;
    unsigned int ix = SLAB_INDEX (allocator, chunk_size);
    /* ensure non-empty slab */
    if (!allocator->slab_stack[ix] || !allocator->slab_stack[ix]->chunks)
        allocator_add_slab (allocator, ix, chunk_size);
    /* allocate chunk */
    chunk = allocator->slab_stack[ix]->chunks;
    allocator->slab_stack[ix]->chunks = chunk->next;
    allocator->slab_stack[ix]->n_allocated++;
    /* rotate empty slabs */
    if (!allocator->slab_stack[ix]->chunks)
        allocator->slab_stack[ix] = allocator->slab_stack[ix]->next;
    return chunk;
}

static void slab_allocator_free_chunk(size_t chunk_size, void *mem)
{
    ChunkLink *chunk;
    BOOL was_empty;
    unsigned int ix = SLAB_INDEX (allocator, chunk_size);
    size_t page_size = allocator_aligned_page_size (allocator, SLAB_BPAGE_SIZE (allocator, chunk_size));
    size_t addr = ((size_t) mem / page_size) * page_size;
    /* mask page address */
    Uint8 *page = (Uint8*) addr;
    SlabInfo *sinfo = (SlabInfo*) (page + page_size - SLAB_INFO_SIZE);
    /* assert valid chunk count */
    mem_assert (sinfo->n_allocated > 0);
    /* add chunk to free list */
    was_empty = sinfo->chunks == NULL;
    chunk = (ChunkLink*) mem;
    chunk->next = sinfo->chunks;
    sinfo->chunks = chunk;
    sinfo->n_allocated--;
    /* keep slab ring partially sorted, empty slabs at end */
    if (was_empty)
    {
        /* unlink slab */
        SlabInfo *next = sinfo->next, *prev = sinfo->prev;
        next->prev = prev;
        prev->next = next;
        if (allocator->slab_stack[ix] == sinfo)
            allocator->slab_stack[ix] = next == sinfo ? NULL : next;
        /* insert slab at head */
        allocator_slab_stack_push (allocator, ix, sinfo);
    }
    /* eagerly free complete unused slabs */
    if (!sinfo->n_allocated)
    {
        /* unlink slab */
        SlabInfo *next = sinfo->next, *prev = sinfo->prev;
        next->prev = prev;
        prev->next = next;
        if (allocator->slab_stack[ix] == sinfo)
            allocator->slab_stack[ix] = next == sinfo ? NULL : next;
        /* free slab */
        allocator_memfree (page_size, page);
    }
}

/* --- memalign implementation --- */
#ifdef HAVE_MALLOC_H
#include <malloc.h>             /* memalign() */
#endif

/* from config.h:
 * define HAVE_POSIX_MEMALIGN           1 // if free(posix_memalign(3)) works, <stdlib.h>
 * define HAVE_MEMALIGN                 1 // if free(memalign(3)) works, <malloc.h>
 * define HAVE_VALLOC                   1 // if free(valloc(3)) works, <stdlib.h> or <malloc.h>
 * if none is provided, we implement malloc(3)-based alloc-only page alignment
 */

#if !(HAVE_POSIX_MEMALIGN || HAVE_MEMALIGN || HAVE_VALLOC)

typedef struct _GTrashStack GTrashStack;
struct _GTrashStack
{
  GTrashStack *next;
};

static GTrashStack *compat_valloc_trash = NULL;

static void g_trash_stack_push (GTrashStack **stack_p,
        void *data_p)
{
    GTrashStack *data = (GTrashStack *) data_p;

    data->next = *stack_p;
    *stack_p = data;
}

void *g_trash_stack_pop (GTrashStack **stack_p)
{
    GTrashStack *data;

    data = *stack_p;
    if (data) {
        *stack_p = data->next;
        /* NULLify private pointer here, most platforms store NULL as
         * subsequent 0 bytes
         */
        data->next = NULL;
    }

    return data;
}
#endif

static void *allocator_memalign (size_t alignment, size_t memsize)
{
    void * aligned_memory = NULL;
    int err = ENOMEM;
#if     HAVE_POSIX_MEMALIGN
    err = posix_memalign (&aligned_memory, alignment, memsize);
#elif   HAVE_MEMALIGN
    errno = 0;
    aligned_memory = memalign (alignment, memsize);
    err = errno;
#elif   HAVE_VALLOC
    errno = 0;
    aligned_memory = valloc (memsize);
    err = errno;
#else
    /* simplistic non-freeing page allocator */
    mem_assert (alignment == sys_page_size);
    mem_assert (memsize <= sys_page_size);
    if (!compat_valloc_trash)
    {
        const unsigned int n_pages = 16;
        Uint8 *mem = malloc (n_pages * sys_page_size);
        err = errno;
        if (mem) {
            int i = n_pages;
            Uint8 *amem = (Uint8*) ALIGN ((size_t) mem, sys_page_size);
            if (amem != mem)
                i--;        /* mem wasn't page aligned */
                while (--i >= 0)
                    g_trash_stack_push (&compat_valloc_trash, amem + i * sys_page_size);
        }
    }

    aligned_memory = g_trash_stack_pop (&compat_valloc_trash);
#endif
    if (!aligned_memory)
        errno = err;
    return aligned_memory;
}

static void allocator_memfree (size_t memsize, void * mem)
{
#if HAVE_POSIX_MEMALIGN || HAVE_MEMALIGN || HAVE_VALLOC
    free (mem);
#else
    mem_assert (memsize <= sys_page_size);
    g_trash_stack_push (&compat_valloc_trash, mem);
#endif
}

static void mem_error (const char *format, ...)
{
    va_list args;
    /* at least, put out "MEMORY-ERROR", in case we segfault during the rest of the function */
    _ERR_PRINTF("\n***SLICED-MEMORY-ERROR***: ");
    va_start (args, format);
    vfprintf (stderr, format, args);
    va_end (args);
    _ERR_PRINTF("\n");
    abort();
    _exit (1);
}

#ifdef _MGDEVEL_MODE
/* --- mg-slice memory checker tree --- */
typedef size_t SmcKType;                /* key type */
typedef size_t SmcVType;                /* value type */
typedef struct {
    SmcKType key;
    SmcVType value;
} SmcEntry;

static void smc_tree_insert (SmcKType key, SmcVType value);
static BOOL smc_tree_lookup (SmcKType key, SmcVType *value_p);
static BOOL smc_tree_remove (SmcKType key);

/* --- mg-slice memory checker implementation --- */
static void smc_notify_alloc (void *pointer, size_t size)
{
    size_t address = (size_t) pointer;
    if (pointer)
        smc_tree_insert (address, size);
}

static int smc_notify_free (void *pointer, size_t size)
{
    size_t address = (size_t) pointer;
    SmcVType real_size;
    BOOL found_one;

    if (!pointer)
        return 1; /* ignore */

    found_one = smc_tree_lookup (address, &real_size);

    if (!found_one) {
        _ERR_PRINTF("MGSlice: MemChecker: attempt to release non-allocated block: %p size=%zu\n",
                pointer, size);
        return 0;
    }

    if (real_size != size && (real_size || size)) {
        _ERR_PRINTF("MGSlice: MemChecker: attempt to release block with invalid size: %p size=%zu invalid-size=%zu\n",
                pointer, real_size, size);
        return 0;
    }

    if (!smc_tree_remove (address)) {
        _ERR_PRINTF("MGSlice: MemChecker: attempt to release non-allocated block: %p size=%zu\n",
                pointer, size);
        return 0;
    }

    return 1; /* all fine */
}

/* --- mg-slice memory checker tree implementation --- */
#define SMC_TRUNK_COUNT     (4093 /* 16381 */)          /* prime, to distribute trunk collisions (big, allocated just once) */
#define SMC_BRANCH_COUNT    (511)                       /* prime, to distribute branch collisions */
#define SMC_TRUNK_EXTENT    (SMC_BRANCH_COUNT * 2039)   /* key address space per trunk, should distribute uniformly across BRANCH_COUNT */
#define SMC_TRUNK_HASH(k)   ((k / SMC_TRUNK_EXTENT) % SMC_TRUNK_COUNT)  /* generate new trunk hash per megabyte (roughly) */
#define SMC_BRANCH_HASH(k)  (k % SMC_BRANCH_COUNT)

/* mutex for MG_SLICE=debug-blocks */
static pthread_mutex_t      smc_tree_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    SmcEntry    *entries;
    unsigned int n_entries;
} SmcBranch;

static SmcBranch     **smc_tree_root = NULL;

static void smc_tree_abort (int errval)
{
    const char *syserr = strerror (errval);
    mem_error ("MemChecker: failure in debugging tree: %s", syserr);
}

static inline SmcEntry* smc_tree_branch_grow_L(SmcBranch *branch, unsigned int index)
{
    unsigned int old_size = branch->n_entries * sizeof (branch->entries[0]);
    unsigned int new_size = old_size + sizeof (branch->entries[0]);
    SmcEntry *entry;
    mem_assert (index <= branch->n_entries);
    branch->entries = (SmcEntry*) realloc (branch->entries, new_size);
    if (!branch->entries)
        smc_tree_abort (errno);
    entry = branch->entries + index;
    memmove (entry + 1, entry, (branch->n_entries - index) * sizeof (entry[0]));
    branch->n_entries += 1;
    return entry;
}

static inline SmcEntry* smc_tree_branch_lookup_nearest_L (SmcBranch *branch, SmcKType key)
{
    unsigned int n_nodes = branch->n_entries, offs = 0;
    SmcEntry *check = branch->entries;
    int cmp = 0;

    while (offs < n_nodes) {
        unsigned int i = (offs + n_nodes) >> 1;
        check = branch->entries + i;
        cmp = key < check->key ? -1 : key != check->key;
        if (cmp == 0)
            return check;                   /* return exact match */
        else if (cmp < 0)
            n_nodes = i;
        else /* (cmp > 0) */
            offs = i + 1;
    }

    /* check points at last mismatch, cmp > 0 indicates greater key */
    return cmp > 0 ? check + 1 : check;   /* return insertion position for inexact match */
}

static void smc_tree_insert (SmcKType key, SmcVType value)
{
    unsigned int ix0, ix1;
    SmcEntry *entry;

    pthread_mutex_lock (&smc_tree_mutex);
    ix0 = SMC_TRUNK_HASH (key);
    ix1 = SMC_BRANCH_HASH (key);
    if (!smc_tree_root) {
        smc_tree_root = calloc (SMC_TRUNK_COUNT, sizeof (smc_tree_root[0]));
        if (!smc_tree_root)
            smc_tree_abort (errno);
    }

    if (!smc_tree_root[ix0]) {
        smc_tree_root[ix0] = calloc (SMC_BRANCH_COUNT, sizeof (smc_tree_root[0][0]));
        if (!smc_tree_root[ix0])
            smc_tree_abort (errno);
    }

    entry = smc_tree_branch_lookup_nearest_L (&smc_tree_root[ix0][ix1], key);
    if (!entry ||                                                                         /* need create */
            entry >= smc_tree_root[ix0][ix1].entries + smc_tree_root[ix0][ix1].n_entries ||   /* need append */
            entry->key != key)                                                                /* need insert */
        entry = smc_tree_branch_grow_L (&smc_tree_root[ix0][ix1], entry - smc_tree_root[ix0][ix1].entries);
    entry->key = key;
    entry->value = value;
    pthread_mutex_unlock (&smc_tree_mutex);
}

static BOOL smc_tree_lookup (SmcKType key, SmcVType *value_p)
{
    SmcEntry *entry = NULL;
    unsigned int ix0 = SMC_TRUNK_HASH (key), ix1 = SMC_BRANCH_HASH (key);
    BOOL found_one = FALSE;
    *value_p = 0;
    pthread_mutex_lock (&smc_tree_mutex);

    if (smc_tree_root && smc_tree_root[ix0]) {
        entry = smc_tree_branch_lookup_nearest_L (&smc_tree_root[ix0][ix1], key);
        if (entry &&
                entry < smc_tree_root[ix0][ix1].entries + smc_tree_root[ix0][ix1].n_entries &&
                entry->key == key) {
            found_one = TRUE;
            *value_p = entry->value;
        }
    }
    pthread_mutex_unlock (&smc_tree_mutex);
    return found_one;
}

static BOOL smc_tree_remove (SmcKType key)
{
    unsigned int ix0 = SMC_TRUNK_HASH (key), ix1 = SMC_BRANCH_HASH (key);
    BOOL found_one = FALSE;

    pthread_mutex_lock (&smc_tree_mutex);
    if (smc_tree_root && smc_tree_root[ix0]) {
        SmcEntry *entry = smc_tree_branch_lookup_nearest_L (&smc_tree_root[ix0][ix1], key);
        if (entry &&
                entry < smc_tree_root[ix0][ix1].entries + smc_tree_root[ix0][ix1].n_entries &&
                entry->key == key) {
            unsigned int i = entry - smc_tree_root[ix0][ix1].entries;
            smc_tree_root[ix0][ix1].n_entries -= 1;
            memmove (entry, entry + 1, (smc_tree_root[ix0][ix1].n_entries - i) * sizeof (entry[0]));
            if (!smc_tree_root[ix0][ix1].n_entries) {
                /* avoid useless pressure on the memory system */
                free (smc_tree_root[ix0][ix1].entries);
                smc_tree_root[ix0][ix1].entries = NULL;
            }
            found_one = TRUE;
        }
    }

    pthread_mutex_unlock (&smc_tree_mutex);
    return found_one;
}

void mg_slice_debug_tree_statistics (void)
{
    pthread_mutex_lock (&smc_tree_mutex);

    if (smc_tree_root) {
        unsigned int i, j, t = 0, o = 0, b = 0, su = 0, ex = 0, en = 4294967295u;
        double tf, bf;
        for (i = 0; i < SMC_TRUNK_COUNT; i++)
            if (smc_tree_root[i])
            {
                t++;
                for (j = 0; j < SMC_BRANCH_COUNT; j++)
                    if (smc_tree_root[i][j].n_entries)
                    {
                        b++;
                        su += smc_tree_root[i][j].n_entries;
                        en = MIN (en, smc_tree_root[i][j].n_entries);
                        ex = MAX (ex, smc_tree_root[i][j].n_entries);
                    }
                    else if (smc_tree_root[i][j].entries)
                        o++; /* formerly used, now empty */
            }
        en = b ? en : 0;
        tf = MAX (t, 1.0); /* max(1) to be a valid divisor */
        bf = MAX (b, 1.0); /* max(1) to be a valid divisor */
        _MG_PRINTF("MGSlice: MemChecker: %u trunks, %u branches, %u old branches\n", t, b, o);
        _MG_PRINTF("MGSlice: MemChecker: %f branches per trunk, %.2f%% utilization\n",
                b / tf,
                100.0 - (SMC_BRANCH_COUNT - b / tf) / (0.01 * SMC_BRANCH_COUNT));
        _MG_PRINTF("MGSlice: MemChecker: %f entries per branch, %u minimum, %u maximum\n",
                su / bf, en, ex);
    }
    else
        _MG_PRINTF("MGSlice: MemChecker: root=NULL\n");

    pthread_mutex_unlock (&smc_tree_mutex);

    /* sample statistics (beast + MGSLice + 24h scripted core & GUI activity):
     *  PID %CPU %MEM   VSZ  RSS      COMMAND
     * 8887 30.3 45.8 456068 414856   beast-0.7.1 empty.bse
     * $ cat /proc/8887/statm # total-program-size resident-set-size shared-pages text/code data/stack library dirty-pages
     * 114017 103714 2354 344 0 108676 0
     * $ cat /proc/8887/status
     * Name:   beast-0.7.1
     * VmSize:   456068 kB
     * VmLck:         0 kB
     * VmRSS:    414856 kB
     * VmData:   434620 kB
     * VmStk:        84 kB
     * VmExe:      1376 kB
     * VmLib:     13036 kB
     * VmPTE:       456 kB
     * Threads:        3
     * (gdb) print mg_slice_debug_tree_statistics ()
     * MGSlice: MemChecker: 422 trunks, 213068 branches, 0 old branches
     * MGSlice: MemChecker: 504.900474 branches per trunk, 98.81% utilization
     * MGSlice: MemChecker: 4.965039 entries per branch, 1 minimum, 37 maximum
     */
}

#endif /* _MGDEVEL_MODE */

#endif /* not defined _MGSLICE_FALLBACK */
