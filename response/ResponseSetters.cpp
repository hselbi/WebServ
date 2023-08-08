#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

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
		_client->set_res_status(ON_PROCESS);
}


void Response::setClient(Client &client)
{
	this->_client = &client;
}

t_header Response::setHeaderElement(std::string key, std::string value)
{
    t_header header_element;

    header_element.key = key;
    header_element.value = value;
    return header_element;
}