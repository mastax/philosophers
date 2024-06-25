/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-bah <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 15:39:33 by elel-bah          #+#    #+#             */
/*   Updated: 2024/06/25 15:39:35 by elel-bah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	check_philosopher_status(t_dining_info *dining_info, int i)
{
	long long	current_time;

	current_time = get_current_time();
	pthread_mutex_lock(&dining_info->status_mutex);
	if (current_time - dining_info->philosophers[i].last_meal_time >= \
		dining_info->time_to_die)
	{
		report_status(&dining_info->philosophers[i], "died");
		pthread_mutex_lock(&dining_info->finish_mutex);
		dining_info->finish = 1;
		pthread_mutex_unlock(&dining_info->finish_mutex);
		pthread_mutex_unlock(&dining_info->status_mutex);
		return (1);
	}
	pthread_mutex_unlock(&dining_info->status_mutex);
	return (0);
}

void	*monitor_philosophers(void *arg)
{
	t_dining_info	*dining_info;
	long long		last_check;
	int				i;

	dining_info = (t_dining_info *)arg;
	last_check = get_current_time();
	while (!dining_info->finish)
	{
		i = 0;
		while (i < dining_info->num_philosophers)
		{
			if (check_philosopher_status(dining_info, i) == 1)
				break ;
			i++;
		}
		if (get_current_time() - last_check >= 1000)
		{
			last_check = get_current_time();
			usleep(1000);
		}
	}
	return (NULL);
}

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
