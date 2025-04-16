#include <unistd.h>
#include <stdint.h>
#include "CMA_internal.h"
#include <unistd.h>
#include <stdint.h>

static void write_str(const char *str)
{
    size_t len = 0;
    while (str[len])
        len++;
    write(1, str, len);
	return ;
}

static void write_hex_address(void *ptr)
{
    uintptr_t address = (uintptr_t)ptr;
    char buf[18];
    int i = 17;
    
    buf[0] = '0';
    buf[1] = 'x';
    do
	{
        int digit = address & 0xF;
        buf[i--] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        address >>= 4;
    }
	while (address != 0 && i > 1);
    write(1, buf, 2);
    write(1, buf + i + 1, 17 - i);
	return ;
}
static void print_decimal(size_t num)
{
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
	if (num == 0)
        buffer[--pos] = '0';
	else
	{
        while (num > 0)
		{
            buffer[--pos] = '0' + (num % 10);
            num /= 10;
        }
    }
    write(1, buffer + pos, 20 - pos);
	return ;
}

void show_alloc_mem(void)
{
    struct Page *page = page_list;
    size_t total = 0;
    
    while (page)
    {
        if (page->alloc_size_type == 0)
            write_str("TINY : ");
        else if (page->alloc_size_type == 1)
            write_str("SMALL : ");
        else
            write_str("LARGE : ");
        write_hex_address(page->start);
        write_str("\n");
        struct Block *block = page->blocks;
        while (block)
        {
            if (!block->free)
            {
                void *data_start = (void *)((char *)block + sizeof(struct Block));
                void *data_end = (void *)((char *)data_start + block->size);
                write_hex_address(data_start);
                write_str(" - ");
                write_hex_address(data_end);
                write_str(" : ");
                print_decimal(block->size);
                write_str(" bytes\n");
                total += block->size;
            }
            block = block->next;
        }
        page = page->next;
    }
    write_str("Total : ");
    print_decimal(total);
    write_str(" bytes\n");
	return ;
}
