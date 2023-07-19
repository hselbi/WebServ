#pragma once

#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#include "../../includes/config/Config.hpp"
#include "../../includes/config/ConfServer.hpp"
#include "../../includes/config/ConfLoca.hpp"
#include "../../includes/request/Request.hpp"

// wrapper for cgi execution and environment variables
struct Cgi
{
	typedef std::map<std::string, std::string> env_vars_t;
	Cgi();
	~Cgi();

	void start_cgi(ConfServer &configServer, ConfLoca configLocation, Request &request);
	void exec_cgi();

	void init_env_vars(ConfServer &configServer, ConfLoca configLocation, Request &request);
	void clean_env_vars();

	void set_body(std::string payload);
	std::string get_path_info(std::string url);

	void set_cgi_bin(

private:
	env_vars_t _env_vars;
	char **_envp;
	char **_argv;
	std::string _extension;
	std::string _body;
	std::string _output;
	std::string _cgi_bin;
	std::string _cgi_script;
	FILE *_cgi_output_file;
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
CONTENT_TYPE: Optionally provides the sort of content i.e. the data type of the content.
HTTP_COOKIE: come back the visitor’s cookies, if one is ready within the type of key try.
PATH_INFO: It provides the trail for the CGI script.
REMOTE_HOST: The hostname of the visitor, i.e. the totally qualified name of the host creating the request
REQUEST_METHOD: The method used for the request. It’s either GET or POST.
SCRIPT_FILENAME: The absolute path of the CGI script.
SCRIPT_NAME: The name of the CGI script.
PATH_INFO: The path of the CGI script.
QUERY_STRING: The query string, if any, that was contained in the request.



CGI error codes
bad gateway error code 502 on unsuccessful execution of CGI script. (unsupoorted gateway)
bad gateway error code 502
*/

/*
	child process run script, output to pipe, parent process read from pipe and store in a string
	get the request payload and store it in a string _payload and pass it to the script, so he can read it from stdin and use it

	if (post)
		{
			payload = request.get_body();
			_env["CONTENT_TYPE"] = request.get_content_type();
			_env["CONTENT_LENGTH"] = payload.length();
		}
		else if (get)
		{
			_env["QUERY_STRING"] = request.get_query_string();
			}
*/
