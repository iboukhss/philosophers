/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 21:08:25 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/15 15:40:49 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ph_main.h"

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

long	get_time_in_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

void	log_philo_state(t_philosopher *philo, const char *msg)
{
	long	timestamp;

	pthread_mutex_lock(&philo->sim->log_lock);
	timestamp = get_time_in_ms() - philo->start_time;
	(void)printf("%7ld %4d %s\n", timestamp, philo->id, msg);
	pthread_mutex_unlock(&philo->sim->log_lock);
}

bool	simulation_is_running(t_simulation *sim)
{
	bool	is_running;

	pthread_mutex_lock(&sim->run_lock);
	is_running = sim->is_running;
	pthread_mutex_unlock(&sim->run_lock);
	return (is_running);
}
