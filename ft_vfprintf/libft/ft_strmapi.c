/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alhamza <alhamza@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:14:09 by alhamza           #+#    #+#             */
/*   Updated: 2025/11/15 21:05:04 by alhamza          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	size_t			size;
	char			*new_str;
	unsigned int	i;

	if (!s)
		return (ft_strdup(""));
	else if (!f)
		return (ft_strdup(s));
	size = ft_strlen(s);
	new_str = malloc(size + 1);
	if (!new_str)
		return (NULL);
	i = 0;
	while ((size_t) i < size)
	{
		new_str[i] = (*f)(i, s[i]);
		i++;
	}
	new_str[size] = '\0';
	return (new_str);
}
