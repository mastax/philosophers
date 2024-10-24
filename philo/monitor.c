/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-bah <elel-bah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 15:39:33 by elel-bah          #+#    #+#             */
/*   Updated: 2024/10/24 16:18:17 by elel-bah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	check_all_ate_enough(t_dining_info *dining_info)
{
	int	i;

	if (dining_info->num_must_eat == -1)
		return (0);
	pthread_mutex_lock(&dining_info->meal_mutex);
	i = 0;
	while (i < dining_info->num_philosophers)
	{
		if (dining_info->philosophers[i].num_of_meals < \
			dining_info->num_must_eat)
		{
			pthread_mutex_unlock(&dining_info->meal_mutex);
			return (0);
		}
		i++;
	}
	pthread_mutex_unlock(&dining_info->meal_mutex);
	return (1);
}

// void    custom_sleep(t_philosopher *philosopher, long long ms)
// {
//     long long    start;

//     start = get_current_time();
//     while (!check_if_simulation_ended(philosopher))
//     {
//         if (get_current_time() - start >= ms)
//             break;
//         usleep(100);
//     }
// }

void	custom_sleep(t_philosopher *philosopher, long long ms)
{
	long long	start;

	start = get_current_time();
	while (1)
	{
		pthread_mutex_lock(&philosopher->dining_info->finish_mutex);
		if (philosopher->dining_info->finish || get_current_time()
			- start >= ms)
		{
			pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
			break ;
		}
		pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
		usleep(100);
	}
}
