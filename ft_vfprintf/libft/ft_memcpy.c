/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alhamza <alhamza@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:11:15 by alhamza           #+#    #+#             */
/*   Updated: 2025/11/15 19:41:34 by alhamza          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char		*dst_tmp;
	const unsigned char	*src_temp;
	size_t				i;

	if (!dst && !src)
		return (NULL);
	if (n == 0 || dst == src)
		return (dst);
	dst_tmp = (unsigned char *)dst;
	src_temp = (const unsigned char *)src;
	i = 0;
	while (i < n)
	{
		dst_tmp[i] = src_temp[i];
		i++;
	}
	return (dst);
}
