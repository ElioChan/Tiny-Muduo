#pragma once
#include <sys/epoll.h>
#include <functional>

class Eventloop;

class Channel
{
private:
    Eventloop *loop;
    int fd;
    uint32_t events;
    uint32_t revents;
    bool inEpoll;
    std::function<void()> callback;
public:
    Channel(Eventloop *_loop, int _fd);
    ~Channel();

    void enableReading();

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();
    void setInEpoll();

    void handleEvent();
    void setEvents(uint32_t);
    void setRevents(uint32_t);
    void setcallback(std::function<void()>);
};