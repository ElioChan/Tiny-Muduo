#include "Connection.h"
#include "Eventloop.h"
#include "Channel.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Buffer.h"
#include "util.h"
#include <string.h>
// #include <string>
#include <unistd.h>
#include <iostream>

#define BUFFER 1024

Connection::Connection(Eventloop *_loop, Socket *_clientSock) : loop(_loop), clientSock(_clientSock), clientChannel(nullptr), readBuffer(nullptr), writeBuffer(nullptr) { 
    clientChannel = new Channel(loop, clientSock->getFd());
    std::function<void()> cb = std::bind(&Connection::echo, this, clientSock->getFd());
    clientChannel->setcallback(cb);
    clientChannel->enableReading();  
    readBuffer = new Buffer();
    writeBuffer = new Buffer();
}

Connection::~Connection() {
    delete clientChannel;
    delete clientSock;
}

void Connection::echo(int fd) {
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
   char buf[BUFFER];
   while(true) {
    bzero(buf, sizeof(buf));
    ssize_t bytes_read = read(fd, buf, sizeof(buf));
    if(bytes_read > 0) {
        readBuffer->append(buf, bytes_read);
    } else if(bytes_read == -1 && (errno == EINTR)) {
        std::cout << "Continue reading form client" << std::endl;
        continue;
    } else if(bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        std::cout << "Current data reading finished!" << std::endl;
        std::string tmp_str = readBuffer->c_str();
        std::cout << "The client " << fd << " sending:" << tmp_str << std::endl;
        errif(write(fd, readBuffer->c_str(), readBuffer->size()) == -1, "socket write error");
        readBuffer->clear();
        break;
    } else if(bytes_read == 0) {
        std::cout << "The client " << fd << " closed connection!" << std::endl;
        deleteCallback(clientSock->getFd());
        break;
    }
   }    
}

void Connection::setDeleteCallback(std::function<void(int)> _cb) {
    deleteCallback = _cb;
}