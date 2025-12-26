/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alhamza <alhamza@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:10:53 by alhamza           #+#    #+#             */
/*   Updated: 2025/11/15 19:31:33 by alhamza          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char	*temp;
	unsigned char		c_us;
	size_t				i;

	temp = (const unsigned char *)s;
	c_us = (unsigned char)c;
	i = 0;
	while (i < n)
	{
		if (temp[i] == c_us)
			return ((void *)(temp + i));
		i++;
	}
	return (NULL);
}
