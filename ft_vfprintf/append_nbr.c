/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   append_nbr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 20:13:16 by marvin            #+#    #+#             */
/*   Updated: 2025/12/12 20:13:16 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_vfprintf.h"

void	append_nbr(t_sbuf *buf, int n)
{
	char	*num_str;

	num_str = ft_itoa(n);
	if (!num_str)
		return ;
	append_buf(buf, num_str, ft_strlen(num_str));
	free(num_str);
}
