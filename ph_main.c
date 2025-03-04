/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_main.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:25:06 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/04 13:17:37 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "ph_main.h"

long	get_time_in_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

static void	*routine(void *arg)
{
	t_philosopher	*philo;
	long			start_time;
	long			stop_time;
	long			time_elapsed;

	philo = (t_philosopher *)arg;
	start_time = philo->sim->start_time;
	stop_time = get_time_in_ms();
	time_elapsed = stop_time - start_time;
	printf("%6ld %d is sleeping 🌜\n", time_elapsed, philo->id);
	usleep(philo->sim->time_to_sleep * 1000);
	while (1)
	{
		stop_time = get_time_in_ms();
		time_elapsed = stop_time - start_time;
		printf("%6ld %d is thinking 💭\n", time_elapsed, philo->id);
		usleep(4000 * 1000);
	}
	printf("Ended philo %d\n", philo->id);
	return (NULL);
}

static int	init_simulation(t_simulation *sim, int argc, char **argv)
{
	sim->philo_count = atoi(argv[1]);
	sim->time_to_die = strtol(argv[2], NULL, 10);
	sim->time_to_eat = strtol(argv[3], NULL, 10);
	sim->time_to_sleep = strtol(argv[4], NULL, 10);
	sim->meals_required = argc == 6 ? atoi(argv[5]) : -1;
	return (0);
}

int	main(int argc, char **argv)
{
	t_simulation	sim;
	t_philosopher	*philo;

	if (argc < 5 || argc > 6)
	{
		fprintf(stderr, "Usage: %s <philo_count> <time_to_die> <time_to_eat> <time_to_sleep> [meals_required]\n", argv[0]);
		return (1);
	}

	// Init user input parameters
	init_simulation(&sim, argc, argv);

	// Start timer
	sim.start_time = get_time_in_ms();

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
	sim.stop_time = get_time_in_ms();
	printf("Total time elapsed: %ld milliseconds\n", sim.stop_time - sim.start_time);

	// Cleanup
	for (int i = 0; i < sim.philo_count; i++)
	{
		pthread_mutex_destroy(&sim.forks[i]);
	}
	free(sim.forks);
	free(philo);

	return (0);
}
