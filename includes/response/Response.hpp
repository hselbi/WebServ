#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include "Utils.hpp"
#include "../request/Request.hpp"

# define RES_BUFFER_SIZE 1024

class Response {
	public:
		Response(Request &request);
		void processing();

	private:
		Request &request;
		std::map <std::string, std::string>	tmpRequest();
		void								readFile(std::string filePath);
		bool								checkRequestIsFormed();
		std::string							getContentType(const std::string& filePath);
		std::string							errorPages(int statusCode, std::string statusMessage);

		// Response error handlers
		

};

#endif