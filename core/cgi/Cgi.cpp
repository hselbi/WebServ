#include "../../includes/core/Cgi.hpp"

Cgi::Cgi()
{
}

Cgi::~Cgi()
{
}

void Cgi::start_cgi(ConfServer &configServer, ConfLoca configLocation, Request &request)
{
	init_env_vars(configServer, configLocation, request);
	// init_env_vars();
	exec_cgi();
}

void Cgi::init_env_vars(ConfServer &configServer, ConfLoca configLocation, Request &request)
{
	_env_vars["SERVER_SOFTWARE"] = "MortalKOMBAT/1.0";
	_env_vars["SERVER_NAME"] = configServer.getServerName();
	_env_vars["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env_vars["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env_vars["SERVER_PORT"] = configServer.getPort();
	_env_vars["REQUEST_METHOD"] = request.getMethod();
	_env_vars["REQUEST_URI"] = request.getPath();
	if (configLocation.getRoot() != "")
		_env_vars["DOCUMENT_ROOT"] = configLocation.getRoot();
	else
		_env_vars["DOCUMENT_ROOT"] = configServer.getRoot();

	_env_vars["SCRIPT_NAME"] = "index.php"; // !! configLocation.getScriptName();
	_env_vars["SCRIPT_FILENAME"] = _env_vars["DOCUMENT_ROOT"] + "/" + _env_vars["SCRIPT_NAME"];
	_env_vars["QUERY_STRING"] = get_query_string(request.getPath());
	_env_vars["PATH_INFO"] = get_path_info(request.getPath());

	_envp = new char *[_env_vars.size() + 1];
	int i = 0;
	for (env_vars_t::iterator it = _env_vars.begin(); it != _env_vars.end(); ++it, ++i)
	{
		std::string env_var = it->first + "=" + it->second;
		_envp[i] = new char[env_var.length() + 1];
		strcpy(_envp[i], env_var.c_str());
	}
	_envp[i] = NULL;

	_argv = new char *[3];
	_argv[0] = new char[_env_vars["CGI_PATH"].length() + 1];
	_argv[1] = new char[_env_vars["SCRIPT_NAME"].length() + 1];
	strcpy(_argv[0], _env_vars["CGI_PATH"].c_str());
	strcpy(_argv[1], _env_vars["SCRIPT_NAME"].c_str());
	_argv[2] = NULL;
}

void Cgi::clean_env_vars()
{
	for (int i = 0; _envp[i]; ++i)
		delete[] _envp[i];
	delete[] _envp;

	for (int i = 0; _argv[i]; ++i)
		delete[] _argv[i];
	delete[] _argv;

	_env_vars.clear();
}

std::string get_query_string(std::string url) // get query params from url
{
	std::string query_string;
	size_t pos = url.find('?');
	if (pos != std::string::npos)
		query_string = url.substr(pos + 1);
	return query_string;
}
// example.com/index.php/parameter1/parameter2
// get what is after index.php
std::string Cgi::get_path_info(std::string url)
{
	std::string path_info;
	long len = _env_vars["SCRIPT_NAME"].length();
	size_t pos = url.find(_env_vars["SCRIPT_NAME"]);
	if (pos != std::string::npos)
		path_info = url.substr(pos + len);
	return path_info;
}

void Cgi::exec_cgi() // !! upload handiinng
{
	pid_t pid;
	int write_to_cgi[2];

	_cgi_output_file = tmpfile();
	if (!_cgi_output_file)
	{
		std::cerr << "tmpfile failed" << std::endl;
		exit(1);
	}

	pipe(write_to_cgi);

	std::string data = "hello lopez22"; // request body // _body.c_str();
	set_body(data);
	write(write_to_cgi[1], _body.c_str(), _body.length()); // write to pipe

	if ((pid = fork()) == -1)
	{
		std::cerr << "fork failed" << std::endl;
		exit(1);
	}
	if (pid == 0)
	{
		close(write_to_cgi[1]);

		dup2(write_to_cgi[0], STDIN_FILENO); // read from pipe
		dup2(fileno(_cgi_output_file), STDOUT_FILENO);

		close(write_to_cgi[0]);

		if (execve("/usr/bin/php", _argv, _envp) == -1)
		{
			std::cerr << "execve failed" << std::endl;
			exit(1);
		}
	}
	else
	{
		close(write_to_cgi[0]);
		close(write_to_cgi[1]);

		// DONT WAIT FOR CHILD PROCESS TO FINISH
		waitpid(pid, NULL, WNOHANG); //

		lseek(_cgi_output_file->_fileno, 0, SEEK_SET);

		char buf[4024];
		read(fileno(_cgi_output_file), buf, 100);

		printf("data from CGI: \n ------------------ \n");
		printf("%s\n", buf);

		close(_cgi_output_file->_fileno);
	}
}

void Cgi::set_body(std::string payload)
{
	_body = payload;
}
