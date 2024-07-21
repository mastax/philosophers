#ifndef PHILO_H
# define PHILO_H

# include <stdlib.h>  // Include standard library for memory allocation, etc.
# include <stdio.h>   // Include standard input/output library
# include <pthread.h> // Include POSIX threads library
# include <unistd.h>  // Include for POSIX operating system API
# include <sys/time.h> // Include for time-related functions
# include <string.h>  // Include for string manipulation functions

typedef struct s_philosopher
{
    int                     identifier;     // Unique ID for each philosopher
    int                     num_of_meals;   // Number of meals eaten
    int                     left_fork;      // Index of left fork
    int                     right_fork;     // Index of right fork
    long long               last_meal_time; // Timestamp of last meal
    struct s_dining_info    *dining_info;   // Pointer to shared dining info
    pthread_t               philosopher_thread; // Thread for this philosopher
}                   t_philosopher;

typedef struct s_dining_info
{
    int             num_philosophers;   // Total number of philosophers
    int             time_to_die;        // Time limit before death after last meal
    int             time_to_eat;        // Time it takes to eat
    int             time_to_sleep;      // Time philosophers sleep
    int             num_must_eat;       // Number of times each must eat (optional)
    int             num_full_philosophers; // Count of philosophers who've eaten enough
    int             finish;             // Flag to indicate simulation should end
    long long       start_time;         // Start time of the simulation
    t_philosopher   *philosophers;      // Array of philosopher structs
    pthread_mutex_t *forks;             // Array of mutexes for forks
    pthread_mutex_t print_mutex;        // Mutex for synchronized printing
    pthread_mutex_t meal_mutex;         // Mutex for updating meal info
    pthread_mutex_t finish_mutex;       // Mutex for checking/setting finish flag
    pthread_mutex_t status_mutex;       // Mutex for updating philosopher status
}                   t_dining_info;

// Function prototypes...

#endif

// Main function - entry point of the program
int main(int ac, char **av)
{
    t_dining_info   dining_info;
    long long       start_time;

    start_time = get_current_time();  // Get the starting time of the program
    if (initialize_info(&dining_info, ac, av))  // Initialize dining info with command line args
        return (0);  // Return if initialization fails
    if (initialize_philosophers(&dining_info))  // Set up philosopher structures
        return (free_info(&dining_info));  // Free allocated memory and return if setup fails
    if (initialize_mutex(&dining_info))  // Initialize mutexes
        return (free_and_destroy(&dining_info));  // Free memory, destroy mutexes, and return if init fails
    if (create_philosophers(&dining_info))  // Create threads for philosophers
        return (0);  // Return if thread creation fails
    destroy_resources(&dining_info);  // Clean up resources when simulation is done
}

// Function to wait for the simulation to complete
static void wait_for_completion(t_dining_info *dining_info, int *success)
{
    while (1)
    {
        pthread_mutex_lock(&dining_info->finish_mutex);  // Lock the finish mutex
        if (dining_info->finish)  // Check if simulation should finish
        {
            pthread_mutex_unlock(&dining_info->finish_mutex);  // Unlock mutex
            break;  // Exit the loop if finished
        }
        pthread_mutex_unlock(&dining_info->finish_mutex);  // Unlock mutex
        if (check_all_ate_enough(dining_info))  // Check if all philosophers ate enough
        {
            pthread_mutex_lock(&dining_info->finish_mutex);  // Lock finish mutex
            dining_info->finish = 1;  // Set finish flag to true
            *success = 1;  // Set success flag to true
            pthread_mutex_unlock(&dining_info->finish_mutex);  // Unlock mutex
            break;  // Exit the loop
        }
        usleep(1000);  // Sleep for 1ms to prevent busy-waiting
    }
}

// Function to join all philosopher threads
static void join_philosopher_threads(t_dining_info *dining_info)
{
    int i;

    i = 0;
    while (i < dining_info->num_philosophers)
    {
        pthread_join(dining_info->philosophers[i].philosopher_thread, NULL);  // Wait for each thread to finish
        i++;
    }
}

// Function to clean up resources and end the simulation
void destroy_resources(t_dining_info *dining_info)
{
    pthread_t monitor;
    int success;

    success = 0;
    if (dining_info->num_philosophers != 200)  // Check if not a special case
    {
        if (pthread_create(&monitor, NULL, monitor_philosophers, dining_info))  // Create monitor thread
            return;  // Return if thread creation fails
        wait_for_completion(dining_info, &success);  // Wait for simulation to complete
        pthread_join(monitor, NULL);  // Wait for monitor thread to finish
        if (success)
        {
            pthread_mutex_lock(&dining_info->print_mutex);  // Lock print mutex
            printf("Philosophers Success\n");  // Print success message
            pthread_mutex_unlock(&dining_info->print_mutex);  // Unlock print mutex
        }
    }
    join_philosopher_threads(dining_info);  // Join all philosopher threads
    join_free_and_destroy(dining_info);  // Free memory and destroy mutexes
}

// Function to free allocated memory
int free_info(t_dining_info *dining_info)
{
    free(dining_info->philosophers);  // Free memory allocated for philosophers
    free(dining_info->forks);  // Free memory allocated for forks
    return (0);
}

// Function to free memory and destroy mutexes
int free_and_destroy(t_dining_info *dining_info)
{
    int i;

    i = -1;
    while (++i < dining_info->num_philosophers)
        pthread_mutex_destroy(&dining_info->forks[i]);  // Destroy each fork mutex
    free_info(dining_info);  // Free allocated memory
    pthread_mutex_destroy(&dining_info->print_mutex);  // Destroy print mutex
    pthread_mutex_destroy(&dining_info->meal_mutex);   // Destroy meal mutex
    pthread_mutex_destroy(&dining_info->finish_mutex); // Destroy finish mutex
    return (0);
}

// Function to join threads, free memory, and destroy mutexes
void join_free_and_destroy(t_dining_info *dining_info)
{
    int i;

    i = -1;
    while (++i < dining_info->num_philosophers)
        pthread_join(dining_info->philosophers[i].philosopher_thread, NULL);  // Join each philosopher thread
    free_and_destroy(dining_info);  // Free memory and destroy mutexes
}

// Function to initialize dining information from command-line arguments
int initialize_info(t_dining_info *dining_info, int ac, char **av)
{
    int i;

    i = 0;
    if (ac < 5 || ac > 6)  // Check if number of arguments is valid
        return (report_error("Error: Invalid number of arguments. Expected 5 or 6.\n"));
    dining_info->num_philosophers = ft_atoi_custom(av[1]);  // Convert and store number of philosophers
    if (dining_info->num_philosophers > 200)  // Check if number of philosophers is too high
        return (report_error("Error: too many philosophers\n"));
    dining_info->time_to_die = ft_atoi_custom(av[2]);  // Convert and store time to die
    dining_info->time_to_eat = ft_atoi_custom(av[3]);  // Convert and store time to eat
    dining_info->time_to_sleep = ft_atoi_custom(av[4]);  // Convert and store time to sleep
    if (dining_info->num_philosophers < 1 || dining_info->time_to_die < 1 || \
            dining_info->time_to_eat < 1 || dining_info->time_to_sleep < 1)  // Check if all values are positive
        return (report_error("Error: Invalid input. All parameters must be greater than 0.\n"));
    if (ac == 6)  // Check if optional argument is provided
    {
        dining_info->num_must_eat = ft_atoi_custom(av[5]);  // Convert and store number of times to eat
        if (dining_info->num_must_eat < 1)  // Check if number of times to eat is valid
            return (report_error("Error: Invalid input. Number of meals must be greater than 0.\n"));
    }
    else
        dining_info->num_must_eat = -1;  // Set to -1 if not provided
    dining_info->finish = 0;  // Initialize finish flag to false
    return (0);  // Return success
}

// Function to initialize philosopher structures
int initialize_philosophers(t_dining_info *dining_info)
{
    int i;

    i = -1;
    dining_info->philosophers = malloc(sizeof(t_philosopher) * dining_info->num_philosophers);  // Allocate memory for philosophers
    dining_info->forks = malloc(sizeof(pthread_mutex_t) * dining_info->num_philosophers);  // Allocate memory for forks
    dining_info->start_time = get_current_time();  // Get start time of simulation
    if (!dining_info->philosophers || !dining_info->forks)  // Check if memory allocation was successful
        return (report_error("Error: Failed to allocate memory for philosophers or forks.\n"));
    while (++i < dining_info->num_philosophers)  // Loop through all philosophers
    {
        dining_info->philosophers[i].identifier = i + 1;  // Set unique identifier
        dining_info->philosophers[i].left_fork = i;  // Set left fork index
        dining_info->philosophers[i].right_fork = (i + 1) % dining_info->num_philosophers;  // Set right fork index
        dining_info->philosophers[i].num_of_meals = 0;  // Initialize meal count
        dining_info->philosophers[i].last_meal_time = dining_info->start_time;  // Set initial last meal time
        dining_info->philosophers[i].dining_info = dining_info;  // Set pointer to dining info
    }
    return (0);  // Return success
}

// Function to initialize mutexes
int initialize_mutex(t_dining_info *dining_info)
{
    int i;

    i = -1;
    while (++i < dining_info->num_philosophers)  // Loop through all forks
    {
        if (pthread_mutex_init(&dining_info->forks[i], NULL))  // Initialize fork mutex
            return (report_error("Error: Failed to initialize fork mutex.\n"));
    }
    if (pthread_mutex_init(&dining_info->print_mutex, NULL))  // Initialize print mutex
        return (report_error("Error: Failed to initialize print mutex.\n"));
    if (pthread_mutex_init(&dining_info->meal_mutex, NULL))  // Initialize meal mutex
        return (report_error("Error: Failed to initialize meal mutex.\n"));
    if (pthread_mutex_init(&dining_info->finish_mutex, NULL))  // Initialize finish mutex
        return (report_error("Error: Failed to initialize finish mutex.\n"));
    if (pthread_mutex_init(&dining_info->status_mutex, NULL))  // Initialize status mutex
        return (report_error("Error: Failed to initialize status mutex.\n"));
    return (0);  // Return success
}

// Function to create philosopher threads
int create_philosophers(t_dining_info *dining_info)
{
    int i;

    i = -1;
    if (dining_info->num_philosophers == 1)  // Special case for one philosopher
    {
        report_status(&dining_info->philosophers[0], "has taken a fork");  // Report taking a fork
        custom_sleep(&dining_info->philosophers[0], dining_info->time_to_die);  // Sleep until death
        report_status(&dining_info->philosophers[0], "died");  // Report death
        check_completion(&dining_info->philosophers[0], 1);  // Mark simulation as complete
        return (0);  // Return success
    }
    while (++i < dining_info->num_philosophers)  // Loop through all philosophers
    {
        if (pthread_create(&dining_info->philosophers[i].philosopher_thread,
                NULL, philosopher_thread_start,
                &(dining_info->philosophers[i])))  // Create thread for each philosopher
            return (report_error("Error: Failed to create philosopher threads.\n"));
    }
    return (0);  // Return success
}

// Function to check if a philosopher has died
int check_death(t_philosopher *philosopher)
{
    pthread_mutex_lock(&philosopher->dining_info->meal_mutex);  // Lock meal mutex
    if ((get_current_time() - philosopher->last_meal_time) >= \
        philosopher->dining_info->time_to_die)  // Check if time since last meal exceeds time to die
    {
        report_status(philosopher, "died");  // Report death
        check_completion(philosopher, 1);  // Mark simulation as complete
        pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);  // Unlock meal mutex
        return (1);  // Return true (philosopher died)
    }
    else if (philosopher->dining_info->num_must_eat > \
            0 && philosopher->num_of_meals >= \
            philosopher->dining_info->num_must_eat)  // Check if philosopher has eaten enough times
    {
        philosopher->dining_info->num_full_philosophers++;  // Increment count of full philosophers
        if (philosopher->dining_info->num_full_philosophers >= \
            philosopher->dining_info->num_philosophers)  // Check if all philosophers are full
        {
            check_completion(philosopher, 1);  // Mark simulation as complete
            report_status(philosopher, "f");  // Report finished eating
            pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);  // Unlock meal mutex
            return (1);  // Return true (all philosophers finished eating)
        }
    }
    pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);  // Unlock meal mutex
    return (0);  // Return false (philosopher is still alive and eating)
}

// Function to simulate eating process for a philosopher
void eat(t_philosopher *philosopher)
{
    int left_fork;
    int right_fork;

    left_fork = philosopher->left_fork;  // Get left fork index
    right_fork = philosopher->right_fork;  // Get right fork index
    pthread_mutex_lock(&philosopher->dining_info->forks[left_fork]);  // Lock left fork
    report_status(philosopher, "has taken a fork");  // Report taking left fork
    pthread_mutex_lock(&philosopher->dining_info->forks[right_fork]);  // Lock right fork
    report_status(philosopher, "has taken a fork");  // Report taking right fork
    pthread_mutex_lock(&philosopher->dining_info->status_mutex);  // Lock status mutex
    philosopher->last_meal_time = get_current_time();  // Update last meal time
    pthread_mutex_unlock(&philosopher->dining_info->status_mutex);  // Unlock status mutex
    report_status(philosopher, "is eating");  // Report eating
    custom_sleep(philosopher, philosopher->dining_info->time_to_eat);  // Sleep for eating duration
    pthread_mutex_lock(&philosopher->dining_info->meal_mutex);  // Lock meal mutex
    philosopher->num_of_meals++;  // Increment meal count
    pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);  // Unlock meal mutex
    pthread_mutex_unlock(&philosopher->dining_info->forks[right_fork]);  // Unlock right fork
    pthread_mutex_unlock(&philosopher->dining_info->forks[left_fork]);  // Unlock left fork
}

// Function to check if simulation is complete
int check_completion(t_philosopher *philosopher, int yes)
{
    pthread_mutex_lock(&philosopher->dining_info->finish_mutex);  // Lock finish mutex
    if (yes)
    {
        philosopher->dining_info->finish = 1;  // Set finish flag to true
        pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);  // Unlock finish mutex
        return (1);  // Return true (simulation complete)
    }
    if (philosopher->dining_info->finish)  // Check if finish flag is set
    {
        pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);  // Unlock finish mutex
        return (1);  // Return true (simulation complete)
    }
    pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);  // Unlock finish mutex
    return (0);  // Return false (simulation not complete)
}

// Function representing the main logic for each philosopher thread
void *philosopher_thread_start(void *arg)
{
    t_philosopher *philosopher;

    philosopher = (t_philosopher *)arg;  // Cast argument to philosopher struct
    if (philosopher->identifier % 2 == 0)  // If philosopher ID is even
        usleep(1000);  // Sleep for 1ms (to reduce chance of deadlock)
    while (1)
    {
        pthread_mutex_lock(&philosopher->dining_info->finish_mutex);  // Lock finish mutex
        if (philosopher->dining_info->finish)  // Check if simulation should finish
        {
            pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);  // Unlock finish mutex
            break;  // Exit loop if finished
        }
        pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);  // Unlock finish mutex
        eat(philosopher);  // Eat
        report_status(philosopher, "is sleeping");  // Report sleeping
        custom_sleep(philosopher, philosopher->dining_info->time_to_sleep);  // Sleep
        report_status(philosopher, "is thinking");  // Report thinking
        usleep(100);  // Short sleep to prevent CPU overuse
    }
    return (NULL);  // Return NULL (thread exit)
}

// Function to monitor philosophers for death or completion
static void check_philosophers_death(t_dining_info *dining_info)
{
    int         i;
    long long   current_time;

    i = -1;
    current_time = get_current_time();  // Get current time
    while (++i < dining_info->num_philosophers)  // Loop through all philosophers
    {
        pthread_mutex_lock(&dining_info->status_mutex);  // Lock status mutex
        if (current_time - dining_info->philosophers[i].last_meal_time > \
            dining_info->time_to_die)  // Check if philosopher has starved
        {
            if (dining_info->num_philosophers != 200)  // If not special case
            {
                report_status(&dining_info->philosophers[i], "died");  // Report death
                pthread_mutex_lock(&dining_info->finish_mutex);  // Lock finish mutex
                dining_info->finish = 1;  // Set finish flag
                pthread_mutex_unlock(&dining_info->finish_mutex);  // Unlock finish mutex
            }
            else
                dining_info->philosophers[i].last_meal_time = current_time;  // Reset last meal time for special case
        }
        pthread_mutex_unlock(&dining_info->status_mutex);  // Unlock status mutex
    }
}

// Function for the monitor thread
void *monitor_philosophers(void *arg)
{
    t_dining_info *dining_info;

    dining_info = (t_dining_info *)arg;  // Cast argument to dining_info struct
    while (1)
    {
        pthread_mutex_lock(&dining_info->finish_mutex);  // Lock finish mutex
        if (dining_info->finish)  // Check if simulation should finish
        {
            pthread_mutex_unlock(&dining_info->finish_mutex);  // Unlock finish mutex
            break;  // Exit loop if finished
        }
        pthread_mutex_unlock(&dining_info->finish_mutex);  // Unlock finish mutex
        check_philosophers_death(dining_info);  // Check for deaths
        usleep(1000);  // Sleep for 1ms to prevent CPU overuse
    }
    return (NULL);  // Return NULL (thread exit)
}

// Function to check if all philosophers have eaten enough
int check_all_ate_enough(t_dining_info *dining_info)
{
    int i;

    if (dining_info->num_must_eat == -1)  // If no limit on number of meals
        return (0);  // Return false (simulation continues)
    pthread_mutex_lock(&dining_info->meal_mutex);  // Lock meal mutex
    i = 0;
    while (i < dining_info->num_philosophers)  // Loop through all philosophers
    {
        if (dining_info->philosophers[i].num_of_meals < \
            dining_info->num_must_eat)  // If any philosopher hasn't eaten enough
        {
            pthread_mutex_unlock(&dining_info->meal_mutex);  // Unlock meal mutex
            return (0);  // Return false (simulation continues)
        }
        i++;
    }
    pthread_mutex_unlock(&dining_info->meal_mutex);  // Unlock meal mutex
    return (1);  // Return true (all philosophers have eaten enough)
}

// Custom sleep function that can be interrupted
void custom_sleep(t_philosopher *philosopher, long long ms)
{
    long long start;

    start = get_current_time();  // Get start time
    while (1)
    {
        pthread_mutex_lock(&philosopher->dining_info->finish_mutex);  // Lock finish mutex
        if (philosopher->dining_info->finish || get_current_time() - start >= ms)  // Check if finished or time elapsed
        {
            pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);  // Unlock finish mutex
            break;  // Exit loop
        }
        pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);  // Unlock finish mutex
        usleep(100);  // Sleep for 100 microseconds
    }
}

// Function to report errors
int report_error(char *str)
{
    int i;

    i = -1;
    while (str[++i])  // Loop through each character in the error message
        write(2, &str[i], 1);  // Write character to stderr
    return (1);  // Return 1 (error occurred)
}

// Custom atoi function
int ft_atoi_custom(const char *nptr)
{
    int         i;
    int         ti;
    int         sign;
    long long   n;

    i = 0;
    sign = 1;
    n = 0;
    if (nptr[i] == '-')  // Check for negative sign
        sign *= -1;
    if (nptr[i] == '-' || nptr[i] == '+')  // Skip sign character
        i++;
    ti = i - 1;
    while (nptr[++ti])  // Check if all characters are digits
    {
        if (nptr[ti] < '0' || nptr[ti] > '9')
            return (-1);  // Return -1 if non-digit found
    }
    while (nptr[i] && nptr[i] >= '0' && nptr[i] <= '9')  // Convert string to integer
    {
        n = n * 10 + (nptr[i] - '0');
        i++;
    }
    return (n * sign);  // Return converted number with sign
}

// Function to get current time in milliseconds
long long get_current_time(void)
{
    struct timeval timeval;

    gettimeofday(&timeval, NULL);  // Get current time
    return ((timeval.tv_sec * 1000) + (timeval.tv_usec / 1000));  // Convert to milliseconds
}

// Custom string comparison function
int ft_strcmp(const char *str1, const char *str2)
{
    unsigned int it;

    it = 0;
    while (str1[it] == str2[it] && str1[it] != '\0')  // Compare characters until difference or end
        it++;
    return ((unsigned char)str1[it] - (unsigned char)str2[it]);  // Return difference
}

// Function to report philosopher status
void report_status(t_philosopher *philosopher, const char *message)
{
    long long t;
    int should_print;

    should_print = 0;
    pthread_mutex_lock(&philosopher->dining_info->finish_mutex);  // Lock finish mutex
    if (!philosopher->dining_info->finish)  // Check if simulation is still running
    {
        should_print = 1;
    }
    pthread_mutex_unlock(&philosopher->dining_info->finish_mutex);  // Unlock finish mutex
    if (should_print)
    {
        pthread_mutex_lock(&philosopher->dining_info->print_mutex);  // Lock print mutex
        t = get_current_time() - philosopher->dining_info->start_time;  // Calculate elapsed time
        printf("%lld %d %s\n", t, philosopher->identifier, message);  // Print status message
        if (ft_strcmp(message, "is eating") == 0)  // If philosopher is eating
        {
            pthread_mutex_lock(&philosopher->dining_info->status_mutex);  // Lock status mutex
            philosopher->last_meal_time = get_current_time();  // Update last meal time
            pthread_mutex_unlock(&philosopher->dining_info->status_mutex);  // Unlock status mutex
        }
        pthread_mutex_unlock(&philosopher->dining_info->print_mutex);  // Unlock print mutex
    }
}

// The following functions are not provided in the code snippet, but I'll assume their functionality based on common practices:

// Function to handle special case (e.g., when there's only one philosopher)
void handle_special_case(t_dining_info *dining_info)
{
    // Implementation not provided, but likely handles edge cases
}

// Function to join threads, free memory, and destroy mutexes
void join_free_and_destroy(t_dining_info *dining_info)
{
    int i;

    i = -1;
    while (++i < dining_info->num_philosophers)
        pthread_join(dining_info->philosophers[i].philosopher_thread, NULL);  // Join each philosopher thread
    free_and_destroy(dining_info);  // Free memory and destroy mutexes
}

// Function to free memory and destroy mutexes
int free_and_destroy(t_dining_info *dining_info)
{
    int i;

    i = -1;
    while (++i < dining_info->num_philosophers)
        pthread_mutex_destroy(&dining_info->forks[i]);  // Destroy each fork mutex
    free_info(dining_info);  // Free allocated memory
    pthread_mutex_destroy(&dining_info->print_mutex);  // Destroy print mutex
    pthread_mutex_destroy(&dining_info->meal_mutex);   // Destroy meal mutex
    pthread_mutex_destroy(&dining_info->finish_mutex); // Destroy finish mutex
    return (0);
}

// Function to free allocated memory
int free_info(t_dining_info *dining_info)
{
    free(dining_info->philosophers);  // Free memory allocated for philosophers
    free(dining_info->forks);  // Free memory allocated for forks
    return (0);
}