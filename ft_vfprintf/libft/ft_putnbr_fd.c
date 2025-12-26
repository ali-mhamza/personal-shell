/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alhamza <alhamza@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:12:12 by alhamza           #+#    #+#             */
/*   Updated: 2025/11/15 19:43:56 by alhamza          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr_fd(int n, int fd)
{
	long int	m;

	m = n;
	if (m < 0)
	{
		ft_putchar_fd('-', fd);
		m *= -1;
	}
	if (m == 0)
		ft_putchar_fd('0', fd);
	if (m > 9)
		ft_putnbr_fd(m / 10, fd);
	if (m != 0)
		ft_putchar_fd('0' + m % 10, fd);
}
