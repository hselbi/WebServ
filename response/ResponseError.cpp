#include "../includes/response/Response.hpp"

std::string	Response::errorPages(int statusCode, std::string statusMessage)
{
	t_responseHeader	responseHeader;
	std::string			strHeader;
	std::string			filePath = "./config/" + Utils::toString(statusCode) + ".html";
	std::ifstream		file;

	file.open(filePath.c_str(), std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("Failed to open file: 501.html");

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	responseHeader.statusCode = statusCode;
	responseHeader.statusMessage = statusMessage;
	responseHeader.headers["Content-Type"] = getContentType(filePath);
	responseHeader.headers["Content-Length"] = Utils::toString(fileSize);
	strHeader = Utils::ResponseHeaderToString(responseHeader);

	sendResponse(strHeader, strHeader.length());
	// send(clientSocket, strHeader.c_str(), strHeader.size(), 0);

	char buffer[RES_BUFFER_SIZE];
	while (!file.eof())
	{
		file.read(buffer, sizeof(buffer));
		// send(clientSocket, buffer, file.gcount(), 0);
		sendResponse(buffer, file.gcount());
	}
	file.close();
	// close(clientSocket);
	return "";
}