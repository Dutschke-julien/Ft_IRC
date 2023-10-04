//
// Created by Antoine Ho on 9/26/23.
//

#include "Serveur.hpp"

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

	del_space = string.begin();
	while (*del_space == ' '){
		string.erase(del_space);
	}
//    if (string == "#test\r\n") {
//        std::cout << "test\n";
//        error = ":yourserver 475 aho #test :Cannot join channel(wrong key)\r\n";
//        send(fd_key, error.c_str(), error.length(), 0);
//        return;
//    }
//    if (string == "#test2\r\n") {
//        std::cout << "test2\n";
//        error = ":yourserver 403 aho : bad channel mask\r\n";
//        send(fd_key, error.c_str(), error.length(), 0);
//        return;
//    }
//    if (string == "#test3\r\n") {
//        std::cout << "test3\n";
//        error = ":yourserver 461 aho /join: not enough parameters\r\n";
//        send(fd_key, error.c_str(), error.length(), 0);
//        return;
//    }
//    if (string == "#test4\r\n") {
//        std::cout << "test4\n";
//        error = ":yourserver 474 aho #test4 :You are banned from this channel\r\n";
//        send(fd_key, error.c_str(), error.length(), 0);
//        return;
//    }
//    if (string == "#test5\r\n") {
//        std::cout << "test5\n";
//        error = ":yourserver 400 aho #test5 :Unexpected error from a /Join command\r\n";
//        send(fd_key, error.c_str(), error.length(), 0);
//        return;
//    }

    string.erase(string.find_last_of('\n'), 1);
    string.erase(string.find_last_of('\r'), 1);

    if (string == ":") {
	    std::cout << "send : verif system" << std::endl;
        error = ":yourserver 001 aho :OK\r\n";
        send(fd_key, error.c_str(), error.length(), 0);
        return;
    }
    if (string.empty()) {
	    std::cout << "send : not enough parameters (empty)" << std::endl;
        error = ":yourserver 461 " + _mapClients[fd_key].get_nickname() + "/join : not enough parameters\r\n";
        send(fd_key, error.c_str(), error.length(), 0);
        return;
    }
	it = string.begin();
    if (*it != '#' && ++it != string.end()) {
	    error = ":yourserver 461 " + _mapClients[fd_key].get_nickname() + "/join : not enough parameters\r\n";
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
		std::cout << "send : bad channel mask (too much key)" << std::endl;
		error = ":yourserver 403 aho : bad channel mask\r\n";
        send(fd_key, error.c_str(), error.length(), 0);
		return;
	} else {
		it_firstchannel = list_channel.begin();
		while (it_firstchannel != list_channel.end()) {
			if (verif_name(*it_firstchannel) == 1) {
                if (list_key.empty())
                    ret_status = -3;
                else {
                    ret_status = _listChannel[*it_firstchannel].add_client(fd_key, *(list_key.begin()));
                }
				switch (ret_status) {
					case -1:
                        std::cout << "send : banned\n";
						error = ":yourserver 474 " + _mapClients[fd_key].get_nickname() + " #" + *it_firstchannel + " :You are banned from this channel\r\n";
						send(fd_key, error.c_str(), error.length(), 0);
						break;
					case -2:
                        std::cout << "already registered\n";
                        _mapClients[fd_key].set_current_channel(*it_firstchannel);
						break;
					case -3:
						std::cout << "send : wrong key\n";
                        error = ":yourserver 475 " + _mapClients[fd_key].get_nickname() + " #" + *it_firstchannel + " :Cannot join channel(wrong key)\r\n";
                        std::cout << error << " = error created\n";
						send(fd_key, error.c_str(), error.length(), 0);
						break;
					case 2:
                        _mapClients[fd_key].set_current_channel(*it_firstchannel);
						std::cout << "ok with key\n";
						list_key.erase(list_key.begin());
						break;
					default:
                        std::cout << "send : default\n";
						error = ":yourserver 400 " + _mapClients[fd_key].get_nickname() + " JOIN :Unexpected error from a /Join command\r\n";
						send(fd_key, error.c_str(), error.length(), 0);
						break;
				}
			} else if (list_key.empty()) {
				std::cout << "channel created without key == " << *it_firstchannel << std::endl;
                _mapClients[fd_key].set_current_channel(*it_firstchannel);
				_listChannel[*it_firstchannel] = Channel(fd_key);
			} else {
				std::cout << "channel created with key == " << *it_firstchannel << " && " << *(list_key.begin()) << std::endl;
				_listChannel[*it_firstchannel] = Channel(fd_key, *(list_key.begin()));
                _mapClients[fd_key].set_current_channel(*it_firstchannel);
				list_key.erase(list_key.begin());
			}
			list_channel.erase(it_firstchannel);
		}
	}
}