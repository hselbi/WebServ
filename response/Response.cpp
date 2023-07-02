#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

#define RES_COLOR "\036[36m"

Response::Response() {}
Response::~Response() {}

std::string Response::getContentType(const std::string &filePath)
{
	std::map<std::string, std::string>::const_iterator it;
	std::map<std::string, std::string> contentTypes;
	std::string extension;
	size_t dotPos;

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

void Response::autoIndex()
{
	DIR *dir;
	struct dirent *ent;
	std::string strHeader;
	t_responseHeader responseHeader;
	std::string path = _client->get_server_block().getRoot() + _client->get_request().getPath();
	std::string body = "<html><head><title>Index of " + _client->get_request().getPath() + "</title></head><body><h1>Index of " + _client->get_request().getPath() + "</h1><hr><pre>";
	std::string tmp;

	std::string defaultPages[] = {"index.html", "index.htm", "index.php"};
	for (size_t i = 0; i < 3; i++)
	{
		char lastChar = path[path.length() - 1];
		std::string tmpPath = (lastChar == '/') ? path + defaultPages[i] : path + "/" + defaultPages[i];
		if (Utils::fileExists(tmpPath))
		{
			readFileByPath(tmpPath);
			return;
		}
	}
	if ((dir = opendir(path.c_str())) != NULL)
	{
		body.append("<a href=\"./\">./</a><br>");
		body.append("<a href=\"../\">../</a><br>");

		while ((ent = readdir(dir)) != NULL)
		{
			tmp = ent->d_name;
			if (Utils::isDirectory(path + "/" + tmp) && tmp != "." && tmp != "..")
				body.append("<a href=\"" + tmp + "/\">" + tmp + "/</a><br>");
			else if (tmp != "." && tmp != "..")
				body.append("<a href=\"" + tmp + "\">" + tmp + "</a><br>");

		}
		body.append("</pre><hr></body></html>");
		closedir(dir);
	}
	else
	{
		std::cout << RED << "autoIndex error" << RESET << std::endl;
		errorPages(404);
		return;
	}
	responseHeader.statusCode = 200;
	responseHeader.statusMessage = "OK";
	responseHeader.headers["Content-Type"] = "text/html";
	responseHeader.headers["Content-Length"] = Utils::toString(body.length());
	responseHeader.headers["Server"] = _client->get_server_block().getServerName();

	strHeader = Utils::ResponseHeaderToString(responseHeader);
	_client->append_response_data(strHeader);
	_client->append_response_data(body);
	_client->set_status(DONE);
}

bool Response::checkRequestIsFormed()
{
	std::map<std::string, std::string> req = _client->get_request().getHeaders();

	if (!req["Transfer-Encoding"].empty() && req["Transfer-Encoding"] != "chunked")
	{
		errorPages(501);
		return false;
	}
	else if (req["Transfer-Encoding"].empty() && req["Content-Length"].empty() && _client->get_request().getMethod() == "POST")
	{
		errorPages(400);
		return false;
	}
	else if (!Utils::isValidURI(_client->get_request().getPath()))
	{
		errorPages(400);
		return false;
	}
	else if (_client->get_request().getPath().length() > 2048)
	{
		errorPages(414);
		return false;
	}
	return true;
}

void Response::readFile()
{
	t_responseHeader responseHeader;
	std::string filePath = _client->get_server_block().getRoot()  + _client->get_request().getPath();
	std::streampos fileSize;

	if (Utils::isDirectory(filePath))
	{
		autoIndex();
		return;
	}
	_file.open(filePath.c_str(), std::ios::binary);
	if (!_file.is_open())
	{
		errorPages(404);
		return;
	}

	_file.seekg(0, std::ios::end);
	fileSize = _file.tellg();
	_file.seekg(0, std::ios::beg);

	responseHeader.statusCode = 200;
	responseHeader.statusMessage = Utils::getStatusMessage(200);
	responseHeader.headers["Content-Type"] = getContentType(filePath);
	responseHeader.headers["Content-Length"] = Utils::toString(fileSize);
	responseHeader.headers["Server"] = _client->get_server_block().getServerName();
	_header_buffer = "";
	_header_buffer = Utils::ResponseHeaderToString(responseHeader);
	_client->set_status(ON_PROCESS);
}

void Response::readFileByPath(std::string filePath)
{
	t_responseHeader responseHeader;
	std::streampos fileSize;

	if (Utils::isDirectory(filePath))
	{
		autoIndex();
		return;
	}
	_file.open(filePath.c_str(), std::ios::binary);
	if (!_file.is_open())
	{
		errorPages(404);
		return;
	}

	_file.seekg(0, std::ios::end);
	fileSize = _file.tellg();
	_file.seekg(0, std::ios::beg);

	responseHeader.statusCode = 200;
	responseHeader.statusMessage = Utils::getStatusMessage(200);
	responseHeader.headers["Content-Type"] = getContentType(filePath);
	responseHeader.headers["Content-Length"] = Utils::toString(fileSize);
	responseHeader.headers["Server"] = _client->get_server_block().getServerName();

	_header_buffer = "";
	_header_buffer = Utils::ResponseHeaderToString(responseHeader);
	_client->set_status(ON_PROCESS);
}

void Response::processing()
{
	int buffer_size = RES_BUFFER_SIZE;
	
	if (_client->get_status() == NOT_STARTED)
	{
		if (checkRequestIsFormed() && getMatchedLocation())
			readFile();
	}
	if (_client->get_status() == ON_PROCESS) // change if to else if
	{	
		if (_header_buffer.length() > 0)
		{
			if (_header_buffer.length() >= buffer_size)
			{	std::string str(_header_buffer, buffer_size);
				_header_buffer = _header_buffer.substr(buffer_size);
				_client->append_response_data(str);
				return;
			}
			else
				buffer_size -= _header_buffer.length();
		}
		if (!_file.eof())
		{
			_file.read(_buffer, buffer_size);
			std::string str(_buffer, _file.gcount());
			str = (_header_buffer.length() > 0) ? _header_buffer + str : str;
			_client->append_response_data(str);
			_header_buffer = "";
			processing(); // TODO: Remove recursion after work chunked response
		}
		else
		{
			_file.close();
			_buffer[0] = '\0';
			_client->set_status(DONE);
			std::cout << YELLOW << "DONE" << RESET << std::endl;
		}
	}
}

void Response::setClient(Client &client)
{
	this->_client = &client;
}

