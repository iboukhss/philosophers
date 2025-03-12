/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_waiter.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 21:06:31 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/11 23:50:57 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ph_main.h"

#include <unistd.h>

static void	process_wait_queue(t_simulation *sim)
{
	t_philosopher	*wait;

	while (1)
	{
		wait = peek(&sim->wait_queue);
		if (!wait)
		{
			return ;
		}
		if (neighbors_are_eating(wait))
		{
			return ;
		}
		else
		{
			pthread_mutex_lock(&wait->state_lock);
			wait->state = EATING;
			pthread_mutex_unlock(&wait->state_lock);
			dequeue(&sim->wait_queue);
		}
	}
}

static void	process_request_queue(t_simulation *sim)
{
	t_philosopher	*req;
	t_philosopher	*wait;
	t_philosopher	*new_wait;

	req = peek(&sim->req_queue);
	if (req)
	{
		wait = peek(&sim->wait_queue);
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
}

void	*waiter_routine(void *arg)
{
	t_simulation	*sim;

	sim = (t_simulation *)arg;
	while (simulation_is_running(sim))
	{
		process_wait_queue(sim);
		process_request_queue(sim);
		usleep(500);
	}
	return (NULL);
}
