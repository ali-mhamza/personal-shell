/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   append_str.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 20:13:56 by marvin            #+#    #+#             */
/*   Updated: 2025/12/12 20:13:56 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_vfprintf.h"

void	append_str(t_sbuf *buf, const char *s)
{
	if (s != NULL)
		append_buf(buf, s, ft_strlen(s));
	else
		append_buf(buf, "(null)", 6);
}
