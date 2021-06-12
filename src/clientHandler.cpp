#include "headers/clientHandler.h"
#include "headers/robbyServer.h"
#include "headers/packet.h"

extern robbyServer *server;

std::vector<std::string> split(std::string in, char delimiter){
    std::vector<std::string> ans;
    std::stringstream ss(in);
    std::string temp;

    while(getline(ss, temp, delimiter)){
        ans.push_back(temp);
    }
    return ans;
}

void clientHandler(client *c)
{
    packet p;
    std::cout << "clientHandler : hello" << std::endl;
    recv(c->_sock, &p, sizeof(packet), 0);
    send(c->_sock, &p, sizeof(packet), 0);
    c->_name = p.data;
    std::cout << "clientHandler 1 : " << c-> _name << std::endl;

    while(1){
        memset(&p, 0, sizeof(p));
        int ch = recv(c->_sock, &p, sizeof(p), 0);
        std::cout << "clientHandler 2 : " << p.cmd <<" "<<p.data << std::endl;
        if (ch <= 0){
            close(c->_sock);
            c->_name = "";
            c->_status = CLT_STAT_NOCNT;
            break;
        }

        if(p.cmd == CMD_SEND_CHAT){
            server->broadcastChat(p);
        }
        else if (p.cmd == CMD_RECV_ROOM){
            auto temp = server->getRoomList();
            for(int i = 0; i < temp.size(); i++)
                send(c->_sock, &temp[i], sizeof(packet), 0);
        }
        else if (p.cmd == CMD_MAKE_ROOM){
            auto temp = split(std::string(p.data), '\\');
            room newRoom;
            newRoom._owner = c;
            strcpy(newRoom._ip, temp[0].c_str());
            strcpy(newRoom._port, temp[1].c_str());
            strcpy(newRoom._name, temp[2].c_str());
            
            std::cout <<"clientHandler make room: " << temp[3].size() << std::endl;
            if(temp[3].size() > 0){
                strcpy(newRoom._passwd, temp[3].c_str());
                newRoom._isLocked = ROOM_LOCKED;
                temp[3] = "";
            }
            else
                newRoom._isLocked = ROOM_UNLOCKED;

            newRoom._status = ROOM_STAT_OPENED;

            p.cmd = CMD_RETRUN_ROOM;
            strcpy(p.data, std::to_string(server->openRoom(newRoom)).c_str());
            send(c->_sock, &p, sizeof(packet), 0);
        }
        else if (p.cmd == CMD_ACCE_ROOM){
            std::cout <<"clientHandler : hello" << std::endl;
            auto temp = split(std::string(p.data), '\\');
            std::cout <<"clientHandler : hello2" << std::endl;
            std::cout <<"clientHandler : hello2 " << std::stoi(temp[0]) << std::endl;
            packet tt;
            if(temp[1].size() > 0){
                std::cout <<"clientHandler : wow "<< temp[1].size() << " : " << temp[1]<< std::endl;
                tt = server->getRoomInfo(std::stoi(temp[0]), temp[1]);
                temp[1] = "";
            }
            else 
                std::cout <<"clientHandler : vov" << std::endl;
                tt = server->getRoomInfo(std::stoi(temp[0]), "");
            
            send(c->_sock, &tt, sizeof(tt), 0);
            std::cout <<"clientHandler : bye"<< std::endl;
        }
        else if (p.cmd == CMD_UPDATE_ROOM){
            auto temp = split(std::string(p.data), '\\');
            room newRoom;
            newRoom._owner = c;
            strcpy(newRoom._ip, temp[0].c_str());
            strcpy(newRoom._port, temp[1].c_str());
            strcpy(newRoom._name, temp[2].c_str());

            if(temp[3].size() > 0){
                strcpy(newRoom._passwd, temp[3].c_str());
                newRoom._isLocked = ROOM_LOCKED;
            }
            newRoom._status = ROOM_STAT_OPENED;
            server->updateRoom(newRoom);      
        }
        else if (p.cmd == CMD_CLOSE){
            close(c->_sock);
            c->_name = "";
            c->_status = CLT_STAT_NOCNT;
            break;
        }
    }
    std::cout << "clientHandler : good bye" << std::endl;
}

