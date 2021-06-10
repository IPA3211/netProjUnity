#pragma once 

#include "headers.h"
#include "packet.h"

#define CLT_STAT_NOCNT      0
#define CLT_STAT_ONROBBY    1
#define CLT_STAT_ONROOM     2
#define CLT_STAT_ONPLAY     3
#define CLT_STAT_ACCESSING  4

#define CMD_SEND_CHAT   1
#define CMD_RECV_ROOM   2
#define CMD_MAKE_ROOM   3
#define CMD_ACCE_ROOM   4
#define CMD_CLOSE       5

#define BUF_SIZE 1024

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

void clientHandler(client *c){
    char buf[BUF_SIZE];
    send(c->_sock, "::CON::", 7 , 0);
    recv(c->_sock, buf, BUF_SIZE, 0);
    c->_name = buf;
    
    packet p;
    while(1){
        int ch = recv(c->_sock, &p, sizeof(p), 0);
        
        if (ch <= 0){
            close(c->_sock);
            c->_name = "";
            c->_status = CLT_STAT_NOCNT;
        }

        switch (p.cmd)
        {
        case CMD_SEND_CHAT:
            
            /* code */
            break;
        
        case CMD_RECV_ROOM:
            /* code */
            break;

        case CMD_MAKE_ROOM:
            /* code */
            break;

        case CMD_ACCE_ROOM:
            /* code */
            break;
        
        case CMD_CLOSE:
            close(c->_sock);
            c->_name = "";
            c->_status = CLT_STAT_NOCNT;
            break;

        default:
            break;
        }
    }
}