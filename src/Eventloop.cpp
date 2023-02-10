#include "Eventloop.h"
#include "Epoll.h"
#include "Channel.h"
#include "ThreadPoll.h"
#include <vector>


Eventloop::Eventloop() : ep(nullptr), quit(false), threadPoll(nullptr) {
    ep = new Epoll();
    threadPoll = new ThreadPoll();
}

Eventloop::~Eventloop() {
    delete ep;
}

void Eventloop::loop() {
    /*
    封装轮询函数 调用ep->poll获得活跃事件 然后轮询并处理 处理函数为channel的回调函数 在创建时绑定
    */
    while(!quit) {
        std::vector<Channel*> activeChannels;
        activeChannels = ep->poll();
        for(auto it = activeChannels.begin(); it != activeChannels.end(); ++it) {
            (*it)->handleEvent();
        }
    }
}

void Eventloop::updateChannel(Channel *channel)  {
    ep->updateChannel(channel);
}

void Eventloop::addThread(std::function<void()> func) {
    threadPoll->add(func);
}



