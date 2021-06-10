#pragma once 

#include "headers.h"
#include <atomic>
#include <vector>
#include <iostream>

class threadHendler{
private:
    bool killHandlerFlag = false;
    void init();
    std::future<void> _hendlerThread;
    std::vector<std::future<void>> _threads;

public:
    threadHendler();
    void pushThread(std::future<void>);
    int getThreadNum();
    void threadHendle();
    void closeHendler();
    ~threadHendler();
};

void threadHendler::init(){
    _hendlerThread = std::async(threadHendle, this);
}

threadHendler::threadHendler(){
    init();
}

void threadHendler::pushThread(std::future<void> bb){
    _threads.push_back(bb);
}

int threadHendler::getThreadNum(){
    _threads.size();
}

void threadHendler::closeHendler(){
    killHandlerFlag = true;
    _hendlerThread.get();
}

void threadHendler::threadHendle(){
    while(!killHandlerFlag){
        for(auto iter = _threads.begin(); iter != _threads.end(); iter++){
            auto status = iter->wait_for(std::chrono::microseconds(1));
            if(status == std::future_status::ready){
                iter->get();
                _threads.erase(iter);
                iter--;

                std::cout << "THREAD HANDLER : Thread out " << "size is"  << getThreadNum() << std::endl;
            }
        }
    }
}

threadHendler::~threadHendler(){
    closeHendler();
}