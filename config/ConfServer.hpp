#ifndef CONFSERVER_HPP
#define CONFSERVER_HPP

#include "includes.hpp"
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
        void set_server_id(int id);
        int get_server_id();
        static MethodType strtoMethod(std::string str);
        void print_server_info();


};

#endif