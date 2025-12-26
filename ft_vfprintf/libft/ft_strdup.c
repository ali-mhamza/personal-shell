/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alhamza <alhamza@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:12:58 by alhamza           #+#    #+#             */
/*   Updated: 2025/11/15 20:01:04 by alhamza          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s)
{
	char	*new;
	size_t	s_size;

	s_size = ft_strlen(s);
	new = malloc(s_size + 1);
	if (!new)
		return (NULL);
	ft_strlcpy(new, s, s_size + 1);
	return (new);
}
