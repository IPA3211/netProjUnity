#pragma once

struct packet
{
    char cmd;
    char data[1024];
};
