#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "src/util.h"
#include "src/Buffer.h"


#define BUFFER_SIZE 1024

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    const char * addr_buf = "127.0.0.1";
    char buf[BUFFER_SIZE];

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);
    inet_pton(AF_INET, addr_buf, &serv_addr.sin_addr);

    errif(connect(fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)), "connect error");

    Buffer *sendBuffer = new Buffer();
    Buffer *readBuffer = new Buffer();

    while(true) {
        sendBuffer->getline();
        ssize_t write_bytes = write(fd, sendBuffer->c_str(), sendBuffer->size());
        if(write_bytes == -1) {
            std::cout << "socket already disonnected, can't write any more!" << std::endl;
        }
        int already_read = 0;
        char buf[1024];
        while(true) {
            bzero(&buf, sizeof(buf));
            ssize_t read_bytes = read(fd, buf, sizeof(buf));
            if(read_bytes > 0) {
                readBuffer->append(buf, read_bytes);
                already_read += read_bytes;
            } else if(read_bytes == 0) {
                std::cout << "server disconnected!" << std::endl;
                exit(EXIT_SUCCESS);
            }
            if(already_read >= sendBuffer->size()) {
                std::string tmp_str = readBuffer->c_str();
                std::cout << "message from server:" << tmp_str << std::endl;
                break;
            }
        }
        readBuffer->clear();
    }

    return 0;

    // while(fgets(buf, sizeof(buf), stdin) != NULL) {
    //     ssize_t bytes_write = write(fd, buf, sizeof(buf));
    //     if(bytes_write == -1) {
    //         std::cout << "Server disconnected!" << std::endl;
    //         break;
    //     }

    //     bzero(buf, sizeof(buf));
    //     ssize_t bytes_read = read(fd, buf, sizeof(buf));
    //     if(bytes_read == -1) {
    //         close(fd);
    //         errif(true, "socket read error");
    //     } else if(bytes_read == 0) {
    //         std::cout << "The Server closed" << std::endl;
    //         break;
    //     } else if(bytes_read > 0) {
    //         std::cout << "message form server:" << buf << std::endl;
    //     }
    //     bzero(buf, sizeof(buf));
    // }
    // close(fd);
    // return 0;
}


