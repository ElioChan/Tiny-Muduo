#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <vector>
#include <iostream>

using namespace std;

int main() {
    
    struct sockaddr_in srvaddr;
    const char * addr = "127.0.0.1";
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(9999);
    char  peer_addr_str[INET_ADDRSTRLEN];
    int ret = inet_pton(AF_INET, addr, &srvaddr.sin_addr);
    auto ptr = inet_ntop(AF_INET, &srvaddr.sin_addr, peer_addr_str, sizeof(peer_addr_str));
    if(ptr == NULL) {
        perror("ntop");
    }
    // for(auto c : peer_addr_str) cout << c;
    // cout << endl;
    cout << peer_addr_str << endl;
    return 0;
}