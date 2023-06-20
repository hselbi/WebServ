#include <iostream>
#include "../../includes/core/Server.hpp"
#include <signal.h>

int main(int ac, char *av[])
{
	signal(SIGPIPE, SIG_IGN);
	Server server;
	try
	{
		if (ac  <= 2)
		{
			server.load_config_file(av[1]);
			server.start(); // configuring and setuping the server socket, ...etc
		}
		else
		{
			std::cerr << "Usage: ./webserv [config_file]" << std::endl;
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		server.cleanup_by_closing_all_sockets(); 
	}
	
	return (0);
}

