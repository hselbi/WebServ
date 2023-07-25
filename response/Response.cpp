#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

Response::Response() {
	_location = NULL;
}
 
Response::~Response() {
	if (_location)
		delete _location;
	_file.close();
}

void Response::setClient(Client &client)
{
	this->_client = &client;
}

void Response::autoIndex()
{
	DIR					*dir;
	struct dirent		*ent;
	std::string			strHeader, path, body, tmp;
	t_responseHeader	responseHeader;
	
	path = getRoot() + _client->get_request().getPath();
	body = "<html><head><title>Index of " + _client->get_request().getPath() 
		+ "</title></head><body><h1>Index of " + _client->get_request().getPath() + "</h1><hr><pre>";
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
	responseHeader.statusCode = 200;
	responseHeader.statusMessage = Utils::getStatusMessage(200);
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
	else if (req["Transfer-Encoding"].empty() && req["Content-Length"].empty()
		&& _client->get_request().getMethod() == "POST")
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
	std::string filePath = getRoot()  + _client->get_request().getPath();
	std::streampos fileSize;

	_file.open(filePath.c_str(), std::ios::binary);
	_file.seekg(0, std::ios::end);
	fileSize = _file.tellg();
	_file.seekg(0, std::ios::beg);

	responseHeader.statusCode = 200;
	responseHeader.statusMessage = Utils::getStatusMessage(200);
	responseHeader.headers["Content-Type"] = getContentType(filePath);
	responseHeader.headers["Content-Length"] = Utils::toString(fileSize);
	responseHeader.headers["Server"] = _client->get_server_block().getServerName();

	_header_buffer = Utils::ResponseHeaderToString(responseHeader);
	_client->set_status(ON_PROCESS);
}

void Response::readFileByPath(std::string filePath)
{
	t_responseHeader responseHeader;
	std::streampos fileSize;

	_file.open(filePath.c_str(), std::ios::binary);
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
	_client->get_cgi().start_cgi();
	if (_client->get_status() == NOT_STARTED)
	{
		if (checkRequestIsFormed() && getMatchedLocation())
			checkWhichRequestedMethod();
	}
	else if (_client->get_status() == ON_PROCESS) // change if to else if
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
		}
		else
		{
			_file.close();
			_buffer[0] = '\0';
			_client->set_status(DONE);
		}
	}
}

void Response::checkWhichRequestedMethod()
{
	std::string method = _client->get_request().getMethod();
	if (method == "GET")
		Method_GET();
}

void Response::setRediration(std::string location)
{
	t_responseHeader responseHeader;

	responseHeader.statusCode = 301;
	responseHeader.statusMessage = Utils::getStatusMessage(301);
	responseHeader.headers["Content-Length"] = "0";
	responseHeader.headers["Location"] = location;
	responseHeader.headers["Server"] = _client->get_server_block().getServerName();

	_header_buffer = "";
	_header_buffer = Utils::ResponseHeaderToString(responseHeader);

	_client->set_status(ON_PROCESS);
}
