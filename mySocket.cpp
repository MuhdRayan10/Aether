#include "MySocket.h"
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
}