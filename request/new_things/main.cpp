#include "includes.hpp"
#include "Request.hpp"

int main(int ac, char *av[])
{
    if (ac != 2)
        std::cout << "really man!! without a file" << std::endl;
    std::string line;
    std::ifstream req_file;
    req_file.open(av[1], std::ios::in);
	if (!req_file) {
		std::cout << "No such file" << std::endl;
        std::exit(0);
	}

    std::getline(req_file, line);

    req_file.close();
    Request r(line);
    std::cout << r << std::endl;

    return 0;
}