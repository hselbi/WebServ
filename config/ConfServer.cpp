#include "../includes/config/ConfServer.hpp"

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
	std::cout << "------------------ Server Info ------------------\n";
	std::cout << "> server_name: " << server_name << "\n";
	std::cout << "> host: " << host << "\n";
	std::cout << "> port: " << port << "\n";
	if (redirect_status == -1)
	{
		std::cout << "> root: " << root << "\n";
		std::cout << "> client_body_limit: " << client_body_limit << "\n";
		std::cout << "> autoindex: " << (autoindex ? "on\n" : "off\n");
		std::cout << "> index: " << index << "\n";
		std::cout << "> allow_methods: " << allow_methods << "\n";
		std::cout << "> error pages: ";
		if (error_pages.size() > 0)
			std::cout << "\n" << error_pages;
		else
			std::cout << "(empty)\n";
		for (unsigned long i = 0; i < locations.size(); i++)
			locations[i].print_loca_info();
	}
	else
	{
		std::cout << "> redirect_status: " << redirect_status << "\n";
		std::cout << "> redirect_url: " << redirect_url << "\n";
	}
	std::cout << "-------------------------------------------------\n";
}