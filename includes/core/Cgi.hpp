#pragma once

#include <map>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#include "../../includes/config/Config.hpp"
#include "../../includes/config/ConfServer.hpp"
#include "../../includes/config/ConfLoca.hpp"
#include "../../includes/request/Request.hpp"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/types.h>

struct Client;

struct Cgi
{
	typedef std::map<std::string, std::string> env_vars_t;
	Cgi();
	~Cgi();
	std::string start_cgi(std::string binary, std::string script_path);
	std::string exec_cgi();
	void init_env_vars();
	void clean_env_vars();
	void set_body(std::string payload);
	void set_cgi_bin(std::string cgi_bin);
	void set_cgi_script(std::string cgi_script);
	void setClient(Client &client);
	void reset();
	int get_ready_to_read_from_cgi();
	void set_ready_to_read_from_cgi(int ready_to_read_from_cgi);
	int get_cgi_status();
	void set_cgi_status(int cgi_status);
	int get_pid();
	int get_status();
	void set_status(int status);
	static size_t _counter;
	std::string generate_cgi_output_file_name();

private:
	Client *_client;
	env_vars_t _env_vars;
	char **_envp;
	char **_argv;
	std::string _body;
	std::string _cgi_bin;
	std::string _cgi_script;
	int _ready_to_read_from_cgi;
	int _cgi_status;
	pid_t _pid;
	int _status;

};
