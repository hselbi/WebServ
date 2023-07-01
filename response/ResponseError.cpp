#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"
#include <vector>

void	Response::errorPages(int statusCode)
{
	t_responseHeader	responseHeader;
	std::string			filePath = "./defaultPages/" + Utils::toString(statusCode) + ".html";

	_file.open(filePath.c_str(), std::ios::binary);
	if (!_file.is_open())
		std::cout << RED <<  "Failed to open file: " << filePath << RESET << std::endl;

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
	std::string filePath = _client->get_server_block().getRoot()  + _client->get_request().getPath();
	std::cout << "filePath: " << filePath << std::endl;

	if (Utils::isDirectory(filePath) || Utils::fileExists(filePath))
	{
		
		return true;
	}
	else
	{
		errorPages(404);
		return false;
	}
}

bool Response::isLocationHaveRedirection()
{
	ConfServer server = _client->get_server_block();
	std::vector<ConfLoca> locations = server.getLocations();
	std::string path = _client->get_request().getPath();
		
	for (int i = 0; i < locations.size(); i++)
	{
		if (path == locations[i].path)
		{
			std::cout << "location: " << locations[i].path << std::endl;
		}
		
	}
	return true;
}