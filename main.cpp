#include "includes/includes.hpp"
#include "includes/request/Request.hpp"
#include "includes/config/Config.hpp"
#include "includes/config/Tools.hpp"



void test(std::vector<ConfServer> &servers)
{
    servers[0].set_server_id(15);
	std::vector<ConfServer>::iterator it = servers.begin();
	// std::vector<ConfServer>::iterator ite = servers.end();
	// int i = 0;
	(*it).print_server_info();
	// for (; it != ite; it++)
	// {
	// }
}


int main(int ac, char *av[])
{
    if (ac > 2)
    {
        std::cerr << "Error: too many arguments" << std::endl;
        exit(1);
    }

    std::cout << "=================================================\n";
	std::cout << "                 Webserv Start!                  \n";
	std::cout << "=================================================\n";

    std::string config = (ac == 1) ? "./config/conf/test.config" : av[1];

    Config configParser(config.c_str());
	std::vector<ConfServer> *servers = configParser.parser();
	test(*servers);

    // const char *test[3] = {"./request/tester_files/lvl3/html-form.txt", "./request/tester_files/lvl3/json-data.txt", "./request/tester_files/lvl3/xml-data.txt"};
    
    // std::cout << YELLOW << "****************** " << test[0] << " ******************" << RESET << std::endl;
    // Request r(test[0]);
    // std::cout << r << std::endl;
    

    // std::cout << YELLOW << "****************** " << test[1] << " ******************" << RESET << std::endl;
    // Request s(test[1]);
    // std::cout << s << std::endl;
    
    // std::cout << YELLOW << "****************** " << test[2] << " ******************" << RESET << std::endl;
    // Request q(test[2]);
    // std::cout << q << std::endl;


    std::cout << "=================================================\n";
	std::cout << "                Webserv Finished                 \n";
	std::cout << "=================================================\n";
    delete servers;
    return 0;
}