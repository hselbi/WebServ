
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

    t_responseHeader responseHeader;

    std::cout << "CGI FILE PATH: " << _cgi_file_path << std::endl;
    std::fstream file(_cgi_file_path, std::ios::in | std::ios::out | std::ios::binary);

    if (!file) {
        std::cerr << "Failed to open the file!" << std::endl;
        return errorPages(500);
    }

    std::ostringstream oss;
    oss << file.rdbuf();
    std::string content = oss.str();

    size_t pos = content.find("\r\n\r\n");
    if (pos != std::string::npos)
        content.erase(0, pos + 4);

    file.clear();
    file.seekp(0, std::ios::beg);
    file << content;
    file.close();

    _file.open(_cgi_file_path.c_str(), std::ios::binary);
    responseHeader.statusMessage = Utils::getStatusMessage(200);
    responseHeader.headers["Content-Type"] = "text/html";
    responseHeader.headers["Content-Length"] = Utils::toString(content.length());
    responseHeader.headers["Server"] = _client->get_server_block().getServerName();
    _header_buffer = Utils::ResponseHeaderToString(responseHeader);
    setResStatus(ON_PROCESS);
}
