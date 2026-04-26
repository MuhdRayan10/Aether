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

int main() {
    
    #ifdef _WIN32

        // Initialize Winsock
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

        if (iResult != 0) {
            std::cerr << "Oops, WSAStartup failed! Error: " << iResult << std::endl;
            return 1;
        }
    #endif

    // TODO: Socket creation 

    class MySocket {
        private:
            SocketHandler internalSocket;

        public:
            MySocket() {
                // Create socket code
                internalSocket = socket(AF_INET, SOCK_STREAM, 0); // We are using IPv4, TCP

                // Error check
                #ifdef _WIN32
                    if (internalSocket == INVALID_SOCKET) {
                        std:cerr << "Windows socket creation failed!" << std::endl;
                    }
                #else
                    if (internalSocket < 0) {
                        std:cerr << "Unix socket creation failed!" << std::endl;
                    }
                #endif

            }

            ~MySocket() {
                // Destroy socket code
                #ifdef _WIN32
                    closesocket(internalSocket);
                #else
                    close(internalSocket);
                #endif
            }

    }

    // TODO: Closing sockets
}