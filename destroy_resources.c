/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destroy_resources.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-bah <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/29 16:06:31 by elel-bah          #+#    #+#             */
/*   Updated: 2024/06/29 16:06:32 by elel-bah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// static void	monitor_and_finish_check(t_dining_info *dining_info, int *success)
// {
// 	pthread_t	monitor;

// 	if (pthread_create(&monitor, NULL, monitor_philosophers, dining_info))
// 		return ;
// 	while (1)
// 	{
// 		pthread_mutex_lock(&dining_info->finish_mutex);
// 		if (dining_info->finish)
// 		{
// 			pthread_mutex_unlock(&dining_info->finish_mutex);
// 			break ;
// 		}
// 		pthread_mutex_unlock(&dining_info->finish_mutex);
// 		if (check_all_ate_enough(dining_info))
// 		{
// 			pthread_mutex_lock(&dining_info->finish_mutex);
// 			dining_info->finish = 1;
// 			*success = 1;
// 			pthread_mutex_unlock(&dining_info->finish_mutex);
// 			break ;
// 		}
// 		usleep(1000);
// 	}
// 	pthread_join(monitor, NULL);
// }

// static void	print_success_message(t_dining_info *dining_info)
// {
// 	pthread_mutex_lock(&dining_info->print_mutex);
// 	printf("Philosophers Success\n");
// 	pthread_mutex_unlock(&dining_info->print_mutex);
// }

// void	destroy_resources(t_dining_info *dining_info)
// {
// 	int	success;

// 	success = 0;
// 	if (dining_info->num_philosophers != 200)
// 	{
// 		monitor_and_finish_check(dining_info, &success);
// 		if (success)
// 			print_success_message(dining_info);
// 	}
// 	else
// 		join_free_and_destroy(dining_info);
// }

static void wait_for_completion(t_dining_info *dining_info, int *success)
{
	while (1)
	{
		pthread_mutex_lock(&dining_info->finish_mutex);
		if (dining_info->finish)
		{
			pthread_mutex_unlock(&dining_info->finish_mutex);
			break;
		}
		pthread_mutex_unlock(&dining_info->finish_mutex);
		if (check_all_ate_enough(dining_info))
		{
			pthread_mutex_lock(&dining_info->finish_mutex);
			dining_info->finish = 1;
			*success = 1;
			pthread_mutex_unlock(&dining_info->finish_mutex);
			break;
		}
		usleep(1000);
	}
}

static void join_philosopher_threads(t_dining_info *dining_info)
{
	int i;

	for (i = 0; i < dining_info->num_philosophers; i++)
	{
		pthread_join(dining_info->philosophers[i].philosopher_thread, NULL);
	}
}

void	destroy_resources(t_dining_info *dining_info)
{
	pthread_t	monitor;
	int			success;

	success = 0;
	if (dining_info->num_philosophers != 200)
	{
		if (pthread_create(&monitor, NULL, monitor_philosophers, dining_info))
			return;
		wait_for_completion(dining_info, &success);
		pthread_join(monitor, NULL);
		if (success)
		{
			pthread_mutex_lock(&dining_info->print_mutex);
			printf("Philosophers Success\n");
			pthread_mutex_unlock(&dining_info->print_mutex);
		}
	}
	join_philosopher_threads(dining_info);
	join_free_and_destroy(dining_info);
}
