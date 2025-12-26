/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alhamza <alhamza@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:14:30 by alhamza           #+#    #+#             */
/*   Updated: 2025/11/15 21:39:04 by alhamza          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *s1, const char *s2, size_t n)
{
	size_t	i;
	size_t	j;
	size_t	find_size;

	if ((*s2 == '\0') || (s1 == s2))
		return ((char *)(s1));
	if (n == 0)
		return (NULL);
	find_size = ft_strlen(s2);
	i = 0;
	while ((s1[i] != '\0') && (i < n))
	{
		j = 0;
		while ((j < find_size) && (s2[j] == s1[i + j])
			&& ((i + j) < n) && s1[i + j] != '\0')
			j++;
		if (j == find_size)
			return ((char *)(s1 + i));
		i++;
	}
	return (NULL);
}
