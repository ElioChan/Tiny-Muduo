#pragma once

class Eventloop;
class Socket;

class Server {
private:
    Eventloop *loop;
public:
    Server(Eventloop*);
    ~Server();

    void newConnection(Socket *_servSock);
    void handleReadEvent(int);
};