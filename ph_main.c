/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_main.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:25:06 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/12 14:16:59 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ph_main.h"

#include <stdio.h>
#include <stdlib.h>

static int	init_user_input_parameters(t_simulation *sim, int argc, char **argv)
{
	if (argc < 5 || argc > 6)
	{
		printf("Usage: %s <philo_count> <time_to_die> <time_to_eat> <time_to_sleep> [meals_required]\n", argv[0]);
		return (1);
	}
	sim->philo_count = atoi(argv[1]);
	sim->time_to_die = strtol(argv[2], NULL, 10);
	sim->time_to_eat = strtol(argv[3], NULL, 10);
	sim->time_to_sleep = strtol(argv[4], NULL, 10);
	if (argc == 6)
	{
		sim->meals_required = atoi(argv[5]);
	}
	else
	{
		sim->meals_required = -1;
	}
	return (0);
}

static int	create_threads(t_simulation *sim)
{
	int	i;

	pthread_create(&sim->monitor, NULL, monitor_routine, sim);
	pthread_create(&sim->waiter, NULL, waiter_routine, sim);
	i = 0;
	while (i < sim->philo_count)
	{
		pthread_create(&sim->philos[i].thread, NULL, philo_routine, &sim->philos[i]);
		i++;
	}
	return (0);
}

static int	join_threads(t_simulation *sim)
{
	int	i;

	pthread_join(sim->monitor, NULL);
	pthread_join(sim->waiter, NULL);
	i = 0;
	while (i < sim->philo_count)
	{
		pthread_join(sim->philos[i].thread, NULL);
		i++;
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_simulation	sim;

	if (init_user_input_parameters(&sim, argc, argv) != 0)
	{
		return (1);
	}
	init_simulation(&sim);
	create_threads(&sim);
	join_threads(&sim);
	destroy_simulation(&sim);
	return (0);
}
