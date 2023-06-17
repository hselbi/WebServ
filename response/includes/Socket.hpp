#ifndef SOCKET_HPP
# define SOCKET_HPP
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <exception>

class Socket
{
	private:
		int serverSocket;
		int clientSocket;
		struct sockaddr_in serverAddress;
		struct sockaddr_in clientAddress;
	public:
		Socket();
		~Socket();
		void create();
		void bind(int port);
		void listen(int backlog);
		int accept();
		void close();
		int getServerSocket() const;
		int getClientSocket() const;
		void closeServerSocket();
		void closeClientSocket();
};


#endif