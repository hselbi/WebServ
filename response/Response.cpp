#include "includes/Response.hpp"



Response::Response(int clientSocket) : clientSocket(clientSocket) {}

ssize_t Response::send(const std::string& response) {
	const char* responseChar;
	
	responseChar = response.c_str();
    return ::send(clientSocket, responseChar, strlen(responseChar), 0);
}

void Response::close() {
    if (clientSocket != -1) {
        ::close(clientSocket);
        clientSocket = -1;
    }
}
std::string Response::ResponseHeaderToString(const t_responseHeader& responseHeader) {
    std::stringstream ss;
    
    ss << "HTTP/1.1 " << responseHeader.statusCode << " " << responseHeader.statusMessage << "\r\n";

    std::map<std::string, std::string>::const_iterator it;
    for (it = responseHeader.headers.begin(); it != responseHeader.headers.end(); ++it) {
        ss << it->first << ": " << it->second << "\r\n";
    }
    ss << "\r\n";

    return ss.str();
}

std::string Response::simpleFileToString(const std::string& path) {
	std::ifstream file;

	file.open(path.c_str(), std::ios::binary);
	if (!file.is_open()) {
		file.open("./pages/404.html");
		if (!file.is_open()) 
			throw std::runtime_error("Failed to open 404.html");
	}

	std::stringstream ss;
	ss << file.rdbuf();
	// file.close();
	return ss.str();
}

void Response::processing() {

	t_responseHeader	responseHeader;
	ssize_t				bytesSent;
	std::string 		responseStr;
	std::string 		responseBody;

	responseBody = simpleFileToString("./pages/um6p_logo.png");
	responseHeader.statusCode = 200;
    responseHeader.statusMessage = "OK";
	responseHeader.headers["Content-Type"] = "image/png";
    responseHeader.headers["Content-Length"] = Utils::toString(responseBody.length());
	responseStr = ResponseHeaderToString(responseHeader);
	responseStr += responseBody;

	bytesSent = this->send(responseStr);
	if (bytesSent == -1) {
		std::cerr << "Failed to send response\n";
	}

}



