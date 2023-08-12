#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

std::string Response::getRoot()
{
    if (_location && _location->root != "")
		return getCorrectPath(_location->root);
	else if (_client->get_server_block().getRoot() != "")
		return getCorrectPath(_client->get_server_block().getRoot());
	return Utils::getWebservRootPath() + "/www/html";
}

std::string Response::getCorrectPath(const std::string &path)
{
	std::string server_path = "[SERVER_PATH]";
	size_t pos = path.find(server_path);
	if (pos == std::string::npos)
		return path;
	else
	{
		std::string root = path;
		root.replace(pos, server_path.length(), Utils::getWebservRootPath());
		return root;
	}
}

bool Response::getAutoIndex()
{
    if (_location && _location->autoindex != DEFAULT)
        return (_location->autoindex == ON) ? true : false;

    return _client->get_server_block().getAutoindex();
}


std::string Response::getErrorPagePath(int statusCode)
{
	if (_location && _location->error_pages.find(statusCode) != _location->error_pages.end()
		&& Utils::fileExists(getCorrectPath(_location->error_pages[statusCode])))
	{
		return getCorrectPath(_location->error_pages[statusCode]);
	}
	
    std::map<int, std::string>  pages = _client->get_server_block().getErrorPages();
    if (pages.find(statusCode) != pages.end() && Utils::fileExists(getCorrectPath(pages[statusCode])))
		return getCorrectPath(pages[statusCode]);
	else if (Utils::fileExists("./default_pages/" + Utils::toString(statusCode) + ".html"))
		return "./default_pages/" + Utils::toString(statusCode) + ".html";
	else
		return "./default_pages/500.html";
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
    contentTypes[".ttf"] = "font/ttf";
    contentTypes[".svg"] = "image/svg+xml";
    contentTypes[".ico"] = "image/x-icon";
    contentTypes[".txt"] = "text/plain";
    contentTypes[".zip"] = "application/zip";
    contentTypes[".gz"] = "application/gzip";
    contentTypes[".tar"] = "application/x-tar";
    contentTypes[".xml"] = "application/xml";
    contentTypes[".json"] = "application/json";
    contentTypes[".mp3"] = "audio/mpeg";
    contentTypes[".wav"] = "audio/wav";
    contentTypes[".avi"] = "video/x-msvideo";
    contentTypes[".mpeg"] = "video/mpeg";
    contentTypes[".mpg"] = "video/mpeg";
    contentTypes[".webm"] = "video/webm";
    contentTypes[".wmv"] = "video/x-ms-wmv";
    contentTypes[".flv"] = "video/x-flv";
    contentTypes[".ogg"] = "video/ogg";
    contentTypes[".ogv"] = "video/ogg";
    contentTypes[".ogx"] = "application/ogg";
    contentTypes[".3gp"] = "video/3gpp";

	dotPos = filePath.rfind('.');
	if (dotPos != std::string::npos)
		extension = filePath.substr(dotPos);

	it = contentTypes.find(extension);
	if (it != contentTypes.end())
		return it->second;

	return "application/octet-stream";
}

bool	Response::getMatchedLocation()
{
	std::vector<ConfLoca> locations = _client->get_server_block().getLocations();
	std::string requestPath = _client->get_request().getPath();
	size_t max_length = 0;
	int		index = -1;
	for (int i = 0; i < locations.size(); i++)
	{
		if (requestPath.find(locations[i].path) == 0 && locations[i].path.length() > 0)
		{
			max_length = locations[i].path.length();
			index = i;
		}
	}
	if (index != -1)
	{
		if (_location == NULL)
			_location = new ConfLoca(locations[index]);
		
		if (isLocationHaveRedirection())
			return false;
		return isMethodAllowedInLocation();
	}
	errorPages(404);
	return false;
}

std::string Response::getRequestPathFile()
{
	std::string path = _client->get_request().getPath();
	std::string root = getRoot();
	if (_location && path.find(_location->path) == 0)
		path =  (_location->path != "/") ? path.substr(_location->path.length()) : path;
	else
		path = "/";
	return  root + path;
}

ConfLoca * Response::getLocation() { return _location; }
bool Response::get_cgi_status() { return _have_cgi; }
