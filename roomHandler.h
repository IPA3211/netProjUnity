#pragma once

#include "headers.h"

#define ROOM_STAT_ERROR     0
#define ROOM_STAT_OPENED    1
#define ROOM_STAT_FULL      2
#define ROOM_STAT_PLAYING   3

#define ROOM_LOCKED     1
#define ROOM_UNLOCKED   0

struct room
{
    SOCKET owner;
    
    char _index;
    char _status;
    char _isLocked;

    char _ip[16];
    char _port[6];

    char _name[20];
    char _passwd[20];

    room(){
        _status = ROOM_STAT_ERROR;
        _isLocked = ROOM_UNLOCKED;
    }
};
