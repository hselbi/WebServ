#include "../includes/response/Response.hpp"


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


void Response::autoIndex(std::string dirPath)
{
	DIR 				*dir;
	struct dirent 		*ent;
	std::string 		strHeader;
	t_responseHeader 	responseHeader;
	std::string path = "./www" + dirPath;
	std::string body = "<html><head><title>Index of " + dirPath + "</title></head><body><h1>Index of " + dirPath + "</h1><hr><pre>";
	std::string tmp;

	std::string defaultPages[] = {"index.html", "index.htm", "index.php"};
	for (size_t i = 0; i < 3; i++)
	{
		std::string tmpPath = (path.back() == '/') ? path + defaultPages[i] : path + "/" + defaultPages[i];
		std::cout << tmpPath << std::endl;
		if (Utils::fileExists(tmpPath))
		{
			readFile("/" + defaultPages[i]);
			return ;
		}
	}
	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			tmp = ent->d_name;
			if (tmp != "." && tmp != "..")
				body += "<a href=\"" + tmp + "\">" + tmp + "</a><br>";
		}
		body += "</pre><hr></body></html>";
		closedir(dir);
	}
	else
	{
		std::cout << "autoIndex error" << std::endl;
		errorPages(404, "Not Found");
		return ;
	}
	std::cout << body << std::endl;

	responseHeader.statusCode = 200;
	responseHeader.statusMessage = "OK";
	responseHeader.headers["Content-Type"] = "text/html";
	responseHeader.headers["Content-Length"] = Utils::toString(body.length());
	responseHeader.headers["Server"] = "WebServ";

	strHeader = Utils::ResponseHeaderToString(responseHeader);
	send(clientSocket, strHeader.c_str(), strHeader.length(), 0);
	send(clientSocket, body.c_str(), body.length(), 0);
	close(clientSocket);
}

void Response::readFile(std::string filePath)
{
	t_responseHeader	responseHeader;
	std::string			strHeader;
	std::ifstream		file;

	if (Utils::isDirectory(filePath))
	{
		autoIndex(filePath);
		return ;
	}
	filePath = "./www" + filePath;
	file.open(filePath.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "readFile error : " << filePath << std::endl;
		errorPages(404, "Not Found");
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
		&& request.find("method") != request.end() && request["method"] == "POST")
	{
		errorPages(400, "Bad Request");
		return false;
	}
	else if (!Utils::isValidURI(request["path"]))
	{
		errorPages(400, "Bad Request");
		return false;
	}
	else if (request["path"].length() > 2048)
	{
		errorPages(414, "Request-URI Too Long");
		return false;
	}
	return true;	
}

void Response::processing()
{
	std::map <std::string, std::string> request;
	std::string filePath = tmpRequest()["path"];
	if (checkRequestIsFormed())
		readFile(filePath);
}

