#include "request.hpp"

const std::string& Request::getMethod() const
{
    return this->_method;
}

const std::string& Request::getPath() const
{
    return this->_path;
}

const std::string& Request::getVersion() const
{
    return this->_version;
}

const std::map<std::string, std::string>& Request::getHeaders() const
{
    return this->_headers;
}

const std::map<std::string, std::string>& Request::getCgiEnv() const
{
    return this->cgi_env;
}

const std::string& Request::getBody() const
{
    return this->_body;
}

int Request::getReturnCode() const
{
    return this->_return_code;
}

int Request::getPort() const
{
    return this->_port;
}

const std::list<std::pair<std::string, float>>& Request::getLang() const
{
    return this->_lang;
}

const std::string& Request::getRaw() const
{
    return this->_raw;
}

const std::string& Request::getQuery() const
{
    return this->_query;
}
