/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tstahlhu <tstahlhu@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 12:23:25 by tstahlhu          #+#    #+#             */
/*   Updated: 2024/02/13 18:04:37 by tstahlhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Allowed external functions:
 * memset, printf, malloc, free, write
 * usleep, gettimeofday
 * pthread_create, pthread_detach, pthread_join
 * pthread_mutex_init, pthread_mutex_destroy
 * pthread_mutex_lock, pthread_mutex_unlock
 * 
 * Libft not authorized!
 * */

#include "philo.h"

/* eating: routine of eating for each philosopher
	1. pick up forks
	2. start time of meal is stored in last_meal
		(protected by mutex because monitoring thread accesses same data)
	3. message that philosopher is eating is printed on screen 
	4. philosophers number of eaten meals is increased by 1
		(protected by mutex because supervisor accesses same data)
	5. puts down forks */

void	eating(t_philo *phil)
{
	t_data	*data;

	data = phil->data;
	pick_up_forks(phil);
	pthread_mutex_lock(&phil->write);
	phil->last_meal = get_time(data->t_zero);
	pthread_mutex_unlock(&phil->write);
	print_phil_status(data, phil, "is eating");
	usleep(data->t_eat * 1000);
	pthread_mutex_lock(&phil->write);
	phil->meals++;
	pthread_mutex_unlock(&phil->write);
	put_down_left_fork(phil);
	if (data->nb > 1)
		put_down_right_fork(phil);
}

/* routine: the routine each philosopher follows while the simulation runs
	1. for each philosopher a monitoring thread is created 
		which checks constantly if the philospher has died from starvation
		if so the simulation stops
	2. while the simulation runs, each philosopher follows the routine:
		1. eat
		2. sleep
		3. think
	3. when the simulation stops, the monitoring thread is joined*/

void	*routine(void *arguments)
{
	t_philo		*phil;
	t_data		*data;
	long int	t_think;

	phil = (t_philo *)arguments;
	data = phil->data;
	t_think = (data->t_die - data->t_eat - data->t_sleep) / 2;
	/*if (pthread_create(&phil->monitor, NULL, &monitor_death, phil) != 0)
	{
		pthread_mutex_lock(&data->write);
		data->stop = 1;
		pthread_mutex_unlock(&data->write);
		ft_error(data, "creating monitoring subthread\n");
	}*/
	while (!stop_simulation(data))
	{
		eating(phil);
		print_phil_status(data, phil, "is sleeping");
		usleep(phil->data->t_sleep * 1000);
		print_phil_status(data, phil, "is thinking");
		if (t_think > 0)
			usleep(t_think * 1000);
	}
	/*if (pthread_join(phil->monitor, NULL) != 0)
		ft_error(data, "joining thread");*/
	return (NULL);
}

/* stop_threads: joins philosopher threads and supervisor thread
	on success, returns 0
	on failure, calls ft_error which exits program and prints error message */

int	stop_threads(t_data *data)
{
	int		i;

	i = -1;
	while (++i < data->nb)
	{
		if (pthread_join(data->phil[i].monitor, NULL) != 0)
			return (ft_error(data, "joining thread"));
		if (pthread_join(data->th[i], NULL) != 0)
			return (ft_error(data, "joining philo thread"));
	}
	if (data->nb_meals > -1 && data->nb > 1)
	{
		if (pthread_join(data->supervisor, NULL) != 0)
			return (ft_error(data, "joining supervisor thread"));
	}
	return (0);
}

/* Philosophers program: 
 * 1. CHECK parameters
 * 	(1) nb of philos, (2) time to die, (3) time to eat, (4) time to sleep, 
	(5) optional: nb of times philo must eat
 * 	 -> all argv must be numbers
 * 2. INIT: ft_init 
 * 	1. Convert user input from char to int (ft_atoi) and store in t_data
 *	2. Malloc pointers in data (th, phil, forks)
 *	3. Init mutexes (forks)
 *	4. Init t_philos
 * 5. START: init threads and thus start simulation (routine)
 * 6. STOP: join threads
 * 7. EXIT: free dynamically allocated memory, destroy mutexes and exit
 */

int	main(int argc, char **argv)
{
	t_data		data;

	if (check_input(argc, argv))
		return (1);
	if (ft_init(argc, argv, &data))
		return (1);
	if (start_threads(&data))
		return (1);
	stop_threads(&data);
	ft_exit(&data);
	return (0);
}
