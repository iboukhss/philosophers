/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_main.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 21:35:24 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/03 15:43:25 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PH_MAIN_H
# define PH_MAIN_H

# include <pthread.h>

typedef struct s_simulation
{
	int				philo_count;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				meals_required;
	pthread_mutex_t	*forks;
	long			start_time;
	long			stop_time;
}	t_simulation;

typedef struct s_philosopher
{
	int				id;
	pthread_t		thread;
	long			last_meal_time;
	int				meals_eaten;
	t_simulation	*sim;
}	t_philosopher;

#endif
