/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tstahlhu <tstahlhu@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/31 12:04:39 by tstahlhu          #+#    #+#             */
/*   Updated: 2024/02/14 18:28:26 by tstahlhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <sys/time.h>
# include <stdlib.h>
# include <unistd.h>

# define MAX_INT_STR "2147483647"

typedef struct s_philo
{
	pthread_t		monitor;
	int				finished;
	int				pos;
	long int		last_meal;
	int				meals;
	int				eating;
	pthread_mutex_t	write;
	struct s_data	*data;
}				t_philo;

typedef struct s_data
{
	pthread_t		*th;
	pthread_t		supervisor;
	t_philo			*phil;
	int				*forks;
	pthread_mutex_t	*mutex_fork;
	int				nb;
	int				t_die;
	int				t_eat;
	int				t_sleep;
	int				nb_meals;
	int				eaten_meals;
	long int		t_zero;
	int				stop;
	int				mutex_init;
	pthread_mutex_t	write;
}				t_data;

/* check.c: user input check */
int			ft_atoi(const char *nptr);
char		*ft_strdup(const char *s);
int			check_input(int argc, char **argv);
int			ft_isint(char *str);
int			ft_isdigit(char c);

/* init.c: initializing variables */
int			malloc_data(t_data *data);
int			ft_init(int argc, char **argv, t_data *data);
void		init_mutex(t_data *data);
void		init_philos(t_data *data);
int			start_threads(t_data *data);

/* philo.c: main, routine */
int			stop_threads(t_data *data);
void		eating(t_philo *phil);
void		*routine(void *arguments);

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
int			ft_atoi(const char *nptr);
long int	get_time(long int time_zero);
void		ft_exit(t_data *data);				
int			ft_error(t_data *data, char *message);
int			stop_simulation(t_data *data);

#endif
