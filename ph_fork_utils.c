/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_fork_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 15:40:39 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/15 15:41:13 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ph_main.h"

#include <unistd.h>

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
		pthread_mutex_lock(&philo->meal_lock);
		philo->can_eat = true;
		pthread_mutex_unlock(&philo->meal_lock);
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
	pthread_mutex_lock(&philo->meal_lock);
	philo->can_eat = false;
	pthread_mutex_unlock(&philo->meal_lock);
	return (0);
}
