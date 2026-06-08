CC = gcc
CFLAGS = -O2 -Wall -Wextra -Wno-unused-result -Werror

GETLINE_DIR = get_line
READLINE_DIR = /usr/local/include

INCLUDE = -I$(READLINE_DIR) -I$(GETLINE_DIR)
GETLINE_LIB = -lgnl
READLINE_LIBS = -lreadline -lhistory

NAME = shell

SRC_DIR = src
SRCS = $(SRC_DIR)/*.c

all:
	@make -C $(GETLINE_DIR) all clean --no-print-directory
	@$(CC) $(CFLAGS) $(INCLUDE) $(SRCS) -L$(GETLINE_DIR) $(GETLINE_LIB) \
	$(READLINE_LIBS) -o $(NAME)

fclean:
	@make -C $(GETLINE_DIR) fclean --no-print-directory
	@rm -rf $(NAME)