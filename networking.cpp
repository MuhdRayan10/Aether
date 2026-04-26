#include "MySocket.h"
#include <iostream>

int main() {

    MySocket socket;

    #ifdef _WIN32
        WSACleanup(); // Finish using Winsock
    #endif
    
    return 0;
}