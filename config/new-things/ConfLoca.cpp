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
	std::cout << "----------------- Location Info -----------------\n";
	std::cout << "> path: " << path << "\n";
	std::cout << "> root: " << root << "\n";
	std::cout << "> index: " << index << "\n";
	std::cout << "> allow_methods: " << allow_methods << std::endl;
}