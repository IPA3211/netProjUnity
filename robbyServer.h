#pragma once 

#include "headers.h"
#include "clientHandler.h"
#include "roomHandler.h"

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)

#define MAX_PLAYER 100

#define CLT_STAT_NOCNT (-1)
#define CLT_STAT_ONROBBY 0
#define CLT_STAT_ONROOM 1
#define CLT_STAT_ONPLAY 2
#define CLT_STAT_ACCESSING 3

typedef int SOCKET;

class robbyServer
{
private:
	SOCKET sock;

	std::recursive_mutex clientLock;
	client *clients[MAX_PLAYER];

	std::recursive_mutex roomLock;
	room *rooms[MAX_PLAYER];

	std::future<void> acceptThread;

	client *acceptClient(SOCKET);
	void loopAccepter();

public:
	robbyServer(char *);

	robbyServer &operator=(const robbyServer &);

	void serverStart();

	void openRoom(room);
	void updateRoom(room);
	void sendRoomList();
	void broadcastChat(packet);

	~robbyServer();
};

robbyServer::robbyServer(char *port)
{
	struct addrinfo hint;
	struct addrinfo *info;

	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE;
	
	if(getaddrinfo(NULL, port, &hint, &info) != 0)
		errorCtrl("getaddrinfo()");
		
	sock = socket(info -> ai_family, info -> ai_socktype, info -> ai_protocol);
	if(sock == INVALID_SOCKET)
		errorCtrl("socket()");
	
	if(bind(sock, info->ai_addr, info->ai_addrlen) == SOCKET_ERROR)
		errorCtrl("bind()");

	if(listen(sock, MAX_PLAYER) == SOCKET_ERROR)
		errorCtrl("listen()");
}

void robbyServer::serverStart(){
	acceptThread = std::async(loopAccepter, this);
}

robbyServer::~robbyServer()
{
}

client *robbyServer::acceptClient(SOCKET sock){
	SOCKET addrSock;
	sockaddr_in clntAddr = {};
	socklen_t clntAddrSize;
	client *newClient = (client *)malloc(sizeof(client));

	clntAddrSize = sizeof(clntAddr);
	addrSock = accept(sock, (sockaddr *)&clntAddr, &clntAddrSize);

	if(addrSock == INVALID_SOCKET){
		errorCtrl("accept()");
	}
	
	newClient->_name = "";
	newClient->_status = CLT_STAT_ACCESSING;
	newClient->_sock = addrSock;

	return newClient;
}

void robbyServer::loopAccepter(){
	while(1){
		client *c ;
		c = acceptClient(sock);

		for(int i = 0; i < MAX_PLAYER; i++){
			if(clients[i] == NULL || clients[i]->_status == CLT_STAT_NOCNT){
				clientLock.lock();
				
				if(clients[i]->_status == CLT_STAT_NOCNT)
					delete(clients[i]);

				clients[i] = c;
				c->_handler = std::async(clientHandler, c);
				c->_status = CLT_STAT_ONROBBY;

				clientLock.unlock();
				break;
			}
		}

		if(c->_status == CLT_STAT_ACCESSING){
			send(c->_sock, "::MAX::", 7, 0);
			close(c->_sock);
			delete(c);
		}
	}
}

void robbyServer::broadcastChat(packet in){
	for(int i = 0; i < MAX_PLAYER; i++){
		clientLock.lock();
		if(clients[i] != NULL && clients[i]->_status == CLT_STAT_ONROBBY)
			send(clients[i]->_sock, &in, sizeof(in), 0);

		clientLock.unlock();
	}
}

void robbyServer::openRoom(room in){
	int index = 0;
	roomLock.lock();
	for(index = 0; index < MAX_PLAYER; index++){
		if(rooms[index] == NULL || rooms[index]->_status == ROOM_STAT_ERROR){

			if(rooms[index]->_status == ROOM_STAT_ERROR)
				delete(rooms[index]);

			rooms[index] = (room *)malloc(sizeof(room));
			memset(rooms[index], 0, sizeof(room));
			
			in._index = index;
			*rooms[index] = in;
			break;
		}
	}
	roomLock.unlock();
}

void robbyServer::updateRoom(room in){
	roomLock.lock();
	*rooms[in._index] = in;
	roomLock.unlock();
}

void robbyServer::sendRoomList(){
	packet in;
	in.cmd = CMD_RECV_ROOM;
	roomLock.lock();
	for(int j = 0; j < MAX_PLAYER; j++){
		if(rooms[j] != NULL && rooms[j]->_status != ROOM_STAT_ERROR){
			std::string dd = std::to_string(rooms[j]->_index);
			dd = dd + "\\" + std::to_string(rooms[j]->_isLocked);
			dd = dd + "\\" + rooms[j]->_name;
			dd = dd + "\\" + rooms[j]->_status;
			strcpy(in.data, dd.c_str());
			
			for(int i = 0; i < MAX_PLAYER; i++){
				clientLock.lock();
				if(clients[i] != NULL && clients[i]->_status == CLT_STAT_ONROBBY)
					send(clients[i]->_sock, &in, sizeof(in), 0);

				clientLock.unlock();
			}
		}
	}
	roomLock.unlock();
}

void errorCtrl(std::string str){
	perror(str.c_str());
	exit(0);
}