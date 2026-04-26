# Aether

A lightweight, high-performance and cross-platform communication protocol written using C++.

Aether is designed to provide a stable protocol between different operating systems (Windows, macOS, and Linux) by providing a unified wrapper around low-level socket APIs.

Built as a study in systems programming, networking architecture, and encryption/decryption.


## Features

- Cross-Platform: Elegantly handles the differences between Winsock2 (for Windows) and POSIX sockets (for Unix-like systems) using C++ preprocessor macros.


- Zero-Configuration Discovery: Using mDNS, Aether nodes automatically announce their presence and can discover peers on a local network. (The aim is to bring the "AirDrop" experience to the command line and cross-platform environments.) No IP addresses required for setup.
[TO IMPLEMENT]

- Persistent Bidirectional Pipeline: Aether creats a long-living TCP connection rather than transient requests. Once a handshake is established, data can be continuously pushed through in both directions.
[TO IMPLEMENT]

- Magic Clipboard Sync: Integrated OS level hooks that monitor the system clipboard, if enabled. When text is copied in one device, Aether securely transmits the payload to all connecter peers, updating their clipboards in real time.
[TO IMPLEMENT]

- File Transfer Engine: To handle large binary payloads (like images, PDFs, etc.), Aether implements a `custom chunking algorithm`. Files are broken into segments, transmitted with integrity checks, and reconstructed back at the destination.
[TO IMPLEMENT]

- Secure Handshake (mTLS): Implementation of mutual TLS in order to ensure that data is not only delivered reliably but also is encrypted end-to-end, protecting local network traffic from packet sniffing.
[TO IMPLEMENT]

## Prerequisites

To build and run Aether, you need the following:

- Compiler: C++ compiler preferably supporting at least C++17 (eg. `g++`, `clang++`)
- Build System: `make` (Unix) or `mingw32-make` (Windows).
- Network: Both machines (or instances) must be on the same local network, although future plans do wish to allow communication between distinct networks.

## Installation 

1. Clone the repository using `git clone <link-to-this-repo>`
2. Compile using the Makefile (`make` or `mingw32-make`)
3. Clean the build files (`make clean`)

### Contribute

Any and all contributions are welcome! Please DM on discord (`@muhdrayan`) if you're interested in working together on this project.

