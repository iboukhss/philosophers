/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_routine.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 21:07:34 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/12 11:25:45 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ph_main.h"

#include <unistd.h>

static void	thinking_phase(t_philosopher *philo)
{
	bool	can_eat;

	enqueue(&philo->sim->req_queue, philo);
	log_philo_state(philo, "is thinking");
	while (simulation_is_running(philo->sim))
	{
		pthread_mutex_lock(&philo->state_lock);
		can_eat = (philo->state == EATING);
		pthread_mutex_unlock(&philo->state_lock);
		if (can_eat)
		{
			return ;
		}
		usleep(200);
	}
}

// NOTE: TSAN may complain about lock-order-inversion but I am fairly
// confident that's a false positive.
static void	eating_phase(t_philosopher *philo)
{
	if (simulation_is_running(philo->sim))
	{
		pthread_mutex_lock(philo->left_fork);
		log_philo_state(philo, "has taken a fork");
		pthread_mutex_lock(philo->right_fork);
		log_philo_state(philo, "has taken a fork");
		pthread_mutex_lock(&philo->meal_lock);
		philo->last_meal_time = get_time_in_ms();
		philo->meal_count++;
		pthread_mutex_unlock(&philo->meal_lock);
		log_philo_state(philo, "is eating");
		usleep(philo->sim->time_to_eat * 1000);
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_lock(&philo->state_lock);
		philo->state = HUNGRY;
		pthread_mutex_unlock(&philo->state_lock);
	}
}

static void	sleeping_phase(t_philosopher *philo)
{
	if (simulation_is_running(philo->sim))
	{
		log_philo_state(philo, "is sleeping");
		usleep(philo->sim->time_to_sleep * 1000);
	}
}

void	*philo_routine(void *arg)
{
	t_philosopher	*philo;

	philo = (t_philosopher *)arg;
	pthread_mutex_lock(&philo->meal_lock);
	philo->start_time = get_time_in_ms();
	philo->last_meal_time = philo->start_time;
	pthread_mutex_unlock(&philo->meal_lock);
	while (simulation_is_running(philo->sim))
	{
		thinking_phase(philo);
		eating_phase(philo);
		sleeping_phase(philo);
	}
	return (NULL);
}
