/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_main.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 21:35:24 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/05 13:30:42 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PH_MAIN_H
# define PH_MAIN_H

# include <pthread.h>
# include <stdbool.h>

// Forward declare
typedef struct s_philosopher	t_philosopher;

typedef struct s_simulation
{
	int				philo_count;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				meals_required;
	long			start_time;
	long			stop_time;
	bool			is_running;
	t_philosopher	*philos;
	pthread_mutex_t	*forks;
	pthread_mutex_t	sim_lock;
}	t_simulation;

typedef struct s_philosopher
{
	t_simulation	*sim;
	int				id;
	pthread_t		thread;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	long			last_meal_time;
	int				meal_count;
	pthread_mutex_t	time_lock;
	pthread_mutex_t	meal_lock;
}	t_philosopher;

#endif
