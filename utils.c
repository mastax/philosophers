/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-bah <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 15:40:19 by elel-bah          #+#    #+#             */
/*   Updated: 2024/06/29 15:14:14 by elel-bah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	report_error(char *str)
{
	int	i;

	i = -1;
	while (str[++i])
		write(2, &str[i], 1);
	return (1);
}

int	ft_atoi_custom(const char *nptr)
{
	int			i;
	int			ti;
	int			sign;
	long long	n;

	i = 0;
	sign = 1;
	n = 0;
	if (nptr[i] == '-')
		sign *= -1;
	if (nptr[i] == '-' || nptr[i] == '+')
		i++;
	ti = i - 1;
	while (nptr[++ti])
	{
		if (nptr[ti] < '0' || nptr[ti] > '9')
			return (-1);
	}
	while (nptr[i] && nptr[i] >= '0' && nptr[i] <= '9')
	{
		n = n * 10 + (nptr[i] - '0');
		i++;
	}
	return (n * sign);
}

long long	get_current_time(void)
{
	struct timeval	timeval;

	gettimeofday(&timeval, NULL);
	return ((timeval.tv_sec * 1000) + (timeval.tv_usec / 1000));
}

int	ft_strcmp(const char *str1, const char *str2)
{
	unsigned int	it;

	it = 0;
	while (str1[it] == str2[it] && str1[it] != '\0')
		it++;
	return ((unsigned char)str1[it] - (unsigned char)str2[it]);
}

void	report_status(t_philosopher *philosopher, const char *message)
{
	long long	t;
	int			should_print;

	should_print = 0;
	pthread_mutex_lock(&philosopher->dining_info->finish_mutex);
	if (!philosopher->dining_info->finish)
	{
		should_print = 1;
	}
	pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
	if (should_print)
	{
		pthread_mutex_lock(&philosopher->dining_info->print_mutex);
		t = get_current_time() - philosopher->dining_info->start_time;
		printf("%lld %d %s\n", t, philosopher->identifier,
			message);
		if (ft_strcmp(message, "is eating") == 0)
		{
			pthread_mutex_lock(&philosopher->dining_info->status_mutex);
			philosopher->last_meal_time = get_current_time();
			pthread_mutex_unlock(&philosopher->dining_info->status_mutex);
		}
		pthread_mutex_unlock(&philosopher->dining_info->print_mutex);
	}
}
