#define dbg() printf("%s %d\n", __FUNCTION__, __LINE__)
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "videomem-bucket.h"


/*#define GAL_VMBUCKET_DEBUG*/

#define TEST_BIT(a, bit) ((a) & (bit))
#define SET_BIT(a, bit) ((a) |= (bit))
#define UNSET_BIT(a, bit) ((a) &= ~(bit))
#define BLOCK_SIZE(block) ((block)->height * (block)->pitch)


#ifdef GAL_VMBUCKET_DEBUG
static void gal_vmbucket_print(const gal_vmbucket_t *bucket) {
    struct list_head *i;
    int unused_total = 0;
    int unused_max = 0;
    int used_max = 0;
    int used_total = 0;
    char bit_flags[32] = "";

    printf( "\n---- GAL Video Memory Bucket -------\n"
            "  start=%p\n"
            "  size=%d\n",
            bucket->start, bucket->size);
    list_for_each(i, &bucket->block_head) {
        gal_vmblock_t *block = (gal_vmblock_t *)i;

        if (TEST_BIT(block->flag, VMBLOCK_FLAG_USED)) {
            used_total++;
            used_max = used_max > (block->height * block->pitch) ? used_max : (block->height * block->pitch); 
            sprintf(bit_flags, " ");
        }
        else {
            unused_total++;
            unused_max = unused_max > (block->height * block->pitch) ? unused_max : (block->height * block->pitch); 
            sprintf(bit_flags, "\t\t");
        }

        printf( "  block:\n"
                "    %soffset=%d\n" 
                "    %sheight=%d\n"
                "    %spitch=%d\n"
                "    %sflag=%d\n"
                "    %ssize=%d\n",
                bit_flags, block->offset, 
                bit_flags, block->height, 
                bit_flags, block->pitch, 
                bit_flags, block->flag,
                bit_flags, block->height*block->pitch);  
    }

    printf( "---------- Total Result -------------\n"
            "unused: count=%d, max_block:%d\n"
            "used  : count=%d, max_block:%d\n"
            "total : count=%d\n"
            "-------------------------------------\n\n",
            unused_total, unused_max,
            used_total, used_max,
            unused_total+used_total);
}
#else
#   define gal_vmbucket_print(x) /* NULL */
#endif

int gal_vmbucket_init(gal_vmbucket_t *bucket, unsigned char *start, int size) {
    bucket->start = start;
    bucket->size = size;
    INIT_LIST_HEAD(&bucket->block_head);

    {
        gal_vmblock_t *block;
        block = (gal_vmblock_t *)calloc(1, sizeof(*block));
        block->offset = 0;
        block->height = 1;
        block->pitch = size;
        list_add(&block->list, &bucket->block_head);
    }
    gal_vmbucket_print(bucket);
    return 0;
}

void gal_vmbucket_destroy(gal_vmbucket_t *bucket) {
    while (! list_empty(&bucket->block_head)) {
        gal_vmblock_t *block = (gal_vmblock_t *)bucket->block_head.prev;
        list_del(bucket->block_head.prev);
        free(block);
    }
}

gal_vmblock_t *gal_vmbucket_alloc(gal_vmbucket_t *bucket, int pitch, int height) {
    struct list_head *i;
    int required_size = height * pitch;

    list_for_each(i, &bucket->block_head) {
        gal_vmblock_t *block;
        block = (gal_vmblock_t *)i;
        if (! TEST_BIT(block->flag, VMBLOCK_FLAG_USED)) {
            int block_size = BLOCK_SIZE(block);
            if (required_size > block_size) {
                continue;
            }else if (required_size < block_size) {
                gal_vmblock_t *new_block;
                new_block = (gal_vmblock_t *)calloc(1, sizeof(*new_block));
                new_block->offset = block->offset + required_size;
                new_block->pitch = block_size - required_size;
                new_block->height = 1;
                list_add(&new_block->list, &block->list);
            }
            block->height = height;
            block->pitch = pitch;
            SET_BIT(block->flag, VMBLOCK_FLAG_USED);
            gal_vmbucket_print(bucket);
            return block;
        }
    }
    fprintf(stderr, "videomem-bucket: Failed to alloc hardware surface(request_size=%d)\n", required_size);

    return NULL;
}

void gal_vmbucket_free(gal_vmbucket_t *bucket, gal_vmblock_t *block)
{
    int size = BLOCK_SIZE(block);
    gal_vmblock_t *near_block;

    //assert(TEST_BIT(block->flag, VMBLOCK_FLAG_USED));
    if (!TEST_BIT(block->flag, VMBLOCK_FLAG_USED)) {
        fprintf(stderr, "videomem-bucket: can't free an unuse block!\n");
        return;
    }
    UNSET_BIT(block->flag, VMBLOCK_FLAG_USED);

    /* look for prev */
    if (block->list.prev != &(bucket->block_head)) {
        near_block = (gal_vmblock_t *)block->list.prev;
        if (!TEST_BIT(near_block->flag, VMBLOCK_FLAG_USED)) {
            size += BLOCK_SIZE(near_block);
            block->offset = near_block->offset;
            block->height = 1;
            block->pitch = size;
            list_del(&near_block->list);
            free(near_block);
        }
    }

    /* look for next */
    if (block->list.next != &(bucket->block_head)) {
        near_block = (gal_vmblock_t *)block->list.next;
        if (!TEST_BIT(near_block->flag, VMBLOCK_FLAG_USED)) {
            size += BLOCK_SIZE(near_block);
            block->height = 1;
            block->pitch = size;
            list_del(&near_block->list);
            free(near_block);
        }
    }

    gal_vmbucket_print(bucket);
}
