#include "request.hpp"

std::vector<std::string> Request::initMethods()
{
    std::vector<std::string> methods;

    methods.push_back("GET");
    methods.push_back("POST");
    methods.push_back("DELETE");

    return methods;
}

std::vector<std::string> Request::_methods = Request::initMethods();

std::ostream& operator<<(std::ostream& os, const Request& re)
{
    std::map<std::string, std::string>::const_iterator it;

    os << "Method: " << re.getMethod() << std::endl;
    os << "Version: " << re.getVersion() << std::endl;
    os << "Port: " << re.getPort() << std::endl;
    os << "Path: " << re.getPath() << std::endl;
    
    for (it = re.getHeaders().begin(); it != re.getHeaders().begin(); it++)
        os << it->first << ": " << it->second << std::endl;
    
    os << "Request Body: " << re.getBody() << std::endl;

    return os;
}

void Request::displayRequest() const
{
    std::map<std::string, std::string>::const_iterator it;

    std::cout << "Method: " << this->_method << std::endl;
    std::cout << "Version: " << this->_version << std::endl;
    std::cout << "Port: " << this->_port << std::endl;
    std::cout << "Path: " << this->_path << std::endl;
    
    for (it = this->_headers.begin(); it != this->_headers.end(); it++)
        std::cout << it->first << ": " << it->second << std::endl;
    
    std::cout << "Request Body: " << this->_body << std::endl;
}

void Request::defaultHeaders()
{
    this->_headers["Accept-Charset"] = ""; // Accept-Charset: iso-8859-5 
    this->_headers["Accept-Language"] = ""; // Accept-Language: en, mi
    this->_headers["Allow"] = ""; // Allow: GET, HEAD
    this->_headers["Auth-Scheme"] = ""; // Auth-Scheme: Basic
    this->_headers["Autho"]


}