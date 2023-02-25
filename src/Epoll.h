#pragma once
#include <sys/epoll.h>
#include <vector>
#include "Channel.h"

class Epoll{
private:
    int epfd;
    struct epoll_event *events;

public:
    Epoll();
    ~Epoll();

    std::vector<Channel*> poll(int timeout = -1);

    void updateChannel(Channel*);
    void deleteChannel(Channel*);
};