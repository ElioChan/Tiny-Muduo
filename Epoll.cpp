#include "Epoll.h"
#include "util.h"
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 100


Epoll::Epoll() : epfd(-1), events(nullptr) {
    // constructor
    epfd = epoll_create1(EPOLL_CLOEXEC);
    errif(epfd == -1, "epoll create error");
    events = new epoll_event[MAX_EVENTS];
    bzero(events, sizeof(*events) * MAX_EVENTS);
}

Epoll::~Epoll() {
    // deconstructor
    if(epfd != -1) {
        close(epfd);
        epfd = -1;
    }
    delete [] events;
}

void Epoll::addFd(int fd, uint32_t op) {
    // 添加监听事件 待添加文件描述符为fd 操作事件为op
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.data.fd = fd;
    event.events = op;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == -1, "epoll add error");
}

std::vector<epoll_event> Epoll::poll(int timeout) {
    // 轮询epoll事件 timeout为超时事件
    // 逻辑 获取所有的事件 然后加入事件
    std::vector<epoll_event> activeEvents;
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
    errif(nfds == -1, "epoll_wait error");
    for(int i = 0; i < nfds; i++) {
        activeEvents.push_back(events[i]);
    }
    return activeEvents;
}






