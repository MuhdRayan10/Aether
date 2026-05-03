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
#include <memory>
#include <vector>

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

        
    public:
        AetherSocket(); // Constructor to create socket
        virtual ~AetherSocket(); // Destructor to close socket

        // Check if socket is connected
        bool isConnected() const { return currentState == SocketState::CONNECTED; } 

        // IO
        bool sendData(const std::string& textMessage, int flags = 0); // Send text messages to peer
        bool sendData(const std::vector<char>& data, PacketType type, int flags = 0); // Send data to peer (main sendData function)
        bool receiveData(std::string& outData, int bufferSize = 1024, int flags = 0); // Receive data from the server

        void receiverLoop(); // Function that will keep on receiving messages from receiveData()
   
};


// Client class for connecting to a server and communicating with it
class AetherClient : public AetherSocket {
    private:
        
        // Threading
        std::thread receiverThread; // For storing receiver thread which will run continuosly to receive messages from peer

    public: // Constructur, destructor inherited from AetherSocket

        // Connect to a server
        bool connectSocket(const char* ipAddress, int port); 

        // Threading
        void startReceiver(); // For user to launch the receiver thread
        void joinReceiver(); // A way to wait for the thread to finish


};

// Session class for handling a client connection on the server side
class AetherSession : public AetherSocket {
    public:
        // Constructor that takes an already connected socket from accept()
        AetherSession(SocketHandler establishedSocket) {
            internalSocket = establishedSocket;
            currentState = SocketState::CONNECTED; 
        }

        ~AetherSession() {}

    // Both sendData() and recieveData() are inherited from AetherSocket class, we can use them to communicate with client
};

// Server class for accepting incoming connections and communicating with clients
class AetherServer : public AetherSocket {
    private:
        // Server side
        std::vector<std::shared_ptr<AetherSession>> activeConnections; // To store client sessions when acting as a server

        // Threading
        void acceptLoop(); // Function for continuously accepting incoming connections in an loop

    public:
        
        // Start a server to listen for incoming connections (backlog is the max number of pending connections)
        bool startServer(int port, int backlog);
};


#endif