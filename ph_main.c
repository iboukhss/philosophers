/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_main.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:25:06 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/11 18:33:36 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "ph_main.h"

static long	get_time_in_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

static void	log_philo_state(t_philosopher *philo, const char *msg)
{
	pthread_mutex_lock(&philo->sim->log_lock);
	printf("%7ld %4d %s\n", get_time_in_ms() - philo->start_time, philo->id, msg);
	pthread_mutex_unlock(&philo->sim->log_lock);
}

static bool	simulation_is_running(t_simulation *sim)
{
	bool	is_running;

	pthread_mutex_lock(&sim->run_lock);
	is_running = sim->is_running;
	pthread_mutex_unlock(&sim->run_lock);
	return (is_running);
}

static void	*philo_routine(void *arg)
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

static bool	neighbors_are_eating(t_philosopher *philo)
{
	bool	left_eating;
	bool	right_eating;

	pthread_mutex_lock(&philo->left->state_lock);
	left_eating = (philo->left->state == EATING);
	pthread_mutex_unlock(&philo->left->state_lock);
	pthread_mutex_lock(&philo->right->state_lock);
	right_eating = (philo->right->state == EATING);
	pthread_mutex_unlock(&philo->right->state_lock);
	return (left_eating || right_eating);
}

static void	*waiter_routine(void *arg)
{
	t_simulation	*sim;
	t_philosopher	*wait;
	t_philosopher	*new_wait;
	t_philosopher	*req;

	sim = (t_simulation *)arg;
	while (simulation_is_running(sim))
	{
		// Check the wait queue first
		while (1)
		{
			wait = peek(&sim->wait_queue);

			if (!wait)
			{
				break ;
			}

			// Ping pong back and forth with the request and wait queue
			if (neighbors_are_eating(wait))
			{
				break ;
			}
			else
			{
				pthread_mutex_lock(&wait->state_lock);
				wait->state = EATING;
				pthread_mutex_unlock(&wait->state_lock);
				dequeue(&sim->wait_queue);
			}
		}

		// Check the request queue right after
		req = peek(&sim->req_queue);

		if (req)
		{
			// Special case, requested item is a neighbor of #1 in wait queue
			if (wait && (req == wait->left || req == wait->right))
			{
				new_wait = dequeue(&sim->req_queue);
				enqueue(&sim->wait_queue, new_wait);
			}
			else if (neighbors_are_eating(req))
			{
				new_wait = dequeue(&sim->req_queue);
				enqueue(&sim->wait_queue, new_wait);
			}
			else
			{
				pthread_mutex_lock(&req->state_lock);
				req->state = EATING;
				pthread_mutex_unlock(&req->state_lock);
				dequeue(&sim->req_queue);
			}
		}
		usleep(500);
	}
	return (NULL);
}

static void	*monitor_routine(void *arg)
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
