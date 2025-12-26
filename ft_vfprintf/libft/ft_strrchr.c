/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alhamza <alhamza@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:14:35 by alhamza           #+#    #+#             */
/*   Updated: 2025/11/15 21:36:40 by alhamza          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *str, int c)
{
	char	*temp;

	temp = (char *)str + ft_strlen(str);
	if ((unsigned char) c == '\0')
		return (temp);
	while (temp >= str)
	{
		if (*temp == (unsigned char) c)
			return (temp);
		temp--;
	}
	return (NULL);
}
