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
	// TODO: Verify this function (!! High Priority)
	std::vector<ConfLoca> locations = _client->get_server_block().getLocations();
	std::string requestPath = _client->get_request().getPath();

	for (int i = 0; i < locations.size(); i++)
	{
		if (requestPath.rfind(locations[i].path, 0) == 0)
		{
			std::cout << "Matched location: " << locations[i].path << std::endl;
			return true;
		}
	}
	errorPages(404);
	return false;
}

bool Response::isLocationHaveRedirection()
{
	std::vector<ConfLoca> locations = _client->get_server_block().getLocations();
	std::string path = _client->get_request().getPath();
	t_responseHeader responseHeader;

		
	for (int i = 0; i < locations.size(); i++)
	{
		if (path == locations[i].path)
		{
			responseHeader.statusCode = 301;
			responseHeader.statusMessage = Utils::getStatusMessage(301);
			responseHeader.headers["Server"] = _client->get_server_block().getServerName();
			responseHeader.headers["Content-Type"] = "text/html";
			responseHeader.headers["Content-Length"] = "0";
			// TODO: Verify this path
			responseHeader.headers["Location"] = "/hello.html";
			_header_buffer = Utils::ResponseHeaderToString(responseHeader);
			_client->append_response_data(_header_buffer);
			_header_buffer = "";
			std::cout << "Location: " << locations[i].path << std::endl;
			_client->set_status(DONE);
			return true;
		}
	}
	return false;
}