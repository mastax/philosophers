#include "philo.h"

int	free_info(t_dining_info *dining_info)
{
	free(dining_info->philosophers);
	free(dining_info->forks);
	return (0);
}

int	free_and_destroy(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	while (++i < dining_info->num_philosophers)
		pthread_mutex_destroy(&dining_info->forks[i]);
	free_info(dining_info);
	pthread_mutex_destroy(&dining_info->print_mutex);
	pthread_mutex_destroy(&dining_info->meal_mutex);
	pthread_mutex_destroy(&dining_info->finish_mutex);
	return (0);
}

void	join_free_and_destroy(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	while (++i < dining_info->num_philosophers)
		pthread_join(dining_info->philosophers[i].philosopher_thread, NULL);
	free_and_destroy(dining_info);
}

void destroy_resources(t_dining_info *dining_info)
{
    pthread_t monitor;
    int success = 0;

    if (pthread_create(&monitor, NULL, monitor_philosophers, dining_info))
        return;

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
            success = 1;
            pthread_mutex_unlock(&dining_info->finish_mutex);
            break;
        }
        usleep(1000);
    }

    pthread_join(monitor, NULL);

    if (success)
    {
        pthread_mutex_lock(&dining_info->print_mutex);
        printf("Philosophers Success\n");
        pthread_mutex_unlock(&dining_info->print_mutex);
    }

    join_free_and_destroy(dining_info);
}