/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_main.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:25:06 by iboukhss          #+#    #+#             */
/*   Updated: 2025/02/19 22:12:48 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>

#include "ph_main.h"

int	main(int argc, char *argv[])
{
	t_simulation	sim;

	if (argc < 5 || argc > 6)
	{
		fprintf(stderr, "Usage: %s <philo_count> <time_to_die> <time_to_eat> <time_to_sleep> [meals_required]\n", argv[0]);
		return (1);
	}
	sim.philo_count = atoi(argv[1]);
	sim.time_to_die = atoi(argv[2]);
	sim.time_to_eat = atoi(argv[3]);
	sim.time_to_sleep = atoi(argv[4]);
	sim.meals_required = argc == 6 ? atoi(argv[5]) : -1;
	return (0);
}
