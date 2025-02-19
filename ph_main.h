/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ph_main.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iboukhss <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 21:35:24 by iboukhss          #+#    #+#             */
/*   Updated: 2025/02/19 22:13:57 by iboukhss         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PH_MAIN_H
# define PH_MAIN_H

typedef struct s_simulation
{
	int		philo_count;
	long	time_to_die;
	long	time_to_eat;
	long	time_to_sleep;
	int		meals_required;
}	t_simulation;

typedef struct s_philosopher
{
	int				id;
	long			last_meal_time;
	int				meals_eaten;
}	t_philosopher;

#endif
