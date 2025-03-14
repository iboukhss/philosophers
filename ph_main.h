/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_main.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 21:35:24 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/14 01:29:45 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PH_MAIN_H
# define PH_MAIN_H

# include <pthread.h>
# include <stdbool.h>

// Forward declare
typedef struct s_philosopher	t_philosopher;

typedef struct s_fork
{
	int				owner_id;
	bool			is_avail;
	pthread_mutex_t	lock;
}	t_fork;

typedef struct s_queue
{
	t_philosopher	**items;
	int				head;
	int				tail;
	int				length;
	int				capacity;
	pthread_mutex_t	lock;
}	t_queue;

typedef struct s_simulation
{
	pthread_t		monitor;
	pthread_t		waiter;
	int				philo_count;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				meals_required;
	long			start_time;
	long			stop_time;
	t_philosopher	*philos;
	t_fork			*forks;
	t_queue			req_queue;
	t_queue			wait_queue;
	bool			is_running;
	pthread_mutex_t	run_lock;
	pthread_mutex_t	log_lock;
}	t_simulation;

typedef struct s_philosopher
{
	int				id;
	pthread_t		thread;
	t_fork			*left_fork;
	t_fork			*right_fork;
	long			start_time;
	long			last_meal_time;
	bool			can_eat;
	int				meal_count;
	pthread_mutex_t	meal_lock;
	pthread_mutex_t	state_lock;
	t_simulation	*sim;
}	t_philosopher;

int				init_simulation(t_simulation *sim);
int				destroy_simulation(t_simulation *sim);

int				init_queue(t_queue *q, int capacity);
int				destroy_queue(t_queue *q);
int				enqueue(t_queue *q, t_philosopher *philo);
t_philosopher	*dequeue(t_queue *q);
t_philosopher	*peek(t_queue *q);

void			*monitor_routine(void *arg);
void			*waiter_routine(void *arg);
void			*philo_routine(void *arg);

long			get_time_in_ms(void);
void			log_philo_state(t_philosopher *philo, const char *msg);
bool			simulation_is_running(t_simulation *sim);

bool			forks_are_available(t_philosopher *philo);
int				take_forks(t_philosopher *philo);
int				release_forks(t_philosopher *philo);

#endif
