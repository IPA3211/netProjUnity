#include "robbyServer.h"
#include "threadHandler.h"

robbyServer* server;

int main(int argc, char ** argv){
	std::string cmd;
	if(argc != 2){
		std::cout << "no port num" << std::endl;
		return 0;
	}
	server = &robbyServer(argv[1]);
	server->serverStart();
	while(1){
		std::cin >> cmd;
		if(cmd == "end"){

		}
		if(cmd == "bro"){
			
		}
	}
}
