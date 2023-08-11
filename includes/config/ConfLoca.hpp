#ifndef CONFLOCA_HPP
#define CONFLOCA_HPP

#include "../includes.hpp"
#include "Tools.hpp"


enum e_autoindex
{
    DEFAULT,
    ON,
    OFF
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

        e_autoindex autoindex;
        std::map<int, std::string> error_pages;
        
        int         redirect_status;
        bool        cgi_status;
        bool        upload_status;
        std::string redirect_url;


        ConfLoca();
        ~ConfLoca();
		std::string getRoot();
		std::string getPath();
        ConfLoca(const ConfLoca &copy);
        ConfLoca &operator=(const ConfLoca &copy);
        static MethodType strtoMethod(std::string str);
        void print_loca_info();
        int getClientBodyLimit();
        int getRedirectStatus();
        std::string getRedirectUrl();
        bool getCgiStatus();
        bool getUploadStatus();

};



#endif
