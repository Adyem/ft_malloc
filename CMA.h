#ifndef CMA_HPP
# define CMA_HPP

#include <stddef.h>

void	*malloc(size_t size) __attribute__ ((warn_unused_result, hot));
void	free(void* ptr) __attribute__ ((hot));
void	*realloc(void* ptr, size_t new_size) __attribute__ ((warn_unused_result, hot));
void	show_alloc_mem(void);

#endif
