#pragma once
#include "headers.h"

struct room
{
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
