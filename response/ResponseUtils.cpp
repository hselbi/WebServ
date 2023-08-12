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
	else if (!req["Transfer-Encoding"].empty() && !req["Content-Length"].empty())
	{
		errorPages(400);
		return false;
	}
	else if (req["Transfer-Encoding"] == "chunked" && _client->get_request().getBodyFlag() == REQUEST_BODY_COMPLETED)
		chunkedResponse();
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
	_client->get_request().setPath(Utils::uriDecode(_client->get_request().getPath()));
	return true;
}

void Response::chunkedResponse()
{
	t_responseHeader responseHeader;
	responseHeader.statusCode = 200;
	responseHeader.statusMessage = Utils::getStatusMessage(200);
	responseHeader.m_headers["Content-Type"] = "application/html";
	responseHeader.m_headers["Content-Length"] = "0";
	responseHeader.m_headers["Server"] = _client->get_server_block().getServerName();

	_header_buffer = Utils::ResponseHeaderToString(responseHeader);
	_client->append_response_data(_header_buffer);
	setResStatus(DONE);
}

void Response::checkWhichRequestedMethod()
{
	std::string method = _client->get_request().getMethod();
	if (method == "GET")
		Method_GET();
	else if (method == "DELETE")
		Method_DELETE();
	else if (method == "POST")
	{	
		std::string content_length = _client->get_request().getHeaders()["Content-Length"];
		if (!content_length.empty() && _location->getClientBodyLimit() != -1)
		{
			long content_length = atol(_client->get_request().getHeaders()["Content-Length"].c_str());
			if (content_length == 0)
			{
				errorPages(411);
				return ;
			}
			else if (content_length > _location->getClientBodyLimit())
			{
				errorPages(413);
				return ;
			}
			
		}
		Method_POST();
	}
}

void Response::sendRediraction(std::string location)
{
	t_responseHeader responseHeader;
	responseHeader.statusCode = 301;
	responseHeader.statusMessage = Utils::getStatusMessage(301);
	responseHeader.m_headers["Location"] = location;
	responseHeader.m_headers["Server"] = _client->get_server_block().getServerName();

	_header_buffer = Utils::ResponseHeaderToString(responseHeader);
	_client->append_response_data(_header_buffer);
	setResStatus(DONE);
}

void	Response::deleteAllFolderFiles()
{
	std::string path = getRequestPathFile();
	DIR *dir;
	struct dirent *ent;
	std::string filePath;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			filePath = path + ent->d_name;
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
	std::string filePath = getRequestPathFile();

	if (remove(filePath.c_str()) != 0)
		errorPages(500);
	else
	{
		t_responseHeader responseHeader;
		responseHeader.statusCode = 204;
		responseHeader.statusMessage = Utils::getStatusMessage(204);
		responseHeader.m_headers["Content-Length"] = "0";
		responseHeader.m_headers["Server"] = _client->get_server_block().getServerName();
		_header_buffer = Utils::ResponseHeaderToString(responseHeader);
		_client->append_response_data(_header_buffer);
		setResStatus(DONE);
	}
}


void Response::autoIndex()
{
	DIR *dir;
	struct dirent *ent;
	std::string strHeader, path, body, tmp;
	t_responseHeader responseHeader;

	path = getRequestPathFile();
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
	responseHeader.m_headers["Content-Type"] = "text/html";
	responseHeader.m_headers["Content-Length"] = Utils::toString(body.length());
	responseHeader.m_headers["Server"] = _client->get_server_block().getServerName();

	strHeader = Utils::ResponseHeaderToString(responseHeader);
	_client->append_response_data(strHeader);
	_client->append_response_data(body);
	setResStatus(DONE);
}


std::string		Response::startCgi(std::string script_path)
{

	std::map<std::string, std::string> cgi_infos = _location->cgi_infos;
	std::string extension = Utils::getExtensionFile(script_path);
	std::string cgi_path = "";
	for (std::map<std::string, std::string>::iterator it = cgi_infos.begin(); it != cgi_infos.end(); it++)
	{
		if (it->first == extension)
			cgi_path = getCorrectPath(it->second);
	}

	if (cgi_path == "" || !Utils::fileExists(cgi_path) || Utils::isDirectory(cgi_path)  ||  !Utils::isExecutable(cgi_path))
	{
		_have_cgi = false;
		return "-1";
	}

	return _client->get_cgi().start_cgi(cgi_path, script_path);
}