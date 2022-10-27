#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <vector>
#include <iostream>

// 服务器段：使用poll 监听多个文件描述符 IO复用 业务流程主要就是回射客户端的数据
//   1. socket 创建监听套接字
//   2. setsocket_opt 设置监听地址的重复利用
//   3. bind 监听套接字绑定服务器地址以及端口号(端口号若不指定 则由系统自动分配)
//   4. listen 开始监听
//   5. 建立pollfd的结构体以及vector 用于存放待访问的fd
//   6. poll 开始轮询 利用while循环 然后 Poll返回值n表示事件个数 
//   7. 若n > 0 则表示有套接字事件发生 由于一开始我们只加入了监听套接字 并发在第一个位置 需要先判断监听套接字的返回事件是否为POLLIN
//   若是 则调用accept 接受客户端的连接 同时得到一个连接的文件描述符 将该文件描述符设置为sock_nonblock | sock_cloexe状态方便后序的操作 或者这里可以直接调用accept4函数 可以直接设置返回的套接字状态
//   8. 处理完上述监听套接字后n-- 并输出客户端的ip以及端口号 并判断 n > 0 判断是否继续轮询
//   9. 若n>0 说明还有其他的套接字事件产生了 第一轮由于只有监听套接字 所以是不肯能的 但是之后由于第一轮新加入了套接字 所以是有可能的
//   10. 若其他套接字产生了事件 则调用read函数 并调用业务代码 这边的业务是回射客户端数据
//   11. 这里遍历其他的套接字事件 可以用for循环做 判断条件为pollfd.reevents == pollin 以及 n > 0

#ifndef SOMAXCONN 
#define SOMAXCONN 1024
#endif
typedef std::vector<struct pollfd> PollFdList;



int main() {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    int lis_fd;
    int conn_fd;
    int on = 1;
    int nready = 0;
    char peer_addr_str[INET_ADDRSTRLEN];
    const char * srv_addr_str = "127.0.0.1";
    char buf[1024];
    struct sockaddr_in srv_addr;
    struct sockaddr_in peer_addr;
    socklen_t peerlen = sizeof(peer_addr);

    // 1.创建监听套接字
    lis_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);

    srv_addr.sin_port = htons(9999);
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // 2.设置监听地址可以重复利用？？？
    setsockopt(lis_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    // 3.bind绑定服务器地址
    bind(lis_fd, (struct sockaddr *) &srv_addr, sizeof(srv_addr));

    // 4.listen 开始监听
    listen(lis_fd, SOMAXCONN);

    // 5.建立poll 列表
    struct pollfd pfd;
    pfd.fd = lis_fd;
    pfd.events = POLLIN;
    
    PollFdList pollfds;
    pollfds.push_back(pfd);

    while(1) {
        // 首先调用poll获取待处理事件数
        // 处理监听事件 若有返回事件 则输出客户端地址以及将连接描述符加入维护列表
        // for循环遍历处理连接描述符的事件 若有返回事件 则读取数据 然后执行业务逻辑 即回射客户端数据
        // 若读取数据为0 则断开连接 同时从维护列表中删去连接描述符 这里删除调用vector的erase函数 
        // erase若是删除某个元素 则返回值为指向删除元素的下一个元素的迭代器 由于在for循环中还要++ 所以这个返回值要先进行--
        nready = poll(&*pollfds.begin(), pollfds.size(), -1);

        if(nready == 0) continue;
        
        // 处理监听事件
        if(pollfds[0].revents & POLLIN) {
            conn_fd = accept4(lis_fd, (struct sockaddr *) &peer_addr, &peerlen, SOCK_CLOEXEC | SOCK_NONBLOCK);

            // 将连接描述符加入维护列表
            pfd.fd = conn_fd;
            pfd.events = POLLIN;
            pollfds.push_back(pfd);

            inet_ntop(AF_INET, &peer_addr.sin_addr, peer_addr_str, sizeof(peer_addr_str));
            std::cout << "ip = " << peer_addr_str << " port  = " << ntohs(peer_addr.sin_port) << std::endl;
            
            nready--;
        }

        for(auto it = pollfds.begin() + 1; it < pollfds.end() && nready > 0; it++) {
            if(it->revents & POLLIN) {
                // 当前连接描述符有事件发生 读取数据 并调用业务逻辑
                nready--;

                int n = read(it->fd, buf, sizeof(buf));
                if(n == 0) {
                    std::cout << "client closed" << std::endl;
                    close(it->fd);
                    it = pollfds.erase(it);
                    it--;
                } else {
                    std::cout << buf << std::endl;
                    write(it->fd, buf, sizeof(buf));
                }
                memset(buf, 0, sizeof(buf));

            }
        }
    }
    return 0;
}






