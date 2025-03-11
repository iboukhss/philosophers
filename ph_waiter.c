/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_waiter.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 21:06:31 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/11 21:15:52 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ph_main.h"

#include <unistd.h>

void	*waiter_routine(void *arg)
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
