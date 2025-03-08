/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_queue.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 23:36:24 by iboukhss          #+#    #+#             */
/*   Updated: 2025/03/07 14:37:24 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ph_main.h"

#include <stdlib.h>

// Not checking any return values because we don't care.

int	init_queue(t_queue *q, int capacity)
{
	q->items = malloc(capacity * sizeof(*q->items));
	q->head = 0;
	q->tail = 0;
	q->length = 0;
	q->capacity = capacity;
	pthread_mutex_init(&q->lock, NULL);
	return (0);
}

int	destroy_queue(t_queue *q)
{
	free(q->items);
	pthread_mutex_destroy(&q->lock);
	return (0);
}

int	enqueue(t_queue *q, t_philosopher *philo)
{
	pthread_mutex_lock(&q->lock);
	q->items[q->tail] = philo;
	q->tail = (q->tail + 1) % q->capacity;
	q->length++;
	pthread_mutex_unlock(&q->lock);
	return (0);
}

t_philosopher	*dequeue(t_queue *q)
{
	t_philosopher	*philo;

	pthread_mutex_lock(&q->lock);
	philo = q->items[q->head];
	q->head = (q->head + 1) % q->capacity;
	q->length--;
	pthread_mutex_unlock(&q->lock);
	return (philo);
}
