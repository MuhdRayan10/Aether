#include "mySocket.h"
#include <iostream>

bool MySocket::initialized = false;
int MySocket::activeSockets = 0;

// Create socket code
MySocket::MySocket() {

    /* If Winsock is not initialized, we initialize it. 
    We only want to initialize it once, so we check the static variable 'initialized'. 
    We also track active sockets to know when to cleanup Winsock. */
    if (!initialized) {    
        #ifdef _WIN32

            // Initialize Winsock
            WSADATA wsaData;
            int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

            if (iResult != 0) {
                std::cerr << "Oops, WSAStartup failed! Error: " << iResult << std::endl;
                return 1;
            }
            else initialized = true;

        #endif
    }
    
    internalSocket = socket(AF_INET, SOCK_STREAM, 0); // We are using IPv4, TCP

    // Error check
    #ifdef _WIN32
        if (internalSocket == INVALID_SOCKET) {
            std::cerr << "Windows socket creation failed!" << std::endl;
        }
        else activeSockets++; // Increment active sockets count
    #else
        if (internalSocket < 0) {
            std::cerr << "Unix socket creation failed!" << std::endl;
        }
        else activeSockets++; // Increment active sockets count
    #endif
}

// Destroy socket code
MySocket::~MySocket() {
    #ifdef _WIN32
        closesocket(internalSocket);   
    #else
        close(internalSocket);
    #endif
    activeSockets--; // Decrement active sockets count

    if (activeSockets == 0 && initialized) { // If no more active sockets and Winsock was initialized, we cleanup
        #ifdef _WIN32
            WSACleanup();
            initialized = false; // Reset initialized flag
        #endif
    }
    currentState = SocketState::DISCONNECTED; // Update state to disconnected
}

// Connect to server code
bool MySocket::connectSocket(const char* ipAddress, int port) {
    
    if (isConnected()) {
        std::cerr << "Socket is already connected!" << std::endl;
        return true; // We can return true since it's already connected, but we warn the user about it
    }
    
    if (currentState == SocketState::CONNECTING) {
        std::cerr << "Socket is already in the process of connecting!" << std::endl;
        return false; // We return false since we can't start a new connection while one is in progress
    }

    currentState = SocketState::CONNECTING; // Update state to connecting

    struct sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress)); // fill with zeroes to remove garbage data

    serverAddress.sin_family = AF_INET; // We are using IPv4

    // Convert port to network byte order (from little-endian to big-endian)
    serverAddress.sin_port = htons(port); 

    // Convert IP address from string to binary form
    int result = inet_pton(AF_INET, ipAddress, &serverAddress.sin_addr);

    if (result != 1) {
        std::cerr << "Invalid IP address!" << std::endl;
        return false;
    }

    // Connect to the server
    // Note: We have to cast sockaddr_in to sockaddr pointer
    result = connect(internalSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)); 
    if (result < 0) {
        std::cerr << "Connection failed :( Error: " << result << std::endl;
        return false;
    }

    // If we reach here, connection was successful, hooray!
    std::cout << "Successful connection to " << ipAddress << ":" << port << std::endl;
    currentState = SocketState::CONNECTED; // Update state to connected
    peerIP = ipAddress; // Store the connected IP address
    return true;

}

bool MySocket::sendData(const std::string& data, int flags) {
    // Check if connection is established or not
    if (!isConnected()) {
        std::cerr << "Connect socket to server first!" << std::endl;
        return false;
    }

    // Sending data to the server
    int result = send(internalSocket, data.c_str(), data.length(), flags);

    if (result < 0) {
        // Error, no message sent
        std::cerr << "Error - No message could be sent :(" << std::endl;
        currentState = SocketState::ERROR_STATE; // Update state to error
        return false;
    }

    // Succesfully sent the message, woohoo!
    std::cout << result << " bytes of data were sent to " << peerIP << "!" << std::endl;
    return true;


}