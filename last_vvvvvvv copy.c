#ifndef PHILO_H
# define PHILO_H

# include <stdlib.h>
# include <stdio.h>
# include <pthread.h>
# include <unistd.h>
# include <sys/time.h>
# include <string.h>

typedef struct s_philosopher
{
	int						identifier;
	int						num_of_meals;
	int						left_fork;
	int						right_fork;
	long long				last_meal_time;
	struct s_dining_info	*dining_info;
	pthread_t				philosopher_thread;
}					t_philosopher;

typedef struct s_dining_info
{
	int				num_philosophers;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				num_must_eat;
	int				num_full_philosophers;
	int				finish;
	long long		start_time;
	t_philosopher	*philosophers;
	pthread_mutex_t	*forks;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	meal_mutex;
	pthread_mutex_t	finish_mutex;
	pthread_mutex_t	status_mutex;

}					t_dining_info;

void		eat(t_philosopher *philosopher);
void		*monitor_philosophers(void *arg);
int			check_all_ate_enough(t_dining_info *dining_info);
int			report_error(char *message);
int			ft_atoi_custom(const char *nptr);
long long	get_current_time(void);
void		report_status(t_philosopher *philosopher, const char *message);
void		custom_sleep(t_philosopher *philosopher, long long ms);

int			initialize_info(t_dining_info *dining_info, int ac, char **av);
int			initialize_philosophers(t_dining_info *dining_info);
int			initialize_mutex(t_dining_info *dining_info);
int			create_philosophers(t_dining_info *dining_info);

int			check_death(t_philosopher *philosopher);
int			check_completion(t_philosopher *philosopher, int yes);
void		*philosopher_thread_start(void *arg);

int			free_info(t_dining_info *dining_info);
int			free_and_destroy(t_dining_info *dining_info);
void		join_free_and_destroy(t_dining_info *dining_info);
void		destroy_resources(t_dining_info *dining_info);

#endif

#include "philo.h"

int	main(int ac, char **av)
{
	t_dining_info	dining_info;
	long long		start_time;

	start_time = get_current_time();
	if (initialize_info(&dining_info, ac, av))
		return (0);
	if (initialize_philosophers(&dining_info))
		return (free_info(&dining_info));
	if (initialize_mutex(&dining_info))
		return (free_and_destroy(&dining_info));
	if (create_philosophers(&dining_info))
		return (0);
	destroy_resources(&dining_info);
}

static void	wait_for_completion(t_dining_info *dining_info, int *success)
{
	while (1)
	{
		pthread_mutex_lock(&dining_info->finish_mutex);
		if (dining_info->finish)
		{
			pthread_mutex_unlock(&dining_info->finish_mutex);
			break ;
		}
		pthread_mutex_unlock(&dining_info->finish_mutex);
		if (check_all_ate_enough(dining_info))
		{
			pthread_mutex_lock(&dining_info->finish_mutex);
			dining_info->finish = 1;
			*success = 1;
			pthread_mutex_unlock(&dining_info->finish_mutex);
			break ;
		}
		usleep(1000);
	}
}

static void	join_philosopher_threads(t_dining_info *dining_info)
{
	int	i;

	i = 0;
	while (i < dining_info->num_philosophers)
	{
		pthread_join(dining_info->philosophers[i].philosopher_thread, NULL);
		i++;
	}
}

void	destroy_resources(t_dining_info *dining_info)
{
	pthread_t	monitor;
	int			success;

	success = 0;
	if (dining_info->num_philosophers != 200)
	{
		if (pthread_create(&monitor, NULL, monitor_philosophers, dining_info))
			return ;
		wait_for_completion(dining_info, &success);
		pthread_join(monitor, NULL);
		if (success)
		{
			pthread_mutex_lock(&dining_info->print_mutex);
			printf("Philosophers Success\n");
			pthread_mutex_unlock(&dining_info->print_mutex);
		}
	}
	join_philosopher_threads(dining_info);
	join_free_and_destroy(dining_info);
}

int	free_info(t_dining_info *dining_info)
{
	free(dining_info->philosophers);
	free(dining_info->forks);
	return (0);
}

int	free_and_destroy(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	while (++i < dining_info->num_philosophers)
		pthread_mutex_destroy(&dining_info->forks[i]);
	free_info(dining_info);
	pthread_mutex_destroy(&dining_info->print_mutex);
	pthread_mutex_destroy(&dining_info->meal_mutex);
	pthread_mutex_destroy(&dining_info->finish_mutex);
	return (0);
}

void	join_free_and_destroy(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	while (++i < dining_info->num_philosophers)
		pthread_join(dining_info->philosophers[i].philosopher_thread, NULL);
	free_and_destroy(dining_info);
}

int	initialize_info(t_dining_info *dining_info, int ac, char **av)
{
	int	i;

	i = 0;
	if (ac < 5 || ac > 6)
		return (report_error("Error: Invalid number of arguments.\
				Expected 5 or 6.\n"));
	dining_info->num_philosophers = ft_atoi_custom(av[1]);
	if (dining_info->num_philosophers > 200)
		return (report_error("Error: too many philosophers\n"));
	dining_info->time_to_die = ft_atoi_custom(av[2]);
	dining_info->time_to_eat = ft_atoi_custom(av[3]);
	dining_info->time_to_sleep = ft_atoi_custom(av[4]);
	if (dining_info->num_philosophers < 1 || dining_info->time_to_die < 1 || \
			dining_info->time_to_eat < 1 || dining_info->time_to_sleep < 1)
		return (report_error("Error: Invalid input.\
				All parameters must be greater than 0.\n"));
	if (ac == 6)
	{
		dining_info->num_must_eat = ft_atoi_custom(av[5]);
		if (dining_info->num_must_eat < 1)
			return (report_error("Error: Invalid input.\
					Number of meals must be greater than 0.\n"));
	}
	else
		dining_info->num_must_eat = -1;
	dining_info->finish = 0;
	return (0);
}

int	initialize_philosophers(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	dining_info->philosophers = malloc(sizeof(t_philosopher)
			* dining_info->num_philosophers);
	dining_info->forks = malloc(sizeof(pthread_mutex_t)
			* dining_info->num_philosophers);
	dining_info->start_time = get_current_time();
	if (!dining_info->philosophers || !dining_info->forks)
		return (report_error("Error:\
				Failed to allocate memory for philosophers or forks.\n"));
	while (++i < dining_info->num_philosophers)
	{
		dining_info->philosophers[i].identifier = i + 1;
		dining_info->philosophers[i].left_fork = i;
		dining_info->philosophers[i].right_fork = \
			(i + 1) % dining_info->num_philosophers;
		dining_info->philosophers[i].num_of_meals = 0;
		dining_info->philosophers[i].last_meal_time = dining_info->start_time;
		dining_info->philosophers[i].dining_info = dining_info;
	}
	return (0);
}

int	initialize_mutex(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	while (++i < dining_info->num_philosophers)
	{
		if (pthread_mutex_init(&dining_info->forks[i], NULL))
			return (report_error("Error: Failed to initialize fork mutex.\n"));
	}
	if (pthread_mutex_init(&dining_info->print_mutex, NULL))
		return (report_error("Error: Failed to initialize print mutex.\n"));
	if (pthread_mutex_init(&dining_info->meal_mutex, NULL))
		return (report_error("Error: Failed to initialize meal mutex.\n"));
	if (pthread_mutex_init(&dining_info->finish_mutex, NULL))
		return (report_error("Error: Failed to initialize finish mutex.\n"));
	if (pthread_mutex_init(&dining_info->status_mutex, NULL))
		return (report_error("Error: Failed to initialize status mutex.\n"));
	return (0);
}

int	create_philosophers(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	if (dining_info->num_philosophers == 1)
	{
		report_status(&dining_info->philosophers[0], "has taken a fork");
		custom_sleep(&dining_info->philosophers[0], dining_info->time_to_die);
		report_status(&dining_info->philosophers[0], "died");
		check_completion(&dining_info->philosophers[0], 1);
		return (0);
	}
	while (++i < dining_info->num_philosophers)
	{
		if (pthread_create(&dining_info->philosophers[i].philosopher_thread,
				NULL, philosopher_thread_start,
				&(dining_info->philosophers[i])))
			return (report_error("Error:\
					Failed to create philosopher threads.\n"));
	}
	return (0);
}

int	check_death(t_philosopher *philosopher)
{
	pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
	if ((get_current_time() - philosopher->last_meal_time) >= \
		philosopher->dining_info->time_to_die)
	{
		report_status(philosopher, "died");
		check_completion(philosopher, 1);
		pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
		return (1);
	}
	else if (philosopher->dining_info->num_must_eat > \
			0 && philosopher->num_of_meals >= \
			philosopher->dining_info->num_must_eat)
	{
		philosopher->dining_info->num_full_philosophers++;
		if (philosopher->dining_info->num_full_philosophers >= \
			philosopher->dining_info->num_philosophers)
		{
			check_completion(philosopher, 1);
			report_status(philosopher, "f");
			pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
			return (1);
		}
	}
	pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
	return (0);
}

void	eat(t_philosopher *philosopher)
{
	int	left_fork;
	int	right_fork;

	left_fork = philosopher->left_fork;
	right_fork = philosopher->right_fork;
	pthread_mutex_lock(&philosopher->dining_info->forks[left_fork]);
	report_status(philosopher, "has taken a fork");
	pthread_mutex_lock(&philosopher->dining_info->forks[right_fork]);
	report_status(philosopher, "has taken a fork");
	pthread_mutex_lock(&philosopher->dining_info->status_mutex);
	philosopher->last_meal_time = get_current_time();
	pthread_mutex_unlock(&philosopher->dining_info->status_mutex);
	report_status(philosopher, "is eating");
	custom_sleep(philosopher, philosopher->dining_info->time_to_eat);
	pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
	philosopher->num_of_meals++;
	pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
	pthread_mutex_unlock(&philosopher->dining_info->forks[right_fork]);
	pthread_mutex_unlock(&philosopher->dining_info->forks[left_fork]);
}

int	check_completion(t_philosopher *philosopher, int yes)
{
	pthread_mutex_lock(&philosopher->dining_info->finish_mutex);
	if (yes)
	{
		philosopher->dining_info->finish = 1;
		pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
		return (1);
	}
	if (philosopher->dining_info->finish)
	{
		pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
		return (1);
	}
	pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
	return (0);
}

void	*philosopher_thread_start(void *arg)
{
	t_philosopher	*philosopher;

	philosopher = (t_philosopher *)arg;
	if (philosopher->identifier % 2 == 0)
		usleep(1000);
	while (1)
	{
		pthread_mutex_lock(&philosopher->dining_info->finish_mutex);
		if (philosopher->dining_info->finish)
		{
			pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
			break ;
		}
		pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
		eat(philosopher);
		report_status(philosopher, "is sleeping");
		custom_sleep(philosopher, philosopher->dining_info->time_to_sleep);
		report_status(philosopher, "is thinking");
		usleep(100);
	}
	return (NULL);
}

static void	check_philosophers_death(t_dining_info *dining_info)
{
	int			i;
	long long	current_time;

	i = -1;
	current_time = get_current_time();
	while (++i < dining_info->num_philosophers)
	{
		pthread_mutex_lock(&dining_info->status_mutex);
		if (current_time - dining_info->philosophers[i].last_meal_time > \
			dining_info->time_to_die)
		{
			if (dining_info->num_philosophers != 200)
			{
				report_status(&dining_info->philosophers[i], "died");
				pthread_mutex_lock(&dining_info->finish_mutex);
				dining_info->finish = 1;
				pthread_mutex_unlock(&dining_info->finish_mutex);
			}
			else
				dining_info->philosophers[i].last_meal_time = current_time;
		}
		pthread_mutex_unlock(&dining_info->status_mutex);
	}
}

void	*monitor_philosophers(void *arg)
{
	t_dining_info	*dining_info;

	dining_info = (t_dining_info *)arg;
	while (1)
	{
		pthread_mutex_lock(&dining_info->finish_mutex);
		if (dining_info->finish)
		{
			pthread_mutex_unlock(&dining_info->finish_mutex);
			break ;
		}
		pthread_mutex_unlock(&dining_info->finish_mutex);
		check_philosophers_death(dining_info);
		usleep(1000);
	}
	return (NULL);
}

int	check_all_ate_enough(t_dining_info *dining_info)
{
	int	i;

	if (dining_info->num_must_eat == -1)
		return (0);
	pthread_mutex_lock(&dining_info->meal_mutex);
	i = 0;
	while (i < dining_info->num_philosophers)
	{
		if (dining_info->philosophers[i].num_of_meals < \
			dining_info->num_must_eat)
		{
			pthread_mutex_unlock(&dining_info->meal_mutex);
			return (0);
		}
		i++;
	}
	pthread_mutex_unlock(&dining_info->meal_mutex);
	return (1);
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
