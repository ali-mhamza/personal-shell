/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alhamza <alhamza@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:11:26 by alhamza           #+#    #+#             */
/*   Updated: 2025/11/15 19:41:52 by alhamza          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t n)
{
	unsigned char		*dptr;
	const unsigned char	*sptr;

	if (src == dst)
		return (dst);
	dptr = (unsigned char *)dst;
	sptr = (unsigned const char *)src;
	if ((sptr < dptr) && (sptr + n > dptr))
	{
		while (n-- > 0)
			dptr[n] = sptr[n];
	}
	else
	{
		while (n-- > 0)
		{
			*dptr = *sptr;
			sptr++;
			dptr++;
		}
	}
	return (dst);
}
