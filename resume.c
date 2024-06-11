#include "philosophers.h"


#ifndef PHILO_H
# define PHILO_H

# include <stdlib.h>
# include <stdio.h>
# include <pthread.h>
# include <unistd.h>
# include <sys/time.h>

typedef struct s_philosopher
{
	int				identifier; // Identifier for the philosopher
	int				num_of_meals; // Number of meals eaten by the philosopher
	int				left_fork; // Index of the left fork
	int				right_fork; // Index of the right fork
	long long		last_meal_time; // Time when the philosopher last ate
	struct s_dining_info	*dining_info; // Pointer to the dining information
	pthread_t		philosopher_thread; // Thread for the philosopher
}					t_philosopher;

typedef struct s_dining_info
{
	int				num_philosophers; // Total number of philosophers
	int				time_to_die; // Time in milliseconds after which a philosopher dies
	int				time_to_eat; // Time in milliseconds it takes for a philosopher to eat
	int				time_to_sleep; // Time in milliseconds a philosopher spends sleeping
	int				num_must_eat; // Number of meals each philosopher must eat (optional)
	int				num_full_philosophers; // Number of philosophers who have eaten enough meals
	int				finish; // Flag indicating if the simulation is finished
	long long		start_time; // Start time of the simulation
	t_philosopher			*philosophers; // Array of philosophers
	pthread_mutex_t	*forks; // Array of mutexes for the forks
	pthread_mutex_t	print_mutex; // Mutex for printing messages
	pthread_mutex_t	meal_mutex; // Mutex for meal-related operations
	pthread_mutex_t	finish_mutex; // Mutex for finishing the simulation
}					t_dining_info;

int		    report_error(char *message); // Function to report errors
int			ft_atoi_custom(const char *nptr); // Function to convert string to integer
long long	get_current_time(void); // Function to get the current time in milliseconds
void		report_status(t_philosopher *philosopher, const char *message); // Function to report the status of a philosopher
void		custom_sleep(t_philosopher *philosopher, long long ms); // Function for custom sleep

int			initialize_info(t_dining_info *dining_info, int ac, char **av); // Function to initialize dining information
int			initialize_philosophers(t_dining_info *dining_info); // Function to initialize philosophers
int			initialize_mutex(t_dining_info *dining_info); // Function to initialize mutexes
int			create_philosophers(t_dining_info *dining_info); // Function to create philosopher threads

int			check_death(t_philosopher *philosopher); // Function to check if a philosopher has died
int			check_completion(t_philosopher *philosopher, int yes); // Function to check if the simulation is complete
void		*philosopher_thread_start(void *arg); // Function to start the philosopher thread

int			free_info(t_dining_info *dining_info); // Function to free memory allocated for dining information
int			free_and_destroy(t_dining_info *dining_info); // Function to free memory and destroy mutexes
void		join_free_and_destroy(t_dining_info *dining_info); // Function to join threads, free memory, and destroy mutexes
void		destroy_resources(t_dining_info *dining_info); // Function to destroy resources

#endif


#include "philo.h"

int	main(int ac, char **av)
{
	t_dining_info	dining_info;

	// Initialize dining information
	if (initialize_info(&dining_info, ac, av))
		return (0);
	// Initialize philosophers
	if (initialize_philosophers(&dining_info))
		return (free_info(&dining_info));
	// Initialize mutexes
	if (initialize_mutex(&dining_info))
		return (free_and_destroy(&dining_info));
	// Create philosopher threads
	if (create_philosophers(&dining_info))
		return (0);
	// Destroy resources after simulation completes
	destroy_resources(&dining_info);
}

int	free_info(t_dining_info *dining_info)
{
	// Free memory allocated for philosophers and forks
	free(dining_info->philosophers);
	free(dining_info->forks);
	return (0);
}


int	free_and_destroy(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	// Destroy mutexes for each fork
	while (++i < dining_info->num_philosophers)
		pthread_mutex_destroy(&dining_info->forks[i]);
	// Free memory and destroy other mutexes
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
	// Wait for each philosopher thread to finish and then free resources
	while (++i < dining_info->num_philosophers)
		pthread_join(dining_info->philosophers[i].philosopher_thread, NULL);
	free_and_destroy(dining_info);
}


int	free_and_destroy(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	// Iterate over each philosopher's fork and destroy its mutex
	while (++i < dining_info->num_philosophers)
		pthread_mutex_destroy(&dining_info->forks[i]);
	// Free memory allocated for philosophers and forks
	free_info(dining_info);
	// Destroy mutexes for printing, meal, and finish
	pthread_mutex_destroy(&dining_info->print_mutex);
	pthread_mutex_destroy(&dining_info->meal_mutex);
	pthread_mutex_destroy(&dining_info->finish_mutex);
	return (0);
}

void	join_free_and_destroy(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	// Iterate over each philosopher thread and wait for its completion
	while (++i < dining_info->num_philosophers)
		pthread_join(dining_info->philosophers[i].philosopher_thread, NULL);
	// Free resources and destroy mutexes
	free_and_destroy(dining_info);
}

void	destroy_resources(t_dining_info *dining_info)
{
	int	i;
	int	yes;

	yes = 1;
	// Continue looping until all philosophers have finished
	while (yes)
	{
		i = -1;
		dining_info->num_full_philosophers = 0;
		// Check if any philosopher has died
		while (++i < dining_info->num_philosophers)
		{
			if (yes && check_death(&dining_info->philosophers[i]))
				yes = 0;
		}
		// Sleep briefly to reduce CPU usage
		usleep(10);
	}
	// Wait for philosopher threads to finish and then free resources
	join_free_and_destroy(dining_info);
}


int	initialize_info(t_dining_info *dining_info, int ac, char **av)
{
	int	i;

	i = 0;
	// Check if the number of arguments is valid
	if (ac < 5 || ac > 6)
		return (report_error("Error: Invalid number of arguments. Expected 5 or 6.\n"));
	// Set the number of philosophers
	dining_info->num_philosophers = ft_atoi_custom(av[1]);
    if (dining_info->num_philosophers > 200)
        return (report_error("Error: too many philosophers\n"));
	// Set other parameters
	dining_info->time_to_die = ft_atoi_custom(av[2]);
	dining_info->time_to_eat = ft_atoi_custom(av[3]);
	dining_info->time_to_sleep = ft_atoi_custom(av[4]);
	// Check if parameters are valid
	if (dining_info->num_philosophers < 1 || dining_info->time_to_die < 1 || \
			dining_info->time_to_eat < 1 || dining_info->time_to_sleep < 1)
		return (report_error("Error: Invalid input. All parameters must be greater than 0.\n"));
	// Set the number of meals each philosopher must eat
	if (ac == 6)
	{
		dining_info->num_must_eat = ft_atoi_custom(av[5]);
		if (dining_info->num_must_eat < 1)
			return (report_error("Error: Invalid input. Number of meals must be greater than 0.\n"));
	}
	else
		dining_info->num_must_eat = -1;
	// Initialize finish flag
	dining_info->finish = 0;
	return (0);
}

int	initialize_philosophers(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	// Allocate memory for philosophers and forks
	dining_info->philosophers = malloc(sizeof(t_philosopher) * dining_info->num_philosophers);
	dining_info->forks = malloc(sizeof(pthread_mutex_t) * dining_info->num_philosophers);
	dining_info->start_time = get_current_time();
	if (!dining_info->philosophers || !dining_info->forks)
		return (report_error("Error: Failed to allocate memory for philosophers or forks.\n"));
	// Initialize each philosopher
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

int	initialize_mutex(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	// Initialize mutexes for forks
	while (++i < dining_info->num_philosophers)
	{
		if (pthread_mutex_init(&dining_info->forks[i], NULL))
			return (report_error("Error: Failed to initialize fork mutex.\n"));
	}
	// Initialize other mutexes
	if (pthread_mutex_init(&dining_info->print_mutex, NULL))
		return (report_error("Error: Failed to initialize print mutex.\n"));
	if (pthread_mutex_init(&dining_info->meal_mutex, NULL))
		return (report_error("Error: Failed to initialize meal mutex.\n"));
	if (pthread_mutex_init(&dining_info->finish_mutex, NULL))
		return (report_error("Error: Failed to initialize finish mutex.\n"));
	return (0);
}
int	create_philosophers(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	// Handle case when there is only one philosopher
	if (dining_info->num_philosophers == 1)
	{
		report_status(&dining_info->philosophers[0], "has taken a fork");
		custom_sleep(&dining_info->philosophers[0], dining_info->time_to_die);
		report_status(&dining_info->philosophers[0], "died");
		check_completion(&dining_info->philosophers[0], 1);
		return (0);
	}
	// Create threads for each philosopher
	while (++i < dining_info->num_philosophers)
	{
		if (pthread_create(&dining_info->philosophers[i].philosopher_thread, NULL, philosopher_thread_start, \
			&(dining_info->philosophers[i])))
			return (report_error("Error: Failed to create philosopher threads.\n"));
	}
	return (0);
}
int	check_death(t_philosopher *philosopher)
{
	pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
	// Check if the philosopher has exceeded the time to die
	if ((get_current_time() - philosopher->last_meal_time) >= philosopher->dining_info->time_to_die)
	{
		report_status(philosopher, "died");
		check_completion(philosopher, 1);
		pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
		return (1);
	}
	// Check if the philosopher has eaten enough meals
	else if (philosopher->dining_info->num_must_eat > 0 && philosopher->num_of_meals >= \
			philosopher->dining_info->num_must_eat)
	{
		philosopher->dining_info->num_full_philosophers++;
		// Check if all philosophers have eaten enough meals
		if (philosopher->dining_info->num_full_philosophers >= philosopher->dining_info->num_philosophers)
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

static void	eat(t_philosopher *philosopher)
{
	// Lock the left fork mutex
	pthread_mutex_lock(&philosopher->dining_info->forks[philosopher->left_fork]);
	// Report that the philosopher has taken the left fork
	report_status(philosopher, "has taken a fork");
	// Lock the right fork mutex
	pthread_mutex_lock(&philosopher->dining_info->forks[philosopher->right_fork]);
	// Report that the philosopher has taken the right fork
	report_status(philosopher, "has taken a fork");
	// Report that the philosopher is eating
	report_status(philosopher, "is eating");
	// Sleep for the specified time_to_eat duration
	custom_sleep(philosopher, philosopher->dining_info->time_to_eat);
	// Lock the meal mutex to update meal count and last meal time
	pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
	philosopher->num_of_meals += 1;
	philosopher->last_meal_time = get_current_time();
	// Unlock the meal mutex
	pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
	// Unlock the right fork mutex
	pthread_mutex_unlock(&philosopher->dining_info->forks[philosopher->right_fork]);
	// Unlock the left fork mutex
	pthread_mutex_unlock(&philosopher->dining_info->forks[philosopher->left_fork]);
}

int	check_completion(t_philosopher *philosopher, int yes)
{
	// Lock the finish mutex
	pthread_mutex_lock(&philosopher->dining_info->finish_mutex);
	// If yes flag is set, mark simulation as finished and unlock finish mutex
	if (yes)
	{
		philosopher->dining_info->finish = 1;
		pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
		return (1);
	}
	// If simulation is already finished, unlock finish mutex and return
	if (philosopher->dining_info->finish)
	{
		pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
		return (1);
	}
	// Otherwise, unlock finish mutex and return
	pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
	return (0);
}

void	*philosopher_thread_start(void *arg)
{
	t_philosopher	*philosopher;

	// Cast argument to t_philosopher pointer
	philosopher = (t_philosopher *)arg;
	// If philosopher's identifier is even, delay eating to avoid deadlock
	if (philosopher->identifier % 2 == 0)
		usleep(philosopher->dining_info->time_to_eat * 1000);
	// Infinite loop representing philosopher's actions until simulation ends
	while (42)
	{
		// If simulation is completed, exit thread
		if (check_completion(philosopher, 0))
			return (0);
		// Perform eating action
		eat(philosopher);
		// Report that philosopher is sleeping
		report_status(philosopher, "is sleeping");
		// Sleep for the specified time_to_sleep duration
		custom_sleep(philosopher, philosopher->dining_info->time_to_sleep);
		// Report that philosopher is thinking
		report_status(philosopher, "is thinking");
	}
	// Return NULL when thread ends
	return (0);
}

int	report_error(char *str)
{
	int	i;

	i = -1;
	// Print error message character by character to standard error
	while (str[++i])
		write(2, &str[i], 1);
	// Return 1 to indicate an error occurred
	return (1);
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
	// Handle sign
	if (nptr[i] == '-')
		sign *= -1;
	if (nptr[i] == '-' || nptr[i] == '+')
		i++;
	ti = i - 1;
	// Check if the input is valid
	while (nptr[++ti])
	{
		if (nptr[ti] < '0' || nptr[ti] > '9')
			return (-1);
	}
	// Convert string to integer
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

	// Get current time in milliseconds
	gettimeofday(&timeval, NULL);
	return ((timeval.tv_sec * 1000) + (timeval.tv_usec / 1000));
}
void	report_status(t_philosopher *philosopher, const char *str)
{
	long long	t;

	pthread_mutex_lock(&philosopher->dining_info->print_mutex);
	// Print philosopher status if simulation is not finished
	if (!check_completion(philosopher, 0))
	{
		t = get_current_time() - philosopher->dining_info->start_time;
		printf("[%lld] Philosopher: [%d] %s\n", t, philosopher->identifier, str);
	}
	pthread_mutex_unlock(&philosopher->dining_info->print_mutex);
	// Print success message if all philosophers have eaten enough meals
	if (str[0] == 'f')
		printf("Philosophers Success\n");
}
void	custom_sleep(t_philosopher *philosopher, long long ms)
{
	long long	t;

	t = get_current_time();
	// Sleep until the specified time or until the simulation is finished
	while (!check_completion(philosopher, 0) && (get_current_time() - t) < ms)
		usleep(700);
}
