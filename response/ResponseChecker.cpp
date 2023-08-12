#include "../includes/response/Response.hpp"
#include "../includes/core/Client.hpp"

std::string Response::isDirHasIndexFiles()
{
    std::string dirPath = getRequestPathFile();
	std::vector<std::string> indexFiles;

	indexFiles = (_location->index.size() > 0) ? _location->index : _client->get_server_block().getIndex();

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
		responseHeader.m_headers["Location"] = _client->get_server_block().getRedirectUrl();
		responseHeader.m_headers["Server"] = _client->get_server_block().getServerName();

		_header_buffer = Utils::ResponseHeaderToString(responseHeader);
		_client->append_response_data(_header_buffer);
		setResStatus(DONE);
		return true;
	}
	else if (redirectStatus != -1)
	{
		responseHeader.statusCode = redirectStatus;
		responseHeader.statusMessage = Utils::getStatusMessage(redirectStatus);
		responseHeader.m_headers["Content-Type"] = "application/octet-stream";
		responseHeader.m_headers["Content-Length"] = "0";
		responseHeader.m_headers["Server"] = _client->get_server_block().getServerName();

		_header_buffer = Utils::ResponseHeaderToString(responseHeader);
		_client->append_response_data(_header_buffer);
		setResStatus(DONE);
		return true;
	}

	return false;
}

bool Response::isLocationHaveRedirection()
{
	t_responseHeader	responseHeader;
	int 				redirectStatus;
	if (_location && _location->redirect_status != -1)
	{
		redirectStatus = _location->getRedirectStatus();
		if (redirectStatus == 301 || redirectStatus == 302 || redirectStatus == 307)
		{
			responseHeader.statusCode = redirectStatus;
			responseHeader.statusMessage = Utils::getStatusMessage(redirectStatus);
			responseHeader.m_headers["Location"] = _location->getRedirectUrl();
			responseHeader.m_headers["Server"] = _client->get_server_block().getServerName();

			_header_buffer = Utils::ResponseHeaderToString(responseHeader);
			_client->append_response_data(_header_buffer);
			setResStatus(DONE);
			return true;
		}
		else if (redirectStatus != -1)
		{
			responseHeader.statusCode = redirectStatus;
			responseHeader.statusMessage = Utils::getStatusMessage(redirectStatus);
			responseHeader.m_headers["Content-Type"] = "application/octet-stream";
			responseHeader.m_headers["Content-Length"] = "0";
			responseHeader.m_headers["Server"] = _client->get_server_block().getServerName();

			_header_buffer = Utils::ResponseHeaderToString(responseHeader);
			_client->append_response_data(_header_buffer);
			setResStatus(DONE);
			return true;
		}
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
			errorPages(405);
			return false;
		}
	}
	return false;
}

bool Response::checkStatusCgi()
{
	if (_location && _location->getCgiStatus() &&  _location->cgi_infos.size() > 0)
		return true;
	
	return false;
}