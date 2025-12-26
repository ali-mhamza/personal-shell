/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sbuf.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 20:15:23 by marvin            #+#    #+#             */
/*   Updated: 2025/12/12 20:15:23 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_vfprintf.h"

void	init_buf(t_sbuf *buf)
{
	buf->chars = NULL;
	buf->count = 0;
	buf->capacity = 0;
}

static void	realloc_buf(t_sbuf *buf, size_t new_size)
{
	char	*new_chars;

	new_chars = ft_calloc(new_size, sizeof(char));
	if (!new_chars)
		return ;
	if (buf->chars)
	{
		ft_memcpy(new_chars, buf->chars, buf->count);
		free(buf->chars);
	}
	buf->chars = new_chars;
	buf->capacity = new_size;
}

void	append_buf(t_sbuf *buf, const char *str, size_t size)
{
	size_t	i;

	if ((str == NULL) || (size == 0))
		return ;
	while (buf->capacity < buf->count + size + 1)
	{
		if (buf->capacity == 0)
			realloc_buf(buf, 8);
		else
			realloc_buf(buf, buf->capacity * 2);
	}
	i = 0;
	while (i < size)
	{
		buf->chars[buf->count + i] = str[i];
		i++;
	}
	buf->count += size;
}

size_t	free_buf(t_sbuf *buf)
{
	int	count;

	free(buf->chars);
	count = buf->count;
	init_buf(buf);
	return (count);
}
