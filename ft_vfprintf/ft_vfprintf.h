/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_vfprintf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 18:48:04 by marvin            #+#    #+#             */
/*   Updated: 2025/12/12 18:48:04 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ft_vfprintf_H
# define ft_vfprintf_H

# include "libft.h"

# include <stdarg.h>
# include <stddef.h>
# include <stdlib.h>

# if defined(_WIN32) || defined(_WIN64)
#  include <io.h>
#  define WRITE _write
# else
#  include <unistd.h>
#  define WRITE write
# endif

typedef struct sbuf
{
	char	*chars;
	size_t	count;
	size_t	capacity;
}	t_sbuf;

void		init_buf(t_sbuf	*buf);
void		append_buf(t_sbuf *buf, const char *str, size_t size);
size_t		free_buf(t_sbuf *buf);

void		append_char(t_sbuf *buf, int c);
void		append_str(t_sbuf *buf, const char *s);
void		append_nbr(t_sbuf *buf, int n);
void		append_unbr(t_sbuf *buf, unsigned int n);
void		append_hex(t_sbuf *buf, unsigned long long n, char opt);
int			ft_vfprintf(int fd, const char *format, va_list *ap);

#endif