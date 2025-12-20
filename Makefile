CC = gcc
CFLAGS = -g -Wall -Wextra -Werror
INCLUDE = -I/usr/local/include
LIBS = -lreadline -lhistory
NAME = shell

SRC_DIR = src
SRCS = $(SRC_DIR)/*.c

all:
	@$(CC) $(CFLAGS) $(INCLUDE) $(SRCS) $(LIBS) -o $(NAME)

fclean:
	@rm -rf $(NAME)