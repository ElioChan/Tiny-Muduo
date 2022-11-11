#include "Channel.h"
#include "Epoll.h"
#include "Eventloop.h"
#include "util.h"

Channel::Channel(Eventloop *_loop, int _fd) : loop(_loop), fd(_fd), events(0), revents(0), inEpoll(false) {

}


Channel::~Channel() {

}

void Channel::enableReading() {
    events = EPOLLIN;
    loop->updateChannel(this);
}

void Channel::handleEvent(){
    callback(); 
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

bool Channel::getInEpoll() {
    return inEpoll;
}

void Channel::setcallback(std::function<void()> _cb) {
    callback = _cb;
}

void Channel::setInEpoll() {
    inEpoll = true;
} 

void Channel::setEvents(uint32_t _ev) {
    events = _ev;
}

void Channel::setRevents(uint32_t _ev) {
    revents = _ev;
}





