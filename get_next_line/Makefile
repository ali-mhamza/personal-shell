CC = cc
CFLAGS = -g -Wall -Wextra -Werror
AR = ar rcs
NAME = libgnl.a

SRCS = get_next_line.c get_next_line_utils.c
OBJS = $(SRCS:.c=.o)

all: $(OBJS)
	@$(AR) $(NAME) $(OBJS)

bonus: $(BONUS_OBJS)
	@$(AR) $(NAME) $(BONUS_OBJS)

%.o: %.c
	@$(CC) $(CFLAGS) $(SIZE) -c $< -o $@

clean:
	@rm -f *.o

fclean: clean
	@rm -f $(NAME)

re: fclean all
