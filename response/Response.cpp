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
	// std::cout << "Response destructor called" << std::endl;
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
		if (_client->get_request().getCodeRet() == 400)
			errorPages(400);
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

bool Response::get_cgi_status()
{
	return _have_cgi;
}
void Response::set_cgi_status(bool status)
{
	_have_cgi = status;
}

void Response::setResStatus(int status)
{
	if (status == DONE)
	{

		if (_have_cgi)
		{
			_client->get_cgi().reset();
			remove(_cgi_file_path.c_str());
		}
		if (_location)
		{
			delete _location;
			_location = NULL;
		}
		if (_file.is_open())
			_file.close();
		_cgi_file_path = "";
		_have_cgi = false;
		_header_buffer = "";
		_buffer[0] = '\0';
		_client->get_request().resetReq();
		_client->set_res_status(DONE);
	}
	else if (status == ON_PROCESS)
	{

		_client->set_res_status(ON_PROCESS);
	}
}
