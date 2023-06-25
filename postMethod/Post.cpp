#include "Post.hpp"


Post::Post(Request &req)
{
    std::string _response; /*this one from response */

    size_t i = 0;
    size_t j = _response.size() - 1;
    

    // ! 1st etap: check is cgi exist or not

    if (/* ^ cgi exist */)
    {
        while (_response.find("\r\n\r\n", i) != std::string::npos || _response.find("\r\n", i) == i)
		{
			std::string	str = _response.substr(i, _response.find("\r\n", i) - i);
			if (str.find("Status: ") == 0)
				_code = std::atoi(str.substr(8, 3).c_str());
			else if (str.find("Content-Type: ") == 0)
				_type = str.substr(14, str.size());
			i += str.size() + 2;
		}
		while (_response.find("\r\n", j) == j)
			j -= 2;

		_response = _response.substr(i, j - i);
    }
    else {
        req.setCodeRet(200);
        _response = "";

    }
    if (req.getCodeRet() >= 500)
    {
        // * check which code
        _response = 
    }

    std::cout << "Post" << std::endl;
    std::string content_type = req.getHeaders()["Content-Type"];
    std::cout << content_type << std::endl;
    std::cout << req << std::endl;
    std::cout << "Post" << std::endl;
}


Post::~Post()
{
}