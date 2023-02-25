#include "Acceptor.h"
#include "Channel.h"
#include "Eventloop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <thread>
#include <string.h>
#include <iostream>


Acceptor::Acceptor(Eventloop *_loop) : loop(_loop), acceptChannel(nullptr), servSock(nullptr) {
    servSock = new Socket();
    InetAddress *servAddr = new InetAddress("127.0.0.1", 8888);
    servSock->bind(servAddr);
    servSock->listen();
    acceptChannel = new Channel(loop, servSock->getFd());
    std::function<void()> cb = std::bind(&Acceptor::acceptNewConnection, this);
    acceptChannel->setReadCallback(cb);
    acceptChannel->enableReading();
    acceptChannel->setUseThreadPool(false);
    delete servAddr;
}

Acceptor::~Acceptor() {
    delete acceptChannel;
    delete servSock;
}

void Acceptor::acceptNewConnection(){
    std::cout << "I am in Acceptor.cpp acceptNewConnectino" << std::endl;
    InetAddress *clientAddr = new InetAddress();
    bzero(clientAddr, sizeof(clientAddr));
    int connFd = accept4(servSock->getFd(), (struct sockaddr *)clientAddr, &clientAddr->addr_len, SOCK_CLOEXEC | SOCK_NONBLOCK);
    // int connFd = accept4(servSock->getFd(), (struct sockaddr *)clientAddr, &clientAddr->addr_len, SOCK_CLOEXEC);
    errif(connFd == -1, "connect client error");
    Socket *clientSock = new Socket(connFd);
    char addr[INET_ADDRSTRLEN];
    std::cout << "Maximum concurrency:" << std::thread::hardware_concurrency() << std::endl;
    std::cout << "Client fd:" << clientSock->getFd() << " IP:" << inet_ntop(AF_INET, &clientAddr->addr.sin_addr, addr, clientAddr->addr_len) << std::endl;
    // std::cout << "Thread id:" << std::this_thread::get_id() << std::endl;
    std::cout << "Port:" << ntohs(clientAddr->addr.sin_port) << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;    
    newConnectionCallback(clientSock);
    delete clientAddr;
}

void Acceptor::setNewConnection(std::function<void(Socket*)> _cb) {
    newConnectionCallback = _cb;
}


