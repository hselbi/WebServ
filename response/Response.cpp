#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

Response::Response() {
	_location = NULL;
	_have_cgi = false;
}
 
Response::~Response() {
	std::cout << "Response destructor called" << std::endl;
	if (_location)
		delete _location;
	_file.close();
}

void Response::setClient(Client &client)
{
	this->_client = &client;
}

void Response::processing()
{
	int buffer_size = RES_BUFFER_SIZE;
	if (_client->get_res_status() == NOT_STARTED)
	{
		if (checkRequestIsFormed() && !isServerHaveRedirection() && getMatchedLocation())
			checkWhichRequestedMethod();
	}
	else if (!_have_cgi && _client->get_res_status() == ON_PROCESS)
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
		if (_file.is_open() && !_file.eof())
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
			_client->set_res_status(DONE);
		}
	}
	else if (_have_cgi && _client->get_status() == ON_PROCESS) 
		processingCgi();
	else if (_client->get_status() == DONE)
	{
		_have_cgi = false;
		delete _location;
		_location = NULL;
	}
}

void Response::processingCgi()
{
	ssize_t			bytesRead;
	int				buffer_size = RES_BUFFER_SIZE;

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
	if ((bytesRead = read(_cgi_file, _buffer, buffer_size)) > 0)
	{
		std::string str(_buffer, bytesRead);
		str = (_header_buffer.length() > 0) ? _header_buffer + str : str;
		_client->append_response_data(str);
		_header_buffer = "";
	}
	else 
	{
		close(_cgi_file);
		_have_cgi = false;
		_buffer[0] = '\0';
		_client->set_status(DONE);
	}
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
