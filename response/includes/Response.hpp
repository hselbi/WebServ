#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <sys/types.h>
#include <sys/socket.h>

class Response {
	public:
		Response(int clientSocket);
		ssize_t send(const char* response);
		void close();

	private:
		int clientSocket;
};

#endif