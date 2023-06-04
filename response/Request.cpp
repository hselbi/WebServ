#include "includes/Request.hpp"

#include <iostream>
#include <cstring>

Request::Request(int clientSocket) : clientSocket(clientSocket) {}

ssize_t Request::receive(char* buffer, size_t bufferSize) {
    return recv(clientSocket, buffer, bufferSize, 0);
}
