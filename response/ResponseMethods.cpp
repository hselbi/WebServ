#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

void Response::Method_GET()
{
    std::string filePath = getRoot() + _client->get_request().getPath();
    std::string index = "";
    std::cout << "FILE PATH: " << filePath << std::endl;
    if (Utils::fileExists(filePath))
    {
        std::cout << "File found"  << filePath << std::endl;

        if (Utils::isDirectory(filePath))
        {
            if (filePath[filePath.length() - 1] != '/')
            {
                std::cout << "REDIRECT TO: " << _client->get_request().getPath() + "/" << std::endl;
                setRediration(_client->get_request().getPath() + "/");
                return;
            }
            else
            {
                if ((index = isDirHasIndexFiles()) != "")
                {

                    if (_location && _location->cgi_infos.size() > 0)
                    {
                        std::cout << "Have CGI" << std::endl;
                        _have_cgi = true;
                        // TODO: Start cgi by index file path
                        _cgi_file = _client->get_cgi().start_cgi(index);
                        if (_cgi_file == -1)
                        {
                            std::cout << "CGI ERROR" << std::endl;
                            errorPages(500);
                            return;
                        }
                        readCgiFile();
                    }
                    else
                    {
                        std::cout << "Have index file "  << filePath<< std::endl;
                        readFileByPath(index);
                    }
                }
                else
                {
                    if (getAutoIndex())
                        autoIndex();
                    else
                        errorPages(403);
                    return;
                }
            }
        }
        else
        {
            if (_location && _location->cgi_infos.size() > 0)
            {
                std::cout << "Have CGI" << std::endl;
                _have_cgi = true;
                _cgi_file = _client->get_cgi().start_cgi(filePath);
                if (_cgi_file == -1)
                {
                    std::cout << "CGI ERROR" << std::endl;
                    errorPages(500);
                    return;
                }
                readCgiFile();
            }
            else
                readFileByPath(filePath);
        }
    }
    else
    {
        std::cout << "File not found"  << filePath << std::endl;
        errorPages(404);
        return;
    }
}

void Response::Method_DELETE()
{
    std::string filePath = getRoot() + _client->get_request().getPath();
    std::string index = "";
    if (Utils::fileExists(filePath))
    {
            
        if (Utils::isDirectory(filePath))
        {
            if (filePath[filePath.length() - 1] != '/')
                return errorPages(409);
            else
                 deleteAllFolderFiles();
        }
        else
            deleteFile();
    }
    else
        errorPages(404);
}

void Response::readCgiFile()
{
    t_responseHeader responseHeader;
    off_t fileSize;
    ssize_t bytesRead;
    int endHeaderPos = 0;
    char buffer[10000];

    fileSize = lseek(_cgi_file, 0, SEEK_END);

    if (fileSize == (off_t)-1)
    {
        std::cerr << "Failed to seek to the end of file!" << std::endl;
        close(_cgi_file);
        _client->set_status(DONE);
        errorPages(400);
    }

    if (lseek(_cgi_file, 0, SEEK_SET) == (off_t)-1)
    {
        std::cerr << "Failed to seek to the beginning of file!" << std::endl;
        close(_cgi_file);
        _client->set_status(DONE);
        errorPages(400);
    }

    while ((bytesRead = read(_cgi_file, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytesRead] = '\0';
        char *found = strstr(buffer, "\r\n\r\n");
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
    _client->set_status(ON_PROCESS);
}

