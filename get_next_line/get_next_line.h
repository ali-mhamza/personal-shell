/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 00:06:36 by marvin            #+#    #+#             */
/*   Updated: 2025/12/16 00:06:36 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stdlib.h>
# if defined(_WIN32) || defined(_WIN64)
#  include <io.h>
#  define READ _read
# else
#  include <unistd.h>
#  define READ read
# endif

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 128
# elif BUFFER_SIZE == 0
#  error "BUFFER_SIZE must be greater than 0."
// # elif BUFFER_SIZE > 8000
// #  error "BUFFER_SIZE too large."
# endif

typedef struct s_sbuf
{
	char	*chars;
	size_t	count;
	size_t	capacity;
}	t_sbuf;

char	*get_next_line(int fd);

#ifndef FREE_CHARS
# define FREE_CHARS		1
#endif

#ifndef NO_FREE_CHARS
# define NO_FREE_CHARS	0
#endif

// Buffer functions.

t_sbuf	*gnl_init_buf(void);
void	gnl_append_buf(t_sbuf *buf, const char *str, size_t size);
char	*gnl_free_buf(t_sbuf **buf, int free_chars);

#endif