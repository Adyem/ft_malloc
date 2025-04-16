ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

# Library names and symbolic link
LIB_NAME = libft_malloc_$(HOSTTYPE).so
SYMLINK  = libft_malloc.so

# Compiler and flags (added -fPIC for shared library support)
CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -std=c99 -fPIC

# Source files for the project
SRC     = cma_free.c cma_malloc.c cma_realloc.c cma_utils.c write_blocks.c
OBJ     = $(SRC:.c=.o)
DEP     = $(OBJ:.o=.d)

# Test executable name
TARGET  = cma

# Extra debug sources (empty for now; add files as needed)
DEBUG_SRC =
DEBUG_OBJ = $(DEBUG_SRC:.c=.o)

# Default target: builds the executable, the shared library, and the symlink.
all: $(LIB_NAME) $(SYMLINK)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Build the shared library using the same object files.
$(LIB_NAME): $(OBJ)
	$(CC) $(CFLAGS) -shared -o $(LIB_NAME) $(OBJ)

# Create or update the symbolic link.
$(SYMLINK): $(LIB_NAME)
	ln -sf $(LIB_NAME) $(SYMLINK)

# Debug target: adds -DDEBUG flag and links additional (currently empty) debug objects.
debug: CFLAGS += -DDEBUG
debug: $(TARGET)_debug

$(TARGET)_debug: $(OBJ) $(DEBUG_OBJ)
	$(CC) $(CFLAGS) -o $(TARGET)_debug $(OBJ) $(DEBUG_OBJ)

# Rule to compile .c files into .o files and generate dependency files.
%.o: %.c CMA.h CMA_internal.h
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Include dependency files if available.
-include $(DEP)

# 'clean' only removes object and dependency files.
clean:
	rm -f $(OBJ) $(DEP)

# 'fclean' removes everything including executables, library, and symlink.
fclean: clean
	rm -f $(TARGET) $(TARGET)_debug $(LIB_NAME) $(SYMLINK)

# 're' target cleans everything and rebuilds.
re: fclean all

.PHONY: all clean fclean debug re
