#ifndef CONFSERVER_HPP
#define CONFSERVER_HPP

#include "../includes.hpp"
#include "ConfLoca.hpp"


class ConfServer
{
    private:
        std::string content;
    public:

        int     server_id;
        int		client_body_limit;
        bool	autoindex;
        std::string root;
        std::string server_name;
        std::vector<std::string> index;
        std::vector<MethodType> allow_methods;
        std::map<int, std::string> error_pages;
        std::map<std::string, std::string> cgi_infos;
        
        std::vector<ConfLoca> locations;
        
        int		redirect_status;
        std::string redirect_url;

        struct timeval send_timeout;
        struct timeval recv_timeout;

        std::string host;
	    std::string port;
	
	    int		listen_socket;

        ConfServer();
        ~ConfServer();
        int getServerId();
        std::string getServerName();
        std::string getHost();
        int getPort();
        std::string getRoot();
        std::vector<std::string> getIndex();
        std::vector<MethodType> getAllowMethods();
        std::map<int, std::string> getErrorPages();
        std::map<std::string, std::string> getCgiInfos();
        std::vector<ConfLoca> getLocations();
        int getClientBodyLimit();
        bool getAutoindex();
        int getRedirectStatus();
        std::string getRedirectUrl();
        

        static MethodType strtoMethod(std::string str);
        void print_server_info();


};

#endif