#include "philo.h"


void *monitor_philosophers(void *arg)
{
    t_dining_info *dining_info = (t_dining_info *)arg;
    int i;
    long long current_time;

    while (1)
    {
        pthread_mutex_lock(&dining_info->finish_mutex);
        if (dining_info->finish)
        {
            pthread_mutex_unlock(&dining_info->finish_mutex);
            break;
        }
        pthread_mutex_unlock(&dining_info->finish_mutex);

        i = -1;
        current_time = get_current_time();
        while (++i < dining_info->num_philosophers)
        {
            pthread_mutex_lock(&dining_info->status_mutex);
            if (current_time - dining_info->philosophers[i].last_meal_time >= dining_info->time_to_die)
            {
                report_status(&dining_info->philosophers[i], "died");
                pthread_mutex_lock(&dining_info->finish_mutex);
                dining_info->finish = 1;
                pthread_mutex_unlock(&dining_info->finish_mutex);
            }
            pthread_mutex_unlock(&dining_info->status_mutex);
        }
        usleep(1000); // Small sleep to reduce CPU usage
    }
    return NULL;
}

int check_all_ate_enough(t_dining_info *dining_info)
{
    int i;
    
    if (dining_info->num_must_eat == -1)
        return 0;

    pthread_mutex_lock(&dining_info->meal_mutex);
    for (i = 0; i < dining_info->num_philosophers; i++)
    {
        if (dining_info->philosophers[i].num_of_meals < dining_info->num_must_eat)
        {
            pthread_mutex_unlock(&dining_info->meal_mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&dining_info->meal_mutex);
    return 1;
}