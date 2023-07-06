#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

std::string Response::getRoot()
{

    if (_location && _location->root != "")
        return _location->root;
    return _client->get_server_block().getRoot() != "" ? _client->get_server_block().getRoot() : "./www/html";
}

bool Response::getAutoIndex()
{
    // TODO: check if can location have also autoindex

    // if (_location && _location->root != "")
    //     return _location->root;
    return _client->get_server_block().getAutoindex();
}


std::string Response::getErrorPagePath(int statusCode)
{
    // TODO: check if can location have also error_pages

    std::map<int, std::string>  pages = _client->get_server_block().getErrorPages();
    if (pages.find(statusCode) != pages.end() && Utils::fileExists(pages[statusCode]))
        return pages[statusCode];

    return "./defaultPages/" + Utils::toString(statusCode) + ".html";
}

std::string Response::isDirHasIndexFiles()
{
    std::string dirPath = getRoot() + _client->get_request().getPath();
    std::vector<std::string> indexFiles = _client->get_server_block().getIndex();
    for (size_t i = 0; i < indexFiles.size(); i++)
    {
        if (Utils::fileExists(dirPath + indexFiles[i]))
            return dirPath + indexFiles[i];
    }

    // TODO: Add index.php after fix php-cgi
	std::string defaultPages[] = {"index.html", "index.htm"};
    for (size_t i = 0; i < 2; i++)
    {
        if (Utils::fileExists(dirPath + defaultPages[i]))
            return dirPath + defaultPages[i];
    }

    return "";
}

std::string Response::getContentType(const std::string &filePath)
{
	std::map<std::string, std::string>::const_iterator it;
	std::map<std::string, std::string> contentTypes;
	std::string extension;
	size_t dotPos;

	contentTypes[".html"] = "text/html";
	contentTypes[".htm"] = "text/html";
	contentTypes[".jpg"] = "image/jpeg";
	contentTypes[".jpeg"] = "image/jpeg";
	contentTypes[".png"] = "image/png";
	contentTypes[".gif"] = "image/gif";
	contentTypes[".pdf"] = "application/pdf";
	contentTypes[".mp4"] = "video/mp4";
    contentTypes[".css"] = "text/css";
    contentTypes[".js"] = "application/javascript";
    contentTypes[".php"] = "text/html";


	dotPos = filePath.rfind('.');
	if (dotPos != std::string::npos)
		extension = filePath.substr(dotPos);

	it = contentTypes.find(extension);
	if (it != contentTypes.end())
		return it->second;

	return "text/plain";
}
