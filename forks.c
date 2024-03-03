/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pick_up_forks.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tstahlhu <tstahlhu@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/07 16:58:33 by tstahlhu          #+#    #+#             */
/*   Updated: 2024/02/13 18:46:54 by tstahlhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/* pick_up_left_fork: self-descriptive
	Philosopher waits until their left fork is available.
		(phil->pos or in case of last philosopher fork no. 0)
	As soon as it is available, sets fork to "in use" (1).
	While changing the value of the fork, it is locked by a mutex 
		to prevent data race (the lock is unlocked when the fork is put down).
	A status message is printed on the screen */

void	pick_up_left_fork(t_philo *phil)
{
	t_data	*data;
	int		n;

	data = phil->data;
	if (phil->pos == data->nb)
		n = 0;
	else
		n = phil->pos;
	pthread_mutex_lock(&data->mutex_fork[n]);
	if (data->forks[n])
		printf("ERROR: fork %i has value %i\n", 
			phil->pos - 1, data->forks[phil->pos - 1]);
	data->forks[n] = 1;
	print_phil_status(data, phil, "has taken a fork");
}

/* pick_up_right_fork: self-descriptive
	Philosopher waits until their right fork (phil->pos - 1) is available.
	As soon as it is available, sets it to "in use" (1).
	While changing the value of the fork, it is locked by a mutex 
		to prevent data race (the lock is unlocked when the fork is put down).
	A status message is printed on the screen. */

void	pick_up_right_fork(t_philo *phil)
{
	t_data	*data;

	data = phil->data;
	pthread_mutex_lock(&data->mutex_fork[phil->pos - 1]);
	if (data->forks[phil->pos - 1])
		printf("ERROR: fork %i has value %i\n", 
			phil->pos - 1, data->forks[phil->pos - 1]);
	data->forks[phil->pos - 1] = 1;
	print_phil_status(data, phil, "has taken a fork");
}

/* pick_up_forks: manages the picking up of forks
	If only one philosopher sits at the table, there is only one fork.
	After picking up the left fork, the philosopher is therefore set to sleep 
		until he dies.
	To prevent deadlock, philsophers with an even position pick up 
		the left fork first, then the right fork. 
	Philosophers with an odd position, do the reverse.
	Note: In the beginning I had only an else and not an else if for the even
	numbered threads. This, however, gave the odd threads a slight advantage
	and sometimes caused deaths of even numbered philos. */

void	pick_up_forks(t_philo *phil)
{
	if (phil->data->nb == 1)
	{
		pick_up_left_fork(phil);
		usleep(phil->data->t_die * 1000);
	}
	else if (phil->pos % 2)
	{
		pick_up_right_fork(phil);
		pick_up_left_fork(phil);
	}
	else if (phil->pos % 2 == 0)
	{
		pick_up_left_fork(phil);
		pick_up_right_fork(phil);
	}
}

/* put_down_left_fork: sets fork value of left fork to 0 -> not in use	
	The locked mutex can now be unlocked and the fork is free for use by
	other threads. 
	The left fork of the last philosopher is fork 0 */

void	put_down_left_fork(t_philo *phil)
{
	t_data	*data;
	int		n;

	data = phil->data;
	if (phil->pos == data->nb)
		n = 0;
	else
		n = phil->pos;
	data->forks[n] = 0;
	pthread_mutex_unlock(&data->mutex_fork[n]);
}

/* put_down_right_fork: sets fork value of right fork to 0 -> not in use	
	The locked mutex can now be unlocked and the fork is free for use by
	other threads.  */

void	put_down_right_fork(t_philo *phil)
{
	t_data	*data;

	data = phil->data;
	data->forks[phil->pos - 1] = 0;
	pthread_mutex_unlock(&data->mutex_fork[phil->pos - 1]);
}
