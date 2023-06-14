#include "includes/Socket.hpp"
#include "includes/Request.hpp"
#include "includes/Response.hpp"

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <exception>

#define MAX_BUFFER_SIZE 1024
#define PORT 8080
int main()
{
	try
	{
		Socket socket;
		socket.create();
		socket.bind(PORT);
		socket.listen(5);
		std::cout << "Server is listening on port " << PORT << std::endl;
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