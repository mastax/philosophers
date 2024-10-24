/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-bah <elel-bah@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 15:40:19 by elel-bah          #+#    #+#             */
/*   Updated: 2024/10/24 17:38:48 by elel-bah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long long	get_current_time(void)
{
	struct timeval	timeval;

	gettimeofday(&timeval, NULL);
	return ((timeval.tv_sec * 1000) + (timeval.tv_usec / 1000));
}

int	check_overflow(long long n, int digit, int sign)
{
	if (n > INT_MAX / 10 || (n == INT_MAX / 10 && digit > INT_MAX % 10))
	{
		if (sign == 1)
			return (-1);
		else if (sign == -1 && n > -(long long)INT_MIN / 10)
			return (0);
		else if (sign == -1 && n == -(long long)INT_MIN / \
			10 && digit > -(INT_MIN % 10))
			return (0);
	}
	return (1);
}

int	ft_parse_number(const char *nptr, int *i, int sign)
{
	long long	n;
	int			overflow;

	n = 0;
	while (nptr[*i] >= '0' && nptr[*i] <= '9')
	{
		overflow = check_overflow(n, nptr[*i] - '0', sign);
		if (overflow != 1)
			return (overflow);
		n = n * 10 + (nptr[*i] - '0');
		(*i)++;
	}
	while (nptr[*i])
	{
		if (nptr[*i] != ' ' && (nptr[*i] < 9 || nptr[*i] > 13))
			return (-1);
		(*i)++;
	}
	return ((int)(n * sign));
}

int	ft_atoi_custom(const char *nptr)
{
	int	i;
	int	sign;

	i = 0;
	sign = 1;
	while (nptr[i] == ' ' || (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			sign = -1;
		i++;
	}
	return (ft_parse_number(nptr, &i, sign));
}

int	ft_strcmp(const char *str1, const char *str2)
{
	unsigned int	it;

	it = 0;
	while (str1[it] == str2[it] && str1[it] != '\0')
		it++;
	return ((unsigned char)str1[it] - (unsigned char)str2[it]);
}
