#include "philo.h"

int	ft_atoi(const char	*str)
{
	int	i;
	int	sign;
	int	result;

	sign = 1;
	result = 0;
	i = 0;
	while (str[i] == '\t' || str[i] == '\n'
		|| str[i] == '\r' || str[i] == '\v'
		|| str[i] == ' ' || str[i] == '\f')
		i++;
	if (str[i] == '+')
		i++;
	else if (str[i] == '-')
	{
		sign *= -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = (result * 10) + (str[i] - '0');
		i++;
	}
	return (result * sign);
}

// static int	func_overflow(int sign)
// {
// 	if (sign == -1)
// 		return (0);
// 	else
// 		return (-1);
// }

// int	ft_atoi(const char *str)
// {
// 	int				i;
// 	int				sign;
// 	unsigned long	result;
// 	int				counter;

// 	i = 0;
// 	sign = 1;
// 	result = 0;
// 	counter = 0;
// 	while (str[i] == 32 || (str[i] >= 9 && str[i] <= 13))
// 		i++;
// 	if ((str[i] == '-') || (str[i] == '+'))
// 	{
// 		if (str[i] == '-')
// 			sign *= -1;
// 		i++;
// 	}
// 	while (str[i] && str[i] >= '0' && str[i] <= '9')
// 	{
// 		counter++;
// 		if (result * 10 + str[i] - '0' > LONG_MAX && counter >= 20)
// 			return (func_overflow(sign));
// 		result = result * 10 + str[i++] - '0';
// 	}
// 	return (result * sign);
// }

// static char	*ft_strdup(const char *str)
// {
// 	int		x;
// 	char	*dup;

// 	x = 0;
// 	dup = malloc(sizeof(char ) * (ft_strlen(str) + 1));
// 	if (!dup)
// 		return (NULL);
// 	while (str[x])
// 	{
// 		dup[x] = str[x];
// 		x++;
// 	}
// 	dup[x] = '\0';
// 	return (dup);
// }

char	*ft_substr(char *s, int start, size_t len)
{
	char			*ptr;
	unsigned int	i;
	int				end;

	i = 0;
	end = len + start;
	if (!s)
		return (NULL);
	ptr = (char *)malloc((len + 1) * sizeof(char));
	if (!ptr)
		return (NULL);
	if (start <= ft_strlen(s))
	{
		while (start < end && s[start] != '\0')
		{
			ptr[i] = s[start];
			i++;
			start++;
		}
	}
	ptr[i] = '\0';
	return (ptr);
}

void	handle_error(void)
{
	write(STDERR_FILENO, "Error\n", 6);
	exit(1);
}
