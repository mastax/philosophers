#include "philo.h"

int	ft_initialize_data(t_data *data)
{
	data->nbr_philos = ft_atoi(data->av[1]);
	data->time_2_die = ft_atoi(data->av[2]);
	data->time_2_eat = ft_atoi(data->av[3]);
	data->time_2_sleep = ft_atoi(data->av[4]);
	if (data->av[5])
		data->must_eat = ft_atoi(data->av[5]);
	else
		data->must_eat = -1;
	data->dead = 0;
	data->all_are_ate = 0;
	if (data->nbr_philos < 1)
		return (0);
	return (1);
}