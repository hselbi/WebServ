#include "includes/Socket.hpp"
#include "includes/Request.hpp"
#include "includes/Response.hpp"

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <exception>

#define MAX_BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./server <port>" << std::endl;
		return 1;
	}

	try
	{
		Socket socket;
		socket.create();
		socket.bind(std::atoi(argv[1]));
		socket.listen(5);
		std::cout << "Server is listening on port " << argv[1] << std::endl;
		while (true)
		{
			int clientSocket = socket.accept();

			Request request(clientSocket);
			char requestBuffer[MAX_BUFFER_SIZE];
			request.receive(requestBuffer, sizeof(requestBuffer));
			Response response(clientSocket);
			response.processing();
		}
		socket.closeServerSocket();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}