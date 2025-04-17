#include <signal.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <sys/mman.h>
#include <pthread.h>
#include "CMA_internal.h"

static int reallocate_block(void *ptr, size_t new_size)
{
    if (!ptr)
        return (-1);
    struct Block* block = (struct Block*)((char*)ptr - sizeof(struct Block));
    if (block->size >= new_size)
    {
        split_block(block, new_size);
        return (0);
    }
    if (block->next && block->next->free &&
        (block->size + sizeof(struct Block) + block->next->size) >= new_size)
    {
        block->size += sizeof(struct Block) + block->next->size;
        block->next = block->next->next;
        if (block->next)
            block->next->prev = block;
        split_block(block, new_size);
        return (0);
    }
    return (-1);
}

void *realloc(void* ptr, size_t new_size)
{
	if (!ptr)
        return (malloc(new_size));
	if (new_size == 0)
        return (NULL);
	if (!is_valid_block(ptr))
		return (NULL);
	new_size = align16(new_size);
	struct Block* old_block = (struct Block*)((char*)ptr - sizeof(struct Block));
	if (old_block->magic != MAGIC_NUMBER)
        raise(SIGABRT);
	if (reallocate_block(ptr, new_size) == 0)
        return (ptr);
    void *new_ptr = malloc(new_size);
    if (!new_ptr)
        return (NULL);
    size_t copy_size = old_block->size < new_size ? old_block->size : new_size;
    memcpy(new_ptr, ptr, copy_size);
    return (new_ptr);
}
