/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_waiter.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 21:06:31 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/15 15:21:01 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ph_main.h"

#include <unistd.h>

static void	process_wait_queue(t_simulation *sim)
{
	t_philosopher	*wait;

	while (simulation_is_running(sim))
	{
		wait = peek(&sim->wait_queue);
		if (!wait)
		{
			return ;
		}
		if (forks_are_available(wait))
		{
			take_forks(wait);
			dequeue(&sim->wait_queue);
		}
		usleep(500);
	}
}

static void	process_request_queue(t_simulation *sim)
{
	t_philosopher	*req;

	while (simulation_is_running(sim))
	{
		req = peek(&sim->req_queue);
		if (!req)
		{
			return ;
		}
		if (forks_are_available(req))
		{
			take_forks(req);
			dequeue(&sim->req_queue);
		}
		else
		{
			dequeue(&sim->req_queue);
			enqueue(&sim->wait_queue, req);
		}
	}
}

void	*waiter_routine(void *arg)
{
	t_simulation	*sim;

	sim = (t_simulation *)arg;
	while (simulation_is_running(sim))
	{
		process_wait_queue(sim);
		process_request_queue(sim);
	}
	return (NULL);
}
