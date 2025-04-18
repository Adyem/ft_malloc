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

/* ─────────── core limits (one per class) ─────────── */
#define SMALL_PAYLOAD_LIMIT   1024                 /* ≤ 1 KiB  → “small”  */
#define MEDIUM_PAYLOAD_LIMIT  (SMALL_PAYLOAD_LIMIT * 16)  /* ≤ 16 KiB → “medium” */

/* ─────────── helper sizes (payload + header) ─────── */
#define SMALL_BLOCK_BYTES  (SMALL_PAYLOAD_LIMIT  + sizeof(struct Block))
#define MEDIUM_BLOCK_BYTES (MEDIUM_PAYLOAD_LIMIT + sizeof(struct Block))

/* ─────────── public names your code already uses ─── */
#define SMALL_SIZE    (SMALL_PAYLOAD_LIMIT)
#define MEDIUM_SIZE   (MEDIUM_PAYLOAD_LIMIT)

/* one zone must hold ≥ 100 blocks of its own class */
#define BASE_SIZE     (100 * SMALL_BLOCK_BYTES)
#define SMALL_ALLOC   (BASE_SIZE)
#define MEDIUM_ALLOC  (100 * MEDIUM_BLOCK_BYTES)

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

_Static_assert(sizeof(struct Block) % 16 == 0,
               "Block header must be 16‑byte multiple");
_Static_assert(sizeof(struct Page) % 16 == 0,
               "Block header must be 16‑byte multiple");

extern struct Page *page_list;

struct Block	*split_block(struct Block *block, size_t size);
struct Page		*create_page(size_t size);
struct Block	*find_free_block(size_t size);
struct Block	*merge_block(struct Block *block);
bool 			is_valid_block(void *ptr);

inline __attribute__((always_inline, hot)) size_t align16(size_t size)
{
    return (size + 15) & ~15;
}

#endif
