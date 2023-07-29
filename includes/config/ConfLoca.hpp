#ifndef CONFLOCA_HPP
#define CONFLOCA_HPP

#include "../includes.hpp"
#include "Tools.hpp"


enum e_autoindex
{
    OFF,
    ON,
    DEFAULT
};

class ConfLoca
{
    public:

        int client_body_limit;
        std::string path;
        std::string root;
        std::vector<std::string> index;
        std::vector<MethodType> allow_methods;
        std::map<std::string, std::string> cgi_infos;
        std::string binary;
        
        int autoindex;
        
        std::map<int, std::string> error_pages;
        // need to be done ...
        // error_pages;
        

    ConfLoca();
    ~ConfLoca();
	std::string getRoot();
	std::string getPath();
    ConfLoca(const ConfLoca &copy);
    static MethodType strtoMethod(std::string str);
    void print_loca_info();

};



#endif
