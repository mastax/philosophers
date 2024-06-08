#include "philo.h"

void	ft_destroy(t_data *data)
{
	int	i;

	i = 0;
	pthread_mutex_destroy(&data->output);
	pthread_mutex_destroy(&data->eat);
	while (i < data->nbr_philos)
	{
		pthread_detach(data->philo[i].thread_id);
		pthread_mutex_destroy(data->philo[i].right_chopsticks);
		pthread_mutex_destroy(data->philo[i].left_chopsticks);
		i++;
	}
}

int	ft_join_destroy(t_data *data)
{
	int	i;

	i = 0;
	if (ft_check_dead(data))
	{
		ft_destroy(data);
		free(data);
		return (0);
	}
	while (i < data->nbr_philos)
	{
		if (pthread_join(data->philo[i].thread_id, NULL) != 0)
		{
			free(data);
			return (0);
		}
		i++;
	}
	return (1);
}

int	ft_create_philosophers(t_data *data)
{
	int	i;

	i = 0;
	data->birth = ft_timestamp();
	while (i < data->nbr_philos)
	{
		data->philo[i].last_meal = ft_timestamp();
		data->n = &data->philo[i];
		if (pthread_create(&data->n->thread_id, NULL, &routine, data->n) != 0)
		{
			free(data);
			return (0);
		}
		usleep(100);
		i++;
	}
	if (!ft_join_destroy(data))
		return (0);
	return (1);
}