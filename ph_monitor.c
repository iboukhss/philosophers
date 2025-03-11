/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_monitor.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 21:05:14 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/11 21:15:33 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ph_main.h"

#include <unistd.h>

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;
	long			current_time;
	long			last_meal_time;
	long			elapsed_time;
	int				meal_count;

	sim = (t_simulation *)arg;
	while (1)
	{
		for (int i = 0; i < sim->philo_count; i++)
		{
			pthread_mutex_lock(&sim->philos[i].meal_count_lock);
			last_meal_time = sim->philos[i].last_meal_time;
			pthread_mutex_unlock(&sim->philos[i].meal_count_lock);
			if (last_meal_time == -1)
			{
				continue ;
			}
			current_time = get_time_in_ms();
			elapsed_time = current_time - last_meal_time;
			if (elapsed_time > sim->time_to_die)
			{
				log_philo_state(&sim->philos[i], "died");
				pthread_mutex_lock(&sim->run_lock);
				sim->is_running = false;
				pthread_mutex_unlock(&sim->run_lock);
				return (NULL);
			}
		}
		if (sim->meals_required > 0)
		{
			for (int i = 0; i < sim->philo_count; i++)
			{
				pthread_mutex_lock(&sim->philos[i].meal_count_lock);
				meal_count = sim->philos[i].meal_count;
				pthread_mutex_unlock(&sim->philos[i].meal_count_lock);
				if (meal_count < sim->meals_required)
				{
					break ;
				}
				if (i == sim->philo_count - 1)
				{
					pthread_mutex_lock(&sim->run_lock);
					sim->is_running = false;
					pthread_mutex_unlock(&sim->run_lock);
					return (NULL);
				}
			}
		}
		usleep(2000);
	}
	return (NULL);
}
