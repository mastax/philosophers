#include "philo.h"

void	ft_initialize_philosophers(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->nbr_philos)
	{
		data->philo[i].eat = 0;
		data->philo[i].id_of_philo = i + 1;
		data->philo[i].data = data;
		data->philo[i].right_chopsticks = &data->chopsticks[i];
		if (i == (data->nbr_philos - 1))
			data->philo[i].left_chopsticks = &data->chopsticks[0];
		else
			data->philo[i].left_chopsticks = &data->chopsticks[i + 1];
		i++;
	}
}