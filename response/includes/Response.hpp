#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <sys/types.h>
#include <sys/socket.h>

#include "Utils.hpp"

# define RES_BUFFER_SIZE 1024

class Response {
	public:
		Response(int clientSocket);
		void processing();

	private:
		int clientSocket;
		std::map <std::string, std::string>	tmpRequest();
		void								readFile(std::string filePath);
		bool								checkRequestIsFormed();
		std::string							getContentType(const std::string& filePath);
		std::string							errorPages(int statusCode, std::string statusMessage);
		void								autoIndex(std::string path);

		// Response error handlers
		

};

#endif