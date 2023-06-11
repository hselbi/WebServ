#include "includes/Response.hpp"

Response::Response(int clientSocket) : clientSocket(clientSocket) {}

std::string Response::ResponseHeaderToString(const t_responseHeader &responseHeader)
{
	std::stringstream ss;

	ss << "HTTP/1.1 " << responseHeader.statusCode << " " << responseHeader.statusMessage << "\r\n";

	std::map<std::string, std::string>::const_iterator it;
	for (it = responseHeader.headers.begin(); it != responseHeader.headers.end(); ++it)
	{
		ss << it->first << ": " << it->second << "\r\n";
	}
	ss << "\r\n";

	return ss.str();
}

std::string Response::getContentType(const std::string &filePath)
{
	std::map<std::string, std::string>::const_iterator	it;
	std::map<std::string, std::string>					contentTypes;
	std::string 										extension;
	size_t												dotPos;

	contentTypes[".html"] = "text/html";
	contentTypes[".htm"] = "text/html";
	contentTypes[".jpg"] = "image/jpeg";
	contentTypes[".jpeg"] = "image/jpeg";
	contentTypes[".png"] = "image/png";
	contentTypes[".gif"] = "image/gif";
	contentTypes[".pdf"] = "application/pdf";
	contentTypes[".mp4"] = "video/mp4";
	
	dotPos = filePath.rfind('.');
	if (dotPos != std::string::npos)
		extension = filePath.substr(dotPos);
	
	it = contentTypes.find(extension);
	if (it != contentTypes.end())
		return it->second;

	return "text/plain";
}

void Response::readFile(std::string filePath)
{
	t_responseHeader	responseHeader;
	std::string			strHeader;
	std::ifstream		file;
	bool				isFileOpen = true;

	file.open(filePath.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		isFileOpen = false;
		filePath = "./www/404.html";
		file.open("./www/404.html", std::ios::binary);
		if (!file.is_open())
			throw std::runtime_error("Failed to open file: 404.html");
	}

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	if (!isFileOpen)
	{
		responseHeader.statusCode = 404;
		responseHeader.statusMessage = "Not Found";
	}
	else
	{
		responseHeader.statusCode = 200;
		responseHeader.statusMessage = "OK";
	}

	responseHeader.headers["Content-Type"] = getContentType(filePath);
	responseHeader.headers["Content-Length"] = Utils::toString(fileSize);
	

	strHeader = ResponseHeaderToString(responseHeader);
	send(clientSocket, strHeader.c_str(), strHeader.length(), 0);

	char buffer[RES_BUFFER_SIZE];
	while (!file.eof())
	{
		file.read(buffer, sizeof(buffer));
		send(clientSocket, buffer, file.gcount(), 0);
	}

	file.close();
	close(clientSocket);
}



void Response::processing()
{

	std::string filePath = "./www/test.pdf";
	readFile(filePath);
	
}
