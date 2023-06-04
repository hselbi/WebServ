#include "includes/Socket.hpp"
#include "includes/Request.hpp"
#include "includes/Response.hpp"

#include <iostream>
#include <cstring>

#define MAX_BUFFER_SIZE 1024
#define PORT 8080

int main() {
   
   	Socket 		serverSocket;
   	Socket 		clientSocket;

	int 		clientSocketFD;
	char 		buffer[MAX_BUFFER_SIZE];
	ssize_t 	bytesRead;
	ssize_t 	bytesSent;
	const char *responseStr;
   
    try
	{
		
		serverSocket.create();
		serverSocket.bind(PORT);
		serverSocket.listen(5);
		std::cout << "Server started. Listening on http://127.0.0.1:" << PORT << std::endl;

		while (true) {
			
			serverSocket.accept(clientSocket);
			clientSocketFD = clientSocket.getSocket();
			
			Request request(clientSocketFD);
			bytesRead = request.receive(buffer, sizeof(buffer) - 1);
			
			if (bytesRead == -1) {
				std::cerr << "Failed to receive request\n";
				clientSocket.close();
				return 1;
			}

			buffer[bytesRead] = '\0';
			std::cout << "Received request:\n" << buffer << std::endl;

			Response response(clientSocketFD);
			responseStr = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
			bytesSent = response.send(responseStr);
			if (bytesSent == -1) {
				std::cerr << "Failed to send response\n";
			}

			response.close();
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	

    return 0;
}