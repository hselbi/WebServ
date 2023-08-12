#include "includes/includes.hpp"
#include "includes/request/Request.hpp"
#include "includes/config/Config.hpp"
#include "includes/config/Tools.hpp"
#include "includes/response/Response.hpp"
#include "includes/core/Server.hpp"



int main(int ac, char *av[])
{
	signal(SIGPIPE, SIG_IGN);
	Server server;
	std::string config = (ac == 1) ? "./config/conf/defaults.config" : av[1];
	try
	{
		if (ac  <= 2)
		{
			server.load_config_file(config.c_str());
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

