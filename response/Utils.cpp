#include "../includes/response/Utils.hpp"

std::string	Utils::toString(int value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}

std::string	Utils::ResponseHeaderToString(const t_responseHeader &responseHeader)
{
	std::stringstream ss;

	ss << "HTTP/1.1 " << responseHeader.statusCode << " " << responseHeader.statusMessage << "\r\n";

	std::map<std::string, std::string>::const_iterator it;
	for (it = responseHeader.headers.begin(); it != responseHeader.headers.end(); ++it)
	{
		ss << it->first << ": " << it->second << "\r\n";
	}
	ss << "\r\n";
 
	return ss.str();
}

bool	Utils::isValidURI(const std::string& uri) {
    
	std::string validChar = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.~/?#[]@!$&'()*+,;=";
	for (size_t i = 0; i < uri.length(); i++) {
		if (validChar.find(uri[i]) == std::string::npos)
			return false;
	}
    return true;
}

bool Utils::isDirectory(const std::string& path)
{
	DIR* dir = opendir(path.c_str());
    if (dir) {
        closedir(dir);
        return true;
    }
    return false;
}

bool Utils::fileExists(const std::string& path)
{
	std::ifstream file(path.c_str());
    return file.good();
}

std::string Utils::getStatusMessage(int statusCode)
{
	switch (statusCode)
	{
		case 200: return "OK";
		case 501: return "Not Implemented";
		case 400: return "Bad Request";
		case 404: return "Not Found";
		case 414: return "URI Too Long";
		default: return "Internal Server Error";
	}
}