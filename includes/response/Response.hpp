#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Utils.hpp"
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
		void								readFile(std::string filePath);
		bool								checkRequestIsFormed();
		void								autoIndex(std::string path);
		void								sendResponse(std::string response, size_t size);
		std::string							getContentType(const std::string& filePath);
		std::string							errorPages(int statusCode);

		// Response error handlers
		

};

#endif