/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alhamza <alhamza@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:14:55 by alhamza           #+#    #+#             */
/*   Updated: 2025/11/15 20:27:29 by alhamza          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	acc_size;
	char	*src;
	char	*new_str;

	if (!s)
		return (NULL);
	if (ft_strlen(s) < start)
		return (ft_strdup(""));
	src = (char *) s + start;
	if (ft_strlen(src) < len)
		acc_size = ft_strlen(src) + 1;
	else
		acc_size = len + 1;
	new_str = malloc(acc_size);
	if (!new_str)
		return (NULL);
	ft_strlcpy(new_str, src, acc_size);
	return (new_str);
}
