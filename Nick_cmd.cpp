/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick_cmd.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdutschk <jdutschk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/04 17:56:57 by jdutschk          #+#    #+#             */
/*   Updated: 2023/10/19 15:32:50 by jdutschk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Serveur.hpp"

void Serveur::cmd_Nick(std::string cmd, int fd_key)
{
	if (_mapClients[fd_key].get_status() == NO_PASS)
	{
		_mapClients[fd_key].set_status(KICK);
		return;
	}
    std::string pseudonyme = cmd.erase(cmd.find_last_not_of("\r\n") + 1);

	for (std::string::iterator it = pseudonyme.begin(); *it == ' '; it++)
	{
		pseudonyme.erase(it);
	}

    // Vérifie si le pseudonyme est vide
	if (_mapClients[fd_key].get_status() == NO_NICK)
		_mapClients[fd_key].set_status(NO_USERNAME);
    if (pseudonyme.empty())
    {
        // Pseudonyme manquant, envoie un message d'erreur
        std::string erreur = ":42Mulhouse 431 * :Pseudonyme manquant. Utilisation : /NICK <pseudonyme>\r\n";
        send(fd_key, erreur.c_str(), erreur.length(), 0);
	return;
    }
    if (pseudonyme.find_first_of("#@$!:%&*,.- ") != std::string::npos)
    {
        // Caractères non autorisés dans le pseudonyme, envoie un message d'erreur
        std::string erreur = ":42Mulhouse 432 * :Caractères non autorisés dans le pseudonyme\r\n";
		std::cout << "voici la chaine [" << pseudonyme << "]" << std::endl;
        send(fd_key, erreur.c_str(), erreur.length(), 0);
	return ;
    }




	 if (isNickTaken(_name_used, pseudonyme))
	{
		std::string  message2 = ":" + _mapClients[fd_key].get_nickname() + " NICK ";
		while(isNickTaken(_name_used, pseudonyme))
			pseudonyme += "_";
		message2 += pseudonyme + "\r\n";
        std::string bienvenue = ":42Mulhouse 001 " + pseudonyme + " :Bienvenue sur le serveur IRC\r\n";
        send(fd_key, bienvenue.c_str(), bienvenue.length(), 0);
		send(fd_key, message2.c_str(), message2.length(), 0);
		del_Nick_toks(_name_used, _mapClients[fd_key].get_nickname());
		_name_used.push_back(pseudonyme);
		_mapClients[fd_key].set_nickname(pseudonyme);
	}
    else
    {
        // Pseudonyme valide, envoie un message de bienvenue
        std::string bienvenue = ":42Mulhouse 001 " + pseudonyme + " :Bienvenue sur le serveur IRC\r\n";
        send(fd_key, bienvenue.c_str(), bienvenue.length(), 0);
		_name_used.push_back(pseudonyme);
		del_Nick_toks(_name_used, _mapClients[fd_key].get_nickname());
		_mapClients[fd_key].set_nickname(pseudonyme);
    }
}
