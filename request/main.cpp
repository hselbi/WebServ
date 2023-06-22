#include "includes.hpp"
#include "Request.hpp"
#include "Post.hpp"
int main(int ac, char *av[])
{
    if (ac != 2)
    {
        std::cout << "really man!! without a file" << std::endl;
        exit(1);
    }
  
    Request r(av[1]);
    std::cout << r << std::endl;
    Post p(r);


    return 0;
}