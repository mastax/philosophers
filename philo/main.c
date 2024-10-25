/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-bah <elel-bah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 15:39:59 by elel-bah          #+#    #+#             */
/*   Updated: 2024/10/25 14:32:43 by elel-bah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	main(int ac, char **av)
{
	t_dining_info	dining_info;

	if (initialize_info(&dining_info, ac, av))
		return (0);
	if (initialize_philosophers(&dining_info))
		return (free_info(&dining_info));
	if (initialize_mutex(&dining_info))
		return (free_and_destroy(&dining_info));
	if (create_philosophers(&dining_info))
		return (free_and_destroy(&dining_info), 0);
	destroy_resources(&dining_info);
}
