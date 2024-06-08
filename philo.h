/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-bah <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 10:55:07 by elel-bah          #+#    #+#             */
/*   Updated: 2024/06/04 12:56:41 by elel-bah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
#define PHILO_H

# include <limits.h>
# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdlib.h>

typedef struct x_philo
{
    int                 id_of_philo;
    int                 eat;//
    long long           last_meal;//
    pthread_mutex_t     *left_chopsticks;
    pthread_mutex_t     *right_chopsticks;
    pthread_t           thread_id;
    struct x_data       *data;
}                               t_philos;

typedef struct x_data
{
    int             ac;
    char            **av;
    int             nbr_philos;
    int             time_2_die;
    int             time_2_sleep;
    int             time_2_eat;
    int             must_eat;
    int             dead;
    int             all_are_ate;
    long long       birth;
    char            **arguments;
    char            *combined_args;
    t_philos        philo[200];
    int             total;
    long long       time;
    long long       norm;
    long long       norm2;
    t_philos        *n;
    pthread_mutex_t eat;
    pthread_mutex_t chopsticks[200];
    pthread_mutex_t output;
}                            t_data;

/* parsing functions*/
int     parse_arguments(t_data *data);
int     validate_arguments(char **arguments);
int     validate_plus(char **arguments);
int     validate_max(char **arg);
char    *ft_joining_args(char **av, int ac);
int     ft_is_digit(int k);
void	handle_error(void);
void    free_split(char **str);
void	*routine(void *ptr);

long long	ft_timestamp(void);
int	ft_start_mutex(t_data *data);
void	ft_output(t_data *data, int philo_id, char *str);
char    *ft_strjoin(char *s1, char *s2);
int	ft_strlen(char *str);
int	ft_initialize_data(t_data *data);
void	ft_initialize_philosophers(t_data *data);
int	ft_create_philosophers(t_data *data);
void	handle_error(void);
char	*ft_substr(char *s, int start, size_t len);
int	ft_atoi(const char *str);
long long	ft_period(long long past, long long present);
void	ft_sleep(long long time, t_data *data);
void	ft_eat(t_philos *philos);
void	ft_destroy(t_data *data);
int	ft_join_destroy(t_data *data);
int	ft_check_total_eat(t_data *data);
int	ft_check_dead(t_data *data);
/*helper functions*/
int     ft_atoi(const char *str);
char	*ft_substr(char *s, int start, size_t len);
char    **ft_split(char *s, char c);
int     ft_strlen(char *str);


#endif
