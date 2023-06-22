#ifndef POST_HPP
#define POST_HPP

#include "Request.hpp"

class Post
{
private:
    bool _chunked;
    bool _length;
    bool _type;
public:
    Post(Request &req);
    ~Post();
};



#endif