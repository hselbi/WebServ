#pragma once

#include "Utils.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include "Utils.hpp"
#include "../config/ConfLoca.hpp"

# define RES_BUFFER_SIZE 64000

struct Client;

class Response {
	public:
		Response();
		~Response();
		void 								processing();
		void 								setClient(Client &client);	

	private:
		Client 								*_client;
		std::ifstream						_file;
		std::string							_header_buffer;
		char	 							_buffer[RES_BUFFER_SIZE];
		ConfLoca 							*_location;	

		void					readFile();
		void					readFileByPath(std::string filePath);
		bool					checkRequestIsFormed();
		void					autoIndex();
		std::string				getContentType(const std::string& filePath);
		void					errorPages(int statusCode);
		bool					getMatchedLocation();
		bool					isLocationHaveRedirection();

		// Response error handlers
		

};
