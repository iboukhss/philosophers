/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_main.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:25:06 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/12 12:31:33 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ph_main.h"

#include <stdio.h>

static int	create_threads(t_simulation *sim)
{
	pthread_create(&sim->monitor, NULL, monitor_routine, sim);
	pthread_create(&sim->waiter, NULL, waiter_routine, sim);
	for (int i = 0; i < sim->philo_count; i++)
	{
		pthread_create(&sim->philos[i].thread, NULL, philo_routine, &sim->philos[i]);
	}
	return (0);
}

static int	join_threads(t_simulation *sim)
{
	pthread_join(sim->monitor, NULL);
	pthread_join(sim->waiter, NULL);
	for (int i = 0; i < sim->philo_count; i++)
	{
		pthread_join(sim->philos[i].thread, NULL);
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_simulation	sim;

	if (argc < 5 || argc > 6)
	{
		printf("Usage: %s <philo_count> <time_to_die> <time_to_eat> <time_to_sleep> [meals_required]\n", argv[0]);
		return (1);
	}
	init_simulation(&sim, argc, argv);
	create_threads(&sim);
	sim.start_time = get_time_in_ms();
	join_threads(&sim);
	sim.stop_time = get_time_in_ms();
	printf("Total time elapsed: %ld milliseconds\n", sim.stop_time - sim.start_time);
	destroy_simulation(&sim);
	return (0);
}
