/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_monitor.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 21:05:14 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/14 14:09:31 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ph_main.h"

#include <unistd.h>

static void	check_last_meal_time(t_simulation *sim)
{
	long	last_meal_time;
	long	elapsed_time;
	int		i;

	i = 0;
	while (i < sim->philo_count)
	{
		pthread_mutex_lock(&sim->philos[i].meal_lock);
		last_meal_time = sim->philos[i].last_meal_time;
		pthread_mutex_unlock(&sim->philos[i].meal_lock);
		if (last_meal_time != -1)
		{
			elapsed_time = get_time_in_ms() - last_meal_time;
			if (elapsed_time > sim->time_to_die)
			{
				log_philo_state(&sim->philos[i], "died");
				pthread_mutex_lock(&sim->run_lock);
				sim->is_running = false;
				pthread_mutex_unlock(&sim->run_lock);
				return ;
			}
		}
		i++;
	}
}

static void	check_meal_count(t_simulation *sim)
{
	int	meal_count;
	int	i;

	if (simulation_is_running(sim) && sim->meals_required > 0)
	{
		i = 0;
		while (i < sim->philo_count)
		{
			pthread_mutex_lock(&sim->philos[i].meal_lock);
			meal_count = sim->philos[i].meal_count;
			pthread_mutex_unlock(&sim->philos[i].meal_lock);
			if (meal_count < sim->meals_required)
			{
				return ;
			}
			if (i == sim->philo_count - 1)
			{
				pthread_mutex_lock(&sim->run_lock);
				sim->is_running = false;
				pthread_mutex_unlock(&sim->run_lock);
				return ;
			}
			i++;
		}
	}
}

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;

	sim = (t_simulation *)arg;
	while (simulation_is_running(sim))
	{
		check_last_meal_time(sim);
		check_meal_count(sim);
		usleep(2000);
	}
	return (NULL);
}
