#include "includes/Socket.hpp"

Socket::Socket() : serverSocket(-1) {}

Socket::~Socket() {
    close();
}

void Socket::create() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
        throw std::runtime_error("Failed to create socket");
	
}

void Socket::bind(int port) {
	serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	if (::bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) 
        throw std::runtime_error("Failed to bind socket");
}

void Socket::listen(int backlog) {
	if (::listen(serverSocket, backlog) == -1)
		throw std::runtime_error("Failed to listen on socket");
}

void Socket::accept(Socket& clientSocket) {
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocketFD = ::accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
    if (clientSocketFD == -1)
		throw std::runtime_error("Failed to accept connection");
    clientSocket.serverSocket = clientSocketFD;
}

void Socket::close() {
    if (serverSocket != -1) {
        ::close(serverSocket);
        serverSocket = -1;
    }
}

int Socket::getSocket() const {
    return serverSocket;
}