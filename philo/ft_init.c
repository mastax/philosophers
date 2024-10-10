/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_init.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-bah <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 15:40:57 by elel-bah          #+#    #+#             */
/*   Updated: 2024/06/29 15:11:43 by elel-bah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	validate_input(int ac, char **av, t_dining_info *dining_info)
{
	if (ac < 5 || ac > 6)
	{
		report_error("Error: Invalid number of arguments. ");
		return (report_error("Expected 5 or 6.\n"));
	}
	dining_info->num_philosophers = ft_atoi_custom(av[1]);
	if (dining_info->num_philosophers > 200)
		return (report_error("Error: too many philosophers\n"));
	dining_info->time_to_die = ft_atoi_custom(av[2]);
	dining_info->time_to_eat = ft_atoi_custom(av[3]);
	dining_info->time_to_sleep = ft_atoi_custom(av[4]);
	if (dining_info->num_philosophers < 1 || dining_info->time_to_die < 1 || \
			dining_info->time_to_eat < 1 || dining_info->time_to_sleep < 1)
	{
		report_error("Error: Invalid input. All parameters");
		return (report_error(" must be greater than 0.\n"));
	}
	return (0);
}

int	initialize_info(t_dining_info *dining_info, int ac, char **av)
{
	int	result;

	result = validate_input(ac, av, dining_info);
	if (result != 0)
		return (result);
	if (ac == 6)
	{
		dining_info->num_must_eat = ft_atoi_custom(av[5]);
		if (dining_info->num_must_eat < 1)
		{
			report_error("Error: Invalid input. Number of meals");
			return (report_error(" must be greater than 0.\n"));
		}
	}
	else
		dining_info->num_must_eat = -1;
	dining_info->finish = 0;
	return (0);
}

int	initialize_philosophers(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	dining_info->philosophers = malloc(sizeof(t_philosopher)
			* dining_info->num_philosophers);
	dining_info->forks = malloc(sizeof(pthread_mutex_t)
			* dining_info->num_philosophers);
	dining_info->start_time = get_current_time();
	if (!dining_info->philosophers || !dining_info->forks)
		return (report_error("Error:\
				Failed to allocate memory for philosophers or forks.\n"));
	while (++i < dining_info->num_philosophers)
	{
		dining_info->philosophers[i].identifier = i + 1;
		dining_info->philosophers[i].left_fork = i;
		dining_info->philosophers[i].right_fork = \
			(i + 1) % dining_info->num_philosophers;
		dining_info->philosophers[i].num_of_meals = 0;
		dining_info->philosophers[i].last_meal_time = dining_info->start_time;
		dining_info->philosophers[i].dining_info = dining_info;
	}
	return (0);
}

int	initialize_mutex(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	while (++i < dining_info->num_philosophers)
	{
		if (pthread_mutex_init(&dining_info->forks[i], NULL))
			return (report_error("Error: Failed to initialize fork mutex.\n"));
	}
	if (pthread_mutex_init(&dining_info->print_mutex, NULL))
		return (report_error("Error: Failed to initialize print mutex.\n"));
	if (pthread_mutex_init(&dining_info->meal_mutex, NULL))
		return (report_error("Error: Failed to initialize meal mutex.\n"));
	if (pthread_mutex_init(&dining_info->finish_mutex, NULL))
		return (report_error("Error: Failed to initialize finish mutex.\n"));
	if (pthread_mutex_init(&dining_info->last_meal_mutex, NULL))
		return (report_error("Error: Failed to initialize status mutex.\n"));
	return (0);
}

int	create_philosophers(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	if (dining_info->num_philosophers == 1)
	{
		report_status(&dining_info->philosophers[0], "has taken a fork");
		custom_sleep(&dining_info->philosophers[0], dining_info->time_to_die);
		report_status(&dining_info->philosophers[0], "died");
		check_completion(&dining_info->philosophers[0], 1);
		return (0);
	}
	while (++i < dining_info->num_philosophers)
	{
		if (pthread_create(&dining_info->philosophers[i].philosopher_thread,
				NULL, philosopher_thread_start,
				&(dining_info->philosophers[i])))
			return (report_error("Error:\
					Failed to create philosopher threads.\n"));
	}
	return (0);
}
