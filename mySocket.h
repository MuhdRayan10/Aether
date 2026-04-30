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
#include "AetherPacket.h"

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
class AetherSocket {
    protected:
        SocketHandler internalSocket;

        static bool initialized; // Tracking if Winsock is initiliazed
        static int activeSockets; // Tracking active sockets to know when to cleanup Winsock
        SocketState currentState = SocketState::DISCONNECTED; // Current state of the socket
        std::string peerIP; // Store peer IP for reference

        bool ensureConnected() const; // Safety check method
        
        // Server side
        std::vector<SocketHandler> clientSockets; // To store client sockets when acting as a server

        // Threading
        void receiverLoop(SocketHandler clientSocket); // Function that will keep on receiving messages from receiveData()
        std::thread receiverThread; // For storing receiver thread which will run continuosly

    protected:
        


    public:
        AetherSocket(); // Constructor to create socket
        ~AetherSocket(); // Destructor to close socket
        bool connectSocket(const char* ipAddress, int port); // Connect to a server
        bool isConnected() const { return currentState == SocketState::CONNECTED; } // Check if socket is connected
        bool startServer(int port); // Start a server to listen for incoming connections

        // IO
        bool sendData(const std::string& textMessage, int flags = 0); // Send text messages to peer
        bool sendData(const std::vector<char>& data, PacketType type, int flags = 0); // Send data to peer (main sendData function)
        bool receiveData(std::string& outData, SocketHandler clientSocket,int bufferSize = 1024, int flags = 0); // Receive data from the server

        // Threading
        void startReceiver(); // For user to launch the receiver thread
        void joinReceiver(); // A way to wait for the thread to finish
};


class AetherClient : public AetherSocket {
    
};

class AetherServer : public AetherSocket {

}

#endif