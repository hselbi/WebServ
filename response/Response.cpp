#include "../includes/response/Response.hpp"


Response::Response(Request &request) : request(request) {}


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
		errorPages(404);
		return ;
	}
	std::cout << body << std::endl;

	responseHeader.statusCode = 200;
	responseHeader.statusMessage = "OK";
	responseHeader.headers["Content-Type"] = "text/html";
	responseHeader.headers["Content-Length"] = Utils::toString(body.length());
	responseHeader.headers["Server"] = "WebServ";

	strHeader = Utils::ResponseHeaderToString(responseHeader);
	sendResponse(strHeader, strHeader.length());
	sendResponse(body, body.length());

	// send(clientSocket, strHeader.c_str(), strHeader.length(), 0);
	// send(clientSocket, body.c_str(), body.length(), 0);
	// close(clientSocket);
}

void Response::readFile(std::string filePath)
{
	t_responseHeader	responseHeader;
	std::string			strHeader;
	std::ifstream		file;

	if (Utils::isDirectory(filePath))
	{
		// autoIndex(filePath);
		return ;
	}
	file.open(filePath.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		errorPages(404);
		return ;
	}

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);


	responseHeader.statusCode = 200;
	responseHeader.statusMessage = Utils::getStatusMessage(200);
	responseHeader.headers["Content-Type"] = getContentType(filePath);
	responseHeader.headers["Content-Length"] = Utils::toString(fileSize);
	responseHeader.headers["Server"] = "WebServ";

	strHeader = Utils::ResponseHeaderToString(responseHeader);
	sendResponse(strHeader, strHeader.length());

	// send(clientSocket, strHeader.c_str(), strHeader.length(), 0);
	char buffer[RES_BUFFER_SIZE];
	while (!file.eof())
	{
		file.read(buffer, sizeof(buffer));
		sendResponse(buffer, file.gcount());
	}

	// file.close();
	// close(clientSocket);

}

bool Response::checkRequestIsFormed()
{
	std::map <std::string, std::string> req = request.getHeaders();
	if (!req["Transfer-Encoding"].empty() && req["Transfer-Encoding"] != "chunked")
	{
		errorPages(501);
		return false;
	}
	else if (req["Transfer-Encoding"].empty() && req["Content-Length"].empty() && request.getMethod() == "POST")
	{
		errorPages(400);
		return false;
	}
	else if (!Utils::isValidURI(request.getPath()))
	{
		errorPages(400);
		return false;
	}
	else if (request.getPath().length() > 2048)
	{
		errorPages(414);
		return false;
	}
	return true;	
}

void Response::processing()
{
	std::cout << "Response processing" << std::endl;
	std::cout << request << std::endl;
	std::string root_path = "./www";
	if (checkRequestIsFormed())
	{
		readFile(root_path + request.getPath());
	}
}

void Response::sendResponse(std::string response, size_t size)
{
	// send(clientSocket, response.c_str(), size, 0);
	std::cout << response << std::endl;
}