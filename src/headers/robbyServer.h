#pragma once 

#include "headers.h"
#include "clientHandler.h"
#include "roomHandler.h"
#include "packet.h"

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)

#define MAX_PLAYER 100

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

	int openRoom(room);
	void updateRoom(room);
	std::vector<packet> getRoomList();
	void broadcastChat(packet);

	~robbyServer();
};
