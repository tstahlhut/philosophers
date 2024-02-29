/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tstahlhu <tstahlhu@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 12:36:51 by tstahlhu          #+#    #+#             */
/*   Updated: 2024/02/13 14:20:50 by tstahlhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/* start_threads: Creates philosopher threads and monitoring thread. 
	If successful, returns 0.
	In case of error, calls ft_error function and program exits. */

int	start_threads(t_data *data)
{
	int		i;

	if (data->nb_meals > -1 && data->nb > 1)
	{
		if (pthread_create(&data->supervisor, NULL, &monitor_meals, data) != 0)
			return (ft_error(data, "creating thread"));
	}
	data->t_zero = get_time(0);
	i = -1;
	while (++i < data->nb)
	{
		if (pthread_create(&data->th[i], NULL, &routine, &data->phil[i]) != 0)
			return (ft_error(data, "creating thread"));
		if (pthread_create(&data->phil[i].monitor, NULL, &monitor_death, &data->phil[i]) != 0)
			return (ft_error(data, "creating monitoring subthread\n"));
	}
	return (0);
}

/* init_philos: Initializes variables in all t_philo structs. */

void	init_philos(t_data *data)
{
	int	i;

	i = -1;
	while (++i < data->nb)
	{
		data->phil[i].pos = (i + 1);
		data->phil[i].last_meal = 0;
		data->phil[i].meals = 0;
		data->phil[i].eating = 0;
		data->phil[i].finished = 0;
		pthread_mutex_init(&data->phil[i].write, NULL);
		data->phil[i].data = data;
	}
}

/* init_mutex: Initializes all mutexes stored in data. */

void	init_mutex(t_data *data)
{
	int	i;

	i = -1;
	while (++i < data->nb)
	{
		pthread_mutex_init(&data->mutex_fork[i], NULL);
		data->forks[i] = 0;
	}
	pthread_mutex_init(&data->write, NULL);
	data->mutex_init = 1;
	return ;
}

/* malloc_data: Allocates memory in data struct
	for threads, philo struct, forks and mutexes 
	If successful, returns 0.
	In case of error, returns 1 and prints error message. */

int	malloc_data(t_data *data)
{
	data->th = malloc(sizeof(pthread_t *) * data->nb);
	if (!data->th)
		return (ft_error(data, "malloc data->th"), 1);
	data->phil = malloc(sizeof(t_philo) * data->nb);
	if (!data->phil)
		return (ft_error(data, "malloc data->phil"), 1);
	data->forks = malloc(sizeof(int) * data->nb);
	if (!data->forks)
		return (ft_error(data, "malloc data->forks"), 1);
	data->mutex_fork = malloc(sizeof(pthread_mutex_t) * data->nb);
	if (!data->mutex_fork)
		return (ft_error(data, "malloc data->mutex_fork"), 1);
	return (0);
}

/* ft_init:
 * 1. Convert user input from char* to int (ft_atoi) 
		and store in data.
 * 2. Set data->stop = 0; if data->stop == 1 -> program exits (ft_exit) 
 * 3. Set pointers to NULL in case a malloc fails and free is called.
 * 4. Allocate memory to pointers (threads, t_philo structs & mutexes).
 * 5. Initalize values for each philosopher (t_philo), see init_philos.
 * If successfully initialized, returns 0.
	In case of error, returns 1 and program exits.*/

int	ft_init(int argc, char **argv, t_data *data)
{
	data->nb = ft_atoi(argv[1]);
	data->t_die = ft_atoi(argv[2]);
	data->t_eat = ft_atoi(argv[3]);
	data->t_sleep = ft_atoi(argv[4]);
	if (argc == 6)
		data->nb_meals = ft_atoi(argv[5]);
	else
		data->nb_meals = -1;
	data->stop = 0;
	data->mutex_init = 0;
	data->eaten_meals = 0;
	data->th = NULL;
	data->phil = NULL;
	data->forks = NULL;
	data->mutex_fork = NULL;
	if (malloc_data(data))
		return (1);
	init_mutex(data);
	init_philos(data);
	return (0);
}
