#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

void Response::Method_GET()
{
    std::string filePath = getRoot() + _client->get_request().getPath();
    std::string index = "";

    if (Utils::fileExists(filePath))
    {
        // std::cout << "File found"  << filePath << std::endl;

        if (Utils::isDirectory(filePath))
        {
            if (filePath[filePath.length() - 1] != '/')
            {
                std::cout << "REDIRECT TO: " << _client->get_request().getPath() + "/" << std::endl;
                return setRediration(_client->get_request().getPath() + "/");
            }
            else
            {
                if ((index = isDirHasIndexFiles()) != "")
                {
                    // TODO: Add python also to this condition  (if file extension is .py)
                    if (_location && _location->cgi_infos.size() > 0 && (index.find(".php") != std::string::npos || index.find(".py") != std::string::npos))
                    {
                        _have_cgi = true;
                        // std::cout << "Start CGI" << std::endl;
                        _cgi_file = _client->get_cgi().start_cgi(index);
						std::cout << "CGI FILE1: " << _cgi_file << std::endl;
                        std::cout << _cgi_file << std::endl;
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
            // TODO: Add python also to this condition  (if file extension is .py)
            if (_location && _location->cgi_infos.size() > 0 && (filePath.find(".php") != std::string::npos || filePath.find(".py") != std::string::npos))
            {
                // std::cout << "Have CGI" << std::endl;
                _have_cgi = true;
				// std::cout << "Start CGI" << std::endl;
				if (_client->get_cgi().get_cgi_status() == 0 && _client->get_cgi().get_ready_to_read_from_cgi() == 1)
				{
					_cgi_file = _client->get_cgi().start_cgi(filePath);
					// std::cout << "COND1: " << _cgi_file << std::endl;
					if (_cgi_file == -1)
					{
						std::cout << "CGI ERROR" << std::endl;
						errorPages(500);
						return;
					}
					_client->get_cgi().set_cgi_status(1);
					// _ready_to_read_from_cgi = 0;
				}
				if (_client->get_cgi().get_cgi_status()  == 1 && _client->get_cgi().get_ready_to_read_from_cgi() == 0)
				{
					// std::cout << "COND2: " << _cgi_file << std::endl;

					_client->get_cgi().set_ready_to_read_from_cgi(waitpid(_client->get_cgi().get_pid(), 0, WNOHANG));
					// std::cout << "READY TO READ FROM CGI: " << _client->get_cgi().get_ready_to_read_from_cgi() << std::endl;
					if (_client->get_cgi().get_ready_to_read_from_cgi() == 0)
					{
						if (time(NULL) - _client->get_cgi().get_start_time() >= TIMEOUT_CGI)
						{
							std::cout << "CGI TIMEOUT" << std::endl;
                            _have_cgi = false;
							// kill(0, SIGKILL);
							errorPages(500);
							return;
						}
					}
				}
				if (_client->get_cgi().get_cgi_status() == 1 && _client->get_cgi().get_ready_to_read_from_cgi() != 0)
                	{
					// std::cout << "COND3: " << _cgi_file << std::endl;

						readCgiFile();
					}
            }
            else
                readFileByPath(filePath);
        }
    }
    else
    {
        // std::cout << "File not found: "  << filePath << std::endl;
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


void Response::Method_POST()
{
    std::string filePath = getRoot() + _client->get_request().getPath();
    std::string index = "";

    if (Utils::fileExists(filePath))
    {
        // std::cout << "File found"  << filePath << std::endl;

        if (Utils::isDirectory(filePath))
        {
            if (filePath[filePath.length() - 1] != '/')
            {
                std::cout << "REDIRECT TO: " << _client->get_request().getPath() + "/" << std::endl;
                return setRediration(_client->get_request().getPath() + "/");
            }
            else
            {
                if ((index = isDirHasIndexFiles()) != "")
                {
                    // TODO: Add python also to this condition  (if file extension is .py)
                    if (_location && _location->cgi_infos.size() > 0 && (index.find(".php") != std::string::npos || index.find(".py") != std::string::npos))
                    {
                        // std::cout << "Have CGI" << std::endl;
                        _have_cgi = true;
                        std::cout << "Start CGI" << std::endl;
                        _cgi_file = _client->get_cgi().start_cgi(index);
                        // std::cout << _cgi_file << std::endl;
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
                    return errorPages(403);
            }
        }
        else
        {
            // TODO: Add python also to this condition  (if file extension is .py)
            if (_location && _location->cgi_infos.size() > 0 && (filePath.find(".php") != std::string::npos || filePath.find(".py") != std::string::npos))
            {
                // std::cout << "Have CGI" << std::endl;
                _have_cgi = true;
                _cgi_file = _client->get_cgi().start_cgi(filePath);
                // std::cout << _cgi_file << std::endl;

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
        // std::cout << "File not found: "  << filePath << std::endl;
        errorPages(404);
        return;
    }
}
