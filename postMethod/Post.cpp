#include "Post.hpp"




void			Response::postMethod(Request & request, RequestConfig & requestConf)
{
	ResponseHeader	head;

	if (requestConf.getCgiPass() != "")
	{
		CgiHandler	cgi(request, requestConf);
		size_t		i = 0;
		size_t		j = _response.size() - 2;

		_response = cgi.executeCgi(requestConf.getCgiPass());

		while (_response.find("\r\n\r\n", i) != std::string::npos || _response.find("\r\n", i) == i)
		{
			std::string	str = _response.substr(i, _response.find("\r\n", i) - i);
			if (str.find("Status: ") == 0)
				_code = std::atoi(str.substr(8, 3).c_str());
			else if (str.find("Content-Type: ") == 0)
				_type = str.substr(14, str.size());
			i += str.size() + 2;
		}
		while (_response.find("\r\n", j) == j)
			j -= 2;

		_response = _response.substr(i, j - i);
	}
	else
	{
		_code = 204;
		_response = "";
	}
	if (_code == 500)
		_response = this->readHtml(_errorMap[_code]);
	_response = head.getHeader(_response.size(), _path, _code, _type, requestConf.getContentLocation(), requestConf.getLang()) + "\r\n" + _response;
}

int		pathIsFile(const std::string& path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0 )
	{
		if (s.st_mode & S_IFDIR)
			return 0;
		else if (s.st_mode & S_IFREG)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

std::string		Response::readHtml(const std::string& path)
{
	std::ofstream		file;
	std::stringstream	buffer;

	if (pathIsFile(path))
	{
		file.open(path.c_str(), std::ifstream::in);
		if (file.is_open() == false)
			return ("<!DOCTYPE html>\n<html><title>40404</title><body>There was an error finding your error page</body></html>\n");

		buffer << file.rdbuf();
		file.close();
		_type = "text/html";

		return (buffer.str());
	}
	else
		return ("<!DOCTYPE html>\n<html><title>40404</title><body>There was an error finding your error page</body></html>\n");
}

// Member functions

std::string		ResponseHeader::getHeader(size_t size, const std::string& path, int code, std::string type, const std::string& contentLocation, const std::string& lang)
{
	std::string	header;

	resetValues();
	setValues(size, path, code, type, contentLocation, lang);

	header = "HTTP/1.1 " + to_string(code) + " " + getStatusMessage(code) + "\r\n";
	header += writeHeader();

	return (header);
}

std::string		ResponseHeader::notAllowed(std::set<std::string> methods, const std::string& path, int code, const std::string& lang)
{
	std::string	header;

	resetValues();
	setValues(0, path, code, "", path, lang);
	setAllow(methods);

	if (code == 405)
		header = "HTTP/1.1 405 Method Not Allowed\r\n";
	else if (code == 413)
		header = "HTTP/1.1 413 Payload Too Large\r\n";
	header += writeHeader();

	return (header);
}

std::string		ResponseHeader::writeHeader(void)
{
	std::string	header = "";

	if (_allow != "")
		header += "Allow: " + _allow + "\r\n";
	if (_contentLanguage != "")
		header += "Content-Language: " + _contentLanguage + "\r\n";
	if (_contentLength != "")
		header += "Content-Length: " + _contentLength + "\r\n";
	if (_contentLocation != "")
		header += "Content-Location: " + _contentLocation + "\r\n";
	if (_contentType != "")
		header += "Content-Type: " + _contentType + "\r\n";
	if (_date != "")
		header += "Date: " + _date + "\r\n";
	if (_lastModified != "")
		header += "Last-Modified: " + _lastModified + "\r\n";
	if (_location != "")
		header += "Location: " + _location + "\r\n";
	if (_retryAfter != "")
		header += "Retry-After: " + _retryAfter + "\r\n";
	if (_server != "")
		header += "Server: " + _server + "\r\n";
	if (_transferEncoding != "")
		header += "Transfer-Encoding: " + _transferEncoding + "\r\n";
	if (_wwwAuthenticate != "")
		header += "WWW-Authenticate: " + _wwwAuthenticate + "\r\n";
	// header += "\r\n";

	return (header);
}

std::string		ResponseHeader::getStatusMessage(int code)
{
	if (_errors.find(code) != _errors.end())
		return _errors[code];
	return ("Unknown Code");
}

void			ResponseHeader::initErrorMap()
{
	_errors[100] = "Continue";
	_errors[200] = "OK";
	_errors[201] = "Created";
	_errors[204] = "No Content";
	_errors[400] = "Bad Request";
	_errors[403] = "Forbidden";
	_errors[404] = "Not Found";
	_errors[405] = "Method Not Allowed";
	_errors[413] = "Payload Too Large";
	_errors[500] = "Internal Server Error";
}

void			ResponseHeader::setValues(size_t size, const std::string& path, int code, std::string type, const std::string& contentLocation, const std::string& lang)
{
	setAllow();
	setContentLanguage(lang);
	setContentLength(size);
	setContentLocation(contentLocation, code);
	setContentType(type, path);
	setDate();
	setLastModified(path);
	setLocation(code, path);
	setRetryAfter(code, 3);
	setServer();
	setTransferEncoding();
	setWwwAuthenticate(code);
}

void			ResponseHeader::resetValues(void)
{
	_allow = "";
	_contentLanguage = "";
	_contentLength = "";
	_contentLocation = "";
	_contentType = "";
	_date = "";
	_lastModified = "";
	_location = "";
	_retryAfter = "";
	_server = "";
	_transferEncoding = "";
	_wwwAuthenticate = "";
	initErrorMap();
}

// Setter functions

void			ResponseHeader::setAllow(std::set<std::string> methods)
{
	std::set<std::string>::iterator it = methods.begin();

	while (it != methods.end())
	{
		_allow += *(it++);

		if (it != methods.end())
			_allow += ", ";
	}
}

void			ResponseHeader::setAllow(const std::string& allow)
{
	_allow = allow;
}

void			ResponseHeader::setContentLanguage(const std::string& lang)
{
	_contentLanguage = lang;
}

void			ResponseHeader::setContentLength(size_t size)
{
	_contentLength = to_string(size);
}

void			ResponseHeader::setContentLocation(const std::string& path, int code)
{
	(void)code;
	// if (code != 404)
		_contentLocation = path;
}

void			ResponseHeader::setContentType(std::string type, std::string path)
{
	if (type != "")
	{
		_contentType = type;
		return ;
	}
	type = path.substr(path.rfind(".") + 1, path.size() - path.rfind("."));
	if (type == "html")
		_contentType = "text/html";
	else if (type == "css")
		_contentType = "text/css";
	else if (type == "js")
		_contentType = "text/javascript";
	else if (type == "jpeg" || type == "jpg")
		_contentType = "image/jpeg";
	else if (type == "png")
		_contentType = "image/png";
	else if (type == "bmp")
		_contentType = "image/bmp";
	else
		_contentType = "text/plain";
}

void			ResponseHeader::setDate(void)
{
	char			buffer[100];
	struct timeval	tv;
	struct tm		*tm;

	gettimeofday(&tv, NULL);
	tm = gmtime(&tv.tv_sec);
	strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", tm);
	_date = std::string(buffer);
}

void			ResponseHeader::setLastModified(const std::string& path)
{
	char			buffer[100];
	struct stat		stats;
	struct tm		*tm;

	if (stat(path.c_str(), &stats) == 0)
	{
		tm = gmtime(&stats.st_mtime);
		strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", tm);
		_lastModified = std::string(buffer);
	}
}

void			ResponseHeader::setLocation(int code, const std::string& redirect)
{
	if (code == 201 || code / 100 == 3)
	{
		_location = redirect;
	}
}

void			ResponseHeader::setRetryAfter(int code, int sec)
{
	if (code == 503 || code == 429 || code == 301)
	{
		_retryAfter = to_string(sec);
	}
}

void			ResponseHeader::setServer(void)
{
	_server = "Weebserv/1.0.0 (Unix)";
}

void			ResponseHeader::setTransferEncoding(void)
{
	_transferEncoding = "identity";
}

void			ResponseHeader::setWwwAuthenticate(int code)
{
	if (code == 401)
	{
		_wwwAuthenticate = "Basic realm=\"Access requires authentification\" charset=\"UTF-8\"";
	}
}

// Overloaders

ResponseHeader & ResponseHeader::operator=(const ResponseHeader & src)
{
	(void)src;
	return (*this);
}

// Constructors and destructors

ResponseHeader::ResponseHeader(void)
{
	resetValues();
}

ResponseHeader::ResponseHeader(const ResponseHeader & src)
{
	(void)src;
}

ResponseHeader::~ResponseHeader(void)
{
}

std::string FileExtension()
{
    std::string path = RequestHeaders["Content-Type"];
    if (path.find("text/css") != std::string::npos)
        return ".css";
    if (path.find("text/csv") != std::string::npos)
        return ".csv";
    if (path.find("image/gif") != std::string::npos)
        return "gif";
    if (path.find("text/htm") != std::string::npos)
        return ".html";
    if (path.find("text/html") != std::string::npos)
        return ".html";
    if (path.find("video/mp4") != std::string::npos)
        return ".mp4";
    if (path.find("image/x-icon") != std::string::npos)
        return ".ico";
    if (path.find("image/jpeg") != std::string::npos)
        return ".jpeg";
    if (path.find("image/jpg") != std::string::npos)
        return ".jpeg";
    if (path.find("application/javascript") != std::string::npos)
        return ".js";
    if (path.find("application/json") != std::string::npos)
        return ".json";
    if (path.find("image/png") != std::string::npos)
        return ".png";
    if (path.find("application/pdf") != std::string::npos)
        return ".pdf";
    if (path.find("image/svg+xml") != std::string::npos)
        return ".svg";
    if (path.find("text/plain") != std::string::npos)
        return ".txt";
    return "";
}

/*

! size_read is ===> bytes_read <=== for US
*/
void getBody(const char *str, size_t size_read)
{
    size_t PosChunked = RequestHeaders["Transfer-Encoding"].find("chunked");

    if (PosChunked != std::string::npos)
        ParseChunked(str, size_read);
    else
        Binary(str, size_read);
}

void Binary(const char *str, size_t size_read)
{
    std::fstream requestBody;
    requestBody.open("bodyRequest" + FileExtension(), std::ios_base::out | std::ios_base::app | std::ios_base::binary);
    requestBody.write(str, size_read);
    lenghtBody += size_read;
    if ((unsigned int)stringToInt((RequestHeaders["Content-Length"])) == (i - lenghtHeader))
    {
        setlenghtBodyFile(stringToInt((RequestHeaders["Content-Length"])));
        set_has_request(true);
        requestBody.close();
    }
}

size_t FindStringInBuffer(const char *buffer, size_t buffer_size, const char *str, int pos)
{
    size_t str_len = strlen(str);
    if (buffer_size < str_len)
        return std::string::npos;
    if (pos == 0)
    {
        for (size_t i = 0; i < buffer_size - str_len + 1; i++)
            if (strncmp(buffer + i, str, str_len) == 0)
                return i;
    }
    else
    {
        for (size_t i = pos; i < buffer_size - str_len + 1; i++)
            if (strncmp(buffer + i, str, str_len) == 0)
                return i;
    }
    return std::string::npos;
}


void	setlenghtBodyFile(std::streampos len){lenghtBodyFile = len;}

Post::Post(Request &req)
{
    std::string _response; /*this one from response */

    size_t i = 0;
    size_t j = _response.size() - 1;
    
	/********************************/

	if (fileExists((std::string("/Users/slops/Desktop/webserv_team/") + "bodyRequest" + FileExtension()).c_str()))
	{
		std::remove((std::string("/Users/slops/Desktop/webserv_team/") + "bodyRequest" + FileExtension()).c_str());
	}
	std::ofstream requestBody;
	size_t PosChunked = RequestHeaders["Transfer-Encoding"].find("chunked");
	requestBody.open("bodyRequest" + FileExtension(), std::ios_base::out | std::ios_base::app | std::ios_base::binary);
	std::string ss(str, size_read);
	lenghtHeader = ss.find("\r\n\r\n") + 4;
	body.assign(ss.erase(0, lenghtHeader));
	if (PosChunked != std::string::npos)
	{
		size_t pos = 0;
		std::string token;
		while ((pos = body.find("\r\n")) != std::string::npos)
		{
			token = body.substr(0, pos);
			if (isHexadecimal(token))
			{
				if (token == "0")
				{
					requestBody.close();
					// make flag true is finished
					// has_request
					set_has_request(true);
					return(0);
				}
				body.erase(0, pos + 2);
				continue;
			}
			requestBody << token + "\r\n";
			body.erase(0, pos + 2);
		}
	}
	requestBody << body;
	if ((unsigned int)stringToInt((RequestHeaders["Content-Length"])) == (i - lenghtHeader)){
		requestBody.close();
		set_has_request(true);
		return 0;
	}
	requestBody.seekg(0, std::ios::end);
    int lenghtBodyFile = requestBody.tellg();
    setlenghtBodyFile(lenghtBodyFile);
    requestBody.seekg(0, std::ios::beg);
    requestBody.close();


    // ! 1st etap: check is cgi exist or not

    if (/* ^ cgi exist */)
    {
        while (_response.find("\r\n\r\n", i) != std::string::npos || _response.find("\r\n", i) == i)
		{
			std::string	str = _response.substr(i, _response.find("\r\n", i) - i);
			if (str.find("Status: ") == 0)
				_code = std::atoi(str.substr(8, 3).c_str());
			else if (str.find("Content-Type: ") == 0)
				_type = str.substr(14, str.size());
			i += str.size() + 2;
		}
		while (_response.find("\r\n", j) == j)
			j -= 2;

		_response = _response.substr(i, j - i);
    }
    else {
        req.setCodeRet(200);
        _response = "";

    }
    if (req.getCodeRet() >= 500)
    {
        // * check which code
        _response = 
    }

    std::cout << "Post" << std::endl;
    std::string content_type = req.getHeaders()["Content-Type"];
    std::cout << content_type << std::endl;
    std::cout << req << std::endl;
    std::cout << "Post" << std::endl;
}


Post::~Post()
{
}