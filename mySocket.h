#ifndef MYSOCKET_H
#define MYSOCKET_H

#ifdef _WIN32
    // For windows
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")

#else
    // For macOS and Linux
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>

#endif

// General imports
#include <string>
#include <thread>

// socket can be of type SOCKET for Windows and int for unix systems
#ifdef _WIN32
    using SocketHandler = SOCKET;
#else
    using SocketHandler = int;
#endif

// Socket states
enum class SocketState {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    ERROR_STATE
};

// Socket wrapper class to handle different socket types for Windows and Unix systems
class MySocket {
    private:
        SocketHandler internalSocket;

        static bool initialized; // Tracking if Winsock is initiliazed
        static int activeSockets; // Tracking active sockets to know when to cleanup Winsock
        SocketState currentState = SocketState::DISCONNECTED; // Current state of the socket
        std::string peerIP; // Store peer IP for reference

        bool ensureConnected() const; // Safety check method
        
        // Threading
        void receiverLoop(); // Function that will keep on receiving messages from receiveData()
        std::thread receiverThread; // For storing receiver thread which will run continuosly


    public:
        MySocket(); // Constructor to create socket
        ~MySocket(); // Destructor to close socket
        bool connectSocket(const char* ipAddress, int port); // Connect to a server
        bool isConnected() const { return currentState == SocketState::CONNECTED; } // Check if socket is connected

        // IO
        bool sendData(const std::string& data, int flags = 0); // Send data to the server
        bool receiveData(std::string& outData, int bufferSize = 1024, int flags = 0); // Receive data from the server

        // Threading
        void startReceiver(); // For user to launch the receiver thread
        void joinReceiver(); // A way to wait for the thread to finish
};

#endif