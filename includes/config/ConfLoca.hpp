#ifndef CONFLOCA_HPP
#define CONFLOCA_HPP

#include "../includes.hpp"
#include "Tools.hpp"



class ConfLoca
{
public:

    int client_body_limit;
	std::string path;
	std::string root;
	std::vector<std::string> index;
	std::vector<MethodType> allow_methods;
    std::map<std::string, std::string> cgi_infos;

    ConfLoca();
    ~ConfLoca();
    static MethodType strtoMethod(std::string str);
    void print_loca_info();

};



#endif