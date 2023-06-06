#include "Config.hpp"

int main(int argc, char **argv)
{
	if (argc > 2)
		return 0;

	std::cout << "=================================================\n";
	std::cout << "                 Webserv Start!                  \n";
	std::cout << "=================================================\n";

	std::string config = (argc == 1) ? "./test.config" : argv[1];
	Config configParser(config.c_str());
	std::vector<ConfServer> *servers = configParser.parser();
    // std::vector<ConfServer>::iterator it;
    // for (it = servers->begin(); it != servers->end(); it++)
    // {
    //     std::cout << *it << std::endl;
    // }
	


	std::cout << "=================================================\n";
	std::cout << "                Webserv Finished                 \n";
	std::cout << "=================================================\n";
	delete servers;
	return 0;
}