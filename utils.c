/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-bah <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 15:40:19 by elel-bah          #+#    #+#             */
/*   Updated: 2024/06/25 15:40:21 by elel-bah         ###   ########.fr       */
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
		printf("[%lld] Philosopher: [%d] %s\n", t, philosopher->identifier,
			message);
		if (ft_strncmpp(message, "is eating") == 0)
		{
			pthread_mutex_lock(&philosopher->dining_info->status_mutex);
			philosopher->last_meal_time = get_current_time();
			pthread_mutex_unlock(&philosopher->dining_info->status_mutex);
		}
		pthread_mutex_unlock(&philosopher->dining_info->print_mutex);
	}
}

void	custom_sleep(t_philosopher *philosopher, long long ms)
{
	long long	start;

	start = get_current_time();
	while (1)
	{
		pthread_mutex_lock(&philosopher->dining_info->finish_mutex);
		if (philosopher->dining_info->finish || get_current_time()
			- start >= ms)
		{
			pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
			break ;
		}
		pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
		usleep(100);
	}
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	unsigned int	i;

	i = 0;
	while (s1[i] == s2[i] && s1[i] != '\0' && i < n)
		i++;
	if (i == n)
		return (0);
	else
		return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}
