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

#ifdef _WIN32

    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult != 0) {
        std::cerr << "Oops, WSAStartup failed! Error: " << iResult << std::endl;
    }
#endif

// TODO: Socket creation 

class MySocket {
}


// TODO: Closing sockets