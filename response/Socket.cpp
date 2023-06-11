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
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

	if (::bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) 
        throw std::runtime_error("Failed to bind socket");
}

void Socket::listen(int backlog) {
	if (::listen(serverSocket, backlog) == -1)
		throw std::runtime_error("Failed to listen on socket");
}

int Socket::accept() {
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocketFD = ::accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
    if (clientSocketFD == -1)
		throw std::runtime_error("Failed to accept connection");
	return clientSocketFD;
}

void Socket::close() {
    if (serverSocket != -1) {
        ::close(serverSocket);
        serverSocket = -1;
    }
}

int Socket::getServerSocket() const {
    return serverSocket;
}

int Socket::getClientSocket() const {
	return clientSocket;
}

void Socket::closeServerSocket() {
	if (serverSocket != -1) {
		::close(serverSocket);
		serverSocket = -1;
	}
}

void Socket::closeClientSocket() {
	if (clientSocket != -1) {
		::close(clientSocket);
		clientSocket = -1;
	}
}
