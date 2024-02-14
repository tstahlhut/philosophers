/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tstahlhu <tstahlhu@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 13:15:39 by tstahlhu          #+#    #+#             */
/*   Updated: 2024/02/14 18:44:13 by tstahlhu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/* ft_atoi: Converts char* to int
	undefined behaviour if char* is bigger than MAX_INT */

int	ft_atoi(const char *nptr)
{
	int	i;
	int	ret;
	int	sign;

	i = 0;
	sign = 1;
	ret = 0;
	while ((nptr[i] == 32) || ((nptr[i] >= 9) && (nptr[i] <= 13)))
		i++;
	if ((nptr[i] == '-') && (ft_isdigit(nptr[i + 1]) == 1))
	{
		sign = sign * (-1);
		i++;
	}
	else if (nptr[i] == '+')
		i++;
	while ('0' <= nptr[i] && nptr[i] <= '9')
	{
		ret = ret * 10 + nptr[i] - '0';
		i++;
	}
	return (ret * sign);
}

/* ft_isdigit: checks if char is a digit
	if it is, returns 1
	if it is not a digit, returns 0 */

int	ft_isdigit(char c)
{
	if (47 < c && c < 58)
		return (1);
	else
		return (0);
}

/* ft_isint: checks if string is a positive int
	thus if it is between 0 and MAX_INT 
	if it is, returns 1
	if it is not, returns 0 */

int	ft_isint(char *str)
{
	int		i;

	i = 0;
	while (str[i] != '\0')
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	if (i > 10)
		return (0);
	if (i == 10)
	{
		i = 0;
		while (i < 10 && str[i] >= MAX_INT_STR[i])
		{
			if (str[i] > MAX_INT_STR[i])
				return (0);
			i++;
		}
	}
	return (1);
}

/* check_input: user input is checked:
 * 1. number of arguments being passed (4 required, 5 possible)
 * 2. only positive number of size of an int are allowed to be passed
   3. only max. 500 philosophers allowed to prevent program from overloading
	if check is passed, function returns 0
	in case of error, returns 1 and prints an error message for the user */

int	check_input(int argc, char **argv)
{
	int	i;

	if (argc < 5 || argc > 6)
	{
		printf("Error: User input: Min. 4 arguments, max. 5 arguments\n");
		return (1);
	}
	i = 1;
	while (i < argc)
	{
		if (!ft_isint(argv[i]))
		{
			printf("Error: Input (%s): ", argv[i]);
			printf("Only positive numbers from 0 to 2147483647 are valid\n");
			return (1);
		}
		i++;
	}
	if (ft_atoi(argv[1]) > 500)
		return (printf("Error: Only max. 500 philosophers allowed\n"), 1);
	return (0);
}
