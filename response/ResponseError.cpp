#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"
#include <vector>

void	Response::errorPages(int statusCode)
{
	t_responseHeader	responseHeader;
	std::string			filePath = getErrorPagePath(statusCode);

	if (_file.is_open())
		_file.close();
	_file.open(filePath.c_str(), std::ios::binary);
	if (!_file.is_open())
	{
		// std::cout << RED <<  "Failed to open file: " << filePath << RESET << std::endl;
		setResStatus(DONE);
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

	setResStatus(ON_PROCESS);
}




