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
                return setRediration(_client->get_request().getPath() + "/");
            else
            {
                if ((index = isDirHasIndexFiles()) != "")
                {
                    // TODO: Add python also to this condition  (if file extension is .py)
                    if (_location && _location->cgi_infos.size() > 0 && (index.rfind(".php") != std::string::npos || index.rfind(".py") != std::string::npos))
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
            // TODO: Add python also to this condition  (if file extension is .py)
            if (_location && _location->cgi_infos.size() > 0 && (filePath.rfind(".php") != std::string::npos || filePath.rfind(".py") != std::string::npos))
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

// void    Response::Method_POST()
// {

//     // check if the request is formed
//     // std::string _response;
//     // int _code;
//     // std::string _type;

//     std::cout << YELLOW << "POST method\n" << std::endl;;
    
//     size_t i = 0;
//     size_t j = _response.size() - 2;

//     while (_response.find("\r\n\r\n", i) != std::string::npos || _response.find("\r\n", i) == i)
//     {
//         std::string str = _response.substr(i, _response.find("\r\n", i) - i);
//         if (str.find("Status: ") == 0)
//             _code = std::atoi(str.substr(8, 3).c_str());
//         else if (str.find("Content-Type: ") == 0)
//             _type = str.substr(14, str.size());
//         i += str.size() + 2;
//     }
//     while (_response.find("\r\n", j) == j)
//         j -= 2;
    
//     _response = _response.substr(i, j - i);

//     std::cout << "RESPONSE: " << _response << std::endl;

//     // there's no content-type header
//     if (_response.find("Content-Type: ") == std::string::npos)
//     {
//         std::cout << "NO CONTENT-TYPE" << std::endl;
//         errorPages(400);
//         return;
//     }

//     // 204 No Content
//     if (_response.find("Content-Length: 0") != std::string::npos)
//     {
//         std::cout << "NO CONTENT-LENGTH" << std::endl;
//         errorPages(204);
//         return;
//     }

//     // 500 Internal Server Error
//     if (_response.find("Status: 500") != std::string::npos)
//     {
//         std::cout << "500 Internal Server Error" << std::endl;
//         errorPages(500);
//         return;
//     }    
    
// }

// void    Response::Method_POST()
// {
//     std::string _response;
//     std::cout << "Post" << std::endl;
//     std::cout << _client->get_request().getBody() << std::endl;
//     std::cout << "Post" << std::endl;

//     std::cout << YELLOW << "POST method\n" << std::endl;;

// 	if (_client->get_request().getHeaders()["Content-Length"] != "chunked" 
// 	&& _client->get_request().getHeaders()["Content-Length"].empty())
// 	{
//         errorPages(411);
// 		return;
// 	}
	
//     if (!_client->get_request().getHeaders()["Content-Length"].empty())
//     {
//         size_t begin = _client->get_request().getHeaders()["Content-Type"].find("boundary=");
//         if (begin != std::string::npos)
//         {
//             std::string boundary = _client->get_request().getHeaders()["Content-Type"].substr(begin + 9);
//             begin = 0;
//             size_t end = 0;
//             std::string name;
//             while (true)
//             {
//                 // inpyt name
//                 begin = _client->get_request().getBody().find("name=", begin) + 6;
//                 // input value
//                 end = _client->get_request().getBody().find_first_of("\"", begin);
//                 if (begin == std::string::npos || end == std::string::npos)
//                     break;
//                 name = _client->get_request().getBody().substr(begin, end - begin);
//                 begin = _client->get_request().getBody().find("\r\n\r\n", end) + 4;
//                 end = _client->get_request().getBody().find(boundary, begin);
//                 if (begin == std::string::npos || end == std::string::npos)
//                     break;
//                 if (write_file_in_path(client, _client->get_request().getBody().substr(begin, end - begin - 4), full_path + "/" + name) < 0)
//                     break;
//                 if (_client->get_request().getBody()[end + boundary.size()] == '-')
//                     break;
//             }
//         }
//         else
//         {
//             errorPages(400);
//             // ERROR PAGE 400
//             return;
//         }
//     }
//     else
//     {
//             errorPages(400);
//             // ERROR PAGE 400
//         return;
//     }

// 	int code = 201;
// 	if (_client->get_request().getHeaders()["Content-Length"] == "0")
// 		code = 204;

// 	// Response response(status_info[code]);
// 	// std::string header = response.make_header();
// 	// int send_ret = send(_client.get, header.c_str(), header.size(), 0);
// 	// if (send_ret < 0)
//     //     // ERROR PAGE 500 (something went wrong) send_ret == -1
// 	// else if (send_ret == 0)
//     //     // ERROR PAGE 400 (client closed the connection)
// 	// else
// 	// 	std::cout << "> " << full_path << " posted(" << code << ")\n";
// }



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
                    if (_location && _location->cgi_infos.size() > 0)
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
            if (_location && _location->cgi_infos.size() > 0)
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
