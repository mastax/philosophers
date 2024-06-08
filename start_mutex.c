#include "philo.h"

int	ft_start_mutex(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->nbr_philos)
	{
		if (pthread_mutex_init(&data->chopsticks[i], NULL) != 0)
		{
			free(data);
			return (0);
		}
		i++;
	}
	if (pthread_mutex_init(&data->output, NULL) != 0)
	{
		free(data);
		return (0);
	}
	if (pthread_mutex_init(&data->eat, NULL) != 0)
	{
		free(data);
		return (0);
	}
	return (1);
}