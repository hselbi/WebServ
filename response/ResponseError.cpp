#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

void	Response::errorPages(int statusCode)
{
	t_responseHeader	responseHeader;
	std::string			strHeader;
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
	strHeader = Utils::ResponseHeaderToString(responseHeader);

	
	_client->append_response_data(strHeader);
	_client->set_status(ON_PROCESS);
}