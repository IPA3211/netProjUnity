#pragma once
#include "headers.h"
#include "clientHandler.h"

struct room
{
    client *_owner;
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
