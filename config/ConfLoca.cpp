#include "../includes/config/ConfLoca.hpp"

ConfLoca::ConfLoca()
{
	path = "";
	root = "";
    autoindex = DEFAULT;

}

ConfLoca::~ConfLoca()
{}

ConfLoca::ConfLoca(const ConfLoca &copy)
{
    std::cout << "ConfLoca copy constructor called\n";
    if (this != &copy)
    {
        this->path = copy.path;
        this->root = copy.root;
        this->index = copy.index;
        this->allow_methods = copy.allow_methods;
        this->client_body_limit = copy.client_body_limit;
        this->cgi_infos = copy.cgi_infos;
        this->autoindex = copy.autoindex;
        // std::cout << "@@@@@autoindex: " << (autoindex ? "on\n" : "off\n");
        this->error_pages = copy.error_pages;
        this->binary = copy.binary;
        
    }

}

ConfLoca &ConfLoca::operator=(const ConfLoca &copy)
{
    if (this != &copy)
    {
        this->path = copy.path;
        this->root = copy.root;
        this->index = copy.index;
        this->allow_methods = copy.allow_methods;
        this->client_body_limit = copy.client_body_limit;
        this->cgi_infos = copy.cgi_infos;
        this->autoindex = copy.autoindex;
        // std::cout << "autoindex: " << (autoindex ? "on\n" : "off\n");
        this->error_pages = copy.error_pages;
        this->binary = copy.binary;

    }
    return *this;
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
    std::cout << "\t\t> autoindex: " << (autoindex ? "on\n" : "off\n");
	std::cout << "\t\t> allow_methods: " << allow_methods << std::endl;
<<<<<<< HEAD
=======
<<<<<<< HEAD
    std::cout << "\t\t> client_body_limit: " << client_body_limit << std::endl;

    std::cout << "\t\t> cgi_infos: " << cgi_infos <<std::endl;
}
=======
>>>>>>> zakaria
	std::cout << "\t\t> client_body_limit: " << client_body_limit << std::endl;
	std::cout << "\t\t> cgi_infos: " << cgi_infos << std::endl;
}

std::string ConfLoca::getRoot()
{
	return root;
}

std::string ConfLoca::getPath()
{
	return path;
}
<<<<<<< HEAD
=======
>>>>>>> 2f7e2652cfdbf0f394e22c85abe4dcf151d8b61b
>>>>>>> zakaria
