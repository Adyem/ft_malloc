###############################################################################
#                                ft_malloc                                    #
###############################################################################

# ─────────────────────────────  HOSTTYPE  ────────────────────────────────────
ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

# ─────────────────────────────  NAMES  ───────────────────────────────────────
NAME := libft_malloc_$(HOSTTYPE).so      # real shared object
LINK := libft_malloc.so                  # mandatory symlink

# ─────────────────────────────  TOOLCHAIN  ───────────────────────────────────
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -std=c99 -fPIC -D_DEFAULT_SOURCE

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)                 # macOS
LDFLAGS := -shared -Wl,-install_name,$(NAME)
else                                     # Linux / BSD
LDFLAGS := -shared -Wl,-soname,$(NAME)
endif

# ─────────────────────────────  FILE LISTS  ──────────────────────────────────
SRC     := cma_free.c cma_malloc.c cma_realloc.c cma_utils.c write_blocks.c
OBJ_DIR := .obj
OBJ     := $(addprefix $(OBJ_DIR)/,$(SRC:.c=.o))
DEP     := $(OBJ:.o=.d)

# ─────────────────────────────  DEFAULT TARGET  ──────────────────────────────
all: $(LINK)

# ─────────────────────────────  BUILD RULES  ─────────────────────────────────
# Compile each .c ⇒ .o (dependency files auto‑generated with -MMD)
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Link the shared object
$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

# Create / update the symlink
$(LINK): $(NAME)
	ln -sf $(NAME) $(LINK)

# ─────────────────────────────  HOUSEKEEPING  ───────────────────────────────
clean:
	$(RM) -rf $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME) $(LINK)

re: fclean all

.PHONY: all clean fclean re
-include $(DEP)
