#include "ConfLoca.hpp"

ConfLoca::ConfLoca()
{
    path = "";
	root = "";
}

ConfLoca::~ConfLoca()
{
}


MethodType ConfLoca::strtoMethod(std::string str)
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

void ConfLoca::print_loca_info()
{
	std::cout << "\t\t----------------- Location Info -----------------\n";
	std::cout << "\t\t> path: " << path << "\n";
	std::cout << "\t\t> root: " << root << "\n";
	std::cout << "\t\t> index: " << index << "\n";
	std::cout << "\t\t> allow_methods: " << allow_methods << std::endl;
    std::cout << "\t\t> client_body_limit: " << client_body_limit << std::endl;
    std::cout << "\t\t> cgi_infos: " << cgi_infos <<std::endl;
}