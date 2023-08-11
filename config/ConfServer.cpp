#include "../includes/config/ConfServer.hpp"
#include "../includes/includes.hpp"

ConfServer::ConfServer()
{
	client_body_limit = -1;

	struct timeval tv;
	tv.tv_sec = 60;
	tv.tv_usec = 0;
	if (recv_timeout.tv_sec != 0)
		recv_timeout = tv;
	if (send_timeout.tv_sec != 0)
		send_timeout = tv;

	autoindex = false;
	host = "127.0.0.1";
	port = "8080";
	redirect_status = -1;
	server_id = 0;
}

int ConfServer::getServerId()
{
	return server_id;
}

std::string ConfServer::getServerName()
{
	return (server_name != "" ? server_name : host + ":" + port);
}

std::string ConfServer::getHost()
{
	return host;
}

int ConfServer::getPort()
{
	return (atoi(port.c_str()));
}

std::string ConfServer::getRoot()
{
	return root;
}

std::vector<std::string> ConfServer::getIndex()
{
	return index;
}

std::vector<MethodType> ConfServer::getAllowMethods()
{
	return allow_methods;
}

std::map<int, std::string> ConfServer::getErrorPages()
{
	return error_pages;
}

std::map<std::string, std::string> ConfServer::getCgiInfos()
{
	return cgi_infos;
}

std::vector<ConfLoca> ConfServer::getLocations()
{
	return locations;
}

int ConfServer::getClientBodyLimit()
{
	return client_body_limit;
}

bool ConfServer::getAutoindex()
{
	return autoindex;
}

int ConfServer::getRedirectStatus()
{
	return redirect_status;
}

std::string ConfServer::getRedirectUrl()
{
	return redirect_url;
}

ConfServer::~ConfServer()
{
}

MethodType ConfServer::strtoMethod(std::string str)
{
	if (str == "GET")
		return GET;
	else if (str == "POST")
		return POST;
	else if (str == "DELETE")
		return DELETE;
	else
		return INVALID;
}

void ConfServer::print_server_info()
{
	std::cout << RED << "------------------ Server Info ------------------\n";
	std::cout << YELLOW << "> ID: " << GREEN << this->server_id << "\n";
	std::cout << YELLOW << "> server_name: " << GREEN << server_name << "\n";
	std::cout << YELLOW << "> host: " << GREEN << host << "\n";
	std::cout << YELLOW << "> port: " << GREEN << port << "\n";
	if (redirect_status == -1)
	{
		std::cout << YELLOW << "> root: " << GREEN << root << "\n";
		std::cout << YELLOW << "> client_body_limit: " << GREEN << client_body_limit << "\n";
		std::cout << YELLOW << "> autoindex: " << GREEN << (autoindex ? "on\n" : "off\n");
		std::cout << YELLOW << "> index: " << GREEN << index << "\n";
		std::cout << YELLOW << "> allow_methods: " << GREEN << allow_methods << "\n";
		std::cout << YELLOW << "> error pages: ";
		if (error_pages.size() > 0)
			std::cout << GREEN << "\n"
					  << error_pages;
		else
			std::cout << GREEN << "(empty)\n";
	}
	else
	{
		std::cout << YELLOW << "> redirect_status: " << GREEN << redirect_status << "\n";
		std::cout << YELLOW << "> redirect_url: " << GREEN << redirect_url << "\n";
	}
	std::cout << "-------------------------------------------------\n"
			  << RESET;
}
