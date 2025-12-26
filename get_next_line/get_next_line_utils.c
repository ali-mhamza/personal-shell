/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 00:05:37 by marvin            #+#    #+#             */
/*   Updated: 2025/12/16 00:05:37 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

t_sbuf	*gnl_init_buf(void)
{
	t_sbuf	*buf;

	if (BUFFER_SIZE == 0)
		return (NULL);
	buf = malloc(sizeof(t_sbuf));
	if (buf == NULL)
		return (NULL);
	buf->chars = NULL;
	buf->capacity = 0;
	buf->count = 0;
	return (buf);
}

static void	realloc_buf(t_sbuf *buf, size_t new_size)
{
	char	*new_chars;
	size_t	i;

	new_chars = malloc(new_size * sizeof(char));
	if (!new_chars)
		return ;
	i = 0;
	while (i < new_size)
		new_chars[i++] = 0;
	if (buf->chars)
	{
		i = 0;
		while (i < buf->count)
		{
			new_chars[i] = buf->chars[i];
			i++;
		}
		free(buf->chars);
	}
	buf->chars = new_chars;
	buf->capacity = new_size;
}

void	gnl_append_buf(t_sbuf *buf, const char *str, size_t size)
{
	size_t	i;

	if ((str == NULL) || (size <= 0))
		return ;
	while (buf->capacity < buf->count + size + 1)
	{
		if (buf->capacity == 0)
			realloc_buf(buf, BUFFER_SIZE);
		else
			realloc_buf(buf, buf->capacity * 2);
	}
	i = 0;
	while ((str[i] != '\0') && (i < size))
	{
		buf->chars[buf->count + i] = str[i];
		i++;
	}
	buf->count += i;
}

char	*gnl_free_buf(t_sbuf **buf, int free_chars)
{
	char	*temp;

	if (!buf)
		return (NULL);
	temp = NULL;
	if (*buf)
	{
		if (free_chars == FREE_CHARS)
			free((*buf)->chars);
		else
			temp = (*buf)->chars;
	}
	free(*buf);
	*buf = NULL;
	return (temp);
}
