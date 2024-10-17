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

// static void	wait_for_completion(t_dining_info *dining_info, int *success)
// {
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
// }


//=-=-=-=-=-

// void destroy_resources(t_dining_info *dining_info) //last
// {
//     int success = 0;
//     long long current_time;
//     int should_break = 0;

//     if (dining_info->num_philosophers)
//     {
//         while (1)
//         {
//             // Check for death
//             current_time = get_current_time();
//             for (int i = 0; i < dining_info->num_philosophers && !should_break; i++)
//             {
//                 pthread_mutex_lock(&dining_info->last_meal_mutex);
//                 if (current_time - dining_info->philosophers[i].last_meal_time > dining_info->time_to_die)
//                 {
//                     report_status(&dining_info->philosophers[i], "died");
//                     pthread_mutex_lock(&dining_info->finish_mutex);
//                     dining_info->finish = 1;
//                     pthread_mutex_unlock(&dining_info->finish_mutex);
//                     pthread_mutex_unlock(&dining_info->last_meal_mutex);
//                     should_break = 1;
//                     break;
//                 }
//                 pthread_mutex_unlock(&dining_info->last_meal_mutex);
//             }

//             // Check if all philosophers have eaten enough
//             pthread_mutex_lock(&dining_info->finish_mutex);
//             if (dining_info->finish)
//             {
//                 pthread_mutex_unlock(&dining_info->finish_mutex);
//                 break;
//             }
//             pthread_mutex_unlock(&dining_info->finish_mutex);

//             if (check_all_ate_enough(dining_info))
//             {
//                 pthread_mutex_lock(&dining_info->finish_mutex);
//                 dining_info->finish = 1;
//                 success = 1;
//                 pthread_mutex_unlock(&dining_info->finish_mutex);
//                 break;
//             }
//             usleep(1000);
//         }
//     }
//     // Join all philosopher threads
//     for (int i = 0; i < dining_info->num_philosophers; i++)
//     {
//         pthread_join(dining_info->philosophers[i].philosopher_thread, NULL);
//     }
//     if (success)
//     {
//         pthread_mutex_lock(&dining_info->print_mutex);
//         printf("Philosophers Success\n");
//         pthread_mutex_unlock(&dining_info->print_mutex);
//     }
//     free_and_destroy(dining_info);
// }

//=-=-=-=-=-
// void destroy_resources(t_dining_info *dining_info)
// {
//     int success = 0;
//     long long current_time;

//     if (dining_info->num_philosophers)
//     {
//         while (1)
//         {
//             // Check for death
//             current_time = get_current_time();
//             for (int i = 0; i < dining_info->num_philosophers; i++)
//             {
//                 pthread_mutex_lock(&dining_info->last_meal_mutex);
//                 if (current_time - dining_info->philosophers[i].last_meal_time > dining_info->time_to_die)
//                 {
//                     report_status(&dining_info->philosophers[i], "died");
//                     pthread_mutex_lock(&dining_info->finish_mutex);
//                     dining_info->finish = 1;
//                     pthread_mutex_unlock(&dining_info->finish_mutex);
//                     pthread_mutex_unlock(&dining_info->last_meal_mutex);
//                 }
//                 pthread_mutex_unlock(&dining_info->last_meal_mutex);
//             }

//             // Check if all philosophers have eaten enough
//             pthread_mutex_lock(&dining_info->finish_mutex);
//             if (dining_info->finish)
//             {
//                 pthread_mutex_unlock(&dining_info->finish_mutex);
//                 break;
//             }
//             pthread_mutex_unlock(&dining_info->finish_mutex);

//             if (check_all_ate_enough(dining_info))
//             {
//                 pthread_mutex_lock(&dining_info->finish_mutex);
//                 dining_info->finish = 1;
//                 success = 1;
//                 pthread_mutex_unlock(&dining_info->finish_mutex);
//                 break;
//             }
//             usleep(1000);
//         }
//     }
//     // Join all philosopher threads
//     for (int i = 0; i < dining_info->num_philosophers; i++)
//     {
//         pthread_join(dining_info->philosophers[i].philosopher_thread, NULL);
//     }
//     if (success)
//     {
//         pthread_mutex_lock(&dining_info->print_mutex);
//         printf("Philosophers Success\n");
//         pthread_mutex_unlock(&dining_info->print_mutex);
//     }
//     free_and_destroy(dining_info);
// }

// static void	join_philosopher_threads(t_dining_info *dining_info)
// {
// 	int	i;

// 	i = 0;
// 	while (i < dining_info->num_philosophers)
// 	{
// 		pthread_join(dining_info->philosophers[i].philosopher_thread, NULL);
// 		i++;
// 	}
// }

// void	destroy_resources(t_dining_info *dining_info)
// {
// 	pthread_t	monitor;
// 	int			success;

// 	success = 0;
// 	if (dining_info->num_philosophers)
// 	{
// 		if (pthread_create(&monitor, NULL, monitor_philosophers, dining_info))
// 			return ;
// 		wait_for_completion(dining_info, &success);
// 		pthread_join(monitor, NULL);
// 		if (success)
// 		{
// 			pthread_mutex_lock(&dining_info->print_mutex);
// 			printf("Philosophers Success\n");
// 			pthread_mutex_unlock(&dining_info->print_mutex);
// 		}
// 	}
// 	join_philosopher_threads(dining_info);
// 	free_and_destroy(dining_info);
// }


static int check_philosopher_death(t_dining_info *dining_info)
{
    long long current_time;
    int i;

    current_time = get_current_time();
    i = -1;
    while (++i < dining_info->num_philosophers)
    {
        pthread_mutex_lock(&dining_info->last_meal_mutex);
        if (current_time - dining_info->philosophers[i].last_meal_time 
            > dining_info->time_to_die)
        {
            report_status(&dining_info->philosophers[i], "died");
            pthread_mutex_lock(&dining_info->finish_mutex);
            dining_info->finish = 1;
            pthread_mutex_unlock(&dining_info->finish_mutex);
            pthread_mutex_unlock(&dining_info->last_meal_mutex);
            return (1);
        }
        pthread_mutex_unlock(&dining_info->last_meal_mutex);
    }
    return (0);
}

static void check_and_handle_completion(t_dining_info *dining_info, int *success)
{
    pthread_mutex_lock(&dining_info->finish_mutex);
    if (dining_info->finish)
    {
        pthread_mutex_unlock(&dining_info->finish_mutex);
        return ;
    }
    pthread_mutex_unlock(&dining_info->finish_mutex);
    if (check_all_ate_enough(dining_info))
    {
        pthread_mutex_lock(&dining_info->finish_mutex);
        dining_info->finish = 1;
        *success = 1;
        pthread_mutex_unlock(&dining_info->finish_mutex);
    }
}

static void join_and_print(t_dining_info *dining_info, int success)
{
    int i;

    i = -1;
    while (++i < dining_info->num_philosophers)
        pthread_join(dining_info->philosophers[i].philosopher_thread, NULL);
    if (success)
    {
        pthread_mutex_lock(&dining_info->print_mutex);
        printf("Philosophers Success\n");
        pthread_mutex_unlock(&dining_info->print_mutex);
    }
}

void destroy_resources(t_dining_info *dining_info)
{
    int success;

    success = 0;
    if (dining_info->num_philosophers)
    {
        while (1)
        {
            if (check_philosopher_death(dining_info))
                break ;
            check_and_handle_completion(dining_info, &success);
            pthread_mutex_lock(&dining_info->finish_mutex);
            if (dining_info->finish)
            {
                pthread_mutex_unlock(&dining_info->finish_mutex);
                break ;
            }
            pthread_mutex_unlock(&dining_info->finish_mutex);
            usleep(1000);
        }
    }
    join_and_print(dining_info, success);
    free_and_destroy(dining_info);
}
