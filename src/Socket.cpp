#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

Socket::Socket() : fd(-1) {
    fd = socket(AF_INET, SOCK_CLOEXEC | SOCK_STREAM | SOCK_NONBLOCK, 0);
    errif(fd == -1, "socket create error");
}

Socket::Socket(int _fd) : fd(_fd) {
    errif(_fd == -1, "socket create error");
}

Socket::~Socket() {
    if(fd != -1) {
        close(fd);
        fd = -1;
    }
}

void Socket::bind(InetAddress* addr) {
    errif(::bind(fd, (sockaddr*)&addr->addr, addr->addr_len) == -1, "socket bind error");
}

void Socket::listen() {
    errif(::listen(fd, SOMAXCONN) == -1, "listen error");
}

void Socket::setblocking() {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
}

void Socket::setnonblocking() {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress* addr) {
    int conn_fd = ::accept4(fd, (struct sockaddr *)& addr->addr, &addr->addr_len, SOCK_CLOEXEC | SOCK_NONBLOCK);
    errif(conn_fd == -1, "accept error");
    return conn_fd;
}

int Socket::getFd() {
    return fd;
}

