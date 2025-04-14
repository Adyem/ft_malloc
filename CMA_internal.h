#ifndef CMA_INTERNAL_HPP
# define CMA_INTERNAL_HPP

#include <stddef.h>
#include <stdint.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/mman.h>

#define PAGE_SIZE 131072
#define BYPASS_ALLOC DEBUG
#define MAGIC_NUMBER 0xDEADBEEF

#define OFFSWITCH 0

#define SIZE 100
#define SMALL_SIZE (SIZE)
#define MEDIUM_SIZE (SIZE * 10)

#define BASE_SIZE 100 * (SIZE + sizeof(struct Block))
#define SMALL_ALLOC (BASE_SIZE * 1)
#define MEDIUM_ALLOC (BASE_SIZE * 10)

#ifndef MAP_ANONYMOUS
  #ifdef MAP_ANON
    #define MAP_ANONYMOUS MAP_ANON
  #elif defined(__linux__)
    /* On Linux, MAP_ANONYMOUS is normally 0x20 */
    #define MAP_ANONYMOUS 0x20
  #else
    #error "Neither MAP_ANONYMOUS nor MAP_ANON is defined on this system. Define one of them manually."
  #endif
#endif

#ifndef DEBUG
# define DEBUG 0
#endif

#if __has_include(<valgrind/memcheck.h>)
#include <valgrind/memcheck.h>
#define PROTECT_METADATA(ptr, size) VALGRIND_MAKE_MEM_NOACCESS(ptr, size)
#define UNPROTECT_METADATA(ptr, size) VALGRIND_MAKE_MEM_DEFINED(ptr, size)
#else
#define PROTECT_METADATA(ptr, size) ((void)0)
#define UNPROTECT_METADATA(ptr, size) ((void)0)
#endif

struct Block
{
    uint32_t		magic;
	size_t			size;
    bool			free;
    struct Block	*next;
    struct Block	*prev;
} __attribute__ ((aligned(16)));

struct Page
{
    void			*start;
	size_t			size;
    struct Page		*next;
    struct Page		*prev;
    struct Block	*blocks;
	bool			heap;
	int8_t			alloc_size_type;	
} __attribute__ ((aligned(16)));

extern struct Page *page_list;

struct Block	*split_block(struct Block *block, size_t size);
struct Page		*create_page(size_t size);
struct Block	*find_free_block(size_t size);
struct Block	*merge_block(struct Block *block);
bool 			is_valid_block(void *ptr);

inline __attribute__((always_inline, hot)) size_t align8(size_t size)
{
    return (size + 15) & ~15;
}

#endif
