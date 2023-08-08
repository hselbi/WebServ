#include "../includes/response/Utils.hpp"

size_t Utils::counter = 0;

std::string Utils::getWebservRootPath() {
    int		bufferSize = 2048;
    char	buffer[bufferSize];

    if (getcwd(buffer, bufferSize) != NULL)
        return std::string(buffer);
    return "";
}

std::string	Utils::toString(int value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}

std::string	Utils::ResponseHeaderToString(const t_responseHeader &responseHeader)
{
	std::stringstream ss;

	ss << "HTTP/1.1 " << responseHeader.statusCode << " " << responseHeader.statusMessage << "\r\n";

	std::map<std::string, std::string>::const_iterator m_it;
	for (m_it = responseHeader.m_headers.begin(); m_it != responseHeader.m_headers.end(); ++m_it)
		ss << m_it->first << ": " << m_it->second << "\r\n";

	std::vector<Header>::const_iterator v_it;
	for (v_it = responseHeader.v_headers.begin(); v_it != responseHeader.v_headers.end(); ++v_it)
		ss << v_it->key << ": " << v_it->value << "\r\n";

	ss << "\r\n";
	return ss.str();
}

std::string Utils::uriDecode(const std::string &uri) {
	std::string result;
    for (size_t i = 0; i < uri.length(); ++i) {
        if (uri[i] == '%') {
            if (i + 2 < uri.length()) {
                std::istringstream iss(uri.substr(i + 1, 2));
                int hexValue;
                iss >> std::hex >> hexValue;
                result += static_cast<char>(hexValue);
                i += 2;
            }
        } else {
            result += uri[i];
        }
    }
    return result;
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

bool Utils::isExecutable(const std::string& path)
{
	struct stat st;
	if (stat(path.c_str(), &st) == 0)
		return (st.st_mode & S_IXUSR) != 0;
	return false;
}

std::string Utils::getStatusMessage(int statusCode)
{
	switch (statusCode)
	{
		case 200: return "OK";
		case 201: return "Created";
		case 202: return "Accepted";
		case 203: return "Non-Authoritative Information";
		case 204: return "No Content";
		case 205: return "Reset Content";
		case 206: return "Partial Content";
		case 300: return "Multiple Choices";
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 303: return "See Other";
		case 304: return "Not Modified";
		case 305: return "Use Proxy";
		case 307: return "Temporary Redirect";
		case 400: return "Bad Request";
		case 401: return "Unauthorized";
		case 402: return "Payment Required";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 406: return "Not Acceptable";
		case 407: return "Proxy Authentication Required";
		case 408: return "Request Time-out";
		case 409: return "Conflict";
		case 410: return "Gone";
		case 411: return "Length Required";
		case 412: return "Precondition Failed";
		case 413: return "Request Entity Too Large";
		case 414: return "URI Too Long";
		case 415: return "Unsupported Media Type";
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		case 503: return "Service Unavailable";
		case 504: return "Gateway Time-out";
		case 505: return "HTTP Version not supported";
		default: return "";
	}
}

std::string Utils::getExtensionFile(const std::string &path)
{
	size_t pos = path.find_last_of(".");
	if (pos != std::string::npos)
		return path.substr(pos + 1);
	return "";
}

bool	Utils::isValidURI(const std::string& uri) {
    
	std::string validChar = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.~/?#[]@!$&'()*+,;=%";
	for (size_t i = 0; i < uri.length(); i++) {
		if (validChar.find(uri[i]) == std::string::npos)
			return false;
	}
    return true;
}


std::string Utils::generateFileName()
{
	srand(time(NULL));
	std::string name =  std::to_string(rand()) + std::to_string(rand()) + std::to_string(counter++);

    return name;
}