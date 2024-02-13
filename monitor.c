/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tstahlhu <tstahlhu@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 16:06:14 by tstahlhu          #+#    #+#             */
/*   Updated: 2024/02/13 18:48:38 by tstahlhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/* monitor_death: monitoring thread for each philosopher which checks
	if the philosopher died of starvation, i.e. if the start time of 
	their last meal is equal or bigger than time_to_die
	The monitoring thread runs until simulation stops.
	If the philosopher it checks has died, it sets data->stop to 1
	and the simulation ends.*/

void	*monitor_death(void *arguments)
{
	t_philo	*phil;
	t_data	*data;

	phil = (t_philo *) arguments;
	data = phil->data;
	while (!stop_simulation(data))
	{
		if (time_since_last_meal(data, phil) >= data->t_die)
		{
			print_phil_status(data, phil, "has died");
			pthread_mutex_lock(&data->write);
			data->stop = 1;
			pthread_mutex_unlock(&data->write);
		}
	}
	return (NULL);
}

/* time_since_last_meal: calculates the time in milliseconds since last meal
		by calculating elapsed time since simulation start 
			with get_time(data->t_zero) 
		and substracting saved time of last_meal */

int	time_since_last_meal(t_data *data, t_philo *phil)
{
	int	t;

	pthread_mutex_lock(&phil->write);
	t = get_time(data->t_zero) - phil->last_meal;
	pthread_mutex_unlock(&phil->write);
	return (t);
}

/* ate_enough_meals: checks if philosopher has eaten required meals
	If user specified a minimum number of meals each philosopher must eat,
		this function is used to check if philosopher did eat enough.
	meals_eaten is protected by a mutex because philosopher thread access 
		this variable, too.
	If they ate min. meals_required, returns 1.
	If not, returns 0.*/

int	ate_enough_meals(t_philo *phil, t_data *data)
{
	int	i;

	i = 0;
	pthread_mutex_lock(&phil->write);
	if (phil->meals >= data->nb_meals)
		i = 1;
	pthread_mutex_unlock(&phil->write);
	return (i);
}

/* phil_finished: checks if philosopher has finished
	If they have finished, returns 1.
	If not, returns 0.*/

int	phil_finished(t_philo *phil)
{
	int	i;

	i = 0;
	pthread_mutex_lock(&phil->write);
	if (phil->finished)
		i = 1;
	pthread_mutex_unlock(&phil->write);
	return (i);
}

/* monitor_meals: checks if all philos have eaten the minimum amount of meals
	This monitoring thread only runs if user inputs argument 5:
		nb_of_times_each_philosopher_must_eat
	If so, this thread runs until simulation stops. 
	In a loop, it checks for each philosopher if they have eaten enough meals and
	if all philosophers have eaten enough meals.
	First, it checks philosophers finished flag, which is set to 1 if philosopher
		has eaten enough meals (note that philosopher still follows the routine
		even if they have finished. It is just a flag to indicate that philo
		has eaten enough.)
	If philosopher has not finished, it checks how many meals philo has eaten.
	If they have eaten enough, finished is set to 1 and data->eaten_meals is 
		increased by 1. (This variable counts the amount of finished philos.)
	If all philosophers have eaten enough meals, data->stop is set to 1 
		and simulation ends. */

void	*monitor_meals(void *arguments)
{
	t_data	*data;
	int		i;

	i = -1;
	data = (t_data *) arguments;
	while (!stop_simulation(data) && ++i < data->nb)
	{
		if (!phil_finished(&data->phil[i]))
		{
			if (ate_enough_meals(&data->phil[i], data))
			{
				pthread_mutex_lock(&data->write);
				data->eaten_meals++;
				if (data->eaten_meals >= data->nb)
					data->stop = 1;
				pthread_mutex_unlock(&data->write);
				pthread_mutex_lock(&data->phil[i].write);
				data->phil[i].finished = 1;
				pthread_mutex_unlock(&data->phil[i].write);
			}
		}
		if ((i + 1) == data->nb)
			i = -1;
	}
	return (NULL);
}
