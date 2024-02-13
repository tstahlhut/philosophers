/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tstahlhu <tstahlhu@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 11:29:42 by tstahlhu          #+#    #+#             */
/*   Updated: 2024/02/13 14:27:17 by tstahlhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	print_phil_status(t_data *data, t_philo *phil, char *status)
{
	if (!stop_simulation(data))
		printf("%ld %i %s\n", get_time(data->t_zero), phil->pos, status);
}

/* get_time: calculates elapsed time since time_zero in milliseconds
	gettimeofday() gets the system's clock time in elapsed seconds and 
		microseconds since 00:00:00, January 1, 1970 (Unix Epoch)
	to calculate milliseconds accurately, seconds and microseconds are added
		to receive milliseconds */

long int	get_time(long int time_zero)
{
	struct timeval	tv;
	long int		time_in_mill;

	gettimeofday(&tv, NULL);
	time_in_mill = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
	return (time_in_mill - time_zero);
}

/* free all allocated memory (malloc) and destroy all mutexes */

void	ft_exit(t_data *data)
{
	int	i;

	i = -1;
	if (data->mutex_fork)
	{
		while (++i < data->nb && data->mutex_init)
			pthread_mutex_destroy(&data->mutex_fork[i]);
	}
	pthread_mutex_destroy(&data->write);
	i = -1;
	if (data->th)
		free(data->th);
	if (data->phil)
		free(data->phil);
	if (data->forks)
		free(data->forks);
	if (data->mutex_fork)
		free(data->mutex_fork);
}

int	ft_error(t_data *data, char *message)
{
	pthread_mutex_lock(&data->write);
	data->stop = 1;
	pthread_mutex_unlock(&data->write);
	printf("Error: %s\n", message);
	ft_exit(data);
	return (1);
}

int	stop_simulation(t_data *data)
{
	int	i;

	i = 0;
	pthread_mutex_lock(&data->write);
	if (data->stop == 1)
		i = 1;
	pthread_mutex_unlock(&data->write);
	return (i);
}

/*time_t	get_time(void)
{
	struct timeval	tv;
	time_t		time_in_mill;

	gettimeofday(&tv, NULL);
	time_in_mill = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
	return (time_in_mill);
}*/
