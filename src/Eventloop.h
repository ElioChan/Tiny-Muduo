#pragma once
#include <functional>

class Epoll;
class Channel;
class ThreadPoll;

class Eventloop {
private:
    Epoll *ep;
    ThreadPoll *threadPoll;
    bool quit;
public:
    Eventloop();
    ~Eventloop();
    
    void loop();
    void updateChannel(Channel*);
    void addThread(std::function<void()>);
};