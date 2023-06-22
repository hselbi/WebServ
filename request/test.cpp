#include "../includes/request/Request.hpp"
#include "../includes/includes.hpp"

int main(int ac, char *av[])
{
    if (ac != 2)
    {
        std::cout << "really man!! without a file" << std::endl;
        exit(1);
    }
  
    Request r(av[1]);
    std::cout << r << std::endl;

    return 0;
}