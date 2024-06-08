#include "philo.h"

int	ft_strlen(char *str)
{
	int	i;

	if (!str)
		return (0);
	i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}

char    *ft_strjoin(char *s1, char *s2)
{
    int     i = 0;
    int     j = 0;
    char    *str;

    if (!s1)
    {
        s1 = (char *)malloc(1 * sizeof(char));
        if (!s1)
            return (NULL);
        s1[0] = '\0';
    }
    if (!s2)
        return (NULL);
    str = malloc (ft_strlen(s1) + ft_strlen(s2) + 2);
    if (!str)
        return (NULL);
    while (s1[++i])
        str[i] = s1[i];
    while (s2[j])
        str[i++] = s2[j++];
    str[ft_strlen(s1) + ft_strlen(s2)] = '\0';
    free(s1);
    return (str);
}

char    *ft_joining_args(char **av, int ac)
{
    char    *arguments;
    int     i;

    i = 1;
    arguments = NULL;
    while (i < ac)
    {
        if (av[i][0] == '\0')
            return (NULL);
        arguments = ft_strjoin(arguments, av[i++]);
        arguments = ft_strjoin(arguments, " ");
    }
    return (arguments);
}

