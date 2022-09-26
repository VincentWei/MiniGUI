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
 *   Copyright (C) 2002~2020, Beijing FMSoft Technologies Co., Ltd.
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
** blockheap.c: The heap of block data.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/01/10
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "constants.h"
#include "misc.h"

BOOL InitBlockDataHeap (PBLOCKHEAP heap, size_t sz_block, size_t sz_heap)
{
#ifdef _MGHAVE_VIRTUAL_WINDOW
    pthread_mutex_init (&heap->lock, NULL);
#endif

    heap->sz_usage_bmp = (sz_heap + 7) >> 3;
    heap->sz_block = ROUND_TO_MULTIPLE (sz_block, SIZEOF_PTR);
    heap->sz_heap = heap->sz_usage_bmp << 3;
    heap->nr_alloc = 0;

    if (heap->sz_heap == 0 || heap->sz_block == 0)
        goto failed;

    if ((heap->heap = calloc (heap->sz_heap, heap->sz_block)) == NULL)
        goto failed;

    if ((heap->usage_bmp = calloc (heap->sz_usage_bmp, sizeof (char))) == NULL) {
        free (heap->heap);
        goto failed;
    }

    memset (heap->usage_bmp, 0xFF, heap->sz_usage_bmp);
    return TRUE;

failed:
    heap->heap = NULL;
    heap->usage_bmp = NULL;
    return FALSE;
}

void* BlockDataAlloc (PBLOCKHEAP heap)
{
    int free_slot;
    unsigned char* block_data = NULL;

#ifdef _MGHAVE_VIRTUAL_WINDOW
    pthread_mutex_lock (&heap->lock);
#endif

    free_slot = __mg_lookfor_unused_slot (heap->usage_bmp, heap->sz_usage_bmp, 1);
    if (free_slot >= 0) {
        block_data = heap->heap + heap->sz_block * free_slot;
        _DBG_PRINTF ("Allocated one block in the block heap: %p (%d)\n",
                heap, free_slot);
        goto ret;
    }

    if ((block_data = calloc (1, heap->sz_block)) == NULL)
        goto ret;

    heap->nr_alloc++;

ret:

#ifdef _MGHAVE_VIRTUAL_WINDOW
    pthread_mutex_unlock (&heap->lock);
#endif
    return block_data;
}

void BlockDataFree (PBLOCKHEAP heap, void* data)
{
    unsigned char* block_data = data;

#ifdef _MGHAVE_VIRTUAL_WINDOW
    pthread_mutex_lock (&heap->lock);
#endif

    if (block_data < heap->heap ||
            block_data > (heap->heap + heap->sz_block * heap->sz_heap)) {
        free (block_data);
        heap->nr_alloc--;
    }
    else {
        int slot;
        slot = (block_data - heap->heap) / heap->sz_block;
        __mg_slot_clear_use (heap->usage_bmp, slot);

        _DBG_PRINTF ("Freed one block in the block heap: %p (%d)\n",
                heap, slot);
    }

#ifdef _MGHAVE_VIRTUAL_WINDOW
    pthread_mutex_unlock (&heap->lock);
#endif
}

void DestroyBlockDataHeap (PBLOCKHEAP heap)
{
    int nr_free_slots;

    if (heap->nr_alloc > 0) {
        _WRN_PRINTF ("There are still not freed extra blocks in the block heap: %p (%zu)\n",
                heap, heap->nr_alloc);
    }

    nr_free_slots = __mg_get_nr_idle_slots (heap->usage_bmp, heap->sz_usage_bmp);
    if (nr_free_slots != heap->sz_heap) {
        _WRN_PRINTF ("There are still not freed blocks in the block heap: %p (%zu)\n",
                heap, heap->sz_heap - nr_free_slots);

#ifdef _DEBUG
        {
            unsigned char* bitmap = heap->usage_bmp;
            int slot = 0;
            int i, j;

            for (i = 0; i < heap->sz_usage_bmp; i++) {
                for (j = 0; j < 8; j++) {
                    if (!(*bitmap & (0x80 >> j))) {
                        _WRN_PRINTF ("Still used slot in the block heap: %p (%d)\n",
                                heap, slot);
                    }
                    slot++;
                }
                bitmap++;
            }
        }

        assert (0);
#endif
    }

    free (heap->heap);
    heap->heap = NULL;

    free (heap->usage_bmp);
    heap->usage_bmp = NULL;

#ifdef _MGHAVE_VIRTUAL_WINDOW
    pthread_mutex_destroy (&heap->lock);
#endif
}

