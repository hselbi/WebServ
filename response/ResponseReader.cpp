
#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

void Response::readFile()
{
    t_responseHeader responseHeader;
    std::streampos fileSize;
    std::string filePath = getRequestPath();

    _file.open(filePath.c_str(), std::ios::binary);
    _file.seekg(0, std::ios::end);
    fileSize = _file.tellg();
    _file.seekg(0, std::ios::beg);

    responseHeader.statusCode = 200;
    responseHeader.statusMessage = Utils::getStatusMessage(200);
    responseHeader.headers["Content-Type"] = getContentType(filePath);
    responseHeader.headers["Content-Length"] = Utils::toString(fileSize);
    responseHeader.headers["Server"] = _client->get_server_block().getServerName();

    _header_buffer = Utils::ResponseHeaderToString(responseHeader);
    setResStatus(ON_PROCESS);
}

void Response::readFileByPath(std::string filePath)
{
    t_responseHeader responseHeader;
    std::streampos fileSize;

    _file.open(filePath.c_str(), std::ios::binary);
    _file.seekg(0, std::ios::end);
    fileSize = _file.tellg();
    _file.seekg(0, std::ios::beg);

    responseHeader.statusCode = 200;
    responseHeader.statusMessage = Utils::getStatusMessage(200);
    responseHeader.headers["Content-Type"] = getContentType(filePath);
    responseHeader.headers["Content-Length"] = Utils::toString(fileSize);
    responseHeader.headers["Server"] = _client->get_server_block().getServerName();

    _header_buffer = Utils::ResponseHeaderToString(responseHeader);
    setResStatus(ON_PROCESS);
}

void Response::readCgiFile()
{
	// std::cout << "readCgiFile" << std::endl;
	// std::cout << "_cgi_file: " << _cgi_file << std::endl;
    t_responseHeader responseHeader;
    off_t fileSize;
    ssize_t bytesRead;
    char *found;
    char buffer[10000];
    int endHeaderPos = 0;

    fileSize = lseek(_cgi_file, 0, SEEK_END);
	std::cout << "fileSize: " << fileSize << std::endl;
	std::cout << "cgi file size: " << _cgi_file << std::endl;
    if (fileSize == (off_t)-1)
    {
        std::cerr << "Failed to seek to the end of file!" << std::endl;
		std::cout << "errno: " << errno << std::endl;
        close(_cgi_file);
        setResStatus(DONE);
        return errorPages(400);
    }

    if (lseek(_cgi_file, 0, SEEK_SET) == (off_t)-1)
    {
        std::cerr << "Failed to seek to the beginning of file!" << std::endl;
        close(_cgi_file);
        setResStatus(DONE);
        return errorPages(400);
    }

    while ((bytesRead = read(_cgi_file, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytesRead] = '\0';
        found = strstr(buffer, "\r\n\r\n");
        if (found)
        {
            endHeaderPos = found - buffer;
            break;
        }
    }

    lseek(_cgi_file, endHeaderPos, SEEK_SET);
    responseHeader.statusCode = 200;
    responseHeader.statusMessage = Utils::getStatusMessage(200);
    responseHeader.headers["Content-Type"] = "text/html";
    responseHeader.headers["Content-Length"] = Utils::toString(fileSize - endHeaderPos);
    responseHeader.headers["Server"] = _client->get_server_block().getServerName();
    _header_buffer = Utils::ResponseHeaderToString(responseHeader);
	// std::cout << "--> "<< _header_buffer << std::endl;
    setResStatus(ON_PROCESS);

}
