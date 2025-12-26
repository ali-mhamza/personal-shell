/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alhamza <alhamza@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:12:47 by alhamza           #+#    #+#             */
/*   Updated: 2025/11/15 19:58:47 by alhamza          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *str, int c)
{
	size_t	i;
	size_t	size;

	i = 0;
	size = ft_strlen(str);
	while (i < size)
	{
		if (str[i] == (char) c)
			return ((char *)(str + i));
		i++;
	}
	if (str[size] == (char) c)
		return ((char *)(str + size));
	return (NULL);
}
