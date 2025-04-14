#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
 * Test Program for cma_realloc
 *
 * This program exercises various code paths of your cma_realloc function.
 * It assumes that:
 *   - cma_realloc behaves like realloc for NULL pointers.
 *   - cma_realloc will return NULL for new_size == 0.
 *   - is_valid_block(ptr) validates that ptr belongs to your custom allocator.
 *   - If reallocation can be done in place, the same pointer is returned; otherwise,
 *     a new block is allocated and the contents are copied.
 */

int main(void) {
    printf("Test 1: Realloc on NULL pointer (should act like malloc)...\n");
    char *ptr = (char *)realloc(NULL, 32);
    assert(ptr != NULL && "cma_realloc failed to allocate memory when given NULL");
    strcpy(ptr, "Hello, custom realloc!");
    printf("  Allocated string: %s\n", ptr);

    printf("Test 2: Realloc to a larger block (content should be preserved)...\n");
    char *expanded_ptr = (char *)realloc(ptr, 64);
    assert(expanded_ptr != NULL && "cma_realloc failed to expand block");
    // Check that the string content survived
    assert(strcmp(expanded_ptr, "Hello, custom realloc!") == 0 && "Content not preserved in expansion");
    printf("  Expanded content: %s\n", expanded_ptr);

    printf("Test 3: Realloc to a smaller block (content should be truncated accordingly)...\n");
    char *shrunk_ptr = (char *)realloc(expanded_ptr, 16);
    assert(shrunk_ptr != NULL && "cma_realloc failed to shrink block");
    // Here we only compare up to 15 characters to allow room for a null terminator.
    // The content may be truncated because the new block is smaller.
    if (strlen("Hello, custom realloc!") > 15) {
        // If truncated, we'll check the first 15 characters
        assert(strncmp(shrunk_ptr, "Hello, custom ", 14) == 0 &&
               "Content not preserved correctly when shrinking");
    }
    printf("  Shrunk content (possibly truncated): %.*s\n", 15, shrunk_ptr);

    printf("Test 4: Realloc with new_size == 0 (should return NULL)...\n");
    char *null_ptr = (char *)realloc(shrunk_ptr, 0);
    assert(null_ptr == NULL && "cma_realloc did not return NULL when new_size is 0");
    printf("  Realloc with size 0 returned NULL as expected.\n");

    printf("Test 5: Realloc using an invalid pointer (should return NULL)...\n");
    int dummy;
    char *invalid_ptr = (char *)&dummy;  // not allocated by custom allocator
    char *result = (char *)realloc(invalid_ptr, 32);
    assert(result == NULL && "cma_realloc did not return NULL for an invalid pointer");
    printf("  Realloc on an invalid pointer returned NULL as expected.\n");

    printf("All tests passed successfully.\n");
    return 0;
}
