#include "Server.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "util.h"
#include "Acceptor.h"
#include "Connection.h"
#include <functional>
#include <iostream>  // cout
#include <unistd.h> // read
#include <string.h> // bzero
#include <errno.h>  // errno


#define READ_BUFFER 1024

Server::Server(Eventloop *_loop) : loop(_loop) {
    acceptor = new Acceptor(loop);
    std::function<void(Socket*)> _cb= std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnection(_cb);
    // debug
    std::cout << "server created success!" << std::endl;
}

Server::~Server() {
    delete acceptor;
}

void Server::newConnection(Socket *clientSock) {
    Connection *clietnConnection = new Connection(loop, clientSock);
    std::function<void(int)> _cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
    clietnConnection->setDeleteCallback(_cb);
    connections[clientSock->getFd()] = clietnConnection;
}

void Server::deleteConnection(int fd) {
    delete connections[fd];
    connections.erase(fd);
}
