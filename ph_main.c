/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_main.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:25:06 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/05 17:53:52 by iboukhss         ###   ########.fr       */
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

static void	*philo_routine(void *arg)
{
	t_philosopher	*philo;
	long			start_time;
	long			stop_time;
	bool			is_still_running;

	philo = (t_philosopher *)arg;
	pthread_mutex_lock(&philo->time_lock);
	start_time = get_time_in_ms();
	philo->last_meal_time = start_time;
	pthread_mutex_unlock(&philo->time_lock);
	while (1)
	{
		// Check the simulation is still running
		pthread_mutex_lock(&philo->sim->sim_lock);
		is_still_running = philo->sim->is_running;
		pthread_mutex_unlock(&philo->sim->sim_lock);
		if (!is_still_running)
		{
			break ;
		}

		// Thinking
		stop_time = get_time_in_ms();
		printf("%6ld %3d is thinking\n", stop_time - start_time, philo->id);

		// Picking up forks
		if (philo->id % 2 == 0)
		{
			pthread_mutex_lock(philo->left_fork);
			stop_time = get_time_in_ms();
			printf("%6ld %3d has taken a fork\n", stop_time - start_time, philo->id);
			pthread_mutex_lock(philo->right_fork);
			stop_time = get_time_in_ms();
			printf("%6ld %3d has taken a fork\n", stop_time - start_time, philo->id);
		}
		else
		{
			pthread_mutex_lock(philo->right_fork);
			stop_time = get_time_in_ms();
			printf("%6ld %3d has taken a fork\n", stop_time - start_time, philo->id);
			pthread_mutex_lock(philo->left_fork);
			stop_time = get_time_in_ms();
			printf("%6ld %3d has taken a fork\n", stop_time - start_time, philo->id);
		}

		// Eating
		pthread_mutex_lock(&philo->time_lock);
		stop_time = get_time_in_ms();
		philo->last_meal_time = stop_time;
		pthread_mutex_unlock(&philo->time_lock);

		printf("%6ld %3d is eating\n", stop_time - start_time, philo->id);
		usleep(philo->sim->time_to_eat * 1000);

		pthread_mutex_lock(&philo->meal_lock);
		philo->meal_count++;
		pthread_mutex_unlock(&philo->meal_lock);

		// Putting down forks
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);

		// Check the simulation is still running
		pthread_mutex_lock(&philo->sim->sim_lock);
		is_still_running = philo->sim->is_running;
		pthread_mutex_unlock(&philo->sim->sim_lock);
		if (!is_still_running)
		{
			break ;
		}

		// Sleeping
		stop_time = get_time_in_ms();
		printf("%6ld %3d is sleeping\n", stop_time - start_time, philo->id);
		usleep(philo->sim->time_to_sleep * 1000);
	}
	return (NULL);
}

static void	*monitor_routine(void *arg)
{
	t_simulation	*sim;
	long			current_time;
	long			elapsed_time;
	bool			is_still_running;
	int				current_meal_count;

	sim = (t_simulation *)arg;
	while (1)
	{
		// Check that we are still running
		pthread_mutex_lock(&sim->sim_lock);
		is_still_running = sim->is_running;
		pthread_mutex_unlock(&sim->sim_lock);
		if (!is_still_running)
		{
			break ;
		}

		// Wait a little before each simulation check
		usleep(1000);

		for (int i = 0; i < sim->philo_count; i++)
		{
			pthread_mutex_lock(&sim->philos[i].time_lock);
			current_time = get_time_in_ms();
			elapsed_time = current_time - sim->philos[i].last_meal_time;
			pthread_mutex_unlock(&sim->philos[i].time_lock);
			if (elapsed_time > sim->time_to_die)
			{
				printf("%6ld %3d died 💀 (after %ld milliseconds)\n", current_time - sim->start_time, i + 1, elapsed_time);
				pthread_mutex_lock(&sim->sim_lock);
				sim->is_running = false;
				pthread_mutex_unlock(&sim->sim_lock);
				break ;
			}
		}
		if (sim->meals_required > 0)
		{
			for (int i = 0; i < sim->philo_count; i++)
			{
				pthread_mutex_lock(&sim->philos[i].meal_lock);
				current_meal_count = sim->philos[i].meal_count;
				pthread_mutex_unlock(&sim->philos[i].meal_lock);
				if (current_meal_count < sim->meals_required)
				{
					break ;
				}
				if (i == sim->philo_count - 1)
				{
					pthread_mutex_lock(&sim->sim_lock);
					sim->is_running = false;
					pthread_mutex_unlock(&sim->sim_lock);
					break ;
				}
			}
		}
	}
	return (NULL);
}

static int	init_simulation(t_simulation *sim, int argc, char **argv)
{
	sim->is_running = true;
	pthread_mutex_init(&sim->sim_lock, NULL);
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
	pthread_t		monitor;

	if (argc < 5 || argc > 6)
	{
		fprintf(stderr, "Usage: %s <philo_count> <time_to_die> <time_to_eat> <time_to_sleep> [meals_required]\n", argv[0]);
		return (1);
	}

	// Init user input parameters
	init_simulation(&sim, argc, argv);

	// Allocate space
	sim.philos = malloc(sim.philo_count * sizeof(*sim.philos));
	sim.forks = malloc(sim.philo_count * sizeof(*sim.forks));

	// Create forks (mutexes)
	for (int i = 0; i < sim.philo_count; i++)
	{
		pthread_mutex_init(&sim.forks[i], NULL);
	}

	// Create philosophers (threads)
	for (int i = 0; i < sim.philo_count; i++)
	{
		sim.philos[i].id = i + 1;
		sim.philos[i].left_fork = &sim.forks[i];
		sim.philos[i].right_fork = &sim.forks[(i + 1) % sim.philo_count];
		sim.philos[i].meal_count = 0;
		sim.philos[i].last_meal_time = -1;
		sim.philos[i].sim = &sim;
		pthread_mutex_init(&sim.philos[i].meal_lock, NULL);
		pthread_mutex_init(&sim.philos[i].time_lock, NULL);
	}

	// Start timer
	sim.start_time = get_time_in_ms();

	for (int i = 0; i < sim.philo_count; i++)
	{
		pthread_create(&sim.philos[i].thread, NULL, philo_routine, &sim.philos[i]);
	}

	// Create monitor thread
	pthread_create(&monitor, NULL, monitor_routine, &sim);
	pthread_join(monitor, NULL);

	// Wait for all threads to join
	for (int i = 0; i < sim.philo_count; i++)
	{
		pthread_join(sim.philos[i].thread, NULL);
	}

	// Stop timer
	sim.stop_time = get_time_in_ms();
	printf("Total time elapsed: %ld milliseconds\n", sim.stop_time - sim.start_time);

	// Cleanup
	for (int i = 0; i < sim.philo_count; i++)
	{
		pthread_mutex_destroy(&sim.forks[i]);
		pthread_mutex_destroy(&sim.philos[i].meal_lock);
		pthread_mutex_destroy(&sim.philos[i].time_lock);
	}

	pthread_mutex_destroy(&sim.sim_lock);

	free(sim.forks);
	free(sim.philos);

	return (0);
}
