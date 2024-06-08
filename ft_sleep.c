#include "philo.h"

long long	ft_period(long long past, long long present)
{
	return (present - past);
}

void	ft_sleep(long long time, t_data *data)
{
	long long	i;

	i = ft_timestamp();
	while (!(data->dead))
	{
		if (ft_period(i, ft_timestamp()) >= time)
			break ;
		usleep(500);
	}
}