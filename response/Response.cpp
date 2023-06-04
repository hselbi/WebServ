#include "includes/Response.hpp"

#include <cstring>
#include <unistd.h>

Response::Response(int clientSocket) : clientSocket(clientSocket) {}

ssize_t Response::send(const char* response) {
    return ::send(clientSocket, response, strlen(response), 0);
}

void Response::close() {
    if (clientSocket != -1) {
        ::close(clientSocket);
        clientSocket = -1;
    }
}


