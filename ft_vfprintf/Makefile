CC = cc
CFLAGS = -g -Wall -Werror -Wextra
LIBFT_DIR = libft
LIBFT_LIB = libft.a
INCLUDE = -I$(LIBFT_DIR)
AR = ar

NAME = libftvfprintf.a

SRCS =	ft_vfprintf.c \
		append_char.c \
		append_hex.c \
		append_nbr.c \
		append_str.c \
		append_unbr.c \
		sbuf.c
BONUS_SRCS = ft_printf_bonus.c \
			append_char_bonus.c \
			append_hex_bonus.c \
			append_nbr_bonus.c \
			append_str_bonus.c \
			append_unbr_bonus.c \
			sbuf_bonus.c \
			format_flag_bonus.c \
			padding_bonus.c
OBJS = $(SRCS:.c=.o)
BONUS_OBJS = $(BONUS_SRCS:.c=.o)

all: $(NAME)

$(NAME) : $(OBJS) $(LIBFT_DIR)/$(LIBFT_LIB)
	@$(AR) rcs $(NAME) $(OBJS) $(LIBFT_DIR)/*.o

bonus: $(BONUS_OBJS) $(LIBFT_DIR)/$(LIBFT_LIB)
	@$(AR) rcs $(NAME) $(BONUS_OBJS) $(LIBFT_DIR)/*.o

$(LIBFT_DIR)/$(LIBFT_LIB):
	@make -C $(LIBFT_DIR) --no-print-directory

%.o: %.c
	@$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	@rm -f *.o
	@make -C $(LIBFT_DIR) --no-print-directory clean

fclean: clean
	@rm -f $(NAME)
	@make -C $(LIBFT_DIR) --no-print-directory fclean

re: fclean all

.PHONY: all clean fclean re