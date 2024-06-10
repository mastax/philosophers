#include "philosophers.h"

//  Main function
int	main(int ac, char **av)
{
	t_dining_info	dining_info;

	// Initialize dining information and start the simulation
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

int initialize_info(t_dining_info *dining_info, int ac, char **av)
{
    int i; // Declare a variable 'i' to be used as a counter
    
    i = 0; // Initialize 'i' to 0
    
    // Check for the correct number of command-line arguments
    if (ac < 5 || ac > 6)
        return (report_error("Error: Invalid number of arguments. Expected 5 or 6.\n")); // Return an error message if the number of arguments is not 5 or 6
    
    // Parse command-line arguments to initialize dining configuration
    dining_info->num_philosophers = ft_atoi_custom(av[1]); // Convert the first argument to an integer and assign it to the number of philosophers
    dining_info->time_to_die = ft_atoi_custom(av[2]); // Convert the second argument to an integer and assign it to the time to die
    dining_info->time_to_eat = ft_atoi_custom(av[3]); // Convert the third argument to an integer and assign it to the time to eat
    dining_info->time_to_sleep = ft_atoi_custom(av[4]); // Convert the fourth argument to an integer and assign it to the time to sleep
    
    // Check if input values are valid
    if (dining_info->num_philosophers < 1 || dining_info->time_to_die < 1 || \
            dining_info->time_to_eat < 1 || dining_info->time_to_sleep < 1)
        return (report_error("Error: Invalid input. All parameters must be greater than 0.\n")); // Return an error message if any of the input values are less than 1
    
    // Set the number of meals if provided as input
    if (ac == 6)
    {
        dining_info->num_must_eat = ft_atoi_custom(av[5]); // Convert the fifth argument to an integer and assign it to the number of meals
        if (dining_info->num_must_eat < 1)
            return (report_error("Error: Invalid input. Number of meals must be greater than 0.\n")); // Return an error message if the number of meals is less than 1
    }
	/* 
	In simpler terms, when ac is not equal to 6, 
	it means that the user didn't specify how many meals each philosopher must eat. 
	So, we set dining_info->num_must_eat to -1 to show that there's no specific number of meals required. 
This tells the program to run the simulation without stopping based on the number of meals eaten. 
Instead, it keeps going until something else, 
like a user stopping it, happens.*/
    else
        dining_info->num_must_eat = -1; // If the number of arguments is not 6, set the number of meals to -1
    
    dining_info->finish = 0; // Initialize the 'finish' flag to 0
    return (0); // Return 0 to indicate successful initialization
}

// Function to initialize philosophers
int initialize_philosophers(t_dining_info *dining_info)
{
    int i;

    i = -1;
    
    // Allocate memory for philosopher and fork structures
    dining_info->philosophers = malloc(sizeof(t_philosopher) * dining_info->num_philosophers);
    dining_info->forks = malloc(sizeof(pthread_mutex_t) * dining_info->num_philosophers);
    dining_info->start_time = get_current_time(); // Record the start time of the simulation
    
    // Check if memory allocation was successful
    if (!dining_info->philosophers || !dining_info->forks)
        return (report_error("Error: Failed to allocate memory for philosophers or forks.\n"));
    
    // Initialize each philosopher
    while (++i < dining_info->num_philosophers)
    {
        dining_info->philosophers[i].identifier = i + 1; // Assign a unique identifier to each philosopher
        dining_info->philosophers[i].left_fork = i; // Assign the left fork index to the philosopher
        dining_info->philosophers[i].right_fork = (i + 1) % dining_info->num_philosophers; // Assign the right fork index to the philosopher
        dining_info->philosophers[i].num_of_meals = 0; // Initialize the number of meals eaten by the philosopher to 0
        dining_info->philosophers[i].last_meal_time = dining_info->start_time; // Set the last meal time to the start time of the simulation
        dining_info->philosophers[i].dining_info = dining_info; // Set a reference to the dining information for each philosopher
    }
    
    return (0); // Return 0 to indicate successful initialization
}

// Function to initialize mutexes
int	initialize_mutex(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	// Initialize mutex for each fork
	while (++i < dining_info->num_philosophers)
	{
		if (pthread_mutex_init(&dining_info->forks[i], NULL))
			return (report_error("Error: Failed to initialize fork mutex.\n"));
	}
	// Initialize mutexes for printing, meal counting, and simulation completion
	if (pthread_mutex_init(&dining_info->print_mutex, NULL))
		return (report_error("Error: Failed to initialize print mutex.\n"));
	if (pthread_mutex_init(&dining_info->meal_mutex, NULL))
		return (report_error("Error: Failed to initialize meal mutex.\n"));
	if (pthread_mutex_init(&dining_info->finish_mutex, NULL))
		return (report_error("Error: Failed to initialize finish mutex.\n"));
	return (0);
}

// Function to create philosopher threads
int	create_philosophers(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	// Handle edge case if there is only one philosopher
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

// Function to free memory allocated for dining information
int	free_info(t_dining_info *dining_info)
{
	free(dining_info->philosophers);
	free(dining_info->forks);
	return (0);
}

// Function to free memory and destroy mutexes
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

// Function to join threads and perform cleanup
void	join_free_and_destroy(t_dining_info *dining_info)
{
	int	i;

	i = -1;
	// Join threads for each philosopher
	while (++i < dining_info->num_philosophers)
		pthread_join(dining_info->philosophers[i].philosopher_thread, NULL);
	// Free memory and destroy mutexes
	free_and_destroy(dining_info);
}

// Function to handle simulation completion and cleanup
void	destroy_resources(t_dining_info *dining_info)
{
	int	i;
	int	yes;

	yes = 1;
	// Wait for all philosophers to finish or die
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
		// Pause briefly
		usleep(10);
	}
	// Join threads and perform cleanup
	join_free_and_destroy(dining_info);
}

// Function to check if a philosopher has died
int	check_death(t_philosopher *philosopher)
{
	pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
	// Check if the time since the last meal exceeds the time limit
	if ((get_current_time() - philosopher->last_meal_time) >= philosopher->dining_info->time_to_die)
	{
		report_status(philosopher, "died");
		check_completion(philosopher, 1);
		pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
		return (1);
	}
	// Check if the philosopher has eaten the required number of meals
	else if (philosopher->dining_info->num_must_eat > 0 && philosopher->num_of_meals >= \
			philosopher->dining_info->num_must_eat)
	{
		philosopher->dining_info->num_full_philosophers++;
		// Check if all philosophers have completed eating
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

// Function representing the behavior of each philosopher thread
void	*philosopher_thread_start(void *arg)
{
	t_philosopher	*philosopher;

	philosopher = (t_philosopher *)arg;
	// If philosopher's ID is even, delay eating to avoid conflicts
	if (philosopher->identifier % 2 == 0)
		usleep(philosopher->dining_info->time_to_eat * 1000);
	// Main loop for philosopher's behavior
	while (42)
	{
		// Check if the simulation has completed
		if (check_completion(philosopher, 0))
			return (0);
		// Perform actions: eat, sleep, think
		eat(philosopher);
		report_status(philosopher, "is sleeping");
		custom_sleep(philosopher, philosopher->dining_info->time_to_sleep);
		report_status(philosopher, "is thinking");
	}
	return (0);
}

// Function to handle errors and report them
int	report_error(char *str)
{
	int	i;

	i = -1;
	while (str[++i])
		write(2, &str[i], 1);
	return (1);
}

// Function similar to atoi but with error handling
int	ft_atoi_custom(const char *nptr)
{
	int			i;
	int			ti;
	int			sign;
	long long	n;

	i = 0;
	sign = 1;
	n = 0;
	// Handle signs
	if (nptr[i] == '-')
		sign *= -1;
	if (nptr[i] == '-' || nptr[i] == '+')
		i++;
	// Check for invalid characters
	ti = i - 1;
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

// Function to get the current time
long long	get_current_time(void)
{
	struct timeval	timeval;

	gettimeofday(&timeval, NULL);
	return ((timeval.tv_sec * 1000) + (timeval.tv_usec / 1000));
}

// Function to report the current status of a philosopher
void	report_status(t_philosopher *philosopher, const char *str)
{
	long long	t;

	pthread_mutex_lock(&philosopher->dining_info->print_mutex);
	if (!check_completion(philosopher, 0))
	{
		// Calculate time since simulation start
		t = get_current_time() - philosopher->dining_info->start_time;
		// Print status message
		printf("%lld %d %s\n", t, philosopher->identifier, str);
	}
	pthread_mutex_unlock(&philosopher->dining_info->print_mutex);
	// Print success message if simulation completes successfully
	if (str[0] == 'f')
		printf("Philosophers Success\n");
}

// Function to simulate a sleep-like behavior with error handling
void	custom_sleep(t_philosopher *philosopher, long long ms)
{
	long long	t;

	t = get_current_time();
	while (!check_completion(philosopher, 0) && (get_current_time() - t) < ms)
		usleep(900);
}
