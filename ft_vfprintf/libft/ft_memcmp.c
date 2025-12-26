/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alhamza <alhamza@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:11:03 by alhamza           #+#    #+#             */
/*   Updated: 2025/11/15 19:39:55 by alhamza          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char	*s1_temp;
	const unsigned char	*s2_temp;
	size_t				i;

	if (n == 0)
		return (0);
	s1_temp = (const unsigned char *)s1;
	s2_temp = (const unsigned char *)s2;
	i = 0;
	while ((i < n) && (s1_temp[i] == s2_temp[i]))
		i++;
	if (i == n)
		return (0);
	return (s1_temp[i] - s2_temp[i]);
}
