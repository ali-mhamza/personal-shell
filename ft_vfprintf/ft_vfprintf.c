/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_vfprintf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 18:52:35 by marvin            #+#    #+#             */
/*   Updated: 2025/12/12 18:52:35 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_vfprintf.h"

static int	reg_flag(t_sbuf *buf, char c)
{
	char	*str;

	str = "%csdiuxXp";
	if (ft_strchr(str, c) == NULL)
		append_buf(buf, "%", 1);
	return (1);
}

static const char	*format_char(t_sbuf *buf, const char *s, va_list *ap)
{
	unsigned long long	ptr;

	if (*s == '%')
		append_buf(buf, "%", 1);
	else if (*s == 'c')
		append_char(buf, va_arg(*ap, int));
	else if (*s == 's')
		append_str(buf, va_arg(*ap, char *));
	else if ((*s == 'd') || (*s == 'i'))
		append_nbr(buf, va_arg(*ap, int));
	else if (*s == 'u')
		append_unbr(buf, va_arg(*ap, unsigned int));
	else if ((*s == 'x') || (*s == 'X'))
		append_hex(buf, va_arg(*ap, unsigned int), *s);
	else if (*s == 'p')
	{
		ptr = va_arg(*ap, unsigned long long);
		if ((void *) ptr == NULL)
			append_buf(buf, "(nil)", 5);
		else
			append_hex(buf, ptr, 'p');
	}
	return (s + reg_flag(buf, *s));
}

int	ft_vfprintf(int fd, const char *format, va_list *ap)
{
	t_sbuf	buf;

	init_buf(&buf);
	while (*format != '\0')
	{
		if (*format == '%')
		{
			format++;
			format = format_char(&buf, format, ap);
			continue ;
		}
		else
			append_buf(&buf, format, 1);
		format++;
	}
	WRITE(fd, buf.chars, buf.count);
	return ((int) free_buf(&buf));
}
