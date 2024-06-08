#include "philo.h"

void	ft_eat(t_philos *philos)
{
	pthread_mutex_lock(philos->right_chopsticks);
	ft_output(philos->data, philos->id_of_philo, "has taken a fork");
	pthread_mutex_lock(philos->left_chopsticks);
	ft_output(philos->data, philos->id_of_philo, "has taken a fork");
	ft_output(philos->data, philos->id_of_philo, "is eating");
	philos->last_meal = ft_timestamp();
	ft_sleep(philos->data->time_2_eat, philos->data);
	philos->data->total++;
	pthread_mutex_lock(&philos->data->eat);
	philos->eat++;
	pthread_mutex_unlock(&philos->data->eat);
	pthread_mutex_unlock(philos->right_chopsticks);
	pthread_mutex_unlock(philos->left_chopsticks);
}