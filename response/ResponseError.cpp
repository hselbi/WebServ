#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"
#include <vector>

void	Response::errorPages(int statusCode)
{
	t_responseHeader	responseHeader;
	std::string			filePath = getErrorPagePath(statusCode);
	open_file:
	_file.open(filePath.c_str(), std::ios::binary);
	if (!_file.is_open())
	{
		std::cout << RED <<  "Failed to open file: " << filePath << RESET << std::endl;
		_client->set_status(DONE);
		return;
	}

	_file.seekg(0, std::ios::end);
	std::streampos fileSize = _file.tellg();
	_file.seekg(0, std::ios::beg);

	responseHeader.statusCode = statusCode;
	responseHeader.statusMessage = Utils::getStatusMessage(statusCode);
	responseHeader.headers["Content-Type"] = getContentType(filePath);
	responseHeader.headers["Content-Length"] = Utils::toString(fileSize);
	_header_buffer = "";
	_header_buffer = Utils::ResponseHeaderToString(responseHeader);
	_client->set_status(ON_PROCESS);
}

bool	Response::getMatchedLocation()
{
	// TODO: Verify this function (!! High Priority)
	std::vector<ConfLoca> locations = _client->get_server_block().getLocations();
	std::string requestPath = _client->get_request().getPath();
	for (int i = 0; i < locations.size(); i++)
	{	
		if (requestPath.rfind(locations[i].path, 0) == 0)
		{
			_location = new ConfLoca(locations[i]);
			if (isLocationHaveRedirection())
				return true;
			else
			{
				if (isMethodAllowedInLocation())
				{
					std::cout << "Allowed method in location" << std::endl;
					return true;
				}
				else
					return false;
			}	
			
			return true;
		}
	}
	std::cout << "No matched location" << std::endl;
	errorPages(404);
	return false;
}

bool Response::isLocationHaveRedirection()
{
	t_responseHeader responseHeader;

		
	// TODO: check if redirection is working

	// if (_location->redirection != "")
	// {
	// 	responseHeader.statusCode = 301;
	// 	responseHeader.statusMessage = Utils::getStatusMessage(301);
	// 	responseHeader.headers["Location"] = _location->redirection;
	// 	_header_buffer = "";
	// 	_header_buffer = Utils::ResponseHeaderToString(responseHeader);
	// 	_client->set_status(DONE);
	// 	return true;
	// }
		
	return false;
}


bool Response::isMethodAllowedInLocation()
{
	std::vector<MethodType> allow_methods;

	if (_location)
	{
		allow_methods = _location->allow_methods;
		if (std::find(allow_methods.begin(), allow_methods.end(), _location->strtoMethod(_client->get_request().getMethod())) != allow_methods.end())
			return true;
		else
		{
			std::cout << "Method not allowed in location" << std::endl;
			errorPages(405);
			return false;
		}
	}
	return false;
}
