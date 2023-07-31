#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

Response::Response()
{
	_location = NULL;
	_have_cgi = false;
}

Response::~Response()
{
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
	std::string filePath = getRequestPath();

	if (_client->get_res_status() == NOT_STARTED)
	{
		std::cout << "FILE PATH: " << filePath << std::endl;
		std::cout << "}}}}}---> |" << tmp_getRequestPath() << "|" << std::endl;
		if (checkRequestIsFormed() && !isServerHaveRedirection() && getMatchedLocation())
			checkWhichRequestedMethod();
	}
	else if (!_have_cgi && _client->get_res_status() == ON_PROCESS)
	{
		if (_header_buffer.length() > 0)
		{
			if (_header_buffer.length() >= buffer_size)
			{
				std::string str(_header_buffer, buffer_size);
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
			setResStatus(DONE);
		}
	}
	else if (_have_cgi && _client->get_res_status() == ON_PROCESS)
	{
		// std::cout << "processingCgi1" << std::endl;
		processingCgi();
	}
	else if (_client->get_res_status() == DONE)
	{

		_have_cgi = false;
		delete _location;
		_location = NULL;
	}
}

void Response::processingCgi()
{
	// std::cout << "processingCgi" << std::endl;
	ssize_t bytesRead;
	int buffer_size = RES_BUFFER_SIZE;

	if (_header_buffer.length() > 0)
	{
		if (_header_buffer.length() >= buffer_size)
		{
			std::string str(_header_buffer, buffer_size);
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
		setResStatus(DONE);
	}
}


bool Response::get_cgi_status()
{
	return _have_cgi;
}
void Response::set_cgi_status(bool status)
{
	_have_cgi = status;
}

int 				Response::get_cgi_file()
{
	return _cgi_file;
}
void 				Response::set_cgi_file(int fd)
{
	_cgi_file = fd;
}

void Response::setResStatus(int status)
{
	if (status == DONE)
	{
		_client->set_res_status(DONE);
		if (_have_cgi)
		{
			_have_cgi = false;
			_client->get_cgi().reset();
			if (_location)
			{
				delete _location;
				_location = NULL;
			}
			close(_cgi_file);
		}
		if (_file.is_open())
			_file.close();
		_header_buffer = "";
		_buffer[0] = '\0';
		_client->get_request().resetReq();
	}
	else
		_client->set_res_status(status);
}
