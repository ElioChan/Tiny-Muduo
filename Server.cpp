#include "Server.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "util.h"
#include <functional>
#include <iostream>  // cout
#include <unistd.h> // read
#include <string.h> // bzero
#include <errno.h>  // errno


#define READ_BUFFER 1024

Server::Server(Eventloop *_loop) : loop(_loop) {
    Socket *servSock = new Socket();
    InetAddress *servAddr = new InetAddress("127.0.0.1", 8888);
    servSock->bind(servAddr);
    servSock->listen();
    Channel *servChannel = new Channel(loop, servSock->getFd());
    std::function<void()> cb = std::bind(&Server::newConnection, this, servSock);
    servChannel->setcallback(cb);
    servChannel->enableReading();
    // debug
    std::cout << "server created success!" << std::endl;
}

Server::~Server() {

}

void Server::newConnection(Socket *servSock) {
    InetAddress *clientAddr = new InetAddress();
    bzero(clientAddr, sizeof(clientAddr));
    int conn_fd = accept4(servSock->getFd(), (struct sockaddr *)clientAddr, &clientAddr->addr_len, SOCK_CLOEXEC | SOCK_NONBLOCK);
    errif(conn_fd == -1, "connect client error");
    char addr[INET_ADDRSTRLEN];
    std::cout << "Client fd:" << conn_fd << " IP:" << inet_ntop(AF_INET, &clientAddr->addr.sin_addr, addr, clientAddr->addr_len) << std::endl;
    std::cout << "Port:" << ntohs(clientAddr->addr.sin_port) << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
    Channel *clientChannel = new Channel(loop, conn_fd);
    std::function<void()> cb = std::bind(&Server::handleReadEvent, this, conn_fd);
    clientChannel->setcallback(cb);
    clientChannel->enableReading();
}

void Server::handleReadEvent(int fd) {
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