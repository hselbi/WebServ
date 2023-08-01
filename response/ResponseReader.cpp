
#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

void Response::readFile()
{
    t_responseHeader responseHeader;
    std::streampos fileSize;
    std::string filePath = getRequestPathFile();

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

std::map<std::string, std::string> Response::parseCgiHeader(std::string header)
{
    std::map<std::string, std::string> headers;
    std::string key;
    std::string value;
    size_t pos = 0;

    while ((pos = header.find(":")) != std::string::npos)
    {
        key = header.substr(0, pos);
        header.erase(0, pos + 1);
        pos = header.find("\r\n");
        value = header.substr(0, pos);
        header.erase(0, pos + 2);
        headers[key] = value;
    }
    return headers;
}

void Response::readCgiFile()
{

    t_responseHeader responseHeader;
    std::map<std::string, std::string> cgi_headers;
    std::map<std::string, std::string>::iterator it;
    std::ifstream ifile;
    std::ofstream ofile;
    std::string content;
    std::string header_file = "";

    ifile.open(_cgi_file_path.c_str(), std::ios::binary);
    if (!ifile)
    {
        std::cerr << "Failed to open the file!" << std::endl;
        return errorPages(500);
    }
    else
    {
        std::ostringstream ss;
        ss << ifile.rdbuf();
        content = ss.str();
        ifile.close();
    }

    size_t pos = content.find("\r\n\r\n");
    if (pos != std::string::npos)
    {
        header_file = content.substr(0, pos + 4);
        content = content.erase(0, pos + 4);
    }

    ofile.open(_cgi_file_path.c_str(), std::ios::binary);
    ofile << content;
    ofile.close();

	std::cout << RED << header_file << RESET << std::endl;
    cgi_headers = parseCgiHeader(header_file);
    it = cgi_headers.begin();
	responseHeader.statusCode = -1;
    while (it != cgi_headers.end())
    {
        if (it->first == "Status")
        {
            int statusCode = std::stoi(it->second.substr(0, 4));
            if (statusCode < 100 || statusCode >= 500)
                return (statusCode == 502) ? errorPages(502) : errorPages(500);
            responseHeader.statusCode = statusCode;
            responseHeader.statusMessage = it->second.substr(4, it->second.length() - 4);
        }
        else
            responseHeader.headers[it->first] = it->second;
        it++;
    }

    _file.open(_cgi_file_path.c_str(), std::ios::binary);
	if (responseHeader.statusCode == -1)
	{
		responseHeader.statusCode = 200;
        responseHeader.statusMessage = Utils::getStatusMessage(200);


	}
    responseHeader.headers["Content-Length"] = Utils::toString(content.length());
    responseHeader.headers["Server"] = _client->get_server_block().getServerName();
    _header_buffer = Utils::ResponseHeaderToString(responseHeader);
    setResStatus(ON_PROCESS);
}
