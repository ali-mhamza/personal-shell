/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alhamza <alhamza@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/15 15:39:37 by alhamza           #+#    #+#             */
/*   Updated: 2025/11/15 19:05:29 by alhamza          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void*))
{
	t_list	*temp;
	t_list	*current;

	if (!lst)
		return ;
	temp = *lst;
	while (temp != NULL)
	{
		current = temp;
		temp = temp->next;
		ft_lstdelone(current, del);
	}
	*lst = NULL;
}
