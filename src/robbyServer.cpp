#include "headers/robbyServer.h"

void errorCtrl(std::string str){
	perror(str.c_str());
	exit(0);
}

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

	for(int i = 0; i < MAX_PLAYER; i++){
		clients[i] = NULL;
		rooms[i] = NULL;
	}
}

void robbyServer::serverStart(){
	acceptThread = std::async(&robbyServer::loopAccepter, this);
	std::cout << "HELLO :)" << std::endl;
}

robbyServer::~robbyServer()
{
}

client *robbyServer::acceptClient(SOCKET sock){
	SOCKET addrSock;
	sockaddr_in clntAddr = {};
	socklen_t clntAddrSize;
	client *newClient = new client();

	std::cout << "acceptClient : hello" << std::endl;
	clntAddrSize = sizeof(clntAddr);
	addrSock = accept(sock, (sockaddr *)&clntAddr, &clntAddrSize);

	if(addrSock == INVALID_SOCKET){
		errorCtrl("accept()");
	}
	
	newClient->_name = "";
	newClient->_status = CLT_STAT_ACCESSING;
	newClient->_sock = addrSock;

	std::cout << "acceptClient : good bye" << std::endl;
	return newClient;
}

void robbyServer::loopAccepter(){
	std::cout << "loopAccepter : hello" << std::endl;
	while(1){
		client *c ;
		std::cout << "loopAccepter : waiting..." << std::endl;
		c = acceptClient(sock);

		for(int i = 0; i < MAX_PLAYER; i++){
			if(clients[i] == NULL || clients[i]->_status == CLT_STAT_NOCNT){
				clientLock.lock();
				
				if(clients[i] != NULL)
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
	std::cout << "loopAccepter : goodbye" << std::endl;
}

void robbyServer::broadcastChat(packet in){
	std::cout << "broadcastChat : hello" << std::endl;
	for(int i = 0; i < MAX_PLAYER; i++){
		clientLock.lock();
		if(clients[i] != NULL && clients[i]->_status == CLT_STAT_ONROBBY)
			send(clients[i]->_sock, &in, sizeof(in), 0);

		clientLock.unlock();
	}
	std::cout << "broadcastChat : goodbye" << std::endl;
}

int robbyServer::openRoom(room in){
	std::cout << "openRoom : hello" << std::endl;
	in._index = -1;
	int index = 0;
	roomLock.lock();
	for(index = 0; index < MAX_PLAYER; index++){
		if(rooms[index] == NULL || rooms[index]->_status == ROOM_STAT_ERROR){

			if(rooms[index] != NULL)
				delete(rooms[index]);

			rooms[index] = (room *)malloc(sizeof(room));
			memset(rooms[index], 0, sizeof(room));
			
			in._index = index;
			*rooms[index] = in;
			break;
		}
	}
	roomLock.unlock();
	std::cout << "openRoom : good bye" << std::endl;
	return in._index;
}

void robbyServer::updateRoom(room in){
	std::cout << "updateRoom : hello" << std::endl;
	roomLock.lock();
	*rooms[in._index] = in;
	roomLock.unlock();
	std::cout << "updateRoom : good bye" << std::endl;
}

packet robbyServer::getRoomInfo(char roomNum, std::string pass){
	packet newPack;
	if(strcmp(rooms[roomNum]->_passwd, pass.c_str()) == 0 && rooms[roomNum]->_status != ROOM_STAT_ERROR){
		newPack.cmd = CMD_ACCE_ROOM;
		std::string dd = rooms[roomNum]->_ip;
		dd = dd + "\\" + rooms[roomNum]->_port;
		strcpy(newPack.data, dd.c_str());
		return newPack;
	}
	else{
		newPack.cmd = CMD_INVAILD_ROOM;
		return newPack;
	}
}

std::vector<packet> robbyServer::getRoomList(){
	std::cout << "getRoomList : hello" << std::endl;
	std::vector<packet> p;
	packet in;
	in.cmd = CMD_RECV_ROOM;
	roomLock.lock();

	for(int j = 0; j < MAX_PLAYER; j++){
		if(rooms[j] != NULL && rooms[j]->_status != ROOM_STAT_ERROR){
			if(rooms[j]->_owner->_status == CLT_STAT_NOCNT) {
				rooms[j]->_status = ROOM_STAT_ERROR;
				continue;
			}
			std::string dd = std::to_string(rooms[j]->_index);
			dd = dd + "\\" + std::to_string(rooms[j]->_isLocked);
			dd = dd + "\\" + rooms[j]->_name;
			dd = dd + "\\" + std::to_string(rooms[j]->_status);
			strcpy(in.data, dd.c_str());
			p.push_back(in);
		}
	}

	roomLock.unlock();
	std::cout << "getRoomList : good bye" << std::endl;
	return p;
}