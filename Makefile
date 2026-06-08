CC = gcc
CFLAGS = -O2 -Wall -Wextra -Wno-unused-result -Werror
GETLINE_DIR = get_line
INCLUDE = -I/usr/local/include -I$(GETLINE_DIR)
GETLINE_LIB = -lgnl
RL_LIBS = -lreadline -lhistory $(GNL_LIB)
NAME = shell

SRC_DIR = src
SRCS = $(SRC_DIR)/*.c

all:
	@make -C $(GETLINE_DIR) all clean --no-print-directory
	@$(CC) $(CFLAGS) $(INCLUDE) $(SRCS) -L$(GETLINE_DIR) $(GETLINE_LIB) \
	$(RL_LIBS) -o $(NAME)

fclean:
	@make -C $(GETLINE_DIR) fclean --no-print-directory
	@rm -rf $(NAME)