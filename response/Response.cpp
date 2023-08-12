#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

Response::Response()
{
	_location = NULL;
	_have_cgi = false;
	_header_buffer = "";
	_buffer[0] = '\0';
	_cgi_file_path = "";
}

Response::~Response()
{
	if (_location)
		delete _location;
	_file.close();
}

Response::Response(const Response &copy)
{
	*this = copy;
}

Response &Response::operator=(const Response &copy)
{
	if (this != &copy)
	{
		_client = copy._client;
		_location = copy._location;
		_header_buffer = copy._header_buffer;
		_cgi_file_path = copy._cgi_file_path;
		_have_cgi = copy._have_cgi;
	}
	return *this;
}

void Response::processing()
{
	int buffer_size = RES_BUFFER_SIZE;
	
	if (_client->get_res_status() == NOT_STARTED)
	{
		if (_client->get_request().getCodeRet() != 200)
			errorPages(_client->get_request().getCodeRet());
		else if (checkRequestIsFormed() && !isServerHaveRedirection() && getMatchedLocation())
			checkWhichRequestedMethod();
	}
	else if (_client->get_res_status() == ON_PROCESS)
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
			setResStatus(DONE);
	}
}
