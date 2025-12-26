/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   append_unbr.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 20:14:21 by marvin            #+#    #+#             */
/*   Updated: 2025/12/12 20:14:21 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_vfprintf.h"

static int	digit_count(unsigned int n)
{
	int				size;

	if (n == 0)
		return (1);
	size = 0;
	while (n > 0)
	{
		n = n / 10;
		size++;
	}
	return (size);
}

static char	*fill_string(char *str, unsigned int n, int digits)
{
	str[digits] = '\0';
	while (digits > 0)
	{
		str[digits - 1] = '0' + (n % 10);
		n = n / 10;
		digits--;
	}
	return (str);
}

static char	*uitoa(unsigned int n)
{
	int			digits;
	char		*str;

	digits = digit_count(n);
	str = malloc(digits + 1);
	if (!str)
		return (NULL);
	return (fill_string(str, n, digits));
}

void	append_unbr(t_sbuf *buf, unsigned int n)
{
	char	*new_str;

	new_str = uitoa(n);
	if (!new_str)
		return ;
	append_buf(buf, new_str, ft_strlen(new_str));
	free(new_str);
}
