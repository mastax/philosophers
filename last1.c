#ifndef PHILO_H
# define PHILO_H

# include <stdlib.h>
# include <stdio.h>
# include <pthread.h>
# include <unistd.h>
# include <sys/time.h>

#define MAX_EATING 50 // Adjust based on testing

int eating_count = 0;
pthread_mutex_t eating_mutex;
pthread_cond_t death_cond;
pthread_mutex_t death_mutex;

typedef struct s_philosopher
{
	int				identifier;
	int				num_of_meals;
	int				left_fork;
	int				right_fork;
	long long		last_meal_time;
	struct s_dining_info	*dining_info;
	pthread_t		philosopher_thread;
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
	t_philosopher			*philosophers;
	pthread_mutex_t	*forks;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	meal_mutex;
	pthread_mutex_t	finish_mutex;
}					t_dining_info;

void		eat(t_philosopher *philosopher);
void 		handle_special_case(t_dining_info *dining_info);
int		    report_error(char *message);
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

int main(int ac, char **av)
{
    t_dining_info dining_info;

    if (initialize_info(&dining_info, ac, av))
        return (0);
    if (initialize_philosophers(&dining_info))
        return (free_info(&dining_info));
    if (initialize_mutex(&dining_info))
        return (free_and_destroy(&dining_info));
    if (create_philosophers(&dining_info))
        return (0);
    destroy_resources(&dining_info);
    // return (0);
}

int check_death(t_philosopher *philosopher)
{
    pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
    if ((get_current_time() - philosopher->last_meal_time) >= philosopher->dining_info->time_to_die)
    {
        report_status(philosopher, "died");
        check_completion(philosopher, 1);
        pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
        return (1);
    }
    pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
    return (0);
}

// int check_death(t_philosopher *philosopher)
// {
//     pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
//     if ((get_current_time() - philosopher->last_meal_time) >= philosopher->dining_info->time_to_die)
//     {
//         report_status(philosopher, "died");
//         check_completion(philosopher, 1);
//         pthread_cond_signal(&death_cond);
//         pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
//         return (1);
//     }
//     pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
//     return (0);
// }

void eat(t_philosopher *philosopher)
{
    pthread_mutex_lock(&philosopher->dining_info->forks[philosopher->left_fork]);
    report_status(philosopher, "has taken a fork");
    pthread_mutex_lock(&philosopher->dining_info->forks[philosopher->right_fork]);
    report_status(philosopher, "has taken a fork");
    report_status(philosopher, "is eating");

    pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
    philosopher->last_meal_time = get_current_time();
    philosopher->num_of_meals++;
    pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);

    custom_sleep(philosopher, philosopher->dining_info->time_to_eat);

    pthread_mutex_unlock(&philosopher->dining_info->forks[philosopher->right_fork]);
    pthread_mutex_unlock(&philosopher->dining_info->forks[philosopher->left_fork]);

    check_completion(philosopher, 0);
}

// void eat(t_philosopher *philosopher)
// {
//     int first_fork = philosopher->left_fork < philosopher->right_fork ? philosopher->left_fork : philosopher->right_fork;
//     int second_fork = philosopher->left_fork < philosopher->right_fork ? philosopher->right_fork : philosopher->left_fork;

//     pthread_mutex_lock(&eating_mutex);
//     while (eating_count >= MAX_EATING)
//     {
//         pthread_mutex_unlock(&eating_mutex);
//         usleep(1000);
//         pthread_mutex_lock(&eating_mutex);
//     }
//     eating_count++;
//     pthread_mutex_unlock(&eating_mutex);

//     pthread_mutex_lock(&philosopher->dining_info->forks[first_fork]);
//     report_status(philosopher, "has taken a fork");
//     pthread_mutex_lock(&philosopher->dining_info->forks[second_fork]);
//     report_status(philosopher, "has taken a fork");
//     report_status(philosopher, "is eating");
    
//     pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
//     long long current_time = get_current_time();
//     long long time_since_last_meal = current_time - philosopher->last_meal_time;
    
//     if (time_since_last_meal > (philosopher->dining_info->time_to_die / 2))
//     {
//         pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
//         usleep(1000);
//         pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
//     }

//     philosopher->last_meal_time = current_time;
//     philosopher->num_of_meals++;
//     pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
    
//     custom_sleep(philosopher, philosopher->dining_info->time_to_eat);
    
//     pthread_mutex_unlock(&philosopher->dining_info->forks[second_fork]);
//     pthread_mutex_unlock(&philosopher->dining_info->forks[first_fork]);
    
//     pthread_mutex_lock(&eating_mutex);
//     eating_count--;
//     pthread_mutex_unlock(&eating_mutex);

//     check_completion(philosopher, 0);
// }


// void eat(t_philosopher *philosopher)
// {
//     int first_fork = philosopher->left_fork < philosopher->right_fork ? philosopher->left_fork : philosopher->right_fork;
//     int second_fork = philosopher->left_fork < philosopher->right_fork ? philosopher->right_fork : philosopher->left_fork;

//     pthread_mutex_lock(&eating_mutex);
//     while (eating_count >= MAX_EATING)
//     {
//         pthread_mutex_unlock(&eating_mutex);
//         usleep(1000);
//         pthread_mutex_lock(&eating_mutex);
//     }
//     eating_count++;
//     pthread_mutex_unlock(&eating_mutex);

//     pthread_mutex_lock(&philosopher->dining_info->forks[first_fork]);
//     report_status(philosopher, "has taken a fork");
//     pthread_mutex_lock(&philosopher->dining_info->forks[second_fork]);
//     report_status(philosopher, "has taken a fork");
//     report_status(philosopher, "is eating");
    
//     pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
//     long long current_time = get_current_time();
//     long long time_since_last_meal = current_time - philosopher->last_meal_time;
    
//     if (time_since_last_meal > (philosopher->dining_info->time_to_die / 2))
//     {
//         pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
//         usleep(1000);
//         pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
//     }

//     philosopher->last_meal_time = current_time;
//     philosopher->num_of_meals++;
//     pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
    
//     custom_sleep(philosopher, philosopher->dining_info->time_to_eat);
    
//     pthread_mutex_unlock(&philosopher->dining_info->forks[second_fork]);
//     pthread_mutex_unlock(&philosopher->dining_info->forks[first_fork]);
    
//     pthread_mutex_lock(&eating_mutex);
//     eating_count--;
//     pthread_mutex_unlock(&eating_mutex);

//     check_completion(philosopher, 0);
// }


int check_completion(t_philosopher *philosopher, int yes)
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

    // Check if all philosophers have eaten enough times
    if (philosopher->dining_info->num_must_eat > 0)
    {
        int all_ate_enough = 1;
        for (int i = 0; i < philosopher->dining_info->num_philosophers; i++)
        {
            if (philosopher->dining_info->philosophers[i].num_of_meals < philosopher->dining_info->num_must_eat)
            {
                all_ate_enough = 0;
                break;
            }
        }
        if (all_ate_enough)
        {
            philosopher->dining_info->finish = 1;
            pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
            printf("All philosophers have eaten at least %d times. Simulation ending.\n", philosopher->dining_info->num_must_eat);
            return (1);
        }
    }

    pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
    return (0);
}

// int check_completion(t_philosopher *philosopher, int yes)
// {
//     pthread_mutex_lock(&philosopher->dining_info->finish_mutex);
//     if (yes)
//     {
//         philosopher->dining_info->finish = 1;
//         pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
//         return (1);
//     }
//     if (philosopher->dining_info->finish)
//     {
//         pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
//         return (1);
//     }
    
//     if (philosopher->dining_info->num_must_eat > 0)
//     {
//         int all_ate_enough = 1;
//         pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
//         for (int i = 0; i < philosopher->dining_info->num_philosophers; i++)
//         {
//             if (philosopher->dining_info->philosophers[i].num_of_meals < philosopher->dining_info->num_must_eat)
//             {
//                 all_ate_enough = 0;
//                 break;
//             }
//         }
//         pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
//         if (all_ate_enough)
//         {
//             philosopher->dining_info->finish = 1;
//             pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
//             printf("All philosophers have eaten at least %d times. Simulation ending.\n", philosopher->dining_info->num_must_eat);
//             return (1);
//         }
//     }
    
//     pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
//     return (0);
// }


// int check_completion(t_philosopher *philosopher, int yes)
// {
//     pthread_mutex_lock(&philosopher->dining_info->finish_mutex);
//     if (yes)
//     {
//         philosopher->dining_info->finish = 1;
//         pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
//         return (1);
//     }
//     if (philosopher->dining_info->finish)
//     {
//         pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
//         return (1);
//     }
    
//     // Check if all philosophers have eaten enough times
//     if (philosopher->dining_info->num_must_eat > 0)
//     {
//         int all_ate_enough = 1;
//         for (int i = 0; i < philosopher->dining_info->num_philosophers; i++)
//         {
//             if (philosopher->dining_info->philosophers[i].num_of_meals < philosopher->dining_info->num_must_eat)
//             {
//                 all_ate_enough = 0;
//                 break;
//             }
//         }
//         if (all_ate_enough)
//         {
//             philosopher->dining_info->finish = 1;
//             pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
//             printf("All philosophers have eaten at least %d times. Simulation ending.\n", philosopher->dining_info->num_must_eat);
//             return (1);
//         }
//     }
    
//     pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
//     return (0);
// }

void *philosopher_thread_start(void *arg)
{
    t_philosopher *philosopher = (t_philosopher *)arg;
    int group = philosopher->identifier % 3;
    usleep(group * 1000); // Stagger start times

    if (philosopher->identifier % 2 == 0)
        usleep(philosopher->dining_info->time_to_eat * 100);
    while (!check_completion(philosopher, 0))
    {
        eat(philosopher);
        if (check_completion(philosopher, 0))
            break;
        report_status(philosopher, "is sleeping");
        custom_sleep(philosopher, philosopher->dining_info->time_to_sleep);
        if (check_completion(philosopher, 0))
            break;
        report_status(philosopher, "is thinking");
    }
    return (0);
}

int	initialize_info(t_dining_info *dining_info, int ac, char **av)
{
		if (ac < 5 || ac > 6)
			return (report_error("Error: Invalid number of arguments. Expected 5 or 6.\n"));
		dining_info->num_philosophers = ft_atoi_custom(av[1]);
		if (dining_info->num_philosophers > 200)
			return (report_error("Error: too many philosophers\n"));
		dining_info->time_to_die = ft_atoi_custom(av[2]);
		dining_info->time_to_eat = ft_atoi_custom(av[3]);
		dining_info->time_to_sleep = ft_atoi_custom(av[4]);
		if (dining_info->num_philosophers < 1 || dining_info->time_to_die < 1 || \
				dining_info->time_to_eat < 1 || dining_info->time_to_sleep < 1)
			return (report_error("Error: Invalid input. All parameters must be greater than 0.\n"));
		if (ac == 6)
		{
			dining_info->num_must_eat = ft_atoi_custom(av[5]);
			if (dining_info->num_must_eat < 1)
				return (report_error("Error: Invalid input. Number of meals must be greater than 0.\n"));
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
		dining_info->philosophers = malloc(sizeof(t_philosopher) * dining_info->num_philosophers);
		dining_info->forks = malloc(sizeof(pthread_mutex_t) * dining_info->num_philosophers);
		dining_info->start_time = get_current_time();
		if (!dining_info->philosophers || !dining_info->forks)
			return (report_error("Error: Failed to allocate memory for philosophers or forks.\n"));
		while (++i < dining_info->num_philosophers)
		{
			dining_info->philosophers[i].identifier = i + 1;
			dining_info->philosophers[i].left_fork = i;
			dining_info->philosophers[i].right_fork = (i + 1) % dining_info->num_philosophers;
			dining_info->philosophers[i].num_of_meals = 0;
			dining_info->philosophers[i].last_meal_time = dining_info->start_time;
			dining_info->philosophers[i].dining_info = dining_info;
		}
		return (0);
	}

int initialize_mutex(t_dining_info *dining_info)
{
    int i;

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
    if (pthread_mutex_init(&eating_mutex, NULL))
        return (report_error("Error: Failed to initialize eating mutex.\n"));
    if (pthread_cond_init(&death_cond, NULL))
        return (report_error("Error: Failed to initialize death condition variable.\n"));
    if (pthread_mutex_init(&death_mutex, NULL))
        return (report_error("Error: Failed to initialize death mutex.\n"));
    return (0);
}


// int initialize_mutex(t_dining_info *dining_info)
// {
//     int i;

//     i = -1;
//     while (++i < dining_info->num_philosophers)
//     {
//         if (pthread_mutex_init(&dining_info->forks[i], NULL))
//             return (report_error("Error: Failed to initialize fork mutex.\n"));
//     }
//     if (pthread_mutex_init(&dining_info->print_mutex, NULL))
//         return (report_error("Error: Failed to initialize print mutex.\n"));
//     if (pthread_mutex_init(&dining_info->meal_mutex, NULL))
//         return (report_error("Error: Failed to initialize meal mutex.\n"));
//     if (pthread_mutex_init(&dining_info->finish_mutex, NULL))
//         return (report_error("Error: Failed to initialize finish mutex.\n"));
//     if (pthread_mutex_init(&eating_mutex, NULL))
//         return (report_error("Error: Failed to initialize eating mutex.\n"));
//     if (pthread_cond_init(&death_cond, NULL))
//         return (report_error("Error: Failed to initialize death condition variable.\n"));
//     if (pthread_mutex_init(&death_mutex, NULL))
//         return (report_error("Error: Failed to initialize death mutex.\n"));
//     return (0);
// }

int create_philosophers(t_dining_info *dining_info)
{
    int i;

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
        if (pthread_create(&dining_info->philosophers[i].philosopher_thread, NULL, philosopher_thread_start, &(dining_info->philosophers[i])))
            return (report_error("Error: Failed to create philosopher threads.\n"));
    }
    return (0);
}

void handle_special_case(t_dining_info *dining_info)
{
    int i;

    while (1)
    {
        i = -1;
        while (++i < dining_info->num_philosophers)
        {
            eat(&dining_info->philosophers[i]);
            report_status(&dining_info->philosophers[i], "is sleeping");
            custom_sleep(&dining_info->philosophers[i], dining_info->time_to_sleep);
            report_status(&dining_info->philosophers[i], "is thinking");
        }
    }
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

void destroy_resources(t_dining_info *dining_info)
{
    pthread_mutex_lock(&dining_info->finish_mutex);
    // Perform resource cleanup
    for (int i = 0; i < dining_info->num_philosophers; i++)
    {
        pthread_mutex_destroy(&dining_info->forks[i]);
    }
    pthread_mutex_destroy(&dining_info->print_mutex);
    pthread_mutex_destroy(&dining_info->meal_mutex);
    pthread_mutex_destroy(&dining_info->finish_mutex);
    free(dining_info->philosophers);
    free(dining_info->forks);
    pthread_mutex_unlock(&dining_info->finish_mutex);
}

// void destroy_resources(t_dining_info *dining_info)
// {
//     pthread_mutex_lock(&death_mutex);
//     while (!dining_info->finish)
//     {
//         pthread_cond_wait(&death_cond, &death_mutex);
//         for (int i = 0; i < dining_info->num_philosophers; i++)
//         {
//             if (check_death(&dining_info->philosophers[i]))
//             {
//                 dining_info->finish = 1;
//                 break;
//             }
//         }
//         if (check_completion(&dining_info->philosophers[0], 0))
//         {
//             break;
//         }
//     }
//     pthread_mutex_unlock(&death_mutex);
//     join_free_and_destroy(dining_info);
// }

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

void	report_status(t_philosopher *philosopher, const char *str)
{
	long long	t;

	pthread_mutex_lock(&philosopher->dining_info->print_mutex);
	if (!check_completion(philosopher, 0))
	{
		t = get_current_time() - philosopher->dining_info->start_time;
		printf("[%lld] Philosopher: [%d] %s\n", t, philosopher->identifier, str);
	}
	pthread_mutex_unlock(&philosopher->dining_info->print_mutex);
	if (str[0] == 'f')
		printf("Philosophers Success\n");
}

void	custom_sleep(t_philosopher *philosopher, long long ms)
{
	long long	t;

	t = get_current_time();
	while (!check_completion(philosopher, 0) && (get_current_time() - t) < ms)
		usleep(600);
}