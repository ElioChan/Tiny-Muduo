#pragma once
#include <sys/epoll.h>
#include <functional>

class Eventloop;
class Socket;

class Channel
{
private:
    Eventloop *loop;
    int fd;
    uint32_t ready;  // io事件信号 EPOLLIN / EPOLLOUT 判断读写
    uint32_t events;
    uint32_t revents;
    bool inEpoll;
    bool useThreadPool;
    std::function<void()> ReadCallback;
    std::function<void()> WriteCallback;
public:
    Channel(Eventloop *_loop, int _fd);
    ~Channel();

    void enableReading();
    void handleEvent();

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    uint32_t getReady();
    bool getInEpoll();

    void setEvents(uint32_t);
    void setRevents(uint32_t);
    void setReady(uint32_t);
    void setInEpoll(bool _in = true);
    void setUseThreadPool(bool use = false);
    void setReadCallback(std::function<void()>);
    void setWriteCallback(std::function<void()>);

    void useET();
};