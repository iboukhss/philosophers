/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 21:08:25 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/14 14:14:38 by iboukhss         ###   ########.fr       */
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

static bool	fork_is_available(t_fork *fork)
{
	bool	is_avail;

	pthread_mutex_lock(&fork->lock);
	is_avail = fork->is_avail;
	pthread_mutex_unlock(&fork->lock);
	return (is_avail);
}

bool	forks_are_available(t_philosopher *philo)
{
	bool	left_avail;
	bool	right_avail;

	left_avail = fork_is_available(philo->left_fork);
	right_avail = fork_is_available(philo->right_fork);
	return (left_avail && right_avail);
}

static int	take_fork(t_philosopher *philo, t_fork *fork)
{
	while (simulation_is_running(philo->sim))
	{
		if (fork_is_available(fork))
		{
			pthread_mutex_lock(&fork->lock);
			fork->owner_id = philo->id;
			fork->is_avail = false;
			pthread_mutex_unlock(&fork->lock);
			log_philo_state(philo, "has taken a fork");
			return (0);
		}
		usleep(200);
	}
	return (1);
}

int	take_forks(t_philosopher *philo)
{
	take_fork(philo, philo->left_fork);
	take_fork(philo, philo->right_fork);
	if (simulation_is_running(philo->sim))
	{
		pthread_mutex_lock(&philo->state_lock);
		philo->can_eat = true;
		pthread_mutex_unlock(&philo->state_lock);
	}
	return (0);
}

int	release_forks(t_philosopher *philo)
{
	pthread_mutex_lock(&philo->left_fork->lock);
	philo->left_fork->owner_id = -1;
	philo->left_fork->is_avail = true;
	pthread_mutex_unlock(&philo->left_fork->lock);
	pthread_mutex_lock(&philo->right_fork->lock);
	philo->right_fork->owner_id = -1;
	philo->right_fork->is_avail = true;
	pthread_mutex_unlock(&philo->right_fork->lock);
	pthread_mutex_lock(&philo->state_lock);
	philo->can_eat = false;
	pthread_mutex_unlock(&philo->state_lock);
	return (0);
}
