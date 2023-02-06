#pragma once
#include <functional>

class Channel;
class Eventloop;
class Socket;
class Buffer;

class Connection {
private:
    Eventloop *loop;
    Channel *clientChannel;
    Socket *clientSock;
    std::function<void(int)> deleteCallback;
    Buffer *readBuffer;
    Buffer *writeBuffer;
public:
    Connection(Eventloop*, Socket*);
    ~Connection();

    void echo(int);
    void setDeleteCallback(std::function<void(int)>);
};