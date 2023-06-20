#include "../includes/response/Response.hpp"

std::string	Response::errorPages(int statusCode)
{
	t_responseHeader	responseHeader;
	std::string			strHeader;
	std::string			filePath = "./defaultPages/" + Utils::toString(statusCode) + ".html";

	_file.open(filePath.c_str(), std::ios::binary);
	if (!_file.is_open())
		throw std::runtime_error("Failed to open file: " + filePath);

	_file.seekg(0, std::ios::end);
	std::streampos fileSize = _file.tellg();
	_file.seekg(0, std::ios::beg);

	responseHeader.statusCode = statusCode;
	responseHeader.statusMessage = Utils::getStatusMessage(statusCode);
	responseHeader.headers["Content-Type"] = getContentType(filePath);
	responseHeader.headers["Content-Length"] = Utils::toString(fileSize);
	strHeader = Utils::ResponseHeaderToString(responseHeader);

	sendResponse(strHeader, strHeader.length());
	// send(clientSocket, strHeader.c_str(), strHeader.size(), 0);

	char buffer[RES_BUFFER_SIZE];
	while (!_file.eof())
	{
		_file.read(buffer, sizeof(buffer));
		// send(clientSocket, buffer, _file.gcount(), 0);
		sendResponse(buffer, _file.gcount());
	}
	_file.close();
	// close(clientSocket);
	return "";
}