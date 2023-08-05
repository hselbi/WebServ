
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
    responseHeader.m_headers["Content-Type"] = getContentType(filePath);
    responseHeader.m_headers["Content-Length"] = Utils::toString(fileSize);
    responseHeader.m_headers["Server"] = _client->get_server_block().getServerName();

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
    responseHeader.m_headers["Content-Type"] = getContentType(filePath);
    responseHeader.m_headers["Content-Length"] = Utils::toString(fileSize);
    responseHeader.m_headers["Server"] = _client->get_server_block().getServerName();

    _header_buffer = Utils::ResponseHeaderToString(responseHeader);
    setResStatus(ON_PROCESS);
}

bool Response::parseCgiHeader(std::string header, int contentLength, int delimiterLength)
{
    t_responseHeader responseHeader;
    std::vector<Header> headers;
    std::vector<Header>::iterator it;
    std::string key;
    std::string value;
    t_header header_element;
    size_t pos = 0;

    while ((pos = header.find(":")) != std::string::npos)
    {
        key = header.substr(0, pos);
        header.erase(0, pos + 1);
        pos = header.find(delimiterLength == 4 ? "\r\n" : "\n");
        value = header.substr(0, pos);
        header.erase(0, pos + 2);
        headers.push_back(setHeaderElement(key, value));
    }

    it = headers.begin();
    responseHeader.statusCode = -1;
    while (it != headers.end())
    {
        if (it->key == "Status")
        {
            int statusCode = std::stoi(it->value.substr(0, 4));
            if (statusCode < 100 || statusCode >= 500)
            {
                (statusCode == 502) ? errorPages(502) : errorPages(500);
                return false;
            }
            responseHeader.statusCode = statusCode;
            responseHeader.statusMessage = it->value.substr(4, it->value.length() - 4);
        }
        else
            responseHeader.v_headers.push_back(setHeaderElement(it->key, it->value));
        it++;
    }

    _file.open(_cgi_file_path.c_str(), std::ios::binary);
    if (responseHeader.statusCode == -1)
    {
        responseHeader.statusCode = 200;
        responseHeader.statusMessage = Utils::getStatusMessage(200);
    }
    
    responseHeader.v_headers.push_back(setHeaderElement("Content-Length", Utils::toString(contentLength)));
    responseHeader.v_headers.push_back(setHeaderElement("Server", _client->get_server_block().getServerName()));
    _header_buffer = Utils::ResponseHeaderToString(responseHeader);

    return true;
}

void Response::readCgiFile()
{

    std::ifstream ifile;
    std::ofstream ofile;
    std::string content = "";
    std::string cgi_header = "";
    size_t delimiterLength;
    size_t pos;
    
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

    if ((pos = content.find("\r\n\r\n")) != std::string::npos)
        delimiterLength = 4;
    else if ((pos = content.find("\n\n")) != std::string::npos)
        delimiterLength = 2;

    if (pos != std::string::npos)
    {
        cgi_header = content.substr(0, pos + delimiterLength);
        content = content.erase(0, pos + delimiterLength);
    }
    ofile.open(_cgi_file_path.c_str(), std::ios::binary);
    ofile << content;
    ofile.close();

    if (parseCgiHeader(cgi_header, content.length(), delimiterLength))
        setResStatus(ON_PROCESS);
}



