#include "mySocket.h"
#include <iostream>

int main() {

    MySocket socket;
    socket.connectSocket("127.0.0.1", 8080); // Replace with actual IP and port
    return 0;
}