#include "../../includes/core/Cgi.hpp"

int main(int ac, char const *av[])
{
	Cgi cgi;
	ConfServer configServer;
	ConfLoca configLocation;
	Request request;

	cgi.start_cgi(configServer, configLocation, request);

    return 0;
}

/*
	is_req_complet
	cgi bin and script
	throw_error
*/
