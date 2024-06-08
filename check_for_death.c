#include "philo.h"

int	ft_check_total_eat(t_data *data)
{
	if (data->ac == 6 && data->total >= data->nbr_philos * data->must_eat)
		return (1);
	return (0);
}

int	ft_check_dead(t_data *data)
{
	int			i;

	while (1)
	{
		i = 1;
		while (i <= data->nbr_philos)
		{
			if (ft_check_total_eat(data))
				return (1);
			data->time = ft_timestamp();
			data->norm = data->philo[i - 1].last_meal;
			if ((data->time - data->norm) >= data->time_2_die)
			{
				data->dead = 1;
				pthread_mutex_lock(&data->output);
				data->norm2 = data->time - data->birth;
				printf("[%lld] Philosopher %d died \n", data->norm2, i);
				return (1);
			}
		}
	}
	return (0);
}