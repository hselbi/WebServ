#include "../../includes/core/Cgi.hpp"
#include "../../includes/core/Client.hpp"

Cgi::Cgi() : _cgi_bin(""), _cgi_script(""), _body(""), _output(""), _cgi_output_file(NULL), _envp(NULL), _argv(NULL), _extension("")
{
}

Cgi::~Cgi()
{
}

void Cgi::setClient(Client &client)
{
	this->_client = &client;
}

int Cgi::start_cgi(std::string script_path)
{
	set_cgi_bin("/home/zmahmoud/Desktop/WebServ/config/cgi_binary/php-cgi");
	set_cgi_script(script_path);
	init_env_vars();
	return exec_cgi();
}

void Cgi::init_env_vars()
{
	std::string root = _client->get_response().getRoot();

	_env_vars["SERVER_SOFTWARE"] = "MortalKOMBAT/1.0";
	_env_vars["SERVER_NAME"] = _client->get_server_block().getServerName();
	_env_vars["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env_vars["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env_vars["SERVER_PORT"] = _client->get_server_block().getPort();
	_env_vars["REQUEST_METHOD"] = _client->get_request().getMethod();
	_env_vars["REQUEST_URI"] = _client->get_request().getPath();
	_env_vars["DOCUMENT_ROOT"] = root;
	_env_vars["SCRIPT_NAME"] = _cgi_script.c_str();
	_env_vars["SCRIPT_FILENAME"] = _env_vars["DOCUMENT_ROOT"] + "/" + _env_vars["SCRIPT_NAME"];
	_env_vars["QUERY_STRING"] = _client->get_request().getQuery();
	_env_vars["PATH_INFO"] = get_path_info(_client->get_request().getPath());

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
	_argv[0] = new char[_cgi_bin.length() + 1];
	_argv[1] = new char[_cgi_script.length() + 1];
	// strcpy(_argv[0], _cgi_bin.c_str());
	strcpy(_argv[0], _cgi_bin.c_str());
	strcpy(_argv[1], _cgi_script.c_str());
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

int Cgi::exec_cgi() // !! upload handiinng
{
	pid_t pid;
	int write_to_cgi[2];

	_cgi_output_file = tmpfile();
	if (!_cgi_output_file)
	{
		std::cerr << "tmpfile failed" << std::endl;
		return -1;
	}

	if(pipe(write_to_cgi) == -1)
	{
		std::cerr << "pipe failed" << std::endl;
		return -1;
	}

	// ! TODO: SET BODY REQUEST TO CGI
	std::string data = "hello lopez22"; // request body // _body.c_str();
	set_body(data);
	write(write_to_cgi[1], _body.c_str(), _body.length()); // write to pipe

	if ((pid = fork()) == -1)
	{
		std::cerr << "fork failed" << std::endl;
		return -1;
	}
	if (pid == 0)
	{
		close(write_to_cgi[1]);

		dup2(write_to_cgi[0], 0); // read from pipe
		dup2(fileno(_cgi_output_file), 1);

		close(write_to_cgi[0]);

		char *newargv[] = {(char*)"/home/zmahmoud/Desktop/WebServ/config/cgi_binary/php-cgi", (char*)_cgi_script.c_str(), NULL};
		if (execve("/home/zmahmoud/Desktop/WebServ/config/cgi_binary/php-cgi", newargv, 0) == -1)
		{
			// TODO: handle error if execve failed (server hang)
			std::cerr << "execve failedd" << std::endl;
			return -1;
		}
	}
	else
	{
		close(write_to_cgi[0]);
		close(write_to_cgi[1]);
		// DONT WAIT FOR CHILD PROCESS TO FINISH
		waitpid(pid, NULL, 0); //
		lseek(fileno(_cgi_output_file), 0, SEEK_SET);
		// char buf[4024];
		// printf("data from CGI: \n ------------------ \n");
		// read(fileno(_cgi_output_file), buf, 100);
		// std::cout << buf << std::endl;
		// close(fileno(_cgi_output_file));
	}
	return fileno(_cgi_output_file);
}

void Cgi::set_body(std::string payload)
{
	_body = payload;
}

void Cgi::set_cgi_bin(std::string cgi_bin)
{
	_cgi_bin = cgi_bin;
}

void Cgi::set_cgi_script(std::string cgi_script)
{
	_cgi_script = cgi_script;
}

// /board/time/index.php, find last occurence of / and get what is after it
std::string Cgi::get_cgi_script_name(std::string path)
{
	std::string script_name;
	size_t pos = path.find_last_of('/');
	if (pos != std::string::npos)
		script_name = path.substr(pos + 1);
	return script_name;
}
