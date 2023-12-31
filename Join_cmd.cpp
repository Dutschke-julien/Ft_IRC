#include "Serveur.hpp"


void Serveur::reply_join(std::string channel, int fd_key) {
    _mapClients[fd_key].set_current_channel(channel);
    std::list<int> user = _listChannel[channel].get_list_user();
	std::string reply;
    for (std::list<int>::iterator i = user.begin(); i != user.end() ; i++) {
		if (*i == fd_key)
			reply = ":" + _mapClients[fd_key].get_nickname() + " JOIN #" + channel + "\r\n";
		else
			reply = ":42Mulhouse PRIVMSG #" + channel
			                    + " :" + _mapClients[fd_key].get_nickname()
			                    + " has joined the channel #" + channel + "\r\n";
		send(*i, reply.c_str(), reply.length(), 0);
	}
	if (!(_listChannel[channel].get_topic().empty()))
		send_topic(_mapClients[fd_key].get_current_channel(), fd_key);

    reply = ":42Mulhouse 353 " + _mapClients[fd_key].get_nickname()
			+ " = #" + channel
			+ " :" ;
    for (std::list<int>::iterator i = user.begin(); i != user.end() ; i++) {
		if (_listChannel[channel].find_oper(*i))
			reply += "@";
		reply += _mapClients[*i].get_nickname() + " ";
    }
	reply += "\r\n";
	send(fd_key, reply.c_str(), reply.length(), 0);
    reply = ":42Mulhouse 366 " + _mapClients[fd_key].get_nickname() + " #" + channel
            + " :End of /NAMES list\r\n";
    send(fd_key, reply.c_str(), reply.length(), 0);
}

void Serveur::cmd_join(std::string string, int fd_key) {
	std::string error;
	std::vector<std::string> list_channel;
	std::vector<std::string> list_key;
	std::string::iterator del_space;
	std::string::iterator it;
	std::vector<std::string>::iterator it_firstchannel;
	std::string::iterator it_tofind;
	std::string word;
	int ret_status;
	int index = 0;

    if (_mapClients[fd_key].get_status() != ALL_OK) {
        return;
    }

	del_space = string.begin();
	while (*del_space == ' '){
		string.erase(del_space);
	}
    string.erase(string.find_last_of('\n'), 1);
    string.erase(string.find_last_of('\r'), 1);

    if (string == ":") {
        return;
    }
    if (string.empty()) {
        error = ":42Mulhouse 461 " + _mapClients[fd_key].get_nickname() + "/join : not enough parameters\r\n";
        send(fd_key, error.c_str(), error.length(), 0);
        return;
    }
	it = string.begin();
    if (*it != '#' && ++it != string.end()) {
	    error = ":42Mulhouse 461 " + _mapClients[fd_key].get_nickname() + "/join : not enough parameters\r\n";
        send(fd_key, error.c_str(), error.length(), 0);
        return;
    } else {
        it_tofind = string.begin();
        while (it_tofind != string.end()){
            while (it_tofind != string.end() && *it_tofind == ' ') {
                it_tofind++;
            }
            while (it_tofind != string.end() && (*it_tofind == '#' || *it_tofind == ',')) {
                if (*it_tofind == '#')
                    index = 1;
                it_tofind++;
            }
            while ( it_tofind != string.end() && *it_tofind != ' ' && *it_tofind != ',') {
                word += *it_tofind;
                it_tofind++;
            }
            if (index == 1 && !(word.empty()))
                list_channel.push_back(word);
            else if (!(word.empty()))
                list_key.push_back(word);
            index = 0;
            word.erase(word.begin(), word.end());
            if (it_tofind != string.end())
                it_tofind++;
        }
    }
	if (list_channel.size() < list_key.size()) {
		error = ":42Mulhouse 403 " + _mapClients[fd_key].get_nickname() + " : bad channel mask\r\n";
        send(fd_key, error.c_str(), error.length(), 0);
		return;
	} else {
		it_firstchannel = list_channel.begin();
		while (it_firstchannel != list_channel.end()) {
			if (verif_name(*it_firstchannel) == 1) {
				if (_listChannel[*it_firstchannel].get_invite_only() == -1
					&& _mapClients[fd_key].find_and_remove_invitation(*it_firstchannel) == 0) {
//					error = ":42Mulhouse KICK #" + *it_firstchannel + " " + _mapClients[fd_key].get_nickname() + "\r\n";
//					send(fd_key, error.c_str(), error.length(), 0);
					error = ":42Mulhouse 473 " + _mapClients[fd_key].get_nickname() + " #" + *it_firstchannel + " :Cannot join channel (invitation only)\r\n";
					send(fd_key, error.c_str(), error.length(), 0);
					return;
				}
                if (_listChannel[*it_firstchannel].get_pass().empty())
                    ret_status = _listChannel[*it_firstchannel].add_client(fd_key);
                else if (list_key.empty())
                    ret_status = -3;
                else
                    ret_status = _listChannel[*it_firstchannel].add_client(fd_key, list_key[0]);
				switch (ret_status) {
					case -1:
						error = ":42Mulhouse 474 " + _mapClients[fd_key].get_nickname() + " #" + *it_firstchannel + " :You are banned from this channel\r\n";
						send(fd_key, error.c_str(), error.length(), 0);
						return;
					case -2:
                        _mapClients[fd_key].set_current_channel(*it_firstchannel);
						return;
					case -3:
                        error = ":42Mulhouse 475 " + _mapClients[fd_key].get_nickname() + " #" + *it_firstchannel + " :Cannot join channel(wrong key)\r\n";
						send(fd_key, error.c_str(), error.length(), 0);
						return;
					case 2:
                        reply_join(*it_firstchannel, fd_key);
						_mapClients[fd_key].add_channel(*it_firstchannel);
						list_key.erase(list_key.begin());
						return;
                    case 0:
                        reply_join(*it_firstchannel, fd_key);
						_mapClients[fd_key].add_channel(*it_firstchannel);
                        return;
					default:
						error = ":42Mulhouse 400 " + _mapClients[fd_key].get_nickname() + " JOIN :Unexpected error from a /Join command\r\n";
						send(fd_key, error.c_str(), error.length(), 0);
						return;
				}
			} else if (list_key.empty()) {
				_listChannel[*it_firstchannel] = Channel(fd_key);
				_mapClients[fd_key].add_channel(*it_firstchannel);
                reply_join(*it_firstchannel, fd_key);
			} else {
				_listChannel[*it_firstchannel] = Channel(fd_key, *(list_key.begin()));
				_mapClients[fd_key].add_channel(*it_firstchannel);
                reply_join(*it_firstchannel, fd_key);
				list_key.erase(list_key.begin());
			}
			list_channel.erase(it_firstchannel);
		}
	}
}