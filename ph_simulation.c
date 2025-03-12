/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_simulation.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 02:42:21 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/12 23:42:36 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ph_main.h"

#include <stdlib.h>

static int	init_philosophers(t_simulation *sim)
{
	t_philosopher	*philos;
	int				i;

	philos = sim->philos;
	i = 0;
	while (i < sim->philo_count)
	{
		philos[i].id = i + 1;
		philos[i].left = &philos[(i + sim->philo_count - 1) % sim->philo_count];
		philos[i].right = &sim->philos[(i + 1) % sim->philo_count];
		philos[i].left_fork = &sim->forks[i];
		philos[i].right_fork = &sim->forks[(i + 1) % sim->philo_count];
		philos[i].start_time = -1;
		philos[i].last_meal_time = -1;
		philos[i].meal_count = 0;
		pthread_mutex_init(&philos[i].meal_lock, NULL);
		philos[i].state = HUNGRY;
		pthread_mutex_init(&philos[i].state_lock, NULL);
		philos[i].sim = sim;
		i++;
	}
	return (0);
}

int	init_simulation(t_simulation *sim)
{
	int	i;

	sim->is_running = true;
	sim->start_time = -1;
	sim->stop_time = -1;
	sim->philos = malloc(sim->philo_count * sizeof(*sim->philos));
	sim->forks = malloc(sim->philo_count * sizeof(*sim->forks));
	i = 0;
	while (i < sim->philo_count)
	{
		pthread_mutex_init(&sim->forks[i], NULL);
		i++;
	}
	init_queue(&sim->req_queue, sim->philo_count);
	init_queue(&sim->wait_queue, sim->philo_count);
	pthread_mutex_init(&sim->run_lock, NULL);
	pthread_mutex_init(&sim->log_lock, NULL);
	init_philosophers(sim);
	return (0);
}

int	destroy_simulation(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->philo_count)
	{
		pthread_mutex_destroy(&sim->forks[i]);
		i++;
	}
	i = 0;
	while (i < sim->philo_count)
	{
		pthread_mutex_destroy(&sim->philos[i].meal_lock);
		pthread_mutex_destroy(&sim->philos[i].state_lock);
		i++;
	}
	pthread_mutex_destroy(&sim->run_lock);
	pthread_mutex_destroy(&sim->log_lock);
	destroy_queue(&sim->req_queue);
	destroy_queue(&sim->wait_queue);
	free(sim->philos);
	free(sim->forks);
	return (0);
}
