#include "Config.hpp"
#include "Tools.hpp"

int main(int argc, char **argv)
{
	if (argc > 2)
		return 0;

	std::cout << "=================================================\n";
	std::cout << "                 Webserv Start!                  \n";
	std::cout << "=================================================\n";

	std::string config = (argc == 1) ? "./conf/test.config" : argv[1];
	Config configParser(config.c_str());
	std::vector<ConfServer> *servers = configParser.parser();
	std::cout << "this is size of servers: " << servers->size() << std::endl;
	// std::cout << "====================" << i << "====================\n";
	std::vector<ConfServer>::iterator it = servers[0].begin();
	std::vector<ConfServer>::iterator ite = servers[0].end();
	int i = 0;
	for (; it != ite; it++)
	{
		(*it).print_server_info();
	}
	


	std::cout << "=================================================\n";
	std::cout << "                Webserv Finished                 \n";
	std::cout << "=================================================\n";
	delete servers;
	return 0;
}