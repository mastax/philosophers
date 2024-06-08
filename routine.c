#include "philo.h"

void	*routine(void *ptr)
{
	t_philos	*philo;

	philo = (t_philos *)ptr;
	while (!(philo->data->dead)
		&& (philo->eat != philo->data->must_eat))
	{
		ft_eat(philo);
		ft_output(philo->data, philo->id_of_philo, "is sleeping");
		ft_sleep(philo->data->time_2_sleep, philo->data);
		ft_output(philo->data, philo->id_of_philo, "is thinking");
	}
	return (NULL);
}