/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_philo.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-bah <elel-bah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 15:40:38 by elel-bah          #+#    #+#             */
/*   Updated: 2024/10/25 19:59:03 by elel-bah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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
		usleep(700);
	while (1)
	{
		pthread_mutex_lock(&philosopher->dining_info->finish_mutex);
		if (philosopher->num_of_meals == \
			philosopher->dining_info->num_must_eat || \
			philosopher->dining_info->finish)
		{
			pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
			break ;
		}
		pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
		eat(philosopher);
		report_status(philosopher, "is sleeping");
		custom_sleep(philosopher, philosopher->dining_info->time_to_sleep);
		report_status(philosopher, "is thinking");
		usleep(300);
	}
	return (NULL);
}

void	report_status(t_philosopher *philosopher, const char *message)
{
	long long	t;
	int			should_print;
	int		is_finish;

	// should_print = 0;
	// pthread_mutex_lock(&philosopher->dining_info->finish_mutex);
	// if (!philosopher->dining_info->finish)
	// {
		should_print = 1;
	// }
	// pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
	if (should_print)
	{
		pthread_mutex_lock(&philosopher->dining_info->print_mutex);
		t = get_current_time() - philosopher->dining_info->start_time;
		pthread_mutex_lock(&philosopher->dining_info->finish_mutex);
		is_finish = philosopher->dining_info->finish;
		pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
		if (!is_finish)
			printf("%lld %d %s \n", t, philosopher->identifier, message);
		if (ft_strcmp(message, "is eating") == 0)
		{
			pthread_mutex_lock(&philosopher->dining_info->last_meal_mutex);
			philosopher->last_meal_time = get_current_time();
			pthread_mutex_unlock(&philosopher->dining_info->last_meal_mutex);
		}
		pthread_mutex_unlock(&philosopher->dining_info->print_mutex);
	}
}
void	print_death(char *message, t_philosopher *philosopher, int identifier)
{
	size_t	time;

	time = get_current_time() - philosopher->dining_info->start_time;
	pthread_mutex_lock(&philosopher->dining_info->print_mutex);
		printf("%zu %d %s\n", time, identifier, message);
	pthread_mutex_unlock(&philosopher->dining_info->print_mutex);
}