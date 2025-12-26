/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   append_hex.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 20:12:36 by marvin            #+#    #+#             */
/*   Updated: 2025/12/12 20:12:36 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_vfprintf.h"

static int	digit_count(unsigned long long n)
{
	int					size;

	if (n == 0)
		return (1);
	size = 0;
	while (n > 0)
	{
		n = n / 16;
		size++;
	}
	return (size);
}

static char	*fill_string(char *str, unsigned long long n, int digits, char opt)
{
	char	*hex;

	if ((opt == 'x') || (opt == 'p'))
		hex = "0123456789abcdef";
	else
		hex = "0123456789ABCDEF";
	str[digits] = '\0';
	while (digits > 0)
	{
		str[digits - 1] = hex[n % 16];
		n = n / 16;
		digits--;
	}
	return (str);
}

static char	*num_hex(unsigned long long n, char opt)
{
	int			digits;
	char		*str;

	digits = digit_count(n);
	str = malloc(digits + 1);
	if (!str)
		return (NULL);
	return (fill_string(str, n, digits, opt));
}

void	append_hex(t_sbuf *buf, unsigned long long n, char opt)
{
	char	*new_str;

	new_str = num_hex(n, opt);
	if (!new_str)
		return ;
	if (opt == 'p')
		append_buf(buf, "0x", 2);
	append_buf(buf, new_str, ft_strlen(new_str));
	free(new_str);
}
