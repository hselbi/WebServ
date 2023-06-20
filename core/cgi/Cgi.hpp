#pragma once

#include <iostream>
#include <string>
#include <map>
#include "Config.hpp"

// wrapper for cgi execution and environment variables
struct Cgi
{
	Cgi();
	~Cgi();

	void init_env_vars(Config &config);
	void exec();

private:
	std::map<std::string, std::string> _cgi_env;
	// std::string _extension;
	// std::string _input;
	// std::string _output;
};

/*
expecting from the request class:
- request body
- request body length
- request headers
- request method
- requested resource


*/

/*
CGI Environments Variables

CONTENT_LENGTH: Optionally provides the length, in bytes. It’s available only for POST requests.
C
QUERY_STRING: The query string, if any, that was contained in the request.
..


CGI error codes
bad gateway error code 502 on unsuccessful execution of CGI script. (unsupoorted gateway)

*/
