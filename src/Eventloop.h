#pragma once

#include "Epoll.h"
#include "Channel.h"

class Eventloop {
private:
    Epoll *ep;
    bool quit;
public:
    Eventloop();
    ~Eventloop();
    
    void loop();
    void updateChannel(Channel*);
};