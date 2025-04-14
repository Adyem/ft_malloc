#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <sys/mman.h>
#include "CMA_internal.h"

struct Page *page_list = NULL;

static size_t determine_page_size(size_t size)
{
    if (size < SMALL_SIZE)
        return (SMALL_ALLOC);
    else if (size < MEDIUM_SIZE)
        return (MEDIUM_ALLOC);
    return (size);
}

static void determine_page_use(struct Page *page)
{
    if (page->heap == false)
        page->alloc_size_type = 0;
    else if (page->size == SMALL_ALLOC)
        page->alloc_size_type = 0;
    else if (page->size == MEDIUM_ALLOC)
        page->alloc_size_type = 1;
    else
        page->alloc_size_type = 2;
    return ;
}

static int8_t determine_which_block_to_use(size_t size)
{
    if (size < SMALL_SIZE)
        return (0);
    else if (size < MEDIUM_SIZE)
        return (1);
    return (2);
}

static void *create_stack_block(void)
{
    static char memory_block[PAGE_SIZE];
    return (memory_block);
}

struct Block* split_block(struct Block* block, size_t size)
{
    if (block->size <= size + sizeof(struct Block))
        return (block);
    struct Block* new_block = (struct Block*)((char*)block + sizeof(struct Block) + size);
    new_block->magic = MAGIC_NUMBER;
    new_block->size = block->size - size - sizeof(struct Block);
    new_block->free = true;
    new_block->next = block->next;
    new_block->prev = block;
    if (new_block->next)
        new_block->next->prev = new_block;
    block->next = new_block;
    block->size = size;
    return (block);
}

struct Page *create_page(size_t size)
{
    size_t page_size = determine_page_size(size);
    bool use_heap = true;

    if (page_list == NULL)
    {
        page_size = PAGE_SIZE;
        use_heap = false;
    }
    else
    {
        if (size + sizeof(struct Block) > determine_page_size(size))
            page_size = size + sizeof(struct Block);
    }
    void* ptr = NULL;
    if (use_heap)
    {
        ptr = mmap(NULL, page_size, PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if (ptr == MAP_FAILED)
            return (NULL);
    }
    else
    {
        ptr = create_stack_block();
        if (!ptr)
            return (NULL);
    }
    struct Page* page = NULL;
    if (use_heap)
    {
        page = (struct Page*) mmap(NULL, sizeof(struct Page), PROT_READ | PROT_WRITE,
                            MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if (page == MAP_FAILED)
        {
            munmap(ptr, page_size);
            return (NULL);
        }
    }
    else
    {
        static struct Page stack_page;
        page = &stack_page;
    }
    page->heap = use_heap;
    page->start = ptr;
    page->size = page_size;
    page->next = NULL;
    page->prev = NULL;
    page->blocks = (struct Block*) ptr;
    page->blocks->magic = MAGIC_NUMBER;
    page->blocks->size = page_size - sizeof(struct Block);
    page->blocks->free = true;
    page->blocks->next = NULL;
    page->blocks->prev = NULL;
    determine_page_use(page);
    if (!page_list) {
        page_list = page;
    }
    else
    {
        page->next = page_list;
        page_list->prev = page;
        page_list = page;
    }
    return (page);
}

struct Block *find_free_block(size_t size)
{
    struct Page* cur_page = page_list;
    int8_t alloc_size_type = determine_which_block_to_use(size);
    while (cur_page)
    {
        if (cur_page->alloc_size_type != alloc_size_type)
        {
            cur_page = cur_page->next;
            continue ;
        }
        struct Block* cur_block = cur_page->blocks;
        while (cur_block)
        {
            if (cur_block->free && cur_block->size >= size)
                return (cur_block);
            cur_block = cur_block->next;
        }
        cur_page = cur_page->next;
    }
    return (NULL);
}

struct Block *merge_block(struct Block *block)
{
    if (block->next && block->next->free)
    {
        block->size += sizeof(struct Block) + block->next->size;
        block->next = block->next->next;
        if (block->next)
            block->next->prev = block;
    }
    if (block->prev && block->prev->free)
    {
        block->prev->size += sizeof(struct Block) + block->size;
        block->prev->next = block->next;
        if (block->next)
            block->next->prev = block->prev;
        block = block->prev;
    }
    return (block);
}

bool is_valid_block(void *ptr)
{
    if (!ptr)
		return (false);
    struct Page *page = page_list;
    while (page != NULL) {
        if ((char*)ptr >= (char*)page->start &&
            (char*)ptr < ((char*)page->start + page->size))
		{
            struct Block *block = page->blocks;
            while (block != NULL)
			{
                void *user_ptr = (void*)((char*)block + sizeof(struct Block));
                if (ptr == user_ptr)
                    return (true);
                block = block->next;
            }
        }
        page = page->next;
    }
    return (false);
}
