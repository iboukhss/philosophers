/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_main.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:25:06 by iboukhss          #+#    #+#             */
/*   Updated: 2025/02/25 22:57:14 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "ph_main.h"

static void	*routine(void *arg)
{
	t_philosopher	*philo;
	struct timeval	current_time;
	long			time_elapsed;

	philo = (t_philosopher *)arg;
	usleep(5000);
	gettimeofday(&current_time, NULL);
	time_elapsed = (current_time.tv_usec - philo->sim->start_time.tv_usec) / 1000;
	printf("Started philo %d after %ld milliseconds\n", philo->id, time_elapsed);
	return (NULL);
}

int	main(int argc, char *argv[])
{
	t_simulation	sim;
	t_philosopher	*philo;

	if (argc < 5 || argc > 6)
	{
		fprintf(stderr, "Usage: %s <philo_count> <time_to_die> <time_to_eat> <time_to_sleep> [meals_required]\n", argv[0]);
		return (1);
	}

	// Init user input parameters
	sim.philo_count = atoi(argv[1]);
	sim.time_to_die = strtol(argv[2], NULL, 10);
	sim.time_to_eat = strtol(argv[3], NULL, 10);
	sim.time_to_sleep = strtol(argv[4], NULL, 10);
	sim.meals_required = argc == 6 ? atoi(argv[5]) : -1;

	// Start timer
	gettimeofday(&sim.start_time, NULL);

	// Create forks (mutexes)
	sim.forks = malloc(sim.philo_count * sizeof(*sim.forks));
	for (int i = 0; i < sim.philo_count; i++)
	{
		pthread_mutex_init(&sim.forks[i], NULL);
	}

	// Create philosophers (threads)
	philo = malloc(sim.philo_count * sizeof(*philo));
	for (int i = 0; i < sim.philo_count; i++)
	{
		philo[i].id = i + 1;
		philo[i].sim = &sim;
		pthread_create(&philo[i].thread, NULL, routine, &philo[i]);
	}

	// Wait for all threads to join
	for (int i = 0; i < sim.philo_count; i++)
	{
		pthread_join(philo[i].thread, NULL);
	}

	// Stop timer
	gettimeofday(&sim.stop_time, NULL);
	printf("Total time elapsed: %ld milliseconds\n", (sim.stop_time.tv_usec - sim.start_time.tv_usec) / 1000);

	// Cleanup
	for (int i = 0; i < sim.philo_count; i++)
	{
		pthread_mutex_destroy(&sim.forks[i]);
	}
	free(sim.forks);
	free(philo);

	return (0);
}
