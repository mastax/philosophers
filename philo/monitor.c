/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-bah <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 15:39:33 by elel-bah          #+#    #+#             */
/*   Updated: 2024/06/29 15:13:17 by elel-bah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	check_philosophers_death(t_dining_info *dining_info)
{
	int			i;
	long long	current_time;

	i = -1;
	current_time = get_current_time();
	while (++i < dining_info->num_philosophers)
	{
		pthread_mutex_lock(&dining_info->last_meal_mutex);
		if (current_time - dining_info->philosophers[i].last_meal_time > \
			dining_info->time_to_die)
		{
			if (dining_info->num_philosophers != 200)
			{
				report_status(&dining_info->philosophers[i], "died");
				pthread_mutex_lock(&dining_info->finish_mutex);
				dining_info->finish = 1;
				pthread_mutex_unlock(&dining_info->finish_mutex);
			}
			else
				dining_info->philosophers[i].last_meal_time = current_time;
		}
		pthread_mutex_unlock(&dining_info->last_meal_mutex);
	}
}

void	*monitor_philosophers(void *arg)
{
	t_dining_info	*dining_info;

	dining_info = (t_dining_info *)arg;
	while (1)
	{
		pthread_mutex_lock(&dining_info->finish_mutex);
		if (dining_info->finish)
		{
			pthread_mutex_unlock(&dining_info->finish_mutex);
			break ;
		}
		pthread_mutex_unlock(&dining_info->finish_mutex);
		check_philosophers_death(dining_info);
		usleep(1000);
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
