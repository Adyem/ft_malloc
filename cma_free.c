#include <assert.h>
#include <signal.h>
#include <sys/mman.h>
#include <pthread.h>
#include "CMA_internal.h"

void free(void* ptr)
{
	if (!ptr)
		return ;
	if (!is_valid_block(ptr))
		return ;
    struct Block* block = (struct Block*)((char*)ptr - sizeof(struct Block));
    if (block->magic != MAGIC_NUMBER)
        raise(SIGABRT);
    block->free = 1;
    merge_block(block);
	return ;
}
