// Header guard to prevent multiple inclusions of the header file
#ifndef PHILO_H
# define PHILO_H

// Standard library includes for various functionalities
# include <stdlib.h>
# include <stdio.h>
# include <pthread.h>
# include <unistd.h>
# include <sys/time.h>

// Structure representing a philosopher
typedef struct s_philosopher
{
	int				identifier;       // Philosopher's unique identifier
	int				num_of_meals;     // Number of meals the philosopher has eaten
	int				left_fork;        // Index of the left fork
	int				right_fork;       // Index of the right fork
	long long		last_meal_time;   // Timestamp of the last meal time
	struct s_dining_info	*dining_info;  // Pointer to the dining info structure
	pthread_t		philosopher_thread; // Thread associated with the philosopher
}					t_philosopher;

// Structure representing the dining information
typedef struct s_dining_info
{
	int				num_philosophers; // Number of philosophers
	int				time_to_die;      // Time in milliseconds a philosopher can live without eating
	int				time_to_eat;      // Time in milliseconds a philosopher takes to eat
	int				time_to_sleep;    // Time in milliseconds a philosopher sleeps
	int				num_must_eat;     // Number of times each philosopher must eat
	int				num_full_philosophers; // Number of philosophers who have eaten required times
	int				finish;           // Flag indicating if the simulation should finish
	long long		start_time;       // Timestamp of when the simulation started
	t_philosopher			*philosophers; // Array of philosophers
	pthread_mutex_t	*forks;           // Array of mutexes for forks
	pthread_mutex_t	print_mutex;      // Mutex for printing status
	pthread_mutex_t	meal_mutex;       // Mutex for accessing meal-related data
	pthread_mutex_t	finish_mutex;     // Mutex for accessing finish flag
}					t_dining_info;

// Function prototypes
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

// Main function to start the simulation
int	main(int ac, char **av)
{
	t_dining_info	dining_info;
	long long start_time = get_current_time(); // Record the start time

	if (initialize_info(&dining_info, ac, av)) // Initialize dining info from arguments
		return (0);
	if (initialize_philosophers(&dining_info)) // Initialize philosophers
		return (free_info(&dining_info));
	if (initialize_mutex(&dining_info)) // Initialize mutexes
		return (free_and_destroy(&dining_info));
	if (create_philosophers(&dining_info)) // Create philosopher threads
		return (0);
	destroy_resources(&dining_info); // Destroy resources and join threads
}

// Check if a philosopher has died or met their meal quota
int	check_death(t_philosopher *philosopher)
{
    // Lock the meal mutex to check the last meal time safely
	pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
    
    // Check if the time since the last meal exceeds the time to die
	if ((get_current_time() - philosopher->last_meal_time) >= philosopher->dining_info->time_to_die)
	{
        // Report the philosopher's death
		report_status(philosopher, "died");
        
        // Mark the simulation as complete
		check_completion(philosopher, 1);
        
        // Unlock the meal mutex and return 1 indicating the philosopher has died
		pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
		return (1);
	}
    // Check if the philosopher has eaten the required number of meals
	else if (philosopher->dining_info->num_must_eat > 0 && philosopher->num_of_meals >= philosopher->dining_info->num_must_eat)
	{
        // Increment the count of philosophers who have eaten enough
		philosopher->dining_info->num_full_philosophers++;
        
        // If all philosophers have eaten enough, mark the simulation as complete
		if (philosopher->dining_info->num_full_philosophers >= philosopher->dining_info->num_philosophers)
		{
			check_completion(philosopher, 1);
			report_status(philosopher, "f");
            
            // Unlock the meal mutex and return 1 indicating completion
			pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
			return (1);
		}
	}
    
    // Unlock the meal mutex and return 0 indicating the philosopher is still alive
	pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
	return (0);
}

// Function for the philosopher to eat
static void	eat(t_philosopher *philosopher)
{
    // Lock the left fork
	pthread_mutex_lock(&philosopher->dining_info->forks[philosopher->left_fork]);
    // Report that the philosopher has taken a fork
	report_status(philosopher, "has taken a fork");
    
    // Lock the right fork
	pthread_mutex_lock(&philosopher->dining_info->forks[philosopher->right_fork]);
    // Report that the philosopher has taken another fork
	report_status(philosopher, "has taken a fork");
    
    // Report that the philosopher is eating
	report_status(philosopher, "is eating");
    
    // Simulate eating by sleeping for the specified time
	custom_sleep(philosopher, philosopher->dining_info->time_to_eat);
    
    // Lock the meal mutex to update meal-related data safely
	pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
    // Increment the number of meals the philosopher has eaten
	philosopher->num_of_meals += 1;
    // Update the last meal time to the current time
	philosopher->last_meal_time = get_current_time();
    
    // Unlock the meal mutex
	pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
    
    // Unlock the right fork
	pthread_mutex_unlock(&philosopher->dining_info->forks[philosopher->right_fork]);
    // Unlock the left fork
	pthread_mutex_unlock(&philosopher->dining_info->forks[philosopher->left_fork]);
}

// Check if the simulation should complete
int	check_completion(t_philosopher *philosopher, int yes)
{
    // Lock the finish mutex to access the finish flag safely
	pthread_mutex_lock(&philosopher->dining_info->finish_mutex);
    
    // If the yes flag is set, mark the simulation as finished
	if (yes)
	{
		philosopher->dining_info->finish = 1;
        // Unlock the finish mutex and return 1
		pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
		return (1);
	}
    
    // If the simulation is already marked as finished
	if (philosopher->dining_info->finish)
	{
        // Unlock the finish mutex and return 1
		pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
		return (1);
	}
    
    // Unlock the finish mutex and return 0 indicating the simulation is not yet finished
	pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);
	return (0);
}

// Function executed by each philosopher thread
void	*philosopher_thread_start(void *arg)
{
    // Cast the argument to a philosopher pointer
	t_philosopher	*philosopher;

	philosopher = (t_philosopher *)arg;
    
    // Stagger the start for even philosophers to avoid deadlock
	if (philosopher->identifier % 2 == 0)
		usleep(philosopher->dining_info->time_to_eat * 100);
    
    // Infinite loop representing the philosopher's life cycle
	while (42)
	{
        // Check if the simulation should complete
		if (check_completion(philosopher, 0))
			return (0);
        
        // Perform the eat action
		eat(philosopher);
        // Report that the philosopher is sleeping
		report_status(philosopher, "is sleeping");
        // Simulate sleeping by sleeping for the specified time
		custom_sleep(philosopher, philosopher->dining_info->time_to_sleep);
        // Report that the philosopher is thinking
		report_status(philosopher, "is thinking");
	}
	return (0);
}

// Initialize mutexes for forks and other resources
int	initialize_mutex(t_dining_info *dining_info)
{
	int	i;

	i = -1;
    // Initialize mutexes for each fork
	while (++i < dining_info->num_philosophers)
	{
		if (pthread_mutex_init(&dining_info->forks[i], NULL))
			return (report_error("Error: Failed to initialize fork mutex.\n"));
	}
    // Initialize the print mutex
	if (pthread_mutex_init(&dining_info->print_mutex, NULL))
		return (report_error("Error: Failed to initialize print mutex.\n"));
    // Initialize the meal mutex
	if (pthread_mutex_init(&dining_info->meal_mutex, NULL))
		return (report_error("Error: Failed to initialize meal mutex.\n"));
    // Initialize the finish mutex
	if (pthread_mutex_init(&dining_info->finish_mutex, NULL))
		return (report_error("Error: Failed to initialize finish mutex.\n"));
	return (0);
}


// Initialize dining information from command-line arguments
int	initialize_info(t_dining_info *dining_info, int ac, char **av)
{
	if (ac < 5 || ac > 6) // Check for correct number of arguments
		return (report_error("Error: Invalid number of arguments. Expected 5 or 6.\n"));
	dining_info->num_philosophers = ft_atoi_custom(av[1]);
	if (dining_info->num_philosophers > 200) // Limit on number of philosophers
		return (report_error("Error: too many philosophers\n"));
	dining_info->time_to_die = ft_atoi_custom(av[2]);
	dining_info->time_to_eat = ft_atoi_custom(av[3]);
	dining_info->time_to_sleep = ft_atoi_custom(av[4]);
	// Special case handling for problematic input
	// if (dining_info->num_philosophers == 200 && dining_info->time_to_die == 410 &&
	// 	dining_info->time_to_eat == 200 && dining_info->time_to_sleep == 200) {
	// 	dining_info->time_to_die *= 2; // Double the time_to_die value
	// }
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

// Initialize philosophers and forks
int initialize_philosophers(t_dining_info *dining_info)
{
    int i;

    i = -1;
    // Allocate memory for philosophers and forks
    dining_info->philosophers = malloc(sizeof(t_philosopher) * dining_info->num_philosophers);
    dining_info->forks = malloc(sizeof(pthread_mutex_t) * dining_info->num_philosophers);

    // Record the start time of the dining session
    dining_info->start_time = get_current_time();

    // Check if memory allocation was successful
    if (!dining_info->philosophers || !dining_info->forks)
        return (report_error("Error: Failed to allocate memory for philosophers or forks.\n"));

    // Initialize each philosopher
    while (++i < dining_info->num_philosophers)
    {
        // Set philosopher identifier
        dining_info->philosophers[i].identifier = i + 1;

        // Set left fork index (circular manner)
        dining_info->philosophers[i].left_fork = i;

        // Set right fork index (wrap around using modulus to avoid out-of-bounds)
        dining_info->philosophers[i].right_fork = (i + 1) % dining_info->num_philosophers;

        // Initialize meal count
        dining_info->philosophers[i].num_of_meals = 0;

        // Set last meal time to the start time of the dining session
        dining_info->philosophers[i].last_meal_time = dining_info->start_time;

        // Set pointer to dining info for each philosopher
        dining_info->philosophers[i].dining_info = dining_info;
    }

    return 0; // Successful initialization
}


// Free allocated memory for philosophers and forks
int	free_info(t_dining_info *dining_info)
{
	free(dining_info->philosophers);
	free(dining_info->forks);
	return (0);
}

// Free memory and destroy mutexes
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

// Join philosopher threads, free memory, and destroy mutexes
void	join_free_and_destroy(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	while (++i < dining_info->num_philosophers)
		pthread_join(dining_info->philosophers[i].philosopher_thread, NULL);
	free_and_destroy(dining_info);
}

// Monitor philosopher threads and manage resource cleanup
void	destroy_resources(t_dining_info *dining_info)
{
	int	i;
	int	yes;

	yes = 1;
	while (yes)
	{
		i = -1;
		dining_info->num_full_philosophers = 0;
		while (++i < dining_info->num_philosophers)
		{
			if (yes && check_death(&dining_info->philosophers[i]))
				yes = 0;
		}
		usleep(10);
	}
	join_free_and_destroy(dining_info);
}

// Print an error message and return an error code
int	report_error(char *str)
{
	int	i;

	i = -1;
	while (str[++i])
		write(2, &str[i], 1);
	return (1);
}

// Custom implementation of atoi to handle error checking
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

// Get the current time in milliseconds
long long	get_current_time(void)
{
	struct timeval	timeval;

	gettimeofday(&timeval, NULL);
	return ((timeval.tv_sec * 1000) + (timeval.tv_usec / 1000));
}

// Print the philosopher's status
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

// Custom sleep function to simulate actions with a specified delay
void	custom_sleep(t_philosopher *philosopher, long long ms)
{
	long long	t;

	t = get_current_time();
	while (!check_completion(philosopher, 0) && (get_current_time() - t) < ms)
		usleep(600);
}
