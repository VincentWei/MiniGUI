#ifndef _VIDEOMEM_BUCKET_H
#define _VIDEOMEM_BUCKET_H

#include "list.h"

typedef struct _memblock {
    struct list_head list;
    int offset;
    int height; /* XXX: There is no need to know width at all */
    int pitch;
#define VMBLOCK_FLAG_USED 0x01
    int flag;
} gal_vmblock_t;

typedef struct _membucket {
    struct list_head block_head;
    unsigned char *start;
    int size;
} gal_vmbucket_t;

int gal_vmbucket_init(gal_vmbucket_t *bucket, unsigned char *start, int size);
void gal_vmbucket_destroy(gal_vmbucket_t *bucket);
gal_vmblock_t *gal_vmbucket_alloc(gal_vmbucket_t *bucket, int pitch, int height);
void gal_vmbucket_free(gal_vmbucket_t *bucket, gal_vmblock_t *block);

#endif
