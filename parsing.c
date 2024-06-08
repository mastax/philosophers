/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elel-bah <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 12:58:43 by elel-bah          #+#    #+#             */
/*   Updated: 2024/06/04 12:58:46 by elel-bah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_is_digit(int k)
{
	if (k >= '0' && k <= '9')
		return (1);
	return (0);
}

int validate_arguments(char **arguments)
{
    int i;
    int j;

    i = 0;
    while (arguments[i])
    {
        j = 0;
        while (arguments[i][j])
        {
            if ((arguments[i][j] != '+') && (arguments[i][j] > '9' || arguments[i][j] < '0'))
            {
                free_split(arguments);
                return (0);
            }
            j++;
        }
        i++;
    }
    return (1);
}

int validate_plus(char **arguments)
{
    int i;
    int j;

    i = 0;
    while  (arguments[i])
    {
        j = 0;
        while (arguments[i][j])
        {
            if (arguments[i][j] == '+')
            {
                if (arguments[i][j + 1] > '9' || arguments[i][j + 1] < '0')
                {
                    free_split(arguments);
                    return (0);
                }
            }
            j++;
        }
        i++;
    }
    return (1);
}

int validate_max(char **arg)
{
    int i;
    int j;
    int found;

    i = 1;
    j = 0;
    found = 0;
    while (arg[i])
    {
        j = 0;
        while (arg[i][j])
        {
            if (found != 1 && ft_is_digit(arg[i][j]))
                found = 1;
            if (found == 1 && arg[i][j] == ' ' && ft_is_digit(arg[i][j + 1]))
                return (0);
            j++;
        }
        i++;
        if (found == 1)
            found = 0;
        else
            return (0);
    }
    return (1);
}

int parse_arguments(t_data *data)
{
    data->combined_args = ft_joining_args(data->av, data->ac);
    if (!data->combined_args)
        return (0);
    data->arguments = ft_split(data->combined_args, ' ');
    if (!data->combined_args)
    {
        free(data->combined_args);
        return (0);
    }
    free(data->combined_args);
    if (!validate_arguments(data->arguments) || !validate_plus(data->arguments) || !validate_max(data->arguments))
        return (0);
    free_split(data->arguments);
    return (1);
}


