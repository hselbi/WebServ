#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

void Response::Method_GET()
{
    std::string filePath = getRequestPathFile();
    std::string index = "";

    if (Utils::fileExists(filePath))
    {
        if (Utils::isDirectory(filePath))
        {
            if (filePath[filePath.length() - 1] != '/')
                return sendRediraction(_client->get_request().getPath() + "/");
            else
            {
                if ((index = isDirHasIndexFiles()) != "")
                {
                    if (checkStatusCgi() && (index.rfind(".php") != std::string::npos || index.rfind(".py") != std::string::npos))
                    {
                        _have_cgi = true;
                        if (_client->get_cgi().get_cgi_status() == 0 && _client->get_cgi().get_ready_to_read_from_cgi() == 1)
                        {
                            _cgi_file_path = startCgi(index);
                            if (_cgi_file_path == "-1")
                                 return errorPages(500);
                            _client->get_cgi().set_cgi_status(1);
                        }
                        if (_client->get_cgi().get_cgi_status() == 1)
                        {
                            if (_client->get_cgi().get_ready_to_read_from_cgi() == 0)
                                _client->get_cgi().set_ready_to_read_from_cgi(waitpid(_client->get_cgi().get_pid(), 0, WNOHANG));
                            else
                                readCgiFile();
                        }
                    }
                    else
                        readFileByPath(index);
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
            if (checkStatusCgi() && (filePath.rfind(".php") != std::string::npos || filePath.rfind(".py") != std::string::npos))
            {
                _have_cgi = true;
                if (_client->get_cgi().get_cgi_status() == 0 && _client->get_cgi().get_ready_to_read_from_cgi() == 1)
                {
                    _cgi_file_path = startCgi(filePath);
                    if (_cgi_file_path == "-1")
                        return errorPages(500);
                    _client->get_cgi().set_cgi_status(1);
                }

                if (_client->get_cgi().get_cgi_status() == 1)
                {
                    if (_client->get_cgi().get_ready_to_read_from_cgi() == 0)
                        _client->get_cgi().set_ready_to_read_from_cgi(waitpid(_client->get_cgi().get_pid(), 0, WNOHANG));
                    else
                        readCgiFile();
                }
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

void Response::Method_DELETE()
{
    std::string filePath = getRequestPathFile();
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
    std::string filePath = getRequestPathFile();
    std::string index = "";
    if (Utils::fileExists(filePath))
    {
        if (Utils::isDirectory(filePath))
        {
            if (filePath[filePath.length() - 1] != '/')
                return  errorPages(400);
            else
            {
                if ((index = isDirHasIndexFiles()) != "")
                {
                    if (checkStatusCgi())
                    {
                        _have_cgi = true;
                        if (_client->get_cgi().get_cgi_status() == 0 && _client->get_cgi().get_ready_to_read_from_cgi() == 1)
                        {
                            _cgi_file_path = startCgi(index);
                            if (_cgi_file_path == "-1")
                                 return errorPages(500);
                            _client->get_cgi().set_cgi_status(1);
                        }
                        if (_client->get_cgi().get_cgi_status() == 1)
                        {
                            if (_client->get_cgi().get_ready_to_read_from_cgi() == 0)
                                _client->get_cgi().set_ready_to_read_from_cgi(waitpid(_client->get_cgi().get_pid(), 0, WNOHANG));
                            else
                                readCgiFile();
                        }
                    }
                    else
                        errorPages(403);
                }
                else
                    errorPages(403);
            }
        }
        else
        {
            if (checkStatusCgi())
            {
                _have_cgi = true;
                if (_client->get_cgi().get_cgi_status() == 0 && _client->get_cgi().get_ready_to_read_from_cgi() == 1)
                {
                    _cgi_file_path = startCgi(filePath);
                    if (_cgi_file_path == "-1")
                        return errorPages(500);
                    _client->get_cgi().set_cgi_status(1);
                }

                if (_client->get_cgi().get_cgi_status() == 1)
                {
                    if (_client->get_cgi().get_ready_to_read_from_cgi() == 0)
                        _client->get_cgi().set_ready_to_read_from_cgi(waitpid(_client->get_cgi().get_pid(), 0, WNOHANG));
                    else
                        readCgiFile();
                }
            }
            else
                errorPages(403);
        }
    }
    else
    {
        errorPages(404);
        return;
    }
}
