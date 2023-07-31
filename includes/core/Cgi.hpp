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

#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


struct Client;

// wrapper for cgi execution and environment variables
struct Cgi
{
	typedef std::map<std::string, std::string> env_vars_t;
	Cgi();
	~Cgi();
	std::string start_cgi(std::string script_path);
	std::string exec_cgi();

	void init_env_vars();
	void clean_env_vars();

	void set_body(std::string payload);
	std::string get_path_info(std::string url);

	void set_cgi_bin(std::string cgi_bin);
	void set_cgi_script(std::string cgi_script);

	std::string get_cgi_script_name(std::string path);

	void setClient(Client &client);
	void reset();

	int get_ready_to_read_from_cgi();
	void set_ready_to_read_from_cgi(int ready_to_read_from_cgi);

	int get_cgi_status();
	void set_cgi_status(int cgi_status);

	int get_pid();

	int get_time_out();
	void set_time_out(int time_out);

	int get_start_time();

	int get_status();
	void set_status(int status);

private:
	env_vars_t _env_vars;
	char **_envp;
	char **_argv;
	Client *_client;
	std::string _extension;
	std::string _body;
	std::string _cgi_bin;
	std::string _cgi_script;
	FILE *_cgi_output_file;
	int _ready_to_read_from_cgi;
	int _cgi_status;
	pid_t _pid;
	int _time_out;
	int _start_time;
		int _status;

};
