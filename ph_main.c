/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_main.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:25:06 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/12 02:57:36 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ph_main.h"

#include <stdio.h>

int	main(int argc, char **argv)
{
	t_simulation	sim;
	pthread_t		monitor;
	pthread_t		waiter;

	if (argc < 5 || argc > 6)
	{
		printf("Usage: %s <philo_count> <time_to_die> <time_to_eat> <time_to_sleep> [meals_required]\n", argv[0]);
		return (1);
	}
	init_simulation(&sim, argc, argv);
	sim.start_time = get_time_in_ms();
	pthread_create(&monitor, NULL, monitor_routine, &sim);
	pthread_create(&waiter, NULL, waiter_routine, &sim);
	for (int i = 0; i < sim.philo_count; i++)
	{
		pthread_create(&sim.philos[i].thread, NULL, philo_routine, &sim.philos[i]);
	}
	pthread_join(monitor, NULL);
	pthread_join(waiter, NULL);
	for (int i = 0; i < sim.philo_count; i++)
	{
		pthread_join(sim.philos[i].thread, NULL);
	}
	sim.stop_time = get_time_in_ms();
	printf("Total time elapsed: %ld milliseconds\n", sim.stop_time - sim.start_time);
	destroy_simulation(&sim);
	return (0);
}
