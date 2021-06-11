#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <memory>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <future>
#include <mutex>
#include <vector>
#include <sstream>

// ROOM STATUS
#define ROOM_STAT_ERROR     0   // NO ROOM
#define ROOM_STAT_OPENED    1   // ROOM OPENED
#define ROOM_STAT_FULL      2   // ROOM FULL
#define ROOM_STAT_PLAYING   3   // ROOM NOW PLAYING

// ROOM LOCK STATUS
#define ROOM_LOCKED     1       // ROOM LOCKED
#define ROOM_UNLOCKED   0       // ROOM UNLOCKED

// CLIENT STATUS
#define CLT_STAT_NOCNT      0   // CLIENT NOT CONNECTED
#define CLT_STAT_ONROBBY    1   // CLIENT ON ROBBY
#define CLT_STAT_ONROOM     2   // CLIENT ON ROOM
#define CLT_STAT_ONPLAY     3   // CLIENT ON PLAYING
#define CLT_STAT_ACCESSING  4   // CLIENT TRY TO ACCESS ROBBY SERVER

// COMMEND 
#define CMD_SEND_CHAT   1       // TO SEND CHAT
#define CMD_RECV_ROOM   2       // TO RECV ROOM INFO
#define CMD_MAKE_ROOM   3       // TO MAKE ROOM
#define CMD_ACCE_ROOM   4       // TO ACCESS ROOM
#define CMD_UPDATE_ROOM 5       // TO UPDATE ROOM INFO
#define CMD_RETRUN_ROOM 6       // SERVER SEND RETURN OF CLIENT's CMD
#define CMD_CLOSE       7       // TO CLOSE CONNECTION WITH SERVER
#define CMD_INVAILD_ROOM 8

#define BUF_SIZE 1024