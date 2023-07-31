#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

bool Response::checkRequestIsFormed()
{
	std::map<std::string, std::string> req = _client->get_request().getHeaders();

	if (!req["Transfer-Encoding"].empty() && req["Transfer-Encoding"] != "chunked")
	{
		errorPages(501);
		return false;
	}
	else if (req["Transfer-Encoding"].empty() && req["Content-Length"].empty()
		&& _client->get_request().getMethod() == "POST")
	{
		errorPages(400);
		return false;
	}
	else if (!Utils::isValidURI(_client->get_request().getPath()))
	{
		errorPages(400);
		return false;
	}
	else if (_client->get_request().getPath().length() > 2048)
	{
		errorPages(414);
		return false;
	}
	return true;
}

void Response::checkWhichRequestedMethod()
{
	std::string method = _client->get_request().getMethod();
	if (method == "GET")
		Method_GET();
	else if (method == "DELETE")
		Method_DELETE();
	else if (method == "POST")
		Method_GET();
}

void Response::setRediration(std::string location)
{
	t_responseHeader responseHeader;
	responseHeader.statusCode = 301;
	responseHeader.statusMessage = Utils::getStatusMessage(301);
	responseHeader.headers["Location"] = location;
	responseHeader.headers["Server"] = _client->get_server_block().getServerName();

	_header_buffer = Utils::ResponseHeaderToString(responseHeader);
	_client->append_response_data(_header_buffer);
	setResStatus(DONE);
}

void	Response::deleteAllFolderFiles()
{
	std::string path = getRoot() + _client->get_request().getPath();
	DIR *dir;
	struct dirent *ent;
	std::string filePath;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			filePath = path + ent->d_name;
			// std::cout << "filePath: " << filePath << std::endl;
			if (remove(filePath.c_str()) != 0)
				errorPages(500);
		}
		closedir(dir);
	}
	else
		errorPages(403);
	deleteFile();
}

void	Response::deleteFile()
{
	std::string filePath = getRoot() + _client->get_request().getPath();

	if (remove(filePath.c_str()) != 0)
		errorPages(500);
	else
	{
		t_responseHeader responseHeader;
		responseHeader.statusCode = 204;
		responseHeader.statusMessage = Utils::getStatusMessage(204);
		responseHeader.headers["Content-Length"] = "0";
		responseHeader.headers["Server"] = _client->get_server_block().getServerName();
		_header_buffer = Utils::ResponseHeaderToString(responseHeader);
		setResStatus(ON_PROCESS);
	}
}


void Response::autoIndex()
{
	DIR *dir;
	struct dirent *ent;
	std::string strHeader, path, body, tmp;
	t_responseHeader responseHeader;

	path = getRoot() + _client->get_request().getPath();
	body = "<html><head><title>Index of " + _client->get_request().getPath() + "</title></head><body><h1>Index of " + _client->get_request().getPath() + "</h1><hr><pre>";
	if ((dir = opendir(path.c_str())) != NULL)
	{
		body.append("<a href=\"./\">./</a><br>");
		body.append("<a href=\"../\">../</a><br>");
		while ((ent = readdir(dir)) != NULL)
		{
			tmp = ent->d_name;
			if (Utils::isDirectory(path + "/" + tmp) && tmp != "." && tmp != "..")
				body.append("<a href=\"" + tmp + "/\">" + tmp + "/</a><br>");
			else if (tmp != "." && tmp != "..")
				body.append("<a href=\"" + tmp + "\">" + tmp + "</a><br>");
		}
		body.append("</pre><hr></body></html>");
		closedir(dir);
	}
	responseHeader.statusCode = 200;
	responseHeader.statusMessage = Utils::getStatusMessage(200);
	responseHeader.headers["Content-Type"] = "text/html";
	responseHeader.headers["Content-Length"] = Utils::toString(body.length());
	responseHeader.headers["Server"] = _client->get_server_block().getServerName();

	strHeader = Utils::ResponseHeaderToString(responseHeader);
	_client->append_response_data(strHeader);
	_client->append_response_data(body);
	setResStatus(DONE);
}
