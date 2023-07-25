#include "../includes/config/Config.hpp"
#include "../includes/config/Tools.hpp"


void    test(std::vector<ConfServer> &servers)
{
    std::vector<ConfServer>::iterator it = servers.begin();
    std::vector<ConfServer>::iterator ite = servers.end();
    int i = 0;

        // std::cout << "ID: " << (*it).getServerId() << std::endl;
        // std::cout << "Name Server: " << (*it).getServerName() << std::endl;
        // std::cout << "Host: " << (*it).getHost() << std::endl;
        // std::cout << "Port: " << (*it).getPort() << std::endl;
        // std::cout << "Root: " << (*it).getRoot() << std::endl;
		std::cout << (*it).getLocations()[0].getPath() << std::endl;

}
int main(int argc, char **argv)
{
	if (argc > 2)
		return 0;

	std::cout << "=================================================\n";
	std::cout << "                 Webserv Start!                  \n";
	std::cout << "=================================================\n";

	std::string config = (argc == 1) ? "./conf/hafid.config" : argv[1];
	Config configParser;
	std::vector<ConfServer> servers = configParser.parser(config.c_str());

	test(servers);


	std::cout << "=================================================\n";
	std::cout << "                Webserv Finished                 \n";
	std::cout << "=================================================\n";
	return 0;
}
