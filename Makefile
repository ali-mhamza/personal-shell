CC = gcc
CFLAGS = -g -Wall -Wextra -Werror
GNL_DIR = get_next_line
INCLUDE = -I/usr/local/include -I$(GNL_DIR)
GNL_LIB = -lgnl
RL_LIBS = -lreadline -lhistory -lgnl
NAME = shell

SRC_DIR = src
SRCS = $(SRC_DIR)/*.c

all:
	@make -C $(GNL_DIR) --no-print-directory
	@$(CC) $(CFLAGS) $(INCLUDE) $(SRCS) -L$(GNL_DIR) $(GNL_LIB) $(RL_LIBS) -o $(NAME)

fclean:
	@make -C $(GNL_DIR) fclean --no-print-directory
	@rm -rf $(NAME)