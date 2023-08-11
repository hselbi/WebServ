#include "../includes/config/ConfLoca.hpp"

ConfLoca::ConfLoca()
{
    path = "";
    root = "";
    autoindex = DEFAULT;
	redirect_status = -1;
	client_body_limit = -1;
    cgi_status = true;
    upload_status = true;

}

ConfLoca::~ConfLoca()
{
}

ConfLoca::ConfLoca(const ConfLoca &copy)
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
        cgi_status = copy.cgi_status;
        upload_status = copy.upload_status;
        this->redirect_status = copy.redirect_status;
        this->redirect_url = copy.redirect_url;
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
        cgi_status = copy.cgi_status;
        upload_status = copy.upload_status;
        this->autoindex = copy.autoindex;
        this->redirect_status = copy.redirect_status;
        this->redirect_url = copy.redirect_url;
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
    std::cout << RED << "\t\t----------------- Location Info -----------------\n" << GREEN;
    std::cout << "\t\t> path: " << YELLOW << path << "\n"<< GREEN;
    std::cout << "\t\t> root: " << YELLOW << root << "\n"<< GREEN;
    std::cout << "\t\t> index: " << YELLOW << index << "\n"<< GREEN;
    std::cout << "\t\t> autoindex: " << YELLOW << (autoindex ? "on\n" : "off\n")<< GREEN;
    std::cout << "\t\t> allow_methods: " << YELLOW << allow_methods << std::endl<< GREEN;
    std::cout << "\t\t> client_body_limit: " << YELLOW << client_body_limit << std::endl<< GREEN;
    std::cout << "\t\t> redirect_status: " << YELLOW << redirect_status << std::endl<< GREEN;
    std::cout << "\t\t> redirect_url: " << YELLOW << redirect_url << std::endl<< GREEN;
    std::cout << "\t\t> cgi_infos: " << YELLOW << cgi_infos << std::endl<< GREEN;
}

std::string ConfLoca::getRoot()
{
    return root;
}

std::string ConfLoca::getPath()
{
    return path;
}


int ConfLoca::getClientBodyLimit()
{
	return (client_body_limit < 0) ? -1 : client_body_limit;
}

int ConfLoca::getRedirectStatus()
{
	return (redirect_status < 0) ? -1 : redirect_status;
}

std::string ConfLoca::getRedirectUrl()
{
	return redirect_url;
}


bool ConfLoca::getCgiStatus()
{
    return cgi_status;
}

bool ConfLoca::getUploadStatus()
{
    return upload_status;
}