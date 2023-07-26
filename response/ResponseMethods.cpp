#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

void    Response::Method_GET()
{
    std::cout << "GET GET" << std::endl;
	std::string filePath = getRoot()  + _client->get_request().getPath();
    std::string index = "";
    if (Utils::fileExists(filePath))
    {
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
                    return readFileByPath(index);
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
            _location->print_loca_info();
            std::cout << "Size : " << _location->cgi_infos.size() << std::endl;
            
            if (_location && _location->cgi_infos.size() > 0)
            {
                std::cout << "Have CGI" << std::endl;
                _have_cgi = true;
                _cgi_file = _client->get_cgi().start_cgi();
                readCgiFile();
            }
            else 
                readFileByPath(filePath);
        }
    }
    else
    {
        errorPages(404);
        return;
    }
}


void Response::readCgiFile()
{
	t_responseHeader responseHeader;
	off_t fileSize;

	
	fileSize = lseek(_cgi_file, 0, SEEK_END);
    
    if (fileSize == (off_t)-1) {
        // TODO: ADD BAD RESPONSE
        std::cerr << "Failed to seek to the end of file!" << std::endl;
        close(_cgi_file);
        _client->set_status(DONE);
    }

    if (lseek(_cgi_file, 0, SEEK_SET) == (off_t)-1) {
        std::cerr << "Failed to seek to the beginning of file!" << std::endl;
        close(_cgi_file);
        _client->set_status(DONE);
    }

	responseHeader.statusCode = 200;
	responseHeader.statusMessage = Utils::getStatusMessage(200);
	responseHeader.headers["Content-Type"] = "text/php";
	responseHeader.headers["Content-Length"] = Utils::toString(fileSize);
	responseHeader.headers["Server"] = _client->get_server_block().getServerName();

	_header_buffer = Utils::ResponseHeaderToString(responseHeader);
	_client->set_status(ON_PROCESS);
}
