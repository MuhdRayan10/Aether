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
    
    public:
        MySocket(); // Constructor to create socket
        ~MySocket(); // Destructor to close socket
        bool connectSocket(const char* ipAddress, int port); // Connect to a server
        bool isConnected() const { return currentState == SocketState::CONNECTED; } // Check if socket is connected
};

#endif