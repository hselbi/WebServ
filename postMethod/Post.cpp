#include "Post.hpp"


Post::Post(Request &req)
{
    std::cout << "Post" << std::endl;
    std::string content_type = req.getHeaders()["Content-Type"];
    std::cout << content_type << std::endl;
    std::cout << req << std::endl;
    std::cout << "Post" << std::endl;
}


Post::~Post()
{
}