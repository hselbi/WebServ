#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <sys/types.h>
#include <sys/socket.h>

class Request {
public:
    Request(int clientSocket);
    ssize_t receive(char* buffer, size_t bufferSize);

private:
    int clientSocket;
};

#endif
