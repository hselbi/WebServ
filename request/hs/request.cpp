#include "request.hpp"


Request::Request(const std::string& str) : _method(""), _version(""), _return_code(200), _body(""), _port(80), _path(""), _query("") , _raw(str)
{
    this->defaultHeaders();
    this->cgi_env.clear();
    this->parseRequest(str);
    if (this->_return_code != 200)
        std::cerr << "Error parsing request" << this->_return_code << std::endl;
}

Request::~Request()
{
}

Request& Request::operator=(const Request& other)
{
    this->_headers = other.getHeaders();
    this->_method = other.getMethod();
    this->_path = other.getPath();
    this->_version = other.getVersion();
    this->_body = other.getBody();
    this->_return_code = other.getReturnCode();
    this->_port = other.getPort();
    this->cgi_env = other.getCgiEnv();

    return *this;
}