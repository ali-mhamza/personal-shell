/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alhamza <alhamza@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:08:44 by alhamza           #+#    #+#             */
/*   Updated: 2025/11/15 18:55:14 by alhamza          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

static int	whitespace(char c)
{
	if (c == ' ' || c == '\n' || c == '\t')
		return (1);
	else if (c == '\v' || c == '\f' || c == '\r')
		return (1);
	return (0);
}

int	ft_atoi(const char *str)
{
	int			i;
	int			negatives;
	long int	output;

	i = 0;
	negatives = 1;
	while (whitespace(str[i]))
		i++;
	if (str[i] == '-')
	{
		negatives = -1;
		i++;
	}
	else if (str[i] == '+')
		i++;
	output = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		output = (output * 10) + (int)(str[i] - '0');
		i++;
	}
	return ((int)(negatives * output));
}
