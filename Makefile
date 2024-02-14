# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tstahlhu <tstahlhu@student.42berlin.de>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/09/04 11:56:29 by tstahlhu          #+#    #+#              #
#    Updated: 2024/02/14 19:01:58 by tstahlhu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME =		philo
CC =		cc
CFLAGS =	-Wall -Werror -Wextra

CFILES =	philo.c utils.c check.c init.c monitor.c forks.c
OBJ =		$(patsubst %.c, %.o, $(CFILES))


all:		$(NAME)

$(NAME):	$(OBJ)
			$(CC) $(CFLAGS) -g -pthread $(CFILES) -o $(NAME)

$(OBJ):		$(CFILES)
			$(CC) $(CFLAGS) -c $(CFILES)

clean:
			rm -f $(OBJ)

fclean:		clean
			rm -f $(NAME)

re:		fclean all

.PHONY: all clean fclean re
