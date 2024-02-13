/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tstahlhu <tstahlhu@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/31 12:04:39 by tstahlhu          #+#    #+#             */
/*   Updated: 2024/02/13 17:07:45 by tstahlhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>	/* for threads; to compile write: gcc -g -pthread main.c */
# include <stdio.h>	/* for printf */
# include <sys/time.h>	/* for gettimeofday */
# include <stdlib.h>	/* for malloc */
# include <unistd.h>	/* for usleep */

typedef struct s_philo
{
	pthread_t		monitor;	/* thread to supervise if philo dies of starvation */
	int				finished;	/* if philo has eaten nb_meals: finished == 1, if not: == 0 */
	int				pos;		/* philo*s position at the table, ranges from 1 to data->nb */
	long int		last_meal;	/* stores the time (in ms) of philo's last meal */
	int				meals;		/* stores the amount of meals eaten */
	int				eating;		/* if philo eats == 1; otherwise == 0 */
	pthread_mutex_t	write;		/* protects philo variables when changes are written to them */
	struct s_data	*data;		/* pointer to data struct */
}				t_philo;

typedef struct s_data
{
	pthread_t		*th;			/* holds all the philo threads created */
	pthread_t		supervisor;		/* thread to montitor all threads (only if nb_meals > 0)*/
	t_philo			*phil;			/* holds the t_philo structs */
	int				*forks;			/* forks (one for each philo)*/
	pthread_mutex_t	*mutex_fork;	/* holds the mutexes of all forks (nb) */
	int				nb;				/* number of philosophers (threads) */
	int				t_die;			/* time to die (specified by user input) */
	int				t_eat;			/* time to eat (specified by user input) */
	int				t_sleep;		/* time to sleep (specified by user input) */
	int				nb_meals;		/* nb of times philo must eat (optional user input) */
	int				eaten_meals;
	long int		t_zero;			/* holds start time in ms (gettimeofday function) */
	int				stop;			/* stop == 0; if stop == 1, execution stops (either because of a philo death or because all philos have eaten enough (nb_meals)) */
	int				mutex_init;		/* to prevent mutex_destroy be called upon uninitialized mutexes in case of error; mutex_init = 0 (uninitialized) or mutex_init = 1 (initialized) */
	pthread_mutex_t	write;			/* protects data variables when changes are written to them */
}				t_data;

/* check.c: user input check */
int			ft_atoi(const char *nptr);			/* converts char* to int */
char		*ft_strdup(const char *s);			/* duplicates string */
int			check_input(int argc, char **argv);	/* checks user input */
int			ft_isint(char *str);				/* checks if char *str is int */
int			ft_isdigit(char c);					/* checks if char c is digit */

/* init.c: initializing variables */
int			malloc_data(t_data *data);						/* allocates memory for arrays in data */
int			ft_init(int argc, char **argv, t_data *data);	/* initializes all variables */
void		init_mutex(t_data *data);						/* initializes mutexes (forks) */
void		init_philos(t_data *data);						/* initializes t_philo */
int			start_threads(t_data *data);					/* creates threads and thus starts simulation */

/* philo.c: main, routine */
int			stop_threads(t_data *data);			/* joins all philosopher threads and supervisor thread */
void		eating(t_philo *phil);				/* eating routine */
void		*routine(void *arguments);			/* routine each philosopher follows: eat, sleep, think, repeat*/

/* pick_up_forks.c: picking up forks */
int			fork_in_use(int n, t_data *data);
void		pick_up_left_fork(t_philo *phil);
void		pick_up_right_fork(t_philo *phil);
void		pick_up_forks(t_philo *phil);

/* put_down_forks.c: putting down forks */
void		put_down_left_fork(t_philo *phil);
void		put_down_right_fork(t_philo *phil);
void		put_down_forks(t_philo *phil);

/* monitor.c: monitoring thread for each philosopher */
int			time_since_last_meal(t_data *data, t_philo *phil);
void		*monitor_death(void *arguments);
void		*monitor_meals(void *arguments);

/* utils.c: helper functions */

void		print_phil_status(t_data *data, t_philo *phil, char *status);
int			ft_atoi(const char *nptr);			/* converts input in char* to int */
long int	get_time(long int time_zero);
void		ft_exit(t_data *data);				
int			ft_error(t_data *data, char *message);		/* returns 1 */
int			stop_simulation(t_data *data);

#endif
