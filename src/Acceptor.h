#pragma once
#include <functional>
class Channel;
class Eventloop;
class Socket;

class Acceptor {
private:
    Eventloop *loop;
    Channel *acceptChannel;
    Socket *servSock;
    std::function<void(Socket*)> newConnectionCallback;
public:
    Acceptor(Eventloop*);
    ~Acceptor();

    void acceptNewConnection();
    void setNewConnection(std::function<void(Socket*)>);
};