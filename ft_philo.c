#include "philo.h"

int	check_death(t_philosopher *philosopher)
{
	pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
	if ((get_current_time() - philosopher->last_meal_time) >= philosopher->dining_info->time_to_die)
	{
		report_status(philosopher, "died", get_current_time());
		check_completion(philosopher, 1);
		pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
		return (1);
	}
	else if (philosopher->dining_info->num_must_eat > 0 && philosopher->num_of_meals >= \
			philosopher->dining_info->num_must_eat)
	{
		philosopher->dining_info->num_full_philosophers++;
		if (philosopher->dining_info->num_full_philosophers >= philosopher->dining_info->num_philosophers)
		{
			check_completion(philosopher, 1);
			report_status(philosopher, "f", get_current_time());
			pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
			return (1);
		}
	}
	pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
	return (0);
}

static void eat(t_philosopher *philosopher)
{
    pthread_mutex_t *left_fork = &philosopher->dining_info->forks[philosopher->left_fork];
    pthread_mutex_t *right_fork = &philosopher->dining_info->forks[philosopher->right_fork];

    // Lock both forks
    pthread_mutex_lock(left_fork);
    pthread_mutex_lock(right_fork);

    report_status(philosopher, "has taken a fork", get_current_time());
    report_status(philosopher, "has taken a fork", get_current_time());
	philosopher->last_event_time = get_current_time();
    report_status(philosopher, "is eating", get_current_time());

    // Eat for the specified time
    custom_sleep(philosopher, philosopher->dining_info->time_to_eat);

    // Update philosopher's state
    pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
    philosopher->num_of_meals += 1;
    philosopher->last_meal_time = get_current_time();
    pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);

    // Release both forks
    pthread_mutex_unlock(right_fork);
    pthread_mutex_unlock(left_fork);
}


// static void	eat(t_philosopher *philosopher)
// {
// 	pthread_mutex_lock(&philosopher->dining_info->forks[philosopher->left_fork]);
// 	report_status(philosopher, "has taken a fork");
// 	pthread_mutex_lock(&philosopher->dining_info->forks[philosopher->right_fork]);
// 	report_status(philosopher, "has taken a fork");
// 	report_status(philosopher, "is eating");
// 	custom_sleep(philosopher, philosopher->dining_info->time_to_eat);
// 	pthread_mutex_lock(&philosopher->dining_info->meal_mutex);
// 	philosopher->num_of_meals += 1;
// 	philosopher->last_meal_time = get_current_time();
// 	pthread_mutex_unlock(&philosopher->dining_info->meal_mutex);
// 	pthread_mutex_unlock(&philosopher->dining_info->forks[philosopher->right_fork]);
// 	pthread_mutex_unlock(&philosopher->dining_info->forks[philosopher->left_fork]);
// }

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
		usleep(philosopher->dining_info->time_to_eat * 1000);
	while (42)
	{
		if (check_completion(philosopher, 0))
			return (0);
		eat(philosopher);
		report_status(philosopher, "is sleeping", get_current_time());
		custom_sleep(philosopher, philosopher->dining_info->time_to_sleep);
		report_status(philosopher, "is thinking", get_current_time());
	}
	return (0);
}