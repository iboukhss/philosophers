/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_routine.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 21:07:34 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/11 21:15:07 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ph_main.h"

#include <unistd.h>

void	*philo_routine(void *arg)
{
	t_philosopher	*philo;
	bool			can_eat;

	philo = (t_philosopher *)arg;
	pthread_mutex_lock(&philo->meal_count_lock);
	philo->start_time = get_time_in_ms();
	philo->last_meal_time = philo->start_time;
	pthread_mutex_unlock(&philo->meal_count_lock);
	while (simulation_is_running(philo->sim))
	{
		enqueue(&philo->sim->req_queue, philo);

		// Thinking phase
		log_philo_state(philo, "is thinking");

		while (1)
		{
			if (!simulation_is_running(philo->sim))
			{
				return (NULL);
			}
			pthread_mutex_lock(&philo->state_lock);
			can_eat = (philo->state == EATING);
			pthread_mutex_unlock(&philo->state_lock);
			if (can_eat)
			{
				break ;
			}
			usleep(200);
		}

		// Eating phase

		// NOTE: TSAN may complain about lock-order-inversion but I am fairly
		// confident that's a false positive.

		pthread_mutex_lock(philo->left_fork);
		log_philo_state(philo, "has taken a fork");
		pthread_mutex_lock(philo->right_fork);
		log_philo_state(philo, "has taken a fork");

		pthread_mutex_lock(&philo->meal_count_lock);
		philo->last_meal_time = get_time_in_ms();
		philo->meal_count++;
		pthread_mutex_unlock(&philo->meal_count_lock);

		log_philo_state(philo, "is eating");
		usleep(philo->sim->time_to_eat * 1000);

		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);

		pthread_mutex_lock(&philo->state_lock);
		philo->state = HUNGRY;
		pthread_mutex_unlock(&philo->state_lock);

		if (!simulation_is_running(philo->sim))
		{
			return (NULL);
		}

		// Sleeping phase
		log_philo_state(philo, "is sleeping");
		usleep(philo->sim->time_to_sleep * 1000);
	}
	return (NULL);
}
