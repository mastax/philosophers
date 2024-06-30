/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   special_case.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-bah <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/29 15:14:26 by elel-bah          #+#    #+#             */
/*   Updated: 2024/06/29 15:14:32 by elel-bah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static	int	create_philosopher_threads(t_dining_info *dining_info, pthread_t *threads)
{
	int	i;

	i = 0;
	while (i < dining_info->num_philosophers)
	{
		if (pthread_create(&threads[i], NULL, philosopher_thread_start,
				&(dining_info->philosophers[i])))
			return (1);
		usleep(100);
		i++;
	}
	return (0);
}

static void	join_all_threads(t_dining_info *dining_info, pthread_t *threads, pthread_t monitor)
{
	int	i;

	i = 0;
	while (i < dining_info->num_philosophers)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	pthread_join(monitor, NULL);
}

void	handle_special_case(t_dining_info *dining_info)
{
	pthread_t	*threads;
	pthread_t	monitor;

	threads = malloc(sizeof(pthread_t) * dining_info->num_philosophers);
	if (!threads)
		return ;
	dining_info->time_to_die += 50;
	if (create_philosopher_threads(dining_info, threads))
	{
		free(threads);
		return ;
	}
	if (pthread_create(&monitor, NULL, monitor_philosophers, dining_info))
	{
		free(threads);
		return ;
	}
	join_all_threads(dining_info, threads, monitor);
	free(threads);
}
