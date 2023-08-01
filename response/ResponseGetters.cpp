#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

std::string Response::getRoot()
{
    if (_location && _location->root != "")
        return _location->root;
    return _client->get_server_block().getRoot() != "" ? _client->get_server_block().getRoot() : "www/html";
}

bool Response::getAutoIndex()
{
    if (_location && _location->autoindex != DEFAULT)
        return (_location->autoindex == ON) ? true : false;

    return _client->get_server_block().getAutoindex();
}


std::string Response::getErrorPagePath(int statusCode)
{
    // TODO: check if can location have also error_pages
	if (_location && _location->error_pages.find(statusCode) != _location->error_pages.end()
		&& Utils::fileExists(_location->error_pages[statusCode]))
	{
		return _location->error_pages[statusCode];
	}
    std::map<int, std::string>  pages = _client->get_server_block().getErrorPages();
    if (pages.find(statusCode) != pages.end() && Utils::fileExists(pages[statusCode]))
		return pages[statusCode];
	else if (Utils::fileExists("./defaultPages/" + Utils::toString(statusCode) + ".html"))
		return "./defaultPages/" + Utils::toString(statusCode) + ".html";
	else
		return "./defaultPages/500.html";
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
	// TODO: Verify this function (!! High Priority)
	std::vector<ConfLoca> locations = _client->get_server_block().getLocations();
	std::string requestPath = _client->get_request().getPath();
	// std::cout << "Request path: " << requestPath << std::endl;
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
		_location = new ConfLoca(locations[index]);
		if (isMethodAllowedInLocation())
		{
			// std::cout << "Allowed method in location" << std::endl;
			return true;
		}
		else
			return false;

	}
	// std::cout << "No matched location" << std::endl;
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

	// std::cout << BLUE << "++path: [" << path << "]   |  root: [" << root;
	// std::cout << "] --> [" << Utils::getWebservRootPath()  + root + path << "]" << RESET << std::endl;
	return  Utils::getWebservRootPath()  + root + path;
}

std::string Response::tmp_getRequestPath()
{
	std::string path = _client->get_request().getPath();
	std::string root = getRoot();
	if (_location)
	{
		
		return  path.replace(0, _location->path.length(), root);
	}

	// return path.replace(0, 1, root);
	return "";
}


std::string Response::isDirHasIndexFiles()
{
    std::string dirPath = getRequestPathFile();
    std::vector<std::string> indexFiles = _client->get_server_block().getIndex();
    for (size_t i = 0; i < indexFiles.size(); i++)
    {
        if (Utils::fileExists(dirPath + indexFiles[i]))
            return dirPath + indexFiles[i];
    }
    if (Utils::fileExists(dirPath + "index.html"))
        return dirPath + "index.html";

    return "";
}



bool Response::isServerHaveRedirection()
{
	t_responseHeader	responseHeader;
	int 				redirectStatus = _client->get_server_block().getRedirectStatus();
	if (redirectStatus == 301 || redirectStatus == 302 || redirectStatus == 307)
	{
		responseHeader.statusCode = redirectStatus;
		responseHeader.statusMessage = Utils::getStatusMessage(redirectStatus);
		responseHeader.headers["Location"] = _client->get_server_block().getRedirectUrl();
		responseHeader.headers["Server"] = _client->get_server_block().getServerName();

		_header_buffer = Utils::ResponseHeaderToString(responseHeader);
		_client->append_response_data(_header_buffer);
		setResStatus(DONE);
		return true;
	}
	else if (redirectStatus != -1)
	{
		responseHeader.statusCode = redirectStatus;
		responseHeader.statusMessage = Utils::getStatusMessage(redirectStatus);
		responseHeader.headers["Content-Type"] = "application/octet-stream";
		responseHeader.headers["Content-Length"] = "0";
		responseHeader.headers["Server"] = _client->get_server_block().getServerName();

		_header_buffer = Utils::ResponseHeaderToString(responseHeader);
		_client->append_response_data(_header_buffer);
		setResStatus(DONE);
		return true;
	}

	return false;
}


bool Response::isMethodAllowedInLocation()
{
	std::vector<MethodType> allow_methods;
	if (_location)
	{
		allow_methods = _location->allow_methods;
		if (std::find(allow_methods.begin(), allow_methods.end(), _location->strtoMethod(_client->get_request().getMethod())) != allow_methods.end())
			return true;
		else
		{
			// std::cout << "Method not allowed in location" << std::endl;
			errorPages(405);
			return false;
		}
	}
	return false;
}
