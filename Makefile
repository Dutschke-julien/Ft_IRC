# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jdutschk <jdutschk@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/07/24 11:12:17 by jdutschk          #+#    #+#              #
#    Updated: 2023/07/24 11:15:17 by jdutschk         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Makefile pour compiler le programme ircserv en C++

# Nom de l'exécutable
NAME = ircserv

# Compilateur
CXX = c++

# Flags de compilation
CXXFLAGS = -Wall -Werror -Wextra -std=c++98

# Fichiers sources du programme
SOURCES =   main.cpp\
			serveur.cpp\
			client.cpp\

# Fichiers objets

all: $(NAME)

$(NAME):
	$(CXX) $(CXXFLAGS) $(SOURCES)-o $(NAME)

clean:
	

fclean: clean
	rm -f $(NAME)

re : fclean all
