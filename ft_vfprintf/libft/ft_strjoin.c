/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alhamza <alhamza@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:13:17 by alhamza           #+#    #+#             */
/*   Updated: 2025/11/15 20:04:09 by alhamza          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	s1_size;
	size_t	s2_size;
	char	*new_str;

	s1_size = ft_strlen(s1);
	s2_size = ft_strlen(s2);
	new_str = ft_calloc(s1_size + s2_size + 1, sizeof(char));
	if (!new_str)
		return (NULL);
	ft_strlcat(new_str, s1, s1_size + 1);
	ft_strlcat(new_str + s1_size, s2, s2_size + 1);
	return (new_str);
}
