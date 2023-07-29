#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

void    Response::Method_GET()
{
	std::string filePath = getRoot()  + _client->get__client->get_request()uest().getPath();
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
                {
                    if (_location && _location->cgi_infos.size() > 0)
                    {
                        // TODO: CGI
                    }
                    else 
                        readFileByPath(index);
                    return;
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
            readFileByPath(filePath);
        }
    }
    else
    {
        errorPages(404);
        return;
    }
}

void    Response::Method_POST()
{
    std::string _response;
    std::cout << "Post" << std::endl;
    std::cout << _client->get_request().getBody() << std::endl;
    std::cout << "Post" << std::endl;
    /*********************************/

    std::cout << YELLOW << "POST method\n" << std::endl;;

	if (_client->get_request().getHeaders()["Content-Length"] != "chunked" 
	&& _client->get_request().getHeaders()["Content-Length"].empty())
	{
        errorPages(411);
        // ERROR PAGE 411
		return;
	}
	
    if (!_client->get_request().getHeaders()["Content-Length"].empty())
    {
        size_t begin = _client->get_request().getHeaders()["Content-Type"].find("boundary=");
        if (begin != std::string::npos)
        {
            std::string boundary = _client->get_request().getHeaders()["Content-Type"].substr(begin + 9);
            begin = 0;
            size_t end = 0;
            std::string name;
            while (true)
            {
                // inpyt name
                begin = _client->get_request().getBody().find("name=", begin) + 6;
                // input value
                end = _client->get_request().getBody().find_first_of("\"", begin);
                if (begin == std::string::npos || end == std::string::npos)
                    break;
                name = _client->get_request().getBody().substr(begin, end - begin);
                begin = _client->get_request().getBody().find("\r\n\r\n", end) + 4;
                end = _client->get_request().getBody().find(boundary, begin);
                if (begin == std::string::npos || end == std::string::npos)
                    break;
                if (write_file_in_path(client, _client->get_request().getBody().substr(begin, end - begin - 4), full_path + "/" + name) < 0)
                    break;
                if (_client->get_request().getBody()[end + boundary.size()] == '-')
                    break;
            }
        }
        else
        {
            errorPages(400);
            // ERROR PAGE 400
            return;
        }
    }
    else
    {
            errorPages(400);
            // ERROR PAGE 400
        return;
    }

	int code = 201;
	if (_client->get_request().getHeaders()["Content-Length"] == "0")
		code = 204;

	// Response response(status_info[code]);
	// std::string header = response.make_header();
	// int send_ret = send(_client.get, header.c_str(), header.size(), 0);
	// if (send_ret < 0)
    //     // ERROR PAGE 500 (something went wrong) send_ret == -1
	// else if (send_ret == 0)
    //     // ERROR PAGE 400 (client closed the connection)
	// else
	// 	std::cout << "> " << full_path << " posted(" << code << ")\n";
}



