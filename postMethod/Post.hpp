#ifndef POST_HPP
#define POST_HPP

#include "../includes/request/Request.hpp"

class Post
{
private:
    std::string _type;
    // bool _chunked;
    // bool _length;
    // bool _type;
public:
    Post(Request &req);
    std::map<std::string, std::string> splitter(std::string body);
    ~Post();
};



#endif


// # to do list
// split with delimiter and return a map