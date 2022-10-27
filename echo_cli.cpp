#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <iostream>

//   1. socket 建立套接字
//   2. ipton 建立服务器地址以及端口号
//   3. connect 连接到服务器
//   4. 输出客户端的ip地址以及端口号
//   5. write 写入数据 stdin 
//   6. read 读取服务器回射数据
//   7. 打印服务器回射数据 stdout
//   8. 关闭套接字

int main() {
    int connfd;
    struct sockaddr_in srvaddr;
    struct sockaddr_in cliaddr;
    socklen_t cliaddr_len = sizeof(cliaddr);
    const char * addr = "127.0.0.1";
    char cliaddr_str[INET_ADDRSTRLEN];
    char sendbuf[1024]; 
    char recvbuf[1024];

    // 1.建立套接字
    connfd = socket(AF_INET, SOCK_STREAM, 0);

    // 2.建立服务器地址以及端口号
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(9999);
    inet_pton(AF_INET, addr, &srvaddr.sin_addr);

    // 3.connect 
    connect(connfd, (struct sockaddr *)&srvaddr, sizeof(srvaddr));

    // 4.输出当前客户端的ip与端口号
    getsockname(connfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
    inet_ntop(AF_INET, &cliaddr.sin_addr, cliaddr_str, sizeof(cliaddr_str));
    std::cout << "ip = " <<  cliaddr_str << " port = " << ntohs(cliaddr.sin_port) << std::endl;

    // 5.write 写入数据 
    while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) {
        write(connfd, sendbuf, sizeof(sendbuf));

        // 6.read服务器回射数据
        read(connfd, &recvbuf, sizeof(recvbuf));
        
        // 7.打印回射数据
        fputs(recvbuf, stdout);
        
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
        
    }

    close(connfd);
    return 0;
}
