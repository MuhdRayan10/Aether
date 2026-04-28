#include "mySocket.h"
#include <iostream>

// Function to communicate with the peer in a loop, sending and receiving messages continuously
void runChatLoop(MySocket& socket) {
    std::string message;
    
    std::cout << "AETHER SESSION (Type 'exit' to quit)" << std::endl;

    // Start listening for messages from peer side
    socket.startReceiver();

    // As long as socket connection is alive, accept input from user
    while (socket.isConnected()) {
        std::cout << "> " << std::flush;

        // If input completes / errors out
        if (!std::getline(std::cin, message) || message == "exit") break;

        // If message fails to send
        if (!socket.sendData(message + "\n")) break;
    }

    // Now, input is completed, wait for responses to finish
    socket.joinReceiver();
}

int main() {

    MySocket socket;
    socket.connectSocket("127.0.0.1", 8080); // Replace with actual IP and port
    
    runChatLoop(socket);

    return 0;
}