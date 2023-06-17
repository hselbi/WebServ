#include "includes/Response.hpp"

Response::Response(int clientSocket) : clientSocket(clientSocket) {}


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

	file.open(filePath.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		errorPages(404, "Not Founded");
		close(clientSocket);
		return ;
	}

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);


	responseHeader.statusCode = 200;
	responseHeader.statusMessage = "OK";
	responseHeader.headers["Content-Type"] = getContentType(filePath);
	responseHeader.headers["Content-Length"] = Utils::toString(fileSize);
	responseHeader.headers["Server"] = "WebServ";

	strHeader = Utils::ResponseHeaderToString(responseHeader);
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

std::map <std::string, std::string> Response::tmpRequest()
{
	std::map <std::string, std::string> request;

	std::ifstream reqFile("./request_file_test/test_request.txt");
	std::string line;

	while (std::getline(reqFile, line))
	{
		std::string key = line.substr(0, line.find(":"));
		std::string value = line.substr(line.find(":") + 1);
		request[key] = value;
	}

	return request;
}

bool Response::checkRequestIsFormed()
{
	std::map <std::string, std::string> request = tmpRequest();

	if (request.find("Transfer-Encoding") != request.end() && request["Transfer-Encoding"] != "chunked")
	{
		errorPages(501, "Not Implemented");
		return false;
	}
	else if (request.find("Transfer-Encoding") == request.end() && request.find("Content-Length") == request.end() 
		&& reques.find("method") != request.end() && request["method"] == "POST")
	{
		errorPages(400, "Bad Request");
		return false;
	}
	else if (!Utils::isValidURI(request["path"]))
	{
		errorPages(400, "Bad Request");
		return false;
	}
	else if (strlen(request["path"]) > 2048)
	{
		errorPages(414, "Request-URI Too Long");
		return false;
	}
	return true;	
}

void Response::processing()
{
	std::map <std::string, std::string> request;
	std::string filePath = "./www" + tmpRequest()["path"];
	std::cout << filePath << std::endl;
	if (checkRequestIsFormed())
		readFile(filePath);
}

