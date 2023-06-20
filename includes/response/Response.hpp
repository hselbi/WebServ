#pragma once

#include "Utils.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include "Utils.hpp"

# define RES_BUFFER_SIZE 1024

struct Client;

class Response {
	public:
		Response();
		~Response();
		void 								processing();
		void 								setClient(Client &client);	

	private:
		std::string 						_buffer;
		Client 								*_client;
		std::ifstream						_file;
		void								readFile(std::string filePath);
		bool								checkRequestIsFormed();
		void								autoIndex(std::string path);
		void								sendResponse(std::string response, size_t size);
		std::string							getContentType(const std::string& filePath);
		std::string							errorPages(int statusCode);
		

		// Response error handlers
		

};
