#include "mySocket.h"
#include <iostream>

int main() {

    MySocket socket;
    socket.connectSocket("127.0.0.1", 8080); // Replace with actual IP and port
    socket.startReceiver();

    std::string inputMessage;
    std::cin >> inputMessage;
    while (input != "exit") {
        if (!socket.sendData(inputMessage)) break;
        std::cin >> inputMessage;
    }

    socket.joinReceiver(); 
    return 0;
}