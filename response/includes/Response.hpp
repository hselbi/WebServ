#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <map>
#include <iostream>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include "Utils.hpp"

# define RES_BUFFER_SIZE 1024
typedef struct ResponseHeader {
    int statusCode;
    std::string statusMessage;
    std::map<std::string, std::string> headers;
} t_responseHeader;

class Response {
	public:
		Response(int clientSocket);
		void processing();

	private:
		int clientSocket;
		std::string ResponseHeaderToString(const t_responseHeader& responseHeader);
		std::string getContentType(const std::string& filePath);
		void		readFile(std::string filePath);
		std::map <std::string, std::string> tmpRequest();
		bool checkRequestIsFormed();

};

#endif