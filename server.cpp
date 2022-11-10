#include "Socket.h"
#include "util.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Channel.h"
#include <string.h>  // bzero
#include <iostream>  // cout
#include <errno.h>   // errno
#include <unistd.h>  // read


#define MAX_EVENT 100
#define READ_BUFFER 1024

void handleReadEvent(int);

int main() {
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    Epoll *ep = new Epoll();
    Channel *servChannel = new Channel(ep, serv_sock->getFd());
    servChannel->enableReading();
    while(true) {
        std::vector<Channel*> activeChannels = ep->poll();
        int nfds = activeChannels.size();
        for(int i = 0; i < nfds; i++) {
            if(activeChannels[i]->getFd() == serv_sock->getFd()) {
                // 连接事件
                InetAddress *client_addr = new InetAddress();
                bzero(client_addr, sizeof(client_addr));
                int conn_fd = accept4(serv_sock->getFd(), (struct sockaddr*) &client_addr->addr, &client_addr->addr_len, SOCK_CLOEXEC | SOCK_NONBLOCK);
                errif(conn_fd == -1, "accept connection error");
                Channel *clntChannel = new Channel(ep, conn_fd);
                clntChannel->enableReading();
                char addr[INET_ADDRSTRLEN];
                std::cout << "Client fd:" << conn_fd << " IP:" << inet_ntop(AF_INET, &client_addr->addr.sin_addr, addr, client_addr->addr_len) << std::endl;
                std::cout << "Port:" << ntohs(client_addr->addr.sin_port) << std::endl;
                std::cout << "-----------------------------------------------" << std::endl;
            } else if(activeChannels[i]->getEvents() & EPOLLIN) {
                handleReadEvent(activeChannels[i]->getFd());
            } else {
                std::cout << "Something else happened!" << std::endl;
            }
        }
    }
}
 
void handleReadEvent(int fd) {
    // 处理可读事件
    /*
    调用read函数读取客户端发送的信息后处理 这里是简单的回射程序 所以略去处理函数
    直接在这里处理 不然可以根据业务逻辑写一堆处理函数 在这里调用后返回结果
    对于read函数 由于这里使用非阻塞IO 所以需要一直读 直到读完为止
    1. read > 1则继续读
    2. read == -1 说明read异常
        若errno = EINTR说明中断 需要重试 继续读取数据
        若errno = EAGAIN / EWOULDBLOCK 这两个错误对于非阻塞IO来说表示数据已经读完
        若errno = 其他 表示读取出错
    3. read = 0 说明客户端断开连接 read等于0 说明读到了文件的末尾即EOF 而在tcp连接中 当一方调用close时 会发送一个FIN信号....
    */
   char buf[READ_BUFFER];
   while(true) {
    bzero(buf, sizeof(buf));
    ssize_t bytes_read = read(fd, buf, sizeof(buf));
    if(bytes_read > 0) {
        std::cout << "The client " << fd << " sending:" << buf << std::endl;
        write(fd, buf, sizeof(buf));
    } else if(bytes_read == -1 && (errno == EINTR)) {
        std::cout << "Retrying read form client" << std::endl;
        continue;
    } else if(bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        std::cout << "Current data reading finished!" << std::endl;
        break;
    } else if(bytes_read == 0) {
        std::cout << "The client " << fd << " closed connection!" << std::endl;
        close(fd);
        break;
    }
   }
}




