#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

void errif(bool condition, const char * errmsg) {
    if(condition) {
        std::cout << errno << std::endl;
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}

void debug(const char* msg, int num) {
    std::cout << *msg << num << std::endl;
}