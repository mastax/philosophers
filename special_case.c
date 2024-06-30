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

static int create_philosopher_threads(t_dining_info *dining_info, pthread_t *threads)
{
    int i;

    for (i = 0; i < dining_info->num_philosophers; i++)
    {
        if (pthread_create(&threads[i], NULL, philosopher_thread_start,
                &(dining_info->philosophers[i])))
            return (1);
        usleep(100);
    }
    return (0);
}

static void join_all_threads(t_dining_info *dining_info, pthread_t *threads, pthread_t monitor)
{
    int i;

    for (i = 0; i < dining_info->num_philosophers; i++)
        pthread_join(threads[i], NULL);
    pthread_join(monitor, NULL);
}

void handle_special_case(t_dining_info *dining_info)
{
    pthread_t *threads;
    pthread_t monitor;

    threads = malloc(sizeof(pthread_t) * dining_info->num_philosophers);
    if (!threads)
        return;
    dining_info->time_to_die += 50;
    if (create_philosopher_threads(dining_info, threads))
    {
        free(threads);
        return;
    }
    if (pthread_create(&monitor, NULL, monitor_philosophers, dining_info))
    {
        free(threads);
        return;
    }
    join_all_threads(dining_info, threads, monitor);
    free(threads);
}

// static void	create_philosopher_threads(t_dining_info *dining_info)
// {
// 	int			i;
// 	pthread_t	*threads;

// 	i = -1;
// 	threads = malloc(sizeof(pthread_t) * dining_info->num_philosophers);
// 	if (!threads)
// 		return ;
// 	dining_info->time_to_die += 50;
// 	while (++i < dining_info->num_philosophers)
// 	{
// 		if (pthread_create(&threads[i], NULL, philosopher_thread_start,
// 				&(dining_info->philosophers[i])))
// 		{
// 			free(threads);
// 			return ;
// 		}
// 		usleep(100);
// 	}
// 	i = 0;
// 	while (i++ < dining_info->num_philosophers)
// 		pthread_join(threads[i], NULL);
// 	free(threads);
// }

// static void	start_monitor_thread(t_dining_info *dining_info)
// {
// 	pthread_t	monitor;

// 	if (pthread_create(&monitor, NULL, monitor_philosophers, dining_info))
// 		return ;
// 	pthread_join(monitor, NULL);
// }

// void	handle_special_case(t_dining_info *dining_info)
// {
// 	create_philosopher_threads(dining_info);
// 	start_monitor_thread(dining_info);
// }
