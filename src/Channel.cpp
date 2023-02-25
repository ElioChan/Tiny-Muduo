#include "Channel.h"
#include "Epoll.h"
#include "Eventloop.h"
#include "util.h"
#include <unistd.h>

Channel::Channel(Eventloop *_loop, int _fd) 
    : loop(_loop), fd(_fd), events(0), revents(0), ready(0), inEpoll(false), useThreadPool(true) {}


Channel::~Channel() {
    if(fd != -1) {
        close(fd);
        fd = -1;
    }
}

void Channel::enableReading() {
    events |= EPOLLIN | EPOLLPRI;
    loop->updateChannel(this);
}

void Channel::handleEvent(){
    if(ready & (EPOLLIN | EPOLLPRI)) {
        if(useThreadPool) loop->addThread(ReadCallback);
        else ReadCallback();
    } 
    if(ready & EPOLLOUT) {
        if(useThreadPool) loop->addThread(WriteCallback);
        else WriteCallback();
    }
}
 
int Channel::getFd() {
    return fd;
}

uint32_t Channel::getEvents() {
    return events;
}

uint32_t Channel::getRevents() {
    return revents;
}

uint32_t Channel::getReady() {
    return ready;
}

bool Channel::getInEpoll() {
    return inEpoll;
}

void Channel::setEvents(uint32_t _ev) {
    events = _ev;
}

void Channel::setRevents(uint32_t _ev) {
    revents = _ev;
}

void Channel::setReady(uint32_t _ready) {
    ready = _ready;
} 

void Channel::setInEpoll(bool _in) {
    inEpoll = _in;
}

void Channel::setUseThreadPool(bool _use) {
    useThreadPool = _use;
}

void Channel::setReadCallback(std::function<void()> _cb) {
    ReadCallback = _cb;
}

void Channel::setWriteCallback(std::function<void()> _cb) {
    WriteCallback = _cb;
}

void Channel::useET() {
    events |= EPOLLET;
    loop->updateChannel(this);
} 





