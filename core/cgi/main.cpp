#include "../../includes/core/Cgi.hpp"

int main(int ac, char const *av[])
{
	Cgi cgi;


	cgi.start_cgi();

    return 0;
}

/*
	cgi.set_cgi_bin("/usr/bin/php-cgi");
	cgi.set_cgi_script("/home/user/42/webserv/www/index.php");
*/
