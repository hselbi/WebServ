#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"
#include <vector>

void	Response::errorPages(int statusCode)
{
	t_responseHeader	responseHeader;
	std::string			filePath = getErrorPagePath(statusCode);
	

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
	_header_buffer = Utils::ResponseHeaderToString(responseHeader);
	_client->set_status(ON_PROCESS);
}

bool	Response::getMatchedLocation()
{
	// TODO: Verify this function (!! High Priority)
	std::vector<ConfLoca> locations = _client->get_server_block().getLocations();
	std::string requestPath = _client->get_request().getPath();
	size_t max_length = 0;
	int		index = -1;
	for (int i = 0; i < locations.size(); i++)
	{	
		if (requestPath.find(locations[i].path) == 0 && locations[i].path.length() > 0)
		{
			max_length = locations[i].path.length();
			index = i;
		}
	}

	if (index != -1)
	{
		_location = new ConfLoca(locations[index]);

		if (isMethodAllowedInLocation())
		{
			std::cout << "Allowed method in location" << std::endl;
			return true;
		}
		else
			return false;
		
	}
	std::cout << "No matched location" << std::endl;
	errorPages(404);
	return false;
}

bool Response::isServerHaveRedirection()
{
	t_responseHeader	responseHeader;
	int 				redirectStatus = _client->get_server_block().getRedirectStatus();
	if (redirectStatus == 301 || redirectStatus == 302 || redirectStatus == 307)
	{
		responseHeader.statusCode = redirectStatus;
		responseHeader.statusMessage = Utils::getStatusMessage(redirectStatus);
		responseHeader.headers["Location"] = _client->get_server_block().getRedirectUrl();
		responseHeader.headers["Server"] = _client->get_server_block().getServerName();

		_header_buffer = Utils::ResponseHeaderToString(responseHeader);
		_client->append_response_data(_header_buffer);
		_client->set_status(DONE);
		return true;
	}
	else if (redirectStatus != -1)
	{
		responseHeader.statusCode = redirectStatus;
		responseHeader.statusMessage = Utils::getStatusMessage(redirectStatus);
		responseHeader.headers["Content-Type"] = "application/octet-stream";
		responseHeader.headers["Content-Length"] = "0";
		responseHeader.headers["Server"] = _client->get_server_block().getServerName();

		_header_buffer = Utils::ResponseHeaderToString(responseHeader);
		_client->append_response_data(_header_buffer);
		_client->set_status(DONE);
		return true;
	}
		
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
