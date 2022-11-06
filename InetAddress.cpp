#include "InetAddress.h"
#include "util.h"
#include <string.h>

InetAddress::InetAddress() : addr_len(sizeof(addr)) {
    bzero(&addr, sizeof(addr));
}

InetAddress::InetAddress(const char * ip, uint16_t port) : addr_len(sizeof(addr)){
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    errif(inet_pton(AF_INET, ip, &addr.sin_addr) == -1, "pton error");
    addr_len = sizeof(addr);
}

InetAddress::~InetAddress() {

}