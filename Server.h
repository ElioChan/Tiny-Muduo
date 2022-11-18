#pragma once
#include <unordered_map>

class Eventloop;
class Socket;
class Acceptor;
class Connection;

class Server {
private:
    Eventloop *loop;
    Acceptor *acceptor;
    std::unordered_map<int, Connection*> connections;
public:
    Server(Eventloop*);
    ~Server();

    void newConnection(Socket *_servSock);
    void deleteConnection(int fd);
};