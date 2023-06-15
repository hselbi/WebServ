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
		std::string getContentType(const std::string& filePath);
		void		readFile(std::string filePath);
		std::map <std::string, std::string> tmpRequest();
		bool checkRequestIsFormed();

		// Response error handlers
		

};

#endif