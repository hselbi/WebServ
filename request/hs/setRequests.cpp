#include "request.hpp"

void Request::setMethod(const std::string& method)
{
    this->_method = method;
}

void Request::setBody(const std::string& body)
{
    char strip[] = {'\r', '\n'};
    
    this->_body.assign(body);
    for (int i = 0; i < 4; i++)
    {
        if(this->_body.size() > 0 && this->_body[this->_body.size() - 1] == strip[i])
            this->_body.pop_back();
        else
            break;
    }
    
}

void Request::setReturnCode(int return_code)
{
    this->_return_code = return_code;
}