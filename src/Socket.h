#pragma once

class InetAddress;

class Socket {
private:
    int fd;

public:
    Socket();
    Socket(int);
    ~Socket();

    void bind(InetAddress*);
    void listen();
    void setblocking();
    void setnonblocking();

    int accept(InetAddress*);
    int getFd();
};