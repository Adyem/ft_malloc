#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include "CMA_internal.h"

void *malloc(size_t size)
{
	if (size <= 0)
        return (NULL);
    size_t aligned_size = align8(size);
    struct Block *block = find_free_block(aligned_size);
    if (!block)
    {
        struct Page* page = create_page(aligned_size);
        if (!page)
            return (NULL);
        block = page->blocks;
    }
    block = split_block(block, aligned_size);
    block->free = false;
    return ((char*)block + sizeof(struct Block));
}
