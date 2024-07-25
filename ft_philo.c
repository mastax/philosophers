/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_philo.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-bah <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 15:40:38 by elel-bah          #+#    #+#             */
/*   Updated: 2024/06/29 15:12:20 by elel-bah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	check_death(t_philosopher *philosopher)
{
	pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
	if ((get_current_time() - philosopher->last_meal_time) >= \
		philosopher->dining_info->time_to_die)
	{
		report_status(philosopher, "died");
		check_completion(philosopher, 1);
		pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
		return (1);
	}
	else if (philosopher->dining_info->num_must_eat > \
			0 && philosopher->num_of_meals >= \
			philosopher->dining_info->num_must_eat)
	{
		philosopher->dining_info->num_full_philosophers++;
		if (philosopher->dining_info->num_full_philosophers >= \
			philosopher->dining_info->num_philosophers)
		{
			check_completion(philosopher, 1);
			report_status(philosopher, "f");
			pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
			return (1);
		}
	}
	pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
	return (0);
}

void	eat(t_philosopher *philosopher)
{
	int	left_fork;
	int	right_fork;

	left_fork = philosopher->left_fork;
	right_fork = philosopher->right_fork;
	pthread_mutex_lock(&philosopher->dining_info->forks[left_fork]);
	report_status(philosopher, "has taken a fork");
	pthread_mutex_lock(&philosopher->dining_info->forks[right_fork]);
	report_status(philosopher, "has taken a fork");
	// pthread_mutex_lock(&philosopher->dining_info->status_mutex); //already in the function of report_status
	// philosopher->last_meal_time = get_current_time();
	// pthread_mutex_unlock(&philosopher->dining_info->status_mutex);
	report_status(philosopher, "is eating");
	custom_sleep(philosopher, philosopher->dining_info->time_to_eat);
	pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
	philosopher->num_of_meals++;
	pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
	pthread_mutex_unlock(&philosopher->dining_info->forks[right_fork]);
	pthread_mutex_unlock(&philosopher->dining_info->forks[left_fork]);
}

int	check_completion(t_philosopher *philosopher, int yes)
{
	pthread_mutex_lock(&philosopher->dining_info->finish_mutex);
	if (yes)
	{
		philosopher->dining_info->finish = 1;
		pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
		return (1);
	}
	if (philosopher->dining_info->finish)
	{
		pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
		return (1);
	}
	pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
	return (0);
}

void	*philosopher_thread_start(void *arg)
{
	t_philosopher	*philosopher;

	philosopher = (t_philosopher *)arg;
	if (philosopher->identifier % 2 == 0)
		usleep(1000);
	while (1)
	{
		pthread_mutex_lock(&philosopher->dining_info->finish_mutex);
		if (philosopher->dining_info->finish)
		{
			pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
			break ;
		}
		pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
		eat(philosopher);
		report_status(philosopher, "is sleeping");
		custom_sleep(philosopher, philosopher->dining_info->time_to_sleep);
		report_status(philosopher, "is thinking");
		usleep(100);
	}
	return (NULL);
}
