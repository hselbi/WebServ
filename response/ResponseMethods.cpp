#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

void    Response::Method_GET()
{
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
                {
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