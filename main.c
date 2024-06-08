/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-bah <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 10:48:43 by elel-bah          #+#    #+#             */
/*   Updated: 2024/06/04 12:56:12 by elel-bah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


 #include "philo.h"

//  int    main(int ac, char **av)
//  {
//     t_data  *data;

//     data = (t_data *)malloc(sizeof(t_data));
//     if (!data)
//         return (0);
//     if (ac != 5 && ac != 6)
//         write(1, "hello\n", 6);
//         // return (1);
//     data->ac = ac;
//     data->av = av;
//     if (!parse_arguments(data))
//         return (1);
//     if (!ft_initialize_data(data))
//         return (1);
//     if (!ft_start_mutex(data))
//         return (1);
//     ft_initialize_philosophers(data);
//     if (!&ft_initialize_philosophers)
//         return (1);
//     if (!ft_create_philosophers(data))
//         return (1);
//     return (0);
// }


NAME = philo
CC = cc 
SRC = philo.c utils.c utils1.c utils2.c utils3.c
OBJ = $(SRC:.c=.o)
RM = rm -f
FLAGS = -Wall -Wextra -Werror -g -fsanitize=thread -O1

$(NAME): $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ)

%.o: %.c philo.h
	$(CC) $(FLAGS) -c $< -o $@

all: $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all


#include "philo.h"

long	get_time(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (1);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	ft_philo_init(t_data *d)
{
	int		i;
	t_philo	*philo;

	i = 0;
	while (i < d->num_philo)
	{
		philo = &d->philos[i];
		philo->id = i + 1;
		philo->meals_count = 0;
		philo->data = d;
		philo->full = false;
		philo->eating = false;
		philo->last_meal = get_time();
		pthread_mutex_init(&philo->eating_mtx, NULL);//
		pthread_mutex_init(&philo->philo_mtx, NULL);
		philo->right_f = &d->forks[i + 1];
		if (philo->id == d->num_philo)
			philo->right_f = &d->forks[0];
		philo->left_f = &d->forks[i];
		i++;
	}
}

int	ft_init_data(t_data	*d)
{
	int	i;

	i = 0;
	d->start_time = get_time();
	d->philos = malloc(sizeof(t_philo) * d->num_philo);
	if (!d->philos)
		return (1);
	d->forks = malloc(sizeof(t_fork) * d->num_philo);
	if (!d->forks)
		return (1);
	pthread_mutex_init(&d->mtx_write, NULL);
	pthread_mutex_init(&d->death_mtx, NULL);
	d->death = false;
	d->all_full = false;
	while (i < d->num_philo)
	{
		pthread_mutex_init(&d->forks[i].fork_mtx, NULL);
		d->forks[i].fork_id = i;
		i++;
	}
	ft_philo_init(d);
	return (0);
}

int	main(int arc, char **arv)
{
	int		i;
	t_data	data;

	if (arc == 5 || arc == 6)
	{
		if (parsing(arc, arv, &data) == 1)
			return (write(2, "wrong input\n", 13), 5);
		if (data.meals_num == 0)
			return (0);
		if (ft_init_data(&data) == 1)
			return (write(2, "malloc error\n", 13), 6);
		ft_philo_start(&data);
		i = 0;
		ft_clean_up(&data);
	}
	else
		printf("number of argement not correct\n");
}

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <stdbool.h>
# include <sys/time.h>

typedef struct s_data	t_data;

// typedef struct s_waiter {
//     int num_forks;
//     pthread_mutex_t waiter_mtx;
//     int available_forks;
// 	pthread_cond_t cond;
// 	 // Number of currently available forks
// } t_waiter;


typedef struct s_fork
{
	int				fork_id;
	pthread_mutex_t	fork_mtx;
}	t_fork;

typedef struct s_philo
{
	long			id;
	bool			full;
	t_data			*data;
	bool			eating;
	t_fork			*left_f;
	t_fork			*right_f;
	long			last_meal;
	pthread_mutex_t	philo_mtx;
	int				meals_count;
	pthread_t		philo_thread;
	pthread_mutex_t eating_mtx;//
	// long            waiting_time;//
}	t_philo;

typedef struct s_monitor
{
    t_philo *philo;
    t_data *data;
    pthread_t thread;
} t_monitor;
struct s_data
{
	long			ttd;
	long			tte;
	long			tts;
	bool			all_full;
	bool			death;
	t_fork			*forks;
	t_philo			*philos;
	pthread_t		monitor;
	long			num_philo;
	long			meals_num;
	pthread_mutex_t	mtx_write;
	pthread_mutex_t	death_mtx;
	long			start_time;//
	bool			should_continue;//
	// t_waiter 		waiter;
};

// void release_forks(int id, t_waiter *waiter);
int		cheack_dead(t_data	*data);//
int		philo_dead(t_philo *philo, long time_to_die);//
long	get_time(void);
long	ft_atol(char *s);
void	*monitor(void	*enf);
void	ft_clean_up(t_data *data);
void	ft_my_usleep(long to_sleep);
void	ft_philo_start(t_data *data);
void	ft_error_find(int i, t_data *data);
void	ft_get_input(char **arv, t_data *d);
int		parsing(int arc, char **arv, t_data *d);
void	write_philo_actions(t_philo *philo, char *action);
void	ft_creat_join_my_monitor(int what_to_do, pthread_t	*thread,
			t_data *data);

#endif

void	ft_get_input(char **arv, t_data *d)
{
	d->num_philo = ft_atol(arv[1]);
	d->ttd = ft_atol(arv[2]);
	d->tte = ft_atol(arv[3]);
	d->tts = ft_atol(arv[4]);
	if (arv[5])
		d->meals_num = ft_atol(arv[5]);
	else
		d->meals_num = -1;
}

long	max_min_handel(long n)
{
	if (n > 2147483647)
		return (-1);
	return (n);
}

long	ft_atol(char *s)
{
	long	r;
	long	sing;
	long	tmp;
	int		i;

	r = 0;
	sing = 1;
	i = 0;
	if (s[i] == '-' || s[i] == '+')
	{
		if (s[i] == '-')
			sing = -1;
		i++;
	}
	while (s[i] >= '0' && s[i] <= '9')
	{
		tmp = r;
		r = r * 10 + (s[i] - 48);
		if (tmp > r)
			max_min_handel(tmp);
		i++;
	}
	return (max_min_handel(r * sing));
}

int	check_args(char **arv)
{
	int	i;
	int	j;

	i = 1;
	while (arv[i])
	{
		j = 0;
		while (arv[i][j] == ' ' )
			j++;
		if (arv[i][j] == '+')
			j++;
		while (arv[i][j])
		{
			if (arv[i][j] >= '0' && arv[i][j] <= '9')
				j++;
			else
				return (0);
		}
		i++;
	}
	return (1);
}

int	parsing(int arc, char **arv, t_data *d)
{
	int	i;

	i = 2;
	if (check_args(arv) == 0)
		return (1);
	if (ft_atol(arv[1]) > 200 || ft_atol(arv[1]) < 1)
		return (1);
	while (i < 5)
	{
		if (ft_atol(arv[i]) < 60)
			return (1);
		i++;
	}
	if (arc == 6 && ft_atol(arv[5]) < 0)
		return (1);
	ft_get_input(arv, d);
	return (0);
}

// static bool acquire_forks(int id, t_waiter *waiter) {
// 	(void)id;
//     pthread_mutex_lock(&waiter->waiter_mtx);
//     if (waiter->available_forks >= 2) {
//         waiter->available_forks -= 2;
//         pthread_mutex_unlock(&waiter->waiter_mtx);
//         return true;
//     }
//     // ... wait for forks (using conditional variable)
//     pthread_mutex_unlock(&waiter->waiter_mtx);
//     return false;
// }

// void release_forks(int id, t_waiter *waiter) {
// 	(void)id;
//     pthread_mutex_lock(&waiter->waiter_mtx);
//     waiter->available_forks += 2;
//     // ... signal waiting philosophers (using conditional variable)
//     pthread_mutex_unlock(&waiter->waiter_mtx);
// }

void	ft_eat(t_philo *philo)
{
	pthread_mutex_lock(&philo->left_f->fork_mtx);
	write_philo_actions(philo, "has taken a fork");
	if (philo->data->num_philo == 1)
	{
		ft_my_usleep(philo->data->ttd);
		pthread_mutex_unlock(&philo->left_f->fork_mtx);
		return ;
	}
	pthread_mutex_lock(&philo->right_f->fork_mtx);
	write_philo_actions(philo, "has taken a fork");
	write_philo_actions(philo, "is eating");
	pthread_mutex_lock(&philo->eating_mtx);
	// pthread_mutex_lock(&philo->philo_mtx);
	philo->eating = true;
	pthread_mutex_unlock(&philo->eating_mtx);
	pthread_mutex_lock(&philo->philo_mtx);
	philo->last_meal = get_time();
	philo->meals_count++;
	pthread_mutex_unlock(&philo->philo_mtx);
	if (philo->data->meals_num > 0
		&& philo->meals_count == philo->data->meals_num)
		{
			philo->full = true;
			pthread_mutex_lock(&philo->data->mtx_write);
        	printf("-%ld- %ld has eaten %d times\n", get_time() - philo->data->start_time, philo->id, philo->meals_count);
        	pthread_mutex_unlock(&philo->data->mtx_write);
		}
	
	ft_my_usleep(philo->data->tte);
	pthread_mutex_lock(&philo->eating_mtx);//
	philo->eating = false;
	pthread_mutex_unlock(&philo->eating_mtx);//
	pthread_mutex_unlock(&philo->left_f->fork_mtx);
	pthread_mutex_unlock(&philo->right_f->fork_mtx);
}

int	ft_cheack_full_or_dead(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->death_mtx);
	if (philo->data->death == true || philo->full == true)
		return (pthread_mutex_unlock(&philo->data->death_mtx), 1);
	pthread_mutex_unlock(&philo->data->death_mtx);
	return (0);
}

void	*rotine(void	*enf)
{
	t_philo	*philo;

	philo = (t_philo *)enf;
	if (philo->data->num_philo == 1)
		ft_eat(philo);
	else
	{
		if (philo->id % 2)
			ft_my_usleep(1);
		while (!ft_cheack_full_or_dead(philo))
		{
			ft_eat(philo);
			write_philo_actions(philo, "is sleeping");
			ft_my_usleep(philo->data->tts);
			write_philo_actions(philo, "is thinking");
		}
	}
	return (NULL);
}

void	ft_creat_join_my_philos(int what_to_do, pthread_t	*thread,
		t_philo *philo)
{
	if (what_to_do == 1)
	{
		if (pthread_create(thread, NULL, rotine, philo) != 0)
			ft_error_find(1, philo->data);
	}
	else if (what_to_do == 2)
	{
		if (pthread_join(*thread, NULL) != 0)
			ft_error_find(2, philo->data);
	}
}

void *monitor_thread(void *arg)
{
    t_monitor *monitor = (t_monitor *)arg;
    t_philo *philo = monitor->philo;
    t_data *data = monitor->data;

    while (!cheack_dead(data))
    {
        if (philo_dead(philo, data->ttd))
        {
            write_philo_actions(philo, "died");
            pthread_mutex_lock(&data->death_mtx);
            data->death = true;
            pthread_mutex_unlock(&data->death_mtx);
            break;
        }
        usleep(900); // Add a small delay to avoid busy waiting

    }

    return NULL;
}

// void *monitor_thread(void *arg)
// {
//     t_monitor *monitor = (t_monitor *)arg;
//     t_philo *philo = monitor->philo;
//     t_data *data = monitor->data;

//     while (!cheack_dead(data) && !philo->full)
//     {
//         if (philo_dead(philo, data->ttd))
//         {
//             write_philo_actions(philo, "died");
//             pthread_mutex_lock(&data->death_mtx);
//             data->death = true;
//             pthread_mutex_unlock(&data->death_mtx);
//             break;
//         }
//     }

//     return NULL;
// }

void ft_philo_start(t_data *data)
{
    int i;
    t_monitor *monitors;

    i = 0;
    monitors = malloc(sizeof(t_monitor) * data->num_philo);
    if (!monitors)
    {
        ft_error_find(3, data); // Add an error code for malloc failure
        return;
    }

    while (i < data->num_philo)
    {
        ft_creat_join_my_philos(1, &data->philos[i].philo_thread, &data->philos[i]);
        monitors[i].philo = &data->philos[i];
        monitors[i].data = data;
        if (pthread_create(&monitors[i].thread, NULL, monitor_thread, &monitors[i]) != 0)
            ft_error_find(1, data);
        i++;
    }

    i = 0;
    while (i < data->num_philo)
    {
        ft_creat_join_my_philos(2, &data->philos[i].philo_thread, &data->philos[i]);
        if (pthread_join(monitors[i].thread, NULL) != 0)
            ft_error_find(2, data);
        i++;
    }

    data->all_full = true;
    free(monitors);
}

// void ft_philo_start(t_data *data)
// {
//     int i;
//     t_monitor *monitors;

//     i = 0;
//     monitors = malloc(sizeof(t_monitor) * data->num_philo);
//     while (i < data->num_philo)
//     {
//         ft_creat_join_my_philos(1, &data->philos[i].philo_thread, &data->philos[i]);
//         monitors[i].philo = &data->philos[i];
//         monitors[i].data = data;
//         pthread_create(&monitors[i].thread, NULL, monitor_thread, &monitors[i]);
//         i++;
//     }

//     i = 0;
//     while (i < data->num_philo)
//     {
//         ft_creat_join_my_philos(2, &data->philos[i].philo_thread, &data->philos[i]);
//         pthread_join(monitors[i].thread, NULL);
//         i++;
//     }

//     data->all_full = true;
//     free(monitors);
// }

// void	ft_philo_start(t_data *data)
// {
// 	int	i;

// 	i = 0;
// 	ft_creat_join_my_monitor(1, &data->monitor, data);
// 	while (i < data->num_philo)
// 	{
// 		ft_creat_join_my_philos(1, &data->philos[i].philo_thread,
// 			&data->philos[i]);
// 		i++;
// 	}
// 	i = 0;
// 	while (i < data->num_philo)
// 	{
// 		ft_creat_join_my_philos(2, &data->philos[i].philo_thread,
// 			&data->philos[i]);
// 		i++;
// 	}
// 	data->all_full = true;
// 	ft_creat_join_my_monitor(2, &data->monitor, data);
// }


void ft_error_find(int i, t_data *data)
{
    if (i == 1)
    {
        write(2, "can't creat thread\n", 20);
        ft_clean_up(data);
        exit(1);
    }
    else if (i == 2)
    {
        write(2, "can't join thread\n", 19);
        ft_clean_up(data);
        exit(1);
    }
    else if (i == 3)
    {
        write(2, "malloc failed\n", 15);
        ft_clean_up(data);
        exit(1);
    }
}

// void	ft_error_find(int i, t_data *data)
// {
// 	if (i == 1)
// 	{
// 		write(2, "can't creat thread\n", 20);
// 		ft_clean_up(data);
// 		exit(1);
// 	}
// 	else if (i == 2)
// 	{
// 		write(2, "can't join thread\n", 19);
// 		ft_clean_up(data);
// 		exit(1);
// 	}
// }

void	ft_clean_up(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_philo)
	{
		pthread_mutex_destroy(&data->forks[i].fork_mtx);
		pthread_mutex_destroy(&data->philos[i].philo_mtx);
		i++;
	}
	pthread_mutex_destroy(&data->death_mtx);
	pthread_mutex_destroy(&data->mtx_write);
	free(data->philos);
	free(data->forks);
}

void	ft_my_usleep(long to_sleep_in_usec)
{
	long	start_time;

	start_time = get_time();
	while ((get_time() - start_time) < to_sleep_in_usec)
	{
		usleep(500);
	}
}

int	cheack_dead(t_data	*data)
{
	pthread_mutex_lock(&data->death_mtx);
	if (data->death == true)
		return (pthread_mutex_unlock(&data->death_mtx), 1);
	pthread_mutex_unlock(&data->death_mtx);
	return (0);
}

void	write_philo_actions(t_philo *philo, char *action)
{
	// (void)action;
	long	actoin_time;

	pthread_mutex_lock(&philo->data->mtx_write);
	if (cheack_dead(philo->data) == 1 || philo->full)
	{
		pthread_mutex_unlock(&philo->data->mtx_write);
		return ;
	}
	actoin_time = get_time() - philo->data->start_time;
	printf("[%ld] Philosopher: [%ld] %s\n", actoin_time, philo->id, action);
	pthread_mutex_unlock(&philo->data->mtx_write);
}

int	philo_dead(t_philo *philo, long time_to_die)//
{
	pthread_mutex_lock(&philo->philo_mtx);
	if (get_time() - philo->last_meal >= time_to_die
		&& philo->eating != true)
		return (pthread_mutex_unlock(&philo->philo_mtx), 1);
	pthread_mutex_unlock(&philo->philo_mtx);
	return (0);
}

int	ft_check_dead(t_data	*data)
{
	int	i;

	i = 0;
	while (i < data->num_philo)
	{
		if (philo_dead(&data->philos[i], data->ttd))
		{
			write_philo_actions(&data->philos[i], "died");
			pthread_mutex_lock(&data->death_mtx);
			data->death = true;
			pthread_mutex_unlock(&data->death_mtx);
			return (1);
		}
		i++;
	}
	return (0);
}

void	*monitor(void	*enf)
{
	// int		i;
	t_data	*data;

	// pthread_cond_t cond;
	data = (t_data *)enf;
	while (1)
	{
		if (ft_check_dead(data) == 1 || data->all_full == true)
			break ;
	}
	return (NULL);
}

// void *monitor(void *enf) {
//     t_data *data = (t_data *)enf;
//     pthread_cond_init(&data->waiter.cond, NULL); // Initialize conditional variable

//     while (1) {
//         if (ft_check_dead(data) == 1 || data->all_full == true) {
//             break;
//         }
//     }

//     return NULL;
// }

void	ft_creat_join_my_monitor(int what_to_do, pthread_t	*thread,
		t_data *data)
{
	if (what_to_do == 1)
	{
		if (pthread_create(thread, NULL, monitor, data) != 0)
			ft_error_find(1, data);
	}
	else if (what_to_do == 2)
	{
		if (pthread_join(*thread, NULL) != 0)
			ft_error_find(2, data);
	}
}

