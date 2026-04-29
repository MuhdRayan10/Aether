#include "mySocket.h"
#include <iostream>
#include <vector>

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

// Ensure connection method
bool MySocket::ensureConnected() const {
    if (currentState != SocketState::CONNECTED) {
        std::cerr << "Connect socket to server first!" << std::endl;
        return false;
    }

    return true;
}

bool MySocket::startServer(int port) {
    sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress)); // fill with zeroes to remove garbage data

    serverAddress.sin_family = AF_INET; // We are using IPv4
    serverAddress.sin_port = htons(port); // Convert port to network byte order (from little-endian to big-endian)
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Listen on all IP addresses of the machine

    // Bind the socket to the specified port
    int result = bind(internalSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    if (result < 0) {
        std::cerr << "Binding failed :( Error: " << result << std::endl;
        return false;
    }

    // Listen for incoming connections
    int maxPendingConnections = 10; // Maximum number of pending connections in the queue
    result = listen(internalSocket, maxPendingConnections); 

    if (result < 0) {
        std::cerr << "Listening failed :( Error: " << result << std::endl;
        return false;
    }
    while (true) {
        int clientSocket = accept(internalSocket, nullptr, nullptr); // Accept an incoming connection
        if (clientSocket < 0) {
        std::cerr << "Accepting connection failed :( Error: " << clientSocket << std::endl;
        return false;
    }

    // Successful connection with client, we can now use clientSocket to send and receive messages with the client
    clientSockets.push_back(clientSocket); 
    std::cout << "Client connected successfully!" << std::endl;
    }

    
    return true;


}

void MySocket::acceptLoop() {
    while (true) {
        SocketHandler clientSocket = accept(internalSocket, nullptr, nullptr); // Accept an incoming connection
        if (clientSocket < 0) {
            std::cerr << "Accepting connection failed :( Error: " << clientSocket << std::endl;
            continue; // We can continue accepting other connections even if one fails
        }
        // Successful connection with client, we can now use clientSocket to send and receive messages with the client
        clientSockets.push_back(clientSocket);
        std::cout << "Client connected successfully!" << std::endl;

        std::thread worker([this, clientSocket]() {
            // Here we can handle communication with the client using clientSocket
            this->receiverLoop(clientSocket); // We can reuse the receiverLoop function
        });
        worker.detach(); // Detach the thread to allow it to run independently
    }
}

/*

Send data methods

We have overloaded sendData with two functions:
1. sendData(textMessage)
2. sendData(data, type)

(1) is merely for convencience
(2) is the main sendData function that handles all types of data payloads

*/

bool MySocket::sendData(const std::string& textMessage, int flags) {
    // Convert string to a vector of chars
    std::vector<char> payload(textMessage.begin(), textMessage.end());

    // Pass it to the real sendData function
    return sendData(payload, PacketType::TEXT_MESSAGE, flags);
}


// NOTE: We are using vector<char> for data since it knows its own size (as opposed to a pointer like const char*)
bool MySocket::sendData(const std::vector<char>& data, PacketType type, int flags) {
    // Check if connection is established or not
    if (!ensureConnected()) return false;

    // Building packet header
    AetherHeader header;
    header.type = type;

    // Convert size to to big endian
    header.payloadSize = htonl(static_cast<uint32_t>(data.size()));

    // Convert header to raw bytes
    const char* rawBytes = reinterpret_cast<const char*>(&header);

    // Send header first
    int headerResult = send(internalSocket, rawBytes, sizeof(AetherHeader), flags);
    
    if (headerResult < 0) { // Handling error
        std::cerr << "Error - No message could be sent :(" << std::endl;
        currentState = SocketState::ERROR_STATE; // Update state to error
        return false;
    }

    // Now sending actual data to the server
    if (!data.empty()) {
        int result = send(internalSocket, data.data(), data.size(), flags);
        
        if (result < 0) {
            // Error, no message sent
            std::cerr << "Error - No message could be sent :(" << std::endl;
            currentState = SocketState::ERROR_STATE; // Update state to error

            return false;
        }
        
        //Total no. of bytes sent
        headerResult += result;
        

    }

    // Succesfully sent the message, woohoo!
    std::cout << headerResult << " bytes of data were sent to " << peerIP << "!" << std::endl;
    return true;


}

// Receive data method

bool MySocket::receiveData(std::string& outData, int bufferSize, int flags) {
    // Check if connection is established or not
    if (!ensureConnected()) return false;

    // Creating a buffer to store the received data
    std::vector<char> buffer(bufferSize);

    // result stores no. of bytes received
    int result = recv(internalSocket, buffer.data(), bufferSize, flags);

    std::cout << buffer.data() << std::endl;

    if (result < 0) {
        // Error, no message received
        std::cerr << "Error, no message could be received" << std::endl;
        currentState = SocketState::ERROR_STATE;
        return false;
    }
    else if (result == 0) {
        // Peer closed the connection
        std::cout << "Peer closed the connection" << std::endl;
        currentState = SocketState::DISCONNECTED;
        return false;
    }

    // Message received successfully!
    outData = std::string(buffer.data(), result);
    return true;
}

// THREADING SECTION

// Receiver loop

void MySocket::receiverLoop(SocketHandler clientSocket) {
    // Keep receiving messages while still connected
    
    while (isConnected()) {
        std::string msg;
        if (receiveData(msg)) {
            std::cout << "[PEER] " << msg << std::endl;
        }
    }
}

// Starting receiver thread

void MySocket::startReceiver() {
    // Creating thread, passing 'this' to let it know which instance to run receiverLoop on
    receiverThread = std::thread(&MySocket::receiverLoop, this);
}

// Joining receiver thread

void MySocket::joinReceiver() {
    if (receiverThread.joinable()) {
        receiverThread.join();
    }
}
