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

void	destroy_resources(t_dining_info *dining_info)
{
	int	i;
	int	yes;

	yes = 1;
	while (yes)
	{
		i = -1;
		dining_info->num_full_philosophers = 0;
		while (++i < dining_info->num_philosophers)
		{
			if (yes && check_death(&dining_info->philosophers[i]))
				yes = 0;
		}
		usleep(10);
	}
		join_free_and_destroy(dining_info);
}