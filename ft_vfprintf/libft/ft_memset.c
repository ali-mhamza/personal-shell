/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alhamza <alhamza@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:11:42 by alhamza           #+#    #+#             */
/*   Updated: 2025/11/15 19:42:43 by alhamza          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*temp;
	unsigned char	c_us;

	i = 0;
	temp = (unsigned char *)s;
	c_us = (unsigned char)c;
	while (i < n)
	{
		temp[i] = c_us;
		i++;
	}
	return (s);
}
