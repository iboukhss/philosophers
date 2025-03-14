/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_routine.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 21:07:34 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/14 14:01:52 by iboukhss         ###   ########.fr       */
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
		can_eat = philo->can_eat;
		pthread_mutex_unlock(&philo->state_lock);
		if (can_eat)
		{
			return ;
		}
		usleep(200);
	}
}

static void	eating_phase(t_philosopher *philo)
{
	if (simulation_is_running(philo->sim))
	{
		pthread_mutex_lock(&philo->meal_lock);
		philo->last_meal_time = get_time_in_ms();
		philo->meal_count++;
		pthread_mutex_unlock(&philo->meal_lock);
		log_philo_state(philo, "is eating");
		usleep(philo->sim->time_to_eat * 1000);
		release_forks(philo);
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
