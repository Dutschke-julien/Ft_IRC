#include "channel.hpp"

Channel::Channel() {}

Channel::~Channel() {}

Channel::Channel(int fd) {
    _invite_only = 1;
    _topic_restriction = 1;
    _password_set = 1;
    _limit_client = 0;
	_full_list.push_back(fd);
	_host = fd;
}

Channel::Channel(int fd, std::string pass) {
	_full_list.push_back(fd);
	_pass = pass;
    _invite_only = 1;
    _topic_restriction = 1;
    _password_set = 1;
    _limit_client = 0;
	_host = fd;
}

int Channel::find_host(int fd_key) {
	if (fd_key == _host)
		return (1);
	return (0);
}

int Channel::find_client(int fd_key) {
    for (std::list<int>::iterator i = _full_list.begin(); i != _full_list.end() ; i++) {
        if (*i == fd_key)
            return (1);
    }
    return (0);
}

int Channel::find_oper(int fd_key) {
    for (std::list<int>::iterator i = _oper.begin(); i != _oper.end() ; i++) {
        if (*i == fd_key)
            return (1);
    }
    return (0);
}

std::list<int> Channel::get_list_user() {
    return _full_list;
}

int Channel::get_topic_restriction() {
    return _topic_restriction;
}

void Channel::set_topic(std::string topic) {
    _topic = topic;
}

std::string Channel::get_topic() {
    return _topic;
}

int Channel::add_client(int fd_key) {
    std::cout << "entering add_client function\n";
	for (std::list<int>::const_iterator it_find = _ban.begin();
	it_find != _ban.end(); it_find++) {
		if (*it_find == fd_key) {
            std::cout << "exit add_client1 function\n";
            return -1;
        }
	}
	for (std::list<int>::const_iterator it = _full_list.begin();
	     it != _full_list.end(); it++) {
		if (*it == fd_key) {
            std::cout << "exit add_client2 function\n";
            return -2;
        }
	}
	_full_list.push_back(fd_key);
    std::cout << "exit add_client3 function\n";
	return 0;
}

int Channel::add_client(int fd_key, std::string pass) {
    std::cout << "entering add_client_pass function\n";
	if (_pass.empty()) {
        add_client(fd_key);
        std::cout << "exiting add_client_pass2 function\n";
        return 2;
        }
    else {
        if (pass != _pass) {
            std::cout << "exiting add_client_pass1 function\n";
            return (-3);
		}
	}
	return (add_client(fd_key));
}

std::string Channel::get_pass() {
    return _pass;
}

int Channel::add_client_ban() {
	return 0;
}

void Channel::remove_client(int fd_key) {
	_full_list.remove(fd_key);
}

void Channel::remove_oper(int fd_key) {
	_oper.remove(fd_key);
}

void Channel::set_mode_topic(int flag) {
	_topic_restriction = flag;
}

void Channel::set_mode_invite(int flag) {
	_invite_only = flag;
}

void Channel::set_password(std::string pass, int flag) {
	_pass = pass;
	_password_set = flag;
}

void Channel::set_limit(std::string number) {
	_limit_client = atol(number.c_str());
}

int Channel::add_oper(int fd_key) {
	_oper.push_back(fd_key);
	return 0;
}

int Channel::get_invite_only() {
	return _invite_only;
}