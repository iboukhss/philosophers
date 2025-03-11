/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_main.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:25:06 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/11 21:11:48 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ph_main.h"

#include <stdio.h>
#include <stdlib.h>

static int	init_simulation(t_simulation *sim, int argc, char **argv)
{
	// Initialize user input parameters
	sim->philo_count = atoi(argv[1]);
	sim->time_to_die = strtol(argv[2], NULL, 10);
	sim->time_to_eat = strtol(argv[3], NULL, 10);
	sim->time_to_sleep = strtol(argv[4], NULL, 10);
	sim->meals_required = argc == 6 ? atoi(argv[5]) : -1;

	sim->start_time = -1;
	sim->stop_time = -1;

	// Allocate space for philos and forks
	sim->philos = malloc(sim->philo_count * sizeof(*sim->philos));
	sim->forks = malloc(sim->philo_count * sizeof(*sim->forks));

	// Initialize philos and forks
	for (int i = 0; i < sim->philo_count; i++)
	{
		pthread_mutex_init(&sim->forks[i], NULL);
	}
	for (int i = 0; i < sim->philo_count; i++)
	{
		sim->philos[i].id = i + 1;
		sim->philos[i].left = &sim->philos[(i + sim->philo_count - 1) % sim->philo_count];
		sim->philos[i].right = &sim->philos[(i + 1) % sim->philo_count];
		sim->philos[i].left_fork = &sim->forks[i];
		sim->philos[i].right_fork = &sim->forks[(i + 1) % sim->philo_count];
		sim->philos[i].start_time = -1;
		sim->philos[i].last_meal_time = -1;
		sim->philos[i].meal_count = 0;
		pthread_mutex_init(&sim->philos[i].meal_count_lock, NULL);
		sim->philos[i].state = HUNGRY;
		pthread_mutex_init(&sim->philos[i].state_lock, NULL);
		sim->philos[i].sim = sim;
	}

	// Queues
	init_queue(&sim->req_queue, sim->philo_count);
	init_queue(&sim->wait_queue, sim->philo_count);

	// Remaining values
	sim->is_running = true;
	pthread_mutex_init(&sim->run_lock, NULL);
	pthread_mutex_init(&sim->log_lock, NULL);
	return (0);
}

static int	destroy_simulation(t_simulation *sim)
{
	// Destroy forks
	for (int i = 0; i < sim->philo_count; i++)
	{
		pthread_mutex_destroy(&sim->forks[i]);
	}

	// Destroy philo locks
	for (int i = 0; i < sim->philo_count; i++)
	{
		pthread_mutex_destroy(&sim->philos[i].meal_count_lock);
		pthread_mutex_destroy(&sim->philos[i].state_lock);
	}

	pthread_mutex_destroy(&sim->run_lock);
	pthread_mutex_destroy(&sim->log_lock);

	// Destroy queues
	destroy_queue(&sim->req_queue);
	destroy_queue(&sim->wait_queue);

	// Free allocated memory
	free(sim->philos);
	free(sim->forks);
	return (0);
}

int	main(int argc, char **argv)
{
	t_simulation	sim;
	pthread_t		monitor;
	pthread_t		waiter;

	if (argc < 5 || argc > 6)
	{
		fprintf(stderr, "Usage: %s <philo_count> <time_to_die> <time_to_eat> <time_to_sleep> [meals_required]\n", argv[0]);
		return (1);
	}

	init_simulation(&sim, argc, argv);

	// Start timer
	sim.start_time = get_time_in_ms();

	// Start monitor thread
	pthread_create(&monitor, NULL, monitor_routine, &sim);

	// Start waiter thread
	pthread_create(&waiter, NULL, waiter_routine, &sim);

	// Start philosophers
	for (int i = 0; i < sim.philo_count; i++)
	{
		pthread_create(&sim.philos[i].thread, NULL, philo_routine, &sim.philos[i]);
	}

	// Wait for monitor thread
	pthread_join(monitor, NULL);

	// Wait for waiter thread
	pthread_join(waiter, NULL);

	// Wait for all philosophers
	for (int i = 0; i < sim.philo_count; i++)
	{
		pthread_join(sim.philos[i].thread, NULL);
	}

	// Stop timer
	sim.stop_time = get_time_in_ms();
	printf("Total time elapsed: %ld milliseconds\n", sim.stop_time - sim.start_time);

	destroy_simulation(&sim);
	return (0);
}
