CC = gcc
CFLAGS = -O2 -Wall -Wextra -Wno-unused-result -Werror
GNL_DIR = get_next_line
VFPRINTF_DIR = ft_vfprintf
INCLUDE = -I/usr/local/include -I$(GNL_DIR)
GNL_LIB = -lgnl
VFPRINTF_LIB = -lftvfprintf
RL_LIBS = -lreadline -lhistory $(GNL_LIB) $(VFPRINTF_LIB)
NAME = minishell

SRC_DIR = src
SRCS = $(SRC_DIR)/*.c

all:
	@make -C $(GNL_DIR) all clean --no-print-directory
	@make -C $(VFPRINTF_DIR) all clean --no-print-directory
	@$(CC) $(CFLAGS) $(INCLUDE) $(SRCS) -L$(GNL_DIR) $(GNL_LIB) \
	-L$(VFPRINTF_DIR) $(VFPRINTF_LIB) $(RL_LIBS) -o $(NAME)

fclean:
	@make -C $(GNL_DIR) fclean --no-print-directory
	@make -C $(VFPRINTF_DIR) fclean --no-print-directory
	@rm -rf $(NAME)