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
#include <vector>
typedef struct ResponseHeader {
    int statusCode;
    std::string statusMessage;
    std::map<std::string, std::string> headers;
} t_responseHeader;

class Response {
	public:
		Response(int clientSocket);
		void processing();
		void	close();

	private:
		int clientSocket;
		ssize_t	send(const std::string& response);
		std::string ResponseHeaderToString(const t_responseHeader& responseHeader);
		std::string simpleFileToString(const std::string& path);

};

#endif