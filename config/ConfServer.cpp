#include "../includes/config/ConfServer.hpp"
#include "../includes/includes.hpp"

ConfServer::ConfServer()
{
    client_body_limit = 1024;

	struct timeval tv;
	tv.tv_sec = 60;
	tv.tv_usec = 0;
	if (recv_timeout.tv_sec != 0) recv_timeout = tv;
	if (send_timeout.tv_sec != 0) send_timeout = tv;
	
	autoindex = false;
	host = "";
	port = "";
	redirect_status = -1;
	server_id = 0;
}

int ConfServer::get_server_id()
{
	return server_id;
}

void ConfServer::set_server_id(int id)
{
	server_id = id;
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
			std::cout << GREEN << "\n" << error_pages;
		else
			std::cout << GREEN << "(empty)\n";
		for (unsigned long i = 0; i < locations.size(); i++)
			locations[i].print_loca_info();
	}
	else
	{
		std::cout << YELLOW << "> redirect_status: " << GREEN << redirect_status << "\n";
		std::cout << YELLOW << "> redirect_url: " << GREEN << redirect_url << "\n";
	}
	std::cout << "-------------------------------------------------\n" << RESET;
}