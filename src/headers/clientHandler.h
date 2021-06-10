#pragma once 

#include "headers.h"

typedef int SOCKET;

struct client {
	int _status;
	SOCKET _sock;
	std::string _name;
    std::future<void> _handler;

	client(){
		_status = CLT_STAT_NOCNT;
		_sock = -1;
		_name = "";
	}
};

void clientHandler(client *c);