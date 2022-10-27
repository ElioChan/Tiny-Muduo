#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <vector>
#include <iostream>


// 1. socket创建监听套接字
// 2. 创建服务器地址 并bind 绑定
// 3. 开始监听listen
// 4. 创建eventlist 并加入listen fd 创建epoll_fd
// 5. 调用epoll_wait 得到活跃事件
// 6. 遍历活跃事件并进行处理 分为监听套接字 连接套接字 连接套接字又可以分为 读EPOLLIN 和 写EPOLLOUT 

typedef std::vector<struct epoll_event> EventList;


int main() {
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    int on = 1;
    int nready;
    int lis_fd;
    int epoll_fd;
    int idlefd = open("/dev/null", O_RDONLY | O_CLOEXEC);
    char buf[1024];
    char addr[INET_ADDRSTRLEN];
    struct sockaddr_in srv_addr;
    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof(peer_addr);

    // 1. socket创建监听套接字 设置监听套接字可以重复使用
    if((lis_fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0)) < 0) {
        perror("socket");
        exit(-1);
    }
    setsockopt(lis_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    // 2. 创建服务器地址 并bind 绑定
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(9999);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(lis_fd, (struct sockaddr *) &srv_addr, sizeof(srv_addr)) < 0) {
        perror("bind");
        exit(-1);
    }
    // 3. 开始监听listen
    if(listen(lis_fd, SOMAXCONN) < 0) {
        perror("listen");
        exit(-1);
    }

    // 4. 创建eventlist 并加入listen fd 创建epoll_fd
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);

    struct epoll_event event;
    event.data.fd = lis_fd;
    event.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, lis_fd, &event);

    EventList events(16);

    // 5. 调用epoll_wait 得到活跃事件
    while(1) {
        nready = epoll_wait(epoll_fd, &*events.begin(), static_cast<int>(events.size()), -1);

        if(nready == -1) {
            perror("epoll_wait");
            exit(-1);;
        }
        else if(nready == 0) continue;

        // 若返回事件过多 则动态扩容 
        if(nready == events.size()) events.resize(events.size() * 2);
        // 6. 遍历活跃事件并进行处理 分为监听套接字 连接套接字 连接套接字又可以分为 读EPOLLIN 和 写EPOLLOUT 
        for(int i = 0; i < nready; i++) {
            auto cur_event = events[i];
            // 监听套接字 调用accpet 处理EMFILE异常 以及将连接套接字加入监听
            if(cur_event.data.fd == lis_fd) {
                int conn_fd = accept4(lis_fd, (struct sockaddr *)& peer_addr, &peer_addr_len, SOCK_CLOEXEC | SOCK_NONBLOCK);
                if(conn_fd < 0) {
                    if(errno == EMFILE) {
                        close(idlefd);
                        idlefd = accept4(lis_fd, (struct sockaddr *)& peer_addr, &peer_addr_len, SOCK_CLOEXEC | SOCK_NONBLOCK);
                        close(idlefd);
                    }
                }
                // 连接成功 输出客户端信息
                std::cout << "ip: " << inet_ntop(AF_INET, &peer_addr.sin_addr, addr, sizeof(addr)) 
                <<" ;port: " << ntohs(peer_addr.sin_port) << std::endl;

                event.data.fd = conn_fd;
                event.events = EPOLLIN;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &event);
            }
            else if(cur_event.events == EPOLLIN) {
                // 非监听套接字的读事件
                int n = read(cur_event.data.fd, buf, sizeof(buf));
                int conn_fd = cur_event.data.fd;
                if(n < 0) {
                    perror("read");
                    exit(-1);
                } else if(n == 0) {
                    std::cout << "client closed" << std::endl;
                    close(conn_fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, conn_fd, &cur_event);
                    continue;
                } 
                std::cout << buf << std::endl;
                int m = write(conn_fd, buf, strlen(buf));
                if(m == -1) {
                    perror("write");
                    exit(-1);
                } else if(m < strlen(buf)) {
                    // 修改EPOLLOUT事件
                    cur_event.events = EPOLLOUT;
                    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, conn_fd, &cur_event);
                }
            } else {
                // 非监听套接字的写事件  cur.event.events == EPOLLOUT
                // 之前需要把消息暂时存到应用层的缓冲区 然后再拿出来 但是这里不涉及应用层 所以先不写？？？
                std::cout << "Not available yet!" << std::endl;
                continue;
            }
        }
        
    }
    


    return 0;
}









